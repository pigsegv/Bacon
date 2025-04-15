#include "ext2.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>

#define IDIV_ROUNDU(x, y)                \
  ({                                     \
    auto tmp1 = (x);                     \
    auto tmp2 = (y);                     \
    tmp1 / tmp2 + (tmp1 % tmp2 ? 1 : 0); \
  })

#define SB_SIZE 1024
#define SB_OFFSET 1024

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
  sector = s;

  {
    const uint32_t sb_offset_sectors = SB_SIZE / FS_COM_SECTOR_SIZE;
    const uint32_t sb_off_from_sect = SB_SIZE % FS_COM_SECTOR_SIZE;
    const uint32_t sb_sectors =
      IDIV_ROUNDU(sb_off_from_sect + SB_SIZE, FS_COM_SECTOR_SIZE);

    void *buf = fs_com_vt.malloc(sb_sectors * FS_COM_SECTOR_SIZE);
    fs_com_vt.read_sectors(buf, sector + sb_offset_sectors, sb_sectors);
    fs_com_vt.memcpy(&sb, buf + sb_off_from_sect, sizeof(sb));
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

  sectors_per_block = IDIV_ROUNDU(block_size, FS_COM_SECTOR_SIZE);

  {
    const uint32_t bg_desc_block = (block_size == 1024) ? 2 : 1;

    const uint64_t bg_descs_size = num_bg * sizeof(*bg_descs);
    bg_descs = fs_com_vt.malloc(bg_descs_size);

    const uint64_t desc_offset_sectors =
      bg_desc_block * block_size / FS_COM_SECTOR_SIZE;
    const uint64_t desc_off_from_sect =
      bg_desc_block * block_size % FS_COM_SECTOR_SIZE;
    const uint64_t desc_sectors =
      IDIV_ROUNDU(bg_descs_size + desc_off_from_sect, FS_COM_SECTOR_SIZE);

    void *buf = fs_com_vt.malloc(desc_sectors * FS_COM_SECTOR_SIZE);

    fs_com_vt.read_sectors(buf, sector + desc_offset_sectors, desc_sectors);
    fs_com_vt.memcpy(bg_descs, buf + desc_off_from_sect, bg_descs_size);

    fs_com_vt.free(buf);
  }

  for (long unsigned int i = 0; i < num_bg; i++) {
    printf("%d\n", bg_descs[i].dirs_count);
  }

  return FS_EXT2_MNT_ERR_NONE;
}

void fs_ext2_cleanup(void) {
  fs_com_vt.free(bg_descs);
  bg_descs = NULL;
}
