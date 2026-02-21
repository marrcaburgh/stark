#include "mb_clite_fptr.h"
#include <stdio.h>

int main() {
  int *array = NULL;

  array = mb_fptr_grow(array, sizeof(*array));
  array = mb_fptr_grow(array, sizeof(*array));

  array[0] = 431;
  array[1] = 43141;

  printf("%d\n", array[0]);

  mb_fptr_free(array);

  return 0;
}
