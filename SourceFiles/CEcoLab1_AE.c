/* Aberth-Ehrlich method.
 * W_k = p(z_k) / prod_{j!=k} (z_k - z_j)
 * z_k <- z_k - W_k / (1 - sum_{j!=k} W_k/(z_k - z_j))
 * Initial points are distributed uniformly on a circle of radius = Cauchy bound.
 */
#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoLab1.h"
#include "CEcoLab1Factory.h"

#define AE_MAX_ITER 3000
#define AE_TOL2     1.0e-28L
#define AE_TOL2_LOW 1.0e-8L
#define AE_IM_SNAP  1.0e-13L
#define AE_SORT_EQ  1.0e-9L
#define AE_PI       3.141592653589793238462643383279502884L

/* complex helpers */
static EcoComplexL AE_Make(long double re, long double im){ EcoComplexL r; r.re=re; r.im=im; return r; }
static EcoComplexL AE_Sub(EcoComplexL a, EcoComplexL b){ return AE_Make(a.re-b.re, a.im-b.im); }
static EcoComplexL AE_Add(EcoComplexL a, EcoComplexL b){ return AE_Make(a.re+b.re, a.im+b.im); }
static EcoComplexL AE_Mul(EcoComplexL a, EcoComplexL b){ return AE_Make(a.re*b.re-a.im*b.im, a.re*b.im+a.im*b.re); }
static EcoComplexL AE_Div(EcoComplexL a, EcoComplexL b){
    long double d = b.re*b.re + b.im*b.im;
    return AE_Make((a.re*b.re + a.im*b.im)/d, (a.im*b.re - a.re*b.im)/d);
}
static long double AE_N2(EcoComplexL a){ return a.re*a.re+a.im*a.im; }
static long double AE_Abs(long double x){ return x<0?-x:x; }

static EcoComplexL AE_Horner(const long double* c, uint32_t n, EcoComplexL z){
    EcoComplexL p = AE_Make(c[0], 0.0L);
    uint32_t i;
    for (i=1;i<=n;i++){ p = AE_Mul(p,z); p.re += c[i]; }
    return p;
}

/* cos/sin via Taylor series (no libc) */
static long double AE_ReduceAngle(long double a){
    while (a >  AE_PI) a -= 2.0L*AE_PI;
    while (a < -AE_PI) a += 2.0L*AE_PI;
    return a;
}
static long double AE_Cos(long double x){
    long double term, sum, x2; int i;
    x = AE_ReduceAngle(x);
    term = 1.0L; sum = 1.0L; x2 = x*x;
    for (i = 1; i < 40; i++){
        term *= -x2 / ((long double)(2*i-1) * (long double)(2*i));
        sum += term;
        if (AE_Abs(term) < 1.0e-22L) break;
    }
    return sum;
}
static long double AE_Sin(long double x){
    long double term, sum, x2; int i;
    x = AE_ReduceAngle(x);
    term = x; sum = x; x2 = x*x;
    for (i = 1; i < 40; i++){
        term *= -x2 / ((long double)(2*i) * (long double)(2*i+1));
        sum += term;
        if (AE_Abs(term) < 1.0e-22L) break;
    }
    return sum;
}

/* sorting */
static int AE_Before(EcoComplexL a, EcoComplexL b){
    long double t = AE_SORT_EQ*(1.0L+AE_Abs(a.re));
    if (AE_Abs(a.re-b.re) <= t) return a.im > b.im;
    return a.re > b.re;
}
static void AE_Sort(EcoComplexL* r, uint32_t n){
    uint32_t i, j; EcoComplexL k;
    for (i=1;i<n;i++){
        k=r[i]; j=i;
        while (j>0 && AE_Before(k,r[j-1])){ r[j]=r[j-1]; j--; }
        r[j]=k;
    }
}

/* core Aberth-Ehrlich */
static int16_t AE_RootsCore(const long double* a, uint32_t m,
                            EcoComplexL* out, uint32_t* count){
    long double c[ECO_POLY_MAX_DEGREE+1];
    EcoComplexL z[ECO_POLY_MAX_DEGREE];
    EcoComplexL W[ECO_POLY_MAX_DEGREE];
    uint32_t start=0, end, nz=0, n, total=0, i, k, j, iter;
    long double bound=0, v, maxstep, step, zn, angle;
    EcoComplexL p, d, delta, diff, sum, one;
    int16_t status;

    if (!a||!out||!count||m==0) return ECO_POLY_ERR_ARG;
    if (m > ECO_POLY_MAX_DEGREE+1) return ECO_POLY_ERR_DEGREE;
    *count = 0;

    /* Strip leading zeros */
    while (start<m && a[start]==0.0L) start++;
    if (start==m) return ECO_POLY_ERR_ZERO;
    /* Strip trailing zeros -> roots at 0 */
    end = m-1;
    while (a[end]==0.0L){ end--; nz++; }
    n = end-start;
    /* Normalize: c[0] = 1 */
    for (i=0;i<=n;i++) c[i] = a[start+i]/a[start];
    for (i=0;i<nz;i++) out[total++] = AE_Make(0.0L,0.0L);

    status = ECO_POLY_OK;
    if (n==1){ out[total++] = AE_Make(-c[1],0.0L); }
    else if (n>=2){
        /* Cauchy bound */
        for (i=1;i<=n;i++){ v=AE_Abs(c[i]); if (v>bound) bound=v; }
        bound += 1.0L;
        
        /* Initial points UNIFORMLY on circle of radius bound */
        for (k=0;k<n;k++){
            angle = 2.0L * AE_PI * (long double)k / (long double)n;
            z[k] = AE_Make(bound * AE_Cos(angle), bound * AE_Sin(angle));
        }

        status = ECO_POLY_ERR_NOCONV;
        one = AE_Make(1.0L, 0.0L);
        for (iter=0; iter<AE_MAX_ITER; iter++){
            maxstep = 0.0L;
            
            /* Weierstrass corrections W_k */
            for (k=0;k<n;k++){
                p = AE_Horner(c, n, z[k]);
                d = one;
                for (j=0;j<n;j++) if (j!=k){
                    diff = AE_Sub(z[k], z[j]);
                    if (AE_N2(diff) < 1.0e-30L){
                        /* Perturb coincident points */
                        z[k].re += 1.0e-6L * (1.0L + AE_Abs(z[k].re));
                        z[k].im += 1.0e-6L;
                        d = one;
                        break;
                    }
                    d = AE_Mul(d, diff);
                }
                if (AE_N2(d) < 1.0e-60L){
                    W[k] = AE_Make(0.0L, 0.0L);
                } else {
                    W[k] = AE_Div(p, d);
                }
            }
            
            /* Aberth update */
            for (k=0;k<n;k++){
                sum = AE_Make(0.0L, 0.0L);
                for (j=0;j<n;j++) if (j!=k){
                    diff = AE_Sub(z[k], z[j]);
                    if (AE_N2(diff) < 1.0e-30L) continue;
                    sum = AE_Add(sum, AE_Div(W[k], diff));
                }
                d = AE_Sub(one, sum);
                
                if (AE_N2(d) < 1.0e-20L){
                    /* Fallback to Weierstrass step */
                    delta = W[k];
                } else {
                    delta = AE_Div(W[k], d);
                }
                
                /* Damping: limit step size to avoid divergence */
                step = AE_N2(delta);
                if (step > 4.0L){
                    long double scale = 2.0L / (step > 100.0L ? 10.0L : (step > 16.0L ? 4.0L : 2.0L));
                    delta.re *= scale;
                    delta.im *= scale;
                    step = AE_N2(delta);
                }
                
                z[k] = AE_Sub(z[k], delta);
                zn = 1.0L + AE_N2(z[k]);
                step = step / zn;
                if (step > maxstep) maxstep = step;
            }
            
            if (maxstep <= AE_TOL2){ status = ECO_POLY_OK; break; }
        }
        if (status != ECO_POLY_OK && maxstep <= AE_TOL2_LOW) status = ECO_POLY_WARN_LOWACC;
        if (status == ECO_POLY_ERR_NOCONV) return status;

        for (k=0;k<n;k++){
            if (AE_Abs(z[k].im) <= AE_IM_SNAP*(1.0L+AE_Abs(z[k].re))) z[k].im = 0.0L;
            out[total++] = z[k];
        }
    }
    AE_Sort(out, total);
    *count = total;
    return status;
}

/* interface methods */
static int16_t ECOCALLMETHOD CEcoLab1_AE_rootsl(IEcoLab1Ptr_t me, const long double* c, uint32_t n, EcoComplexL* o, uint32_t* cnt){
    if (!me||!c||!o||!cnt||n==0) return ECO_POLY_ERR_ARG;
    return AE_RootsCore(c, n, o, cnt);
}
static int16_t ECOCALLMETHOD CEcoLab1_AE_roots(IEcoLab1Ptr_t me, const double* c, uint32_t n, EcoComplexD* o, uint32_t* cnt){
    long double a[ECO_POLY_MAX_DEGREE+1]; EcoComplexL r[ECO_POLY_MAX_DEGREE];
    uint32_t i, k=0; int16_t res;
    if (!me||!c||!o||!cnt||n==0) return ECO_POLY_ERR_ARG;
    if (n > ECO_POLY_MAX_DEGREE+1) return ECO_POLY_ERR_DEGREE;
    for (i=0;i<n;i++) a[i]=(long double)c[i];
    res = AE_RootsCore(a, n, r, &k);
    if (res>=0){ for (i=0;i<k;i++){ o[i].re=(double)r[i].re; o[i].im=(double)r[i].im; } }
    *cnt = (res>=0)?k:0; return res;
}
static int16_t ECOCALLMETHOD CEcoLab1_AE_rootsf(IEcoLab1Ptr_t me, const float* c, uint32_t n, EcoComplexF* o, uint32_t* cnt){
    long double a[ECO_POLY_MAX_DEGREE+1]; EcoComplexL r[ECO_POLY_MAX_DEGREE];
    uint32_t i, k=0; int16_t res;
    if (!me||!c||!o||!cnt||n==0) return ECO_POLY_ERR_ARG;
    if (n > ECO_POLY_MAX_DEGREE+1) return ECO_POLY_ERR_DEGREE;
    for (i=0;i<n;i++) a[i]=(long double)c[i];
    res = AE_RootsCore(a, n, r, &k);
    if (res>=0){ for (i=0;i<k;i++){ o[i].re=(float)r[i].re; o[i].im=(float)r[i].im; } }
    *cnt = (res>=0)?k:0; return res;
}

/* IEcoUnknown */
static int16_t ECOCALLMETHOD CEcoLab1_AE_QI(IEcoLab1Ptr_t me, const UGUID* riid, void** ppv){
    CEcoLab1_18D23C4C* p=(CEcoLab1_18D23C4C*)me;
    if (!me||!ppv) return ERR_ECO_POINTER;
    if (IsEqualUGUID(riid,&IID_IEcoLab1)||IsEqualUGUID(riid,&IID_IEcoUnknown)){
        *ppv=&p->m_pVTblIEcoLab1; p->m_pVTblIEcoLab1->AddRef((IEcoLab1*)p); return ERR_ECO_SUCCESS;
    }
    *ppv=0; return ERR_ECO_NOINTERFACE;
}
static uint32_t ECOCALLMETHOD CEcoLab1_AE_AddRef(IEcoLab1Ptr_t me){ CEcoLab1_18D23C4C* p=(CEcoLab1_18D23C4C*)me; if(!me) return -1; return ++p->m_cRef; }
static uint32_t ECOCALLMETHOD CEcoLab1_AE_Release(IEcoLab1Ptr_t me){ CEcoLab1_18D23C4C* p=(CEcoLab1_18D23C4C*)me; if(!me) return -1; if(--p->m_cRef==0){ p->Delete(p); return 0; } return p->m_cRef; }

/* init/create/delete */
static int16_t ECOCALLMETHOD initCEcoLab1_AE(CEcoLab1_18D23C4CPtr_t me, IEcoUnknownPtr_t pIUnkSystem){
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
static int16_t ECOCALLMETHOD createCEcoLab1_AE(CEcoLab1_18D23C4CPtr_t p, IEcoUnknownPtr_t s, IEcoUnknownPtr_t o){ return p?ERR_ECO_SUCCESS:ERR_ECO_POINTER; }
static void ECOCALLMETHOD deleteCEcoLab1_AE(CEcoLab1_18D23C4CPtr_t p){
    IEcoMemoryAllocator1* m=0; if(!p) return; m=p->m_pIMem;
    if (p->m_Name) m->pVTbl->Free(m,p->m_Name);
    if (p->m_pISys) p->m_pISys->pVTbl->Release(p->m_pISys);
    m->pVTbl->Free(m,p); m->pVTbl->Release(m);
}

static IEcoLab1VTbl g_VTbl_AE = {
    CEcoLab1_AE_QI, CEcoLab1_AE_AddRef, CEcoLab1_AE_Release,
    CEcoLab1_AE_roots, CEcoLab1_AE_rootsf, CEcoLab1_AE_rootsl
};
CEcoLab1_18D23C4C g_xCEcoLab1_AE = {
    &g_VTbl_AE, initCEcoLab1_AE, createCEcoLab1_AE, deleteCEcoLab1_AE,
    1, 0, 0, 0
};

/* Factory */
static IEcoComponentFactoryVTbl g_FVTbl_AE;
static CEcoLab1_18D23C4CFactory g_F_AE = { &g_FVTbl_AE, 0, "EcoLab1_AE\0", "1.0.0.0\0", "\0" };
static int16_t ECOCALLMETHOD F_AE_QI(IEcoComponentFactory* me, const UGUID* riid, void** ppv){
    if (IsEqualUGUID(riid,&IID_IEcoUnknown)||IsEqualUGUID(riid,&IID_IEcoComponentFactory)){
        *ppv = me; ((IEcoUnknown*)*ppv)->pVTbl->AddRef((IEcoUnknown*)*ppv); return ERR_ECO_SUCCESS;
    }
    *ppv = 0; return ERR_ECO_NOINTERFACE;
}
static uint32_t ECOCALLMETHOD F_AE_AddRef(IEcoComponentFactory* me){ CEcoLab1_18D23C4CFactory* p=(CEcoLab1_18D23C4CFactory*)me; if(!me) return -1; return ++p->m_cRef; }
static uint32_t ECOCALLMETHOD F_AE_Release(IEcoComponentFactory* me){ CEcoLab1_18D23C4CFactory* p=(CEcoLab1_18D23C4CFactory*)me; if(!me) return -1; if(--p->m_cRef==0) return 0; return p->m_cRef; }
static int16_t ECOCALLMETHOD F_AE_Init(IEcoComponentFactory* me, IEcoUnknown* pIUnkSystem, void* pv){ return me?ERR_ECO_SUCCESS:ERR_ECO_POINTER; }
static char_t* ECOCALLMETHOD F_AE_Name(IEcoComponentFactory* me){ return ((CEcoLab1_18D23C4CFactory*)me)->m_Name; }
static char_t* ECOCALLMETHOD F_AE_Ver(IEcoComponentFactory* me){ return ((CEcoLab1_18D23C4CFactory*)me)->m_Version; }
static char_t* ECOCALLMETHOD F_AE_Man(IEcoComponentFactory* me){ return ((CEcoLab1_18D23C4CFactory*)me)->m_Manufacturer; }
static int16_t ECOCALLMETHOD F_AE_Alloc(IEcoComponentFactory* me, IEcoUnknown* pISystem, IEcoUnknown* pIOuter, const UGUID* riid, void** ppv){
    CEcoLab1_18D23C4CFactory* pCMe=(CEcoLab1_18D23C4CFactory*)me;
    IEcoUnknown* pIUnk=0; int16_t result=ERR_ECO_POINTER;
    IEcoSystem1* pISys=0; IEcoInterfaceBus1* pIBus=0;
    IEcoInterfaceBus1MemExt* pIMemExt=0; IEcoMemoryAllocator1* pIMem=0;
    CEcoLab1_18D23C4C* pCObj=0; UGUID* rcid=(UGUID*)&CID_EcoMemoryManager1;
    if (!me||!pISystem) return result;
    if (pIOuter && !IsEqualUGUID(riid,&IID_IEcoUnknown)) return ERR_ECO_NOAGGREGATION;
    result=pISystem->pVTbl->QueryInterface(pISystem,&GID_IEcoSystem,(void**)&pISys);
    if (result!=0||!pISys) return ERR_ECO_NOSYSTEM;
    result=pISys->pVTbl->QueryInterface(pISys,&IID_IEcoInterfaceBus1,(void**)&pIBus);
    if (result!=0||!pIBus){ pISys->pVTbl->Release(pISys); return ERR_ECO_NOBUS; }
    result=pIBus->pVTbl->QueryInterface(pIBus,&IID_IEcoInterfaceBus1MemExt,(void**)&pIMemExt);
    if (result==0 && pIMemExt){ rcid=(UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt); pIMemExt->pVTbl->Release(pIMemExt); }
    pIBus->pVTbl->QueryComponent(pIBus,rcid,0,&IID_IEcoMemoryAllocator1,(void**)&pIMem);
    if (result!=0||!pIMem){ pIBus->pVTbl->Release(pIBus); pISys->pVTbl->Release(pISys); return ERR_ECO_GET_MEMORY_ALLOCATOR; }
    pCObj=(CEcoLab1_18D23C4C*)pIMem->pVTbl->Alloc(pIMem,sizeof(CEcoLab1_18D23C4C));
    if (!pCObj){ pIBus->pVTbl->Release(pIBus); pISys->pVTbl->Release(pISys); return ERR_ECO_OUTOFMEMORY; }
    pCObj=(CEcoLab1_18D23C4C*)pIMem->pVTbl->Copy(pIMem,pCObj,&g_xCEcoLab1_AE,sizeof(CEcoLab1_18D23C4C));
    pCObj->Create(pCObj,pISystem,pIOuter);
    result=pCObj->Init(pCObj,pISystem);
    pIUnk=(IEcoUnknown*)pCObj;
    result=pIUnk->pVTbl->QueryInterface(pIUnk,riid,ppv);
    pIUnk->pVTbl->Release(pIUnk);
    return result;
}
static IEcoComponentFactoryVTbl g_FVTbl_AE = {
    F_AE_QI, F_AE_AddRef, F_AE_Release, F_AE_Alloc, F_AE_Init,
    F_AE_Name, F_AE_Ver, F_AE_Man
};
#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr_AE(void){ return (IEcoComponentFactory*)&g_F_AE; }
#elif ECO_LIB
IEcoComponentFactory* GetIEcoComponentFactoryPtr_22222222AAAABBBBCCCC000000000002 = (IEcoComponentFactory*)&g_F_AE;
#endif
