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

#ifndef STARK_INTERNAL_HASH_TABLE_H
#define STARK_INTERNAL_HASH_TABLE_H

//
// Uncomment this to get syntax highlighting of the majority of the code in
// this header, the default (stack) parts will still be grayed out:
//
// #define STARK_HASH_TABLE_ENABLE_HEAP
//

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_RETURN_DUPLICATE (1u << 0)
#define HASH_TABLE_MODE_EXTRACT (1u << 1)
#define HASH_TABLE_MODE_BINARY (1u << 2)
#define HASH_TABLE_MODE_RESIZE (1u << 3)

STARK_COLD static void hash_table_error(enum stark_hash_table_err errc,
                                        enum stark_hash_table_err *rcp,
#ifdef STARK_HASH_TABLE_DISABLE_ERROR_PRINTING
                                        STARK_UNUSED
#endif // STARK_HASH_TABLE_DISABLE_ERROR_PRINTING

                                            size_t const key_len,
#ifdef STARK_HASH_TABLE_DISABLE_ERROR_PRINTING
                                        STARK_UNUSED
#endif // STARK_HASH_TABLE_DISABLE_ERROR_PRINTING
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
    fprintf(stderr, "duplicate key: '%.*s'", (int)key_len, ctx);

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

static inline bool hash_table_insert(struct stark_hash_table *htp,
                                     enum stark_hash_table_err *,
                                     void const *key, size_t key_len,
                                     void const *val, uint8_t flags);

STARK_ALWAYS_INLINE static inline struct stark_hash_table_bucket *
hash_table_probe(struct stark_hash_table *const restrict htp,
                 enum stark_hash_table_err *const restrict rcp,
                 size_t *const restrict klp, void const *const restrict key,
                 size_t const key_len, uint8_t flags) {
  if (STARK_EXPECT_FALSE(htp == NULL)) {
    hash_table_error(STARK_HASH_TABLE_ERR_NULL, rcp, 3, "htp");

    return NULL;
  } else if (STARK_EXPECT_FALSE(key == NULL)) {
    hash_table_error(STARK_HASH_TABLE_ERR_NULL, rcp, 3, "key");

    return NULL;
  } else if (STARK_EXPECT_FALSE(htp->tbl_size == 0 ||
                                (htp->tbl_size & (htp->tbl_size - 1)) != 0)) {
    hash_table_error(STARK_HASH_TABLE_ERR_NOT_POWER_OF_TWO, rcp, 0, NULL);

    return NULL;
  }

  char const *tcp = key;
  size_t idx = 0, ht_idx
#ifdef STARK_HASH_TABLE_ENABLE_HEAP
      ,
         ht_oidx;
#else  // STARK_HASH_TABLE_ENABLE_HEAP
      ;
#endif // STARK_HASH_TABLE_ENABLE_HEAP

  switch (htp->alg) {
  case STARK_HASH_TABLE_ALG_FNV1:
    ht_idx = sizeof(size_t) == 8 ? 14695981039346656037ULL : 2166136261U;

    for (;; idx++, ht_idx *= sizeof(size_t) == 8 ? 1099511628211ULL : 16777619U,
            ht_idx ^= (unsigned char)*tcp++) {
      if (idx == key_len ||
          (*tcp == '\0' && !(flags & HASH_TABLE_MODE_BINARY))) {
        break;
      }
    }

    break;
  case STARK_HASH_TABLE_ALG_FNV1A:
    ht_idx = sizeof(size_t) == 8 ? 14695981039346656037ULL : 2166136261U;

    for (;; idx++, ht_idx ^= (unsigned char)*tcp++,
            ht_idx *= sizeof(size_t) == 8 ? 1099511628211ULL : 16777619U) {
      if (idx == key_len ||
          (*tcp == '\0' && !(flags & HASH_TABLE_MODE_BINARY))) {
        break;
      }
    }

    break;
  case STARK_HASH_TABLE_ALG_DJB2:
    ht_idx = 5381;

    for (;; idx++, ht_idx = (ht_idx * 33) + (unsigned char)*tcp++) {
      if (idx == key_len ||
          (*tcp == '\0' && !(flags & HASH_TABLE_MODE_BINARY))) {
        break;
      }
    }

    break;
  case STARK_HASH_TABLE_ALG_DJB2_XOR:
    ht_idx = 5381;

    for (;; idx++, ht_idx = (ht_idx * 33) ^ (unsigned char)*tcp++) {
      if (idx == key_len ||
          (*tcp == '\0' && !(flags & HASH_TABLE_MODE_BINARY))) {
        break;
      }
    }

    break;
  case STARK_HASH_TABLE_ALG_SDBM:
    ht_idx = 0;

    for (;; idx++, ht_idx = (unsigned char)*tcp++ + (ht_idx << 6) +
                            (ht_idx << 16) - ht_idx) {
      if (idx == key_len ||
          (*tcp == '\0' && !(flags & HASH_TABLE_MODE_BINARY))) {
        break;
      }
    }

    break;
  default:
    hash_table_error(STARK_HASH_TABLE_ERR_INVALID_HASH_ALGORITHM, rcp, 0, NULL);

    return NULL;
  }

  *klp = idx;
#ifdef STARK_HASH_TABLE_ENABLE_HEAP
  ht_oidx = ht_idx;
#endif // STARK_HASH_TABLE_ENABLE_HEAP

#ifdef STARK_HASH_TABLE_ENABLE_HEAP
  if (STARK_EXPECT_FALSE(htp->bkts == NULL)) {
    htp->bkts = calloc(htp->tbl_size, sizeof(*htp->bkts));

    if (STARK_EXPECT_FALSE(htp->bkts == NULL)) {
      hash_table_error(STARK_HASH_TABLE_ERR_OUT_OF_MEMORY, rcp, 0, NULL);

      return NULL;
    }
  }

hash_table_probe_retry:
#endif // STARK_HASH_TABLE_ENABLE_HEAP

  for (size_t pc = 0; pc != htp->tbl_size; pc++) {
    struct stark_hash_table_bucket *bkt;

    if ((bkt = &htp->bkts[ht_idx = (ht_idx + (pc != 0)) & (htp->tbl_size - 1)])
            ->key != NULL) {
      if (*klp == bkt->key_len && memcmp(bkt->key, key, *klp) == 0) {
        goto hash_table_probe_check_duplicate;
      }

      continue;

    hash_table_probe_check_duplicate:
      if (!(flags & HASH_TABLE_MODE_EXTRACT) &&
          !(flags & HASH_TABLE_RETURN_DUPLICATE)) {
        hash_table_error(STARK_HASH_TABLE_ERR_DUPLICATE, rcp, *klp, key);

        return NULL;
      }

      return bkt;
    }

    return !(flags & HASH_TABLE_MODE_EXTRACT) ? bkt : NULL;
  }

  if (flags & HASH_TABLE_MODE_EXTRACT) {
    return NULL;
  }

#ifdef STARK_HASH_TABLE_ENABLE_HEAP
  struct stark_hash_table_bucket *bkts = htp->bkts;
  void *tp = calloc(htp->tbl_size *= 2, sizeof(*bkts));

  if (STARK_EXPECT_FALSE(tp == NULL)) {
    hash_table_error(STARK_HASH_TABLE_ERR_OUT_OF_MEMORY, rcp, 0, NULL);

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

  free((void *)bkts);

  ht_idx = ht_oidx;

  goto hash_table_probe_retry;
#else
  hash_table_error(STARK_HASH_TABLE_ERR_FULL, rcp, 0, NULL);
#endif // STARK_HASH_TABLE_ENABLE_HEAP

  return NULL;
}

static inline bool
hash_table_insert(struct stark_hash_table *const restrict htp,
                  enum stark_hash_table_err *const restrict rcp,
                  void const *const restrict key, size_t const key_len,
                  void const *const restrict val, uint8_t const flags) {
  if (STARK_EXPECT_FALSE(val == NULL)) {
    hash_table_error(STARK_HASH_TABLE_ERR_NULL, rcp, 3, "val");

    return false;
  }

  void const *ktp, *vtp;
  struct stark_hash_table_bucket *bkt;
  size_t tkl;

  if ((bkt = hash_table_probe(htp, rcp, &tkl, key, key_len, flags)) == NULL) {
    return false;
  }

#ifdef STARK_HASH_TABLE_ENABLE_HEAP
  if (STARK_EXPECT_TRUE(!(flags & HASH_TABLE_MODE_RESIZE))) {
    if (STARK_EXPECT_FALSE(
            (ktp = bkt->key != NULL
                       ? bkt->key
                       : malloc(tkl + !(flags & HASH_TABLE_MODE_BINARY))) ==
            NULL)) {
      hash_table_error(STARK_HASH_TABLE_ERR_OUT_OF_MEMORY, rcp, 0, NULL);

      return false;
    }

    memcpy((void *)ktp, (void *)key, tkl + !(flags & HASH_TABLE_MODE_BINARY));

    if (STARK_EXPECT_FALSE(
            (vtp = bkt->val != NULL ? bkt->val : malloc(htp->elem_size)) ==
            NULL)) {
      if (bkt->key == NULL) {
        free((void *)ktp);
      }

      hash_table_error(STARK_HASH_TABLE_ERR_OUT_OF_MEMORY, rcp, 0, NULL);

      return false;
    }

    memcpy((void *)vtp, (void *)val, htp->elem_size);
  } else {
#endif // STARK_HASH_TABLE_ENABLE_HEAP

    ktp = key;
    vtp = val;

#ifdef STARK_HASH_TABLE_ENABLE_HEAP
  }
#endif // STARK_HASH_TABLE_ENABLE_HEAP

  memcpy(
      (void *)bkt,
      &(struct stark_hash_table_bucket){.key = ktp, .val = vtp, .key_len = tkl},
      sizeof(*bkt));

  return true;
}

#endif // STARK_INTERNAL_HASH_TABLE_H

#ifdef INTERNAL_HASH_TABLE_UNDEF
#undef HASH_TABLE_RETURN_DUPLICATE
#undef HASH_TABLE_MODE_EXTRACT
#undef HASH_TABLE_MODE_BINARY
#undef HASH_TABLE_MODE_RESIZE
#endif // INTERNAL_HASH_TABLE_UNDEF
