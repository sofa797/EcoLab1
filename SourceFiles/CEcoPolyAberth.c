/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   CEcoPolyAberth_659EAEED
 * </summary>
 *
 * <description>
 *   This source code describes the implementation of the interfaces for CEcoPolyAberth_659EAEED
 *   Task 43: roots of a polynomial. Algorithm: Aberth-Ehrlich method.
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
#include "CEcoPolyAberth.h"
#include "EcoPolyCommon.h"

/* ===================================================================== */
/*  Algorithm core: Aberth-Ehrlich method                                              */
/* ===================================================================== */

/*
 * Aberth-Ehrlich method (cubic convergence for simple roots).
 *   w_k = p(z_k) / p'(z_k)
 *   z_k <- z_k - w_k / (1 - w_k * sum_{j != k} 1 / (z_k - z_j))
 */
static int16_t EcoPolyAberth_Core(const long double* c, uint32_t n, EcoComplexL* z) {
    EcoComplexL p, dp, w, s, den, delta, diff;
    long double maxstep, step;
    long double best;
    uint32_t j, k, iter, stall;
    int16_t bad;

    EcoPoly_InitialGuess(c, n, z);
    w = EcoPoly_Make(0.0L, 0.0L);
    den = EcoPoly_Make(1.0L, 0.0L);
    maxstep = 1.0L;
    best = 1.0e30L;
    stall = 0;
    for (iter = 0; iter < ECO_POLY_MAX_ITER; iter++) {
        maxstep = 0.0L;
        for (k = 0; k < n; k++) {
            EcoPoly_HornerPD(c, n, z[k], &p, &dp);
            if (EcoPoly_Norm2(p) == 0.0L) {
                continue;                         /* exact root */
            }
            bad = 0;
            if (EcoPoly_Norm2(dp) == 0.0L) {
                bad = 1;
            }
            else {
                w = EcoPoly_Div(p, dp);
                s = EcoPoly_Make(0.0L, 0.0L);
                for (j = 0; j < n && !bad; j++) {
                    if (j != k) {
                        diff = EcoPoly_Sub(z[k], z[j]);
                        if (EcoPoly_Norm2(diff) == 0.0L) {
                            bad = 1;
                        }
                        else {
                            s = EcoPoly_Add(s, EcoPoly_Div(EcoPoly_Make(1.0L, 0.0L), diff));
                        }
                    }
                }
                if (!bad) {
                    den = EcoPoly_Sub(EcoPoly_Make(1.0L, 0.0L), EcoPoly_Mul(w, s));
                    if (EcoPoly_Norm2(den) == 0.0L) {
                        bad = 1;
                    }
                }
            }
            if (bad) {
                EcoPoly_Perturb(&z[k]);
                maxstep = 1.0L;
                continue;
            }
            delta = EcoPoly_Div(w, den);
            z[k] = EcoPoly_Sub(z[k], delta);
            step = EcoPoly_Norm2(delta) / (1.0L + EcoPoly_Norm2(z[k]));
            if (step > maxstep) {
                maxstep = step;
            }
        }
        if (maxstep <= ECO_POLY_TOL2) {
            return ECO_POLY_OK;
        }
        /* stagnation: the step stopped decreasing (rounding-noise level, multiple roots) -
         * do not waste the remaining iterations */
        if (maxstep < 0.5L * best) {
            best = maxstep;
            stall = 0;
        }
        else if (++stall >= 30 && maxstep <= ECO_POLY_TOL2_LOW) {
            return ECO_POLY_WARN_LOWACC;
        }
    }
    return (maxstep <= ECO_POLY_TOL2_LOW) ? ECO_POLY_WARN_LOWACC : ECO_POLY_ERR_NOCONV;
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
static int16_t ECOCALLMETHOD CEcoPolyAberth_659EAEED_QueryInterface(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoPolyAberth_659EAEED* pCMe = (CEcoPolyAberth_659EAEED*)me;

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
static uint32_t ECOCALLMETHOD CEcoPolyAberth_659EAEED_AddRef(/* in */ IEcoPolynomialMathPtr_t me) {
    CEcoPolyAberth_659EAEED* pCMe = (CEcoPolyAberth_659EAEED*)me;

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
static uint32_t ECOCALLMETHOD CEcoPolyAberth_659EAEED_Release(/* in */ IEcoPolynomialMathPtr_t me) {
    CEcoPolyAberth_659EAEED* pCMe = (CEcoPolyAberth_659EAEED*)me;

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
 *   of the algorithm (EcoPolyAberth_Core).
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CEcoPolyAberth_659EAEED_roots(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const double* coeffs, /* in */ uint32_t num_coeffs, /* out */ EcoComplexD* out_roots, /* out */ uint32_t* out_count) {
    if (me == 0) {
        return ECO_POLY_ERR_ARG;
    }
    return EcoPoly_RootsD(EcoPolyAberth_Core, coeffs, num_coeffs, out_roots, out_count);
}

static int16_t ECOCALLMETHOD CEcoPolyAberth_659EAEED_rootsf(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const float* coeffs, /* in */ uint32_t num_coeffs, /* out */ EcoComplexF* out_roots, /* out */ uint32_t* out_count) {
    if (me == 0) {
        return ECO_POLY_ERR_ARG;
    }
    return EcoPoly_RootsF(EcoPolyAberth_Core, coeffs, num_coeffs, out_roots, out_count);
}

static int16_t ECOCALLMETHOD CEcoPolyAberth_659EAEED_rootsl(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const long double* coeffs, /* in */ uint32_t num_coeffs, /* out */ EcoComplexL* out_roots, /* out */ uint32_t* out_count) {
    if (me == 0) {
        return ECO_POLY_ERR_ARG;
    }
    return EcoPoly_RootsL(EcoPolyAberth_Core, coeffs, num_coeffs, out_roots, out_count);
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
static int16_t ECOCALLMETHOD initCEcoPolyAberth_659EAEED(/*in*/ CEcoPolyAberth_659EAEEDPtr_t me, /* in */ IEcoUnknownPtr_t pIUnkSystem) {
    CEcoPolyAberth_659EAEED* pCMe = (CEcoPolyAberth_659EAEED*)me;
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
static int16_t ECOCALLMETHOD createCEcoPolyAberth_659EAEED(/* in */ CEcoPolyAberth_659EAEEDPtr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter) {
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
static void ECOCALLMETHOD deleteCEcoPolyAberth_659EAEED(/* in */ CEcoPolyAberth_659EAEEDPtr_t pCMe) {
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
IEcoPolynomialMathVTbl g_xFDF2ECDE99324C0AAEDF2125217B6AE3VTbl_659EAEED = {
    CEcoPolyAberth_659EAEED_QueryInterface,
    CEcoPolyAberth_659EAEED_AddRef,
    CEcoPolyAberth_659EAEED_Release,
    CEcoPolyAberth_659EAEED_roots,
    CEcoPolyAberth_659EAEED_rootsf,
    CEcoPolyAberth_659EAEED_rootsl
};

/* Object Instance */
CEcoPolyAberth_659EAEED g_xCEcoPolyAberth_659EAEED = {
    &g_xFDF2ECDE99324C0AAEDF2125217B6AE3VTbl_659EAEED,

    initCEcoPolyAberth_659EAEED,
    createCEcoPolyAberth_659EAEED,
    deleteCEcoPolyAberth_659EAEED,
    1, /* m_cRef */
    0, /* m_pIMem */
    0  /* m_pISys */
};
