# Configurations

# Platform detection
PLATFORM = Windows

ifeq ($(OS), Windows_NT)
	#WINDOWS USE THESE DEFINITIONS
#	RM = del /Q /F
	RM = -rm -f
#	SLASH = \\
	SLASH = /
	SWALLOW_OUTPUT = >nul 2>nul
    FRAMEWORK := $(subst \,/,$(ECO_FRAMEWORK))	
	TOOLCHAIN = $(subst \,/,$(ECO_TOOLCHAIN))
	TOOL = 
	MSSDK = $(MSSDK_ROOT)
else
	#LINUX USE THESE DEFINITIONS
	RM = -rm -f
	SLASH = /
	SWALLOW_OUTPUT =
	TOOLCHAIN = 
	TOOL = 
endif 

# Target configuration
ifeq ($(TARGET), 1)
	OUT_TARGET = 011BF193DAAC48E0B83B3A8618D23C4C.lib
	BASE_TARGET = Static
    CCFLAGS_TARGET = /Gz /TC /W4 /Zc:__cplusplus /D_CRT_SECURE_NO_WARNINGS
    CCFLAGS_TARGET += /D__STDC_VERSION__=199901L /DUGUID_UTILITY /DECO_WINDOWS /DECO_LIB
    LDFLAGS_TARGET = /NOLOGO /SUBSYSTEM:CONSOLE /INCREMENTAL:NO
else
	OUT_TARGET = 011BF193DAAC48E0B83B3A8618D23C4C.dll
	BASE_TARGET = Dynamic
    CCFLAGS_TARGET = /Gz /TC /W4 /Zc:__cplusplus /D_CRT_SECURE_NO_WARNINGS
    CCFLAGS_TARGET += /D__STDC_VERSION__=199901L /DUGUID_UTILITY /DECO_WINDOWS /DECO_DLL
    LDFLAGS_TARGET = /NOLOGO /SUBSYSTEM:CONSOLE /INCREMENTAL:NO
endif

# Debug symbols configuration
ifeq ($(DEBUG), 1)
	CONFIG_TARGET = $(BASE_TARGET)Debug
    CCFLAGS_TARGET += /D_DEBUG /Zi /Fd$(subst /,$(SLASH),$(OUT_DIR)/$(OUT_TARGET:.lib=.pdb))
    LDFLAGS_TARGET += 
    ifeq ($(TARGET), 1)
        CCFLAGS_TARGET += /MTd
    else
        LDFLAGS_TARGET += /PDB:$(subst /,$(SLASH),$(OUT_DIR)/$(OUT_TARGET:.dll=.pdb))
    endif
else
	CONFIG_TARGET = $(BASE_TARGET)Release
    CCFLAGS_TARGET += /DNDEBUG /Z7
    ifeq ($(TARGET), 1)
        CCFLAGS_TARGET += /MT
    endif
    LDFLAGS_TARGET += /DEBUG:NONE
endif

# Architecture selection
ifeq ($(ARCH), x86_32)
	CCFLAGS_TARGET += /DECO_X86_32
	LDFLAGS_TARGET += /MACHINE:X86
	ARCH_TARGET = x86
else ifeq ($(ARCH), x86)
	CCFLAGS_TARGET += /DECO_X86_32
	LDFLAGS_TARGET += /MACHINE:X86
	ARCH_TARGET = x86
else ifeq ($(ARCH), x86_64)
	CCFLAGS_TARGET += /DECO_X86_64  
	LDFLAGS_TARGET += /MACHINE:X64
	ARCH_TARGET = amd64
else ifeq ($(ARCH), x64)
	CCFLAGS_TARGET += /DECO_X86_64  
	LDFLAGS_TARGET += /MACHINE:X64
	ARCH_TARGET = amd64
endif

# Device selection
ifdef DEVICE
    DEVICE_TARGET = $(DEVICE)$(SLASH)
endif

# Include directories
INC = /I "$(ECO_FRAMEWORK)$(SLASH)Eco.Core1$(SLASH)SharedFiles" \
/I "$(ECO_FRAMEWORK)$(SLASH)Eco.InterfaceBus1$(SLASH)SharedFiles" \
/I "$(ECO_FRAMEWORK)$(SLASH)Eco.MemoryManager1$(SLASH)SharedFiles" \
/I "$(ECO_FRAMEWORK)$(SLASH)Eco.FileSystemManagement1$(SLASH)SharedFiles" \
/I "$(ECO_FRAMEWORK)$(SLASH)Eco.System1$(SLASH)SharedFiles"\
/I ..$(SLASH)..$(SLASH)..$(SLASH)HeaderFiles \
/I ..$(SLASH)..$(SLASH)..$(SLASH)SharedFiles

# Source files.
SRC = \
..$(SLASH)..$(SLASH)..$(SLASH)SourceFiles$(SLASH)CEcoLab1.c \
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

# Libraries
LIBS = "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib"

# Object files
OBJ = $(SRC:.c=.o)

OUT = $(OUT_TARGET)
OUT_DIR = ..$(SLASH)..$(SLASH)..$(SLASH)BuildFiles$(SLASH)$(PLATFORM)$(SLASH)$(ARCH_TARGET)$(SLASH)$(DEVICE_TARGET)$(CONFIG_TARGET)

# C/C++ compiler flags
CCFLAGS = $(CCFLAGS_TARGET)
 
 # Compile flags
LDFLAGS = $(LDFLAGS_TARGET)

# Tools
CC = cl
LD = link
AR = lib

ifeq ($(TARGET), 1)
	CMD_TARGET=$(AR) /OUT:$(OUT_DIR)$(SLASH)$(OUT) $(OBJ) $(LDFLAGS)
else
	CMD_TARGET=$(LD) /DLL $(LIBS) /OUT:$(OUT_DIR)$(SLASH)$(OUT) $(OBJ) $(LDFLAGS)
endif
 
.SUFFIXES: .c

all:  $(OUT)

.c.o:
	mkdir -p "$(subst /,\,$(OUT_DIR))"
	$(CC) $(INC) $(CCFLAGS) /c $< /Fo$@
 
$(OUT): $(OBJ)
	$(CMD_TARGET)
	$(RM) $(OBJ)

clean:
	$(RM) $(OBJ) $(OUT_DIR)$(SLASH)$(OUT) $(OUT_DIR)$(SLASH)*.pdb $(OUT_DIR)$(SLASH)*.ilk
