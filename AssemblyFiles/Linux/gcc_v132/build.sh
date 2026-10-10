#!/usr/bin/env bash
echo "Build Eco.Lab1"
DIR=$(cd "$(dirname "$0")"; pwd)
echo $DIR
# ARCH=x86_64 | ARCH=rv64 | ARCH=aarch64
# ARCH_EXT= | ARCH_EXT=gcv | ARCH_EXT=-v8a
make clean -f Makefile TARGET=0 DEBUG=0 ARCH=x86_64
make -f Makefile TARGET=0 DEBUG=0 ARCH=x86_64
make clean -f Makefile TARGET=0 DEBUG=1 ARCH=x86_64
make -f Makefile TARGET=0 DEBUG=1 ARCH=x86_64
make clean -f Makefile TARGET=1 DEBUG=0 ARCH=x86_64
make -f Makefile TARGET=1 DEBUG=0 ARCH=x86_64
make clean -f Makefile TARGET=1 DEBUG=1 ARCH=x86_64
make -f Makefile TARGET=1 DEBUG=1 ARCH=x86_64
make clean -f MakefileExe TARGET=0 DEBUG=0 ARCH=x86_64
make -f MakefileExe TARGET=0 DEBUG=0 ARCH=x86_64
make clean -f MakefileExe TARGET=0 DEBUG=1 ARCH=x86_64
make -f MakefileExe TARGET=0 DEBUG=1 ARCH=x86_64
make clean -f MakefileExe TARGET=1 DEBUG=0 ARCH=x86_64
make -f MakefileExe TARGET=1 DEBUG=0 ARCH=x86_64
make clean -f MakefileExe TARGET=1 DEBUG=1 ARCH=x86_64
make -f MakefileExe TARGET=1 DEBUG=1 ARCH=x86_64
