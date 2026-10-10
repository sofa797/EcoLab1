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
 *   This source code describes the implementation of the interfaces for CEcoLab1_18D23C4C
 *   Task 43: roots of a polynomial (Durand-Kerner method), no libc functions used
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
#include "CEcoLab1.h"
/*
 *
 * <summary>
 *   QueryInterface Function
 * </summary>
 *
 * <description>
 *   QueryInterface function for the IEcoLab1 interface
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_QueryInterface(/* in */ IEcoLab1Ptr_t me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoLab1_18D23C4C* pCMe = (CEcoLab1_18D23C4C*)me;

    /* Pointer Validation */
    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    /* Validate and retrieve requested interface */
    if ( IsEqualUGUID(riid, &IID_IEcoLab1) ) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*)pCMe);
    }
    
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*)pCMe);
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }
    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   AddRef Function
 * </summary>
 *
 * <description>
 *   AddRef function for the IEcoLab1 interface
 * </description>
 *
 */
static uint32_t ECOCALLMETHOD CEcoLab1_18D23C4C_AddRef(/* in */ IEcoLab1Ptr_t me) {
    CEcoLab1_18D23C4C* pCMe = (CEcoLab1_18D23C4C*)me;

    /* Pointer Validation */
    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    return ++pCMe->m_cRef;
}

/*
 *
 * <summary>
 *   Release Function
 * </summary>
 *
 * <description>
 *   Release function for the IEcoLab1 interface
 * </description>
 *
 */
static uint32_t ECOCALLMETHOD CEcoLab1_18D23C4C_Release(/* in */ IEcoLab1Ptr_t me) {
    CEcoLab1_18D23C4C* pCMe = (CEcoLab1_18D23C4C*)me;

    /* Pointer Validation */
    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    /* Decrementing the component's reference count */
    --pCMe->m_cRef;
    /* If the count is zero, free the instance data */
    if ( pCMe->m_cRef == 0 ) {
        pCMe->Delete(pCMe);
        
        return 0;
    }
    return pCMe->m_cRef;
}

/*
 *
 * <summary>
 *   MyFunction Function
 * </summary>
 *
 * <description>
 *   Function
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_MyFunction(/* in */ IEcoLab1Ptr_t me, /* in */ char_t* Name, /* out */ char_t** copyName) {
    CEcoLab1_18D23C4C* pCMe = (CEcoLab1_18D23C4C*)me;
    int16_t index = 0;

    /* Pointer Validation */
    if (me == 0 || Name == 0 || copyName == 0) {
        return ERR_ECO_POINTER;
    }

    /* Copying the string */
    while(Name[index] != 0) {
        index++;
    }
    pCMe->m_Name = (char_t*)pCMe->m_pIMem->pVTbl->Alloc(pCMe->m_pIMem, index + 1);
    index = 0;
    while(Name[index] != 0) {
        pCMe->m_Name[index] = Name[index];
        index++;
    }
    *copyName = pCMe->m_Name;

    return ERR_ECO_SUCCESS;
}

/* ===================================================================== */
/*  TASK 43: roots of a polynomial                                       */
/*  Algorithm: Durand-Kerner (Weierstrass) method.                       */
/*  No libc: no math.h / string.h / stdlib.h, no malloc (stack arrays).  */
/*      z_k <- z_k - p(z_k) / prod_{j != k} (z_k - z_j)                  */
/*  where p is the monic polynomial.                                     */
/* ===================================================================== */

#define ECO_POLY_MAX_ITER     2000
#define ECO_POLY_TOL2         1.0e-30L   /* (1e-15)^2 : порог на квадрат относит. шага  */
#define ECO_POLY_TOL2_LOW     1.0e-8L    /* (1e-4)^2  : порог "сниженной точности" (кратные корни) */
#define ECO_POLY_IM_SNAP      1.0e-14L   /* отсечение шумовой мнимой части               */
#define ECO_POLY_SORT_EQ      1.0e-9L    /* "равенство" re при сортировке                */

/* ---------- вспомогательные функции (static, без libc) ---------- */

static long double EcoPoly_Abs(long double x) {
    return (x < 0.0L) ? -x : x;
}

static EcoComplexL EcoPoly_Make(long double re, long double im) {
    EcoComplexL r;
    r.re = re;
    r.im = im;
    return r;
}

static EcoComplexL EcoPoly_Sub(EcoComplexL a, EcoComplexL b) {
    return EcoPoly_Make(a.re - b.re, a.im - b.im);
}

static EcoComplexL EcoPoly_Mul(EcoComplexL a, EcoComplexL b) {
    return EcoPoly_Make(a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re);
}

static long double EcoPoly_Norm2(EcoComplexL a) {
    return a.re * a.re + a.im * a.im;
}

/* a / b ; вызывать только при Norm2(b) != 0 */
static EcoComplexL EcoPoly_Div(EcoComplexL a, EcoComplexL b) {
    long double d = EcoPoly_Norm2(b);
    return EcoPoly_Make((a.re * b.re + a.im * b.im) / d,
                        (a.im * b.re - a.re * b.im) / d);
}

/* Схема Горнера для полинома с вещественными коэффициентами c[0..n] */
static EcoComplexL EcoPoly_Horner(const long double* c, uint32_t n, EcoComplexL z) {
    EcoComplexL p;
    uint32_t i;
    p = EcoPoly_Make(c[0], 0.0L);
    for (i = 1; i <= n; i++) {
        p = EcoPoly_Mul(p, z);
        p.re += c[i];
    }
    return p;
}

/* Сортировка: по убыванию Re, при (почти) равных Re - по убыванию Im */
static int16_t EcoPoly_Before(EcoComplexL a, EcoComplexL b) {
    long double tol = ECO_POLY_SORT_EQ * (1.0L + EcoPoly_Abs(a.re));
    if (EcoPoly_Abs(a.re - b.re) <= tol) {
        return (a.im > b.im) ? 1 : 0;
    }
    return (a.re > b.re) ? 1 : 0;
}

static void EcoPoly_Sort(EcoComplexL* r, uint32_t n) {
    uint32_t i, j;
    EcoComplexL key;
    for (i = 1; i < n; i++) {
        key = r[i];
        j = i;
        while (j > 0 && EcoPoly_Before(key, r[j - 1])) {
            r[j] = r[j - 1];
            j--;
        }
        r[j] = key;
    }
}

/* ---------- ядро: все вычисления в long double ---------- */

static int16_t EcoPoly_RootsCore(const long double* a, uint32_t m,
                                 EcoComplexL* out, uint32_t* count) {
    long double c[ECO_POLY_MAX_DEGREE + 1];
    EcoComplexL z[ECO_POLY_MAX_DEGREE];
    EcoComplexL w, p, d, delta, diff;
    long double bound, v, maxstep, step, zn;
    uint32_t start, end, nz, n, i, k, j, iter, total;
    int16_t status;

    if (a == 0 || out == 0 || count == 0 || m == 0) {
        return ECO_POLY_ERR_ARG;
    }
    if (m > ECO_POLY_MAX_DEGREE + 1) {
        return ECO_POLY_ERR_DEGREE;
    }
    *count = 0;

    /* 1. Отбрасываем старшие нулевые коэффициенты */
    start = 0;
    while (start < m && a[start] == 0.0L) {
        start++;
    }
    if (start == m) {
        return ECO_POLY_ERR_ZERO;           /* p(x) == 0 */
    }

    /* 2. Младшие нулевые коэффициенты = корни x = 0 */
    end = m - 1;
    nz = 0;
    while (a[end] == 0.0L) {
        end--;
        nz++;
    }
    n = end - start;                        /* степень "ненулевой части" */

    /* 3. Нормировка: старший коэффициент = 1 */
    for (i = 0; i <= n; i++) {
        c[i] = a[start + i] / a[start];
    }

    /* 4. Нулевые корни */
    total = 0;
    for (i = 0; i < nz; i++) {
        out[total++] = EcoPoly_Make(0.0L, 0.0L);
    }

    status = ECO_POLY_OK;

    if (n == 1) {
        /* линейное уравнение x + c1 = 0 */
        out[total++] = EcoPoly_Make(-c[1], 0.0L);
    }
    else if (n >= 2) {
        /* 5. Начальные приближения: r * (0.4 + 0.9i)^k, r - граница Коши */
        bound = 0.0L;
        for (i = 1; i <= n; i++) {
            v = EcoPoly_Abs(c[i]);
            if (v > bound) {
                bound = v;
            }
        }
        bound += 1.0L;
        w = EcoPoly_Make(0.4L, 0.9L);
        z[0] = EcoPoly_Make(bound, 0.0L);
        for (k = 1; k < n; k++) {
            z[k] = EcoPoly_Mul(z[k - 1], w);
        }

        /* 6. Итерации Дюрана-Кернера (с немедленным использованием обновлённых z) */
        status = ECO_POLY_ERR_NOCONV;
        maxstep = 1.0L;
        for (iter = 0; iter < ECO_POLY_MAX_ITER; iter++) {
            maxstep = 0.0L;
            for (k = 0; k < n; k++) {
                p = EcoPoly_Horner(c, n, z[k]);
                d = EcoPoly_Make(1.0L, 0.0L);
                for (j = 0; j < n; j++) {
                    if (j != k) {
                        diff = EcoPoly_Sub(z[k], z[j]);
                        d = EcoPoly_Mul(d, diff);
                    }
                }
                if (EcoPoly_Norm2(d) == 0.0L) {
                    /* два приближения совпали - слегка сдвигаем */
                    z[k].re += 1.0e-7L * (1.0L + EcoPoly_Abs(z[k].re));
                    z[k].im += 1.0e-7L;
                    maxstep = 1.0L;
                    continue;
                }
                delta = EcoPoly_Div(p, d);
                z[k] = EcoPoly_Sub(z[k], delta);
                zn = 1.0L + EcoPoly_Norm2(z[k]);
                step = EcoPoly_Norm2(delta) / zn;
                if (step > maxstep) {
                    maxstep = step;
                }
            }
            if (maxstep <= ECO_POLY_TOL2) {
                status = ECO_POLY_OK;
                break;
            }
        }
        if (status != ECO_POLY_OK && maxstep <= ECO_POLY_TOL2_LOW) {
            status = ECO_POLY_WARN_LOWACC;
        }
        if (status == ECO_POLY_ERR_NOCONV) {
            return status;
        }

        /* 7. Отсечение шумовой мнимой части (коэффициенты вещественные) */
        for (k = 0; k < n; k++) {
            if (EcoPoly_Abs(z[k].im) <= ECO_POLY_IM_SNAP * (1.0L + EcoPoly_Abs(z[k].re))) {
                z[k].im = 0.0L;
            }
            out[total++] = z[k];
        }
    }

    EcoPoly_Sort(out, total);
    *count = total;
    return status;
}

/* ---------- методы интерфейса IEcoLab1 ---------- */

static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_rootsl(/* in */ IEcoLab1Ptr_t me,
        /* in */ const long double* coeffs, /* in */ uint32_t num_coeffs,
        /* out */ EcoComplexL* out_roots, /* out */ uint32_t* out_count) {
    if (me == 0 || coeffs == 0 || out_roots == 0 || out_count == 0 || num_coeffs == 0) {
        return ECO_POLY_ERR_ARG;
    }
    return EcoPoly_RootsCore(coeffs, num_coeffs, out_roots, out_count);
}

static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_roots(/* in */ IEcoLab1Ptr_t me,
        /* in */ const double* coeffs, /* in */ uint32_t num_coeffs,
        /* out */ EcoComplexD* out_roots, /* out */ uint32_t* out_count) {
    long double a[ECO_POLY_MAX_DEGREE + 1];
    EcoComplexL r[ECO_POLY_MAX_DEGREE];
    uint32_t i, cnt;
    int16_t res;

    if (me == 0 || coeffs == 0 || out_roots == 0 || out_count == 0 || num_coeffs == 0) {
        return ECO_POLY_ERR_ARG;
    }
    if (num_coeffs > ECO_POLY_MAX_DEGREE + 1) {
        return ECO_POLY_ERR_DEGREE;
    }
    for (i = 0; i < num_coeffs; i++) {
        a[i] = (long double)coeffs[i];
    }
    cnt = 0;
    res = EcoPoly_RootsCore(a, num_coeffs, r, &cnt);
    if (res >= 0) {
        for (i = 0; i < cnt; i++) {
            out_roots[i].re = (double)r[i].re;
            out_roots[i].im = (double)r[i].im;
        }
    }
    *out_count = (res >= 0) ? cnt : 0;
    return res;
}

static int16_t ECOCALLMETHOD CEcoLab1_18D23C4C_rootsf(/* in */ IEcoLab1Ptr_t me,
        /* in */ const float* coeffs, /* in */ uint32_t num_coeffs,
        /* out */ EcoComplexF* out_roots, /* out */ uint32_t* out_count) {
    long double a[ECO_POLY_MAX_DEGREE + 1];
    EcoComplexL r[ECO_POLY_MAX_DEGREE];
    uint32_t i, cnt;
    int16_t res;

    if (me == 0 || coeffs == 0 || out_roots == 0 || out_count == 0 || num_coeffs == 0) {
        return ECO_POLY_ERR_ARG;
    }
    if (num_coeffs > ECO_POLY_MAX_DEGREE + 1) {
        return ECO_POLY_ERR_DEGREE;
    }
    for (i = 0; i < num_coeffs; i++) {
        a[i] = (long double)coeffs[i];
    }
    cnt = 0;
    res = EcoPoly_RootsCore(a, num_coeffs, r, &cnt);
    if (res >= 0) {
        for (i = 0; i < cnt; i++) {
            out_roots[i].re = (float)r[i].re;
            out_roots[i].im = (float)r[i].im;
        }
    }
    *out_count = (res >= 0) ? cnt : 0;
    return res;
}


/*
 *
 * <summary>
 *   Init Function
 * </summary>
 *
 * <description>
 *   Instance initialization function
 * </description>
 *
 */
static int16_t ECOCALLMETHOD initCEcoLab1_18D23C4C(/*in*/ CEcoLab1_18D23C4CPtr_t me, /* in */ IEcoUnknownPtr_t pIUnkSystem) {
    CEcoLab1_18D23C4C* pCMe = (CEcoLab1_18D23C4C*)me;
    IEcoInterfaceBus1* pIBus = 0;

    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    int16_t result = ERR_ECO_POINTER;
    UGUID* rcid = (UGUID*)&CID_EcoMemoryManager1;

    /* Pointer Validation */
    if (me == 0 ) {
        return result;
    }

    /* Storing the pointer to the system interface */
    pCMe->m_pISys = (IEcoSystem1*)pIUnkSystem;

    /* Getting the interface for working with the interface bus */
    result = pCMe->m_pISys->pVTbl->QueryInterface(pCMe->m_pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);

    /* Getting the component ID for working with memory */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        rcid = (UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt);
        pIMemExt->pVTbl->Release(pIMemExt);
    }

    /* Getting the memory allocator interface */
    result = pIBus->pVTbl->QueryComponent(pIBus, rcid, 0, &IID_IEcoMemoryAllocator1, (void**) &pCMe->m_pIMem);
    /* Check */
    if (result != 0 || pCMe->m_pIMem == 0) {
        result = ERR_ECO_GET_MEMORY_ALLOCATOR;
    }



    /* Freeing */
    pIBus->pVTbl->Release(pIBus);

    return result;
}

/*
 *
 * <summary>
 *   Create Function
 * </summary>
 *
 * <description>
 *   Instance creation function
 * </description>
 *
 */
static int16_t ECOCALLMETHOD createCEcoLab1_18D23C4C(/* in */ CEcoLab1_18D23C4CPtr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter) {
    int16_t result = ERR_ECO_POINTER;

    /* Pointer Validation */
    if (pCMe == 0) {
        return result; /* ERR_ECO_POINTER */
    }



    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   Delete Function
 * </summary>
 *
 * <description>
 *   Instance freeing function
 * </description>
 *
 */
static void ECOCALLMETHOD deleteCEcoLab1_18D23C4C(/* in */ CEcoLab1_18D23C4CPtr_t pCMe) {
    IEcoMemoryAllocator1* pIMem = 0;

    if (pCMe != 0 ) {
        pIMem = pCMe->m_pIMem;
        /* Freeing */
        if ( pCMe->m_Name != 0 ) {
            pIMem->pVTbl->Free(pIMem, pCMe->m_Name);
        }
        if ( pCMe->m_pISys != 0 ) {
            pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
        }
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
    }
}

/* IEcoLab1 Virtual Table */
IEcoLab1VTbl g_x4EC65D4BCA6843AC9F740962906FA32DVTbl_18D23C4C = {
    CEcoLab1_18D23C4C_QueryInterface,
    CEcoLab1_18D23C4C_AddRef,
    CEcoLab1_18D23C4C_Release,
    CEcoLab1_18D23C4C_MyFunction,
    CEcoLab1_18D23C4C_roots,
    CEcoLab1_18D23C4C_rootsf,
    CEcoLab1_18D23C4C_rootsl
};



/* Object Instance */
CEcoLab1_18D23C4C g_xCEcoLab1_18D23C4C = {
    &g_x4EC65D4BCA6843AC9F740962906FA32DVTbl_18D23C4C,
   
    initCEcoLab1_18D23C4C,
    createCEcoLab1_18D23C4C,
    deleteCEcoLab1_18D23C4C,
    1, /* m_cRef */
    0, /* m_pISys */
    0, /* m_pISys */
    0  /* m_Name */
};
