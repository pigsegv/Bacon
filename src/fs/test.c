#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <argp.h>

#include "fs.h"
#include "common.h"

#define ERROR(msg)                                  \
  do {                                              \
    fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
    perror(msg);                                    \
  } while (false)

#define NUM_ARGS 1

#define USAGE 0xc0ffee

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

struct argp_option options[] = {
  {
    .name = "image",
    .key = 'i',
    .arg = "IMAGE",
    .doc = "Specify the filesystem image to use",
  },

  {
    .name = "print",
    .key = 'p',
    .doc = "Print the contents of the file",
  },

  {
    .name = "help",
    .key = 'h',
    .doc = "Show this message",
  },

  {
    .name = "usage",
    .key = USAGE,
    .doc = "Show usage",
  },

  { 0 },
};

struct args {
  const char *img_path;
  const char *file_path;
  bool print;
};

static error_t parser(int key, char *arg, struct argp_state *state) {
  struct args *args = state->input;
  switch (key) {
    case 'h': {
      argp_state_help(state, state->out_stream, ARGP_HELP_STD_HELP);
      break;
    }

    case USAGE: {
      argp_state_help(state, state->out_stream,
                      ARGP_HELP_STD_USAGE | ARGP_HELP_EXIT_OK);
      break;
    }

    case 'i': {
      args->img_path = arg;
      break;
    }

    case 'p': {
      args->print = true;
      break;
    }

    case ARGP_KEY_ARG: {
      if (state->arg_num >= NUM_ARGS) {
        argp_usage(state);
      }

      args->file_path = arg;

      break;
    }

    case ARGP_KEY_END: {
      if (state->arg_num < NUM_ARGS) {
        argp_usage(state);
      }

      break;
    }

    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

static const char *args_doc = "FILEPATH";
static const char *doc = "An ext2 file system implementation";
static struct args args = { 0 };

int main(int argc, char **argv) {
  struct argp argp = {
    .options = options,
    .parser = parser,
    .args_doc = args_doc,
    .doc = doc,
  };

  argp_parse(&argp, argc, argv, ARGP_NO_HELP, NULL, &args);

  fs_img = fopen(args.img_path, "rb");
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

  fs_cleanup();
  fclose(fs_img);
  return 0;
}
