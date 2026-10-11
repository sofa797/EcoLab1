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
 *   This header describes the implementation of the CEcoPolyAberth_659EAEED component
 *   Algorithm: Aberth-Ehrlich method
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */

#ifndef __C_ECOPOLYABERTH_H__
#define __C_ECOPOLYABERTH_H__

#include "IEcoPolynomialMath.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CEcoPolyAberth_659EAEED* CEcoPolyAberth_659EAEEDPtr_t;

typedef struct CEcoPolyAberth_659EAEED {

    /* IEcoPolynomialMath interface function table */
    IEcoPolynomialMathVTbl* m_pVTblIEcoPolynomialMath;


    /* Instance initialization */
    int16_t (ECOCALLMETHOD *Init)(/*in*/ CEcoPolyAberth_659EAEEDPtr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem);
    /* Instance creation */
    int16_t (ECOCALLMETHOD *Create)(/*in*/ CEcoPolyAberth_659EAEEDPtr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter);
    /* Deletion */
    void (ECOCALLMETHOD *Delete)(/*in*/ CEcoPolyAberth_659EAEEDPtr_t pCMe);


    /* Reference counter */
    uint32_t m_cRef;

    /* Interface for memory operations */
    IEcoMemoryAllocator1* m_pIMem;

    /* System interface */
    IEcoSystem1* m_pISys;

    /* The component is stateless (service approach): all data are passed to the methods as parameters */

} CEcoPolyAberth_659EAEED;

#endif /* __C_ECOPOLYABERTH_H__ */
