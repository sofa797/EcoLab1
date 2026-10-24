/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   IEcoLab2
 * </summary>
 *
 * <description>
 *   Own interface of the component EcoLab2 (Lab 2). The methods use the calculators that are
 *   included into / aggregated by EcoLab2.
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */

#ifndef __I_ECOLAB2_H__
#define __I_ECOLAB2_H__

#include "IEcoBase1.h"

/* IEcoLab2 IID = {9B3E7A12-58C4-4D06-8F21-C4A0D7E69B35} */
#ifndef __IID_IEcoLab2
static const UGUID IID_IEcoLab2 = {0x01, 0x10, {0x9B, 0x3E, 0x7A, 0x12, 0x58, 0xC4, 0x4D, 0x06, 0x8F, 0x21, 0xC4, 0xA0, 0xD7, 0xE6, 0x9B, 0x35}};
#endif /* __IID_IEcoLab2 */

/* Return codes of the methods of IEcoLab2 */
#define ECOLAB2_OK              0
#define ECOLAB2_ERR_ARG        -1   /* NULL pointer / n == 0                         */
#define ECOLAB2_ERR_NOCALC     -2   /* the needed calculator is not connected        */
#define ECOLAB2_ERR_RANGE      -3   /* an intermediate value does not fit into int16 */

struct IEcoLab2;
typedef struct IEcoLab2* IEcoLab2Ptr_t;

typedef struct IEcoLab2VTbl {

    /* IEcoUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoLab2Ptr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoLab2Ptr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoLab2Ptr_t me);

    /* IEcoLab2 */
    /*
     * HornerInt: value of the polynomial c[0]*x^n + ... + c[n] (n = num_coeffs - 1) with INTEGER
     * coefficients at the point x. Only the calculators are used: Multiplication (IEcoCalculatorY)
     * and Addition (IEcoCalculatorX) - the Horner scheme  r = r*x + c[i].
     */
    int16_t (ECOCALLMETHOD *HornerInt)(/* in */ IEcoLab2Ptr_t me, /* in */ const int16_t* coeffs, /* in */ uint32_t num_coeffs, /* in */ int16_t x, /* out */ int32_t* result);
    /*
     * CheckCalculators: performs +, -, *, / on (a, b) through EVERY connected calculator
     * (E with the aggregated B, D with the included A, C) and compares the results.
     * Returns 0 if all of them agree, otherwise the number (1..4) of the first operation that differs.
     * b must be non-zero.
     */
    int16_t (ECOCALLMETHOD *CheckCalculators)(/* in */ IEcoLab2Ptr_t me, /* in */ int16_t a, /* in */ int16_t b);

} IEcoLab2VTbl, *IEcoLab2VTblPtr_t;

interface IEcoLab2 {
    struct IEcoLab2VTbl *pVTbl;
} IEcoLab2;

#endif /* __I_ECOLAB2_H__ */
