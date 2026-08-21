//
// SPDX-License-Identifier: MIT
//
// stark - a C99+ utility library - stark_hash_table - a flexible and simple
// hash table
// Copyright (C) 2026 marrcaburgh
//

#ifndef STARK_HASH_TABLE_H
#define STARK_HASH_TABLE_H

//
// Uncomment these to get syntax highlighting of the majority of the code in
// this header, the default (stack) parts will still be grayed out:
//
// #define STARK_HASH_TABLE_ENABLE_HEAP
// #define STARK_HASH_TABLE_IMPL
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

#include "stark/core.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum stark_hash_table_err {
  STARK_HASH_TABLE_ERR_NULL,
  STARK_HASH_TABLE_ERR_NOT_POWER_OF_TWO,
  STARK_HASH_TABLE_ERR_INVALID_HASH_ALGORITHM,
  STARK_HASH_TABLE_ERR_DUPLICATE,
  STARK_HASH_TABLE_ERR_FULL,
  STARK_HASH_TABLE_ERR_OUT_OF_MEMORY,
} stark_hash_table_err_t;

typedef enum stark_hash_table_algorithm {
  STARK_HASH_TABLE_ALG_FNV1,
  STARK_HASH_TABLE_ALG_FNV1A,
  STARK_HASH_TABLE_ALG_DJB2,
  STARK_HASH_TABLE_ALG_DJB2_XOR,
  STARK_HASH_TABLE_ALG_SDBM,
} stark_hash_table_algorithm_t;

typedef struct STARK_ALIGNED(32) stark_hash_table_bucket {
  void const *const key;
  void const *const val;
  size_t const key_len;
} stark_hash_table_bucket_t;

typedef struct STARK_ALIGNED(32) stark_hash_table {
  enum stark_hash_table_algorithm const alg;
  size_t tbl_size;
  size_t const elem_size;
  struct stark_hash_table_bucket *bkts;
} stark_hash_table_t;

bool stark_hash_table_insert(struct stark_hash_table *htp,
                             enum stark_hash_table_err *rcp, char const *key,
                             void const *val, bool overwrite);

bool stark_hash_table_insert_key_len(struct stark_hash_table *htp,
                                     enum stark_hash_table_err *rcp,
                                     void const *key, size_t key_len,
                                     void const *val, bool overwrite);

struct stark_hash_table_bucket *
stark_hash_table_extract(struct stark_hash_table *htp, char const *key);

struct stark_hash_table_bucket *
stark_hash_table_extract_key_len(struct stark_hash_table *htp, void const *key,
                                 size_t key_len);

#ifdef STARK_HASH_TABLE_ENABLE_HEAP
void stark_hash_table_free_buckets(struct stark_hash_table *htp);
#endif // STARK_HASH_TABLE_ENABLE_HEAP

#ifdef STARK_HASH_TABLE_IMPL

#include "stark/internal/hash_table.h"

bool stark_hash_table_insert(struct stark_hash_table *const restrict htp,
                             enum stark_hash_table_err *const restrict rcp,
                             char const *const restrict key,
                             void const *const restrict val,
                             bool const overwrite) {
  return hash_table_insert(htp, rcp, (void *)key, SIZE_MAX, val,
                           overwrite ? HASH_TABLE_RETURN_DUPLICATE : 0);
}

bool stark_hash_table_insert_key_len(
    struct stark_hash_table *const restrict htp,
    enum stark_hash_table_err *const restrict rcp,
    void const *const restrict key, size_t const key_len,
    void const *const restrict val, bool const overwrite) {
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
      free((void *)bkt->key);
    }

    if (bkt->val != NULL) {
      free((void *)bkt->val);
    }
  }

  free(htp->bkts);
  htp->bkts = NULL;
}
#endif // STARK_HASH_TABLE_ENABLE_HEAP

#define INTERNAL_HASH_TABLE_UNDEF
#include "stark/internal/hash_table.h"

#endif // STARK_HASH_TABLE_IMPL

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // STARK_HASH_TABLE_H
