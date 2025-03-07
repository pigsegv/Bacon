#pragma once
#ifndef UTIL_FILE_IO_H
#define UTIL_FILE_IO_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdint.h>

// Compatible with Windows
char *fio_read_file(const char *path, uint64_t *size);

#ifdef UTIL_FILE_IO_IMPLEMENTATION

char *fio_read_file(const char *path, uint64_t *size) {
  FILE *f = fopen(path, "rb");

  if (!f) {
    return NULL;
  }

  struct stat s;
  fstat(fileno(f), &s);

  if (size != NULL) {
    *size = s.st_size;
  }

  char *buf = malloc(s.st_size + 1);

  fread(buf, 1, s.st_size, f);
  buf[s.st_size] = 0;

  fclose(f);

  return buf;
}

#endif // UTIL_FILE_IO_IMPLEMENTATION

#endif // UTIL_FILE_IO_H
