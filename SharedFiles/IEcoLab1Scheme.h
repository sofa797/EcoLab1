/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   IEcoLab1Scheme
 * </summary>
 *
 * <description>
 *   Auxiliary interface of the component EcoLab1 (Lab 2). It selects, at run time, HOW the
 *   calculator components are connected to EcoLab1 (which inner components are included /
 *   aggregated). It lets the unit test demonstrate every variation of the schemes of the task
 *   (Fig. 1 and Fig. 2) in one run. Like any other interface it is obtained with QueryInterface.
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */

#ifndef __I_ECOLAB1_SCHEME_H__
#define __I_ECOLAB1_SCHEME_H__

#include "IEcoBase1.h"

/* IEcoLab1Scheme IID = {6E1B9D30-4F27-4C85-A3D9-0B72E5C8A1F4} */
#ifndef __IID_IEcoLab1Scheme
static const UGUID IID_IEcoLab1Scheme = {0x01, 0x10, {0x6E, 0x1B, 0x9D, 0x30, 0x4F, 0x27, 0x4C, 0x85, 0xA3, 0xD9, 0x0B, 0x72, 0xE5, 0xC8, 0xA1, 0xF4}};
#endif /* __IID_IEcoLab1Scheme */

/*
 * Connection schemes of the calculators to EcoLab1:
 *                                 IEcoCalculatorX                 IEcoCalculatorY
 *   1 (Fig.1)  inclusion          A (inclusion)                   D (inclusion; D includes A)
 *   2 (Fig.1)  inclusion          B (inclusion)                   E (inclusion; E aggregates B)
 *   3 (Fig.2)  aggregation        B (aggregation)                 E (inclusion)      <- default
 *   4 (Fig.2)  aggregation        B (aggregation)                 D (inclusion)
 *   5          inclusion of C     C (inclusion)                   C (inclusion, the same instance)
 */
#define ECOLAB1_SCHEME_FIG1_A_D    1
#define ECOLAB1_SCHEME_FIG1_B_E    2
#define ECOLAB1_SCHEME_FIG2_B_E    3
#define ECOLAB1_SCHEME_FIG2_B_D    4
#define ECOLAB1_SCHEME_C           5
#define ECOLAB1_SCHEME_DEFAULT     ECOLAB1_SCHEME_FIG2_B_E

struct IEcoLab1Scheme;
typedef struct IEcoLab1Scheme* IEcoLab1SchemePtr_t;

typedef struct IEcoLab1SchemeVTbl {

    /* IEcoUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoLab1SchemePtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoLab1SchemePtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoLab1SchemePtr_t me);

    /* IEcoLab1Scheme */
    /* Disconnect the current inner components and connect the ones of the given scheme (1..5) */
    int16_t (ECOCALLMETHOD *SelectScheme)(/* in */ IEcoLab1SchemePtr_t me, /* in */ int16_t scheme);
    /* Number of the current scheme (0 - no calculators connected) */
    int16_t (ECOCALLMETHOD *get_Scheme)(/* in */ IEcoLab1SchemePtr_t me);

} IEcoLab1SchemeVTbl, *IEcoLab1SchemeVTblPtr_t;

interface IEcoLab1Scheme {
    struct IEcoLab1SchemeVTbl *pVTbl;
} IEcoLab1Scheme;

#endif /* __I_ECOLAB1_SCHEME_H__ */
