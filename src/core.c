//
// libmbx - a C99+ utility library - mbx_core - freestanding compatible libc
// replacement
// Copyright (C) 2026  marrcaburgh
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

#include "mbx/core.h"

#include <stdint.h>
#include <string.h>

MBX_HOT MBX_PURE bool mbx_bcmp(void const *a, void const *b, size_t n) {
  uintptr_t v1, v2;
  uint8_t *s1 = (uint8_t *)a, *s2 = (uint8_t *)b;

  for (; n >= sizeof(uintptr_t); s1 += sizeof(uintptr_t),
                                 s2 += sizeof(uintptr_t),
                                 n -= sizeof(uintptr_t)) {
    MBX_MEMCPY(&v1, s1, sizeof(uintptr_t));
    MBX_MEMCPY(&v2, s2, sizeof(uintptr_t));

    if (v1 != v2) {
      return false;
    }
  }

  for (; n > 0; n--, s1++, s2++) {
    if (*s1 != *s2) {
      return false;
    }
  }

  return true;
}

MBX_HOT MBX_CONST size_t mbx_strlen(char const *str) {
  size_t len = 0;

  while (str[len] != '\0') {
    len++;
  }

  return len;
}
