#include "kernel.h"
#include "memory/slab.h"
#include "drivers/fs/vfs.h"
#include "drivers/fs/tmpfs/tmpfs.h"
#include "utils.h"
#include "panic.h"
#include "print.h"

static Mountpoint mountpoints[MAX_MOUNTPOINTS];
static size_t mountpoint_count = 0;

intptr_t inode_create(Inode* parent, const char* name, size_t namelen, inodekind_t kind) {
        kinfo("inode_create(%p, %s, %d, %d)", parent, name, namelen, kind);
        return parent->ops->create(parent, name, namelen, kind);
}

intptr_t inode_get_dir_entries(Inode* dir, DirEntry* out, size_t max_entries) {
        return dir->ops->get_dir_entries(dir, out, max_entries);
}

intptr_t inode_read(Inode* file, void* buf, size_t size, size_t offset) {
        return file->ops->read(file, buf, size, offset);
}

intptr_t inode_write(Inode* file, const void* buf, size_t size, size_t offset) {
        return file->ops->write(file, buf, size, offset);
}

intptr_t vfs_mount(const char *label, Superblock* sb) {
        if (!label || !sb || !sb->root || str_len(label) >= MAX_LABEL_LEN) return -1;

        for (size_t i = 0; i < mountpoint_count; i++) {
                if(strcmp(mountpoints[i].label, label) == 0) return -2;
        }

        if (mountpoint_count >= MAX_MOUNTPOINTS) return -3;

        strncpy(mountpoints[mountpoint_count].label, label, MAX_LABEL_LEN);
        mountpoints[mountpoint_count].superblock = sb;
        mountpoint_count++;
        return 0;
}

Superblock *vfs_get_mount(const char* label) {
        if (!label) return NULL;

        for (size_t i = 0; i < mountpoint_count; i++) {
                if(strcmp(mountpoints[i].label, label) == 0) return mountpoints[i].superblock;
        }

        return NULL;
}

static intptr_t sb_get_inode(Superblock* sb, inodeid id, Inode** inode) {
        return sb->ops->get_inode(sb, id, inode);
}

intptr_t fetch_inode(Superblock *sd, inodeid id, Inode** result) {
        if (!sd || !result) return -1;
        sb_get_inode(sd, id, result);
        return 0;
}

static Fs* get_rootfs() {
        return &tmpfs;
}

void init_vfs() {
        kernel.inode_cache = cache_create(sizeof(Inode), "Inode");
        Fs* rootfs = get_rootfs();
        if(rootfs->init && rootfs->init(rootfs) < 0) kpanic("Could not initialize rootfs!!!");
        if(rootfs->mount(rootfs, &kernel.root_block) < 0 || vfs_mount("root", &kernel.root_block)) {
                kpanic("Failed to mount rootfs.");
        }
        kinfo("rootfs is mounted at %s:/", mountpoints[0].label);
}

intptr_t inode_find(Inode* dir, const char* name, size_t namelen, Inode** inode) {
        kinfo("inode_find(%p, %s, %d, %p)", dir, name, namelen, inode);
        return dir->ops->find(dir, name, namelen, inode);
}

static const char* path_dir_next(const char* path) {
        while (*path)
        {
                if(*path++ == '/') {
                        return path;
                }
        }
        return NULL;
}

intptr_t vfs_find_parent(Path* path, const char** pathend, Inode** inode) {
    intptr_t e = 0;
    const char* dirbegin = path->path;
    const char* dirend; 
    *inode = path->from;
    while((dirend = path_dir_next(dirbegin))) {
        Inode* parent = *inode;
        if((e=inode_find(parent, dirbegin, dirend-1-dirbegin, inode)) < 0) {
                kwarning("inode_find(%p, %s, %d, %p) failed with error code %p", parent, dirbegin, dirend-1-dirbegin, inode,e);
            return e;
        }
        dirbegin = dirend;
    }
    *pathend = dirbegin;
    return 0;
}



intptr_t vfs_find(Path* path, Inode** inode) {
        intptr_t e;
        const char* pathend;
        Inode* parent;
        if((e=vfs_find_parent(path, &pathend, &parent)) < 0) {
                kwarning("vfs_find_parent failed with error code %p", e);
                return e;
        }
        if(pathend[0] == '\0') {
                kwarning("You're looking for root.");
                *inode = parent;
                return 0;
        } 
        if((e=inode_find(parent, pathend, str_len(pathend), inode)) < 0) {
                kwarning("inode_find(%p, %s, %d, %p) = %p", parent, pathend, str_len(pathend), inode, e);
                return e;
        }
        return 0;
}

intptr_t vfs_parse(const char* path, Path* res) {
        kinfo("vfs_parse(%s, %p)", path, res);
        if (!path) return -1;

        const char* colon = strstr(path, ":/");
        if (!colon) return -1;
        
        size_t label_len = colon - path;
        if (label_len == 0 || label_len >= MAX_LABEL_LEN) return -1;

        char label[MAX_LABEL_LEN];
        memcpy(label, path, label_len);
        label[label_len] = '\0';

        Superblock* sb = vfs_get_mount(label);
        if (!sb) return -2;

        intptr_t e;
        Inode* root;
        if ((e=fetch_inode(sb, sb->root, &root)) < 0) return e;
        res->from = root;
        res->path = colon + 2;
        return 0;
}

void inode_init(Inode* inode, Cache* cache) {
        memset(inode, 0, sizeof(*inode));
        inode->cache = cache;
        list_init(&inode->list);
}

intptr_t vfs_create(Path* path, inodekind_t kind) {
        kinfo("vfs_create(%p, %d)", path, kind);
        Inode* parent;
        intptr_t e;
        const char* pathend = NULL;
        if((e=vfs_find_parent(path, &pathend, &parent)) < 0 ) {
                kwarning("VFS find parent did not complete sucessfully, error code %p", e);
                return e;
        }
        if(pathend[0] == '\0') {
                kwarning("Cannot create file because it already exists.");
                return -2;
        }
        if((e=inode_create(parent, pathend, str_len(pathend), kind)) < 0) return e;
        return 0;
}