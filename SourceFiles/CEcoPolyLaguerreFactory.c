/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   CEcoPolyLaguerre_4D0499F1Factory
 * </summary>
 *
 * <description>
 *   This header describes the implementation of the factory for the component
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
#include "CEcoPolyLaguerreFactory.h"

extern CEcoPolyLaguerre_4D0499F1 g_xCEcoPolyLaguerre_4D0499F1;

static int16_t ECOCALLMETHOD CEcoPolyLaguerre_4D0499F1Factory_QueryInterface(IEcoComponentFactory* me, const UGUID* riid, void** ppv) {
    if ( IsEqualUGUID(riid, &IID_IEcoUnknown) || IsEqualUGUID(riid, &IID_IEcoComponentFactory) ) {
        *ppv = me;
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }
    ((IEcoUnknown*)(*ppv))->pVTbl->AddRef((IEcoUnknown*)*ppv);

    return ERR_ECO_SUCCESS;
}

static uint32_t ECOCALLMETHOD CEcoPolyLaguerre_4D0499F1Factory_AddRef(/* in */ IEcoComponentFactory* me) {
    CEcoPolyLaguerre_4D0499F1Factory* pCMe = (CEcoPolyLaguerre_4D0499F1Factory*)me;

    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    return ++pCMe->m_cRef;
}

static uint32_t ECOCALLMETHOD CEcoPolyLaguerre_4D0499F1Factory_Release(/* in */ IEcoComponentFactory* me) {
    CEcoPolyLaguerre_4D0499F1Factory* pCMe = (CEcoPolyLaguerre_4D0499F1Factory*)me;

    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    /* Decrementing the component reference counter */
    --pCMe->m_cRef;

    /* If the counter is zeroed, free the instance data */
    if ( pCMe->m_cRef == 0 ) {
        return 0;
    }
    return pCMe->m_cRef;
}

static int16_t ECOCALLMETHOD CEcoPolyLaguerre_4D0499F1Factory_Init(/* in */ struct IEcoComponentFactory* me, /* in */ struct IEcoUnknown *pIUnkSystem, /* in */ void* pv) {
    int16_t result = ERR_ECO_POINTER;

    if (me == 0 ) {
        return result;
    }

    /* Initializing the component with parameters */

    return result;
}

/*
 * The function creates a component instance (allocates memory, copies the prototype g_xCEcoPolyLaguerre_4D0499F1,
 * calls Create and Init, returns the requested interface).
 */
static int16_t ECOCALLMETHOD CEcoPolyLaguerre_4D0499F1Factory_Alloc(/* in */ struct IEcoComponentFactory* me, /* in */ struct IEcoUnknown *pISystem, /* in */ struct IEcoUnknown *pIUnknownOuter, /* in */ const UGUID* riid, /* out */ void** ppv) {
    IEcoUnknown* pIUnk = 0;
    int16_t result = ERR_ECO_POINTER;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    CEcoPolyLaguerre_4D0499F1* pCObj = 0;
    UGUID* rcid = (UGUID*)&CID_EcoMemoryManager1;

    if (me == 0 || pISystem == 0 ) {
        return result; /* ERR_ECO_POINTER */
    }

    /* Aggregation provided that IID is IID_IEcoUnknown */
    if ( ( pIUnknownOuter != 0 ) && !IsEqualUGUID(riid, &IID_IEcoUnknown ) ) {
        /* aggregation not supported */
        return ERR_ECO_NOAGGREGATION;
    }

    /* Getting the application system interface */
    result = pISystem->pVTbl->QueryInterface(pISystem, &GID_IEcoSystem, (void **)&pISys);
    /* Check */
    if (result != 0 || pISys == 0) {
        return ERR_ECO_NOSYSTEM;
    }

    /* Getting the interface for working with the interface bus */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    /* Check */
    if (result != 0 || pIBus == 0) {
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_NOBUS;
    }

    /* Getting the component ID for memory operations */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        rcid = (UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt);
        pIMemExt->pVTbl->Release(pIMemExt);
    }

    /* Getting the memory allocator interface */
    result = pIBus->pVTbl->QueryComponent(pIBus, rcid, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);
    /* Check */
    if (result != 0 || pIMem == 0) {
        /* Freeing in case of an error */
        pIBus->pVTbl->Release(pIBus);
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_GET_MEMORY_ALLOCATOR;
    }

    /* Allocating memory for instance data */
    pCObj = (CEcoPolyLaguerre_4D0499F1*)pIMem->pVTbl->Alloc(pIMem, sizeof(CEcoPolyLaguerre_4D0499F1));
    if (pCObj == 0) {
        /* Freeing in case of an error */
        pIBus->pVTbl->Release(pIBus);
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_OUTOFMEMORY;
    }

    /* Forming instance data */
    pCObj = (CEcoPolyLaguerre_4D0499F1*)pIMem->pVTbl->Copy(pIMem, pCObj, &g_xCEcoPolyLaguerre_4D0499F1, sizeof(CEcoPolyLaguerre_4D0499F1));

    /* Component creation */
    pCObj->Create(pCObj, pISystem, pIUnknownOuter);

    /* Component initialization */
    result = pCObj->Init(pCObj, pISystem);

    /* Getting a pointer to the interface */
    pIUnk = (IEcoUnknown*)pCObj;
    result = pIUnk->pVTbl->QueryInterface(pIUnk, riid, ppv);

    /* Decrementing the reference requested by the Component Factory */
    pIUnk->pVTbl->Release(pIUnk);

    return result;
}

static char_t* ECOCALLMETHOD CEcoPolyLaguerre_4D0499F1Factory_get_Name(/* in */ struct IEcoComponentFactory* me) {
    CEcoPolyLaguerre_4D0499F1Factory* pCMe = (CEcoPolyLaguerre_4D0499F1Factory*)me;

    if (me == 0 ) {
        return 0; /* ERR_ECO_POINTER */
    }

    return pCMe->m_Name;
}

static char_t* ECOCALLMETHOD CEcoPolyLaguerre_4D0499F1Factory_get_Version(/* in */ struct IEcoComponentFactory* me) {
    CEcoPolyLaguerre_4D0499F1Factory* pCMe = (CEcoPolyLaguerre_4D0499F1Factory*)me;

    if (me == 0 ) {
        return 0; /* ERR_ECO_POINTER */
    }

    return pCMe->m_Version;
}

static char_t* ECOCALLMETHOD CEcoPolyLaguerre_4D0499F1Factory_get_Manufacturer(/* in */ struct IEcoComponentFactory* me) {
    CEcoPolyLaguerre_4D0499F1Factory* pCMe = (CEcoPolyLaguerre_4D0499F1Factory*)me;

    if (me == 0 ) {
        return 0; /* ERR_ECO_POINTER */
    }

    return pCMe->m_Manufacturer;
}

/* Create Virtual Table */
IEcoComponentFactoryVTbl g_x9EEDAACC3D7144FAA5608DB94D0499F1FactoryVTbl = {
    CEcoPolyLaguerre_4D0499F1Factory_QueryInterface,
    CEcoPolyLaguerre_4D0499F1Factory_AddRef,
    CEcoPolyLaguerre_4D0499F1Factory_Release,
    CEcoPolyLaguerre_4D0499F1Factory_Alloc,
    CEcoPolyLaguerre_4D0499F1Factory_Init,
    CEcoPolyLaguerre_4D0499F1Factory_get_Name,
    CEcoPolyLaguerre_4D0499F1Factory_get_Version,
    CEcoPolyLaguerre_4D0499F1Factory_get_Manufacturer
};

/* Factory instance */
CEcoPolyLaguerre_4D0499F1Factory g_x9EEDAACC3D7144FAA5608DB94D0499F1Factory = {
    &g_x9EEDAACC3D7144FAA5608DB94D0499F1FactoryVTbl,
    0,
    "EcoPolyLaguerre\0",
    "1.0.0.0\0",
    "\0"
};

/* The component library is linked statically: one library contains three components,
 * therefore the exported (dll) variant GetIEcoComponentFactoryPtr() is not defined here. */
#ifdef ECO_LIB
IEcoComponentFactory* GetIEcoComponentFactoryPtr_9EEDAACC3D7144FAA5608DB94D0499F1 = (IEcoComponentFactory*)&g_x9EEDAACC3D7144FAA5608DB94D0499F1Factory;
#endif
