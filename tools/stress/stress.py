#!/usr/bin/env python3
"""
Стресс-тест: N случайных полиномов (случайные коэффициенты, полиномы с известными корнями,
разреженные x^n + a*x^k + b) -> три алгоритма -> сравнение с numpy.roots (аналог MATLAB roots).

    cd tools/stress
    clang -std=gnu11 -O1 -Istub -I../../SharedFiles -I../../HeaderFiles hrand.c -o hrand
    pip3 install numpy
    python3 stress.py [N=4000]
"""
import sys
import subprocess
import random
import os
import numpy as np

N = int(sys.argv[1]) if len(sys.argv) > 1 else 4000
BIN = os.path.join(os.path.dirname(os.path.abspath(__file__)), "hrand")
random.seed(1)
np.random.seed(1)
polys = []
for t in range(N):
    kind = t % 3
    if kind == 0:                                   # случайные коэффициенты, степень 2..20
        n = random.randint(2, 20)
        c = list(np.random.randn(n + 1))
    elif kind == 1:                                 # известные корни (вещественные + сопряжённые пары)
        rts = [complex(random.uniform(-3, 3), 0) for _ in range(random.randint(1, 6))]
        for _ in range(random.randint(0, 4)):
            z = complex(random.uniform(-3, 3), random.uniform(0.2, 3))
            rts += [z, z.conjugate()]
        c = list(np.real(np.poly(rts)))
    else:                                           # разреженные x^n + a x^k + b
        n = random.randint(3, 16)
        c = [0.0] * (n + 1)
        c[0] = 1
        c[random.randint(1, n)] = random.uniform(-3, 3)
        c[n] = random.uniform(-2, 2) or 1
    polys.append((c, kind))

inp = "".join(f"{len(c)} " + " ".join(repr(float(x)) for x in c) + "\n" for c, _ in polys)
out = subprocess.run([BIN], input=inp, capture_output=True, text=True).stdout.strip().split("\n")
names = ["Durand-Kerner", "Aberth-Ehrlich", "Laguerre"]
kinds = ["случайные коэфф.", "известные корни", "разреженные"]
print(f"Полиномов: {N}")
for a in range(3):
    fail = warn = 0
    mx = [0.0, 0.0, 0.0]
    bad = []
    for i, (c, kind) in enumerate(polys):
        ref = np.roots(c)
        p = out[i * 3 + a].split()
        code, n = int(p[0]), int(p[1])
        if code < 0:
            fail += 1
            bad.append((i, kinds[kind], len(c) - 1, f"код {code}"))
            continue
        warn += code == 1
        rs = [complex(float(p[2 + 2 * k]), float(p[3 + 2 * k])) for k in range(n)]
        rem = list(ref)
        err = 0.0
        for r in rs:
            j = min(range(len(rem)), key=lambda q: abs(rem[q] - r))
            err = max(err, abs(rem[j] - r) / (1 + abs(r)))
            rem.pop(j)
        mx[kind] = max(mx[kind], err)
        if err > 1e-5:
            bad.append((i, kinds[kind], len(c) - 1, f"погрешность {err:.1e}"))
    print(f"{names[a]:15s} отказов: {fail:3d}  код 1 (сниж. точность): {warn:3d}  "
          f"макс. отн. погрешность по группам: " + ", ".join(f"{kinds[k]} {mx[k]:.1e}" for k in range(3)))
    if bad:
        print("   проблемные случаи:", bad[:6])
