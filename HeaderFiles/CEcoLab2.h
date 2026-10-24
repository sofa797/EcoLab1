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
 *   This header describes the implementation of the component CEcoLab2 (Lab 2).
 *   Fig. 3 of the task: the component INCLUDES E (which itself aggregates B), D (which includes A)
 *   and C, and AGGREGATES the component Eco.Lab1.
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */

#ifndef __C_ECOLAB2_H__
#define __C_ECOLAB2_H__

#include "IEcoLab2.h"
#include "IEcoPolynomialMath.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CEcoLab2* CEcoLab2Ptr_t;

/*
 * IMPORTANT: the first four members are pointers to function tables and must stay at the start of the
 * structure in this order (the object is found from an interface pointer by the position of the table).
 */
typedef struct CEcoLab2 {

    /* [0] IEcoLab2 interface function table (delegating IEcoUnknown) */
    IEcoLab2VTbl* m_pVTblIEcoLab2;

    /* [1] Non-delegating IEcoUnknown (the component can be aggregated as well) */
    IEcoUnknownVTbl* m_pVTblINondelegatingUnk;

    /* [2] IEcoCalculatorX interface function table (inclusion wrapper over E -> B) */
    IEcoCalculatorXVTbl* m_pVTblIX;

    /* [3] IEcoCalculatorY interface function table (inclusion wrapper over E) */
    IEcoCalculatorYVTbl* m_pVTblIY;

    /* Instance initialization */
    int16_t (ECOCALLMETHOD *Init)(/*in*/ CEcoLab2Ptr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem);

    /* Instance creation */
    int16_t (ECOCALLMETHOD *Create)(/*in*/ CEcoLab2Ptr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter);

    /* Deletion */
    void (ECOCALLMETHOD *Delete)(/*in*/ CEcoLab2Ptr_t pCMe);

    /* Reference counter */
    uint32_t m_cRef;

    /* Interface for memory operations */
    IEcoMemoryAllocator1* m_pIMem;

    /* System interface */
    IEcoSystem1* m_pISys;

    /* Delegating IEcoUnknown: the outer component (if aggregated) or the non-delegating IEcoUnknown */
    IEcoUnknown* m_pIUnkOuter;

    /* INCLUSION of E: Y of E and X of the component B that E aggregates */
    IEcoCalculatorY* m_pIncEY;
    IEcoCalculatorX* m_pIncEX;

    /* INCLUSION of D (D includes A itself) */
    IEcoCalculatorY* m_pIncDY;
    IEcoCalculatorX* m_pIncDX;

    /* INCLUSION of C */
    IEcoCalculatorY* m_pIncCY;
    IEcoCalculatorX* m_pIncCX;

    /* AGGREGATION of Eco.Lab1: its non-delegating IEcoUnknown */
    IEcoUnknown* m_pLab1Unk;

} CEcoLab2;

#endif /* __C_ECOLAB2_H__ */
