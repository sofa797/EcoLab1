/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   CEcoPolyLaguerre_4D0499F1
 * </summary>
 *
 * <description>
 *   This source code describes the implementation of the interfaces for CEcoPolyLaguerre_4D0499F1
 *   Task 43: roots of a polynomial. Algorithm: Laguerre method with implicit deflation.
 *   No libc functions are used (see EcoPolyCommon.h).
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */


#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoPolyLaguerre.h"
#include "EcoPolyCommon.h"

/* ===================================================================== */
/*  Algorithm core: Laguerre method with implicit deflation                                              */
/* ===================================================================== */

/*
 * Laguerre method with IMPLICIT deflation.
 * Roots are found one by one. Instead of dividing the polynomial by (x - root) (which accumulates
 * errors), the already found roots r_j are taken into account analytically:
 *   Gp = p'/p,  S1 = sum 1/(x - r_j),  S2 = sum 1/(x - r_j)^2
 *   G  = Gp - S1,   H = Gp^2 - p''/p - S2
 *   a  = m / (G +- sqrt((m-1)(m*H - G^2))),   m = n - (number of found roots)
 *   x <- x - a        (sign chosen with the larger |denominator|)
 * p, p', p'' are always evaluated on the ORIGINAL polynomial, so no polishing step is needed.
 */

/* Fractions of a full step used every 10th iteration to break possible cycles */
static const long double g_LaguerreFrac[8] = {0.5L, 0.25L, 0.75L, 0.13L, 0.38L, 0.62L, 0.88L, 1.0L};

/* Cauchy bound: all roots lie inside |z| <= bound */
static long double EcoPolyLaguerre_Bound(const long double* c, uint32_t n) {
    long double bound, v;
    uint32_t i;
    bound = 0.0L;
    for (i = 1; i <= n; i++) {
        v = EcoPoly_Abs(c[i]);
        if (v > bound) {
            bound = v;
        }
    }
    return bound + 1.0L;
}

/* One root of the monic polynomial c[0..n] (c[0] = 1) different from the k found roots r[0..k-1] */
static int16_t EcoPolyLaguerre_Find(const long double* c, uint32_t n, const EcoComplexL* r, uint32_t k,
                                    long double bound, EcoComplexL* x) {
    EcoComplexL p, dp, d2, gp, g, h, s1, s2, t, sq, gplus, gminus, den, dx;
    long double dm, ap, ab, dn;
    uint32_t i, j, iter, m;
    int16_t bad;

    m = n - k;                                       /* degree of the "deflated" polynomial */
    g = EcoPoly_Make(0.0L, 0.0L);
    h = EcoPoly_Make(0.0L, 0.0L);
    sq = EcoPoly_Make(0.0L, 0.0L);
    for (iter = 1; iter <= 120; iter++) {
        /* p, p', p''/2 by Horner scheme; ap - bound of the rounding error of p */
        p = EcoPoly_Make(c[0], 0.0L);
        dp = EcoPoly_Make(0.0L, 0.0L);
        d2 = EcoPoly_Make(0.0L, 0.0L);
        ap = EcoPoly_Abs(c[0]);
        ab = EcoPoly_Norm1(*x);
        for (i = 1; i <= n; i++) {
            d2 = EcoPoly_Add(EcoPoly_Mul(d2, *x), dp);
            dp = EcoPoly_Add(EcoPoly_Mul(dp, *x), p);
            p = EcoPoly_Mul(p, *x);
            p.re += c[i];
            ap = ap * ab + EcoPoly_Abs(c[i]);
        }
        if (EcoPoly_Norm1(p) <= 1.0e-16L * ap) {
            return ECO_POLY_OK;                      /* p(x) is at the rounding-noise level: root found */
        }
        gp = EcoPoly_Div(dp, p);
        s1 = EcoPoly_Make(0.0L, 0.0L);
        s2 = EcoPoly_Make(0.0L, 0.0L);
        bad = 0;
        for (j = 0; j < k; j++) {
            t = EcoPoly_Sub(*x, r[j]);
            if (EcoPoly_Norm2(t) == 0.0L) {
                bad = 1;
                break;
            }
            t = EcoPoly_Div(EcoPoly_Make(1.0L, 0.0L), t);
            s1 = EcoPoly_Add(s1, t);
            s2 = EcoPoly_Add(s2, EcoPoly_Mul(t, t));
        }
        den = EcoPoly_Make(0.0L, 0.0L);
        if (!bad) {
            g = EcoPoly_Sub(gp, s1);
            h = EcoPoly_Sub(EcoPoly_Sub(EcoPoly_Mul(gp, gp), EcoPoly_MulR(EcoPoly_Div(d2, p), 2.0L)), s2);
            sq = EcoPoly_CSqrt(EcoPoly_MulR(EcoPoly_Sub(EcoPoly_MulR(h, (long double)m), EcoPoly_Mul(g, g)),
                                            (long double)(m - 1)));
            gplus = EcoPoly_Add(g, sq);
            gminus = EcoPoly_Sub(g, sq);
            den = (EcoPoly_Norm2(gplus) >= EcoPoly_Norm2(gminus)) ? gplus : gminus;
        }
        if (bad || EcoPoly_Norm2(den) == 0.0L) {
            /* degenerate point (x coincides with a found root, or G = 0): shift x */
            x->re += (1.0L + EcoPoly_Norm1(*x)) * 0.3L;
            x->im += (1.0L + EcoPoly_Norm1(*x)) * 0.2L;
            continue;
        }
        dx = EcoPoly_Div(EcoPoly_Make((long double)m, 0.0L), den);
        dn = EcoPoly_Norm2(dx);
        dm = dn / (1.0L + EcoPoly_Norm2(*x));
        if (dn > 4.0L * bound * bound) {
            /* too long step (p' is almost zero): limit it, otherwise the method can cycle */
            dx = EcoPoly_MulR(dx, 2.0L * bound / EcoPoly_Sqrt(dn));
        }
        if (iter % 10 == 0) {
            dx = EcoPoly_MulR(dx, g_LaguerreFrac[(iter / 10) % 8]);
        }
        *x = EcoPoly_Sub(*x, dx);
        if (dm <= ECO_POLY_TOL2) {
            return ECO_POLY_OK;
        }
    }
    return ECO_POLY_ERR_NOCONV;
}

static int16_t EcoPolyLaguerre_Core(const long double* c, uint32_t n, EcoComplexL* z) {
    EcoComplexL x, w;
    long double bound;
    uint32_t k, tries;
    int16_t st;

    bound = EcoPolyLaguerre_Bound(c, n);
    w = EcoPoly_Make(0.4L, 0.9L);
    for (k = 0; k < n; k++) {
        /* start point: "generic" direction at the distance of the Cauchy bound
         * (x = 0 is bad: p'(0) = 0 for many polynomials, for example x^n - 1) */
        x = EcoPoly_MulR(w, bound);
        st = EcoPolyLaguerre_Find(c, n, z, k, bound, &x);
        /* if the iteration cycles, restart from another point */
        for (tries = 1; st < 0 && tries < 8; tries++) {
            x = EcoPoly_MulR(EcoPoly_Mul(w, EcoPoly_Make(0.4L + 0.1L * (long double)tries, 0.9L)), bound);
            st = EcoPolyLaguerre_Find(c, n, z, k, bound, &x);
        }
        if (st < 0) {
            return st;
        }
        z[k] = x;
    }
    return ECO_POLY_OK;
}


/*
 *
 * <summary>
 *   QueryInterface Function
 * </summary>
 *
 * <description>
 *   QueryInterface function for the IEcoPolynomialMath interface
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CEcoPolyLaguerre_4D0499F1_QueryInterface(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoPolyLaguerre_4D0499F1* pCMe = (CEcoPolyLaguerre_4D0499F1*)me;

    /* Pointer Validation */
    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    /* Validate and retrieve requested interface */
    if ( IsEqualUGUID(riid, &IID_IEcoPolynomialMath) ) {
        *ppv = &pCMe->m_pVTblIEcoPolynomialMath;
        pCMe->m_pVTblIEcoPolynomialMath->AddRef((IEcoPolynomialMath*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblIEcoPolynomialMath;
        pCMe->m_pVTblIEcoPolynomialMath->AddRef((IEcoPolynomialMath*)pCMe);
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }
    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   AddRef Function
 * </summary>
 *
 * <description>
 *   AddRef function for the IEcoPolynomialMath interface
 * </description>
 *
 */
static uint32_t ECOCALLMETHOD CEcoPolyLaguerre_4D0499F1_AddRef(/* in */ IEcoPolynomialMathPtr_t me) {
    CEcoPolyLaguerre_4D0499F1* pCMe = (CEcoPolyLaguerre_4D0499F1*)me;

    /* Pointer Validation */
    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    return ++pCMe->m_cRef;
}

/*
 *
 * <summary>
 *   Release Function
 * </summary>
 *
 * <description>
 *   Release function for the IEcoPolynomialMath interface
 * </description>
 *
 */
static uint32_t ECOCALLMETHOD CEcoPolyLaguerre_4D0499F1_Release(/* in */ IEcoPolynomialMathPtr_t me) {
    CEcoPolyLaguerre_4D0499F1* pCMe = (CEcoPolyLaguerre_4D0499F1*)me;

    /* Pointer Validation */
    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    /* Decrementing the component's reference count */
    --pCMe->m_cRef;
    /* If the count is zero, free the instance data */
    if ( pCMe->m_cRef == 0 ) {
        pCMe->Delete(pCMe);

        return 0;
    }
    return pCMe->m_cRef;
}

/*
 *
 * <summary>
 *   roots / rootsf / rootsl Functions
 * </summary>
 *
 * <description>
 *   Roots of the polynomial c[0]*x^n + ... + c[n] (double / float / long double versions).
 *   The common driver EcoPoly_Solve (EcoPolyCommon.h) prepares the polynomial and calls the core
 *   of the algorithm (EcoPolyLaguerre_Core).
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CEcoPolyLaguerre_4D0499F1_roots(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const double* coeffs, /* in */ uint32_t num_coeffs, /* out */ EcoComplexD* out_roots, /* out */ uint32_t* out_count) {
    if (me == 0) {
        return ECO_POLY_ERR_ARG;
    }
    return EcoPoly_RootsD(EcoPolyLaguerre_Core, coeffs, num_coeffs, out_roots, out_count);
}

static int16_t ECOCALLMETHOD CEcoPolyLaguerre_4D0499F1_rootsf(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const float* coeffs, /* in */ uint32_t num_coeffs, /* out */ EcoComplexF* out_roots, /* out */ uint32_t* out_count) {
    if (me == 0) {
        return ECO_POLY_ERR_ARG;
    }
    return EcoPoly_RootsF(EcoPolyLaguerre_Core, coeffs, num_coeffs, out_roots, out_count);
}

static int16_t ECOCALLMETHOD CEcoPolyLaguerre_4D0499F1_rootsl(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const long double* coeffs, /* in */ uint32_t num_coeffs, /* out */ EcoComplexL* out_roots, /* out */ uint32_t* out_count) {
    if (me == 0) {
        return ECO_POLY_ERR_ARG;
    }
    return EcoPoly_RootsL(EcoPolyLaguerre_Core, coeffs, num_coeffs, out_roots, out_count);
}

/*
 *
 * <summary>
 *   Init Function
 * </summary>
 *
 * <description>
 *   Instance initialization function
 * </description>
 *
 */
static int16_t ECOCALLMETHOD initCEcoPolyLaguerre_4D0499F1(/*in*/ CEcoPolyLaguerre_4D0499F1Ptr_t me, /* in */ IEcoUnknownPtr_t pIUnkSystem) {
    CEcoPolyLaguerre_4D0499F1* pCMe = (CEcoPolyLaguerre_4D0499F1*)me;
    IEcoInterfaceBus1* pIBus = 0;

    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    int16_t result = ERR_ECO_POINTER;
    UGUID* rcid = (UGUID*)&CID_EcoMemoryManager1;

    /* Pointer Validation */
    if (me == 0 ) {
        return result;
    }

    /* Storing the pointer to the system interface */
    pCMe->m_pISys = (IEcoSystem1*)pIUnkSystem;

    /* Getting the interface for working with the interface bus */
    result = pCMe->m_pISys->pVTbl->QueryInterface(pCMe->m_pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);

    /* Getting the component ID for working with memory */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        rcid = (UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt);
        pIMemExt->pVTbl->Release(pIMemExt);
    }

    /* Getting the memory allocator interface */
    result = pIBus->pVTbl->QueryComponent(pIBus, rcid, 0, &IID_IEcoMemoryAllocator1, (void**) &pCMe->m_pIMem);
    /* Check */
    if (result != 0 || pCMe->m_pIMem == 0) {
        result = ERR_ECO_GET_MEMORY_ALLOCATOR;
    }

    /* Freeing */
    pIBus->pVTbl->Release(pIBus);

    return result;
}

/*
 *
 * <summary>
 *   Create Function
 * </summary>
 *
 * <description>
 *   Instance creation function
 * </description>
 *
 */
static int16_t ECOCALLMETHOD createCEcoPolyLaguerre_4D0499F1(/* in */ CEcoPolyLaguerre_4D0499F1Ptr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter) {
    int16_t result = ERR_ECO_POINTER;

    /* Pointer Validation */
    if (pCMe == 0) {
        return result; /* ERR_ECO_POINTER */
    }

    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   Delete Function
 * </summary>
 *
 * <description>
 *   Instance freeing function
 * </description>
 *
 */
static void ECOCALLMETHOD deleteCEcoPolyLaguerre_4D0499F1(/* in */ CEcoPolyLaguerre_4D0499F1Ptr_t pCMe) {
    IEcoMemoryAllocator1* pIMem = 0;

    if (pCMe != 0 ) {
        pIMem = pCMe->m_pIMem;
        /* Freeing */
        if ( pCMe->m_pISys != 0 ) {
            pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
        }
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
    }
}

/* IEcoPolynomialMath Virtual Table */
IEcoPolynomialMathVTbl g_xFDF2ECDE99324C0AAEDF2125217B6AE3VTbl_4D0499F1 = {
    CEcoPolyLaguerre_4D0499F1_QueryInterface,
    CEcoPolyLaguerre_4D0499F1_AddRef,
    CEcoPolyLaguerre_4D0499F1_Release,
    CEcoPolyLaguerre_4D0499F1_roots,
    CEcoPolyLaguerre_4D0499F1_rootsf,
    CEcoPolyLaguerre_4D0499F1_rootsl
};

/* Object Instance */
CEcoPolyLaguerre_4D0499F1 g_xCEcoPolyLaguerre_4D0499F1 = {
    &g_xFDF2ECDE99324C0AAEDF2125217B6AE3VTbl_4D0499F1,

    initCEcoPolyLaguerre_4D0499F1,
    createCEcoPolyLaguerre_4D0499F1,
    deleteCEcoPolyLaguerre_4D0499F1,
    1, /* m_cRef */
    0, /* m_pIMem */
    0  /* m_pISys */
};
