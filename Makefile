################################################################################
# Stefan Bylund 2017
#
# Makefile for compiling layer 2 demo program for Sinclair ZX Spectrum Next.
# Supports both the SCCZ80 and SDCC compilers.
################################################################################

MKDIR := mkdir -p

RM := rm -rf

ZXNEXT_LAYER2 := ../zxnext_layer2

ZXNEXT_LAYER2_INCLUDE := $(ZXNEXT_LAYER2)/include

ZXNEXT_LAYER2_LIB_SCCZ80 := $(ZXNEXT_LAYER2)/lib/sccz80

ZXNEXT_LAYER2_LIB_SDCC_IX := $(ZXNEXT_LAYER2)/lib/sdcc_ix

ZXNEXT_LAYER2_LIB_SDCC_IY := $(ZXNEXT_LAYER2)/lib/sdcc_iy

SRCS := src/zxnext_layer2_demo.c

BINDIR_SCCZ80 := bin/sccz80

BINDIR_SDCC_IX := bin/sdcc_ix

BINDIR_SDCC_IY := bin/sdcc_iy

ifeq ($(BUILD_SNA),true)
SNA := -Cz"--sna"
endif

DEBUGFLAGS := --list --c-code-in-asm

all: all_sccz80 all_sdcc_ix all_sdcc_iy

all_sccz80:
	$(MKDIR) $(BINDIR_SCCZ80)
	zcc +zx -vn -O3 -startup=30 -clib=new -m $(DEBUG) -L$(ZXNEXT_LAYER2_LIB_SCCZ80) -lzxnext_layer2 -I$(ZXNEXT_LAYER2_INCLUDE) $(SRCS) -o $(BINDIR_SCCZ80)/zxnext_layer2_demo -create-app $(SNA)

all_sdcc_ix:
	$(MKDIR) $(BINDIR_SDCC_IX)
	zcc +zx -vn -SO3 -startup=30 -clib=sdcc_ix -m $(DEBUG) --max-allocs-per-node200000 -L$(ZXNEXT_LAYER2_LIB_SDCC_IX) -lzxnext_layer2 -I$(ZXNEXT_LAYER2_INCLUDE) $(SRCS) -o $(BINDIR_SDCC_IX)/zxnext_layer2_demo -create-app $(SNA)

all_sdcc_iy:
	$(MKDIR) $(BINDIR_SDCC_IY)
	zcc +zx -vn -SO3 -startup=30 -clib=sdcc_iy -m $(DEBUG) --max-allocs-per-node200000 -L$(ZXNEXT_LAYER2_LIB_SDCC_IY) -lzxnext_layer2 -I$(ZXNEXT_LAYER2_INCLUDE) $(SRCS) -o $(BINDIR_SDCC_IY)/zxnext_layer2_demo -create-app $(SNA)

debug_sccz80: DEBUG = $(DEBUGFLAGS)

debug_sdcc_ix: DEBUG = $(DEBUGFLAGS)

debug_sdcc_iy: DEBUG = $(DEBUGFLAGS)

debug_sccz80: all_sccz80

debug_sdcc_ix: all_sdcc_ix

debug_sdcc_iy: all_sdcc_iy

clean:
	$(RM) bin zcc_opt.def zcc_proj.lst src/*.lis
