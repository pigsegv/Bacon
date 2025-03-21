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

  FILE *outfile = fopen(argv[2], "rb+");
  if (outfile == NULL) {
    fprintf(stderr, "Failed to open file %s: ", argv[2]);
    perror(NULL);

    return 1;
  }

  struct mbr mbr = { 0 };
  memcpy(mbr.boostrap, bootstrap, size);

  uint64_t disk_size = ((PARTITION_START_LBA * 512) +
                        ((uint64_t)PARTITION_SIZE_GiB * 1024 * 1024 * 1024));

  uint32_t spt = 63;
  uint32_t hpc = 0;
  if (disk_size <= 504u * 1024 * 1024) {
    hpc = 16;
  } else if (disk_size <= 1008u * 1024 * 1024) {
    hpc = 32;

  } else if (disk_size <= 2016u * 1024 * 1024) {
    hpc = 64;

  } else if (disk_size <= 4032u * 1024 * 1024) {
    hpc = 128;

  } else {
    hpc = 255;
  }

  uint32_t partition_sectors =
    (uint32_t)PARTITION_SIZE_GiB * (1024 * 1024 * 1024) / 512;

  uint32_t start_cylinder = PARTITION_START_LBA / (hpc * spt);
  uint32_t start_head = (PARTITION_START_LBA / spt) % hpc;
  uint32_t start_sector = (PARTITION_START_LBA % spt) + 1;

  uint32_t end_cylinder =
    (PARTITION_START_LBA + partition_sectors - 1) / (hpc * spt);
  uint32_t end_head =
    ((PARTITION_START_LBA + partition_sectors - 1) / spt) % hpc;
  uint32_t end_sector =
    ((PARTITION_START_LBA + partition_sectors - 1) % spt) + 1;

  mbr.partitions[0] = (struct partition){
    .attr = BOOTABLE,
    .start_chs = {
    	[0] = start_head & 0xff,
    	[1] = ((start_cylinder >> 2) & 0xc0) | (start_sector & 0x3f),
    	[2] = start_cylinder & 0xff,
    },
    .type = PARTITION_TYPE,
    .end_chs = {
    	[0] = end_head & 0xff,
    	[1] = ((end_cylinder >> 2) & 0xc0) | (end_sector & 0x3f),
    	[2] = end_cylinder & 0xff,
    },
    .LBA = PARTITION_START_LBA,
    .sectors = partition_sectors,
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
