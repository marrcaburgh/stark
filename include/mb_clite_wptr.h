#ifndef MB_CLITE_WPTR_H
#define MB_CLITE_WPTR_H

#include <stdbool.h>
#include <stddef.h>
typedef struct mb_wptr {
  void *ptr;
  size_t len;
} mb_wptr;

static inline mb_wptr mb_wptr_wrap(void *ptr, size_t len) {
  return (mb_wptr){.ptr = ptr, .len = len};
}

static inline bool mb_wptr_empty(mb_wptr wptr) {
  return wptr.ptr == NULL || wptr.len == 0;
}

#define mb_wptr_from_fat(fptr)                                                 \
  ((mb_wptr){.ptr = (fptr), .len = mb_fptr_len(fptr)})

#endif // MB_CLITE_WPTR_H
