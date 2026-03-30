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

MBX_HOT MBX_ALWAYS_INLINE MBX_PURE static int
memcmp_internal(void const *p1, void const *p2, size_t n) {
  uint8_t *u1 = (uint8_t *)p1, *u2 = (uint8_t *)p2;

  for (uintptr_t v1, v2; n >= sizeof(uintptr_t); u1 += sizeof(uintptr_t),
                                                 u2 += sizeof(uintptr_t),
                                                 n -= sizeof(uintptr_t)) {
    MBX_MEMCPY(&v1, u1, sizeof(uintptr_t));
    MBX_MEMCPY(&v2, u2, sizeof(uintptr_t));

    if (v1 != v2) {
      goto memcmp_internal_mismatch;
    }
  }

  for (; n > 0; n--, u1++, u2++) {
    if (*u1 != *u2) {
      goto memcmp_internal_mismatch;
    }
  }

  return 0;

memcmp_internal_mismatch:
  return *u1 - *u2;
}

MBX_HOT MBX_PURE bool mbx_bcmp(void const *p1, void const *p2, size_t n) {
  return memcmp_internal(p1, p2, n) == 0;
}

MBX_HOT MBX_PURE int mbx_memcmp(void const *const p1, void const *const p2,
                                size_t n) {
  return memcmp_internal(p1, p2, n);
}

MBX_HOT MBX_PURE size_t mbx_strlen(char const *str) {
  for (size_t len = 0;; len++) {
    if (str[len] == '\0') {
      return len;
    }
  }

  MBX_UNREACHABLE();
}

MBX_HOT MBX_PURE char const *mbx_strchr(char const *str, char const c) {
  for (; *str != '\0'; str++) {
    if (*str == c) {
      return str;
    }
  }

  return NULL;
}

MBX_HOT MBX_PURE int mbx_strcmp(char const *s1, char const *s2) {
  uint8_t *u1 = (uint8_t *)s1, *u2 = (uint8_t *)s2;

  for (int d;; u1++, u2++) {
    d = *u1 - *u2;

    if (d != 0) {
      return d;
    } else if (*u1 == '\0') {
      break;
    }
  }

  return 0;
}
