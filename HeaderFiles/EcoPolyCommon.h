/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   EcoPolyCommon
 * </summary>
 *
 * <description>
 *   Private helpers shared by the three "roots of a polynomial" components:
 *   complex arithmetic, Horner scheme, real/complex square root, pre-processing
 *   of the coefficient vector (leading/trailing zeros, normalisation), post-processing
 *   (cleaning of noise, sorting) and the float/double/long double wrappers.
 *   NO libc functions are used (no math.h, string.h, stdlib.h).
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */

#ifndef __ECO_POLY_COMMON_H__
#define __ECO_POLY_COMMON_H__

#include "IEcoPolynomialMath.h"

#define ECO_POLY_MAX_ITER     2000
#define ECO_POLY_TOL2         1.0e-30L   /* (1e-15)^2 : threshold for the squared relative step      */
#define ECO_POLY_TOL2_LOW     1.0e-8L    /* (1e-4)^2  : "reduced accuracy" threshold (multiple roots) */
#define ECO_POLY_IM_SNAP      1.0e-12L   /* imaginary part treated as rounding noise                  */
#define ECO_POLY_SORT_EQ      1.0e-9L    /* "equal" real parts when sorting                           */

/*
 * Core of an algorithm: finds all roots of the MONIC polynomial
 *   c[0]*x^n + ... + c[n],  c[0] == 1, n >= 2
 * and writes them to z[0..n-1].  Returns ECO_POLY_OK, ECO_POLY_WARN_LOWACC or ECO_POLY_ERR_NOCONV.
 */
typedef int16_t (*EcoPolyCoreFn)(const long double* c, uint32_t n, EcoComplexL* z);

/* ---------------- complex arithmetic ---------------- */

static inline long double EcoPoly_Abs(long double x) {
    return (x < 0.0L) ? -x : x;
}

static inline EcoComplexL EcoPoly_Make(long double re, long double im) {
    EcoComplexL r;
    r.re = re;
    r.im = im;
    return r;
}

static inline EcoComplexL EcoPoly_Add(EcoComplexL a, EcoComplexL b) {
    return EcoPoly_Make(a.re + b.re, a.im + b.im);
}

static inline EcoComplexL EcoPoly_Sub(EcoComplexL a, EcoComplexL b) {
    return EcoPoly_Make(a.re - b.re, a.im - b.im);
}

static inline EcoComplexL EcoPoly_Mul(EcoComplexL a, EcoComplexL b) {
    return EcoPoly_Make(a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re);
}

static inline EcoComplexL EcoPoly_MulR(EcoComplexL a, long double r) {
    return EcoPoly_Make(a.re * r, a.im * r);
}

static inline long double EcoPoly_Norm2(EcoComplexL a) {
    return a.re * a.re + a.im * a.im;
}

/* "L1 norm" |re| + |im|: cheap magnitude bound without square root */
static inline long double EcoPoly_Norm1(EcoComplexL a) {
    return EcoPoly_Abs(a.re) + EcoPoly_Abs(a.im);
}

/* a / b ; call only when Norm2(b) != 0 */
static inline EcoComplexL EcoPoly_Div(EcoComplexL a, EcoComplexL b) {
    long double d = EcoPoly_Norm2(b);
    return EcoPoly_Make((a.re * b.re + a.im * b.im) / d,
                        (a.im * b.re - a.re * b.im) / d);
}

/* Square root of a real number >= 0 (scaling by 4^k + Newton iterations) */
static inline long double EcoPoly_Sqrt(long double x) {
    long double y;
    int32_t k = 0;
    uint32_t i;
    if (!(x > 0.0L)) {
        return 0.0L;
    }
    for (i = 0; i < 4000 && x > 4.0L; i++) {
        x *= 0.25L;
        k++;
    }
    for (i = 0; i < 4000 && x < 1.0L; i++) {
        x *= 4.0L;
        k--;
    }
    y = 0.5L * (1.0L + x);
    for (i = 0; i < 8; i++) {
        y = 0.5L * (y + x / y);
    }
    for (i = 0; i < 4000 && k > 0; i++) {
        y *= 2.0L;
        k--;
    }
    for (i = 0; i < 4000 && k < 0; i++) {
        y *= 0.5L;
        k++;
    }
    return y;
}

/* Principal square root of a complex number */
static inline EcoComplexL EcoPoly_CSqrt(EcoComplexL z) {
    long double m, t;
    if (z.re == 0.0L && z.im == 0.0L) {
        return EcoPoly_Make(0.0L, 0.0L);
    }
    m = EcoPoly_Sqrt(EcoPoly_Norm2(z));                 /* |z| */
    if (z.re >= 0.0L) {
        t = EcoPoly_Sqrt(0.5L * (m + z.re));
        return EcoPoly_Make(t, z.im / (2.0L * t));
    }
    t = EcoPoly_Sqrt(0.5L * (m - z.re));
    return EcoPoly_Make(EcoPoly_Abs(z.im) / (2.0L * t), (z.im < 0.0L) ? -t : t);
}

/* Horner scheme for a polynomial with real coefficients c[0..n] */
static inline EcoComplexL EcoPoly_Horner(const long double* c, uint32_t n, EcoComplexL z) {
    EcoComplexL p;
    uint32_t i;
    p = EcoPoly_Make(c[0], 0.0L);
    for (i = 1; i <= n; i++) {
        p = EcoPoly_Mul(p, z);
        p.re += c[i];
    }
    return p;
}

/* Value and first derivative (real coefficients) */
static inline void EcoPoly_HornerPD(const long double* c, uint32_t n, EcoComplexL z,
                                    EcoComplexL* p, EcoComplexL* dp) {
    EcoComplexL pv, dv;
    uint32_t i;
    pv = EcoPoly_Make(c[0], 0.0L);
    dv = EcoPoly_Make(0.0L, 0.0L);
    for (i = 1; i <= n; i++) {
        dv = EcoPoly_Add(EcoPoly_Mul(dv, z), pv);
        pv = EcoPoly_Mul(pv, z);
        pv.re += c[i];
    }
    *p = pv;
    *dp = dv;
}

/* Initial approximations: r * (0.4 + 0.9i)^k, r - Cauchy bound (no trigonometry needed) */
static inline void EcoPoly_InitialGuess(const long double* c, uint32_t n, EcoComplexL* z) {
    long double bound, v;
    EcoComplexL w;
    uint32_t i;
    bound = 0.0L;
    for (i = 1; i <= n; i++) {
        v = EcoPoly_Abs(c[i]);
        if (v > bound) {
            bound = v;
        }
    }
    bound += 1.0L;
    w = EcoPoly_Make(0.4L, 0.9L);
    z[0] = EcoPoly_Make(bound, 0.0L);
    for (i = 1; i < n; i++) {
        z[i] = EcoPoly_Mul(z[i - 1], w);
    }
}

/* Small shift used when two approximations coincide */
static inline void EcoPoly_Perturb(EcoComplexL* z) {
    z->re += 1.0e-7L * (1.0L + EcoPoly_Abs(z->re));
    z->im += 1.0e-7L;
}

/* ---------------- sorting: descending Re, then descending Im ---------------- */

static inline int16_t EcoPoly_Before(EcoComplexL a, EcoComplexL b) {
    long double tol = ECO_POLY_SORT_EQ * (1.0L + EcoPoly_Abs(a.re));
    if (EcoPoly_Abs(a.re - b.re) <= tol) {
        return (a.im > b.im) ? 1 : 0;
    }
    return (a.re > b.re) ? 1 : 0;
}

static inline void EcoPoly_Sort(EcoComplexL* r, uint32_t n) {
    uint32_t i, j;
    EcoComplexL key;
    for (i = 1; i < n; i++) {
        key = r[i];
        j = i;
        while (j > 0 && EcoPoly_Before(key, r[j - 1])) {
            r[j] = r[j - 1];
            j--;
        }
        r[j] = key;
    }
}

/* ---------------- common driver ---------------- */

static inline int16_t EcoPoly_Solve(EcoPolyCoreFn core, const long double* a, uint32_t m,
                                    EcoComplexL* out, uint32_t* count) {
    long double c[ECO_POLY_MAX_DEGREE + 1];
    EcoComplexL z[ECO_POLY_MAX_DEGREE];
    uint32_t start, end, nz, n, i, total;
    int16_t status;

    if (a == 0 || out == 0 || count == 0 || m == 0) {
        return ECO_POLY_ERR_ARG;
    }
    if (m > ECO_POLY_MAX_DEGREE + 1) {
        return ECO_POLY_ERR_DEGREE;
    }
    *count = 0;

    /* 1. Drop leading zero coefficients */
    start = 0;
    while (start < m && a[start] == 0.0L) {
        start++;
    }
    if (start == m) {
        return ECO_POLY_ERR_ZERO;           /* p(x) == 0 */
    }

    /* 2. Trailing zero coefficients = roots x = 0 */
    end = m - 1;
    nz = 0;
    while (a[end] == 0.0L) {
        end--;
        nz++;
    }
    n = end - start;                        /* degree of the non-zero part */

    /* 3. Normalisation: leading coefficient = 1 */
    for (i = 0; i <= n; i++) {
        c[i] = a[start + i] / a[start];
    }

    /* 4. Zero roots */
    total = 0;
    for (i = 0; i < nz; i++) {
        out[total++] = EcoPoly_Make(0.0L, 0.0L);
    }

    status = ECO_POLY_OK;
    if (n == 1) {
        out[total++] = EcoPoly_Make(-c[1], 0.0L);
    }
    else if (n >= 2) {
        status = core(c, n, z);
        if (status < 0) {
            return status;
        }
        /* 5. Remove rounding noise from imaginary parts (real coefficients) */
        for (i = 0; i < n; i++) {
            if (EcoPoly_Abs(z[i].im) <= ECO_POLY_IM_SNAP * (1.0L + EcoPoly_Abs(z[i].re))) {
                z[i].im = 0.0L;
            }
            out[total++] = z[i];
        }
    }

    EcoPoly_Sort(out, total);
    *count = total;
    return status;
}

/* ---------------- wrappers for float / double / long double ---------------- */

static inline int16_t EcoPoly_RootsL(EcoPolyCoreFn core, const long double* coeffs, uint32_t num,
                                     EcoComplexL* out, uint32_t* count) {
    if (coeffs == 0 || out == 0 || count == 0 || num == 0) {
        return ECO_POLY_ERR_ARG;
    }
    return EcoPoly_Solve(core, coeffs, num, out, count);
}

static inline int16_t EcoPoly_RootsD(EcoPolyCoreFn core, const double* coeffs, uint32_t num,
                                     EcoComplexD* out, uint32_t* count) {
    long double a[ECO_POLY_MAX_DEGREE + 1];
    EcoComplexL r[ECO_POLY_MAX_DEGREE];
    uint32_t i, cnt;
    int16_t res;
    if (coeffs == 0 || out == 0 || count == 0 || num == 0) {
        return ECO_POLY_ERR_ARG;
    }
    if (num > ECO_POLY_MAX_DEGREE + 1) {
        return ECO_POLY_ERR_DEGREE;
    }
    for (i = 0; i < num; i++) {
        a[i] = (long double)coeffs[i];
    }
    cnt = 0;
    res = EcoPoly_Solve(core, a, num, r, &cnt);
    if (res >= 0) {
        for (i = 0; i < cnt; i++) {
            out[i].re = (double)r[i].re;
            out[i].im = (double)r[i].im;
        }
    }
    *count = (res >= 0) ? cnt : 0;
    return res;
}

static inline int16_t EcoPoly_RootsF(EcoPolyCoreFn core, const float* coeffs, uint32_t num,
                                     EcoComplexF* out, uint32_t* count) {
    long double a[ECO_POLY_MAX_DEGREE + 1];
    EcoComplexL r[ECO_POLY_MAX_DEGREE];
    uint32_t i, cnt;
    int16_t res;
    if (coeffs == 0 || out == 0 || count == 0 || num == 0) {
        return ECO_POLY_ERR_ARG;
    }
    if (num > ECO_POLY_MAX_DEGREE + 1) {
        return ECO_POLY_ERR_DEGREE;
    }
    for (i = 0; i < num; i++) {
        a[i] = (long double)coeffs[i];
    }
    cnt = 0;
    res = EcoPoly_Solve(core, a, num, r, &cnt);
    if (res >= 0) {
        for (i = 0; i < cnt; i++) {
            out[i].re = (float)r[i].re;
            out[i].im = (float)r[i].im;
        }
    }
    *count = (res >= 0) ? cnt : 0;
    return res;
}

#endif /* __ECO_POLY_COMMON_H__ */
