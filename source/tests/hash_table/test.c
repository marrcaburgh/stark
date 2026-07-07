#define STARK_HASH_TABLE_IMPL
#include "stark/hash_table.h"

#include <inttypes.h>
#include <stdio.h>

int main(void) {
  struct stark_hash_table_bucket buckets[64] = {0};

  struct stark_hash_table hash_table = {.alg = STARK_HASH_TABLE_ALG_FNV1A,
                                        .tbl_size = sizeof(buckets) /
                                                    sizeof(buckets[0]),
                                        .elem_size = sizeof(int32_t),
                                        .bkts = buckets};

  if (!stark_hash_table_insert(&hash_table, NULL, "my_key", &(int32_t){3532},
                               false)) {
    return 1;
  }

  struct stark_hash_table_bucket *bucket =
      stark_hash_table_extract("my_key", &hash_table);

  if (bucket == NULL) {
    return 2;
  }

  printf("val: %d\n", *(int *)bucket->val);

#ifdef STARK_HASH_TABLE_ENABLE_HEAP
  stark_hash_table_free_buckets(&hash_table);
#endif // STARK_HASH_TABLE_ENABLE_HEAP

  return 0;
}
