/* Laguerre method with synthetic deflation.
 * G = p'(z)/p(z),  H = G^2 - p''(z)/p(z)
 * denom = G +/- sqrt((n-1)*(n*H - G^2))   (sign: larger |denom|)
 * z <- z - n / denom
 * After each root is found, polish it with Newton on the ORIGINAL polynomial.
 */
#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoLab1.h"
#include "CEcoLab1Factory.h"

#define LAG_MAX_ITER 2000
#define LAG_TOL      1.0e-14L
#define LAG_IM_SNAP  1.0e-13L
#define LAG_SORT_EQ  1.0e-9L
#define LAG_PI       3.141592653589793238462643383279502884L

/* complex helpers */
static EcoComplexL LAG_Make(long double re, long double im){ EcoComplexL r; r.re=re; r.im=im; return r; }
static EcoComplexL LAG_Sub(EcoComplexL a, EcoComplexL b){ return LAG_Make(a.re-b.re, a.im-b.im); }
static EcoComplexL LAG_Add(EcoComplexL a, EcoComplexL b){ return LAG_Make(a.re+b.re, a.im+b.im); }
static EcoComplexL LAG_Mul(EcoComplexL a, EcoComplexL b){ return LAG_Make(a.re*b.re-a.im*b.im, a.re*b.im+a.im*b.re); }
static EcoComplexL LAG_Div(EcoComplexL a, EcoComplexL b){
    long double d = b.re*b.re + b.im*b.im;
    return LAG_Make((a.re*b.re+a.im*b.im)/d, (a.im*b.re-a.re*b.im)/d);
}
static long double LAG_N2(EcoComplexL a){ return a.re*a.re+a.im*a.im; }
static long double LAG_Abs(long double x){ return x<0?-x:x; }
static EcoComplexL LAG_Scale(EcoComplexL a, long double s){ return LAG_Make(a.re*s, a.im*s); }

/* sqrt for complex via algebraic formula (no trig needed):
 *   sqrt(a+bi) = ( sqrt((|z|+a)/2) , sign(b)*sqrt((|z|-a)/2) )
 * where |z| = sqrt(a^2 + b^2) computed by Newton.
 */
static long double LAG_RealSqrt(long double x){
    long double g, ng; int i;
    if (x <= 0.0L) return 0.0L;
    g = x * 0.5L;
    for (i = 0; i < 80; i++){
        if (g == 0.0L) break;
        ng = 0.5L * (g + x / g);
        if (ng == g) break;
        g = ng;
    }
    return g;
}
static EcoComplexL LAG_CSqrt(EcoComplexL z){
    long double a = z.re, b = z.im;
    long double r = LAG_RealSqrt(a*a + b*b);   /* |z| */
    long double t1 = r + a;                    /* |z| + a >= 0 */
    long double t2 = r - a;                    /* |z| - a >= 0 */
    if (t1 < 0.0L) t1 = 0.0L;
    if (t2 < 0.0L) t2 = 0.0L;
    long double re = LAG_RealSqrt(t1 * 0.5L);
    long double im = LAG_RealSqrt(t2 * 0.5L);
    if (b < 0.0L) im = -im;
    return LAG_Make(re, im);
}

/* Evaluate p, p', p'' at z via Horner-like scheme (real coeffs c[0..n]) */
static void LAG_Eval(const long double* c, uint32_t n, EcoComplexL z,
                     EcoComplexL* pp, EcoComplexL* pd, EcoComplexL* pdd){
    EcoComplexL p  = LAG_Make(c[0], 0.0L);
    EcoComplexL pd_ = LAG_Make(0.0L, 0.0L);
    EcoComplexL pdd_= LAG_Make(0.0L, 0.0L);
    uint32_t i;
    for (i=1;i<=n;i++){
        pdd_ = LAG_Add(LAG_Mul(pdd_, z), LAG_Scale(pd_, 2.0L));
        pd_  = LAG_Add(LAG_Mul(pd_,  z), p);
        p    = LAG_Add(LAG_Mul(p,    z), LAG_Make(c[i], 0.0L));
    }
    *pp = p; *pd = pd_; *pdd = pdd_;
}

/* synthetic division by (x - root): c[0..n] -> q[0..n-1] */
static void LAG_Deflate(const long double* c, uint32_t n, EcoComplexL root, long double* q){
    EcoComplexL acc = LAG_Make(c[0], 0.0L);
    uint32_t i;
    q[0] = acc.re;
    for (i=1;i<n;i++){
        acc = LAG_Add(LAG_Mul(acc, root), LAG_Make(c[i], 0.0L));
        q[i] = acc.re;
    }
}

/* cos/sin via Taylor (for initial points on circle) */
static long double LAG_ReduceAngle(long double a){
    while (a >  LAG_PI) a -= 2.0L*LAG_PI;
    while (a < -LAG_PI) a += 2.0L*LAG_PI;
    return a;
}
static long double LAG_Cos(long double x){
    long double term, sum, x2; int i;
    x = LAG_ReduceAngle(x);
    term = 1.0L; sum = 1.0L; x2 = x*x;
    for (i = 1; i < 40; i++){
        term *= -x2 / ((long double)(2*i-1) * (long double)(2*i));
        sum += term;
        if (LAG_Abs(term) < 1.0e-22L) break;
    }
    return sum;
}
static long double LAG_Sin(long double x){
    long double term, sum, x2; int i;
    x = LAG_ReduceAngle(x);
    term = x; sum = x; x2 = x*x;
    for (i = 1; i < 40; i++){
        term *= -x2 / ((long double)(2*i) * (long double)(2*i+1));
        sum += term;
        if (LAG_Abs(term) < 1.0e-22L) break;
    }
    return sum;
}

static int LAG_Before(EcoComplexL a, EcoComplexL b){
    long double t = LAG_SORT_EQ*(1.0L+LAG_Abs(a.re));
    if (LAG_Abs(a.re-b.re) <= t) return a.im > b.im;
    return a.re > b.re;
}
static void LAG_Sort(EcoComplexL* r, uint32_t n){
    uint32_t i, j; EcoComplexL k;
    for (i=1;i<n;i++){
        k=r[i]; j=i;
        while (j>0 && LAG_Before(k,r[j-1])){ r[j]=r[j-1]; j--; }
        r[j]=k;
    }
}

/* Polish root by Newton iterations on the ORIGINAL polynomial */
static EcoComplexL LAG_Polish(const long double* orig, uint32_t origN, EcoComplexL z){
    int i;
    for (i=0;i<30;i++){
        EcoComplexL p, pd, pdd;
        LAG_Eval(orig, origN, z, &p, &pd, &pdd);
        if (LAG_N2(pd) < 1.0e-60L) break;
        EcoComplexL dz = LAG_Div(p, pd);
        z = LAG_Sub(z, dz);
        if (LAG_N2(dz) < 1.0e-30L * (1.0L + LAG_N2(z))) break;
    }
    return z;
}

/* core Laguerre */
static int16_t LAG_RootsCore(const long double* a, uint32_t m,
                             EcoComplexL* out, uint32_t* count){
    long double c[ECO_POLY_MAX_DEGREE+1];
    long double orig[ECO_POLY_MAX_DEGREE+1];   /* original normalized coeffs */
    long double q[ECO_POLY_MAX_DEGREE+1];
    EcoComplexL z, G, H, sq, denom1, denom2, delta, p, pd, pdd;
    uint32_t start=0, end, nz=0, n, total=0, i, iter, curN, r;
    long double bound=0, v, abs1, abs2, angle;
    int16_t status;

    if (!a||!out||!count||m==0) return ECO_POLY_ERR_ARG;
    if (m > ECO_POLY_MAX_DEGREE+1) return ECO_POLY_ERR_DEGREE;
    *count = 0;

    while (start<m && a[start]==0.0L) start++;
    if (start==m) return ECO_POLY_ERR_ZERO;
    end = m-1;
    while (a[end]==0.0L){ end--; nz++; }
    n = end-start;
    for (i=0;i<=n;i++) c[i] = a[start+i]/a[start];
    /* Save original for polishing */
    for (i=0;i<=n;i++) orig[i] = c[i];
    for (i=0;i<nz;i++) out[total++] = LAG_Make(0.0L,0.0L);

    status = ECO_POLY_OK;
    if (n==1){ out[total++] = LAG_Make(-c[1],0.0L); return status; }

    /* Cauchy bound */
    for (i=1;i<=n;i++){ v=LAG_Abs(c[i]); if (v>bound) bound=v; }
    bound += 1.0L;

    /* Main deflation loop */
    curN = n;
    for (r=0; r<n; r++){
        /* Initial guess: point on circle of radius 'bound' at angle 2*pi*r/n + small offset */
        angle = 2.0L * LAG_PI * (long double)r / (long double)n + 0.3L;
        z = LAG_Make(bound * LAG_Cos(angle), bound * LAG_Sin(angle));

        status = ECO_POLY_ERR_NOCONV;
        for (iter=0; iter<LAG_MAX_ITER; iter++){
            LAG_Eval(c, curN, z, &p, &pd, &pdd);
            if (LAG_N2(p) < 1.0e-30L * (1.0L + LAG_N2(z))){ status = ECO_POLY_OK; break; }
            if (LAG_N2(pd) < 1.0e-60L){
                z.re += 1.0e-5L * (1.0L + LAG_Abs(z.re));
                z.im += 1.0e-5L;
                continue;
            }
            G = LAG_Div(pd, p);
            H = LAG_Sub(LAG_Mul(G, G), LAG_Div(pdd, p));
            {
                long double cn = (long double)curN;
                EcoComplexL inner = LAG_Sub(
                    LAG_Scale(H, cn*(cn-1.0L)),
                    LAG_Scale(LAG_Mul(G,G), (cn-1.0L))
                );
                sq = LAG_CSqrt(inner);
                denom1 = LAG_Add(G, sq);
                denom2 = LAG_Sub(G, sq);
                abs1 = LAG_N2(denom1); abs2 = LAG_N2(denom2);
                EcoComplexL denom = (abs1 >= abs2) ? denom1 : denom2;
                if (LAG_N2(denom) < 1.0e-60L){
                    z.re += 1.0e-5L; z.im += 1.0e-5L;
                    continue;
                }
                delta = LAG_Div(LAG_Make(cn, 0.0L), denom);
            }
            z = LAG_Sub(z, delta);
            if (LAG_N2(delta) < LAG_TOL*LAG_TOL * (1.0L + LAG_N2(z))){ status = ECO_POLY_OK; break; }
        }
        if (status != ECO_POLY_OK) return status;

        /* Polish against ORIGINAL polynomial */
        z = LAG_Polish(orig, n, z);
        if (LAG_Abs(z.im) <= LAG_IM_SNAP*(1.0L+LAG_Abs(z.re))) z.im = 0.0L;
        out[total++] = z;

        /* Deflate */
        if (curN == 1) break;
        LAG_Deflate(c, curN, z, q);
        for (i=0;i<curN;i++) c[i] = q[i];
        curN--;
    }

    LAG_Sort(out, total);
    *count = total;
    return status;
}

/* interface methods */
static int16_t ECOCALLMETHOD CEcoLab1_LAG_rootsl(IEcoLab1Ptr_t me, const long double* c, uint32_t n, EcoComplexL* o, uint32_t* cnt){
    if (!me||!c||!o||!cnt||n==0) return ECO_POLY_ERR_ARG;
    return LAG_RootsCore(c, n, o, cnt);
}
static int16_t ECOCALLMETHOD CEcoLab1_LAG_roots(IEcoLab1Ptr_t me, const double* c, uint32_t n, EcoComplexD* o, uint32_t* cnt){
    long double a[ECO_POLY_MAX_DEGREE+1]; EcoComplexL r[ECO_POLY_MAX_DEGREE];
    uint32_t i, k=0; int16_t res;
    if (!me||!c||!o||!cnt||n==0) return ECO_POLY_ERR_ARG;
    if (n > ECO_POLY_MAX_DEGREE+1) return ECO_POLY_ERR_DEGREE;
    for (i=0;i<n;i++) a[i]=(long double)c[i];
    res = LAG_RootsCore(a, n, r, &k);
    if (res>=0){ for (i=0;i<k;i++){ o[i].re=(double)r[i].re; o[i].im=(double)r[i].im; } }
    *cnt = (res>=0)?k:0; return res;
}
static int16_t ECOCALLMETHOD CEcoLab1_LAG_rootsf(IEcoLab1Ptr_t me, const float* c, uint32_t n, EcoComplexF* o, uint32_t* cnt){
    long double a[ECO_POLY_MAX_DEGREE+1]; EcoComplexL r[ECO_POLY_MAX_DEGREE];
    uint32_t i, k=0; int16_t res;
    if (!me||!c||!o||!cnt||n==0) return ECO_POLY_ERR_ARG;
    if (n > ECO_POLY_MAX_DEGREE+1) return ECO_POLY_ERR_DEGREE;
    for (i=0;i<n;i++) a[i]=(long double)c[i];
    res = LAG_RootsCore(a, n, r, &k);
    if (res>=0){ for (i=0;i<k;i++){ o[i].re=(float)r[i].re; o[i].im=(float)r[i].im; } }
    *cnt = (res>=0)?k:0; return res;
}

/* IEcoUnknown */
static int16_t ECOCALLMETHOD CEcoLab1_LAG_QI(IEcoLab1Ptr_t me, const UGUID* riid, void** ppv){
    CEcoLab1_18D23C4C* p=(CEcoLab1_18D23C4C*)me;
    if (!me||!ppv) return ERR_ECO_POINTER;
    if (IsEqualUGUID(riid,&IID_IEcoLab1)||IsEqualUGUID(riid,&IID_IEcoUnknown)){
        *ppv=&p->m_pVTblIEcoLab1; p->m_pVTblIEcoLab1->AddRef((IEcoLab1*)p); return ERR_ECO_SUCCESS;
    }
    *ppv=0; return ERR_ECO_NOINTERFACE;
}
static uint32_t ECOCALLMETHOD CEcoLab1_LAG_AddRef(IEcoLab1Ptr_t me){ CEcoLab1_18D23C4C* p=(CEcoLab1_18D23C4C*)me; if(!me) return -1; return ++p->m_cRef; }
static uint32_t ECOCALLMETHOD CEcoLab1_LAG_Release(IEcoLab1Ptr_t me){ CEcoLab1_18D23C4C* p=(CEcoLab1_18D23C4C*)me; if(!me) return -1; if(--p->m_cRef==0){ p->Delete(p); return 0; } return p->m_cRef; }

/* init/create/delete */
static int16_t ECOCALLMETHOD initCEcoLab1_LAG(CEcoLab1_18D23C4CPtr_t me, IEcoUnknownPtr_t pIUnkSystem){
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
static int16_t ECOCALLMETHOD createCEcoLab1_LAG(CEcoLab1_18D23C4CPtr_t p, IEcoUnknownPtr_t s, IEcoUnknownPtr_t o){ return p?ERR_ECO_SUCCESS:ERR_ECO_POINTER; }
static void ECOCALLMETHOD deleteCEcoLab1_LAG(CEcoLab1_18D23C4CPtr_t p){
    IEcoMemoryAllocator1* m=0; if(!p) return; m=p->m_pIMem;
    if (p->m_Name) m->pVTbl->Free(m,p->m_Name);
    if (p->m_pISys) p->m_pISys->pVTbl->Release(p->m_pISys);
    m->pVTbl->Free(m,p); m->pVTbl->Release(m);
}

static IEcoLab1VTbl g_VTbl_LAG = {
    CEcoLab1_LAG_QI, CEcoLab1_LAG_AddRef, CEcoLab1_LAG_Release,
    CEcoLab1_LAG_roots, CEcoLab1_LAG_rootsf, CEcoLab1_LAG_rootsl
};
CEcoLab1_18D23C4C g_xCEcoLab1_LAG = {
    &g_VTbl_LAG, initCEcoLab1_LAG, createCEcoLab1_LAG, deleteCEcoLab1_LAG,
    1, 0, 0, 0
};

/* Factory */
static IEcoComponentFactoryVTbl g_FVTbl_LAG;
static CEcoLab1_18D23C4CFactory g_F_LAG = { &g_FVTbl_LAG, 0, "EcoLab1_Laguerre\0", "1.0.0.0\0", "\0" };
static int16_t ECOCALLMETHOD F_LAG_QI(IEcoComponentFactory* me, const UGUID* riid, void** ppv){
    if (IsEqualUGUID(riid,&IID_IEcoUnknown)||IsEqualUGUID(riid,&IID_IEcoComponentFactory)){
        *ppv = me; ((IEcoUnknown*)*ppv)->pVTbl->AddRef((IEcoUnknown*)*ppv); return ERR_ECO_SUCCESS;
    }
    *ppv = 0; return ERR_ECO_NOINTERFACE;
}
static uint32_t ECOCALLMETHOD F_LAG_AddRef(IEcoComponentFactory* me){ CEcoLab1_18D23C4CFactory* p=(CEcoLab1_18D23C4CFactory*)me; if(!me) return -1; return ++p->m_cRef; }
static uint32_t ECOCALLMETHOD F_LAG_Release(IEcoComponentFactory* me){ CEcoLab1_18D23C4CFactory* p=(CEcoLab1_18D23C4CFactory*)me; if(!me) return -1; if(--p->m_cRef==0) return 0; return p->m_cRef; }
static int16_t ECOCALLMETHOD F_LAG_Init(IEcoComponentFactory* me, IEcoUnknown* pIUnkSystem, void* pv){ return me?ERR_ECO_SUCCESS:ERR_ECO_POINTER; }
static char_t* ECOCALLMETHOD F_LAG_Name(IEcoComponentFactory* me){ return ((CEcoLab1_18D23C4CFactory*)me)->m_Name; }
static char_t* ECOCALLMETHOD F_LAG_Ver(IEcoComponentFactory* me){ return ((CEcoLab1_18D23C4CFactory*)me)->m_Version; }
static char_t* ECOCALLMETHOD F_LAG_Man(IEcoComponentFactory* me){ return ((CEcoLab1_18D23C4CFactory*)me)->m_Manufacturer; }
static int16_t ECOCALLMETHOD F_LAG_Alloc(IEcoComponentFactory* me, IEcoUnknown* pISystem, IEcoUnknown* pIOuter, const UGUID* riid, void** ppv){
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
    pCObj=(CEcoLab1_18D23C4C*)pIMem->pVTbl->Copy(pIMem,pCObj,&g_xCEcoLab1_LAG,sizeof(CEcoLab1_18D23C4C));
    pCObj->Create(pCObj,pISystem,pIOuter);
    result=pCObj->Init(pCObj,pISystem);
    pIUnk=(IEcoUnknown*)pCObj;
    result=pIUnk->pVTbl->QueryInterface(pIUnk,riid,ppv);
    pIUnk->pVTbl->Release(pIUnk);
    return result;
}
static IEcoComponentFactoryVTbl g_FVTbl_LAG = {
    F_LAG_QI, F_LAG_AddRef, F_LAG_Release, F_LAG_Alloc, F_LAG_Init,
    F_LAG_Name, F_LAG_Ver, F_LAG_Man
};
#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr_Laguerre(void){ return (IEcoComponentFactory*)&g_F_LAG; }
#elif ECO_LIB
IEcoComponentFactory* GetIEcoComponentFactoryPtr_33333333AAAABBBBCCCC000000000003 = (IEcoComponentFactory*)&g_F_LAG;
#endif
