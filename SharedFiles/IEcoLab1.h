/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>IEcoLab1</summary>
 * <description>
 *   Interface for task 43: polynomial roots.
 *   Three implementations: Durand-Kerner, Aberth-Ehrlich, Laguerre.
 * </description>
 */

#ifndef __I_ECOLAB1_H__
#define __I_ECOLAB1_H__

#include "IEcoBase1.h"

/* IEcoLab1 IID = {4EC65D4B-CA68-43AC-9F74-0962906FA32D} */
#ifndef __IID_IEcoLab1
static const UGUID IID_IEcoLab1 = {0x01, 0x10,
    {0x4E,0xC6,0x5D,0x4B,0xCA,0x68,0x43,0xAC,
     0x9F,0x74,0x09,0x62,0x90,0x6F,0xA3,0x2D}};
#endif

/* Max polynomial degree (stack-only, no malloc) */
#define ECO_POLY_MAX_DEGREE   32

/* Return codes */
#define ECO_POLY_OK            0
#define ECO_POLY_WARN_LOWACC   1   /* multiple roots, reduced accuracy */
#define ECO_POLY_ERR_ARG      -1
#define ECO_POLY_ERR_ZERO     -2
#define ECO_POLY_ERR_DEGREE   -3
#define ECO_POLY_ERR_NOCONV   -4

/* Complex types (no <complex.h>) */
typedef struct EcoComplexF { float       re; float       im; } EcoComplexF;
typedef struct EcoComplexD { double      re; double      im; } EcoComplexD;
typedef struct EcoComplexL { long double re; long double im; } EcoComplexL;

typedef struct IEcoLab1* IEcoLab1Ptr_t;

typedef struct IEcoLab1VTbl {
    /* IEcoUnknown */
    int16_t  (ECOCALLMETHOD *QueryInterface)(IEcoLab1Ptr_t me, const UGUID* riid, voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(IEcoLab1Ptr_t me);
    uint32_t (ECOCALLMETHOD *Release)(IEcoLab1Ptr_t me);

    /* IEcoLab1: roots of polynomial
     *   p(x) = c[0]*x^n + c[1]*x^(n-1) + ... + c[n]   (descending powers)
     *   coeffs     - num_coeffs coefficients
     *   num_coeffs - count (degree n = num_coeffs - 1)
     *   out_roots  - capacity >= num_coeffs - 1
     *   out_count  - number of roots written
     */
    int16_t (ECOCALLMETHOD *roots )(IEcoLab1Ptr_t me, const double*      coeffs, uint32_t num_coeffs, EcoComplexD* out_roots, uint32_t* out_count);
    int16_t (ECOCALLMETHOD *rootsf)(IEcoLab1Ptr_t me, const float*       coeffs, uint32_t num_coeffs, EcoComplexF* out_roots, uint32_t* out_count);
    int16_t (ECOCALLMETHOD *rootsl)(IEcoLab1Ptr_t me, const long double* coeffs, uint32_t num_coeffs, EcoComplexL* out_roots, uint32_t* out_count);
} IEcoLab1VTbl, *IEcoLab1VTblPtr_t;

interface IEcoLab1 { struct IEcoLab1VTbl *pVTbl; } IEcoLab1;

#endif
