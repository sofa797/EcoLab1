/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   IdEcoPolyLaguerre
 * </summary>
 *
 * <description>
 *   This header describes the component identifier (CID) of EcoPolyLaguerre
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */

#ifndef __ID_ECOPOLYLAGUERRE_H__
#define __ID_ECOPOLYLAGUERRE_H__

#include "IEcoBase1.h"
#include "IEcoPolynomialMath.h"

/* EcoPolyLaguerre CID = {9EEDAACC-3D71-44FA-A560-8DB94D0499F1} */
#ifndef __CID_EcoPolyLaguerre
static const UGUID CID_EcoPolyLaguerre = {0x01, 0x10, {0x9E, 0xED, 0xAA, 0xCC, 0x3D, 0x71, 0x44, 0xFA, 0xA5, 0x60, 0x8D, 0xB9, 0x4D, 0x04, 0x99, 0xF1}};
#endif /* __CID_EcoPolyLaguerre */

/* Component factory (static layout) */
#ifdef ECO_LIB
extern IEcoComponentFactory* GetIEcoComponentFactoryPtr_9EEDAACC3D7144FAA5608DB94D0499F1;
#endif

#endif /* __ID_ECOPOLYLAGUERRE_H__ */
