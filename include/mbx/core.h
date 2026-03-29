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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#if defined(__GNUC__) || defined(__clang__)

// Integer overflow
#define MBX_ADD_OVERFLOW(a, b, r) __builtin_add_overflow((a), (b), (r))
#define MBX_SUB_OVERFLOW(a, b, r) __builtin_sub_overflow((a), (b), (r))
#define MBX_MUL_OVERFLOW(a, b, r) __builtin_mul_overflow((a), (b), (r))
#define MBX_SADD_OVERFLOW(a, b, r) __builtin_sadd_overflow((a), (b), (r))
#define MBX_SSUB_OVERFLOW(a, b, r) __builtin_ssub_overflow((a), (b), (r))
#define MBX_SMUL_OVERFLOW(a, b, r) __builtin_smul_overflow((a), (b), (r))
#define MBX_UADD_OVERFLOW(a, b, r) __builtin_uadd_overflow((a), (b), (r))
#define MBX_USUB_OVERFLOW(a, b, r) __builtin_usub_overflow((a), (b), (r))
#define MBX_UMUL_OVERFLOW(a, b, r) __builtin_umul_overflow((a), (b), (r))

// Bit manipulation
#define MBX_CLZ(x) __builtin_clz((x))
#define MBX_CLZLL(x) __builtin_clzll((x))
#define MBX_CTZ(x) __builtin_ctz((x))
#define MBX_CTZLL(x) __builtin_ctzll((x))
#define MBX_FFS(x) __builtin_ffs((x))
#define MBX_FFSLL(x) __builtin_ffsll((x))
#define MBX_POPCOUNT(x) __builtin_popcount((x))
#define MBX_POPCOUNTLL(x) __builtin_popcountll((x))
#define MBX_PARITY(x) __builtin_parity((x))
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
#define MBX_FMOD(x, y) __builtin_fmod((x), (y))
#define MBX_FMODF(x, y) __builtin_fmodf((x), (y))
#define MBX_FMODL(x, y) __builtin_fmodl((x), (y))
#define MBX_SQRT(x) __builtin_sqrt((x))
#define MBX_SQRTF(x) __builtin_sqrtf((x))
#define MBX_SQRTL(x) __builtin_sqrtl((x))
#define MBX_CBRT(x) __builtin_cbrt((x))
#define MBX_CBRTF(x) __builtin_cbrtf((x))
#define MBX_CBRTL(x) __builtin_cbrtl((x))
#define MBX_HYPOT(x, y) __builtin_hypot((x), (y))
#define MBX_HYPOTF(x, y) __builtin_hypotf((x), (y))
#define MBX_POW(x, y) __builtin_pow((x), (y))
#define MBX_POWF(x, y) __builtin_powf((x), (y))
#define MBX_POWL(x, y) __builtin_powl((x), (y))
#define MBX_EXP(x) __builtin_exp((x))
#define MBX_EXPF(x) __builtin_expf((x))
#define MBX_EXPL(x) __builtin_expl((x))
#define MBX_EXP2(x) __builtin_exp2((x))
#define MBX_EXP2F(x) __builtin_exp2f((x))
#define MBX_EXP2L(x) __builtin_exp2l((x))
#define MBX_EXPM1(x) __builtin_expm1((x))
#define MBX_LOG(x) __builtin_log((x))
#define MBX_LOGF(x) __builtin_logf((x))
#define MBX_LOGL(x) __builtin_logl((x))
#define MBX_LOG2(x) __builtin_log2((x))
#define MBX_LOG2F(x) __builtin_log2f((x))
#define MBX_LOG2L(x) __builtin_log2l((x))
#define MBX_LOG10(x) __builtin_log10((x))
#define MBX_LOG10F(x) __builtin_log10f((x))
#define MBX_LOG10L(x) __builtin_log10l((x))
#define MBX_LOG1P(x) __builtin_log1p((x))
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
#define MBX_NEARBYINT(x) __builtin_nearbyint((x))
#define MBX_RINT(x) __builtin_rint((x))
#define MBX_RINTF(x) __builtin_rintf((x))
#define MBX_LRINT(x) __builtin_lrint((x))
#define MBX_LRINTF(x) __builtin_lrintf((x))
#define MBX_LLRINT(x) __builtin_llrint((x))
#define MBX_LLRINTF(x) __builtin_llrintf((x))
#define MBX_LROUND(x) __builtin_lround((x))
#define MBX_LROUNDF(x) __builtin_lroundf((x))
#define MBX_LLROUND(x) __builtin_llround((x))
#define MBX_LLROUNDF(x) __builtin_llroundf((x))

// Floating point - misc
#define MBX_COPYSIGN(x, y) __builtin_copysign((x), (y))
#define MBX_COPYSIGNF(x, y) __builtin_copysignf((x), (y))
#define MBX_COPYSIGNL(x, y) __builtin_copysignl((x), (y))
#define MBX_NEXTAFTER(x, y) __builtin_nextafter((x), (y))
#define MBX_NEXTAFTERF(x, y) __builtin_nextafterf((x), (y))
#define MBX_NEXTAFTERL(x, y) __builtin_nextafterl((x), (y))
#define MBX_LDEXP(x, n) __builtin_ldexp((x), (n))
#define MBX_LDEXPF(x, n) __builtin_ldexpf((x), (n))
#define MBX_LDEXPL(x, n) __builtin_ldexpl((x), (n))
#define MBX_FREXP(x, p) __builtin_frexp((x), (p))
#define MBX_FREXPF(x, p) __builtin_frexpf((x), (p))
#define MBX_FREXPL(x, p) __builtin_frexpl((x), (p))
#define MBX_MODF(x, p) __builtin_modf((x), (p))
#define MBX_MODFF(x, p) __builtin_modff((x), (p))
#define MBX_MODFL(x, p) __builtin_modfl((x), (p))
#define MBX_FMAX(x, y) __builtin_fmax((x), (y))
#define MBX_FMAXF(x, y) __builtin_fmaxf((x), (y))
#define MBX_FMAXL(x, y) __builtin_fmaxl((x), (y))
#define MBX_FMIN(x, y) __builtin_fmin((x), (y))
#define MBX_FMINF(x, y) __builtin_fminf((x), (y))
#define MBX_FMINL(x, y) __builtin_fminl((x), (y))
#define MBX_FMA(x, y, z) __builtin_fma((x), (y), (z))
#define MBX_FMAF(x, y, z) __builtin_fmaf((x), (y), (z))
#define MBX_FMAL(x, y, z) __builtin_fmal((x), (y), (z))
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
#define MBX_CABS(x) __builtin_cabs((x))
#define MBX_CABSF(x) __builtin_cabsf((x))
#define MBX_CABSL(x) __builtin_cabsl((x))
#define MBX_CARG(x) __builtin_carg((x))
#define MBX_CARGF(x) __builtin_cargf((x))
#define MBX_CARGL(x) __builtin_cargl((x))

// Memory
#define MBX_MEMCPY(dst, src, n) __builtin_memcpy((dst), (src), (n))
#define MBX_MEMMOVE(dst, src, n) __builtin_memmove((dst), (src), (n))
#define MBX_MEMSET(dst, val, n) __builtin_memset((dst), (val), (n))
#define MBX_MEMCMP(a, b, n) __builtin_memcmp((a), (b), (n))

// Control flow
#define MBX_UNREACHABLE() __builtin_unreachable()
#define MBX_PREFETCH(addr) __builtin_prefetch((addr))
#define MBX_ASSUME(cond) __builtin_assume((cond))
#define MBX_CONSTANT_P(x) __builtin_constant_p((x))
#define MBX_TRAP() __builtin_trap()

// Alignment
#define MBX_ASSUME_ALIGNED(p, align) __builtin_assume_aligned((p), (align))

// Type checking
#define MBX_TYPES_COMPATIBLE(a, b) __builtin_types_compatible_p((a), (b))

// Object size
#define MBX_OBJECT_SIZE(p, t) __builtin_object_size((p), (t))

// Stack
#define MBX_RETURN_ADDRESS(level) __builtin_return_address((level))
#define MBX_FRAME_ADDRESS(level) __builtin_frame_address((level))

// Handwritten wrappers
#define MBX_EXPECT_TRUE(x) __builtin_expect(!!(x), 1)
#define MBX_EXPECT_FALSE(x) __builtin_expect(!!(x), 0)

// Attributes
#define MBX_HOT __attribute__((hot))
#define MBX_COLD __attribute__((cold))
#define MBX_FLATTEN __attribute__((flatten))
#define MBX_ALWAYS_INLINE __attribute__((always_inline))
#define MBX_NOINLINE __attribute__((noinline))
#define MBX_PURE __attribute__((pure))
#define MBX_CONST __attribute__((const))
#define MBX_NORETURN __attribute__((noreturn))
#define MBX_UNUSED __attribute__((unused))
#define MBX_DEPRECATED(msg) __attribute__((deprecated(msg)))
#define MBX_MALLOC __attribute__((malloc))
#define MBX_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#define MBX_RETURNS_NONNULL __attribute__((returns_nonnull))
#define MBX_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#define MBX_FALLTHROUGH __attribute__((fallthrough))

#else

// Fallbacks for unsupported compilers
#define MBX_EXPECT_TRUE(x) (x)
#define MBX_EXPECT_FALSE(x) (x)
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
#define MBX_MALLOC
#define MBX_NONNULL(...)
#define MBX_RETURNS_NONNULL
#define MBX_WARN_UNUSED_RESULT
#define MBX_FALLTHROUGH

#endif // GCC/Clang

// ============================================================
// Freestanding functions
// ============================================================

// Compare N bytes of p1 and p2 for equality
MBX_HOT MBX_PURE bool mbx_bcmp(void const *p1, void const *p2, size_t n);
MBX_HOT MBX_CONST size_t mbx_strlen(char const *str);

#ifdef __cplusplus
}
#endif

#endif // MBX_CORE_H
