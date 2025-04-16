#include "fs.h"

struct fs_com_vtable fs_com_vt = { 0 };
enum fs_type fs_type = FS_TYPE_NONE;

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
