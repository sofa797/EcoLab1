/*
 * Laguerre method with IMPLICIT deflation.
 * Roots are found one by one. Instead of dividing the polynomial by (x - root) (which accumulates
 * errors), the already found roots r_j are taken into account analytically:
 *   Gp = p'/p,  S1 = sum 1/(x - r_j),  S2 = sum 1/(x - r_j)^2
 *   G  = Gp - S1,   H = Gp^2 - p''/p - S2
 *   a  = m / (G +- sqrt((m-1)(m*H - G^2))),   m = n - (number of found roots)
 *   x <- x - a        (sign chosen with the larger |denominator|)
 * p, p', p'' are always evaluated on the ORIGINAL polynomial, so no polishing step is needed.
 */

/* Fractions of a full step used every 10th iteration to break possible cycles */
static const long double g_LaguerreFrac[8] = {0.5L, 0.25L, 0.75L, 0.13L, 0.38L, 0.62L, 0.88L, 1.0L};

/* Cauchy bound: all roots lie inside |z| <= bound */
static long double EcoPolyLaguerre_Bound(const long double* c, uint32_t n) {
    long double bound, v;
    uint32_t i;
    bound = 0.0L;
    for (i = 1; i <= n; i++) {
        v = EcoPoly_Abs(c[i]);
        if (v > bound) {
            bound = v;
        }
    }
    return bound + 1.0L;
}

/* One root of the monic polynomial c[0..n] (c[0] = 1) different from the k found roots r[0..k-1] */
static int16_t EcoPolyLaguerre_Find(const long double* c, uint32_t n, const EcoComplexL* r, uint32_t k,
                                    long double bound, EcoComplexL* x) {
    EcoComplexL p, dp, d2, gp, g, h, s1, s2, t, sq, gplus, gminus, den, dx;
    long double dm, ap, ab, dn;
    uint32_t i, j, iter, m;
    int16_t bad;

    m = n - k;                                       /* degree of the "deflated" polynomial */
    g = EcoPoly_Make(0.0L, 0.0L);
    h = EcoPoly_Make(0.0L, 0.0L);
    sq = EcoPoly_Make(0.0L, 0.0L);
    for (iter = 1; iter <= 120; iter++) {
        /* p, p', p''/2 by Horner scheme; ap - bound of the rounding error of p */
        p = EcoPoly_Make(c[0], 0.0L);
        dp = EcoPoly_Make(0.0L, 0.0L);
        d2 = EcoPoly_Make(0.0L, 0.0L);
        ap = EcoPoly_Abs(c[0]);
        ab = EcoPoly_Norm1(*x);
        for (i = 1; i <= n; i++) {
            d2 = EcoPoly_Add(EcoPoly_Mul(d2, *x), dp);
            dp = EcoPoly_Add(EcoPoly_Mul(dp, *x), p);
            p = EcoPoly_Mul(p, *x);
            p.re += c[i];
            ap = ap * ab + EcoPoly_Abs(c[i]);
        }
        if (EcoPoly_Norm1(p) <= 1.0e-16L * ap) {
            return ECO_POLY_OK;                      /* p(x) is at the rounding-noise level: root found */
        }
        gp = EcoPoly_Div(dp, p);
        s1 = EcoPoly_Make(0.0L, 0.0L);
        s2 = EcoPoly_Make(0.0L, 0.0L);
        bad = 0;
        for (j = 0; j < k; j++) {
            t = EcoPoly_Sub(*x, r[j]);
            if (EcoPoly_Norm2(t) == 0.0L) {
                bad = 1;
                break;
            }
            t = EcoPoly_Div(EcoPoly_Make(1.0L, 0.0L), t);
            s1 = EcoPoly_Add(s1, t);
            s2 = EcoPoly_Add(s2, EcoPoly_Mul(t, t));
        }
        den = EcoPoly_Make(0.0L, 0.0L);
        if (!bad) {
            g = EcoPoly_Sub(gp, s1);
            h = EcoPoly_Sub(EcoPoly_Sub(EcoPoly_Mul(gp, gp), EcoPoly_MulR(EcoPoly_Div(d2, p), 2.0L)), s2);
            sq = EcoPoly_CSqrt(EcoPoly_MulR(EcoPoly_Sub(EcoPoly_MulR(h, (long double)m), EcoPoly_Mul(g, g)),
                                            (long double)(m - 1)));
            gplus = EcoPoly_Add(g, sq);
            gminus = EcoPoly_Sub(g, sq);
            den = (EcoPoly_Norm2(gplus) >= EcoPoly_Norm2(gminus)) ? gplus : gminus;
        }
        if (bad || EcoPoly_Norm2(den) == 0.0L) {
            /* degenerate point (x coincides with a found root, or G = 0): shift x */
            x->re += (1.0L + EcoPoly_Norm1(*x)) * 0.3L;
            x->im += (1.0L + EcoPoly_Norm1(*x)) * 0.2L;
            continue;
        }
        dx = EcoPoly_Div(EcoPoly_Make((long double)m, 0.0L), den);
        dn = EcoPoly_Norm2(dx);
        dm = dn / (1.0L + EcoPoly_Norm2(*x));
        if (dn > 4.0L * bound * bound) {
            /* too long step (p' is almost zero): limit it, otherwise the method can cycle */
            dx = EcoPoly_MulR(dx, 2.0L * bound / EcoPoly_Sqrt(dn));
        }
        if (iter % 10 == 0) {
            dx = EcoPoly_MulR(dx, g_LaguerreFrac[(iter / 10) % 8]);
        }
        *x = EcoPoly_Sub(*x, dx);
        if (dm <= ECO_POLY_TOL2) {
            return ECO_POLY_OK;
        }
    }
    return ECO_POLY_ERR_NOCONV;
}

static int16_t EcoPolyLaguerre_Core(const long double* c, uint32_t n, EcoComplexL* z) {
    EcoComplexL x, w;
    long double bound;
    uint32_t k, tries;
    int16_t st;

    bound = EcoPolyLaguerre_Bound(c, n);
    w = EcoPoly_Make(0.4L, 0.9L);
    for (k = 0; k < n; k++) {
        /* start point: "generic" direction at the distance of the Cauchy bound
         * (x = 0 is bad: p'(0) = 0 for many polynomials, for example x^n - 1) */
        x = EcoPoly_MulR(w, bound);
        st = EcoPolyLaguerre_Find(c, n, z, k, bound, &x);
        /* if the iteration cycles, restart from another point */
        for (tries = 1; st < 0 && tries < 8; tries++) {
            x = EcoPoly_MulR(EcoPoly_Mul(w, EcoPoly_Make(0.4L + 0.1L * (long double)tries, 0.9L)), bound);
            st = EcoPolyLaguerre_Find(c, n, z, k, bound, &x);
        }
        if (st < 0) {
            return st;
        }
        z[k] = x;
    }
    return ECO_POLY_OK;
}
