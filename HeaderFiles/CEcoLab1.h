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
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CEcoLab1_18D23C4C* CEcoLab1_18D23C4CPtr_t;

typedef struct CEcoLab1_18D23C4C {

    /* IEcoPolynomialMath interface function table */
    IEcoPolynomialMathVTbl* m_pVTblIEcoPolynomialMath;


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

    /* The component is stateless (service approach): all data are passed to the methods as parameters */

} CEcoLab1_18D23C4C;

#endif /* __C_ECOLAB1_H__ */
