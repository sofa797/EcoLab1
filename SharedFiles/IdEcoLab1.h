#ifndef __ID_ECOLAB1_H__
#define __ID_ECOLAB1_H__

#include "IEcoBase1.h"
#include "IEcoLab1.h"

/* CID Durand-Kerner  = {11111111-AAAA-BBBB-CCCC-000000000001} */
#ifndef __CID_EcoLab1_DK
static const UGUID CID_EcoLab1_DK = {0x01, 0x10,
    {0x11,0x11,0x11,0x11,0xAA,0xAA,0xBB,0xBB,
     0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x01}};
#endif

/* CID Aberth-Ehrlich = {22222222-AAAA-BBBB-CCCC-000000000002} */
#ifndef __CID_EcoLab1_AE
static const UGUID CID_EcoLab1_AE = {0x01, 0x10,
    {0x22,0x22,0x22,0x22,0xAA,0xAA,0xBB,0xBB,
     0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x02}};
#endif

/* CID Laguerre       = {33333333-AAAA-BBBB-CCCC-000000000003} */
#ifndef __CID_EcoLab1_Laguerre
static const UGUID CID_EcoLab1_Laguerre = {0x01, 0x10,
    {0x33,0x33,0x33,0x33,0xAA,0xAA,0xBB,0xBB,
     0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,0x03}};
#endif

#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory ECOCALLMETHOD GetIEcoComponentFactoryPtr_DK(void);
ECO_EXPORT IEcoComponentFactory ECOCALLMETHOD GetIEcoComponentFactoryPtr_AE(void);
ECO_EXPORT IEcoComponentFactory ECOCALLMETHOD GetIEcoComponentFactoryPtr_Laguerre(void);
#elif ECO_LIB
extern IEcoComponentFactory* GetIEcoComponentFactoryPtr_11111111AAAABBBBCCCC000000000001;
extern IEcoComponentFactory* GetIEcoComponentFactoryPtr_22222222AAAABBBBCCCC000000000002;
extern IEcoComponentFactory* GetIEcoComponentFactoryPtr_33333333AAAABBBBCCCC000000000003;
#endif

#endif
