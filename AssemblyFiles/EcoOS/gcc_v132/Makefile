# --- Host Platform Detection ---
ifeq ($(OS), Windows_NT)
    SHELL := cmd.exe
    HOST_OS = win-x86_64
    RM = -rm -f
    SLASH = /
    # Function to fix paths for Windows shell commands (mkdir/del)
    FIX_PATH = $(subst /,\,$1)
    FRAMEWORK := $(subst \,/,$(ECO_FRAMEWORK))
    TOOLCHAIN := $(subst \,/,$(ECO_TOOLCHAIN))
else
    HOST_OS = nix-x86_64
    RM = rm -f
    SLASH = /
    FIX_PATH = $1
    FRAMEWORK := $(ECO_FRAMEWORK)
    TOOLCHAIN := $(ECO_TOOLCHAIN)
endif

# --- Default Project Configurations ---
# Use: make TARGET=0 ARCH=rv64 DEBUG=1
TARGET ?= 1
ARCH ?= x64
DEBUG ?= 0
PLATFORM = EcoOS

# --- Toolchain and Architecture Selection ---
# Format: eco-<arch>-<type>-gcc-13.2.0-<host>
ifeq ($(ARCH), x86_16)
    TOOL = $(TOOLCHAIN)$(SLASH)eco-i686-win-dwarf-gcc-13.2.0-$(HOST_OS)$(SLASH)bin$(SLASH)i686-w64-mingw32-
    CCFLAGS_ARCH = -DECO_X86_16
    ARCH_TARGET = x86_16

else ifeq ($(ARCH), x86_32)
    TOOL = $(TOOLCHAIN)$(SLASH)eco-i686-win-dwarf-gcc-13.2.0-$(HOST_OS)$(SLASH)bin$(SLASH)i686-w64-mingw32-
    CCFLAGS_ARCH = -DECO_X86_32
    LDFLAGS_ARCH = -Wl,--enable-stdcall-fixup -Wl,--add-stdcall-alias
    ARCH_TARGET = x86

else ifeq ($(ARCH), x86)
    TOOL = $(TOOLCHAIN)$(SLASH)eco-i686-win-dwarf-gcc-13.2.0-$(HOST_OS)$(SLASH)bin$(SLASH)i686-w64-mingw32-
    CCFLAGS_ARCH = -DECO_X86_32
    LDFLAGS_ARCH = -Wl,--enable-stdcall-fixup -Wl,--add-stdcall-alias
    ARCH_TARGET = x86

else ifeq ($(ARCH), x86_64)
    TOOL = $(TOOLCHAIN)$(SLASH)eco-x86_64-w64-mingw32-gcc-13.2.0-$(HOST_OS)$(SLASH)bin$(SLASH)x86_64-w64-mingw32-
    CCFLAGS_ARCH = -DECO_X86_64
    ARCH_TARGET = amd64

else ifeq ($(ARCH), x64)
    TOOL = $(TOOLCHAIN)$(SLASH)eco-x86_64-w64-mingw32-gcc-13.2.0-$(HOST_OS)$(SLASH)bin$(SLASH)x86_64-w64-mingw32-
    CCFLAGS_ARCH = -DECO_X86_64
    ARCH_TARGET = amd64

else ifeq ($(ARCH), rv32)
    TOOL = $(TOOLCHAIN)$(SLASH)eco-riscv-none-elf-gcc-13.2.0-$(HOST_OS)$(SLASH)bin$(SLASH)riscv-none-elf-
    CCFLAGS_ARCH = -DECO_RISCV_32
    ARCH_TARGET = rv32

else ifeq ($(ARCH), rv64)
    TOOL = $(TOOLCHAIN)$(SLASH)eco-riscv-none-elf-gcc-13.2.0-$(HOST_OS)$(SLASH)bin$(SLASH)riscv-none-elf-
    CCFLAGS_ARCH = -DECO_RISCV_64
    ARCH_TARGET = rv64

else ifeq ($(ARCH), mips)
    TOOL = $(TOOLCHAIN)$(SLASH)eco-mips-none-elf-gcc-13.2.0-$(HOST_OS)$(SLASH)bin$(SLASH)mips-none-elf-
    CCFLAGS_ARCH = -DECO_MIPS
    ARCH_TARGET = mips

else ifeq ($(ARCH), aarch32)
    TOOL = $(TOOLCHAIN)$(SLASH)eco-arm-none-eabi-gcc-13.2.0-$(HOST_OS)$(SLASH)bin$(SLASH)arm-none-eabi-
    CCFLAGS_ARCH = -DECO_AARCH32
    ARCH_TARGET = armv7

else ifeq ($(ARCH), aarch64)
    TOOL = $(TOOLCHAIN)$(SLASH)eco-aarch64-none-elf-gcc-13.2.0-$(HOST_OS)$(SLASH)bin$(SLASH)aarch64-none-elf-
    CCFLAGS_ARCH = -DECO_AARCH64
    ARCH_TARGET = armv8

else ifeq ($(ARCH), avr)
    TOOL = $(TOOLCHAIN)$(SLASH)eco-avr-none-elf-gcc-13.2.0-$(HOST_OS)$(SLASH)bin$(SLASH)avr-
    CCFLAGS_ARCH = -DECO_AVR
    ARCH_TARGET = avr

else ifeq ($(ARCH), avr_e)
    TOOL = $(TOOLCHAIN)$(SLASH)eco-avr-none-elf-gcc-13.2.0-$(HOST_OS)$(SLASH)bin$(SLASH)avr-
    CCFLAGS_ARCH = -DECO_AVR_E
    ARCH_TARGET = avre
else
    $(error Unknown ARCH: $(ARCH). Please check your ARCH variable)
endif

# --- Target Configuration (Static vs Dynamic) ---
ifeq ($(TARGET), 1)
    # Static Library (.a)
    OUT_TARGET = lib011BF193DAAC48E0B83B3A8618D23C4C.a
    BASE_TARGET = Static
    CCFLAGS_BASE = -DECO_LIB -DUGUID_UTILITY -DECO_OS
    # Static link command (archive)
    CMD_LINK = $(AR) rcs $@ $(OBJ)
else
    # Dynamic Library (.so)
    OUT_TARGET = 011BF193DAAC48E0B83B3A8618D23C4C.so
    BASE_TARGET = Dynamic
    CCFLAGS_BASE = -DECO_DLL -fPIC -DUGUID_UTILITY -DECO_OS
    # Dynamic link command (shared object)
    CMD_LINK = $(CC) -shared -o $@ $(OBJ) $(LDFLAGS) $(LIBS)
endif

# --- Output Directory Mapping ---
CONFIG_DIR = $(if $(filter 1,$(DEBUG)),Debug,Release)
OUT_DIR = ..$(SLASH)..$(SLASH)..$(SLASH)BuildFiles$(SLASH)$(PLATFORM)$(SLASH)$(ARCH_TARGET)$(SLASH)$(BASE_TARGET)$(CONFIG_DIR)

# --- Compilation Flags ---
# Includes
INC = -I "$(ECO_FRAMEWORK)$(SLASH)Eco.Core1$(SLASH)SharedFiles" \
-I "$(ECO_FRAMEWORK)$(SLASH)Eco.InterfaceBus1$(SLASH)SharedFiles" \
-I "$(ECO_FRAMEWORK)$(SLASH)Eco.MemoryManager1$(SLASH)SharedFiles" \
-I "$(ECO_FRAMEWORK)$(SLASH)Eco.FileSystemManagement1$(SLASH)SharedFiles" \
-I "$(ECO_FRAMEWORK)$(SLASH)Eco.System1$(SLASH)SharedFiles" \
      -I ..$(SLASH)..$(SLASH)..$(SLASH)HeaderFiles \
      -I ..$(SLASH)..$(SLASH)..$(SLASH)SharedFiles

# Global Compiler Flags (Bare-metal optimization)
CCFLAGS = -Wall -fms-extensions -ffreestanding $(CCFLAGS_BASE) $(CCFLAGS_ARCH) $(INC)

# Debug vs Release Flags
ifeq ($(DEBUG), 1)
    CCFLAGS += -g -DDEBUG
else
    CCFLAGS += -O2 -DNDEBUG
endif

# Linker flags and Bare-metal libraries
LDFLAGS = $(LDFLAGS_ARCH)
LIBS = -lgcc

# --- Tool Definitions ---
CC = $(TOOL)gcc
AR = $(TOOL)gcc-ar

# --- Source and Object Files ---
SRC = ..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoLab1.c \
      ..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoLab1Factory.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoPolyAberth.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoPolyAberthFactory.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoPolyLaguerre.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoPolyLaguerreFactory.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoCalculatorA.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoCalculatorAFactory.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoCalculatorB.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoCalculatorBFactory.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoCalculatorC.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoCalculatorCFactory.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoCalculatorD.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoCalculatorDFactory.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoCalculatorE.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoCalculatorEFactory.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoLab2.c \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoLab2Factory.c

OBJ = $(SRC:.c=.o)

# --- Build Rules ---
.PHONY: all clean

all: $(OUT_DIR)$(SLASH)$(OUT_TARGET)

# Compile C/C++ source files to object files
.c.o:
ifeq ($(OS), Windows_NT)
	@if not exist "$(call FIX_PATH,$(OUT_DIR))" mkdir "$(call FIX_PATH,$(OUT_DIR))"
else
	@mkdir -p $(OUT_DIR)
endif
	$(CC) $(CCFLAGS) -c $< -o $@

# Link object files into the final target
$(OUT_DIR)$(SLASH)$(OUT_TARGET): $(OBJ)
	$(CMD_LINK)
	@$(RM) $(call FIX_PATH,$(OBJ))

# Clean build artifacts
clean:
	$(RM) $(call FIX_PATH,$(OBJ))
	$(RM) $(call FIX_PATH,$(OUT_DIR)$(SLASH)$(OUT_TARGET))
