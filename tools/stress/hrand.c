/* Стресс-тест трёх алгоритмов на случайных полиномах.
 * Читает со стандартного ввода строки "m c0 c1 ... c(m-1)", печатает для каждого алгоритма
 * "код n re0 im0 re1 im1 ...". Запускается скриптом stress.py (сравнение с numpy.roots).
 *
 * Сборка (macOS / Linux), из папки tools/stress:
 *   clang -std=gnu11 -O1 -Istub -I../../SharedFiles -I../../HeaderFiles hrand.c -o hrand
 * (ядра алгоритмов лежат в cores/ - это дословные копии из компонентов)
 */
#include <stdio.h>
#include "IEcoBase1.h"          /* заглушка из stub/ (настоящий заголовок Eco не нужен) */
#include "IEcoPolynomialMath.h"
#include "EcoPolyCommon.h"
#include "cores/dk.c"
#include "cores/aberth.c"
#include "cores/laguerre.c"

int main(void) {
    EcoPolyCoreFn cores[3] = { EcoPolyDK_Core, EcoPolyAberth_Core, EcoPolyLaguerre_Core };
    unsigned m, a, k;
    double c[40];
    while (scanf("%u", &m) == 1) {
        for (k = 0; k < m; k++) {
            if (scanf("%lf", &c[k]) != 1) {
                return 1;
            }
        }
        for (a = 0; a < 3; a++) {
            EcoComplexD r[40];
            uint32_t n = 0;
            int16_t res = EcoPoly_RootsD(cores[a], c, m, r, &n);
            printf("%d %u", (int)res, (unsigned)n);
            for (k = 0; k < n; k++) {
                printf(" %.17g %.17g", r[k].re, r[k].im);
            }
            printf("\n");
        }
    }
    return 0;
}
