/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   CEcoLab2
 * </summary>
 *
 * <description>
 *   This source code describes the implementation of the interfaces of CEcoLab2 (Lab 2, Fig. 3).
 *
 *   Inner components:
 *     E        - INCLUSION. IEcoCalculatorY of E is wrapped by our own IEcoCalculatorY; E itself
 *                AGGREGATES B, so IEcoCalculatorX of B is obtained with E->QueryInterface and wrapped
 *                by our own IEcoCalculatorX (E and B do not know about EcoLab2, they cannot be aggregated
 *                by it: E has no non-delegating IEcoUnknown).
 *     D        - INCLUSION (D includes A itself); C - INCLUSION. They are used by CheckCalculators.
 *     Eco.Lab1 - AGGREGATION. EcoLab1 gets our controlling IEcoUnknown, and every other requested
 *                interface (IEcoPolynomialMath, IEcoLab1Scheme) is passed to it directly.
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
#include "CEcoLab2.h"
#include "IdEcoLab1.h"
#include "IdEcoCalculatorC.h"
#include "IdEcoCalculatorD.h"
#include "IdEcoCalculatorE.h"

#define ECOLAB2_FROM_INONDELEGATING(me) ((CEcoLab2*)((char*)(me) - 1 * sizeof(void*)))
#define ECOLAB2_FROM_IX(me)             ((CEcoLab2*)((char*)(me) - 2 * sizeof(void*)))
#define ECOLAB2_FROM_IY(me)             ((CEcoLab2*)((char*)(me) - 3 * sizeof(void*)))
#define ECOLAB2_ADDREF_OUTER(pCMe)      ((pCMe)->m_pIUnkOuter->pVTbl->AddRef((pCMe)->m_pIUnkOuter))

/* ===================================================================== */
/*  Non-delegating IEcoUnknown                                           */
/* ===================================================================== */

static int16_t ECOCALLMETHOD CEcoLab2_NondelegatingQueryInterface(/* in */ IEcoUnknownPtr_t me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoLab2* pCMe = 0;

    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    pCMe = ECOLAB2_FROM_INONDELEGATING(me);

    if ( IsEqualUGUID(riid, &IID_IEcoLab2) ) {
        *ppv = &pCMe->m_pVTblIEcoLab2;
        ECOLAB2_ADDREF_OUTER(pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        /* identity of the component */
        *ppv = &pCMe->m_pVTblINondelegatingUnk;
        ++pCMe->m_cRef;
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoCalculatorX) ) {
        if (pCMe->m_pIncEX == 0) {
            *ppv = 0;
            return ERR_ECO_NOINTERFACE;
        }
        *ppv = &pCMe->m_pVTblIX;
        ECOLAB2_ADDREF_OUTER(pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoCalculatorY) ) {
        if (pCMe->m_pIncEY == 0) {
            *ppv = 0;
            return ERR_ECO_NOINTERFACE;
        }
        *ppv = &pCMe->m_pVTblIY;
        ECOLAB2_ADDREF_OUTER(pCMe);
    }
    else {
        /* AGGREGATION of Eco.Lab1: everything else (IEcoPolynomialMath, IEcoLab1Scheme) is asked
         * from the aggregated component, and its pointer goes directly to the client */
        if (pCMe->m_pLab1Unk == 0) {
            *ppv = 0;
            return ERR_ECO_NOINTERFACE;
        }
        return pCMe->m_pLab1Unk->pVTbl->QueryInterface(pCMe->m_pLab1Unk, riid, ppv);
    }
    return ERR_ECO_SUCCESS;
}

static uint32_t ECOCALLMETHOD CEcoLab2_NondelegatingAddRef(/* in */ IEcoUnknownPtr_t me) {
    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    return ++ECOLAB2_FROM_INONDELEGATING(me)->m_cRef;
}

static uint32_t ECOCALLMETHOD CEcoLab2_NondelegatingRelease(/* in */ IEcoUnknownPtr_t me) {
    CEcoLab2* pCMe = 0;

    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    pCMe = ECOLAB2_FROM_INONDELEGATING(me);
    --pCMe->m_cRef;
    if ( pCMe->m_cRef == 0 ) {
        pCMe->Delete(pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

/* ===================================================================== */
/*  IEcoLab2 (delegating IEcoUnknown)                                    */
/* ===================================================================== */

static int16_t ECOCALLMETHOD CEcoLab2_QueryInterface(/* in */ IEcoLab2Ptr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv) {
    CEcoLab2* pCMe = (CEcoLab2*)me;

    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    return pCMe->m_pIUnkOuter->pVTbl->QueryInterface(pCMe->m_pIUnkOuter, riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoLab2_AddRef(/* in */ IEcoLab2Ptr_t me) {
    CEcoLab2* pCMe = (CEcoLab2*)me;

    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    return pCMe->m_pIUnkOuter->pVTbl->AddRef(pCMe->m_pIUnkOuter);
}

static uint32_t ECOCALLMETHOD CEcoLab2_Release(/* in */ IEcoLab2Ptr_t me) {
    CEcoLab2* pCMe = (CEcoLab2*)me;

    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    return pCMe->m_pIUnkOuter->pVTbl->Release(pCMe->m_pIUnkOuter);
}

#define ECOLAB2_FITS_INT16(v) ((v) >= -32768L && (v) <= 32767L)

/* Horner scheme with INTEGER coefficients: only the included calculators do the arithmetic */
static int16_t ECOCALLMETHOD CEcoLab2_HornerInt(/* in */ IEcoLab2Ptr_t me, /* in */ const int16_t* coeffs, /* in */ uint32_t num_coeffs, /* in */ int16_t x, /* out */ int32_t* result) {
    CEcoLab2* pCMe = (CEcoLab2*)me;
    int32_t r = 0;
    uint32_t i;

    if (me == 0 || coeffs == 0 || result == 0 || num_coeffs == 0) {
        return ECOLAB2_ERR_ARG;
    }
    if (pCMe->m_pIncEX == 0 || pCMe->m_pIncEY == 0) {
        return ECOLAB2_ERR_NOCALC;
    }
    r = coeffs[0];
    for (i = 1; i < num_coeffs; i++) {
        if (!ECOLAB2_FITS_INT16(r)) {
            return ECOLAB2_ERR_RANGE;
        }
        /* r = r * x   (IEcoCalculatorY of E) */
        r = pCMe->m_pIncEY->pVTbl->Multiplication(pCMe->m_pIncEY, (int16_t)r, x);
        if (!ECOLAB2_FITS_INT16(r)) {
            return ECOLAB2_ERR_RANGE;
        }
        /* r = r + c[i]   (IEcoCalculatorX of B, taken through E) */
        r = pCMe->m_pIncEX->pVTbl->Addition(pCMe->m_pIncEX, (int16_t)r, coeffs[i]);
    }
    *result = r;
    return ECOLAB2_OK;
}

/* The same four operations through E+B, D+A and C must give the same results */
static int16_t ECOCALLMETHOD CEcoLab2_CheckCalculators(/* in */ IEcoLab2Ptr_t me, /* in */ int16_t a, /* in */ int16_t b) {
    CEcoLab2* pCMe = (CEcoLab2*)me;
    int32_t ref;

    if (me == 0 || b == 0) {
        return ECOLAB2_ERR_ARG;
    }
    if (pCMe->m_pIncEX == 0 || pCMe->m_pIncEY == 0 || pCMe->m_pIncDX == 0 || pCMe->m_pIncDY == 0 ||
        pCMe->m_pIncCX == 0 || pCMe->m_pIncCY == 0) {
        return ECOLAB2_ERR_NOCALC;
    }

    /* 1. Addition */
    ref = pCMe->m_pIncEX->pVTbl->Addition(pCMe->m_pIncEX, a, b);
    if (pCMe->m_pIncDX->pVTbl->Addition(pCMe->m_pIncDX, a, b) != ref ||
        pCMe->m_pIncCX->pVTbl->Addition(pCMe->m_pIncCX, a, b) != ref) {
        return 1;
    }
    /* 2. Subtraction */
    ref = pCMe->m_pIncEX->pVTbl->Subtraction(pCMe->m_pIncEX, a, b);
    if (pCMe->m_pIncDX->pVTbl->Subtraction(pCMe->m_pIncDX, a, b) != ref ||
        pCMe->m_pIncCX->pVTbl->Subtraction(pCMe->m_pIncCX, a, b) != ref) {
        return 2;
    }
    /* 3. Multiplication */
    ref = pCMe->m_pIncEY->pVTbl->Multiplication(pCMe->m_pIncEY, a, b);
    if (pCMe->m_pIncDY->pVTbl->Multiplication(pCMe->m_pIncDY, a, b) != ref ||
        pCMe->m_pIncCY->pVTbl->Multiplication(pCMe->m_pIncCY, a, b) != ref) {
        return 3;
    }
    /* 4. Division */
    ref = pCMe->m_pIncEY->pVTbl->Division(pCMe->m_pIncEY, a, b);
    if (pCMe->m_pIncDY->pVTbl->Division(pCMe->m_pIncDY, a, b) != ref ||
        pCMe->m_pIncCY->pVTbl->Division(pCMe->m_pIncCY, a, b) != ref) {
        return 4;
    }
    return ECOLAB2_OK;
}

/* ===================================================================== */
/*  IEcoCalculatorX / IEcoCalculatorY - INCLUSION of E (and B inside E)  */
/* ===================================================================== */

static int16_t ECOCALLMETHOD CEcoLab2_IX_QueryInterface(/* in */ IEcoCalculatorXPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv) {
    CEcoLab2* pCMe = 0;

    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    pCMe = ECOLAB2_FROM_IX(me);
    return pCMe->m_pIUnkOuter->pVTbl->QueryInterface(pCMe->m_pIUnkOuter, riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoLab2_IX_AddRef(/* in */ IEcoCalculatorXPtr_t me) {
    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    return ECOLAB2_FROM_IX(me)->m_pIUnkOuter->pVTbl->AddRef(ECOLAB2_FROM_IX(me)->m_pIUnkOuter);
}

static uint32_t ECOCALLMETHOD CEcoLab2_IX_Release(/* in */ IEcoCalculatorXPtr_t me) {
    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    return ECOLAB2_FROM_IX(me)->m_pIUnkOuter->pVTbl->Release(ECOLAB2_FROM_IX(me)->m_pIUnkOuter);
}

static int32_t ECOCALLMETHOD CEcoLab2_Addition(/* in */ IEcoCalculatorXPtr_t me, /* in */ int16_t a, /* in */ int16_t b) {
    CEcoLab2* pCMe = 0;

    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    pCMe = ECOLAB2_FROM_IX(me);
    if (pCMe->m_pIncEX == 0) {
        return 0;
    }
    return pCMe->m_pIncEX->pVTbl->Addition(pCMe->m_pIncEX, a, b);
}

static int16_t ECOCALLMETHOD CEcoLab2_Subtraction(/* in */ IEcoCalculatorXPtr_t me, /* in */ int16_t a, /* in */ int16_t b) {
    CEcoLab2* pCMe = 0;

    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    pCMe = ECOLAB2_FROM_IX(me);
    if (pCMe->m_pIncEX == 0) {
        return 0;
    }
    return pCMe->m_pIncEX->pVTbl->Subtraction(pCMe->m_pIncEX, a, b);
}

static int16_t ECOCALLMETHOD CEcoLab2_IY_QueryInterface(/* in */ IEcoCalculatorYPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv) {
    CEcoLab2* pCMe = 0;

    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    pCMe = ECOLAB2_FROM_IY(me);
    return pCMe->m_pIUnkOuter->pVTbl->QueryInterface(pCMe->m_pIUnkOuter, riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoLab2_IY_AddRef(/* in */ IEcoCalculatorYPtr_t me) {
    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    return ECOLAB2_FROM_IY(me)->m_pIUnkOuter->pVTbl->AddRef(ECOLAB2_FROM_IY(me)->m_pIUnkOuter);
}

static uint32_t ECOCALLMETHOD CEcoLab2_IY_Release(/* in */ IEcoCalculatorYPtr_t me) {
    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    return ECOLAB2_FROM_IY(me)->m_pIUnkOuter->pVTbl->Release(ECOLAB2_FROM_IY(me)->m_pIUnkOuter);
}

static int32_t ECOCALLMETHOD CEcoLab2_Multiplication(/* in */ IEcoCalculatorYPtr_t me, /* in */ int16_t a, /* in */ int16_t b) {
    CEcoLab2* pCMe = 0;

    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    pCMe = ECOLAB2_FROM_IY(me);
    if (pCMe->m_pIncEY == 0) {
        return 0;
    }
    return pCMe->m_pIncEY->pVTbl->Multiplication(pCMe->m_pIncEY, a, b);
}

static int16_t ECOCALLMETHOD CEcoLab2_Division(/* in */ IEcoCalculatorYPtr_t me, /* in */ int16_t a, /* in */ int16_t b) {
    CEcoLab2* pCMe = 0;

    if (me == 0) {
        return -1; /* ERR_ECO_POINTER */
    }
    pCMe = ECOLAB2_FROM_IY(me);
    if (pCMe->m_pIncEY == 0 || b == 0) {
        return 0;
    }
    return pCMe->m_pIncEY->pVTbl->Division(pCMe->m_pIncEY, a, b);
}

/* ===================================================================== */
/*  Init / Create / Delete                                               */
/* ===================================================================== */

static int16_t ECOCALLMETHOD initCEcoLab2(/*in*/ CEcoLab2Ptr_t me, /* in */ IEcoUnknownPtr_t pIUnkSystem) {
    CEcoLab2* pCMe = (CEcoLab2*)me;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    int16_t result = ERR_ECO_POINTER;
    UGUID* rcid = (UGUID*)&CID_EcoMemoryManager1;

    if (me == 0) {
        return result;
    }

    pCMe->m_pISys = (IEcoSystem1*)pIUnkSystem;

    result = pCMe->m_pISys->pVTbl->QueryInterface(pCMe->m_pISys, &IID_IEcoInterfaceBus1, (void**)&pIBus);
    if (result != 0 || pIBus == 0) {
        return ERR_ECO_NOBUS;
    }

    /* Memory allocator */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        rcid = (UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt);
        pIMemExt->pVTbl->Release(pIMemExt);
    }
    result = pIBus->pVTbl->QueryComponent(pIBus, rcid, 0, &IID_IEcoMemoryAllocator1, (void**)&pCMe->m_pIMem);
    if (result != 0 || pCMe->m_pIMem == 0) {
        pIBus->pVTbl->Release(pIBus);
        return ERR_ECO_GET_MEMORY_ALLOCATOR;
    }

    /* INCLUSION of E: IEcoCalculatorY of E ... */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorE, 0, &IID_IEcoCalculatorY, (void**)&pCMe->m_pIncEY);
    if (result != 0 || pCMe->m_pIncEY == 0) {
        goto Fail;
    }
    /* ... and IEcoCalculatorX of B, which E aggregates (any interface gives access to any other) */
    result = pCMe->m_pIncEY->pVTbl->QueryInterface(pCMe->m_pIncEY, &IID_IEcoCalculatorX, (void**)&pCMe->m_pIncEX);
    if (result != 0 || pCMe->m_pIncEX == 0) {
        goto Fail;
    }

    /* INCLUSION of D (D includes A): IEcoCalculatorY and, through it, IEcoCalculatorX */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorD, 0, &IID_IEcoCalculatorY, (void**)&pCMe->m_pIncDY);
    if (result != 0 || pCMe->m_pIncDY == 0) {
        goto Fail;
    }
    result = pCMe->m_pIncDY->pVTbl->QueryInterface(pCMe->m_pIncDY, &IID_IEcoCalculatorX, (void**)&pCMe->m_pIncDX);
    if (result != 0 || pCMe->m_pIncDX == 0) {
        goto Fail;
    }

    /* INCLUSION of C: IEcoCalculatorX and, through it, IEcoCalculatorY */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorC, 0, &IID_IEcoCalculatorX, (void**)&pCMe->m_pIncCX);
    if (result != 0 || pCMe->m_pIncCX == 0) {
        goto Fail;
    }
    result = pCMe->m_pIncCX->pVTbl->QueryInterface(pCMe->m_pIncCX, &IID_IEcoCalculatorY, (void**)&pCMe->m_pIncCY);
    if (result != 0 || pCMe->m_pIncCY == 0) {
        goto Fail;
    }

    /* AGGREGATION of Eco.Lab1: our controlling IEcoUnknown is passed as the outer one,
     * IID_IEcoUnknown is requested (the rule of the aggregation) */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, pCMe->m_pIUnkOuter, &IID_IEcoUnknown, (void**)&pCMe->m_pLab1Unk);
    if (result != 0 || pCMe->m_pLab1Unk == 0) {
        goto Fail;
    }

    pIBus->pVTbl->Release(pIBus);
    return ERR_ECO_SUCCESS;

Fail:
    /* the references are released by Delete (the factory releases the instance) */
    pIBus->pVTbl->Release(pIBus);
    return (result != 0) ? result : ERR_ECO_POINTER;
}

static int16_t ECOCALLMETHOD createCEcoLab2(/* in */ CEcoLab2Ptr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter) {
    if (pCMe == 0) {
        return ERR_ECO_POINTER;
    }
    if (pIUnkOuter != 0) {
        pCMe->m_pIUnkOuter = pIUnkOuter;
    }
    else {
        pCMe->m_pIUnkOuter = (IEcoUnknown*)&pCMe->m_pVTblINondelegatingUnk;
    }
    return ERR_ECO_SUCCESS;
}

static void ECOCALLMETHOD deleteCEcoLab2(/* in */ CEcoLab2Ptr_t pCMe) {
    IEcoMemoryAllocator1* pIMem = 0;

    if (pCMe != 0) {
        pIMem = pCMe->m_pIMem;

        /* Included calculators: X before Y, the owning component (E, D, C) goes last */
        if (pCMe->m_pIncEX != 0) { pCMe->m_pIncEX->pVTbl->Release(pCMe->m_pIncEX); pCMe->m_pIncEX = 0; }
        if (pCMe->m_pIncEY != 0) { pCMe->m_pIncEY->pVTbl->Release(pCMe->m_pIncEY); pCMe->m_pIncEY = 0; }
        if (pCMe->m_pIncDX != 0) { pCMe->m_pIncDX->pVTbl->Release(pCMe->m_pIncDX); pCMe->m_pIncDX = 0; }
        if (pCMe->m_pIncDY != 0) { pCMe->m_pIncDY->pVTbl->Release(pCMe->m_pIncDY); pCMe->m_pIncDY = 0; }
        if (pCMe->m_pIncCX != 0) { pCMe->m_pIncCX->pVTbl->Release(pCMe->m_pIncCX); pCMe->m_pIncCX = 0; }
        if (pCMe->m_pIncCY != 0) { pCMe->m_pIncCY->pVTbl->Release(pCMe->m_pIncCY); pCMe->m_pIncCY = 0; }
        /* Aggregated Eco.Lab1: release the reference we own (non-delegating Release) */
        if (pCMe->m_pLab1Unk != 0) { pCMe->m_pLab1Unk->pVTbl->Release(pCMe->m_pLab1Unk); pCMe->m_pLab1Unk = 0; }

        if (pCMe->m_pISys != 0) {
            pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
        }
        if (pIMem != 0) {
            pIMem->pVTbl->Free(pIMem, pCMe);
            pIMem->pVTbl->Release(pIMem);
        }
    }
}

/* IEcoLab2 Virtual Table */
static IEcoLab2VTbl g_x9B3E7A1258C44D068F21C4A0D7E69B35VTbl = {
    CEcoLab2_QueryInterface,
    CEcoLab2_AddRef,
    CEcoLab2_Release,
    CEcoLab2_HornerInt,
    CEcoLab2_CheckCalculators
};

/* Non-delegating IEcoUnknown Virtual Table */
static IEcoUnknownVTbl g_x000000000000000000000000000000AAVTblLab2 = {
    CEcoLab2_NondelegatingQueryInterface,
    CEcoLab2_NondelegatingAddRef,
    CEcoLab2_NondelegatingRelease
};

/* IEcoCalculatorX Virtual Table (inclusion) */
static IEcoCalculatorXVTbl g_x9322111622484742AE0682819447843DVTblLab2 = {
    CEcoLab2_IX_QueryInterface,
    CEcoLab2_IX_AddRef,
    CEcoLab2_IX_Release,
    CEcoLab2_Addition,
    CEcoLab2_Subtraction
};

/* IEcoCalculatorY Virtual Table (inclusion) */
static IEcoCalculatorYVTbl g_xBD6414C29096423EA90C04D77AFD1CADVTblLab2 = {
    CEcoLab2_IY_QueryInterface,
    CEcoLab2_IY_AddRef,
    CEcoLab2_IY_Release,
    CEcoLab2_Multiplication,
    CEcoLab2_Division
};

/* Object Instance (prototype that the factory copies) */
CEcoLab2 g_xCEcoLab2 = {
    &g_x9B3E7A1258C44D068F21C4A0D7E69B35VTbl,
    &g_x000000000000000000000000000000AAVTblLab2,
    &g_x9322111622484742AE0682819447843DVTblLab2,
    &g_xBD6414C29096423EA90C04D77AFD1CADVTblLab2,

    initCEcoLab2,
    createCEcoLab2,
    deleteCEcoLab2,
    1, /* m_cRef */
    0, /* m_pIMem */
    0, /* m_pISys */
    0, /* m_pIUnkOuter */
    0, 0, /* m_pIncEY, m_pIncEX */
    0, 0, /* m_pIncDY, m_pIncDX */
    0, 0, /* m_pIncCY, m_pIncCX */
    0     /* m_pLab1Unk */
};
