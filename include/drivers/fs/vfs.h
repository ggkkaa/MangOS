#pragma once
#include <stdint.h>
#include "list.h"
#include "memory/slab.h"

typedef struct Inode Inode;

typedef struct InodeOps {
        intptr_t (*create)(Inode* parent, const char* name, size_t namelen);
        intptr_t (*read)(Inode* file, void* buf, size_t size, intptr_t offset);
        intptr_t (*write)(Inode* file, const void* buf, size_t size, intptr_t offset);
} InodeOps;

typedef struct Inode {
        struct list list;
        InodeOps* ops;
        Cache* cache;
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