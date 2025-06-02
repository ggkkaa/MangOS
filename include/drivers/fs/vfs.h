#pragma once
#include <stdint.h>
#include "memory/list.h"
#include "memory/slab.h"
#include "print.h"

#define MAX_MOUNTPOINTS 32
#define MAX_LABEL_LEN 16

typedef struct Inode Inode;
typedef struct Fs Fs;
typedef size_t inodeid;

typedef enum {
        INODE_DIR,
        INODE_FILE,
} InodeKind;
typedef int inodekind_t;

typedef struct Superblock Superblock;

struct SuperBlockOps {
    intptr_t (*get_inode)(Superblock* sb, inodeid id, Inode** result);
};

typedef struct SuperBlockOps SuperBlockOps;

typedef struct Superblock {
        Fs *fs;
        inodeid root;
        struct SuperBlockOps* ops;
} Superblock;

typedef struct Mountpoint {
        char label[MAX_LABEL_LEN];
        Superblock* superblock;
} Mountpoint;

struct Fs {
        intptr_t (*init)(Fs *fs);
        intptr_t (*mount)(Fs *fs, Superblock *superblock);
};

typedef struct DirEntry {
        size_t size;
        inodeid id;
        inodekind_t kind;
        char name[];
} DirEntry;

typedef struct InodeOps {
        intptr_t (*find)(Inode* dir, const char* name, size_t namelen, Inode** inode);
        intptr_t (*create)(Inode* parent, const char* name, size_t namelen, inodekind_t kind);
        intptr_t (*get_dir_entries)(Inode* dir, DirEntry* out, size_t max_entries);

        intptr_t (*read)(Inode* file, void* buf, size_t size, size_t offset);
        intptr_t (*write)(Inode* file, const void* buf, size_t size, size_t offset);
} InodeOps;

intptr_t inode_create(Inode* parent, const char* name, size_t namelen, inodekind_t kind);
intptr_t inode_get_dir_entries(Inode* dir, DirEntry* out, size_t max_entries);

intptr_t inode_read(Inode* file, void* buf, size_t size, size_t offset);
intptr_t inode_write(Inode* file, const void* buf, size_t size, size_t offset);

typedef struct Inode {
        struct list list;
        InodeOps* ops;
        Cache* cache;
        inodekind_t kind;
        Superblock *superblock;
} Inode; 

typedef struct Path {
        const char* path;
        Inode* from;
} Path;

void init_vfs();

intptr_t vfs_parse(const char* path, Path* res);
intptr_t vfs_find(Path* path, Inode** inode);

static intptr_t vfs_find_abs(const char* path, Inode** inode) {
        Path abs;
        intptr_t e;
        if((e=vfs_parse(path, &abs)) < 0) return e;
        return vfs_find(&abs, inode);
}

intptr_t vfs_create(Path* path, inodekind_t kind);
static intptr_t vfs_create_abs(const char* path, inodekind_t kind) {
        kinfo("vfs_create_abs(%s, %d)", path, kind);
        Path abs;
        if(vfs_parse(path, &abs) < 0) {
                kwarning("There was an error while parsing the path.");
                return -1;
        }
        return vfs_create(&abs, kind);
}

void inode_init(Inode* inode, Cache* cache);