#include "ext2.h"
#include "common.h"

#include <stdio.h>

static struct fs_ext2_sb sb = { 0 };

static uint64_t sector = 0;

static uint64_t num_bg = 0;
static uint64_t blk_size = 0;
static uint64_t frag_size = 0;

enum fs_ext2_mnt fs_ext2_init(uint64_t s) {
  sector = s;

  void *buf = fs_com_malloc(FS_EXT2_SB_SECTORS * FS_COM_SECTOR_SIZE);
  fs_com_read_sectors(buf, sector + FS_EXT2_SB_OFFSET_SECTORS,
                      FS_EXT2_SB_SECTORS);
  fs_com_memcpy(&sb, buf, sizeof(sb));
  fs_com_free(buf);

  if (sb.magic != 0xef53) {
    return FS_EXT2_MNT_INVALID_SB;
  }

  if ((sb.req_features & ~(uint32_t)0x2) != 0) {
    return FS_EXT2_MNT_INCOMPAT;
  }

  return FS_EXT2_MNT_NONE;
}
