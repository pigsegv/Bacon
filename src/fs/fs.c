#include "fs.h"

struct fs_com_vtable fs_com_vt = { 0 };
enum fs_type fs_type = FS_TYPE_NONE;

int fs_file_size(const char *path, uint64_t *size) {
  switch (fs_type) {
    case FS_TYPE_EXT2:
      return fs_ext2_file_size(path, size);

    default:
      return -1;
  }
}

uint64_t fs_read_file(void *dest, uint64_t offset, int64_t count,
                      const char *path) {
  switch (fs_type) {
    case FS_TYPE_EXT2:
      return fs_ext2_read_file(dest, offset, count, path);

    default:
      return 0;
  }
}

int fs_init(struct fs_com_vtable *v, uint32_t sector_size, uint64_t offset) {
  fs_com_vt = *v;

  fs_type = FS_TYPE_EXT2; // TODO: Add support for other filesystems

  int res = 0;

  /*
	 * This switch case doesn't matter right now as I'm hard coding
	 * fs_type, but it will be useful when I add support for 
	 * other filesystems
	 */
  switch (fs_type) {
    case FS_TYPE_EXT2:
      res = fs_ext2_init(sector_size, offset);
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
