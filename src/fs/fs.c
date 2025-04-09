#include "fs.h"

void (*fs_com_read_sectors)(void *dest, uint64_t offset, uint64_t count) = NULL;
void *(*fs_com_memcpy)(void *restrict dest, const void *restrict src,
                       size_t n) = NULL;
void *(*fs_com_malloc)(size_t size) = NULL;
void (*fs_com_free)(void *ptr) = NULL;

enum fs_type fs_type = FS_TYPE_NONE;

int fs_init(struct fs_com_vtable *v, uint64_t offset) {
  fs_com_read_sectors = v->read_sectors;

  fs_com_memcpy = v->memcpy;

  fs_com_malloc = v->malloc;
  fs_com_free = v->free;

  fs_type = FS_TYPE_EXT2; // TODO: Add support for other filesystems

  int res = 0;

  /*
	 * This switch case doesn't matter right now as I'm hard coding
	 * fs_type, but it will be useful when I add support for 
	 * other filesystems
	 */
  switch (fs_type) {
    case FS_TYPE_EXT2:
      res = fs_ext2_init(offset);
      break;

    default:
      res = 1;
      break;
  }

  return res;
}

void fs_cleanup(void) {
  switch (fs_type) {
    case FS_TYPE_EXT2:
      fs_ext2_cleanup();
      break;

    default:
      break;
  }
}
