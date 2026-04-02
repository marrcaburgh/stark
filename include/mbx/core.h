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

#ifndef MBX_CORE_H
#define MBX_CORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================
// Compiler detection
// ============================================================

#ifdef __clang__
#define MBX_COMPILER_CLANG 1
#elif defined(__GNUC__)
#define MBX_COMPILER_GCC 1
#endif

#if defined(MBX_COMPILER_CLANG) || defined(MBX_COMPILER_GCC)
#define MBX_COMPILER_GCC_LIKE 1
#endif

// ============================================================
// GCC/Clang macros
// ============================================================

#ifdef MBX_COMPILER_GCC_LIKE

// Inline assembly
#define MBX_ASM(...) __asm__(__VA_ARGS__)
#define MBX_ASM_VOLATILE(...) __asm__ __volatile__(__VA_ARGS__)

// Optimization hints
#define MBX_HOT __attribute__((hot))
#define MBX_COLD __attribute__((cold))
#define MBX_FLATTEN __attribute__((flatten))
#define MBX_ALWAYS_INLINE __attribute__((always_inline))
#define MBX_NOINLINE __attribute__((noinline))

// Purity
#define MBX_PURE __attribute__((pure))
#define MBX_CONST __attribute__((const))

// Control flow
#define MBX_NORETURN __attribute__((noreturn))

// Diagnostics
#define MBX_UNUSED __attribute__((unused))
#define MBX_DEPRECATED(msg) __attribute__((deprecated(msg)))
#define MBX_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#define MBX_FALLTHROUGH __attribute__((fallthrough))

// Nullability
#define MBX_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#define MBX_RETURNS_NONNULL __attribute__((returns_nonnull))

// Memory
#define MBX_MALLOC __attribute__((malloc))
#define MBX_ALLOC_SIZE(n) __attribute__((alloc_size(n)))
#define MBX_ALLOC_SIZE2(n, m) __attribute__((alloc_size(n, m)))
#define MBX_ALLOC_ALIGN(n) __attribute__((alloc_align(n)))

// Layout
#define MBX_PACKED __attribute__((packed))
#define MBX_ALIGNED(n) __attribute__((aligned(n)))

// Linker
#define MBX_SECTION(name) __attribute__((section(name)))
#define MBX_WEAK __attribute__((weak))
#define MBX_USED __attribute__((used))
#define MBX_ALIAS(name) __attribute__((alias(name)))
#define MBX_CONSTRUCTOR __attribute__((constructor))
#define MBX_DESTRUCTOR __attribute__((destructor))
#define MBX_VISIBILITY(v) __attribute__((visibility(v)))

// Integer overflow
#define MBX_ADD_OVERFLOW(a, b, r) __builtin_add_overflow((a), (b), (r))
#define MBX_SUB_OVERFLOW(a, b, r) __builtin_sub_overflow((a), (b), (r))
#define MBX_MUL_OVERFLOW(a, b, r) __builtin_mul_overflow((a), (b), (r))
#define MBX_ADD_OVERFLOW_P(a, b) __builtin_add_overflow_p((a), (b))
#define MBX_SUB_OVERFLOW_P(a, b) __builtin_sub_overflow_p((a), (b))
#define MBX_MUL_OVERFLOW_P(a, b) __builtin_mul_overflow_p((a), (b))

#define MBX_SADD_OVERFLOW(a, b, r) __builtin_sadd_overflow((a), (b), (r))
#define MBX_SSUB_OVERFLOW(a, b, r) __builtin_ssub_overflow((a), (b), (r))
#define MBX_SMUL_OVERFLOW(a, b, r) __builtin_smul_overflow((a), (b), (r))
#define MBX_SADDL_OVERFLOW(a, b, r) __builtin_saddl_overflow((a), (b), (r))
#define MBX_SSUBL_OVERFLOW(a, b, r) __builtin_ssubl_overflow((a), (b), (r))
#define MBX_SMULL_OVERFLOW(a, b, r) __builtin_smull_overflow((a), (b), (r))
#define MBX_SADDLL_OVERFLOW(a, b, r) __builtin_saddll_overflow((a), (b), (r))
#define MBX_SSUBLL_OVERFLOW(a, b, r) __builtin_ssubll_overflow((a), (b), (r))
#define MBX_SMULLL_OVERFLOW(a, b, r) __builtin_smulll_overflow((a), (b), (r))

#define MBX_UADD_OVERFLOW(a, b, r) __builtin_uadd_overflow((a), (b), (r))
#define MBX_USUB_OVERFLOW(a, b, r) __builtin_usub_overflow((a), (b), (r))
#define MBX_UMUL_OVERFLOW(a, b, r) __builtin_umull_overflow((a), (b), (r))
#define MBX_UADDL_OVERFLOW(a, b, r) __builtin_uaddl_overflow((a), (b), (r))
#define MBX_USUBL_OVERFLOW(a, b, r) __builtin_usubl_overflow((a), (b), (r))
#define MBX_UMULL_OVERFLOW(a, b, r) __builtin_umull_overflow((a), (b), (r))
#define MBX_UADDLL_OVERFLOW(a, b, r) __builtin_uaddll_overflow((a), (b), (r))
#define MBX_USUBLL_OVERFLOW(a, b, r) __builtin_usubll_overflow((a), (b), (r))
#define MBX_UMULLL_OVERFLOW(a, b, r) __builtin_umulll_overflow((a), (b), (r))

// Bit manipulation
#define MBX_CLZ(x) __builtin_clz((x))
#define MBX_CLZL(x) __builtin_clzl((x))
#define MBX_CLZLL(x) __builtin_clzll((x))
#define MBX_CTZ(x) __builtin_ctz((x))
#define MBX_CTZL(x) __builtin_ctzl((x))
#define MBX_CTZLL(x) __builtin_ctzll((x))
#define MBX_CLRSB(x) __builtin_clrsb((x))
#define MBX_CLRSBL(x) __builtin_clrsbl((x))
#define MBX_CLRSBLL(x) __builtin_clrsbll((x))
#define MBX_FFS(x) __builtin_ffs((x))
#define MBX_FFSL(x) __builtin_ffsl((x))
#define MBX_FFSLL(x) __builtin_ffsll((x))
#define MBX_POPCOUNT(x) __builtin_popcount((x))
#define MBX_POPCOUNTL(x) __builtin_popcountl((x))
#define MBX_POPCOUNTLL(x) __builtin_popcountll((x))
#define MBX_PARITY(x) __builtin_parity((x))
#define MBX_PARITYL(x) __builtin_parityl((x))
#define MBX_PARITYLL(x) __builtin_parityll((x))
#define MBX_BSWAP16(x) __builtin_bswap16((x))
#define MBX_BSWAP32(x) __builtin_bswap32((x))
#define MBX_BSWAP64(x) __builtin_bswap64((x))

// Floating point - classification
#define MBX_FPCLASSIFY(x) __builtin_fpclassify((x))
#define MBX_ISFINITE(x) __builtin_isfinite((x))
#define MBX_ISINF(x) __builtin_isinf((x))
#define MBX_ISNAN(x) __builtin_isnan((x))
#define MBX_ISNORMAL(x) __builtin_isnormal((x))
#define MBX_ISZERO(x) __builtin_iszero((x))
#define MBX_SIGNBIT(x) __builtin_signbit((x))
#define MBX_ISNANF(x) __builtin_isnanf((x))
#define MBX_ISINFF(x) __builtin_isinff((x))

// Floating point - arithmetic
#define MBX_FABS(x) __builtin_fabs((x))
#define MBX_FABSF(x) __builtin_fabsf((x))
#define MBX_FABSL(x) __builtin_fabsl((x))
#define MBX_SQRT(x) __builtin_sqrt((x))
#define MBX_SQRTF(x) __builtin_sqrtf((x))
#define MBX_SQRTL(x) __builtin_sqrtl((x))
#define MBX_CEIL(x) __builtin_ceil((x))
#define MBX_CEILF(x) __builtin_ceilf((x))
#define MBX_CEILL(x) __builtin_ceill((x))
#define MBX_FLOOR(x) __builtin_floor((x))
#define MBX_FLOORF(x) __builtin_floorf((x))
#define MBX_FLOORL(x) __builtin_floorl((x))
#define MBX_ROUND(x) __builtin_round((x))
#define MBX_ROUNDF(x) __builtin_roundf((x))
#define MBX_ROUNDL(x) __builtin_roundl((x))
#define MBX_TRUNC(x) __builtin_trunc((x))
#define MBX_TRUNCF(x) __builtin_truncf((x))
#define MBX_TRUNCL(x) __builtin_truncl((x))

// Floating point - misc
#define MBX_COPYSIGN(x, y) __builtin_copysign((x), (y))
#define MBX_COPYSIGNF(x, y) __builtin_copysignf((x), (y))
#define MBX_COPYSIGNL(x, y) __builtin_copysignl((x), (y))
#define MBX_FMAX(x, y) __builtin_fmax((x), (y))
#define MBX_FMAXF(x, y) __builtin_fmaxf((x), (y))
#define MBX_FMAXL(x, y) __builtin_fmaxl((x), (y))
#define MBX_FMIN(x, y) __builtin_fmin((x), (y))
#define MBX_FMINF(x, y) __builtin_fminf((x), (y))
#define MBX_FMINL(x, y) __builtin_fminl((x), (y))
#define MBX_ISGREATER(x, y) __builtin_isgreater((x), (y))
#define MBX_ISGREATEREQUAL(x, y) __builtin_isgreaterequal((x), (y))
#define MBX_ISLESS(x, y) __builtin_isless((x), (y))
#define MBX_ISLESSEQUAL(x, y) __builtin_islessequal((x), (y))
#define MBX_ISLESSGREATER(x, y) __builtin_islessgreater((x), (y))
#define MBX_ISUNORDERED(x, y) __builtin_isunordered((x), (y))
#define MBX_HUGE_VAL() __builtin_huge_val()
#define MBX_HUGE_VALF() __builtin_huge_valf()
#define MBX_HUGE_VALL() __builtin_huge_vall()
#define MBX_INF() __builtin_inf()
#define MBX_INFF() __builtin_inff()
#define MBX_INFL() __builtin_infl()
#define MBX_NAN(n) __builtin_nan((n))
#define MBX_NANF(n) __builtin_nanf((n))
#define MBX_NANL(n) __builtin_nanl((n))
#define MBX_NANS(n) __builtin_nans((n))
#define MBX_NANSF(n) __builtin_nansf((n))
#define MBX_NANSL(n) __builtin_nansl((n))

// Floating point - complex
#define MBX_CREAL(x) __builtin_creal((x))
#define MBX_CREALF(x) __builtin_crealf((x))
#define MBX_CREALL(x) __builtin_creall((x))
#define MBX_CIMAG(x) __builtin_cimag((x))
#define MBX_CIMAGF(x) __builtin_cimagf((x))
#define MBX_CIMAGL(x) __builtin_cimagl((x))
#define MBX_CONJ(x) __builtin_conj((x))
#define MBX_CONJF(x) __builtin_conjf((x))
#define MBX_CONJL(x) __builtin_conjl((x))

// Control flow
#define MBX_UNREACHABLE() __builtin_unreachable()
#define MBX_TRAP() __builtin_trap()
#define MBX_DEBUGTRAP() __builtin_debugtrap()
#define MBX_ASSUME(cond) __builtin_assume((cond))
#define MBX_CONSTANT_P(x) __builtin_constant_p((x))

// Memory
#define MBX_MEMCPY(dst, src, n) __builtin_memcpy((dst), (src), (n))
#define MBX_MEMMOVE(dst, src, n) __builtin_memmove((dst), (src), (n))
#define MBX_MEMSET(dst, val, n) __builtin_memset((dst), (val), (n))
#define MBX_MEMCMP(a, b, n) __builtin_memcmp((a), (b), (n))
#define MBX_MEMCHR(s, c, n) __builtin_memchr((s), (c), (n))

// Strings
#define MBX_STRLEN(s) __builtin_strlen((s))
#define MBX_STRCHR(s, c) (char const *)__builtin_strchr((s), (c))
#define MBX_STRCMP(s1, s2) __builtin_strcmp((s1), (s2))
#define MBX_STRNCMP(s1, s2, n) __builtin_strncmp((s1), (s2), (n))

// Branch hints
#define MBX_EXPECT_TRUE(x) __builtin_expect(!!(x), 1)
#define MBX_EXPECT_FALSE(x) __builtin_expect(!!(x), 0)

// Prefetch
#define MBX_PREFETCH(addr, rw, locality)                                       \
  __builtin_prefetch((addr), (rw), (locality))

// Alignment
#define MBX_ASSUME_ALIGNED(p, align) __builtin_assume_aligned((p), (align))

// Type checking
#define MBX_TYPES_COMPATIBLE(a, b) __builtin_types_compatible_p((a), (b))

// Compile-time selection
#define MBX_CHOOSE_EXPR(cond, a, b) __builtin_choose_expr((cond), (a), (b))

// Object size
#define MBX_OBJECT_SIZE(p, t) __builtin_object_size((p), (t))

// Stack
#define MBX_RETURN_ADDRESS(level) __builtin_return_address((level))
#define MBX_FRAME_ADDRESS(level) __builtin_frame_address((level))
#define MBX_EXTRACT_RETURN_ADDR(addr) __builtin_extract_return_addr((addr))

// Stack allocation
#define MBX_ALLOCA(n) __builtin_alloca((n))

// Instruction cache
#define MBX_CLEAR_CACHE(begin, end) __builtin___clear_cache((begin), (end))

#else

// Fallbacks for unsupported compilers
#define MBX_HOT
#define MBX_COLD
#define MBX_FLATTEN
#define MBX_ALWAYS_INLINE
#define MBX_NOINLINE
#define MBX_PURE
#define MBX_CONST
#define MBX_NORETURN
#define MBX_UNUSED
#define MBX_DEPRECATED(msg)
#define MBX_WARN_UNUSED_RESULT
#define MBX_FALLTHROUGH
#define MBX_NONNULL(...)
#define MBX_RETURNS_NONNULL
#define MBX_MALLOC
#define MBX_PACKED
#define MBX_ALIGNED(n)
#define MBX_SECTION(name)
#define MBX_WEAK
#define MBX_USED
#define MBX_ALIAS(name)
#define MBX_CONSTRUCTOR
#define MBX_DESTRUCTOR
#define MBX_EXPECT_TRUE(x) (x)
#define MBX_EXPECT_FALSE(x) (x)
#define MBX_UNREACHABLE() (*(volatile int *)NULL = NULL)
#define MBX_TRAP() (*(volatile int *)NULL = NULL)
#define MBX_DEBUGTRAP() ((void)NULL)

static inline void _mbx_memcpy(void *dst, void const *src, size_t n) {
  uint8_t const *restrict s = (uint8_t const *)src;

  for (uint8_t *restrict d = dst; n-- > 0; *d++ = *s++)
    ;
}

#define MBX_MEMCPY(dst, src, n) _mbx_memcpy((dst), (src), (n))

static inline void _mbx_memmove(void *dst, void const *src, size_t n) {
  uint8_t *d = dst;
  uint8_t const *s = src;

  if (d < s) {
    for (; n-- > 0; *d++ = *s++)
      ;
  } else {
    d += n;
    s += n;

    for (; n-- > 0; *--d = *--s)
      ;
  }
}

#define MBX_MEMMOVE(dst, src, n) _mbx_memmove((dst), (src), (n))

static inline void _mbx_memset(void *restrict dst, int val, size_t n) {
  for (uint8_t *d = dst; n-- > 0; *d++ = (uint8_t)val)
    ;
}

#define MBX_MEMSET(dst, val, n) _mbx_memset((dst), (val), (n))

static inline int _mbx_memcmp(void const *restrict p1, void const *restrict p2,
                              size_t n) {
  for (uint8_t const *restrict u1 = p1, *restrict u2 = p2; n-- > 0;
       u1++, u2++) {
    if (*u1 != *u2) {
      return *u1 - *u2;
    }
  }

  return 0;
}

#define MBX_MEMCMP(a, b, n) _mbx_memcmp((a), (b), (n))

static inline size_t _mbx_strlen(char const *restrict str) {
  for (size_t len = 0;; len++) {
    if (str[len] == '\0') {
      return len;
    }
  }
}

#define MBX_STRLEN(s) _mbx_strlen((s))

static inline char const *_mbx_strchr(char const *restrict str, char const c) {
  for (;; str++) {
    if (*str == c) {
      return str;
    } else if (*str == '\0') {
      return NULL;
    }
  }
}

#define MBX_STRCHR(s, c) _mbx_strchr(s, c)

static inline int _mbx_strcmp(char const *s1, char const *s2) {
  uint8_t const *restrict u1 = (uint8_t const *)s1, *restrict u2 =
                                                        (uint8_t const *)s2;

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

#define MBX_STRCMP(s1, s2) _mbx_strcmp(s1, s2)

#endif // MBX_COMPILER_GCC_LIKE

// ============================================================
// C language macros
// ============================================================

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define MBX_STATIC_ASSERT(cond, msg) _Static_assert((cond), (msg))
#define MBX_ALIGNOF(type) _Alignof(type)
#else
#define MBX_STATIC_ASSERT_CONCAT_(a, b) a##b
#define MBX_STATIC_ASSERT_CONCAT(a, b) MBX_STATIC_ASSERT_CONCAT_(a, b)
#define MBX_STATIC_ASSERT(cond, msg)                                           \
  MBX_UNUSED typedef char MBX_STATIC_ASSERT_CONCAT(mbx_static_assert_,         \
                                                   __LINE__)[(cond) ? 1 : -1]
#define MBX_ALIGNOF(type)                                                      \
  offsetof(                                                                    \
      struct {                                                                 \
        char c;                                                                \
        type t;                                                                \
      },                                                                       \
      t)
#endif

#ifdef __cplusplus
}
#endif

#endif // MBX_CORE_H
