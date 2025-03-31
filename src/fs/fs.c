#include "fs.h"

uint64_t fs_sector = 0;

void (*fs_read_sectors)(void *dest, uint64_t offset, uint64_t count) = NULL;
void *(*fs_memcpy)(void *restrict dest, const void *restrict src,
                   size_t n) = NULL;
void *(*fs_malloc)(size_t size) = NULL;
void (*fs_free)(void *ptr) = NULL;

enum fs_type fs_type = FS_TYPE_NONE;

int fs_init(struct fs_vtable *v, uint64_t offset) {
  fs_read_sectors = v->read_sectors;
  fs_memcpy = v->memcpy;
  fs_malloc = v->malloc;
  fs_free = v->free;

  fs_type = FS_TYPE_EXT2; // TODO: Add support for other filesystems

  fs_sector = offset;

  /*
	 * This condition doesn't matter right now, as I'm hardcoding
	 * fs_type, but it will be useful when I add support for 
	 * other filesystems
	 */
  if (fs_type == FS_TYPE_EXT2) {
    void *buf = fs_malloc(FS_EXT2_SB_SECTORS * FS_SECTOR_SIZE);

    fs_read_sectors(buf, fs_sector + FS_EXT2_SB_OFFSET_SECTORS,
                    FS_EXT2_SB_SECTORS);
    fs_memcpy(&fs_ext2_sb, buf, sizeof(fs_ext2_sb));

    fs_free(buf);
    if (fs_ext2_sb.magic != 0xef53) {
      return 1;
    }
  }

  return 0;
}
