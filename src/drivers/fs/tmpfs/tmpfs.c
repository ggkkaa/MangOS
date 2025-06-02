#include "drivers/fs/tmpfs/tmpfs.h"
#include "memory/slab.h"
#include "utils.h"
#include "memory/linked_list.h"
#include "memory/memory.h"
#include "print.h"

typedef struct TmpfsInode
{
        Inode inode;
        char name[TMPFS_NAME_LIMIT];
        size_t size;
        void *data;
} TmpfsInode;

typedef struct TmpfsData TmpfsData;
struct TmpfsData {
        TmpfsData* next;
        char data[];
};

#define TMPFS_DATABLOCK_BYTES (PAGE_SIZE-sizeof(TmpfsData))
#define TMPFS_DATABLOCK_DIRECTORY_ENTS ((TMPFS_DATABLOCK_BYTES) / sizeof(TmpfsInode*))

static Cache* tmpfs_inode_cache = NULL;

static TmpfsData* datablock_new() {
        TmpfsData* data = (TmpfsData*)kernel_malloc(PAGE_SIZE);
        if (!data) return NULL;
        data->next = NULL;
        memset(data->data, 0, PAGE_SIZE-sizeof(*data));
        return data;
}

static intptr_t tmpfs_read(Inode* file, void* buf, size_t size, size_t offset) {
        TmpfsInode* inode = (TmpfsInode*)file;
        if (inode->inode.kind != INODE_FILE) return -1;

        TmpfsData* head = inode->data;
        TmpfsData* prev = (TmpfsData*)(&inode->data);

        size_t left = inode->size;

        while(offset >= TMPFS_DATABLOCK_BYTES) {
                offset -= TMPFS_DATABLOCK_BYTES;
                left -= TMPFS_DATABLOCK_BYTES;
                prev = head;
                head = head->next;
        }
        if(!head && offset > 0) return -2;
        size_t read = 0;
        uint8_t* bytes = buf;
        while(read < size) {
                size_t block_left = TMPFS_DATABLOCK_BYTES - offset;
                size_t to_read = size-read;
                if (to_read > block_left) to_read = block_left;
                if(!head) {
                        prev->next = datablock_new();
                        if(!prev->next) return read;
                        head = prev->next;
                }
                if(to_read > left-offset) inode->size += to_read-(left-offset);
                memcpy(bytes+read, head->data+offset, to_read);
                read += to_read;
                prev = head;
                head = head->next;
                offset = 0;
                if (left < TMPFS_DATABLOCK_BYTES) left = 0;
                else left -= TMPFS_DATABLOCK_BYTES;
        }
        return read;
}

intptr_t tmpfs_get_dir_entries(Inode* dir, DirEntry* out, size_t max_entries) {
        TmpfsInode* inode = (TmpfsInode*)dir;
        if(inode->inode.kind != INODE_DIR) return -1;
        TmpfsData* head = inode->data;
        size_t count = 0;
        size_t offset = 0;

        while(head && count < max_entries) {
                size_t to_read = TMPFS_DATABLOCK_DIRECTORY_ENTS;
                if(inode->size - offset < to_read) {
                        to_read = inode->size - offset;
                }

                for (size_t i = offset; i < to_read && count < max_entries; i++) {
                        DirEntry* entry = &out[count];
                        TmpfsInode* tmpfs_inode = ((TmpfsInode**)head->data)[i];
                        size_t entry_size = sizeof(DirEntry) + str_len(tmpfs_inode->name) + 1;

                        if (entry_size > TMPFS_DATABLOCK_DIRECTORY_ENTS) return -2;

                        entry->size = entry_size;
                        entry->id = (inodeid)tmpfs_inode;
                        entry->kind = tmpfs_inode->inode.kind;
                        memcpy(entry->name, tmpfs_inode->name, str_len(tmpfs_inode->name) + 1);
                        count++;
                }

                head = head->next;
                offset = 0;
        }

        return count;
}

static intptr_t tmpfs_write(Inode* file, const void* buf, size_t size, size_t offset) {
        TmpfsInode* inode = (TmpfsInode*)file;
        if (inode->inode.kind != INODE_FILE) return -1;

        TmpfsData* head = inode->data;
        TmpfsData* prev = (TmpfsData*)(&inode->data);

        size_t left = inode->size;
        
        while(offset >= TMPFS_DATABLOCK_BYTES) {
                offset -= TMPFS_DATABLOCK_BYTES;
                left -= TMPFS_DATABLOCK_BYTES;
                prev = head;
                head = head->next;
        }
        if(!head && offset > 0) return -2;
        size_t written = 0;
        const uint8_t* bytes = buf;
        while(written < size) {
                size_t block_left = TMPFS_DATABLOCK_BYTES - offset;
                size_t to_write = size-written;
                if (to_write > block_left) to_write = block_left;
                if(!head) {
                        prev->next = datablock_new();
                        if(!prev->next) return written;
                        head = prev->next;
                }
                if(to_write > left-offset) inode->size += to_write-(left-offset);
                memcpy(head->data+offset, bytes+written, to_write);
                written += to_write;
                prev = head;
                head = head->next;
                offset = 0;
                if (left < TMPFS_DATABLOCK_BYTES) left = 0;
                else left -= TMPFS_DATABLOCK_BYTES;
        }
        return written;
}

static intptr_t tmpfs_find(Inode* dir, const char* name, size_t namelen, Inode** result) {
        TmpfsInode* inode = (TmpfsInode*)dir;
        if(inode->inode.kind != INODE_DIR) { 
                kwarning("Inode is not a directory!");
                return -1;
        }
        TmpfsData* head = inode->data;
        size_t size = inode->size;
        kinfo("Looking for '%s' in dir %p", name, dir);
        while(size && head) {
                size_t to_read = size > TMPFS_DATABLOCK_DIRECTORY_ENTS ? TMPFS_DATABLOCK_DIRECTORY_ENTS : size;
                for(size_t i = 0; i < to_read; i++) {
                        TmpfsInode* entry = ((TmpfsInode**)head->data)[i];
                        char tmp[TMPFS_NAME_LIMIT];
                        memcpy(tmp, entry->name, str_len(entry->name));
                        tmp[str_len(entry->name)] = '\0';
                        kinfo("Entry %d: '%s'", (int)i, tmp);
                        if (str_len(entry->name) == namelen && memcmp(entry->name, name, namelen) == 0) {
                                *result = &entry->inode;
                                return 0;
                        }
                        kinfo("It failed because either %d != %d, or because memcmp != 0.", str_len(entry->name), namelen);
                }
                head = head->next;
                size -= to_read;
        }
        return -2;
}

static intptr_t tmpfs_create(Inode* parent, const char* name, size_t namelen, inodekind_t kind);

static InodeOps tmpfs_inode_ops = {
        .create = tmpfs_create,
        .get_dir_entries = tmpfs_get_dir_entries,
        .read = tmpfs_read,
        .write = tmpfs_write,
        .find = tmpfs_find,
};

static TmpfsInode *tmpfs_new_inode(Superblock *sb, size_t size, inodekind_t kind, void *data, const char *name, size_t namelen) {
        TmpfsInode *node = cache_alloc(tmpfs_inode_cache);
        if (!node) return NULL;
        inode_init(&node->inode, tmpfs_inode_cache);
        node->inode.superblock = sb;
        node->inode.kind = kind;
        node->inode.ops = &tmpfs_inode_ops;
        node->size = size;
        node->data = data;
        strncpy(node->name, name, namelen);
        node->name[namelen] = '\0';
        return node;
}

static TmpfsInode *directory_new(Superblock *sb, const char* name, size_t namelen) {
        return tmpfs_new_inode(sb, 0, INODE_DIR, NULL, name, namelen);
}

static TmpfsInode *file_new(Superblock *sb, const char* name, size_t namelen) {
        return tmpfs_new_inode(sb, 0, INODE_FILE, NULL, name, namelen);
}

static intptr_t tmpfs_put(TmpfsInode* dir, TmpfsInode* entry) {
        TmpfsData* prev = (TmpfsData*)(&(dir->data));
        TmpfsData* head = dir->data;
        size_t size = dir->size;

        while (head && size >= TMPFS_DATABLOCK_DIRECTORY_ENTS) {
                size -= TMPFS_DATABLOCK_DIRECTORY_ENTS;
                prev = head;
                head = head->next;
        }
        if (size >= TMPFS_DATABLOCK_DIRECTORY_ENTS) return -1;
        if (!head) {
                TmpfsData* new_block = datablock_new();
                if(!new_block) return -1;
                prev->next = new_block;
                head = new_block;
                if (!dir->data) dir->data = new_block;
        }
        ((TmpfsInode**)head->data)[size] = entry;
        dir->size++;
        return 0;
}

static intptr_t tmpfs_create(Inode* parent, const char* name, size_t namelen, inodekind_t kind) {
        if(parent->kind != INODE_DIR) return -1;
        TmpfsInode* inode;
        if(kind == INODE_DIR) inode = directory_new(parent->superblock, name, namelen);
        else inode = file_new(parent->superblock, name, namelen);
        if (!inode) return -2;
        intptr_t e;
        if((e=tmpfs_put((TmpfsInode*)parent, inode)) < 0) return e;
        kinfo("Created inode at %p", parent);
        return 0;
}

static intptr_t tmpfs_init(Fs *_fs) {
        (void)_fs;
        if(!(tmpfs_inode_cache = cache_create(sizeof(TmpfsInode), "TmpfsInode"))) return -1;
        return 0;
}

static intptr_t tmpfs_get_inode(Superblock* _sb, inodeid id, Inode** result) {
        (void)_sb;
        TmpfsInode* node = (TmpfsInode*)id;
        *result = &node->inode;
        return 0;
}

static struct SuperBlockOps tmpfs_superblock_ops = {
        .get_inode = tmpfs_get_inode,
};

static intptr_t tmpfs_mount(Fs *fs, Superblock *superblock) {
        TmpfsInode *root = directory_new(superblock, NULL, 0);
        superblock->fs = fs;
        superblock->root = (inodeid)root;
        superblock->ops = &tmpfs_superblock_ops;
        return 0;
}

Fs tmpfs = {
        .init = tmpfs_init,
        .mount = tmpfs_mount,
};
