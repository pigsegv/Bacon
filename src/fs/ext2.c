#include "ext2.h"
#include "common.h"
#define UTIL_STRING_VIEW_IMPLEMENTATION
#include "util/string_view.h"

#define DA_USE_CUSTOM
#define DA_REALLOC fs_com_vt.realloc
#define DA_MEM_FREE fs_com_vt.free
#define DA_ASSERT(x)
#define DA_MEMCPY fs_com_vt.memcpy
#define DA_MEMMOVE fs_com_vt.memmove
#include "include/util/dynamic_array.h"

#include <stdio.h>

#define IDIV_ROUNDU(x, y)                \
  ({                                     \
    auto tmp1 = (x);                     \
    auto tmp2 = (y);                     \
    tmp1 / tmp2 + (tmp1 % tmp2 ? 1 : 0); \
  })

#define MIN(x, y)      \
  ({                   \
    auto _x = (x);     \
    auto _y = (y);     \
    _x < _y ? _x : _y; \
  })

#define GET_INODE_SIZE(inode)                            \
  ((uint64_t)(inode).size |                              \
   (((inode).mode & 0x8000) /* 0x8000 -> Regular file */ \
      ? (uint64_t)(inode).dir_acl << 32                  \
      : 0))

struct string_views {
  struct sv *items;
  uint64_t count;
  uint64_t capacity;
};

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

#define ROOT_INODE 2

static struct fs_ext2_sb sb = { 0 };
static struct fs_ext2_bg_desc *bg_descs = NULL;

static uint64_t num_bg = 0;
static uint64_t block_size = 0;
static uint64_t frag_size = 0;
static uint64_t sectors_per_block = 0;

static struct fs_ext2_inode root_inode = { 0 };

struct string_views split_path(const char *path) {
  struct string_views views = { 0 };

  struct sv v = {
    .view = path,
  };
  while (*path) {
    if (*path == '/') {
      if (v.length != 0) {
        DA_APPEND(&views, v);
      }
      v.view = path + 1;
      v.length = 0;

    } else {
      v.length++;
    }

    path++;
  }

  if (v.length != 0) {
    DA_APPEND(&views, v);
  }

  return views;
}

static char *read_inode(const struct fs_ext2_inode *inode) {
  uint64_t size = GET_INODE_SIZE(*inode);

  uint32_t blocks_count = IDIV_ROUNDU(size, block_size);
  char *buf = fs_com_vt.malloc(blocks_count * block_size);

  char *cursor = buf;
  uint32_t blocks_read = 0;

  //uint32_t max_blocks = 12;

  uint32_t i = 0;
  while (blocks_read < blocks_count && blocks_read < 12) {
    if (inode->block_ptr[i] == 0) {
      continue;
    }

    read_from_disk(cursor, inode->block_ptr[i] * block_size, block_size);

    cursor += block_size;
    blocks_read++;
  }

  if (blocks_read == blocks_count) {
    goto Exit;
  }

  if (inode->si_block_ptr != 0) {
    char *tmp = fs_com_vt.malloc(block_size);
    read_from_disk(tmp, inode->si_block_ptr * block_size, block_size);

    for (uint32_t i = 0;
         blocks_read < blocks_count && i < block_size / sizeof(uint32_t); i++) {
      uint32_t b = ((uint32_t *)tmp)[i];
      if (b == 0) {
        continue;
      }

      read_from_disk(cursor, b * block_size, block_size);

      cursor += block_size;
      blocks_read++;
    }

    fs_com_vt.free(tmp);
  }

  if (blocks_read < blocks_count) {
    // TODO: Do the above for doubly and triply indirect blocks

    fs_com_vt.free(buf);
    return NULL;
  }

Exit:
  return buf;
}

static int read_dir(const struct fs_ext2_inode *dir, const struct sv *file,
                    struct fs_ext2_inode *inode) {
  int code = 0;

  if ((dir->mode & 0x4000) == 0) {
    code = 1;
    goto Exit;
  }

  char *buf = read_inode(dir);

  if (buf == NULL) {
    code = 1;
    goto Exit;
  }

  char *tmp = buf;
  while (tmp < buf + dir->size) {
    struct fs_ext2_dir *entry = (struct fs_ext2_dir *)tmp;

    if (entry->inode == 0) {
      continue;
    }

    if (file->length == entry->name_len &&
        sv_cmp(file, &(struct sv){ .view = entry->name,
                                   .length = entry->name_len }) == 0) {
      uint32_t bg = (entry->inode - 1) / sb.inodes_per_group;
      uint32_t id = (entry->inode - 1) % sb.inodes_per_group;
      uint32_t block = bg_descs[bg].inode_table;

      read_from_disk(inode, block_size * block + id * sb.inode_size,
                     sizeof(*inode));
      goto Exit;
    }

    tmp += entry->rec_len;
  }

  // File not found
  code = 1;
  goto Exit;

Exit:
  fs_com_vt.free(buf);
  return code;
}

// TODO: Define proper error codes for this functions
static int get_inode(const char *path, struct fs_ext2_inode *inode) {
  if (path == NULL) {
    return 0;
  }

  if (inode == NULL) {
    return 0;
  }

  int code = 0;

  struct string_views filenames = split_path(path);
  struct fs_ext2_inode dir = root_inode;
  for (uint32_t i = 0; i < filenames.count - 1; i++) {
    if (read_dir(&dir, &DA_AT(filenames, i), &dir) != 0) {
      code = 1;
      goto Exit;
    }
  }

  if (read_dir(&dir, &DA_AT(filenames, filenames.count - 1), inode) != 0) {
    code = 1;
    goto Exit;
  }

Exit:
  DA_FREE(&filenames);
  return code;
}

int fs_ext2_file_size(const char *path, uint64_t *size) {
  struct fs_ext2_inode inode;
  if (get_inode(path, &inode) != 0) {
    return -1;
  }

  *size = GET_INODE_SIZE(inode);

  return 0;
}

uint64_t fs_ext2_read_file(void *dest, uint64_t offset, int64_t count,
                           const char *path) {
  uint64_t bytes_read = 0;

  struct fs_ext2_inode inode;
  if (get_inode(path, &inode) != 0) {
    return 0;
  }

  char *buf = read_inode(&inode);
  if (buf == NULL) {
    bytes_read = 0;
    goto Exit;
  }

  uint64_t size = GET_INODE_SIZE(inode);
  if (count < 0) {
    count = size - offset;
  }

  if (offset >= size) {
    bytes_read = 0;
    goto Exit;
  }

  bytes_read = MIN((uint64_t)count, size - offset);
  fs_com_vt.memcpy(dest, buf + offset, bytes_read);
  goto Exit;

Exit:
  fs_com_vt.free(buf);
  return bytes_read;
}

enum fs_ext2_mnt_err fs_ext2_init(uint32_t sec_size, uint64_t s) {
  sector = s;
  sector_size = sec_size;

  read_from_disk(&sb, SB_OFFSET, sizeof(sb));

  if (sb.magic != 0xef53) {
    return FS_EXT2_MNT_ERR_INVALID_SB;
  }

  if (sb.rev_major != 1) {
    return FS_EXT2_MNT_ERR_UNSUPPORTED_VERSION;
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

  // Read inode of root directory
  {
    uint32_t bg = (ROOT_INODE - 1) / sb.inodes_per_group;
    uint32_t id = (ROOT_INODE - 1) % sb.inodes_per_group;
    uint32_t block = bg_descs[bg].inode_table;

    read_from_disk(&root_inode, block_size * block + id * sb.inode_size,
                   sizeof(root_inode));
  }

  return FS_EXT2_MNT_ERR_NONE;
}

void fs_ext2_cleanup(void) {
  fs_com_vt.free(bg_descs);
  bg_descs = NULL;
}
