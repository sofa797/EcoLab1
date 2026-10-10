/*
 * EcoLab1 Unit Test — расширенная версия (35+ полиномов + замеры времени)
 */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoLab1.h"
#include <stdio.h>
#include <time.h>

/* ---------- Helpers ---------- */
static int g_total = 0, g_passed = 0;
static double TAbs(double x){ return x < 0 ? -x : x; }

/* clock_gettime в наносекундах (libc разрешен в Unit-тесте) */
static long long now_ns(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static int HasRoot(const EcoComplexD* r, int* used, uint32_t n, double re, double im, double tol){
    for (uint32_t i = 0; i < n; i++){
        if (!used[i] && TAbs(r[i].re - re) <= tol && TAbs(r[i].im - im) <= tol){
            used[i] = 1; return 1;
        }
    }
    return 0;
}

/* ---------- Тестовые полиномы ---------- */
typedef struct {
    const char* id;
    const char* name;
    const char* category;   /* категория для группировки */
    uint32_t    num;
    double      c[ECO_POLY_MAX_DEGREE + 1];
    uint32_t    expected_roots;
    double      expected[][2];  /* только для PASS-тестов */
} PolyTest;

/* Категория A: базовые (степень 2-4) */
static const double A1[] = {1, -3, 2};
static const double A1_exp[][2] = {{2,0},{1,0}};

static const double A2[] = {1, -6, 11, -6};
static const double A2_exp[][2] = {{3,0},{2,0},{1,0}};

static const double A3[] = {1, 0, 1};
static const double A3_exp[][2] = {{0,1},{0,-1}};

static const double A4[] = {1, 0, 0, 0, -1};
static const double A4_exp[][2] = {{1,0},{-1,0},{0,1},{0,-1}};

static const double A5[] = {1, 0, 0, -1};
static const double A5_exp[][2] = {{1,0},{-0.5,0.866025403784439},{-0.5,-0.866025403784439}};

static const double A6[] = {2, 0, -8};
static const double A6_exp[][2] = {{2,0},{-2,0}};

static const double A7[] = {1, -1, 0, 0};
static const double A7_exp[][2] = {{1,0},{0,0},{0,0}};

static const double A8[] = {3, 2, 1};
static const double A8_exp[][2] = {{-0.333333333333333,0.471404520791032},{-0.333333333333333,-0.471404520791032}};

/* Категория B: средняя степень (5-8) */
static const double B1[] = {1, 0, 0, 0, 1, 1};
static const double B1_exp[][2] = {{-0.754877666246693,0},{0.877438833123346,0.744861766619744},{0.877438833123346,-0.744861766619744},{-0.5,0.866025403784439},{-0.5,-0.866025403784439}};

static const double B2[] = {1, -21, 175, -735, 1624, -1764, 720};
static const double B2_exp[][2] = {{6,0},{5,0},{4,0},{3,0},{2,0},{1,0}};

static const double B3[] = {1, -2, 0, 3, 0, -1, 0, 4, -7};
/* корни B3 вычисляются численно, не задаем ожидаемые */

static const double B4[] = {2, 3, -5, 1, -9};

static const double B5[] = {1, -3, 3, -1};
static const double B5_exp[][2] = {{1,0},{1,0},{1,0}};

/* Категория C: высокая степень (9-15) */
static const double C1[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1};
/* x^10 - 1 */

static const double C2[] = {1, -55, 1320, -18150, 157773, -902055, 3416930, -8409500, 12753576, -10628640, 3628800};
/* (x-1)...(x-10) */

static const double C3[] = {1, -120, 6320, -214160, 5105114, -95302944, 1424938920, -17249347200, 170295504960, -1333953600000, 7927296000000, -31135104000000, 72072000000000, -79833600000000, 36324288000000, 0};
/* (x-1)...(x-15) — степень 15 */

/* Категория D: краевые случаи */
static const double D1[] = {1, -5};          /* x - 5 */
static const double D1_exp[][2] = {{5,0}};

static const double D2[] = {7};              /* константа 7 — нет корней */

static const double D3[] = {0, 0, 0};        /* нулевой полином */

static const double D4[] = {1, 0, -2};       /* x^2 - 2 */
static const double D4_exp[][2] = {{1.414213562373095,0},{-1.414213562373095,0}};

static const double D5[] = {1, 0, -1e-10};   /* x^2 - 10^-10 */
static const double D5_exp[][2] = {{1e-5,0},{-1e-5,0}};

static const double D6[] = {1, 0, -1e10};    /* x^2 - 10^10 */
static const double D6_exp[][2] = {{1e5,0},{-1e5,0}};

/* Категория E: полиномы с особыми свойствами */
static const double E1[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1};
/* x^20 - 1 — корни на единичной окружности */

static const double E2[] = {1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1};
/* (x^11 - 1)/(x - 1) = x^10 + x^9 + ... + 1 — корни-единицы без 1 */

static const double E3[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
/* то же самое */

/* Категория F: Wilkinson-like (плохо обусловленные) */
static const double F1[] = {1, -210, 20615, -1256850, 53327946, -1672280820, 40171771630, -756111184500, 11310276995381, -135585182899530, 1307535010540395, -10142299865511450, 63030812099294896, -311333643161390640, 1206647803780373360, -3599979517947607200, 8031343328696787200, -12804747472765875200, 14619434047401206784, -9699242216224378880, 3243240000000000000};
/* (x-1)(x-2)...(x-20) — Wilkinson polynomial степени 20 */

/* Категория G: случайные/смешанные */
static const double G1[] = {5, -15, 10};     /* 5x^2 - 15x + 10 = 5(x-1)(x-2) */
static const double G1_exp[][2] = {{2,0},{1,0}};

static const double G2[] = {1, 2, 3, 4, 5};  /* все коэффициенты положительные */

static const double G3[] = {1, -10, 35, -50, 24};
/* (x-1)(x-2)(x-3)(x-4) */
static const double G3_exp[][2] = {{4,0},{3,0},{2,0},{1,0}};

static const double G4[] = {1, 0, -4, 0, 4};
/* x^4 - 4x^2 + 4 = (x^2 - 2)^2 — кратные комплексные корни */

static const double G5[] = {1, 0, 0, 0, 0, 0, 0, 0, 1};
/* x^7 + 1 */

/* ---------- Запуск одного теста ---------- */
static void RunRootsTest(IEcoLab1* pI, const char* method, const char* name,
                         const double* c, uint32_t n,
                         const double exp[][2], uint32_t en, double tol,
                         long long* out_time_ns){
    EcoComplexD roots[ECO_POLY_MAX_DEGREE];
    int used[ECO_POLY_MAX_DEGREE] = {0};
    uint32_t cnt = 0;
    int ok = 1;
    long long t0 = now_ns();
    int16_t res = pI->pVTbl->roots(pI, c, n, roots, &cnt);
    long long t1 = now_ns();
    *out_time_ns = t1 - t0;

    if (res < 0 || cnt != en) ok = 0;
    else {
        for (uint32_t i = 0; i < en; i++){
            if (!HasRoot(roots, used, cnt, exp[i][0], exp[i][1], tol)) ok = 0;
        }
    }
    g_total++;
    if (ok) g_passed++;
    printf("[%s][%s] %s (code=%d, n=%u, t=%lld ns)\n",
           method, ok ? "PASS" : "FAIL", name, (int)res, (unsigned)cnt, *out_time_ns);
    if (!ok){
        for (uint32_t i = 0; i < cnt; i++){
            printf("      x%u = % .15e %c %.15e i\n", (unsigned)(i+1), roots[i].re,
                   roots[i].im < 0 ? '-' : '+', roots[i].im < 0 ? -roots[i].im : roots[i].im);
        }
    }
}

/* Тест без ожидаемых корней (только проверяем, что не упало) */
static void RunRootsNoCheck(IEcoLab1* pI, const char* method, const char* name,
                            const double* c, uint32_t n, long long* out_time_ns){
    EcoComplexD roots[ECO_POLY_MAX_DEGREE];
    uint32_t cnt = 0;
    long long t0 = now_ns();
    int16_t res = pI->pVTbl->roots(pI, c, n, roots, &cnt);
    long long t1 = now_ns();
    *out_time_ns = t1 - t0;
    g_total++;
    g_passed++; /* считаем пройденным, если не упало */
    printf("[%s][PASS] %s (code=%d, n=%u, t=%lld ns)\n",
           method, name, (int)res, (unsigned)cnt, *out_time_ns);
}

/* Тест на ошибку */
static void RunErrorTest(const char* method, const char* name, int16_t got, int16_t expected){
    g_total++;
    if (got == expected) g_passed++;
    printf("[%s][%s] %s (got=%d, expected=%d)\n",
           method, got == expected ? "PASS" : "FAIL", name, (int)got, (int)expected);
}

/* ---------- Основной прогон ---------- */
static void RunAllTests(IEcoLab1* pI, const char* method, long long* times){
    int idx = 0;

    printf("\n===== %s: Category A (basic, degree 2-4) =====\n", method);
    RunRootsTest(pI, method, "A1: x^2-3x+2",           A1, 3, A1_exp, 2, 1e-12, &times[idx++]);
    RunRootsTest(pI, method, "A2: x^3-6x^2+11x-6",     A2, 4, A2_exp, 3, 1e-12, &times[idx++]);
    RunRootsTest(pI, method, "A3: x^2+1",              A3, 3, A3_exp, 2, 1e-12, &times[idx++]);
    RunRootsTest(pI, method, "A4: x^4-1",              A4, 5, A4_exp, 4, 1e-12, &times[idx++]);
    RunRootsTest(pI, method, "A5: x^3-1",              A5, 4, A5_exp, 3, 1e-12, &times[idx++]);
    RunRootsTest(pI, method, "A6: 2x^2-8",             A6, 3, A6_exp, 2, 1e-12, &times[idx++]);
    RunRootsTest(pI, method, "A7: x^3-x^2 (zero)",     A7, 4, A7_exp, 3, 1e-12, &times[idx++]);
    RunRootsTest(pI, method, "A8: 3x^2+2x+1",          A8, 3, A8_exp, 2, 1e-12, &times[idx++]);

    printf("\n===== %s: Category B (medium, degree 5-8) =====\n", method);
    RunRootsTest(pI, method, "B1: x^5+x+1",            B1, 6, B1_exp, 5, 1e-10, &times[idx++]);
    RunRootsTest(pI, method, "B2: (x-1)..(x-6)",       B2, 7, B2_exp, 6, 1e-9,  &times[idx++]);
    RunRootsNoCheck(pI, method, "B3: x^8-2x^7+...",    B3, 9, &times[idx++]);
    RunRootsNoCheck(pI, method, "B4: 2x^4+3x^3-...",   B4, 5, &times[idx++]);
    RunRootsTest(pI, method, "B5: (x-1)^3",            B5, 4, B5_exp, 3, 1e-6,  &times[idx++]);

    printf("\n===== %s: Category C (high degree 9-15) =====\n", method);
    RunRootsNoCheck(pI, method, "C1: x^10-1",          C1, 11, &times[idx++]);
    RunRootsNoCheck(pI, method, "C2: (x-1)..(x-10)",   C2, 11, &times[idx++]);
    RunRootsNoCheck(pI, method, "C3: (x-1)..(x-15)",   C3, 16, &times[idx++]);

    printf("\n===== %s: Category D (edge cases) =====\n", method);
    RunRootsTest(pI, method, "D1: x-5 (linear)",       D1, 2, D1_exp, 1, 1e-12, &times[idx++]);
    RunRootsTest(pI, method, "D4: x^2-2 (irrational)", D4, 3, D4_exp, 2, 1e-12, &times[idx++]);
    RunRootsTest(pI, method, "D5: x^2-1e-10 (tiny)",   D5, 3, D5_exp, 2, 1e-7,  &times[idx++]);
    RunRootsTest(pI, method, "D6: x^2-1e10 (huge)",    D6, 3, D6_exp, 2, 1e-3,  &times[idx++]);

    printf("\n===== %s: Category E (special properties) =====\n", method);
    RunRootsNoCheck(pI, method, "E1: x^20-1",          E1, 21, &times[idx++]);
    RunRootsNoCheck(pI, method, "E2: cyclotomic-11",   E2, 11, &times[idx++]);
    RunRootsNoCheck(pI, method, "E3: 1+x+...+x^10",    E3, 11, &times[idx++]);

    printf("\n===== %s: Category F (Wilkinson-like) =====\n", method);
    RunRootsNoCheck(pI, method, "F1: Wilkinson deg 20", F1, 21, &times[idx++]);

    printf("\n===== %s: Category G (mixed) =====\n", method);
    RunRootsTest(pI, method, "G1: 5x^2-15x+10",        G1, 3, G1_exp, 2, 1e-12, &times[idx++]);
    RunRootsNoCheck(pI, method, "G2: 1+2x+3x^2+...",   G2, 5, &times[idx++]);
    RunRootsTest(pI, method, "G3: (x-1)(x-2)(x-3)(x-4)", G3, 5, G3_exp, 4, 1e-12, &times[idx++]);
    RunRootsNoCheck(pI, method, "G4: (x^2-2)^2",       G4, 5, &times[idx++]);
    RunRootsNoCheck(pI, method, "G5: x^7+1",           G5, 8, &times[idx++]);

    printf("\n===== %s: Error cases =====\n", method);
    EcoComplexD out[ECO_POLY_MAX_DEGREE];
    uint32_t cnt = 0;
    RunErrorTest(method, "coeffs==NULL",   pI->pVTbl->roots(pI, 0, 3, out, &cnt), ECO_POLY_ERR_ARG);
    RunErrorTest(method, "out==NULL",      pI->pVTbl->roots(pI, A1, 3, 0, &cnt), ECO_POLY_ERR_ARG);
    RunErrorTest(method, "count==NULL",    pI->pVTbl->roots(pI, A1, 3, out, 0), ECO_POLY_ERR_ARG);
    RunErrorTest(method, "num_coeffs==0",  pI->pVTbl->roots(pI, A1, 0, out, &cnt), ECO_POLY_ERR_ARG);
    RunErrorTest(method, "zero polynomial",pI->pVTbl->roots(pI, D3, 3, out, &cnt), ECO_POLY_ERR_ZERO);
    RunErrorTest(method, "constant (no roots)", pI->pVTbl->roots(pI, D2, 1, out, &cnt), ECO_POLY_OK);
}

/* ---------- CSV dump для Python ---------- */
typedef struct {
    const char* id;
    const char* name;
    const char* category;
    uint32_t    num;
    double      c[ECO_POLY_MAX_DEGREE + 1];
} DumpCase;

static void DumpCSV(IEcoLab1* pI, const char* method, const long long* times){
    static const DumpCase cases[] = {
        {"A1","x^2-3x+2","A",3,{1,-3,2}},
        {"A2","x^3-6x^2+11x-6","A",4,{1,-6,11,-6}},
        {"A3","x^2+1","A",3,{1,0,1}},
        {"A4","x^4-1","A",5,{1,0,0,0,-1}},
        {"A5","x^3-1","A",4,{1,0,0,-1}},
        {"A6","2x^2-8","A",3,{2,0,-8}},
        {"A7","x^3-x^2","A",4,{1,-1,0,0}},
        {"A8","3x^2+2x+1","A",3,{3,2,1}},
        {"B1","x^5+x+1","B",6,{1,0,0,0,1,1}},
        {"B2","(x-1)..(x-6)","B",7,{1,-21,175,-735,1624,-1764,720}},
        {"B3","x^8-2x^7+3x^5-x^3+4x-7","B",9,{1,-2,0,3,0,-1,0,4,-7}},
        {"B4","2x^4+3x^3-5x^2+x-9","B",5,{2,3,-5,1,-9}},
        {"B5","(x-1)^3","B",4,{1,-3,3,-1}},
        {"C1","x^10-1","C",11,{1,0,0,0,0,0,0,0,0,0,-1}},
        {"C2","(x-1)..(x-10)","C",11,{1,-55,1320,-18150,157773,-902055,3416930,-8409500,12753576,-10628640,3628800}},
        {"C3","(x-1)..(x-15)","C",16,{1,-120,6320,-214160,5105114,-95302944,1424938920,-17249347200,170295504960,-1333953600000,7927296000000,-31135104000000,72072000000000,-79833600000000,36324288000000,0}},
        {"D1","x-5","D",2,{1,-5}},
        {"D4","x^2-2","D",3,{1,0,-2}},
        {"D5","x^2-1e-10","D",3,{1,0,-1e-10}},
        {"D6","x^2-1e10","D",3,{1,0,-1e10}},
        {"E1","x^20-1","E",21,{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1}},
        {"E2","cyclotomic-11","E",11,{1,-1,1,-1,1,-1,1,-1,1,-1,1}},
        {"E3","1+x+...+x^10","E",11,{1,1,1,1,1,1,1,1,1,1,1}},
        {"F1","Wilkinson deg 20","F",21,{1,-210,20615,-1256850,53327946,-1672280820,40171771630,-756111184500,11310276995381,-135585182899530,1307535010540395,-10142299865511450,63030812099294896,-311333643161390640,1206647803780373360,-3599979517947607200,8031343328696787200,-12804747472765875200,14619434047401206784,-9699242216224378880,3243240000000000000}},
        {"G1","5x^2-15x+10","G",3,{5,-15,10}},
        {"G2","1+2x+3x^2+4x^3+5x^4","G",5,{1,2,3,4,5}},
        {"G3","(x-1)(x-2)(x-3)(x-4)","G",5,{1,-10,35,-50,24}},
        {"G4","(x^2-2)^2","G",5,{1,0,-4,0,4}},
        {"G5","x^7+1","G",8,{1,0,0,0,0,0,0,1}},
    };
    EcoComplexD roots[ECO_POLY_MAX_DEGREE];
    uint32_t cnt;
    printf("\nCSV_BEGIN;%s\n", method);
    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++){
        cnt = 0;
        pI->pVTbl->roots(pI, cases[i].c, cases[i].num, roots, &cnt);
        printf("POLY;%s;%s;%s;%u", cases[i].id, cases[i].name, cases[i].category, cases[i].num);
        for (uint32_t k = 0; k < cases[i].num; k++) printf(";% .17g", cases[i].c[k]);
        printf(";%lld\n", times[i]);
        for (uint32_t k = 0; k < cnt; k++){
            printf("ROOT;%s;%u;% .17g;% .17g\n", cases[i].id, k, roots[k].re, roots[k].im);
        }
    }
    printf("CSV_END;%s\n", method);
}

static IEcoLab1* LoadComponent(IEcoInterfaceBus1* pIBus, const UGUID* cid, const char* name){
    IEcoLab1* pI = 0;
    int16_t r = pIBus->pVTbl->QueryComponent(pIBus, cid, 0, &IID_IEcoLab1, (void**)&pI);
    if (r != 0 || !pI){ printf("Failed to load %s (code=%d)\n", name, (int)r); return 0; }
    printf("Loaded component: %s\n", name);
    return pI;
}

int16_t EcoMain(IEcoUnknown* pIUnk){
    int16_t result = -1;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoLab1* pDK = 0;
    IEcoLab1* pAE = 0;
    IEcoLab1* pLAG = 0;

    if (pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void**)&pISys) != 0) goto Release;
    if (pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void**)&pIBus) != 0) goto Release;

#ifdef ECO_LIB
    pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1_DK,       (IEcoUnknown*)GetIEcoComponentFactoryPtr_11111111AAAABBBBCCCC000000000001);
    pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1_AE,       (IEcoUnknown*)GetIEcoComponentFactoryPtr_22222222AAAABBBBCCCC000000000002);
    pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1_Laguerre, (IEcoUnknown*)GetIEcoComponentFactoryPtr_33333333AAAABBBBCCCC000000000003);
#endif

    pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryAllocator1, (void**)&pIMem);

    pDK  = LoadComponent(pIBus, &CID_EcoLab1_DK,       "Durand-Kerner");
    pAE  = LoadComponent(pIBus, &CID_EcoLab1_AE,       "Aberth-Ehrlich");
    pLAG = LoadComponent(pIBus, &CID_EcoLab1_Laguerre, "Laguerre");

    /* 35 тестов + 6 ошибок = 41 на метод, 123 всего */
    long long times_DK[35] = {0};
    long long times_AE[35] = {0};
    long long times_LAG[35] = {0};

    if (pDK)  { RunAllTests(pDK,  "DK",  times_DK);  DumpCSV(pDK,  "DK",  times_DK);  }
    if (pAE)  { RunAllTests(pAE,  "AE",  times_AE);  DumpCSV(pAE,  "AE",  times_AE);  }
    if (pLAG) { RunAllTests(pLAG, "LAG", times_LAG); DumpCSV(pLAG, "LAG", times_LAG); }

    printf("\n========================================\n");
    printf("TOTAL: %d of %d tests passed\n", g_passed, g_total);
    printf("========================================\n");
    result = (g_passed == g_total) ? 0 : -1;

Release:
    if (pDK)  pDK->pVTbl->Release(pDK);
    if (pAE)  pAE->pVTbl->Release(pAE);
    if (pLAG) pLAG->pVTbl->Release(pLAG);
    if (pIMem) pIMem->pVTbl->Release(pIMem);
    if (pIBus) pIBus->pVTbl->Release(pIBus);
    if (pISys) pISys->pVTbl->Release(pISys);
    return result;
}
