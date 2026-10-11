/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   IdEcoPolyAberth
 * </summary>
 *
 * <description>
 *   This header describes the component identifier (CID) of EcoPolyAberth
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */

#ifndef __ID_ECOPOLYABERTH_H__
#define __ID_ECOPOLYABERTH_H__

#include "IEcoBase1.h"
#include "IEcoPolynomialMath.h"

/* EcoPolyAberth CID = {C1642CE5-94A7-40DE-8F79-D08B659EAEED} */
#ifndef __CID_EcoPolyAberth
static const UGUID CID_EcoPolyAberth = {0x01, 0x10, {0xC1, 0x64, 0x2C, 0xE5, 0x94, 0xA7, 0x40, 0xDE, 0x8F, 0x79, 0xD0, 0x8B, 0x65, 0x9E, 0xAE, 0xED}};
#endif /* __CID_EcoPolyAberth */

/* Component factory (static layout) */
#ifdef ECO_LIB
extern IEcoComponentFactory* GetIEcoComponentFactoryPtr_C1642CE594A740DE8F79D08B659EAEED;
#endif

#endif /* __ID_ECOPOLYABERTH_H__ */
