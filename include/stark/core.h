//
// stark - a C99+ utility library - stark_core - freestanding compatible libc
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

#ifndef STARK_CORE_H
#define STARK_CORE_H

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
#define STARK_COMPILER_CLANG 1
#elif defined(__GNUC__)
#define STARK_COMPILER_GCC 1
#endif

#if defined(STARK_COMPILER_CLANG) || defined(STARK_COMPILER_GCC)
#define STARK_COMPILER_GCC_LIKE 1
#endif

// ============================================================
// GCC/Clang macros
// ============================================================

#ifdef STARK_COMPILER_GCC_LIKE

// Inline assembly
#define STARK_ASM(...) __asm__(__VA_ARGS__)
#define STARK_ASM_VOLATILE(...) __asm__ __volatile__(__VA_ARGS__)

// Optimization hints
#define STARK_HOT __attribute__((hot))
#define STARK_COLD __attribute__((cold))
#define STARK_FLATTEN __attribute__((flatten))
#define STARK_ALWAYS_INLINE __attribute__((always_inline))
#define STARK_NOINLINE __attribute__((noinline))

// Purity
#define STARK_PURE __attribute__((pure))
#define STARK_CONST __attribute__((const))

// Control flow
#define STARK_NORETURN __attribute__((noreturn))

// Diagnostics
#define STARK_UNUSED __attribute__((unused))
#define STARK_DEPRECATED(msg) __attribute__((deprecated(msg)))
#define STARK_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#define STARK_FALLTHROUGH __attribute__((fallthrough))

// Nullability
#define STARK_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#define STARK_RETURNS_NONNULL __attribute__((returns_nonnull))

// Memory
#define STARK_MALLOC __attribute__((malloc))
#define STARK_ALLOC_SIZE(n) __attribute__((alloc_size(n)))
#define STARK_ALLOC_SIZE2(n, m) __attribute__((alloc_size(n, m)))
#define STARK_ALLOC_ALIGN(n) __attribute__((alloc_align(n)))

// Layout
#define STARK_PACKED __attribute__((packed))
#define STARK_ALIGNED(n) __attribute__((aligned(n)))

// Linker
#define STARK_SECTION(name) __attribute__((section(name)))
#define STARK_WEAK __attribute__((weak))
#define STARK_USED __attribute__((used))
#define STARK_ALIAS(name) __attribute__((alias(name)))
#define STARK_CONSTRUCTOR __attribute__((constructor))
#define STARK_DESTRUCTOR __attribute__((destructor))
#define STARK_VISIBILITY(v) __attribute__((visibility(v)))

// Integer overflow
#define STARK_ADD_OVERFLOW(a, b, r) __builtin_add_overflow((a), (b), (r))
#define STARK_SUB_OVERFLOW(a, b, r) __builtin_sub_overflow((a), (b), (r))
#define STARK_MUL_OVERFLOW(a, b, r) __builtin_mul_overflow((a), (b), (r))
#define STARK_ADD_OVERFLOW_P(a, b) __builtin_add_overflow_p((a), (b))
#define STARK_SUB_OVERFLOW_P(a, b) __builtin_sub_overflow_p((a), (b))
#define STARK_MUL_OVERFLOW_P(a, b) __builtin_mul_overflow_p((a), (b))

#define STARK_SADD_OVERFLOW(a, b, r) __builtin_sadd_overflow((a), (b), (r))
#define STARK_SSUB_OVERFLOW(a, b, r) __builtin_ssub_overflow((a), (b), (r))
#define STARK_SMUL_OVERFLOW(a, b, r) __builtin_smul_overflow((a), (b), (r))
#define STARK_SADDL_OVERFLOW(a, b, r) __builtin_saddl_overflow((a), (b), (r))
#define STARK_SSUBL_OVERFLOW(a, b, r) __builtin_ssubl_overflow((a), (b), (r))
#define STARK_SMULL_OVERFLOW(a, b, r) __builtin_smull_overflow((a), (b), (r))
#define STARK_SADDLL_OVERFLOW(a, b, r) __builtin_saddll_overflow((a), (b), (r))
#define STARK_SSUBLL_OVERFLOW(a, b, r) __builtin_ssubll_overflow((a), (b), (r))
#define STARK_SMULLL_OVERFLOW(a, b, r) __builtin_smulll_overflow((a), (b), (r))

#define STARK_UADD_OVERFLOW(a, b, r) __builtin_uadd_overflow((a), (b), (r))
#define STARK_USUB_OVERFLOW(a, b, r) __builtin_usub_overflow((a), (b), (r))
#define STARK_UMUL_OVERFLOW(a, b, r) __builtin_umull_overflow((a), (b), (r))
#define STARK_UADDL_OVERFLOW(a, b, r) __builtin_uaddl_overflow((a), (b), (r))
#define STARK_USUBL_OVERFLOW(a, b, r) __builtin_usubl_overflow((a), (b), (r))
#define STARK_UMULL_OVERFLOW(a, b, r) __builtin_umull_overflow((a), (b), (r))
#define STARK_UADDLL_OVERFLOW(a, b, r) __builtin_uaddll_overflow((a), (b), (r))
#define STARK_USUBLL_OVERFLOW(a, b, r) __builtin_usubll_overflow((a), (b), (r))
#define STARK_UMULLL_OVERFLOW(a, b, r) __builtin_umulll_overflow((a), (b), (r))

// Bit manipulation
#define STARK_CLZ(x) __builtin_clz((x))
#define STARK_CLZL(x) __builtin_clzl((x))
#define STARK_CLZLL(x) __builtin_clzll((x))
#define STARK_CTZ(x) __builtin_ctz((x))
#define STARK_CTZL(x) __builtin_ctzl((x))
#define STARK_CTZLL(x) __builtin_ctzll((x))
#define STARK_CLRSB(x) __builtin_clrsb((x))
#define STARK_CLRSBL(x) __builtin_clrsbl((x))
#define STARK_CLRSBLL(x) __builtin_clrsbll((x))
#define STARK_FFS(x) __builtin_ffs((x))
#define STARK_FFSL(x) __builtin_ffsl((x))
#define STARK_FFSLL(x) __builtin_ffsll((x))
#define STARK_POPCOUNT(x) __builtin_popcount((x))
#define STARK_POPCOUNTL(x) __builtin_popcountl((x))
#define STARK_POPCOUNTLL(x) __builtin_popcountll((x))
#define STARK_PARITY(x) __builtin_parity((x))
#define STARK_PARITYL(x) __builtin_parityl((x))
#define STARK_PARITYLL(x) __builtin_parityll((x))
#define STARK_BSWAP16(x) __builtin_bswap16((x))
#define STARK_BSWAP32(x) __builtin_bswap32((x))
#define STARK_BSWAP64(x) __builtin_bswap64((x))

// Floating point - classification
#define STARK_FPCLASSIFY(x) __builtin_fpclassify((x))
#define STARK_ISFINITE(x) __builtin_isfinite((x))
#define STARK_ISINF(x) __builtin_isinf((x))
#define STARK_ISNAN(x) __builtin_isnan((x))
#define STARK_ISNORMAL(x) __builtin_isnormal((x))
#define STARK_ISZERO(x) __builtin_iszero((x))
#define STARK_SIGNBIT(x) __builtin_signbit((x))
#define STARK_ISNANF(x) __builtin_isnanf((x))
#define STARK_ISINFF(x) __builtin_isinff((x))

// Floating point - arithmetic
#define STARK_FABS(x) __builtin_fabs((x))
#define STARK_FABSF(x) __builtin_fabsf((x))
#define STARK_FABSL(x) __builtin_fabsl((x))
#define STARK_SQRT(x) __builtin_sqrt((x))
#define STARK_SQRTF(x) __builtin_sqrtf((x))
#define STARK_SQRTL(x) __builtin_sqrtl((x))
#define STARK_CEIL(x) __builtin_ceil((x))
#define STARK_CEILF(x) __builtin_ceilf((x))
#define STARK_CEILL(x) __builtin_ceill((x))
#define STARK_FLOOR(x) __builtin_floor((x))
#define STARK_FLOORF(x) __builtin_floorf((x))
#define STARK_FLOORL(x) __builtin_floorl((x))
#define STARK_ROUND(x) __builtin_round((x))
#define STARK_ROUNDF(x) __builtin_roundf((x))
#define STARK_ROUNDL(x) __builtin_roundl((x))
#define STARK_TRUNC(x) __builtin_trunc((x))
#define STARK_TRUNCF(x) __builtin_truncf((x))
#define STARK_TRUNCL(x) __builtin_truncl((x))

// Floating point - misc
#define STARK_COPYSIGN(x, y) __builtin_copysign((x), (y))
#define STARK_COPYSIGNF(x, y) __builtin_copysignf((x), (y))
#define STARK_COPYSIGNL(x, y) __builtin_copysignl((x), (y))
#define STARK_FMAX(x, y) __builtin_fmax((x), (y))
#define STARK_FMAXF(x, y) __builtin_fmaxf((x), (y))
#define STARK_FMAXL(x, y) __builtin_fmaxl((x), (y))
#define STARK_FMIN(x, y) __builtin_fmin((x), (y))
#define STARK_FMINF(x, y) __builtin_fminf((x), (y))
#define STARK_FMINL(x, y) __builtin_fminl((x), (y))
#define STARK_ISGREATER(x, y) __builtin_isgreater((x), (y))
#define STARK_ISGREATEREQUAL(x, y) __builtin_isgreaterequal((x), (y))
#define STARK_ISLESS(x, y) __builtin_isless((x), (y))
#define STARK_ISLESSEQUAL(x, y) __builtin_islessequal((x), (y))
#define STARK_ISLESSGREATER(x, y) __builtin_islessgreater((x), (y))
#define STARK_ISUNORDERED(x, y) __builtin_isunordered((x), (y))
#define STARK_HUGE_VAL() __builtin_huge_val()
#define STARK_HUGE_VALF() __builtin_huge_valf()
#define STARK_HUGE_VALL() __builtin_huge_vall()
#define STARK_INF() __builtin_inf()
#define STARK_INFF() __builtin_inff()
#define STARK_INFL() __builtin_infl()
#define STARK_NAN(n) __builtin_nan((n))
#define STARK_NANF(n) __builtin_nanf((n))
#define STARK_NANL(n) __builtin_nanl((n))
#define STARK_NANS(n) __builtin_nans((n))
#define STARK_NANSF(n) __builtin_nansf((n))
#define STARK_NANSL(n) __builtin_nansl((n))

// Floating point - complex
#define STARK_CREAL(x) __builtin_creal((x))
#define STARK_CREALF(x) __builtin_crealf((x))
#define STARK_CREALL(x) __builtin_creall((x))
#define STARK_CIMAG(x) __builtin_cimag((x))
#define STARK_CIMAGF(x) __builtin_cimagf((x))
#define STARK_CIMAGL(x) __builtin_cimagl((x))
#define STARK_CONJ(x) __builtin_conj((x))
#define STARK_CONJF(x) __builtin_conjf((x))
#define STARK_CONJL(x) __builtin_conjl((x))

// Control flow
#define STARK_UNREACHABLE() __builtin_unreachable()
#define STARK_TRAP() __builtin_trap()
#define STARK_DEBUGTRAP() __builtin_debugtrap()
#define STARK_ASSUME(cond) __builtin_assume((cond))
#define STARK_CONSTANT_P(x) __builtin_constant_p((x))

// Memory
#define STARK_MEMCPY(dst, src, n) __builtin_memcpy((dst), (src), (n))
#define STARK_MEMMOVE(dst, src, n) __builtin_memmove((dst), (src), (n))
#define STARK_MEMSET(dst, val, n) __builtin_memset((dst), (val), (n))
#define STARK_MEMCMP(a, b, n) __builtin_memcmp((a), (b), (n))
#define STARK_MEMCHR(s, c, n) __builtin_memchr((s), (c), (n))

// Strings
#define STARK_STRLEN(s) __builtin_strlen((s))
#define STARK_STRCHR(s, c) (char const *)__builtin_strchr((s), (c))
#define STARK_STRCMP(s1, s2) __builtin_strcmp((s1), (s2))
#define STARK_STRNCMP(s1, s2, n) __builtin_strncmp((s1), (s2), (n))

// Branch hints
#define STARK_EXPECT_TRUE(x) __builtin_expect(!!(x), 1)
#define STARK_EXPECT_FALSE(x) __builtin_expect(!!(x), 0)

// Prefetch
#define STARK_PREFETCH(addr, rw, locality)                                     \
  __builtin_prefetch((addr), (rw), (locality))

// Alignment
#define STARK_ASSUME_ALIGNED(p, align) __builtin_assume_aligned((p), (align))

// Type checking
#define STARK_TYPES_COMPATIBLE(a, b) __builtin_types_compatible_p((a), (b))

// Compile-time selection
#define STARK_CHOOSE_EXPR(cond, a, b) __builtin_choose_expr((cond), (a), (b))

// Object size
#define STARK_OBJECT_SIZE(p, t) __builtin_object_size((p), (t))

// Stack
#define STARK_RETURN_ADDRESS(level) __builtin_return_address((level))
#define STARK_FRAME_ADDRESS(level) __builtin_frame_address((level))
#define STARK_EXTRACT_RETURN_ADDR(addr) __builtin_extract_return_addr((addr))

// Stack allocation
#define STARK_ALLOCA(n) __builtin_alloca((n))

// Instruction cache
#define STARK_CLEAR_CACHE(begin, end) __builtin___clear_cache((begin), (end))

#else

// Fallbacks for unsupported compilers
#define STARK_HOT
#define STARK_COLD
#define STARK_FLATTEN
#define STARK_ALWAYS_INLINE
#define STARK_NOINLINE
#define STARK_PURE
#define STARK_CONST
#define STARK_NORETURN
#define STARK_UNUSED
#define STARK_DEPRECATED(msg)
#define STARK_WARN_UNUSED_RESULT
#define STARK_FALLTHROUGH
#define STARK_NONNULL(...)
#define STARK_RETURNS_NONNULL
#define STARK_MALLOC
#define STARK_PACKED
#define STARK_ALIGNED(n)
#define STARK_SECTION(name)
#define STARK_WEAK
#define STARK_USED
#define STARK_ALIAS(name)
#define STARK_CONSTRUCTOR
#define STARK_DESTRUCTOR
#define STARK_EXPECT_TRUE(x) (x)
#define STARK_EXPECT_FALSE(x) (x)
#define STARK_UNREACHABLE() (*(volatile int *)NULL = 0)
#define STARK_TRAP() (*(volatile int *)NULL = 0)
#define STARK_DEBUGTRAP() ((void)NULL)

static inline void _stark_memcpy(void *dst, void const *src, size_t n) {
  uint8_t const *restrict s = (uint8_t const *)src;

  for (uint8_t *restrict d = dst; n-- > 0; *d++ = *s++)
    ;
}

#define STARK_MEMCPY(dst, src, n) _stark_memcpy((dst), (src), (n))

static inline void _stark_memmove(void *dst, void const *src, size_t n) {
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

#define STARK_MEMMOVE(dst, src, n) _stark_memmove((dst), (src), (n))

static inline void _stark_memset(void *restrict dst, int val, size_t n) {
  for (uint8_t *d = dst; n-- > 0; *d++ = (uint8_t)val)
    ;
}

#define STARK_MEMSET(dst, val, n) _stark_memset((dst), (val), (n))

static inline int _stark_memcmp(void const *restrict p1,
                                void const *restrict p2, size_t n) {
  for (uint8_t const *restrict u1 = p1, *restrict u2 = p2; n-- > 0;
       u1++, u2++) {
    if (*u1 != *u2) {
      return *u1 - *u2;
    }
  }

  return 0;
}

#define STARK_MEMCMP(a, b, n) _stark_memcmp((a), (b), (n))

static inline size_t _stark_strlen(char const *restrict str) {
  for (size_t len = 0;; len++) {
    if (str[len] == '\0') {
      return len;
    }
  }

  STARK_UNREACHABLE();
}

#define STARK_STRLEN(s) _stark_strlen((s))

static inline char const *_stark_strchr(char const *restrict str,
                                        char const c) {
  for (;; str++) {
    if (*str == c) {
      return str;
    } else if (*str == '\0') {
      return NULL;
    }
  }

  STARK_UNREACHABLE();
}

#define STARK_STRCHR(s, c) _stark_strchr(s, c)

static inline int _stark_strcmp(char const *s1, char const *s2) {
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

#define STARK_STRCMP(s1, s2) _stark_strcmp(s1, s2)

#endif // STARK_COMPILER_GCC_LIKE

// ============================================================
// C language macros
// ============================================================

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define STARK_STATIC_ASSERT(cond, msg) _Static_assert((cond), (msg))
#define STARK_ALIGNOF(type) _Alignof(type)
#else
#define STARK_STATIC_ASSERT_CONCAT_(a, b) a##b
#define STARK_STATIC_ASSERT_CONCAT(a, b) STARK_STATIC_ASSERT_CONCAT_(a, b)
#define STARK_STATIC_ASSERT(cond, msg)                                         \
  STARK_UNUSED typedef char STARK_STATIC_ASSERT_CONCAT(                        \
      stark_static_assert_, __LINE__)[(cond) ? 1 : -1]
#define STARK_ALIGNOF(type)                                                    \
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

#endif // STARK_CORE_H
