#!/bin/bash
# Проверка: компоненты не используют libc.
#   1) в исходниках нет #include <...> и запрещённых функций;
#   2) (необязательно) в собранной библиотеке/объектном файле нет неразрешённых внешних символов.
# Использование:
#   ./tools/check_no_libc.sh <корень проекта> [путь к libXXX.a или .o]
ROOT="${1:-.}"
LIB="$2"
FILES="$ROOT/SourceFiles/CEcoLab1.c $ROOT/SourceFiles/CEcoPolyAberth.c $ROOT/SourceFiles/CEcoPolyLaguerre.c \
$ROOT/HeaderFiles/EcoPolyCommon.h $ROOT/SharedFiles/IEcoPolynomialMath.h"
FORBID='\b(malloc|calloc|realloc|free|memcpy|memmove|memset|memcmp|strlen|strcpy|strcmp|sqrt|sqrtf|fabs|fabsl|abs|labs|sin|cos|tan|atan|atan2|exp|log|pow|floor|ceil|fmod|hypot|printf|scanf|puts|getchar)[[:space:]]*\('
fail=0
echo "== 1. #include <...> в файлах компонента (должно быть пусто)"
grep -nE '^[[:space:]]*#[[:space:]]*include[[:space:]]*<' $FILES && fail=1
echo "== 2. Запрещённые вызовы libc (должно быть пусто)"
grep -nE "$FORBID" $FILES | grep -vE '^\S+:[0-9]+:\s*(/\*|\*)' && fail=1
if [ -n "$LIB" ]; then
  echo "== 3. Неразрешённые символы в $LIB"
  nm -u "$LIB" 2>/dev/null | grep -v '^$' | grep -vE ':$' | sort -u
  echo "(ожидается пусто; допустимы только ___stack_chk_fail / ___stack_chk_guard -"
  echo " это защита стека, добавляемая компилятором, а не вызов libc из кода)"
fi
if [ $fail -eq 0 ]; then echo "РЕЗУЛЬТАТ: OK - libc не используется"; else echo "РЕЗУЛЬТАТ: НАЙДЕНЫ ЗАПРЕЩЁННЫЕ ВЫЗОВЫ"; fi
exit $fail
