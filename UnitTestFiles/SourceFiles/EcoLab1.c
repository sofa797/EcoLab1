/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   EcoLab1
 * </summary>
 *
 * <description>
 *   This source file is the entry point (console client / unit test).
 *   Task 43: roots of a polynomial. Three components (algorithms) implement the same
 *   interface IEcoPolynomialMath; the client chooses the algorithm by choosing the component (CID).
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */


/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoLab1.h"
#include "IdEcoPolyAberth.h"
#include "IdEcoPolyLaguerre.h"

/* The test client may use libc (only the components themselves must not) */
#include <stdio.h>
#include <time.h>

/*
 * Algorithm selection ("calculator" mode):
 *   0 - run all three algorithms and compare them with each other
 *   1 - Durand-Kerner (component EcoLab1)
 *   2 - Aberth-Ehrlich (component EcoPolyAberth)
 *   3 - Laguerre (component EcoPolyLaguerre)
 */
#define ALGORITHM_CHOICE   0
#define ALGORITHM_COUNT    3

static const char* AlgoName(int n) {
    if (n == 1) return "Durand-Kerner (EcoLab1)";
    if (n == 2) return "Aberth-Ehrlich (EcoPolyAberth)";
    return "Laguerre (EcoPolyLaguerre)";
}

static const UGUID* AlgoCID(int n) {
    if (n == 1) return &CID_EcoLab1;
    if (n == 2) return &CID_EcoPolyAberth;
    return &CID_EcoPolyLaguerre;
}

static int g_total  = 0;
static int g_passed = 0;

static double TestAbs(double x) { return (x < 0.0) ? -x : x; }

/* Is there a root (re, im) among the found ones (tolerance tol)? The used one is marked. */
static int TestHasRoot(const EcoComplexD* r, int* used, uint32_t n, double re, double im, double tol) {
    uint32_t i;
    for (i = 0; i < n; i++) {
        if (!used[i] && TestAbs(r[i].re - re) <= tol && TestAbs(r[i].im - im) <= tol) {
            used[i] = 1;
            return 1;
        }
    }
    return 0;
}

/* One test: coeffs[num] -> expected roots exp[expN][2] (re, im). */
static void RunRootsTest(IEcoPolynomialMath* pI, const char* name,
                         const double* coeffs, uint32_t num,
                         const double exp[][2], uint32_t expN, double tol) {
    EcoComplexD roots[ECO_POLY_MAX_DEGREE];
    int used[ECO_POLY_MAX_DEGREE];
    uint32_t cnt = 0, i;
    int16_t res;
    int ok = 1;

    for (i = 0; i < ECO_POLY_MAX_DEGREE; i++) used[i] = 0;
    res = pI->pVTbl->roots(pI, coeffs, num, roots, &cnt);
    if (res < 0 || cnt != expN) {
        ok = 0;
    } else {
        for (i = 0; i < expN; i++) {
            if (!TestHasRoot(roots, used, cnt, exp[i][0], exp[i][1], tol)) ok = 0;
        }
    }
    g_total++;
    if (ok) g_passed++;
    printf("[%s] %s (code=%d, n=%u)\n", ok ? "PASS" : "FAIL", name, (int)res, (unsigned)cnt);
    if (!ok) {
        for (i = 0; i < cnt; i++) {
            printf("      x%u = % .15f %c %.15f i\n", (unsigned)(i + 1), roots[i].re,
                   roots[i].im < 0 ? '-' : '+', roots[i].im < 0 ? -roots[i].im : roots[i].im);
        }
    }
}

static void RunErrorTest(const char* name, int16_t got, int16_t expected) {
    g_total++;
    if (got == expected) g_passed++;
    printf("[%s] %s (code=%d, expected %d)\n", got == expected ? "PASS" : "FAIL", name, (int)got, (int)expected);
}

static void TestRoots(IEcoPolynomialMath* pI) {
    /* 1. x^2 - 3x + 2 */
    static const double c1[] = { 1, -3, 2 };
    static const double e1[][2] = { {2,0}, {1,0} };
    /* 2. x^3 - 6x^2 + 11x - 6 */
    static const double c2[] = { 1, -6, 11, -6 };
    static const double e2[][2] = { {3,0}, {2,0}, {1,0} };
    /* 3. x^2 + 1 */
    static const double c3[] = { 1, 0, 1 };
    static const double e3[][2] = { {0,1}, {0,-1} };
    /* 4. x^4 - 1 */
    static const double c4[] = { 1, 0, 0, 0, -1 };
    static const double e4[][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
    /* 5. x^3 - 1 */
    static const double c5[] = { 1, 0, 0, -1 };
    static const double e5[][2] = { {1,0}, {-0.5, 0.866025403784438647}, {-0.5, -0.866025403784438647} };
    /* 6. 2x^2 - 8  (leading coefficient != 1) */
    static const double c6[] = { 2, 0, -8 };
    static const double e6[][2] = { {2,0}, {-2,0} };
    /* 7. x^3 - x^2 = x^2 (x - 1)  (zero roots) */
    static const double c7[] = { 1, -1, 0, 0 };
    static const double e7[][2] = { {1,0}, {0,0}, {0,0} };
    /* 8. 0x^3 + x^2 - 1  (leading zero coefficient) */
    static const double c8[] = { 0, 1, 0, -1 };
    static const double e8[][2] = { {1,0}, {-1,0} };
    /* 9. 2x - 4 */
    static const double c9[] = { 2, -4 };
    static const double e9[][2] = { {2,0} };
    /* 10. (x-1)(x-2)...(x-6) */
    static const double c10[] = { 1, -21, 175, -735, 1624, -1764, 720 };
    static const double e10[][2] = { {6,0}, {5,0}, {4,0}, {3,0}, {2,0}, {1,0} };
    /* 11. x^5 + x + 1 */
    static const double c11[] = { 1, 0, 0, 0, 1, 1 };
    static const double e11[][2] = { {-0.754877666246693, 0}, {0.877438833123346, 0.744861766619744},
                                     {0.877438833123346, -0.744861766619744}, {-0.5, 0.866025403784439},
                                     {-0.5, -0.866025403784439} };
    /* 12. 3x^2 + 2x + 1 : complex pair */
    static const double c12[] = { 3, 2, 1 };
    static const double e12[][2] = { {-0.333333333333333, 0.471404520791032}, {-0.333333333333333, -0.471404520791032} };
    /* 13. (x-1)^3 : triple root (reduced accuracy is expected) */
    static const double c13[] = { 1, -3, 3, -1 };
    static const double e13[][2] = { {1,0}, {1,0}, {1,0} };
    /* 14. x^10 - 1 : roots of unity (symmetric polynomial) */
    static const double c14[] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1 };
    static const double e14[][2] = { {1,0}, {-1,0},
        {0.809016994374947, 0.587785252292473}, {0.809016994374947, -0.587785252292473},
        {-0.809016994374947, 0.587785252292473}, {-0.809016994374947, -0.587785252292473},
        {0.309016994374947, 0.951056516295154}, {0.309016994374947, -0.951056516295154},
        {-0.309016994374947, 0.951056516295154}, {-0.309016994374947, -0.951056516295154} };

    static const double zero[] = { 0, 0, 0 };
    static const double konst[] = { 5 };
    static const double big[ECO_POLY_MAX_DEGREE + 2] = { 1 };
    EcoComplexD out[ECO_POLY_MAX_DEGREE];
    uint32_t cnt = 0;

    printf("\n--- roots (double) ---\n");
    RunRootsTest(pI, "x^2-3x+2",                c1,  3, e1,  2, 1e-12);
    RunRootsTest(pI, "x^3-6x^2+11x-6",          c2,  4, e2,  3, 1e-12);
    RunRootsTest(pI, "x^2+1",                   c3,  3, e3,  2, 1e-12);
    RunRootsTest(pI, "x^4-1",                   c4,  5, e4,  4, 1e-12);
    RunRootsTest(pI, "x^3-1",                   c5,  4, e5,  3, 1e-12);
    RunRootsTest(pI, "2x^2-8",                  c6,  3, e6,  2, 1e-12);
    RunRootsTest(pI, "x^3-x^2 (zero roots)",    c7,  4, e7,  3, 1e-12);
    RunRootsTest(pI, "leading zero: 0x^3+x^2-1", c8, 4, e8,  2, 1e-12);
    RunRootsTest(pI, "2x-4",                    c9,  2, e9,  1, 1e-12);
    RunRootsTest(pI, "(x-1)...(x-6)",           c10, 7, e10, 6, 1e-9);
    RunRootsTest(pI, "x^5+x+1",                 c11, 6, e11, 5, 1e-10);
    RunRootsTest(pI, "3x^2+2x+1",               c12, 3, e12, 2, 1e-12);
    RunRootsTest(pI, "(x-1)^3 (triple root)",   c13, 4, e13, 3, 1e-4);
    RunRootsTest(pI, "x^10-1 (symmetric)",      c14, 11, e14, 10, 1e-12);

    printf("--- errors and edge cases ---\n");
    RunErrorTest("coeffs == NULL",   pI->pVTbl->roots(pI, 0, 3, out, &cnt), ECO_POLY_ERR_ARG);
    RunErrorTest("out == NULL",      pI->pVTbl->roots(pI, c1, 3, 0, &cnt), ECO_POLY_ERR_ARG);
    RunErrorTest("count == NULL",    pI->pVTbl->roots(pI, c1, 3, out, 0), ECO_POLY_ERR_ARG);
    RunErrorTest("num_coeffs == 0",  pI->pVTbl->roots(pI, c1, 0, out, &cnt), ECO_POLY_ERR_ARG);
    RunErrorTest("zero polynomial",  pI->pVTbl->roots(pI, zero, 3, out, &cnt), ECO_POLY_ERR_ZERO);
    RunErrorTest("degree > MAX",     pI->pVTbl->roots(pI, big, ECO_POLY_MAX_DEGREE + 2, out, &cnt), ECO_POLY_ERR_DEGREE);
    RunErrorTest("constant (no roots)", pI->pVTbl->roots(pI, konst, 1, out, &cnt), ECO_POLY_OK);
    g_total++;
    if (cnt == 0) g_passed++;
    printf("[%s] constant: count == 0 (count=%u)\n", cnt == 0 ? "PASS" : "FAIL", (unsigned)cnt);
}

static void TestRootsFloatLong(IEcoPolynomialMath* pI) {
    static const float  cf[] = { 1.0f, -3.0f, 2.0f };
    static const long double cl[] = { 1.0L, 0.0L, 1.0L };
    EcoComplexF rf[2];
    EcoComplexL rl[2];
    uint32_t n = 0;
    int16_t res;
    int ok;

    printf("--- rootsf / rootsl ---\n");
    res = pI->pVTbl->rootsf(pI, cf, 3, rf, &n);
    ok = (res >= 0 && n == 2 && TestAbs(rf[0].re - 2.0) < 1e-5 && TestAbs(rf[1].re - 1.0) < 1e-5);
    g_total++; if (ok) g_passed++;
    printf("[%s] rootsf x^2-3x+2: %f, %f\n", ok ? "PASS" : "FAIL", rf[0].re, rf[1].re);

    res = pI->pVTbl->rootsl(pI, cl, 3, rl, &n);
    ok = (res >= 0 && n == 2 && TestAbs((double)rl[0].im - 1.0) < 1e-12 && TestAbs((double)rl[1].im + 1.0) < 1e-12);
    g_total++; if (ok) g_passed++;
    printf("[%s] rootsl x^2+1: %.15f%+.15fi, %.15f%+.15fi\n", ok ? "PASS" : "FAIL",
           (double)rl[0].re, (double)rl[0].im, (double)rl[1].re, (double)rl[1].im);
}

/* ------------------------------------------------------------------ */
/* Polynomials used for the comparison of algorithms and for the dump  */
/* ------------------------------------------------------------------ */
typedef struct DumpCase {
    const char* id;
    const char* name;
    uint32_t    num;
    double      c[ECO_POLY_MAX_DEGREE + 1];
    double      tol;     /* tolerance for the comparison of algorithms with each other */
} DumpCase;

static const DumpCase g_Cases[] = {
    {"P1",  "x^2-3x+2",                    3,  {1, -3, 2}, 1e-9},
    {"P2",  "x^3-6x^2+11x-6",              4,  {1, -6, 11, -6}, 1e-9},
    {"P3",  "x^2+1",                       3,  {1, 0, 1}, 1e-9},
    {"P4",  "x^4-1",                       5,  {1, 0, 0, 0, -1}, 1e-9},
    {"P5",  "x^3-1",                       4,  {1, 0, 0, -1}, 1e-9},
    {"P6",  "(x-1)...(x-6)",               7,  {1, -21, 175, -735, 1624, -1764, 720}, 1e-8},
    {"P7",  "x^5+x+1",                     6,  {1, 0, 0, 0, 1, 1}, 1e-9},
    {"P8",  "x^3-x^2",                     4,  {1, -1, 0, 0}, 1e-9},
    {"P9",  "(x-1)^3",                     4,  {1, -3, 3, -1}, 1e-3},
    {"P10", "x^8-2x^7+3x^5-x^3+4x-7",      9,  {1, -2, 0, 3, 0, -1, 0, 4, -7}, 1e-9},
    {"P11", "2x^4+3x^3-5x^2+x-9",          5,  {2, 3, -5, 1, -9}, 1e-9},
    {"P12", "x^10-1",                      11, {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1}, 1e-9},
    {"P13", "(x-1)...(x-10)",              11, {1, -55, 1320, -18150, 157773, -902055, 3416930, -8409500, 12753576, -10628640, 3628800}, 1e-5}
};
#define CASE_COUNT ((int)(sizeof(g_Cases) / sizeof(g_Cases[0])))

/* Compare algorithms: roots of algorithm a must coincide with roots of the first algorithm */
static void CrossCheck(IEcoPolynomialMath** pAlg, int first, int last) {
    EcoComplexD ref[ECO_POLY_MAX_DEGREE];
    EcoComplexD cur[ECO_POLY_MAX_DEGREE];
    uint32_t nr, nc, i, j;
    int a, c, ok, found;
    double dre, dim;

    if (first == last) {
        return;
    }
    printf("\n--- comparison of algorithms with %s ---\n", AlgoName(first));
    for (a = first + 1; a <= last; a++) {
        for (c = 0; c < CASE_COUNT; c++) {
            nr = 0; nc = 0;
            pAlg[first]->pVTbl->roots(pAlg[first], g_Cases[c].c, g_Cases[c].num, ref, &nr);
            pAlg[a]->pVTbl->roots(pAlg[a], g_Cases[c].c, g_Cases[c].num, cur, &nc);
            ok = (nr == nc);
            for (i = 0; ok && i < nc; i++) {
                found = 0;
                for (j = 0; j < nr && !found; j++) {
                    dre = cur[i].re - ref[j].re;
                    dim = cur[i].im - ref[j].im;
                    if (TestAbs(dre) <= g_Cases[c].tol && TestAbs(dim) <= g_Cases[c].tol) found = 1;
                }
                if (!found) ok = 0;
            }
            g_total++;
            if (ok) g_passed++;
            printf("[%s] algorithm %d vs %d: %s %s\n", ok ? "PASS" : "FAIL", a, first, g_Cases[c].id, g_Cases[c].name);
        }
    }
}

/*
 * Data dump for plots / comparison tables (see python/plot_roots.py).
 * Copy everything between CSV_BEGIN and CSV_END from the console into a text file.
 */
static void DumpCSV(IEcoPolynomialMath** pAlg, int first, int last) {
    EcoComplexD roots[ECO_POLY_MAX_DEGREE];
    uint32_t k, cnt, rep;
    int a, c, kk;
    int16_t res;
    clock_t t0;
    double usec;

    printf("\nCSV_BEGIN\n");
    for (a = first; a <= last; a++) {
        printf("ALG;%d;%s\n", a, AlgoName(a));
    }
    for (a = first; a <= last; a++) {
        for (c = 0; c < CASE_COUNT; c++) {
            cnt = 0;
            res = pAlg[a]->pVTbl->roots(pAlg[a], g_Cases[c].c, g_Cases[c].num, roots, &cnt);
            printf("POLY;%d;%s;%s;", a, g_Cases[c].id, g_Cases[c].name);
            for (kk = 0; kk < (int)g_Cases[c].num; kk++) {
                printf("%s%.17g", kk ? "," : "", g_Cases[c].c[kk]);
            }
            printf(";%d;%u\n", (int)res, (unsigned)cnt);
            for (k = 0; k < cnt; k++) {
                printf("ROOT;%d;%s;%u;%.17g;%.17g\n", a, g_Cases[c].id, (unsigned)k, roots[k].re, roots[k].im);
            }
            /* time of one call, microseconds (average over 2000 calls) */
            t0 = clock();
            for (rep = 0; rep < 2000; rep++) {
                pAlg[a]->pVTbl->roots(pAlg[a], g_Cases[c].c, g_Cases[c].num, roots, &cnt);
            }
            usec = ((double)(clock() - t0)) * 1.0e6 / (double)CLOCKS_PER_SEC / 2000.0;
            printf("TIME;%d;%s;%.3f\n", a, g_Cases[c].id, usec);
        }
    }
    printf("CSV_END\n");
}

/*
 *
 * <summary>
 *   EcoMain Function
 * </summary>
 *
 * <description>
 *   EcoMain function - entry point
 * </description>
 *
 */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Pointer to the system interface */
    IEcoSystem1* pISys = 0;
    /* Pointer to the interface for working with the system interface bus */
    IEcoInterfaceBus1* pIBus = 0;
    /* Pointers to the tested interfaces (index = algorithm number 1..3) */
    IEcoPolynomialMath* pAlg[ALGORITHM_COUNT + 1] = { 0, 0, 0, 0 };
    int a, first, last;

    /* System interface check and creation */
    if (pISys == 0) {
        result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void **)&pISys);
        if (result != 0 && pISys == 0) {
        /* Free the system interface in case of an error */
            goto Release;
        }
    }

    /* Getting the interface for working with the interface bus */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
        /* Free in case of an error */
        goto Release;
    }
#ifdef ECO_LIB
    /* Registration of the static components (one library contains all three) */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_011BF193DAAC48E0B83B3A8618D23C4C);
    if (result != 0 ) {
        goto Release;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoPolyAberth, (IEcoUnknown*)GetIEcoComponentFactoryPtr_C1642CE594A740DE8F79D08B659EAEED);
    if (result != 0 ) {
        goto Release;
    }
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoPolyLaguerre, (IEcoUnknown*)GetIEcoComponentFactoryPtr_9EEDAACC3D7144FAA5608DB94D0499F1);
    if (result != 0 ) {
        goto Release;
    }
#endif

    /* Choice of the algorithm = choice of the component to load */
    first = (ALGORITHM_CHOICE == 0) ? 1 : ALGORITHM_CHOICE;
    last  = (ALGORITHM_CHOICE == 0) ? ALGORITHM_COUNT : ALGORITHM_CHOICE;
    for (a = first; a <= last; a++) {
        result = pIBus->pVTbl->QueryComponent(pIBus, AlgoCID(a), 0, &IID_IEcoPolynomialMath, (void**) &pAlg[a]);
        if (result != 0 || pAlg[a] == 0) {
            /* Free interfaces in case of an error */
            goto Release;
        }
    }

    /* ---------------- Task 43: roots of a polynomial ---------------- */
    for (a = first; a <= last; a++) {
        printf("\n################ Algorithm %d: %s ################\n", a, AlgoName(a));
        TestRoots(pAlg[a]);
        TestRootsFloatLong(pAlg[a]);
    }
    CrossCheck(pAlg, first, last);
    printf("\nTOTAL: %d of %d tests passed\n", g_passed, g_total);
    DumpCSV(pAlg, first, last);
    result = (g_passed == g_total) ? 0 : -1;

Release:

    /* Free the tested interfaces */
    for (a = 1; a <= ALGORITHM_COUNT; a++) {
        if (pAlg[a] != 0) {
            pAlg[a]->pVTbl->Release(pAlg[a]);
        }
    }

    /* Free the interface for working with the interface bus */
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }

    /* Free the system interface */
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }

    return result;
}
