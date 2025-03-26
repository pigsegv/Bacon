#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "ext2.h"

#define SECTOR_SIZE 512
#define SB_SIZE 1024

static FILE *fs_img = NULL;
static int img_size = 0;

static uint64_t sector = 0;

char scratch[4096];

void *(*fs_read_sectors)(uint64_t offset, uint64_t count) = NULL;

void *read_sectors(uint64_t offset, uint64_t count) {
  assert(img_size >= SB_SIZE + 1024 &&
         "Image smaller than minimum ext2 filesystem size");
  assert(count * SECTOR_SIZE <= sizeof(scratch));

  if (fseek(fs_img, offset * SECTOR_SIZE, SEEK_SET) < 0) {
    fprintf(stderr, "%s:%d: ", __FILE__, __LINE__);
    perror("Failed to read file");
    exit(EXIT_FAILURE);
  }

  int iter = 100;
  int n = count * SECTOR_SIZE;
  do {
    n -=
      fread(&scratch[count * SECTOR_SIZE - n], 1, count * SECTOR_SIZE, fs_img);
  } while (n && --iter);

  if (iter == 0 && n != 0) {
    fprintf(stderr, "Failed to read file\n");
    exit(EXIT_FAILURE);
  }
  return scratch;
}

int init_sb(struct fs_ext2_sb *sb) {
  memcpy(sb, fs_read_sectors(sector + 2, 2), sizeof(*sb));
  return 0;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [IMAGE]\n", argv[0]);
    return 1;
  }

  fs_img = fopen(argv[1], "rb");
  if (fs_img == NULL) {
    fprintf(stderr, "%s:%d: ", __FILE__, __LINE__);
    perror("Failed to open file");
    return 1;
  }

  struct stat stat;
  if (fstat(fileno(fs_img), &stat) != 0) {
    fprintf(stderr, "%s:%d: ", __FILE__, __LINE__);
    perror("Failed to get file size");
    return 1;
  }
  img_size = stat.st_size;

  fs_read_sectors = read_sectors;
  struct fs_ext2_sb sb = { 0 };
  init_sb(&sb);
  printf("%x\n", sb.magic);

  return 0;
}
