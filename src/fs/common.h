#ifndef FS_COMMON_H
#define FS_COMMON_H

#include <stdint.h>
#include <stddef.h>

#define FS_COM_SECTOR_SIZE 512

extern void (*fs_com_read_sectors)(void *dest, uint64_t offset, uint64_t count);

extern void *(*fs_com_memcpy)(void *restrict dest, const void *restrict src,
                              size_t n);

extern void *(*fs_com_malloc)(size_t size);
extern void (*fs_com_free)(void *ptr);

struct fs_com_vtable {
  typeof(fs_com_read_sectors) read_sectors;

  typeof(fs_com_memcpy) memcpy;

  typeof(fs_com_malloc) malloc;
  typeof(fs_com_free) free;
};

#endif // FS_COMMON_H
