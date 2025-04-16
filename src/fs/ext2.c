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

#define SB_OFFSET 1024

static uint64_t sector = 0;
static uint32_t sector_size = 0;

static void read_from_disk(void *dest, uint64_t byte_off, uint64_t size) {
  uint64_t sect_off = byte_off / sector_size;
  uint64_t off_from_sect = byte_off % sector_size;
  uint64_t sects = IDIV_ROUNDU(off_from_sect + size, sector_size);

  void *buf = fs_com_vt.malloc(sects * sector_size);
  fs_com_vt.read_sectors(buf, sector + sect_off, sects);
  fs_com_vt.memcpy(dest, buf + off_from_sect, size);
  fs_com_vt.free(buf);
}

static struct fs_ext2_sb sb = { 0 };
static struct fs_ext2_bg_desc *bg_descs = NULL;

static uint64_t num_bg = 0;
static uint64_t block_size = 0;
static uint64_t frag_size = 0;
static uint64_t sectors_per_block = 0;

static void get_inode(const char *path, struct fs_ext2_inode *inode) {
  (void)path, (void)inode;
}

uint64_t fs_ext2_read_file(void *dest, uint64_t offset, uint64_t count,
                           const char *path) {
  struct fs_ext2_inode inode;
  get_inode(path, &inode);

  return 0;
}

enum fs_ext2_mnt_err fs_ext2_init(uint32_t sec_size, uint64_t s) {
  sector = s;
  sector_size = sec_size;

  read_from_disk(&sb, SB_OFFSET, sizeof(sb));

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

  sectors_per_block = IDIV_ROUNDU(block_size, sector_size);

  {
    const uint32_t bg_desc_block = (block_size == 1024) ? 2 : 1;

    const uint64_t bg_descs_size = num_bg * sizeof(*bg_descs);
    bg_descs = fs_com_vt.malloc(bg_descs_size);

    read_from_disk(bg_descs, bg_desc_block * block_size, bg_descs_size);
  }

  for (long unsigned int i = 0; i < num_bg; i++) {
    printf("%d\n", bg_descs[i].dirs_count);
  }

  printf("hash_version: %u\n", sb.def_hash_version);
  return FS_EXT2_MNT_ERR_NONE;
}

void fs_ext2_cleanup(void) {
  fs_com_vt.free(bg_descs);
  bg_descs = NULL;
}
