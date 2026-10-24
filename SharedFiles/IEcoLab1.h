/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   IEcoLab1
 * </summary>
 *
 * <description>
 *   Compatibility header. The interface of the component is the common interface
 *   IEcoPolynomialMath (see IEcoPolynomialMath.h) implemented by all three algorithm components.
 *   Lab 2: the name IEcoLab1 of the task is an alias of IEcoPolynomialMath, so the client can
 *   write  IEcoLab1* pIEcoLab1 = 0;  as in the task text.
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 . All rights reserved.
 * </author>
 *
 */

#ifndef __I_ECOLAB1_H__
#define __I_ECOLAB1_H__

#include "IEcoPolynomialMath.h"

typedef IEcoPolynomialMath IEcoLab1;
#define IID_IEcoLab1 IID_IEcoPolynomialMath

#endif /* __I_ECOLAB1_H__ */
