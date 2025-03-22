#include <stdio.h>

#include "ext2.h"

int main(void) {
  printf("sb_size: %lu\n"
         "bdg_size: %lu\n"
         "inode_size: %lu\n",
         sizeof(struct fs_ext2_sb), sizeof(struct fs_ext2_bg_desc),
         sizeof(struct fs_ext2_inode));
  return 0;
}
