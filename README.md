# EcoLab1. нахождение корней полинома (`roots`), задание 43

Три компонента технологии **АОМК (ACOM / Eco OS)** на языке C находят все (в том числе комплексные) корни полинома
`p(x) = c₀·xⁿ + c₁·xⁿ⁻¹ + … + cₙ` — аналог функции `roots` в MATLAB. Стандартная библиотека C (libc) и динамическая
память **не используются**: комплексные числа, схема Горнера и квадратный корень реализованы вручную.

| Компонент         | Алгоритм       | CID                                    |
|-------------------|----------------|----------------------------------------|
| `EcoLab1`         | Дюран–Кернер   | `011BF193-DAAC-48E0-B83B-3A8618D23C4C` |
| `EcoPolyAberth`   | Аберт–Эрлих    | `C1642CE5-94A7-40DE-8F79-D08B659EAEED` |
| `EcoPolyLaguerre` | Лагерр         | `9EEDAACC-3D71-44FA-A560-8DB94D0499F1` |

Все три реализуют общий интерфейс `IEcoPolynomialMath` (`roots` — double, `rootsf` — float, `rootsl` — long double).
Клиент выбирает алгоритм, загружая компонент с шины по его CID (константа `ALGORITHM_CHOICE`).

## Сборка и запуск
**Вариант 1 - Xcode.** Откройте `EcoLab1.xcodeproj`, схема **Static** → Build. Затем `EcoLab1UnitTest.xcodeproj`,
схема **Static** → Run.

**Вариант 2 - терминал / VS Code** (из корня проекта):
```bash
make info    # проверить, что найдены библиотеки Eco (если нет: make ECO_FRAMEWORK=/путь/к/Eco)
make run     # собрать и запустить тесты
```
В VS Code: Command (Ctrl) + Shift + B - сборка, Terminal → Run Task → **Run tests** — запуск.

Ожидаемый результат — в конце вывода:
```
TOTAL: 98 of 98 tests passed
```
(по 24 проверки на каждый из трёх алгоритмов + 26 попарных сравнений алгоритмов друг с другом).

Выбор алгоритма: в `UnitTestFiles/SourceFiles/EcoLab1.c` константа `ALGORITHM_CHOICE`
(`0` — все три и сравнение, `1` — Дюран–Кернер, `2` — Аберт–Эрлих, `3` — Лагерр).

## Как воспроизвести результаты из отчёта
1. **Графики и таблицы** (сравнение с `numpy.roots` — аналогом MATLAB `roots`, время работы):
   ```bash
   make plots          # запуск + roots_output.txt + AssemblyFiles/python/plot_roots.py -> папка out/
   ```
   В папке `out/`: `fig1…fig4*.png`, `table_summary.md`, `table_roots.md`, `table_time.md`.
   (Вручную: скопируйте из консоли блок `CSV_BEGIN … CSV_END` в `roots_output.txt` и запустите
   `python3 AssemblyFiles/python/plot_roots.py roots_output.txt out`.)
2. **Стресс-тест** (4000 случайных полиномов, сравнение с `numpy.roots`):
   ```bash
   cd AssemblyFiles/tools/stress
   clang -std=gnu11 -O1 -Istub -I../../../SharedFiles -I../../../HeaderFiles hrand.c -o hrand
   python3 stress.py 4000
   ```
## Коды возврата методов `roots / rootsf / rootsl`
| Код | Значение |
|---|---|
| `0` `ECO_POLY_OK` | успех |
| `1` `ECO_POLY_WARN_LOWACC` | корни найдены, точность снижена (кратные корни, плохо обусловленные полиномы) |
| `-1` `ECO_POLY_ERR_ARG` | нулевой указатель или `num_coeffs == 0` |
| `-2` `ECO_POLY_ERR_ZERO` | все коэффициенты нулевые |
| `-3` `ECO_POLY_ERR_DEGREE` | степень больше `ECO_POLY_MAX_DEGREE` (32) |
| `-4` `ECO_POLY_ERR_NOCONV` | итерации не сошлись |

## Пример использования интерфейса
```c
double c[3] = { 1, -3, 2 };                  /* x^2 - 3x + 2 */
EcoComplexD r[2];
uint32_t n = 0;
int16_t code = pAlg->pVTbl->roots(pAlg, c, 3, r, &n);   /* r = {2, 1}, n = 2 */
```

## Заметки
- Тип `long double` на Apple Silicon (arm64) совпадает с `double`, поэтому `rootsl` там не точнее `roots`, а методы
  Дюрана–Кернера и Аберта чаще возвращают код `1` (шаг не достигает порога 1e-15 из-за шума округления).
- Все три компонента собираются в одну статическую библиотеку, но регистрируются на шине отдельно
  (`RegisterComponent`) и загружаются по собственным CID. Схема Dynamic (dll) поддерживает только `EcoLab1`.

---

# Структура проекта (Project Structure)

Организация папок и файлов репозитория. Используется строгое соглашение об именах: каждая папка содержит файлы
определённого типа, имена папок записаны в `PascalCase` и заканчиваются суффиксом `Files`.

```
/
├── AssemblyFiles
├── BuildFiles
├── DependenciesFiles
├── DesignFiles
├── HeaderFiles
├── SharedFiles
├── SourceFiles
└── UnitTestFiles
```

| Папка | Назначение по соглашению | Что лежит в этой работе |
|---|---|---|
| `AssemblyFiles` | Скрипты, конфигурации и инструменты сборки (`Makefile`, CI/CD, автоматизация) | `python/plot_roots.py` (графики и таблицы), `tools/` (`check_no_libc.sh`, `check_files.sh`, `stress/` — стресс-тест). `Makefile` и `.vscode/` лежат в корне для удобства запуска `make` |
| `BuildFiles` | Результаты сборки (исполняемые файлы, библиотеки, артефакты), готовые к запуску | Проекты Xcode и собранные библиотеки (в Git не хранятся, см. `.gitignore`) |
| `DependenciesFiles` | Сторонние библиотеки и внешние зависимости | Не используются (сторонние библиотеки запрещены условием; фреймворк Eco подключается снаружи) |
| `DesignFiles` | Проектирование и документация: ТЗ, диаграммы, схемы | `Report_EcoLab1_roots.docx` (отчёт), `SYSTEM_PROMPT.md` и `PROMPT_LOG.md` (промт и журнал разработки с ИИ-ассистентом) |
| `HeaderFiles` | Приватные заголовки с внутренней структурой компонентов АОМК | `CEcoLab1.h`, `CEcoPolyAberth.h`, `CEcoPolyLaguerre.h`, `CEco*Factory.h`, `EcoPolyCommon.h` (общий код: комплексная арифметика, драйвер, обёртки float/double/long double) |
| `SharedFiles` | Публичные заголовки интерфейсов и идентификаторов, доступные клиентам | `IEcoPolynomialMath.h` (общий интерфейс), `IdEcoLab1.h`, `IdEcoPolyAberth.h`, `IdEcoPolyLaguerre.h`, `IEcoLab1.h` |
| `SourceFiles` | Основной исходный код | `CEcoLab1.c`, `CEcoPolyAberth.c`, `CEcoPolyLaguerre.c` и их фабрики `CEco*Factory.c` |
| `UnitTestFiles` | Модульные и интеграционные тесты, скрипты автоматического тестирования | `SourceFiles/EcoLab1.c` — консольный клиент: unit-тесты, сравнение алгоритмов, вывод данных для графиков |

---

# Лабораторная работа 2: включение и агрегирование (калькуляторы A–E, EcoLab1, EcoLab2)

Все компоненты собираются в ту же статическую библиотеку и регистрируются клиентом на шине.

| Компонент | Интерфейсы | Приём | CID |
|---|---|---|---|
| `EcoCalculatorA` | X (+, −) | — | `4828F655-2E45-40E7-8121-EBD220DC360E` |
| `EcoCalculatorB` | X (+, −) | можно агрегировать | `AE202E54-3CE5-4550-8996-03BD70C62565` |
| `EcoCalculatorC` | X, Y (+, −, ×, ÷) | — | `A4D0E5C1-7B3F-4E2A-9C58-1F6B2D8E3A47` (в уроках совпадает с CID A, здесь свой — иначе A и C не могут быть в одной библиотеке) |
| `EcoCalculatorD` | Y, X | включает A | `3A8E4467-7E82-475C-B4A3-719ED8397E61` |
| `EcoCalculatorE` | Y, X | агрегирует B | `872FEF1D-E331-4B87-AD44-D1E7C232C2F0` |
| `EcoLab1` (Дюран–Кернер) | IEcoLab1 (= IEcoPolynomialMath), X, Y, IEcoLab1Scheme | можно агрегировать; включает/агрегирует калькуляторы (5 схем) | `011BF193-DAAC-48E0-B83B-3A8618D23C4C` |
| `EcoLab2` | IEcoLab2, X, Y, IEcoPolynomialMath | включает E(+B), D(+A), C; агрегирует EcoLab1 | `2C8F6E4B-1D93-4A57-B0E6-5A7C93D1F482` |

Схемы подключения калькуляторов к `EcoLab1` (выбираются через `IEcoLab1Scheme::SelectScheme`):
1 — Рис.1: X ← A, Y ← D; 2 — Рис.1: X ← B, Y ← E; 3 — Рис.2: X ← B (агрегирование), Y ← E (включение), схема по
умолчанию; 4 — Рис.2: X ← B (агрегирование), Y ← D; 5 — X и Y ← C.

Клиент (`UnitTestFiles/SourceFiles/EcoLab1.c`) проверяет: каждый из A–E отдельно, 5 схем EcoLab1, свойства
интерфейсов (QueryInterface через любой указатель на любой другой, одинаковый IEcoUnknown), EcoLab2.
Ожидаемый результат: `TOTAL: 127 of 127 tests passed` (98 из лабы 1 + 29 новых).
