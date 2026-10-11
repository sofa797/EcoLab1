/*
 * Aberth-Ehrlich method (cubic convergence for simple roots).
 *   w_k = p(z_k) / p'(z_k)
 *   z_k <- z_k - w_k / (1 - w_k * sum_{j != k} 1 / (z_k - z_j))
 */
static int16_t EcoPolyAberth_Core(const long double* c, uint32_t n, EcoComplexL* z) {
    EcoComplexL p, dp, w, s, den, delta, diff;
    long double maxstep, step;
    long double best;
    uint32_t j, k, iter, stall;
    int16_t bad;

    EcoPoly_InitialGuess(c, n, z);
    w = EcoPoly_Make(0.0L, 0.0L);
    den = EcoPoly_Make(1.0L, 0.0L);
    maxstep = 1.0L;
    best = 1.0e30L;
    stall = 0;
    for (iter = 0; iter < ECO_POLY_MAX_ITER; iter++) {
        maxstep = 0.0L;
        for (k = 0; k < n; k++) {
            EcoPoly_HornerPD(c, n, z[k], &p, &dp);
            if (EcoPoly_Norm2(p) == 0.0L) {
                continue;                         /* exact root */
            }
            bad = 0;
            if (EcoPoly_Norm2(dp) == 0.0L) {
                bad = 1;
            }
            else {
                w = EcoPoly_Div(p, dp);
                s = EcoPoly_Make(0.0L, 0.0L);
                for (j = 0; j < n && !bad; j++) {
                    if (j != k) {
                        diff = EcoPoly_Sub(z[k], z[j]);
                        if (EcoPoly_Norm2(diff) == 0.0L) {
                            bad = 1;
                        }
                        else {
                            s = EcoPoly_Add(s, EcoPoly_Div(EcoPoly_Make(1.0L, 0.0L), diff));
                        }
                    }
                }
                if (!bad) {
                    den = EcoPoly_Sub(EcoPoly_Make(1.0L, 0.0L), EcoPoly_Mul(w, s));
                    if (EcoPoly_Norm2(den) == 0.0L) {
                        bad = 1;
                    }
                }
            }
            if (bad) {
                EcoPoly_Perturb(&z[k]);
                maxstep = 1.0L;
                continue;
            }
            delta = EcoPoly_Div(w, den);
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
