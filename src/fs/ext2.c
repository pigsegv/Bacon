#include "ext2.h"
#include "common.h"

#include <stdio.h>

#define IDIV_ROUNDU(x, y)                \
  ({                                     \
    auto tmp1 = (x);                     \
    auto tmp2 = (y);                     \
    tmp1 / tmp2 + (tmp1 % tmp2 ? 1 : 0); \
  })

static uint64_t sector = 0;

static struct fs_ext2_sb sb = { 0 };
static struct fs_ext2_bg_desc *bg_descs = NULL;

static uint64_t num_bg = 0;
static uint64_t block_size = 0;
static uint64_t frag_size = 0;
static uint64_t sectors_per_block = 0;

uint64_t fs_read_file(const char *path, char *dest, uint64_t offset,
                      int64_t n) {
  (void)path, (void)dest, (void)offset, (void)n;
  return n;
}

enum fs_ext2_mnt_err fs_ext2_init(uint64_t s) {
  static_assert(FS_COM_SECTOR_SIZE == 512);

  sector = s;

  {
    void *buf = fs_com_vt.malloc(FS_EXT2_SB_SECTORS * FS_COM_SECTOR_SIZE);
    fs_com_vt.read_sectors(buf, sector + FS_EXT2_SB_OFFSET_SECTORS,
                           FS_EXT2_SB_SECTORS);
    fs_com_vt.memcpy(&sb, buf, sizeof(sb));
    fs_com_vt.free(buf);
  }

  if (sb.magic != 0xef53) {
    return FS_EXT2_MNT_ERR_INVALID_SB;
  }

  if ((sb.feature_incompat & ~(uint32_t)FS_EXT2_INCOMPAT_FILETYPE) != 0) {
    return FS_EXT2_MNT_ERR_INCOMPAT;
  }

  {
    uint64_t tmp1 = IDIV_ROUNDU(sb.blocks_count, sb.blocks_per_group);
    uint64_t tmp2 = IDIV_ROUNDU(sb.inodes_count, sb.inodes_per_group);

    if (tmp1 != tmp2) {
      return FS_EXT2_MNT_ERR_INVALID_SB;
    }

    num_bg = tmp1;
  }

  block_size = 1024 << sb.log_block_size;
  if (sb.log_frag_size >= 0) {
    frag_size = 1024 << sb.log_frag_size;

  } else {
    frag_size = 1024 >> -sb.log_frag_size;
  }

  sectors_per_block = block_size / FS_COM_SECTOR_SIZE; // See static_assert

  {
    uint32_t bg_desc_block = (block_size == 1024) ? 2 : 1;
    uint32_t bg_desc_num_blocks =
      IDIV_ROUNDU(num_bg * sizeof(*bg_descs), block_size);

    uint64_t bg_descs_size = num_bg * sizeof(*bg_descs);
    bg_descs = fs_com_vt.malloc(bg_descs_size);

    void *buf = fs_com_vt.malloc(sectors_per_block * FS_COM_SECTOR_SIZE);

    // Guaranteed to be sector-aligned (I hope)
    fs_com_vt.read_sectors(buf, sector + bg_desc_block * sectors_per_block,
                           bg_desc_num_blocks * sectors_per_block);
    fs_com_vt.memcpy(bg_descs, buf, bg_descs_size);

    fs_com_vt.free(buf);
  }

  return FS_EXT2_MNT_ERR_NONE;
}

void fs_ext2_cleanup(void) {
  fs_com_vt.free(bg_descs);
  bg_descs = NULL;
}
