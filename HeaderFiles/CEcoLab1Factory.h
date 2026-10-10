#ifndef __C_ECOLAB1_FACTORY_H__
#define __C_ECOLAB1_FACTORY_H__

#include "IEcoSystem1.h"

typedef struct CEcoLab1_18D23C4CFactory {
    IEcoComponentFactoryVTbl* m_pVTblICF;
    uint32_t m_cRef;
    char_t m_Name[64];
    char_t m_Version[16];
    char_t m_Manufacturer[64];
} CEcoLab1_18D23C4CFactory;

#endif
