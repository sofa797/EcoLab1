/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   CEcoLab1_18D23C4C
 * </summary>
 *
 * <description>
 *   This source code describes the implementation of the interfaces for CEcoLab1_18D23C4C
 *   Task 43: roots of a polynomial. Algorithm: Durand-Kerner method.
 *   Lab 2: the component can be AGGREGATED by an outer component (non-delegating IEcoUnknown) and
 *   itself uses calculator components (IEcoCalculatorX / IEcoCalculatorY) by INCLUSION and AGGREGATION.
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
#include "CEcoLab1.h"
#include "EcoPolyCommon.h"
#include "IdEcoCalculatorA.h"
#include "IdEcoCalculatorB.h"
#include "IdEcoCalculatorC.h"
#include "IdEcoCalculatorD.h"
#include "IdEcoCalculatorE.h"

/* ===================================================================== */
/*  Algorithm core: Durand-Kerner method                                              */
/* ===================================================================== */

/*
 * Durand-Kerner (Weierstrass) method.
 *   z_k <- z_k - p(z_k) / prod_{j != k} (z_k - z_j),  p - monic polynomial.
 * All roots are refined simultaneously; the updated values are used at once (Gauss-Seidel).
 */
static int16_t EcoPolyDK_Core(const long double* c, uint32_t n, EcoComplexL* z) {
    EcoComplexL p, d, delta, diff;
    long double maxstep, step;
    long double best;
    uint32_t j, k, iter, stall;

    EcoPoly_InitialGuess(c, n, z);
    maxstep = 1.0L;
    best = 1.0e30L;
    stall = 0;
    for (iter = 0; iter < ECO_POLY_MAX_ITER; iter++) {
        maxstep = 0.0L;
        for (k = 0; k < n; k++) {
            p = EcoPoly_Horner(c, n, z[k]);
            d = EcoPoly_Make(1.0L, 0.0L);
            for (j = 0; j < n; j++) {
                if (j != k) {
                    diff = EcoPoly_Sub(z[k], z[j]);
                    d = EcoPoly_Mul(d, diff);
                }
            }
            if (EcoPoly_Norm2(d) == 0.0L) {
                /* two approximations coincide - shift slightly */
                EcoPoly_Perturb(&z[k]);
                maxstep = 1.0L;
                continue;
            }
            delta = EcoPoly_Div(p, d);
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



/* ===================================================================== */
/*  Locating the object from an interface pointer                         */
/* ===================================================================== */
/* The function tables are the first members of the object (see CEcoLab1.h), the pointers are of one size */
#define ECOLAB1_FROM_INONDELEGATING(me) ((CEcoLab1_18D23C4C*)((char*)(me) - 1 * sizeof(void*)))
#define ECOLAB1_FROM_IX(me)             ((CEcoLab1_18D23C4C*)((char*)(me) - 2 * sizeof(void*)))
#define ECOLAB1_FROM_IY(me)             ((CEcoLab1_18D23C4C*)((char*)(me) - 3 * sizeof(void*)))
#define ECOLAB1_FROM_ISCHEME(me)        ((CEcoLab1_18D23C4C*)((char*)(me) - 4 * sizeof(void*)))

/* AddRef of the CONTROLLING unknown: the reference is counted by the outer component when aggregated */
#define ECOLAB1_ADDREF_OUTER(pCMe)  ((pCMe)->m_pIUnkOuter->pVTbl->AddRef((pCMe)->m_pIUnkOuter))

/* ===================================================================== */
/*  Connecting the calculator components (inclusion / aggregation)       */
/* ===================================================================== */

/* Release everything that was connected (inclusion pointers and the aggregated component) */
static void EcoLab1_Disconnect(CEcoLab1_18D23C4C* pCMe) {
    if (pCMe->m_pIncX != 0) {
        pCMe->m_pIncX->pVTbl->Release(pCMe->m_pIncX);
        pCMe->m_pIncX = 0;
    }
    if (pCMe->m_pIncY != 0) {
        pCMe->m_pIncY->pVTbl->Release(pCMe->m_pIncY);
        pCMe->m_pIncY = 0;
    }
    if (pCMe->m_pAggUnk != 0) {
        /* non-delegating Release of the aggregated component: this is the reference we own */
        pCMe->m_pAggUnk->pVTbl->Release(pCMe->m_pAggUnk);
        pCMe->m_pAggUnk = 0;
    }
    pCMe->m_Scheme = 0;
}

/* Connect the calculators of the given scheme (see IEcoLab1Scheme.h) */
static int16_t EcoLab1_Connect(CEcoLab1_18D23C4C* pCMe, int16_t scheme) {
    IEcoInterfaceBus1* pIBus = 0;
    int16_t result = ERR_ECO_POINTER;

    if (pCMe->m_pISys == 0) {
        return result;
    }
    result = pCMe->m_pISys->pVTbl->QueryInterface(pCMe->m_pISys, &IID_IEcoInterfaceBus1, (void**)&pIBus);
    if (result != 0 || pIBus == 0) {
        return ERR_ECO_NOBUS;
    }

    switch (scheme) {
    case ECOLAB1_SCHEME_FIG1_A_D:
        /* Fig.1, inclusion: X <- A, Y <- D (D includes A itself) */
        result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorA, 0, &IID_IEcoCalculatorX, (void**)&pCMe->m_pIncX);
        if (result == 0) {
            result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorD, 0, &IID_IEcoCalculatorY, (void**)&pCMe->m_pIncY);
        }
        break;
    case ECOLAB1_SCHEME_FIG1_B_E:
        /* Fig.1, inclusion: X <- B (included, not aggregated), Y <- E (E aggregates its own B) */
        result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorB, 0, &IID_IEcoCalculatorX, (void**)&pCMe->m_pIncX);
        if (result == 0) {
            result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorE, 0, &IID_IEcoCalculatorY, (void**)&pCMe->m_pIncY);
        }
        break;
    case ECOLAB1_SCHEME_FIG2_B_E:
    case ECOLAB1_SCHEME_FIG2_B_D:
        /* Fig.2, AGGREGATION of B: the outer unknown is passed, IID_IEcoUnknown is requested */
        result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorB, pCMe->m_pIUnkOuter, &IID_IEcoUnknown, (void**)&pCMe->m_pAggUnk);
        if (result == 0) {
            /* ... and inclusion of E or D for IEcoCalculatorY */
            result = pIBus->pVTbl->QueryComponent(pIBus,
                        (scheme == ECOLAB1_SCHEME_FIG2_B_E) ? &CID_EcoCalculatorE : &CID_EcoCalculatorD,
                        0, &IID_IEcoCalculatorY, (void**)&pCMe->m_pIncY);
        }
        break;
    case ECOLAB1_SCHEME_C:
        /* inclusion of C: both interfaces of one component (two references to one object) */
        result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorC, 0, &IID_IEcoCalculatorX, (void**)&pCMe->m_pIncX);
        if (result == 0) {
            result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorC, 0, &IID_IEcoCalculatorY, (void**)&pCMe->m_pIncY);
        }
        break;
    default:
        result = ERR_ECO_POINTER;
        break;
    }

    pIBus->pVTbl->Release(pIBus);

    if (result != 0) {
        /* partially connected - undo (the calculators are optional for the component) */
        EcoLab1_Disconnect(pCMe);
        return result;
    }
    pCMe->m_Scheme = scheme;
    return ERR_ECO_SUCCESS;
}

/* ===================================================================== */
/*  Non-delegating IEcoUnknown (the real implementation of QI / counting) */
/* ===================================================================== */

/*
 * <summary>
 *   NondelegatingQueryInterface Function
 * </summary>
 *
 * <description>
 *   The real QueryInterface of the component. All delegating QueryInterface functions of all the
 *   interfaces end up here (directly or through the outer component).
 * </description>
 */
static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_NondelegatingQueryInterface(/* in */ IEcoUnknownPtr_t me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoLab1_18D23C4C* pCMe = 0;

    /* Pointer Validation */
    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    pCMe = ECOLAB1_FROM_INONDELEGATING(me);

    if ( IsEqualUGUID(riid, &IID_IEcoPolynomialMath) ) {
        *ppv = &pCMe->m_pVTblIEcoPolynomialMath;
        ECOLAB1_ADDREF_OUTER(pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        /* the identity of the component: always the non-delegating IEcoUnknown */
        *ppv = &pCMe->m_pVTblINondelegatingUnk;
        ++pCMe->m_cRef;
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoLab1Scheme) ) {
        *ppv = &pCMe->m_pVTblIScheme;
        ECOLAB1_ADDREF_OUTER(pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoCalculatorX) ) {
        if (pCMe->m_pAggUnk != 0) {
            /* AGGREGATION: the pointer of the inner component goes directly to the client */
            return pCMe->m_pAggUnk->pVTbl->QueryInterface(pCMe->m_pAggUnk, riid, ppv);
        }
        if (pCMe->m_pIncX == 0) {
            *ppv = 0;
            return ERR_ECO_NOINTERFACE;
        }
        /* INCLUSION: our own interface that forwards the calls to the included component */
        *ppv = &pCMe->m_pVTblIX;
        ECOLAB1_ADDREF_OUTER(pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoCalculatorY) ) {
        if (pCMe->m_pIncY == 0) {
            *ppv = 0;
            return ERR_ECO_NOINTERFACE;
        }
        *ppv = &pCMe->m_pVTblIY;
        ECOLAB1_ADDREF_OUTER(pCMe);
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }
    return ERR_ECO_SUCCESS;
}

static uint32_t ECOCALLMETHOD CEcoLab1_18D23C4C_NondelegatingAddRef(/* in */ IEcoUnknownPtr_t me) {
    CEcoLab1_18D23C4C* pCMe = 0;

    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    pCMe = ECOLAB1_FROM_INONDELEGATING(me);
    return ++pCMe->m_cRef;
}

static uint32_t ECOCALLMETHOD CEcoLab1_18D23C4C_NondelegatingRelease(/* in */ IEcoUnknownPtr_t me) {
    CEcoLab1_18D23C4C* pCMe = 0;

    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    pCMe = ECOLAB1_FROM_INONDELEGATING(me);

    --pCMe->m_cRef;
    if ( pCMe->m_cRef == 0 ) {
        pCMe->Delete(pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

/* ===================================================================== */
/*  IEcoPolynomialMath (delegating IEcoUnknown)                          */
/* ===================================================================== */

static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_QueryInterface(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoLab1_18D23C4C* pCMe = (CEcoLab1_18D23C4C*)me;

    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    return pCMe->m_pIUnkOuter->pVTbl->QueryInterface(pCMe->m_pIUnkOuter, riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoLab1_18D23C4C_AddRef(/* in */ IEcoPolynomialMathPtr_t me) {
    CEcoLab1_18D23C4C* pCMe = (CEcoLab1_18D23C4C*)me;

    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }
    return pCMe->m_pIUnkOuter->pVTbl->AddRef(pCMe->m_pIUnkOuter);
}

static uint32_t ECOCALLMETHOD CEcoLab1_18D23C4C_Release(/* in */ IEcoPolynomialMathPtr_t me) {
    CEcoLab1_18D23C4C* pCMe = (CEcoLab1_18D23C4C*)me;

    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }
    return pCMe->m_pIUnkOuter->pVTbl->Release(pCMe->m_pIUnkOuter);
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
 *   of the algorithm (EcoPolyDK_Core).
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_roots(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const double* coeffs, /* in */ uint32_t num_coeffs, /* out */ EcoComplexD* out_roots, /* out */ uint32_t* out_count) {
    if (me == 0) {
        return ECO_POLY_ERR_ARG;
    }
    return EcoPoly_RootsD(EcoPolyDK_Core, coeffs, num_coeffs, out_roots, out_count);
}

static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_rootsf(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const float* coeffs, /* in */ uint32_t num_coeffs, /* out */ EcoComplexF* out_roots, /* out */ uint32_t* out_count) {
    if (me == 0) {
        return ECO_POLY_ERR_ARG;
    }
    return EcoPoly_RootsF(EcoPolyDK_Core, coeffs, num_coeffs, out_roots, out_count);
}

static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_rootsl(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const long double* coeffs, /* in */ uint32_t num_coeffs, /* out */ EcoComplexL* out_roots, /* out */ uint32_t* out_count) {
    if (me == 0) {
        return ECO_POLY_ERR_ARG;
    }
    return EcoPoly_RootsL(EcoPolyDK_Core, coeffs, num_coeffs, out_roots, out_count);
}

/* ===================================================================== */
/*  IEcoCalculatorX - INCLUSION: the calls are forwarded to the included  */
/*  component                                                              */
/* ===================================================================== */

static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_IX_QueryInterface(/* in */ IEcoCalculatorXPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv) {
    CEcoLab1_18D23C4C* pCMe = 0;

    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    pCMe = ECOLAB1_FROM_IX(me);
    return pCMe->m_pIUnkOuter->pVTbl->QueryInterface(pCMe->m_pIUnkOuter, riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoLab1_18D23C4C_IX_AddRef(/* in */ IEcoCalculatorXPtr_t me) {
    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    return ECOLAB1_FROM_IX(me)->m_pIUnkOuter->pVTbl->AddRef(ECOLAB1_FROM_IX(me)->m_pIUnkOuter);
}

static uint32_t ECOCALLMETHOD CEcoLab1_18D23C4C_IX_Release(/* in */ IEcoCalculatorXPtr_t me) {
    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    return ECOLAB1_FROM_IX(me)->m_pIUnkOuter->pVTbl->Release(ECOLAB1_FROM_IX(me)->m_pIUnkOuter);
}

static int32_t ECOCALLMETHOD CEcoLab1_18D23C4C_Addition(/* in */ IEcoCalculatorXPtr_t me, /* in */ int16_t a, /* in */ int16_t b) {
    CEcoLab1_18D23C4C* pCMe = 0;

    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    pCMe = ECOLAB1_FROM_IX(me);
    if (pCMe->m_pIncX == 0) {
        return 0;
    }
    return pCMe->m_pIncX->pVTbl->Addition(pCMe->m_pIncX, a, b);
}

static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_Subtraction(/* in */ IEcoCalculatorXPtr_t me, /* in */ int16_t a, /* in */ int16_t b) {
    CEcoLab1_18D23C4C* pCMe = 0;

    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    pCMe = ECOLAB1_FROM_IX(me);
    if (pCMe->m_pIncX == 0) {
        return 0;
    }
    return pCMe->m_pIncX->pVTbl->Subtraction(pCMe->m_pIncX, a, b);
}

/* ===================================================================== */
/*  IEcoCalculatorY - INCLUSION                                          */
/* ===================================================================== */

static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_IY_QueryInterface(/* in */ IEcoCalculatorYPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv) {
    CEcoLab1_18D23C4C* pCMe = 0;

    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    pCMe = ECOLAB1_FROM_IY(me);
    return pCMe->m_pIUnkOuter->pVTbl->QueryInterface(pCMe->m_pIUnkOuter, riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoLab1_18D23C4C_IY_AddRef(/* in */ IEcoCalculatorYPtr_t me) {
    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    return ECOLAB1_FROM_IY(me)->m_pIUnkOuter->pVTbl->AddRef(ECOLAB1_FROM_IY(me)->m_pIUnkOuter);
}

static uint32_t ECOCALLMETHOD CEcoLab1_18D23C4C_IY_Release(/* in */ IEcoCalculatorYPtr_t me) {
    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    return ECOLAB1_FROM_IY(me)->m_pIUnkOuter->pVTbl->Release(ECOLAB1_FROM_IY(me)->m_pIUnkOuter);
}

static int32_t ECOCALLMETHOD CEcoLab1_18D23C4C_Multiplication(/* in */ IEcoCalculatorYPtr_t me, /* in */ int16_t a, /* in */ int16_t b) {
    CEcoLab1_18D23C4C* pCMe = 0;

    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    pCMe = ECOLAB1_FROM_IY(me);
    if (pCMe->m_pIncY == 0) {
        return 0;
    }
    return pCMe->m_pIncY->pVTbl->Multiplication(pCMe->m_pIncY, a, b);
}

static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_Division(/* in */ IEcoCalculatorYPtr_t me, /* in */ int16_t a, /* in */ int16_t b) {
    CEcoLab1_18D23C4C* pCMe = 0;

    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    pCMe = ECOLAB1_FROM_IY(me);
    if (pCMe->m_pIncY == 0 || b == 0) {
        return 0;
    }
    return pCMe->m_pIncY->pVTbl->Division(pCMe->m_pIncY, a, b);
}

/* ===================================================================== */
/*  IEcoLab1Scheme                                                       */
/* ===================================================================== */

static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_IScheme_QueryInterface(/* in */ IEcoLab1SchemePtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv) {
    CEcoLab1_18D23C4C* pCMe = 0;

    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    pCMe = ECOLAB1_FROM_ISCHEME(me);
    return pCMe->m_pIUnkOuter->pVTbl->QueryInterface(pCMe->m_pIUnkOuter, riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoLab1_18D23C4C_IScheme_AddRef(/* in */ IEcoLab1SchemePtr_t me) {
    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    return ECOLAB1_FROM_ISCHEME(me)->m_pIUnkOuter->pVTbl->AddRef(ECOLAB1_FROM_ISCHEME(me)->m_pIUnkOuter);
}

static uint32_t ECOCALLMETHOD CEcoLab1_18D23C4C_IScheme_Release(/* in */ IEcoLab1SchemePtr_t me) {
    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    return ECOLAB1_FROM_ISCHEME(me)->m_pIUnkOuter->pVTbl->Release(ECOLAB1_FROM_ISCHEME(me)->m_pIUnkOuter);
}

/*
 * SelectScheme: disconnects the current calculators and connects the ones of the new scheme.
 * Call it BEFORE the client takes IEcoCalculatorX / IEcoCalculatorY pointers: pointers of an
 * aggregated component taken earlier would stay without their owner.
 */
static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_SelectScheme(/* in */ IEcoLab1SchemePtr_t me, /* in */ int16_t scheme) {
    CEcoLab1_18D23C4C* pCMe = 0;

    if (me == 0) {
        return ERR_ECO_POINTER;
    }
    pCMe = ECOLAB1_FROM_ISCHEME(me);
    EcoLab1_Disconnect(pCMe);
    return EcoLab1_Connect(pCMe, scheme);
}

static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_get_Scheme(/* in */ IEcoLab1SchemePtr_t me) {
    if (me == 0) {
        return -1;
    }
    return ECOLAB1_FROM_ISCHEME(me)->m_Scheme;
}

/*
 *
 * <summary>
 *   Init Function
 * </summary>
 *
 * <description>
 *   Instance initialization function. Connects the calculators of the default scheme
 *   (the calculators are optional: if they are not registered on the bus the component still
 *   works as a polynomial solver).
 * </description>
 *
 */
static int16_t ECOCALLMETHOD initCEcoLab1_18D23C4C(/*in*/ CEcoLab1_18D23C4CPtr_t me, /* in */ IEcoUnknownPtr_t pIUnkSystem) {
    CEcoLab1_18D23C4C* pCMe = (CEcoLab1_18D23C4C*)me;
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

    /* Lab 2: connecting the calculators of the default scheme (an error is not fatal) */
    if (result == 0) {
        EcoLab1_Connect(pCMe, ECOLAB1_SCHEME_DEFAULT);
    }

    return result;
}

/*
 *
 * <summary>
 *   Create Function
 * </summary>
 *
 * <description>
 *   Instance creation function. Stores the controlling (delegating) IEcoUnknown: the outer
 *   component if the component is aggregated, otherwise its own non-delegating IEcoUnknown.
 * </description>
 *
 */
static int16_t ECOCALLMETHOD createCEcoLab1_18D23C4C(/* in */ CEcoLab1_18D23C4CPtr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter) {
    int16_t result = ERR_ECO_POINTER;

    /* Pointer Validation */
    if (pCMe == 0) {
        return result; /* ERR_ECO_POINTER */
    }

    if (pIUnkOuter != 0) {
        /* aggregation: the reference counting and QueryInterface are delegated to the outer component */
        pCMe->m_pIUnkOuter = pIUnkOuter;
    }
    else {
        /* not aggregated: the non-delegating IEcoUnknown is used */
        pCMe->m_pIUnkOuter = (IEcoUnknown*)&pCMe->m_pVTblINondelegatingUnk;
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
static void ECOCALLMETHOD deleteCEcoLab1_18D23C4C(/* in */ CEcoLab1_18D23C4CPtr_t pCMe) {
    IEcoMemoryAllocator1* pIMem = 0;

    if (pCMe != 0 ) {
        pIMem = pCMe->m_pIMem;
        /* Freeing the included / aggregated calculators */
        EcoLab1_Disconnect(pCMe);
        /* Freeing */
        if ( pCMe->m_pISys != 0 ) {
            pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
        }
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
    }
}

/* IEcoPolynomialMath Virtual Table */
IEcoPolynomialMathVTbl g_xFDF2ECDE99324C0AAEDF2125217B6AE3VTbl_18D23C4C = {
    CEcoLab1_18D23C4C_QueryInterface,
    CEcoLab1_18D23C4C_AddRef,
    CEcoLab1_18D23C4C_Release,
    CEcoLab1_18D23C4C_roots,
    CEcoLab1_18D23C4C_rootsf,
    CEcoLab1_18D23C4C_rootsl
};

/* Non-delegating IEcoUnknown Virtual Table */
IEcoUnknownVTbl g_x000000000000000000000000000000AAVTbl_18D23C4C = {
    CEcoLab1_18D23C4C_NondelegatingQueryInterface,
    CEcoLab1_18D23C4C_NondelegatingAddRef,
    CEcoLab1_18D23C4C_NondelegatingRelease
};

/* IEcoCalculatorX Virtual Table (inclusion) */
IEcoCalculatorXVTbl g_x9322111622484742AE0682819447843DVTbl_18D23C4C = {
    CEcoLab1_18D23C4C_IX_QueryInterface,
    CEcoLab1_18D23C4C_IX_AddRef,
    CEcoLab1_18D23C4C_IX_Release,
    CEcoLab1_18D23C4C_Addition,
    CEcoLab1_18D23C4C_Subtraction
};

/* IEcoCalculatorY Virtual Table (inclusion) */
IEcoCalculatorYVTbl g_xBD6414C29096423EA90C04D77AFD1CADVTbl_18D23C4C = {
    CEcoLab1_18D23C4C_IY_QueryInterface,
    CEcoLab1_18D23C4C_IY_AddRef,
    CEcoLab1_18D23C4C_IY_Release,
    CEcoLab1_18D23C4C_Multiplication,
    CEcoLab1_18D23C4C_Division
};

/* IEcoLab1Scheme Virtual Table */
IEcoLab1SchemeVTbl g_x6E1B9D304F274C85A3D90B72E5C8A1F4VTbl_18D23C4C = {
    CEcoLab1_18D23C4C_IScheme_QueryInterface,
    CEcoLab1_18D23C4C_IScheme_AddRef,
    CEcoLab1_18D23C4C_IScheme_Release,
    CEcoLab1_18D23C4C_SelectScheme,
    CEcoLab1_18D23C4C_get_Scheme
};

/* Object Instance (prototype that the factory copies) */
CEcoLab1_18D23C4C g_xCEcoLab1_18D23C4C = {
    &g_xFDF2ECDE99324C0AAEDF2125217B6AE3VTbl_18D23C4C,
    &g_x000000000000000000000000000000AAVTbl_18D23C4C,
    &g_x9322111622484742AE0682819447843DVTbl_18D23C4C,
    &g_xBD6414C29096423EA90C04D77AFD1CADVTbl_18D23C4C,
    &g_x6E1B9D304F274C85A3D90B72E5C8A1F4VTbl_18D23C4C,

    initCEcoLab1_18D23C4C,
    createCEcoLab1_18D23C4C,
    deleteCEcoLab1_18D23C4C,
    1, /* m_cRef */
    0, /* m_pIMem */
    0, /* m_pISys */
    0, /* m_pIUnkOuter */
    0, /* m_Scheme */
    0, /* m_pIncX */
    0, /* m_pIncY */
    0  /* m_pAggUnk */
};
