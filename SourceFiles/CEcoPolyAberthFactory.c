/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   CEcoPolyAberth_659EAEEDFactory
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

#include "CEcoPolyAberth.h"
#include "CEcoPolyAberthFactory.h"

extern CEcoPolyAberth_659EAEED g_xCEcoPolyAberth_659EAEED;

static int16_t ECOCALLMETHOD CEcoPolyAberth_659EAEEDFactory_QueryInterface(IEcoComponentFactory* me, const UGUID* riid, void** ppv) {
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

static uint32_t ECOCALLMETHOD CEcoPolyAberth_659EAEEDFactory_AddRef(/* in */ IEcoComponentFactory* me) {
    CEcoPolyAberth_659EAEEDFactory* pCMe = (CEcoPolyAberth_659EAEEDFactory*)me;

    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    return ++pCMe->m_cRef;
}

static uint32_t ECOCALLMETHOD CEcoPolyAberth_659EAEEDFactory_Release(/* in */ IEcoComponentFactory* me) {
    CEcoPolyAberth_659EAEEDFactory* pCMe = (CEcoPolyAberth_659EAEEDFactory*)me;

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

static int16_t ECOCALLMETHOD CEcoPolyAberth_659EAEEDFactory_Init(/* in */ struct IEcoComponentFactory* me, /* in */ struct IEcoUnknown *pIUnkSystem, /* in */ void* pv) {
    int16_t result = ERR_ECO_POINTER;

    if (me == 0 ) {
        return result;
    }

    /* Initializing the component with parameters */

    return result;
}

/*
 * The function creates a component instance (allocates memory, copies the prototype g_xCEcoPolyAberth_659EAEED,
 * calls Create and Init, returns the requested interface).
 */
static int16_t ECOCALLMETHOD CEcoPolyAberth_659EAEEDFactory_Alloc(/* in */ struct IEcoComponentFactory* me, /* in */ struct IEcoUnknown *pISystem, /* in */ struct IEcoUnknown *pIUnknownOuter, /* in */ const UGUID* riid, /* out */ void** ppv) {
    IEcoUnknown* pIUnk = 0;
    int16_t result = ERR_ECO_POINTER;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    CEcoPolyAberth_659EAEED* pCObj = 0;
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
    pCObj = (CEcoPolyAberth_659EAEED*)pIMem->pVTbl->Alloc(pIMem, sizeof(CEcoPolyAberth_659EAEED));
    if (pCObj == 0) {
        /* Freeing in case of an error */
        pIBus->pVTbl->Release(pIBus);
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_OUTOFMEMORY;
    }

    /* Forming instance data */
    pCObj = (CEcoPolyAberth_659EAEED*)pIMem->pVTbl->Copy(pIMem, pCObj, &g_xCEcoPolyAberth_659EAEED, sizeof(CEcoPolyAberth_659EAEED));

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

static char_t* ECOCALLMETHOD CEcoPolyAberth_659EAEEDFactory_get_Name(/* in */ struct IEcoComponentFactory* me) {
    CEcoPolyAberth_659EAEEDFactory* pCMe = (CEcoPolyAberth_659EAEEDFactory*)me;

    if (me == 0 ) {
        return 0; /* ERR_ECO_POINTER */
    }

    return pCMe->m_Name;
}

static char_t* ECOCALLMETHOD CEcoPolyAberth_659EAEEDFactory_get_Version(/* in */ struct IEcoComponentFactory* me) {
    CEcoPolyAberth_659EAEEDFactory* pCMe = (CEcoPolyAberth_659EAEEDFactory*)me;

    if (me == 0 ) {
        return 0; /* ERR_ECO_POINTER */
    }

    return pCMe->m_Version;
}

static char_t* ECOCALLMETHOD CEcoPolyAberth_659EAEEDFactory_get_Manufacturer(/* in */ struct IEcoComponentFactory* me) {
    CEcoPolyAberth_659EAEEDFactory* pCMe = (CEcoPolyAberth_659EAEEDFactory*)me;

    if (me == 0 ) {
        return 0; /* ERR_ECO_POINTER */
    }

    return pCMe->m_Manufacturer;
}

/* Create Virtual Table */
IEcoComponentFactoryVTbl g_xC1642CE594A740DE8F79D08B659EAEEDFactoryVTbl = {
    CEcoPolyAberth_659EAEEDFactory_QueryInterface,
    CEcoPolyAberth_659EAEEDFactory_AddRef,
    CEcoPolyAberth_659EAEEDFactory_Release,
    CEcoPolyAberth_659EAEEDFactory_Alloc,
    CEcoPolyAberth_659EAEEDFactory_Init,
    CEcoPolyAberth_659EAEEDFactory_get_Name,
    CEcoPolyAberth_659EAEEDFactory_get_Version,
    CEcoPolyAberth_659EAEEDFactory_get_Manufacturer
};

/* Factory instance */
CEcoPolyAberth_659EAEEDFactory g_xC1642CE594A740DE8F79D08B659EAEEDFactory = {
    &g_xC1642CE594A740DE8F79D08B659EAEEDFactoryVTbl,
    0,
    "EcoPolyAberth\0",
    "1.0.0.0\0",
    "\0"
};

/* The component library is linked statically: one library contains three components,
 * therefore the exported (dll) variant GetIEcoComponentFactoryPtr() is not defined here. */
#ifdef ECO_LIB
IEcoComponentFactory* GetIEcoComponentFactoryPtr_C1642CE594A740DE8F79D08B659EAEED = (IEcoComponentFactory*)&g_xC1642CE594A740DE8F79D08B659EAEEDFactory;
#endif
