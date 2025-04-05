#ifndef FS_EXT_2_H
#define FS_EXT_2_H

#include <stdint.h>

#define FS_EXT2_SB_SECTORS 2
#define FS_EXT2_SB_OFFSET_SECTORS 2

struct fs_ext2_sb {
  uint32_t inodes_count;
  uint32_t blocks_count;
  uint32_t r_blocks_count;

  uint32_t free_blocks_count;
  uint32_t free_inodes_count;

  uint32_t first_data_block;

  uint32_t log_block_size;
  int32_t log_frag_size;

  uint32_t blocks_per_group;
  uint32_t frags_per_group;
  uint32_t inodes_per_group;

  int32_t mnt_time;
  int32_t write_time;

  int16_t mnt_count;
  int16_t max_mnt_count;

  uint16_t magic;

  uint16_t state;
  uint16_t errors;

  uint16_t rev_minor;

  int32_t last_check;
  int32_t check_interval;

  uint32_t os_id;

  uint32_t rev_major;

  uint16_t def_resuid;
  uint16_t def_resgid;

  // Extended attributes
  uint32_t first_inode;
  uint16_t inode_size;

  uint16_t sb_block_group;

  uint32_t opt_features;
  uint32_t req_features;
  uint32_t ro_features;

  uint8_t uuid[16];
  uint8_t volume_name[16];

  uint8_t last_mnt_path[64];

  uint32_t compression;

  uint8_t prealloc_blocks;
  uint8_t prealloc_dir_blocks;

  uint16_t unused;

  uint8_t journal_uuid[16];
  uint32_t journal_inode;
  uint32_t journal_dev;
  uint32_t last_orphan;
} __attribute__((packed));

struct fs_ext2_bg_desc {
  uint32_t block_bitmap;
  uint32_t inode_bitmap;
  uint32_t inode_table;

  uint16_t free_blocks_count;
  uint16_t free_inodes_count;
  uint16_t dirs_count;

  uint16_t padding;

  uint8_t reserved[12];
} __attribute__((packed));

struct fs_ext2_inode {
  uint16_t mode;
  uint16_t uid;

  uint32_t size;

  int32_t atime;
  int32_t ctime;
  int32_t mtime;
  int32_t dtime;

  int16_t gid;

  int16_t links_count;

  uint32_t sectors_count;

  uint32_t flags;

  uint32_t osd1;

  uint32_t block_ptr[12];
  uint32_t si_block_ptr;
  uint32_t di_block_ptr;
  uint32_t ti_block_ptr;

  uint32_t generation;

  uint32_t file_acl;
  uint32_t dir_acl;

  uint32_t frag_addr;

  uint8_t osd2[12];
} __attribute__((packed));

enum fs_ext2_mnt {
  FS_EXT2_MNT_NONE = 0,
  FS_EXT2_MNT_INVALID_SB,
  FS_EXT2_MNT_INCOMPAT,
};

enum fs_ext2_mnt fs_ext2_init(uint64_t sector);

#endif // FS_EXT_2_H
