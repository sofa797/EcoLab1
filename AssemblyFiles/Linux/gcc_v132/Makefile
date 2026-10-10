# --- Host Platform Detection ---
ifeq ($(OS), Windows_NT)
    RM = -rm -f
    SLASH = /
    ECO_TOOLCHAIN_ENV := $(if $(ECO_TOOLCHAIN),$(subst \,/,$(ECO_TOOLCHAIN)),)
else
    RM = rm -f
    SLASH = /
    ECO_TOOLCHAIN_ENV := $(ECO_TOOLCHAIN)
endif

# --- Default Project Configurations ---
TARGET ?= 1
ARCH ?= x86_64
DEBUG ?= 0
PLATFORM = Linux

# --- Toolchain, Path and Architecture Selection ---
ifeq ($(ARCH), x86_64)
    CCFLAGS_ARCH = -DECO_X86_64
    ARCH_TARGET = x86_64
    ECO_PKG_ID = eco-x86_64-linux-gnu-gcc
    ECO_PKG_VER = 13.2.0
    ifneq ($(ECO_TOOLCHAIN_ENV),)
        TOOL = x86_64-linux-gnu-
    endif
else ifeq ($(ARCH), x86_32)
    CCFLAGS_ARCH = -DECO_X86_32 -m32
    ARCH_TARGET = x86_32
    ECO_PKG_ID = eco-x86_32-linux-gnu-gcc
    ECO_PKG_VER = 13.2.0
    ifneq ($(ECO_TOOLCHAIN_ENV),)
        TOOL = i386-linux-gnu-
    endif
else ifeq ($(ARCH), rv64)
    CCFLAGS_ARCH = -DECO_RISCV_64
    ARCH_TARGET = rv64
    ECO_PKG_ID = eco-riscv64-linux-gnu-gcc
    ECO_PKG_VER = 13.2.0
    ifneq ($(ECO_TOOLCHAIN_ENV),)
        TOOL = riscv64-linux-gnu-
    endif
else ifeq ($(ARCH), aarch64)
    CCFLAGS_ARCH = -DECO_AARCH64
    ARCH_TARGET = armv8
    ECO_PKG_ID = eco-aarch64-linux-gnu-gcc
    ECO_PKG_VER = 13.2.0
    ifneq ($(ECO_TOOLCHAIN_ENV),)
        TOOL = aarch64-linux-gnu-
    endif
else
    $(error Unknown ARCH: $(ARCH). Please check your ARCH variable)
endif

ifneq ($(ECO_TOOLCHAIN_ENV),)
    TOOLCHAIN_SUBDIR = $(if $(ECO_PKG_ID),$(ECO_PKG_ID)-$(ECO_PKG_VER)$(SLASH),)
    TOOLCHAIN = $(ECO_TOOLCHAIN_ENV)$(SLASH)$(TOOLCHAIN_SUBDIR)bin$(SLASH)
    SYSROOT = --sysroot=$(ECO_TOOLCHAIN_ENV)$(SLASH)$(TOOLCHAIN_SUBDIR)
else
    TOOL =
    TOOLCHAIN =
    SYSROOT =
endif

# --- Tool Definitions ---
CC_c = $(TOOLCHAIN)$(TOOL)gcc
CC_cpp = $(TOOLCHAIN)$(TOOL)g++
AR = $(TOOLCHAIN)$(TOOL)gcc-ar

LANG_STD_c = -std=c89
LANG_STD_cpp = -std=c++11

CC = $(CC_c)
LINK_CC = $(CC_c)
LANG_STD_FLAGS = $(LANG_STD_c)

# --- Target Configuration (Static vs Dynamic) ---
ifeq ($(TARGET), 1)
    # Static Library (.a)
    OUT_TARGET = lib011BF193DAAC48E0B83B3A8618D23C4C.a
    BASE_TARGET = Static
    CCFLAGS_BASE = -DECO_LIB
    CMD_LINK = $(AR) rcs $@ $(OBJ)
else
    # Dynamic Library (.so)
    OUT_TARGET = 011BF193DAAC48E0B83B3A8618D23C4C.so
    BASE_TARGET = Dynamic
    CCFLAGS_BASE = -DECO_DLL -fPIC
    CMD_LINK = $(LINK_CC) -shared -o $@ $(OBJ) $(LDFLAGS) $(LIBS)
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

# Global Compiler Flags (-DECO_LINUX )
CCFLAGS = $(SYSROOT) -Wall $(CCFLAGS_BASE) $(CCFLAGS_ARCH) -DUGUID_UTILITY -DECO_LINUX $(LANG_STD_FLAGS) $(INC)

# Debug vs Release Flags
ifeq ($(DEBUG), 1)
    CCFLAGS += -g -DDEBUG
else
    CCFLAGS += -O2 -DNDEBUG
endif

# Linker flags and Bare-metal libraries
LDFLAGS = $(SYSROOT) $(LDFLAGS_ARCH)
LIBS = -lgcc

# --- Source and Object Files ---
SRC = ..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoLab1.c \
      ..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoLab1Factory.c

OBJ = $(SRC:.c=.o)

# --- Build Rules ---
.PHONY: all clean

all: $(OUT_DIR)$(SLASH)$(OUT_TARGET)

# Compile C/C++ source files to object files
.c.o:
	@mkdir -p $(OUT_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@

# Link object files into the final target
$(OUT_DIR)$(SLASH)$(OUT_TARGET): $(OBJ)
	$(CMD_LINK)
	@$(RM) $(OBJ)

# Clean build artifacts
clean:
	$(RM) $(OBJ)
	$(RM) $(OUT_DIR)$(SLASH)$(OUT_TARGET)
