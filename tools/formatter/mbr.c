#include <stdio.h>
#include <stdint.h>
#include <argp.h>

#define UTIL_FILE_IO_IMPLEMENTATION
#include "include/util/fileIO.h"

#define BOOTSTRAP_SIZE 440

#define DISK_SIZE

#define PARTITION_TYPE 0x83
#define PARTITION_START_LBA 64
#define PARTITION_SIZE_GiB 2

#define BOOTABLE 0x80

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

  if (argc != 3) {
    fprintf(stderr,
            "Bad usage: %s [path to bootstrap code] [path to out file]\n",
            argv[0]);
    return 1;
  }

  uint64_t size;
  char *bootstrap = fio_read_file(argv[1], &size);
  if (bootstrap == NULL) {
    fprintf(stderr, "[ERROR]: Failed to open file %s: ", argv[1]);
    perror(NULL);

    return 1;
  }

  if (size > BOOTSTRAP_SIZE) {
    fprintf(stderr, "Bootstrap code: Expected 440 bytes or less, got %lu\n",
            size);
    return 1;
  }

  FILE *outfile = fopen(argv[2], "wb");
  if (outfile == NULL) {
    fprintf(stderr, "Failed to open file %s: ", argv[2]);
    perror(NULL);

    return 1;
  }

  struct mbr mbr = { 0 };
  memcpy(mbr.boostrap, bootstrap, BOOTSTRAP_SIZE);

  // Legacy CHS addressing is unused
  mbr.partitions[0] = (struct partition){
    .attr = BOOTABLE,
    .type = PARTITION_TYPE,
    .LBA = PARTITION_START_LBA,
    .sectors = (uint32_t)PARTITION_SIZE_GiB * (1024 * 1024 * 1024) / 512,
  };

  mbr.magic = 0xaa55;

  if (fio_write_to_file(outfile, &mbr, sizeof(mbr)) != 0) {
    fprintf(stderr, "Failed to write the entire mbr to file\n");
    fclose(outfile);

    return 1;
  }

  fclose(outfile);
  return 0;
}
