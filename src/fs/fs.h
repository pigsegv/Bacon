#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <stddef.h>

#include "ext2.h"
#include "common.h"

enum fs_type {
  FS_TYPE_NONE = 0,
  FS_TYPE_EXT2,
};

extern enum fs_type fs_type;

int fs_init(struct fs_com_vtable *v, uint64_t offset);
void fs_cleanup(void);

#endif // FS_H
