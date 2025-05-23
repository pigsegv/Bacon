#ifndef FS_COMMON_H
#define FS_COMMON_H

#include <stdint.h>
#include <stddef.h>

struct fs_com_vtable {
  void (*read_sectors)(void *dest, uint64_t offset, uint64_t count);

  void *(*memcpy)(void *restrict dest, const void *restrict src, size_t n);
  size_t (*strlen)(const char *s);

  void *(*malloc)(size_t size);
  void *(*realloc)(void *ptr, size_t size);
  void (*free)(void *ptr);
};

extern struct fs_com_vtable fs_com_vt;

#endif // FS_COMMON_H
