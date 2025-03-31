#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <stddef.h>

#include "ext2.h"

#define FS_SECTOR_SIZE 512

enum fs_type {
  FS_TYPE_NONE = 0,
  FS_TYPE_EXT2,
};

extern uint64_t fs_sector;

extern void (*fs_read_sectors)(void *dest, uint64_t offset, uint64_t count);
extern void *(*fs_memcpy)(void *restrict dest, const void *restrict src,
                          size_t n);
extern void *(*fs_malloc)(size_t size);
extern void (*fs_free)(void *ptr);

struct fs_vtable {
  typeof(fs_read_sectors) read_sectors;
  typeof(fs_memcpy) memcpy;
  typeof(fs_malloc) malloc;
  typeof(fs_free) free;
};

extern enum fs_type fs_type;

int fs_init(struct fs_vtable *v, uint64_t offset);

#endif // FS_H
