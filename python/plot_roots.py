#!/usr/bin/env python3
import sys, os, math, cmath

try:
    import numpy as np
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
except ImportError:
    sys.exit("Установите зависимости:  pip3 install numpy matplotlib")

DECIMALS = 12
COLORS = {1: "tab:blue", 2: "tab:green", 3: "tab:purple"}
MARKERS = {1: "o", 2: "s", 3: "^"}

EXACT = {
    "P1": [2, 1], "P2": [3, 2, 1], "P3": [1j, -1j], "P4": [1, -1, 1j, -1j],
    "P5": [1, complex(-0.5, math.sqrt(3) / 2), complex(-0.5, -math.sqrt(3) / 2)],
    "P6": [1, 2, 3, 4, 5, 6], "P8": [1, 0, 0], "P9": [1, 1, 1],
    "P12": [cmath.exp(2j * math.pi * k / 10) for k in range(10)],
    "P13": list(range(1, 11)),
}

def parse(path):
    algs, polys, order, times = {}, {}, [], {}
    with open(path, encoding="utf-8", errors="replace") as f:
        for line in f:
            line = line.strip()
            if line.startswith("ALG;"):
                _, a, name = line.split(";", 2); algs[int(a)] = name
            elif line.startswith("POLY;"):
                _, a, pid, name, coeffs, code, cnt = line.split(";")
                polys[(int(a), pid)] = dict(name=name, coeffs=[float(x) for x in coeffs.split(",")], code=int(code), count=int(cnt), roots=[])
                if pid not in order: order.append(pid)
            elif line.startswith("ROOT;"):
                _, a, pid, _k, re, im = line.split(";")
                polys[(int(a), pid)]["roots"].append(complex(float(re), float(im)))
            elif line.startswith("TIME;"):
                _, a, pid, us = line.split(";"); times[(int(a), pid)] = float(us)
    if not polys: sys.exit("В файле нет строк POLY;/ROOT;")
    return algs, polys, order, times

def reference_roots(coeffs): return list(np.roots(coeffs))

def match(found, ref):
    ref = list(ref); out = []
    for r in found:
        j = min(range(len(ref)), key=lambda i: abs(ref[i] - r))
        out.append((r, ref[j], abs(ref[j] - r))); ref.pop(j)
    return out

def fmt_c(z):
    sign = "-" if z.imag < 0 else "+"
    return f"{z.real:.{DECIMALS}f} {sign} {abs(z.imag):.{DECIMALS}f}i"

def sci(x): return "-" if x is None else ("0" if x == 0 else f"{x:.2e}")

def main():
    if len(sys.argv) < 2: sys.exit("Использование: python3 plot_roots.py roots_output.txt [папка_результатов]")
    outdir = sys.argv[2] if len(sys.argv) > 2 else "out"
    os.makedirs(outdir, exist_ok=True)
    algs, polys, order, times = parse(sys.argv[1])
    alg_ids = sorted(algs)
    short = {a: algs[a].split(" (")[0] for a in alg_ids}
    floor = 1e-18

    stat, refs = {}, {}
    for pid in order: refs[pid] = reference_roots(polys[(alg_ids[0], pid)]["coeffs"])
    for a in alg_ids:
        for pid in order:
            p = polys[(a, pid)]
            m = match(p["roots"], refs[pid])
            err_np = max((x[2] for x in m), default=0.0)
            err_ex = max(x[2] for x in match(p["roots"], EXACT[pid])) if pid in EXACT and len(EXACT[pid]) == len(p["roots"]) else None
            stat[(a, pid)] = dict(err_np=err_np, err_ex=err_ex, code=p["code"])
    err_np_ex = {pid: max(x[2] for x in match(refs[pid], EXACT[pid])) for pid in order if pid in EXACT and len(EXACT[pid]) == len(refs[pid])}

    # Таблицы
    head = "| Полином | n |" + "".join(f" {short[a]}: код | {short[a]}: abs(алг-numpy) | {short[a]}: abs(алг-точное) |" for a in alg_ids) + " abs(numpy-точное) |"
    sep = "|---|---|" + "---|---|---|" * len(alg_ids) + "---|"
    with open(os.path.join(outdir, "table_summary.md"), "w", encoding="utf-8") as f:
        f.write(head + "\n" + sep + "\n")
        for pid in order:
            p0 = polys[(alg_ids[0], pid)]
            row = f"| {pid}: {p0['name']} | {len(p0['coeffs']) - 1} |"
            for a in alg_ids:
                s = stat[(a, pid)]
                row += f" {s['code']} | {sci(s['err_np'])} | {sci(s['err_ex'])} |"
            row += f" {sci(err_np_ex.get(pid))} |"
            f.write(row + "\n")
            
    with open(os.path.join(outdir, "table_roots.md"), "w", encoding="utf-8") as f:
        for pid in order:
            p0 = polys[(alg_ids[0], pid)]
            f.write(f"\n### {pid}: {p0['name']}\n\n| № | numpy.roots |" + "".join(f" {short[a]} | Δ |" for a in alg_ids) + "\n|---|---|" + "---|---|" * len(alg_ids) + "\n")
            rem = {a: list(polys[(a, pid)]["roots"]) for a in alg_ids}
            for i, rr in enumerate(refs[pid], 1):
                row = f"| {i} | {fmt_c(rr)} |"
                for a in alg_ids:
                    if rem[a]:
                        j = min(range(len(rem[a])), key=lambda q: abs(rem[a][q] - rr))
                        z = rem[a].pop(j)
                        row += f" {fmt_c(z)} | {sci(abs(z - rr))} |"
                    else: row += " - | - |"
                f.write(row + "\n")

    with open(os.path.join(outdir, "table_time.md"), "w", encoding="utf-8") as f:
        f.write("| Полином |" + "".join(f" {short[a]}, мкс |" for a in alg_ids) + "\n|---|" + "---|" * len(alg_ids) + "\n")
        for pid in order:
            f.write(f"| {pid}: {polys[(alg_ids[0], pid)]['name']} |" + "".join(f" {times.get((a, pid), float('nan')):.2f} |" for a in alg_ids) + "\n")
        f.write("| **Сумма** |" + "".join(f" {sum(times.get((a, pid), 0) for pid in order):.2f} |" for a in alg_ids) + "\n")

    # Рис. 1: Комплексная плоскость
    n, cols, rws = len(order), 4, math.ceil(len(order) / 4)
    fig, axs = plt.subplots(rws, cols, figsize=(4 * cols, 3.6 * rws))
    axs = np.array(axs).reshape(-1)
    for ax, pid in zip(axs, order):
        ax.axhline(0, color="gray", lw=0.5); ax.axvline(0, color="gray", lw=0.5)
        ax.scatter([z.real for z in refs[pid]], [z.imag for z in refs[pid]], marker="x", s=130, color="tab:red", label="numpy.roots", zorder=2)
        for a in alg_ids:
            r = polys[(a, pid)]["roots"]
            ax.scatter([z.real for z in r], [z.imag for z in r], marker=MARKERS[a], s=50 - 10 * (a - 1), facecolors="none", edgecolors=COLORS[a], label=short[a], zorder=3)
        ax.set_title(f"{pid}: {polys[(alg_ids[0], pid)]['name']}", fontsize=9)
        ax.set_xlabel("Re"); ax.set_ylabel("Im"); ax.set_aspect("equal", adjustable="datalim"); ax.grid(alpha=0.3)
    for ax in axs[n:]: ax.axis("off")
    axs[0].legend(fontsize=7); fig.suptitle("Корни полиномов на комплексной плоскости"); fig.tight_layout()
    fig.savefig(os.path.join(outdir, "fig1_complex_plane.png"), dpi=150); plt.close(fig)

    # Рис. 2 и 3: Погрешности
    def grouped(ids, getter, ylabel, title, fname, extra=None):
        x = np.arange(len(ids)); k = len(alg_ids) + (1 if extra else 0); w = 0.8 / k
        fig, ax = plt.subplots(figsize=(11, 4.8))
        for i, a in enumerate(alg_ids):
            ax.bar(x + (i - (k - 1) / 2) * w, [max(getter(a, pid), floor) for pid in ids], w, label=short[a], color=COLORS[a])
        if extra:
            ax.bar(x + ((k - 1) - (k - 1) / 2) * w, [max(extra(pid), floor) for pid in ids], w, label="numpy.roots", color="tab:red")
        ax.set_xticks(x); ax.set_xticklabels(ids); ax.set_yscale("log"); ax.set_ylabel(ylabel); ax.set_title(title)
        ax.legend(fontsize=8); ax.grid(axis="y", alpha=0.3); fig.tight_layout()
        fig.savefig(os.path.join(outdir, fname), dpi=150); plt.close(fig)

    grouped(order, lambda a, pid: stat[(a, pid)]["err_np"], "max |алгоритм - numpy|", "Расхождение алгоритмов с numpy.roots", "fig2_error_vs_numpy.png")
    ex_ids = [pid for pid in order if pid in err_np_ex]
    if ex_ids:
        grouped(ex_ids, lambda a, pid: stat[(a, pid)]["err_ex"], "max |корень - точное значение|", "Погрешность относительно точных корней", "fig3_error_vs_exact.png", extra=lambda pid: err_np_ex[pid])

    # Рис. 4: Время
    if times:
        x = np.arange(len(order)); w = 0.8 / len(alg_ids)
        fig, ax = plt.subplots(figsize=(11, 4.8))
        for i, a in enumerate(alg_ids):
            ax.bar(x + (i - (len(alg_ids) - 1) / 2) * w, [times.get((a, pid), 0) for pid in order], w, label=short[a], color=COLORS[a])
        ax.set_xticks(x); ax.set_xticklabels(order); ax.set_yscale("log")
        ax.set_ylabel("время одного вызова roots, мкс"); ax.set_title("Быстродействие алгоритмов")
        ax.legend(fontsize=8); ax.grid(axis="y", alpha=0.3); fig.tight_layout()
        fig.savefig(os.path.join(outdir, "fig4_time.png"), dpi=150); plt.close(fig)

    print(f"Файлы сохранены в папке: {os.path.abspath(outdir)}\n")
    print("Сводка (max |алгоритм - numpy|, код возврата):")
    print("  полином " + "".join(f"{short[a][:16]:>22s}" for a in alg_ids))
    for pid in order:
        print(f"  {pid:5s}   " + "".join(f"{sci(stat[(a, pid)]['err_np']):>14s} (код {stat[(a, pid)]['code']})" for a in alg_ids))

if __name__ == "__main__":
    main()
