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
 *   This header describes the implementation of the CEcoLab1_18D23C4C component
 *   Algorithm: Durand-Kerner method
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */

#ifndef __C_ECOLAB1_H__
#define __C_ECOLAB1_H__

#include "IEcoPolynomialMath.h"
#include "IEcoLab1Scheme.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CEcoLab1_18D23C4C* CEcoLab1_18D23C4CPtr_t;

/*
 * IMPORTANT: the first five members are pointers to function tables and must stay at the start of the
 * structure in this order. Several interfaces of one object are distinguished by the position of
 * the table pointer: the object is found from an interface pointer by subtracting
 * (index * sizeof(pointer)).
 */
typedef struct CEcoLab1_18D23C4C {

    /* [0] IEcoPolynomialMath interface function table (delegating IEcoUnknown) */
    IEcoPolynomialMathVTbl* m_pVTblIEcoPolynomialMath;

    /* [1] Non-delegating IEcoUnknown (aggregation support: this component can be aggregated) */
    IEcoUnknownVTbl* m_pVTblINondelegatingUnk;

    /* [2] IEcoCalculatorX interface function table (inclusion wrapper) */
    IEcoCalculatorXVTbl* m_pVTblIX;

    /* [3] IEcoCalculatorY interface function table (inclusion wrapper) */
    IEcoCalculatorYVTbl* m_pVTblIY;

    /* [4] IEcoLab1Scheme interface function table */
    IEcoLab1SchemeVTbl* m_pVTblIScheme;

    /* Instance initialization */
    int16_t (ECOCALLMETHOD *Init)(/*in*/ CEcoLab1_18D23C4CPtr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem);

    /* Instance creation */
    int16_t (ECOCALLMETHOD *Create)(/*in*/ CEcoLab1_18D23C4CPtr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter);

    /* Deletion */
    void (ECOCALLMETHOD *Delete)(/*in*/ CEcoLab1_18D23C4CPtr_t pCMe);

    /* Reference counter */
    uint32_t m_cRef;

    /* Interface for memory operations */
    IEcoMemoryAllocator1* m_pIMem;

    /* System interface */
    IEcoSystem1* m_pISys;

    /* Delegating IEcoUnknown: the outer component (if aggregated) or the non-delegating IEcoUnknown */
    IEcoUnknown* m_pIUnkOuter;

    /* Number of the connected calculator scheme (see IEcoLab1Scheme.h), 0 - nothing connected */
    int16_t m_Scheme;

    /* INCLUSION: interfaces of the included components (EcoLab1 calls their methods itself) */
    IEcoCalculatorX* m_pIncX;
    IEcoCalculatorY* m_pIncY;

    /* AGGREGATION: non-delegating IEcoUnknown of the aggregated component (B) */
    IEcoUnknown* m_pAggUnk;

    /* The component is stateless as far as the algorithm is concerned (service approach):
     * all data are passed to the methods as parameters */
} CEcoLab1_18D23C4C;

#endif /* __C_ECOLAB1_H__ */
