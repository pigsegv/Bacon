#include <stdio.h>
#include <stdint.h>
#include <argp.h>

#define UTIL_FILE_IO_IMPLEMENTATION
#include "util/fileIO.h"

#define BOOTSTRAP_SIZE 440

#define DISK_SIZE

struct partition {
  uint8_t attr;

  uint8_t start_chs[3];
  uint8_t type;
  uint8_t end_chs[3];

  uint32_t LBA;

  uint32_t sectors;
} __attribute__((packed));

struct mbr {
  uint8_t boostrap[440];

  uint32_t signature;
  uint16_t reserved;

  struct partition partitions[4];

  uint16_t magic;
} __attribute__((packed));

int main(int argc, char **argv) {
  // TODO: Accept more options for mbr construction

  if (argc != 2) {
    fprintf(stderr, "Bad usage: %s [path to bootstrap code]\n", argv[0]);
    return 1;
  }

  uint64_t size;
  char *bootstrap = fio_read_file(argv[1], &size);
  if (bootstrap == NULL) {
    fprintf(stderr, "[ERROR]: Failed to open file %s: ", argv[1]);
    perror(NULL);

    return 1;
  }

  if (size != BOOTSTRAP_SIZE) {
    fprintf(stderr, "Bootstrap code: Expected 440 bytes, got %lu\n", size);
    return 1;
  }

  struct mbr mbr = { 0 };
  memcpy(mbr.boostrap, bootstrap, BOOTSTRAP_SIZE);

  mbr.magic = 0xaa55;

  return 0;
}
