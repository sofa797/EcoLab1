@echo off

if "%MSVS_BT_ROOT%"=="" (
    echo ERROR: Env MSVS_BT_ROOT not set!
    pause
    exit /b 1
)

echo Configure MSVC...
set ARCH_TARGET=x86
call "%MSVS_BT_ROOT%\VC\Auxiliary\Build\vcvarsall.bat" %ARCH_TARGET% > nul

echo Build all ...
call make clean -f Makefile TARGET=0 DEBUG=0 ARCH=x86
call make -f Makefile TARGET=0 DEBUG=0 ARCH=x86
call make clean -f Makefile TARGET=0 DEBUG=1 ARCH=x86
call make -f Makefile TARGET=0 DEBUG=1 ARCH=x86
call make clean -f Makefile TARGET=1 DEBUG=0 ARCH=x86
call make -f Makefile TARGET=1 DEBUG=0 ARCH=x86
call make clean -f Makefile TARGET=1 DEBUG=1 ARCH=x86
call make -f Makefile TARGET=1 DEBUG=1 ARCH=x86
set ARCH_TARGET=x64
call "%MSVS_BT_ROOT%\VC\Auxiliary\Build\vcvarsall.bat" %ARCH_TARGET% > nul
call make clean -f Makefile TARGET=0 DEBUG=0 ARCH=x64
call make -f Makefile TARGET=0 DEBUG=0 ARCH=x64
call make clean -f Makefile TARGET=0 DEBUG=1 ARCH=x64
call make -f Makefile TARGET=0 DEBUG=1 ARCH=x64
call make clean -f Makefile TARGET=1 DEBUG=0 ARCH=x64
call make -f Makefile TARGET=1 DEBUG=0 ARCH=x64
call make clean -f Makefile TARGET=1 DEBUG=1 ARCH=x64
call make -f Makefile TARGET=1 DEBUG=1 ARCH=x64
set ARCH_TARGET=x86
call "%MSVS_BT_ROOT%\VC\Auxiliary\Build\vcvarsall.bat" %ARCH_TARGET% > nul
set ARCH_TARGET=x64
call "%MSVS_BT_ROOT%\VC\Auxiliary\Build\vcvarsall.bat" %ARCH_TARGET% > nul
pause
