#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "fs.h"
#include "common.h"

#define ERROR(msg)                                  \
  do {                                              \
    fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
    perror(msg);                                    \
  } while (false)

static FILE *fs_img = NULL;

void read_sectors(void *dest, uint64_t offset, uint64_t count) {
  if (fseek(fs_img, offset * FS_COM_SECTOR_SIZE, SEEK_SET) < 0) {
    ERROR("Failed to read file");
    exit(EXIT_FAILURE);
  }

  int iter = 100;
  int n = count * FS_COM_SECTOR_SIZE;
  do {
    n -= fread(&((char *)dest)[count * FS_COM_SECTOR_SIZE - n], 1,
               count * FS_COM_SECTOR_SIZE, fs_img);
  } while (n && iter++);

  if (iter == 0 && n != 0) {
    fprintf(stderr, "Failed to read file\n");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [IMAGE]\n", argv[0]);
    return 1;
  }

  fs_img = fopen(argv[1], "rb");
  if (fs_img == NULL) {
    ERROR("Failed to open file");
    return 1;
  }

  struct stat stat;
  if (fstat(fileno(fs_img), &stat) != 0) {
    ERROR("Failed to get file size");
    return 1;
  }

  struct fs_com_vtable v = {
    .read_sectors = read_sectors,

    .memcpy = memcpy,

    .malloc = malloc,
    .free = free,
  };

  if (fs_init(&v, 0) != 0) {
    fprintf(stderr, "Invalid filesystem\n");
    return 1;
  }

  return 0;
}
