/*
 * Durand-Kerner (Weierstrass) method.
 *   z_k <- z_k - p(z_k) / prod_{j != k} (z_k - z_j),  p - monic polynomial.
 * All roots are refined simultaneously; the updated values are used at once (Gauss-Seidel).
 */
static int16_t EcoPolyDK_Core(const long double* c, uint32_t n, EcoComplexL* z) {
    EcoComplexL p, d, delta, diff;
    long double maxstep, step;
    long double best;
    uint32_t j, k, iter, stall;

    EcoPoly_InitialGuess(c, n, z);
    maxstep = 1.0L;
    best = 1.0e30L;
    stall = 0;
    for (iter = 0; iter < ECO_POLY_MAX_ITER; iter++) {
        maxstep = 0.0L;
        for (k = 0; k < n; k++) {
            p = EcoPoly_Horner(c, n, z[k]);
            d = EcoPoly_Make(1.0L, 0.0L);
            for (j = 0; j < n; j++) {
                if (j != k) {
                    diff = EcoPoly_Sub(z[k], z[j]);
                    d = EcoPoly_Mul(d, diff);
                }
            }
            if (EcoPoly_Norm2(d) == 0.0L) {
                /* two approximations coincide - shift slightly */
                EcoPoly_Perturb(&z[k]);
                maxstep = 1.0L;
                continue;
            }
            delta = EcoPoly_Div(p, d);
            z[k] = EcoPoly_Sub(z[k], delta);
            step = EcoPoly_Norm2(delta) / (1.0L + EcoPoly_Norm2(z[k]));
            if (step > maxstep) {
                maxstep = step;
            }
        }
        if (maxstep <= ECO_POLY_TOL2) {
            return ECO_POLY_OK;
        }
        /* stagnation: the step stopped decreasing (rounding-noise level, multiple roots) -
         * do not waste the remaining iterations */
        if (maxstep < 0.5L * best) {
            best = maxstep;
            stall = 0;
        }
        else if (++stall >= 30 && maxstep <= ECO_POLY_TOL2_LOW) {
            return ECO_POLY_WARN_LOWACC;
        }
    }
    return (maxstep <= ECO_POLY_TOL2_LOW) ? ECO_POLY_WARN_LOWACC : ECO_POLY_ERR_NOCONV;
}
