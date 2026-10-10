/* Durand-Kerner (Weierstrass) simultaneous root-finding.
 * z_k <- z_k - p(z_k) / prod_{j!=k} (z_k - z_j)
 * No libc, no malloc. All math in long double.
 */
#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoLab1.h"
#include "CEcoLab1Factory.h"

#define DK_MAX_ITER 2000
#define DK_TOL2      1.0e-30L
#define DK_TOL2_LOW  1.0e-8L
#define DK_IM_SNAP   1.0e-14L
#define DK_SORT_EQ   1.0e-9L

/* complex helpers (long double) */
static EcoComplexL DK_Make(long double re, long double im){ EcoComplexL r; r.re=re; r.im=im; return r; }
static EcoComplexL DK_Sub (EcoComplexL a, EcoComplexL b){ return DK_Make(a.re-b.re, a.im-b.im); }
static EcoComplexL DK_Mul (EcoComplexL a, EcoComplexL b){ return DK_Make(a.re*b.re - a.im*b.im, a.re*b.im + a.im*b.re); }
static long double DK_N2  (EcoComplexL a){ return a.re*a.re + a.im*a.im; }
static EcoComplexL DK_Div (EcoComplexL a, EcoComplexL b){
    long double d = DK_N2(b);
    return DK_Make((a.re*b.re + a.im*b.im)/d, (a.im*b.re - a.re*b.im)/d);
}
static long double DK_Abs(long double x){ return x < 0 ? -x : x; }

/* Horner for real coeffs c[0..n] at complex z */
static EcoComplexL DK_Horner(const long double* c, uint32_t n, EcoComplexL z){
    EcoComplexL p = DK_Make(c[0], 0.0L);
    for (uint32_t i = 1; i <= n; i++){ p = DK_Mul(p, z); p.re += c[i]; }
    return p;
}

static int DK_Before(EcoComplexL a, EcoComplexL b){
    long double t = DK_SORT_EQ * (1.0L + DK_Abs(a.re));
    if (DK_Abs(a.re - b.re) <= t) return a.im > b.im;
    return a.re > b.re;
}
static void DK_Sort(EcoComplexL* r, uint32_t n){
    for (uint32_t i = 1; i < n; i++){
        EcoComplexL k = r[i]; uint32_t j = i;
        while (j > 0 && DK_Before(k, r[j-1])){ r[j] = r[j-1]; j--; }
        r[j] = k;
    }
}

static int16_t DK_RootsCore(const long double* a, uint32_t m,
                            EcoComplexL* out, uint32_t* count){
    long double c[ECO_POLY_MAX_DEGREE + 1];
    EcoComplexL z[ECO_POLY_MAX_DEGREE];
    uint32_t start=0, end, nz=0, n, total=0, i, k, j, iter;
    long double bound=0, v, maxstep, step, zn;
    EcoComplexL w, p, d, delta, diff;
    int16_t status;

    if (!a || !out || !count || m==0) return ECO_POLY_ERR_ARG;
    if (m > ECO_POLY_MAX_DEGREE + 1) return ECO_POLY_ERR_DEGREE;
    *count = 0;

    while (start < m && a[start] == 0.0L) start++;
    if (start == m) return ECO_POLY_ERR_ZERO;

    end = m - 1;
    while (a[end] == 0.0L){ end--; nz++; }
    n = end - start;

    for (i = 0; i <= n; i++) c[i] = a[start + i] / a[start];

    for (i = 0; i < nz; i++) out[total++] = DK_Make(0.0L, 0.0L);

    status = ECO_POLY_OK;
    if (n == 1){ out[total++] = DK_Make(-c[1], 0.0L); }
    else if (n >= 2){
        for (i = 1; i <= n; i++){ v = DK_Abs(c[i]); if (v > bound) bound = v; }
        bound += 1.0L;
        w = DK_Make(0.4L, 0.9L);
        z[0] = DK_Make(bound, 0.0L);
        for (k = 1; k < n; k++) z[k] = DK_Mul(z[k-1], w);

        status = ECO_POLY_ERR_NOCONV;
        maxstep = 1.0L;
        for (iter = 0; iter < DK_MAX_ITER; iter++){
            maxstep = 0.0L;
            for (k = 0; k < n; k++){
                p = DK_Horner(c, n, z[k]);
                d = DK_Make(1.0L, 0.0L);
                for (j = 0; j < n; j++) if (j != k){
                    diff = DK_Sub(z[k], z[j]); d = DK_Mul(d, diff);
                }
                if (DK_N2(d) == 0.0L){
                    z[k].re += 1.0e-7L * (1.0L + DK_Abs(z[k].re));
                    z[k].im += 1.0e-7L;
                    maxstep = 1.0L; continue;
                }
                delta = DK_Div(p, d);
                z[k] = DK_Sub(z[k], delta);
                zn = 1.0L + DK_N2(z[k]);
                step = DK_N2(delta) / zn;
                if (step > maxstep) maxstep = step;
            }
            if (maxstep <= DK_TOL2){ status = ECO_POLY_OK; break; }
        }
        if (status != ECO_POLY_OK && maxstep <= DK_TOL2_LOW) status = ECO_POLY_WARN_LOWACC;
        if (status == ECO_POLY_ERR_NOCONV) return status;

        for (k = 0; k < n; k++){
            if (DK_Abs(z[k].im) <= DK_IM_SNAP * (1.0L + DK_Abs(z[k].re))) z[k].im = 0.0L;
            out[total++] = z[k];
        }
    }
    DK_Sort(out, total);
    *count = total;
    return status;
}

/* interface methods */
static int16_t ECOCALLMETHOD CEcoLab1_DK_rootsl(IEcoLab1Ptr_t me, const long double* c, uint32_t n, EcoComplexL* o, uint32_t* cnt){
    if (!me || !c || !o || !cnt || n==0) return ECO_POLY_ERR_ARG;
    return DK_RootsCore(c, n, o, cnt);
}
static int16_t ECOCALLMETHOD CEcoLab1_DK_roots(IEcoLab1Ptr_t me, const double* c, uint32_t n, EcoComplexD* o, uint32_t* cnt){
    long double a[ECO_POLY_MAX_DEGREE+1]; EcoComplexL r[ECO_POLY_MAX_DEGREE];
    uint32_t i, k=0; int16_t res;
    if (!me||!c||!o||!cnt||n==0) return ECO_POLY_ERR_ARG;
    if (n > ECO_POLY_MAX_DEGREE+1) return ECO_POLY_ERR_DEGREE;
    for (i=0;i<n;i++) a[i]=(long double)c[i];
    res = DK_RootsCore(a, n, r, &k);
    if (res >= 0){ for (i=0;i<k;i++){ o[i].re=(double)r[i].re; o[i].im=(double)r[i].im; } }
    *cnt = (res>=0)?k:0; return res;
}
static int16_t ECOCALLMETHOD CEcoLab1_DK_rootsf(IEcoLab1Ptr_t me, const float* c, uint32_t n, EcoComplexF* o, uint32_t* cnt){
    long double a[ECO_POLY_MAX_DEGREE+1]; EcoComplexL r[ECO_POLY_MAX_DEGREE];
    uint32_t i, k=0; int16_t res;
    if (!me||!c||!o||!cnt||n==0) return ECO_POLY_ERR_ARG;
    if (n > ECO_POLY_MAX_DEGREE+1) return ECO_POLY_ERR_DEGREE;
    for (i=0;i<n;i++) a[i]=(long double)c[i];
    res = DK_RootsCore(a, n, r, &k);
    if (res >= 0){ for (i=0;i<k;i++){ o[i].re=(float)r[i].re; o[i].im=(float)r[i].im; } }
    *cnt = (res>=0)?k:0; return res;
}

/* IEcoUnknown boilerplate */
static int16_t ECOCALLMETHOD CEcoLab1_DK_QI(IEcoLab1Ptr_t me, const UGUID* riid, void** ppv){
    CEcoLab1_18D23C4C* p=(CEcoLab1_18D23C4C*)me;
    if (!me||!ppv) return ERR_ECO_POINTER;
    if (IsEqualUGUID(riid,&IID_IEcoLab1)||IsEqualUGUID(riid,&IID_IEcoUnknown)){
        *ppv=&p->m_pVTblIEcoLab1; p->m_pVTblIEcoLab1->AddRef((IEcoLab1*)p); return ERR_ECO_SUCCESS;
    }
    *ppv=0; return ERR_ECO_NOINTERFACE;
}
static uint32_t ECOCALLMETHOD CEcoLab1_DK_AddRef(IEcoLab1Ptr_t me){
    CEcoLab1_18D23C4C* p=(CEcoLab1_18D23C4C*)me; if(!me) return -1; return ++p->m_cRef;
}
static uint32_t ECOCALLMETHOD CEcoLab1_DK_Release(IEcoLab1Ptr_t me){
    CEcoLab1_18D23C4C* p=(CEcoLab1_18D23C4C*)me; if(!me) return -1;
    if (--p->m_cRef == 0){ p->Delete(p); return 0; } return p->m_cRef;
}

/* init / create / delete */
static int16_t ECOCALLMETHOD initCEcoLab1_DK(CEcoLab1_18D23C4CPtr_t me, IEcoUnknownPtr_t pIUnkSystem){
    CEcoLab1_18D23C4C* p=(CEcoLab1_18D23C4C*)me;
    IEcoInterfaceBus1* pIBus=0; IEcoInterfaceBus1MemExt* pIMemExt=0;
    int16_t result=ERR_ECO_POINTER; UGUID* rcid=(UGUID*)&CID_EcoMemoryManager1;
    if (!me) return result;
    p->m_pISys=(IEcoSystem1*)pIUnkSystem;
    result=p->m_pISys->pVTbl->QueryInterface(p->m_pISys,&IID_IEcoInterfaceBus1,(void**)&pIBus);
    result=pIBus->pVTbl->QueryInterface(pIBus,&IID_IEcoInterfaceBus1MemExt,(void**)&pIMemExt);
    if (result==0 && pIMemExt){ rcid=(UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt); pIMemExt->pVTbl->Release(pIMemExt); }
    result=pIBus->pVTbl->QueryComponent(pIBus,rcid,0,&IID_IEcoMemoryAllocator1,(void**)&p->m_pIMem);
    if (result!=0||!p->m_pIMem) result=ERR_ECO_GET_MEMORY_ALLOCATOR;
    pIBus->pVTbl->Release(pIBus); return result;
}
static int16_t ECOCALLMETHOD createCEcoLab1_DK(CEcoLab1_18D23C4CPtr_t p, IEcoUnknownPtr_t s, IEcoUnknownPtr_t o){ return p?ERR_ECO_SUCCESS:ERR_ECO_POINTER; }
static void ECOCALLMETHOD deleteCEcoLab1_DK(CEcoLab1_18D23C4CPtr_t p){
    IEcoMemoryAllocator1* m=0; if(!p) return; m=p->m_pIMem;
    if (p->m_Name) m->pVTbl->Free(m,p->m_Name);
    if (p->m_pISys) p->m_pISys->pVTbl->Release(p->m_pISys);
    m->pVTbl->Free(m,p); m->pVTbl->Release(m);
}

/* VTable + instance */
static IEcoLab1VTbl g_VTbl_DK = {
    CEcoLab1_DK_QI, CEcoLab1_DK_AddRef, CEcoLab1_DK_Release,
    CEcoLab1_DK_roots, CEcoLab1_DK_rootsf, CEcoLab1_DK_rootsl
};
CEcoLab1_18D23C4C g_xCEcoLab1_DK = {
    &g_VTbl_DK, initCEcoLab1_DK, createCEcoLab1_DK, deleteCEcoLab1_DK,
    1, 0, 0, 0
};

/* Factory */
static IEcoComponentFactoryVTbl g_FVTbl_DK;
static CEcoLab1_18D23C4CFactory g_F_DK = { &g_FVTbl_DK, 0, "EcoLab1_DK\0", "1.0.0.0\0", "\0" };

static int16_t ECOCALLMETHOD F_DK_QI(IEcoComponentFactory* me, const UGUID* riid, void** ppv){
    if (IsEqualUGUID(riid,&IID_IEcoUnknown)||IsEqualUGUID(riid,&IID_IEcoComponentFactory)){
        *ppv=me;
        ((IEcoUnknown*)*ppv)->pVTbl->AddRef((IEcoUnknown*)*ppv);
        return ERR_ECO_SUCCESS;
    }
    *ppv=0;
    return ERR_ECO_NOINTERFACE;
}

static uint32_t ECOCALLMETHOD F_DK_AddRef(IEcoComponentFactory* me){
    CEcoLab1_18D23C4CFactory* p=(CEcoLab1_18D23C4CFactory*)me;
    if(!me) return -1;
    return ++p->m_cRef;
}

static uint32_t ECOCALLMETHOD F_DK_Release(IEcoComponentFactory* me){
    CEcoLab1_18D23C4CFactory* p=(CEcoLab1_18D23C4CFactory*)me;
    if(!me) return -1;
    if(--p->m_cRef==0) return 0;
    return p->m_cRef;
}

static int16_t ECOCALLMETHOD F_DK_Init(IEcoComponentFactory* me, IEcoUnknown* pIUnkSystem, void* pv){
    return me ? ERR_ECO_SUCCESS : ERR_ECO_POINTER;
}

static char_t* ECOCALLMETHOD F_DK_Name(IEcoComponentFactory* me){
    return ((CEcoLab1_18D23C4CFactory*)me)->m_Name;
}

static char_t* ECOCALLMETHOD F_DK_Ver(IEcoComponentFactory* me){
    return ((CEcoLab1_18D23C4CFactory*)me)->m_Version;
}

static char_t* ECOCALLMETHOD F_DK_Man(IEcoComponentFactory* me){
    return ((CEcoLab1_18D23C4CFactory*)me)->m_Manufacturer;
}

static int16_t ECOCALLMETHOD F_DK_Alloc(IEcoComponentFactory* me, IEcoUnknown* pISystem, IEcoUnknown* pIOuter, const UGUID* riid, void** ppv){
    CEcoLab1_18D23C4CFactory* pCMe=(CEcoLab1_18D23C4CFactory*)me;
    IEcoUnknown* pIUnk=0;
    int16_t result=ERR_ECO_POINTER;
    IEcoSystem1* pISys=0;
    IEcoInterfaceBus1* pIBus=0;
    IEcoInterfaceBus1MemExt* pIMemExt=0;
    IEcoMemoryAllocator1* pIMem=0;
    CEcoLab1_18D23C4C* pCObj=0;
    UGUID* rcid=(UGUID*)&CID_EcoMemoryManager1;
    
    if (!me||!pISystem) return result;
    if (pIOuter && !IsEqualUGUID(riid,&IID_IEcoUnknown)) return ERR_ECO_NOAGGREGATION;
    
    result=pISystem->pVTbl->QueryInterface(pISystem,&GID_IEcoSystem,(void**)&pISys);
    if (result!=0||!pISys) return ERR_ECO_NOSYSTEM;
    
    result=pISys->pVTbl->QueryInterface(pISys,&IID_IEcoInterfaceBus1,(void**)&pIBus);
    if (result!=0||!pIBus){
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_NOBUS;
    }
    
    result=pIBus->pVTbl->QueryInterface(pIBus,&IID_IEcoInterfaceBus1MemExt,(void**)&pIMemExt);
    if (result==0 && pIMemExt){
        rcid=(UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt);
        pIMemExt->pVTbl->Release(pIMemExt);
    }
    
    pIBus->pVTbl->QueryComponent(pIBus,rcid,0,&IID_IEcoMemoryAllocator1,(void**)&pIMem);
    if (result!=0||!pIMem){
        pIBus->pVTbl->Release(pIBus);
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_GET_MEMORY_ALLOCATOR;
    }
    
    pCObj=(CEcoLab1_18D23C4C*)pIMem->pVTbl->Alloc(pIMem,sizeof(CEcoLab1_18D23C4C));
    if (!pCObj){
        pIBus->pVTbl->Release(pIBus);
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_OUTOFMEMORY;
    }
    
    pCObj=(CEcoLab1_18D23C4C*)pIMem->pVTbl->Copy(pIMem,pCObj,&g_xCEcoLab1_DK,sizeof(CEcoLab1_18D23C4C));
    pCObj->Create(pCObj,pISystem,pIOuter);
    result=pCObj->Init(pCObj,pISystem);
    
    pIUnk=(IEcoUnknown*)pCObj;
    result=pIUnk->pVTbl->QueryInterface(pIUnk,riid,ppv);
    pIUnk->pVTbl->Release(pIUnk);
    
    return result;
}

static IEcoComponentFactoryVTbl g_FVTbl_DK = {
    F_DK_QI, F_DK_AddRef, F_DK_Release, F_DK_Alloc, F_DK_Init,
    F_DK_Name, F_DK_Ver, F_DK_Man
};

#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr_DK(void){
    return (IEcoComponentFactory*)&g_F_DK;
}
#elif ECO_LIB
IEcoComponentFactory* GetIEcoComponentFactoryPtr_11111111AAAABBBBCCCC000000000001 = (IEcoComponentFactory*)&g_F_DK;
#endif
