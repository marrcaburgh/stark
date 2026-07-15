//
// stark - a C99+ utility library - stark_hash_table - a flexible and simple
// hash table
// Copyright (C) 2026 marrcaburgh
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
//

#ifndef STARK_HASH_TABLE_H
#define STARK_HASH_TABLE_H

//
// Uncomment these to get syntax highlighting of the code in this header, the
// default (stack) parts will still be grayed out:
//
// #define STARK_HASH_TABLE_IMPL
// #define STARK_HASH_TABLE_ENABLE_HEAP
//
//
// Define these macros before including this header or with your build system:
//
// Define STARK_HASH_TABLE_ENABLE_HEAP to enable heap allocation and copying for
// keys and values along with dynamic resizing. The default (stack) path expects
// the keys and values to remain valid throughout the entire lifetime of the
// bucket.
//
// Define STARK_HASH_TABLE_DISABLE_ERROR_PRINTING to disable builtin
// developer-facing errors. You can still use rcp to get a return code.
//

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "core.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum stark_hash_table_err {
  STARK_HASH_TABLE_ERR_NULL,
  STARK_HASH_TABLE_ERR_NOT_POWER_OF_TWO,
  STARK_HASH_TABLE_ERR_INVALID_HASH_ALGORITHM,
  STARK_HASH_TABLE_ERR_DUPLICATE,
  STARK_HASH_TABLE_ERR_FULL,
  STARK_HASH_TABLE_ERR_OUT_OF_MEMORY,
} stark_hash_table_err_t;

typedef enum stark_hash_table_algorithm {
  STARK_HASH_TABLE_ALG_FNV1A,
  STARK_HASH_TABLE_ALG_DJB2,
} stark_hash_table_algorithm_t;

typedef struct STARK_ALIGNED(32) stark_hash_table_bucket {
  void *key;
  void *val;
  size_t key_len;
} stark_hash_table_bucket_t;

typedef struct STARK_ALIGNED(32) stark_hash_table {
  enum stark_hash_table_algorithm const alg;
  size_t tbl_size, elem_size;
  struct stark_hash_table_bucket *bkts;
} stark_hash_table_t;

bool stark_hash_table_insert(struct stark_hash_table *htp,
                             enum stark_hash_table_err *rcp, char const *key,
                             void *val, bool overwrite);

bool stark_hash_table_insert_key_len(struct stark_hash_table *htp,
                                     enum stark_hash_table_err *rcp,
                                     void const *key, size_t key_len, void *val,
                                     bool overwrite);

struct stark_hash_table_bucket *
stark_hash_table_extract(struct stark_hash_table *htp, char const *key);

struct stark_hash_table_bucket *
stark_hash_table_extract_key_len(struct stark_hash_table *htp, void const *key,
                                 size_t key_len);

#ifdef STARK_HASH_TABLE_ENABLE_HEAP
void stark_hash_table_free_buckets(struct stark_hash_table *htp);
#endif // STARK_HASH_TABLE_ENABLE_HEAP

#ifdef STARK_HASH_TABLE_IMPL
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_RETURN_DUPLICATE (1 << 0)
#define HASH_TABLE_MODE_EXTRACT (1 << 1)
#define HASH_TABLE_MODE_BINARY (1 << 2)
#define HASH_TABLE_MODE_RESIZE (1 << 3)

STARK_COLD static void hash_table_error(enum stark_hash_table_err errc,
                                        enum stark_hash_table_err *rcp,
                                        char const *ctx) {
  if (rcp != NULL) {
    *rcp = errc;
  }

#ifndef STARK_HASH_TABLE_DISABLE_ERROR_PRINTING
  fprintf(stderr, "stark_hash_table error: ");

  switch (errc) {
  case STARK_HASH_TABLE_ERR_NULL:
    fprintf(stderr, "%s cannot be NULL", ctx);

    break;
  case STARK_HASH_TABLE_ERR_NOT_POWER_OF_TWO:
    fprintf(stderr, "tbl_size must be a power of two");

    break;
  case STARK_HASH_TABLE_ERR_INVALID_HASH_ALGORITHM:
    fprintf(stderr, "invalid hash algorithm");

    break;
  case STARK_HASH_TABLE_ERR_DUPLICATE:
    fprintf(stderr, "duplicate key: '%s'", ctx);

    break;
  case STARK_HASH_TABLE_ERR_OUT_OF_MEMORY:
    fprintf(stderr, "out of memory");

    break;
  case STARK_HASH_TABLE_ERR_FULL:
    fprintf(stderr, "hash table is full; increase table_size");

    break;
  default:
    break;
  }

  fprintf(stderr, "\n");
#endif // STARK_HASH_TABLE_DISABLE_ERROR_PRINTING
}

STARK_ALWAYS_INLINE static inline size_t
hash_table_hashn_djb2(char const *restrict str, size_t const key_len,
                      size_t *const restrict klp, bool binary) {
  size_t hash = 5381, idx;

  for (idx = 0; idx != key_len;
       idx++, hash = (hash * 33) + (unsigned char)*str++) {
    if (*str == '\0' && !binary) {
      break;
    }
  }

  *klp = idx;

  return hash;
}

STARK_ALWAYS_INLINE static inline size_t
hash_table_hashn_fnv1a(char const *restrict str, size_t const key_len,
                       size_t *const restrict klp, bool binary) {
  size_t hash = sizeof(size_t) == 8 ? 14695981039346656037ULL : 2166136261U,
         idx;

  for (idx = 0; idx != key_len; idx++, hash ^= (unsigned char)*str++,
      hash *= sizeof(size_t) == 8 ? 1099511628211ULL : 16777619U) {
    if (*str == '\0' && !binary) {
      break;
    }
  }

  *klp = idx;

  return hash;
}

static inline bool hash_table_insert(struct stark_hash_table *htp,
                                     enum stark_hash_table_err *rcp, void *key,
                                     size_t key_len, void *val, uint8_t flags);

STARK_ALWAYS_INLINE static inline struct stark_hash_table_bucket *
hash_table_probe(struct stark_hash_table *htp, enum stark_hash_table_err *rcp,
                 size_t *const restrict klp, void *const restrict key,
                 size_t const key_len, uint8_t flags) {
  if (STARK_EXPECT_FALSE(htp == NULL)) {
    hash_table_error(STARK_HASH_TABLE_ERR_NULL, rcp, "htp");

    return NULL;
  } else if (STARK_EXPECT_FALSE(key == NULL)) {
    hash_table_error(STARK_HASH_TABLE_ERR_NULL, rcp, "key");

    return NULL;
  } else if (STARK_EXPECT_FALSE(htp->tbl_size == 0 ||
                                (htp->tbl_size & (htp->tbl_size - 1)) != 0)) {
    hash_table_error(STARK_HASH_TABLE_ERR_NOT_POWER_OF_TWO, rcp, NULL);

    return NULL;
  }

  size_t idx;

  switch (htp->alg) {
  case STARK_HASH_TABLE_ALG_FNV1A:
    idx = hash_table_hashn_fnv1a(key, key_len, klp,
                                 flags & HASH_TABLE_MODE_BINARY);

    break;
  case STARK_HASH_TABLE_ALG_DJB2:
    idx = hash_table_hashn_djb2(key, key_len, klp,
                                flags & HASH_TABLE_MODE_BINARY);

    break;
  default:
    hash_table_error(STARK_HASH_TABLE_ERR_INVALID_HASH_ALGORITHM, rcp, NULL);

    return NULL;
  }

#ifdef STARK_HASH_TABLE_ENABLE_HEAP
  if (STARK_EXPECT_FALSE(htp->bkts == NULL)) {
    htp->bkts = calloc(htp->tbl_size, sizeof(*htp->bkts));

    if (STARK_EXPECT_FALSE(htp->bkts == NULL)) {
      hash_table_error(STARK_HASH_TABLE_ERR_OUT_OF_MEMORY, rcp, NULL);

      return NULL;
    }
  }

hash_table_probe_retry:
#endif // STARK_HASH_TABLE_ENABLE_HEAP

  for (size_t pc = 0; pc != htp->tbl_size; pc++) {
    struct stark_hash_table_bucket *bkt =
        &htp->bkts[idx = (pc == 0 ? idx : idx + 1) & (htp->tbl_size - 1)];

    if (bkt->key != NULL) {
      if (*klp == bkt->key_len && memcmp(bkt->key, key, *klp) == 0) {
        goto hash_table_probe_check_duplicate;
      }

      continue;

    hash_table_probe_check_duplicate:
      if (!(flags & HASH_TABLE_MODE_EXTRACT) &&
          !(flags & HASH_TABLE_RETURN_DUPLICATE)) {
        hash_table_error(STARK_HASH_TABLE_ERR_DUPLICATE, rcp, key);

        return NULL;
      }

      return bkt;
    }

    return !(flags & HASH_TABLE_MODE_EXTRACT) ? bkt : NULL;
  }

#ifdef STARK_HASH_TABLE_ENABLE_HEAP
  struct stark_hash_table_bucket *bkts = htp->bkts;
  void *tp = calloc(htp->tbl_size *= 2, sizeof(*bkts));

  if (STARK_EXPECT_FALSE(tp == NULL)) {
    hash_table_error(STARK_HASH_TABLE_ERR_OUT_OF_MEMORY, rcp, NULL);

    htp->tbl_size >>= 1;

    return NULL;
  }

  htp->bkts = tp;

  for (size_t i = 0; i < (htp->tbl_size >> 1); i++) {
    if (bkts[i].key != NULL) {
      hash_table_insert(htp, rcp, bkts[i].key, bkts[i].key_len, bkts[i].val,
                        HASH_TABLE_MODE_BINARY | HASH_TABLE_MODE_RESIZE);
    }
  }

  free(bkts);

  goto hash_table_probe_retry;
#else
  hash_table_error(STARK_HASH_TABLE_ERR_FULL, rcp, NULL);
#endif // STARK_HASH_TABLE_ENABLE_HEAP

  return NULL;
}

static inline bool
hash_table_insert(struct stark_hash_table *const restrict htp,
                  enum stark_hash_table_err *const restrict rcp,
                  void *const restrict key, size_t const key_len,
                  void *const restrict val, uint8_t const flags) {
  if (STARK_EXPECT_FALSE(val == NULL)) {
    hash_table_error(STARK_HASH_TABLE_ERR_NULL, rcp, "val");

    return false;
  }

  size_t tkl;
  struct stark_hash_table_bucket *bkt =
      hash_table_probe(htp, rcp, &tkl, key, key_len, flags);

  if (bkt == NULL) {
    return false;
  }

#ifdef STARK_HASH_TABLE_ENABLE_HEAP
  if (STARK_EXPECT_TRUE(!(flags & HASH_TABLE_MODE_RESIZE))) {
    void *const ktp = bkt->key != NULL ? bkt->key : malloc(tkl);

    if (STARK_EXPECT_FALSE(ktp == NULL)) {
      hash_table_error(STARK_HASH_TABLE_ERR_OUT_OF_MEMORY, rcp, NULL);

      return false;
    }

    memcpy(ktp, key, tkl);

    void *const vtp = bkt->val != NULL ? bkt->val : malloc(htp->elem_size);

    if (STARK_EXPECT_FALSE(vtp == NULL)) {
      if (bkt->key == NULL) {
        free(ktp);
      }

      hash_table_error(STARK_HASH_TABLE_ERR_OUT_OF_MEMORY, rcp, NULL);

      return false;
    }

    memcpy(vtp, val, htp->elem_size);

    bkt->key = ktp;
    bkt->val = vtp;
  } else {
#endif // STARK_HASH_TABLE_ENABLE_HEAP

    bkt->key = key;
    bkt->val = val;
#ifdef STARK_HASH_TABLE_ENABLE_HEAP
  }
#endif // STARK_HASH_TABLE_ENABLE_HEAP

  bkt->key_len = tkl;

  return true;
}

bool stark_hash_table_insert(struct stark_hash_table *const restrict htp,
                             enum stark_hash_table_err *const restrict rcp,
                             char const *const restrict key,
                             void *const restrict val, bool const overwrite) {
  return hash_table_insert(htp, rcp, (void *)key, SIZE_MAX, val,
                           overwrite ? HASH_TABLE_RETURN_DUPLICATE : 0);
}

bool stark_hash_table_insert_key_len(
    struct stark_hash_table *const restrict htp,
    enum stark_hash_table_err *const restrict rcp,
    void const *const restrict key, size_t const key_len,
    void *const restrict val, bool const overwrite) {
  return hash_table_insert(htp, rcp, (void *)key, key_len, (void *)val,
                           (overwrite ? HASH_TABLE_RETURN_DUPLICATE : 0) |
                               HASH_TABLE_MODE_BINARY);
}

struct stark_hash_table_bucket *
stark_hash_table_extract(struct stark_hash_table *const restrict htp,
                         char const *const restrict key) {
  size_t tkl;

  return hash_table_probe(htp, NULL, &tkl, (void *)key, SIZE_MAX,
                          HASH_TABLE_RETURN_DUPLICATE |
                              HASH_TABLE_MODE_EXTRACT);
}

struct stark_hash_table_bucket *
stark_hash_table_extract_key_len(struct stark_hash_table *const restrict htp,
                                 void const *const restrict key,
                                 size_t const key_len) {
  size_t tkl;

  return hash_table_probe(htp, NULL, &tkl, (void *)key, key_len,
                          HASH_TABLE_RETURN_DUPLICATE | HASH_TABLE_MODE_BINARY |
                              HASH_TABLE_MODE_EXTRACT);
}

#ifdef STARK_HASH_TABLE_ENABLE_HEAP
void stark_hash_table_free_buckets(
    struct stark_hash_table *const restrict htp) {
  if (htp->bkts == NULL) {
    return;
  }

  for (size_t idx = 0; idx < htp->tbl_size; idx++) {
    struct stark_hash_table_bucket *const bkt = &htp->bkts[idx];

    if (bkt->key != NULL) {
      free((char *)bkt->key);

      bkt->key = NULL;
    }

    if (bkt->val != NULL) {
      free(bkt->val);

      bkt->val = NULL;
    }
  }

  free(htp->bkts);
  htp->bkts = NULL;
}
#endif // STARK_HASH_TABLE_ENABLE_HEAP

#endif // STARK_HASH_TABLE_IMPL

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // STARK_HASH_TABLE_H
