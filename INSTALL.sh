#!/bin/bash

DEST="${1:?Укажите корень проекта, например /Users/username/Dev/Eco.Lab1}"
HERE="$(cd "$(dirname "$0")" && pwd)"

for d in HeaderFiles SharedFiles SourceFiles UnitTestFiles/SourceFiles; do
  mkdir -p "$DEST/$d"
  cp -v "$HERE/$d/"* "$DEST/$d/"
done
