#define STARK_HASH_TABLE_IMPL
#include "stark/hash_table.h"

#include <inttypes.h>
#include <stdio.h>

#define DEFAULT_TABLE_SIZE 64

int main(void) {
#ifndef STARK_HASH_TABLE_ENABLE_HEAP
  struct stark_hash_table_bucket buckets[DEFAULT_TABLE_SIZE] = {0};
#endif // STARK_HASH_TABLE_ENABLE_HEAP

  struct stark_hash_table hash_table = {.alg = STARK_HASH_TABLE_ALG_FNV1A,
                                        .tbl_size = DEFAULT_TABLE_SIZE,
                                        .elem_size = sizeof(int32_t),
#ifndef STARK_HASH_TABLE_ENABLE_HEAP
                                        .bkts = buckets
#endif // STARK_HASH_TABLE_ENABLE_HEAP
  };

  if (!stark_hash_table_insert(&hash_table, NULL, "my_key", &(int32_t){3532},
                               false)) {
    return 1;
  }

  struct stark_hash_table_bucket *bucket =
      stark_hash_table_extract(&hash_table, "my_key");

  if (bucket == NULL) {
    return 2;
  }

  printf("val: %" PRId32 "\n", *(int32_t *)bucket->val);

#ifdef STARK_HASH_TABLE_ENABLE_HEAP
  stark_hash_table_free_buckets(&hash_table);
#endif // STARK_HASH_TABLE_ENABLE_HEAP

  return 0;
}
