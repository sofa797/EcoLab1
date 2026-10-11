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
 *   This header describes the implementation of the CEcoPolyLaguerre_4D0499F1 component
 *   Algorithm: Laguerre method with implicit deflation
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */

#ifndef __C_ECOPOLYLAGUERRE_H__
#define __C_ECOPOLYLAGUERRE_H__

#include "IEcoPolynomialMath.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CEcoPolyLaguerre_4D0499F1* CEcoPolyLaguerre_4D0499F1Ptr_t;

typedef struct CEcoPolyLaguerre_4D0499F1 {

    /* IEcoPolynomialMath interface function table */
    IEcoPolynomialMathVTbl* m_pVTblIEcoPolynomialMath;


    /* Instance initialization */
    int16_t (ECOCALLMETHOD *Init)(/*in*/ CEcoPolyLaguerre_4D0499F1Ptr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem);
    /* Instance creation */
    int16_t (ECOCALLMETHOD *Create)(/*in*/ CEcoPolyLaguerre_4D0499F1Ptr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter);
    /* Deletion */
    void (ECOCALLMETHOD *Delete)(/*in*/ CEcoPolyLaguerre_4D0499F1Ptr_t pCMe);


    /* Reference counter */
    uint32_t m_cRef;

    /* Interface for memory operations */
    IEcoMemoryAllocator1* m_pIMem;

    /* System interface */
    IEcoSystem1* m_pISys;

    /* The component is stateless (service approach): all data are passed to the methods as parameters */

} CEcoPolyLaguerre_4D0499F1;

#endif /* __C_ECOPOLYLAGUERRE_H__ */
