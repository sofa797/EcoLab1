#ifndef __C_ECOLAB1_H__
#define __C_ECOLAB1_H__

#include "IEcoLab1.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CEcoLab1_18D23C4C* CEcoLab1_18D23C4CPtr_t;

typedef struct CEcoLab1_18D23C4C {
    IEcoLab1VTbl* m_pVTblIEcoLab1;

    int16_t (ECOCALLMETHOD *Init)(CEcoLab1_18D23C4CPtr_t pCMe, IEcoUnknownPtr_t pIUnkSystem);
    int16_t (ECOCALLMETHOD *Create)(CEcoLab1_18D23C4CPtr_t pCMe, IEcoUnknownPtr_t pIUnkSystem, IEcoUnknownPtr_t pIUnkOuter);
    void    (ECOCALLMETHOD *Delete)(CEcoLab1_18D23C4CPtr_t pCMe);

    uint32_t m_cRef;
    IEcoMemoryAllocator1* m_pIMem;
    IEcoSystem1*          m_pISys;
    char_t* m_Name;
} CEcoLab1_18D23C4C;

#endif
