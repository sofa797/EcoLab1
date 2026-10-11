/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   IdEcoLab1
 * </summary>
 *
 * <description>
 *   This header describes the interface IdEcoLab1
 * </description>
 *
 * <reference>
 *
 * </reference>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */

#ifndef __ID_ECOLAB1_H__
#define __ID_ECOLAB1_H__

#include "IEcoBase1.h"
#include "IEcoPolynomialMath.h"

/* EcoLab1 CID = {011BF193-DAAC-48E0-B83B-3A8618D23C4C} */
#ifndef __CID_EcoLab1
static const UGUID CID_EcoLab1 = {0x01, 0x10, {0x01, 0x1B, 0xF1, 0x93, 0xDA, 0xAC, 0x48, 0xE0, 0xB8, 0x3B, 0x3A, 0x86, 0x18, 0xD2, 0x3C, 0x4C}};
#endif /* __CID_EcoLab1 */

/* Component factory for dynamic and static layout */
#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr();
#elif ECO_LIB
extern IEcoComponentFactory* GetIEcoComponentFactoryPtr_011BF193DAAC48E0B83B3A8618D23C4C;
#endif

#endif /* __ID_ECOLAB1_H__ */
