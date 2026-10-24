/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   IdEcoCalculatorC
 * </сводка>
 *
 * <описание>
 *   Данный заголовок описывает интерфейс IdEcoCalculatorC
 * </описание>
 *
 * <ссылка>
 *
 * </ссылка>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#ifndef __ID_ECO_CALCULATOR_C_H__
#define __ID_ECO_CALCULATOR_C_H__

#include "IEcoBase1.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"

/* EcoCalculatorC CID = {A4D0E5C1-7B3F-4E2A-9C58-1F6B2D8E3A47} */
#ifndef __CID_EcoCalculatorC
static const UGUID CID_EcoCalculatorC = {0x01, 0x10, {0xA4, 0xD0, 0xE5, 0xC1, 0x7B, 0x3F, 0x4E, 0x2A, 0x9C, 0x58, 0x1F, 0x6B, 0x2D, 0x8E, 0x3A, 0x47}};
#endif /* __CID_EcoCalculatorC */

/* Фабрика компонента для динамической и статической компановки */
#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr();
#elif ECO_LIB
extern IEcoComponentFactory* GetIEcoComponentFactoryPtr_A4D0E5C17B3F4E2A9C581F6B2D8E3A47;
#endif

#endif /* __ID_ECO_CALCULATOR_C_H__ */
