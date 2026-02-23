#ifndef MB_CLITE_FPTR_H
#define MB_CLITE_FPTR_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MB_FPTR_HEADER sizeof(size_t)

typedef struct mb_fptr_hdr {
  size_t len;
  size_t cap;
  char data[];
} mb_fptr_hdr_t;

static inline size_t mb_fptr_len(const void *const fptr) {
  return fptr != NULL ? ((size_t *)fptr)[-1] : 0;
}

static inline void *mb_fptr_grow(const void *const fptr, size_t elem_size) {
  size_t len = mb_fptr_len(fptr);
  size_t *base =
      fptr != NULL ? (size_t *)((char *)fptr - MB_FPTR_HEADER) : NULL;

  base = (size_t *)realloc(base, MB_FPTR_HEADER + ((len + 1) * elem_size));

  if (base == NULL) {
    fprintf(stderr, "mb_fptr_grow error: failed to reallocate memory");
    return base;
  }

  *base = len + 1;
  return (char *)base + MB_FPTR_HEADER;
}

#define mb_fptr_free(fptr)                                                     \
  do {                                                                         \
    void *mb_tmp_ptr_ = (fptr);                                                \
                                                                               \
    if (mb_tmp_ptr_ != NULL) {                                                 \
      free((size_t *)mb_tmp_ptr_ - 1);                                         \
    }                                                                          \
  } while (0)

#endif // MB_CLITE_FPTR_H
