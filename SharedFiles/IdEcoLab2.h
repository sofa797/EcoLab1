/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   IdEcoLab2
 * </summary>
 *
 * <description>
 *   This header describes the identifiers of the component EcoLab2
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */

#ifndef __ID_ECOLAB2_H__
#define __ID_ECOLAB2_H__

#include "IEcoBase1.h"
#include "IEcoLab2.h"
#include "IEcoPolynomialMath.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"

/* EcoLab2 CID = {2C8F6E4B-1D93-4A57-B0E6-5A7C93D1F482} */
#ifndef __CID_EcoLab2
static const UGUID CID_EcoLab2 = {0x01, 0x10, {0x2C, 0x8F, 0x6E, 0x4B, 0x1D, 0x93, 0x4A, 0x57, 0xB0, 0xE6, 0x5A, 0x7C, 0x93, 0xD1, 0xF4, 0x82}};
#endif /* __CID_EcoLab2 */

/* Component factory for dynamic and static layout */
#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr();
#elif ECO_LIB
extern IEcoComponentFactory* GetIEcoComponentFactoryPtr_2C8F6E4B1D934A57B0E65A7C93D1F482;
#endif

#endif /* __ID_ECOLAB2_H__ */
