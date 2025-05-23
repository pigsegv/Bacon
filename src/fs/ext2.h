#ifndef FS_EXT_2_H
#define FS_EXT_2_H

#include <stdint.h>

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

  uint32_t feature_compat;
  uint32_t feature_incompat;
  uint32_t feature_ro_compat;

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

  uint8_t hash_seed[16];
  uint8_t def_hash_version;

  uint8_t padding[3];

  uint32_t default_mount_opts;
  uint32_t first_meta_bg;
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

struct fs_ext2_dir {
  uint32_t inode;
  uint16_t rec_len;
  uint8_t name_len;
  uint8_t file_type;
  char name[];
} __attribute__((packed));

enum fs_ext2_mnt_err {
  FS_EXT2_MNT_ERR_NONE = 0,
  FS_EXT2_MNT_ERR_INVALID_SB,
  FS_EXT2_MNT_ERR_UNSUPPORTED_VERSION,
  FS_EXT2_MNT_ERR_INCOMPAT,
};

enum fs_ext2_incompat {
  FS_EXT2_INCOMPAT_NONE = 0x0,
  FS_EXT2_INCOMPAT_COMPRESSION = 0x1,
  FS_EXT2_INCOMPAT_FILETYPE = 0x2,
  FS_EXT2_INCOMPAT_RECOVER = 0x4,
  FS_EXT2_INCOMPAT_JOURNAL_DEV = 0x8,
  FS_EXT_INCOMPAT_META_BG = 0x10,
};

int fs_ext2_file_size(const char *path, uint64_t *size);
uint64_t fs_ext2_read_file(void *dest, uint64_t offset, int64_t count,
                           const char *path);
enum fs_ext2_mnt_err fs_ext2_init(uint32_t sector_size, uint64_t sector);
void fs_ext2_cleanup(void);

#endif // FS_EXT_2_H
