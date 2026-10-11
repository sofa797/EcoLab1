/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   IEcoPolynomialMath
 * </summary>
 *
 * <description>
 *   Common interface of all "roots of a polynomial" components (task 43).
 *   It is implemented by three independent components (algorithms):
 *     1) EcoLab1        - Durand-Kerner method
 *     2) EcoPolyAberth  - Aberth-Ehrlich method
 *     3) EcoPolyLaguerre- Laguerre method with deflation
 *   The client chooses the algorithm by choosing the component (CID).
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */

#ifndef __I_ECOPOLYNOMIALMATH_H__
#define __I_ECOPOLYNOMIALMATH_H__

#include "IEcoBase1.h"

/* IEcoPolynomialMath IID = {FDF2ECDE-9932-4C0A-AEDF-2125217B6AE3} */
#ifndef __IID_IEcoPolynomialMath
static const UGUID IID_IEcoPolynomialMath = {0x01, 0x10, {0xFD, 0xF2, 0xEC, 0xDE, 0x99, 0x32, 0x4C, 0x0A, 0xAE, 0xDF, 0x21, 0x25, 0x21, 0x7B, 0x6A, 0xE3}};
#endif /* __IID_IEcoPolynomialMath */

/* Maximum polynomial degree (static memory only, no dynamic allocation) */
#define ECO_POLY_MAX_DEGREE   32

/* Return codes of roots / rootsf / rootsl */
#define ECO_POLY_OK            0   /* success                                          */
#define ECO_POLY_WARN_LOWACC   1   /* roots found, reduced accuracy (multiple roots)   */
#define ECO_POLY_ERR_ARG      -1   /* NULL pointer / num_coeffs == 0                   */
#define ECO_POLY_ERR_ZERO     -2   /* all coefficients are zero                        */
#define ECO_POLY_ERR_DEGREE   -3   /* degree is greater than ECO_POLY_MAX_DEGREE       */
#define ECO_POLY_ERR_NOCONV   -4   /* iterations did not converge                      */

/* Own complex number types (no <complex.h>) */
typedef struct EcoComplexF { float       re; float       im; } EcoComplexF;
typedef struct EcoComplexD { double      re; double      im; } EcoComplexD;
typedef struct EcoComplexL { long double re; long double im; } EcoComplexL;

typedef struct IEcoPolynomialMath* IEcoPolynomialMathPtr_t;

typedef struct IEcoPolynomialMathVTbl {

    /* IEcoUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoPolynomialMathPtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoPolynomialMathPtr_t me);

    /* IEcoPolynomialMath */
    /*
     * roots: roots of the polynomial
     *   p(x) = c[0]*x^n + c[1]*x^(n-1) + ... + c[n]     (same order as MATLAB roots)
     *
     *   coeffs      - num_coeffs coefficients in descending order of powers
     *   num_coeffs  - number of coefficients (degree n = num_coeffs - 1)
     *   out_roots   - output array, capacity >= num_coeffs - 1 elements
     *   out_count   - number of roots written
     */
    int16_t (ECOCALLMETHOD *roots )(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const double*      coeffs, /* in */ uint32_t num_coeffs, /* out */ EcoComplexD* out_roots, /* out */ uint32_t* out_count);
    int16_t (ECOCALLMETHOD *rootsf)(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const float*       coeffs, /* in */ uint32_t num_coeffs, /* out */ EcoComplexF* out_roots, /* out */ uint32_t* out_count);
    int16_t (ECOCALLMETHOD *rootsl)(/* in */ IEcoPolynomialMathPtr_t me, /* in */ const long double* coeffs, /* in */ uint32_t num_coeffs, /* out */ EcoComplexL* out_roots, /* out */ uint32_t* out_count);

} IEcoPolynomialMathVTbl, *IEcoPolynomialMathVTblPtr_t;

interface IEcoPolynomialMath {
    struct IEcoPolynomialMathVTbl *pVTbl;
} IEcoPolynomialMath;

#endif /* __I_ECOPOLYNOMIALMATH_H__ */
