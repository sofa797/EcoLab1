#!/bin/bash
# Проверяет, что все 21 файл проекта лежат на своих местах.
# Использование:  bash tools/check_files.sh /Users/sofiakocina/Dev/Eco.Lab1
ROOT="${1:-.}"
missing=0
check() { if [ ! -f "$ROOT/$1" ]; then echo "НЕТ ФАЙЛА: $1"; missing=1; fi; }
for f in CEcoLab1.h CEcoLab1Factory.h CEcoPolyAberth.h CEcoPolyAberthFactory.h CEcoPolyLaguerre.h CEcoPolyLaguerreFactory.h EcoPolyCommon.h; do check "HeaderFiles/$f"; done
for f in IEcoLab1.h IdEcoLab1.h IEcoPolynomialMath.h IdEcoPolyAberth.h IdEcoPolyLaguerre.h; do check "SharedFiles/$f"; done
for f in CEcoLab1.c CEcoLab1Factory.c CEcoPolyAberth.c CEcoPolyAberthFactory.c CEcoPolyLaguerre.c CEcoPolyLaguerreFactory.c; do check "SourceFiles/$f"; done
check "UnitTestFiles/SourceFiles/EcoLab1.c"
if [ $missing -eq 0 ]; then echo "OK: все 19 файлов на месте"; else echo "Скопируйте недостающие файлы из архива в указанные папки."; fi
