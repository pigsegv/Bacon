#ifndef UTIL_STRING_VIEW_H
#define UTIL_STRING_VIEW_H

#include <stdint.h>

struct sv {
  const char *view;
  uint64_t length;
};

#ifdef UTIL_STRING_VIEW_IMPLEMENTATION

int sv_cmp(const struct sv *sv1, const struct sv *sv2) {
  for (uint64_t i = 0; i < sv2->length; i++) {
    if (i >= sv1->length) {
      return 0;
    }

    if (sv1->view[i] != sv2->view[i]) {
      return *(const unsigned char *)&sv1->view[i] -
             *(const unsigned char *)&sv2->view[i];
    }
  }

  return 0;
}

#endif

#endif // UTIL_STRING_VIEW_H
