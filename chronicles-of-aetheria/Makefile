# =============================================================
# Chronicles of Aetheria — devkitARM Makefile
#
# Requires:
#   devkitARM (part of devkitPro)
#   Set DEVKITPRO and DEVKITARM environment variables:
#     export DEVKITPRO=/opt/devkitpro
#     export DEVKITARM=$(DEVKITPRO)/devkitARM
#
# Usage:
#   make          — build chronicles.gba
#   make clean    — remove build artifacts
#   make run      — build and run in mGBA (if installed)
# =============================================================

# ---- Tool Paths -------------------------------------------
# Supports both devkitPro and system arm-none-eabi toolchain
DEVKITPRO ?= /opt/devkitpro
DEVKITARM ?= $(DEVKITPRO)/devkitARM

ifneq ($(wildcard $(DEVKITARM)/bin/arm-none-eabi-gcc),)
PREFIX  := $(DEVKITARM)/bin/arm-none-eabi-
else
PREFIX  := arm-none-eabi-
endif

CC      := $(PREFIX)gcc
AS      := $(PREFIX)gcc
LD      := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy

# ---- Target -----------------------------------------------
TARGET  := chronicles
ELF     := $(TARGET).elf
GBA     := $(TARGET).gba

# ---- Source Directories -----------------------------------
SRCDIR  := src
INCDIR  := include

# ---- Source Files (auto-discovered) -----------------------
SRCS_C := $(wildcard $(SRCDIR)/*.c)              \
           $(wildcard $(SRCDIR)/engine/*.c)       \
           $(wildcard $(SRCDIR)/data/*.c)         \
           $(wildcard $(SRCDIR)/game/*.c)          \
           $(wildcard $(SRCDIR)/game/battle/*.c)  \
           $(wildcard $(SRCDIR)/game/character/*.c) \
           $(wildcard $(SRCDIR)/game/item/*.c)      \
           $(wildcard $(SRCDIR)/game/quest/*.c)     \
           $(wildcard $(SRCDIR)/game/ui/*.c)

CRT0_S  := src/crt0.s
CRT0_O  := src/crt0.o

OBJS_C := $(SRCS_C:.c=.o)
OBJS   := $(CRT0_O) $(OBJS_C)

# ---- Compiler Flags ---------------------------------------
# -marm         : generate ARM (not Thumb) for IWRAM code
# -mthumb-interwork : allow calling between ARM and Thumb
# -mcpu=arm7tdmi : exact GBA CPU target
# -Os           : optimize for size (ROM budget matters)
# -fno-strict-aliasing : common GBA pattern of type-punning
ARCH    := -marm -mthumb-interwork -mcpu=arm7tdmi -mfloat-abi=soft

CFLAGS  := $(ARCH)                              \
            -Os                                 \
            -Wall                               \
            -Wextra                             \
            -fno-strict-aliasing                \
            -fomit-frame-pointer                \
            -ffunction-sections                 \
            -fdata-sections                     \
            -I$(INCDIR)                         \
            -I$(INCDIR)/gba                     \
            -I$(INCDIR)/engine                  \
            -I$(INCDIR)/game                    \
            -I$(INCDIR)/game/battle             \
            -I$(INCDIR)/game/character          \
            -I$(INCDIR)/game/item               \
            -I$(INCDIR)/game/quest              \
            -I$(INCDIR)/game/ui                 \
            -I$(INCDIR)/game/dialogue           \
            -I$(INCDIR)/data                    \


# ---- Linker Flags -----------------------------------------
# Use local gba.ld when devkitPro is not present
ifneq ($(wildcard $(DEVKITPRO)/libgba/gba.ld),)
LDSCRIPT := $(DEVKITPRO)/libgba/gba.ld
LDLIBS   := -L$(DEVKITPRO)/libgba/lib -lgba
else
LDSCRIPT := gba.ld
LDLIBS   := /usr/lib/gcc/arm-none-eabi/13.2.1/libgcc.a
endif

LDFLAGS := $(ARCH)                    \
            -T$(LDSCRIPT)             \
            -Wl,--gc-sections         \
            -Wl,-Map,$(TARGET).map    \
            -nostdlib

# ---- GBA ROM Header fix -----------------------------------
GBAFIX  := $(DEVKITPRO)/tools/bin/gbafix

# ---- crt0 assembly ----------------------------------------
CRT0_S  := src/crt0.s
CRT0_O  := src/crt0.o

SRCS_S  := $(CRT0_S)

# ---- Build Rules ------------------------------------------
.PHONY: all clean run

all: $(GBA)

$(GBA): $(ELF)
	$(OBJCOPY) -O binary -R .bss -R .ewram.bss -R .iwram.bss $< $@
	@if [ -f "$(GBAFIX)" ]; then $(GBAFIX) $@ -t"Aetheria" -c"CAEGBA" -m96; fi
	@echo ""
	@echo "  Built: $@"
	@ls -lh $@

$(CRT0_O): $(CRT0_S)
	$(AS) $(ARCH) -c -o $@ $<

$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# C compilation
$(SRCDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(SRCDIR)/engine/%.o: $(SRCDIR)/engine/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(SRCDIR)/game/%.o: $(SRCDIR)/game/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(SRCDIR)/game/battle/%.o: $(SRCDIR)/game/battle/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(SRCDIR)/game/character/%.o: $(SRCDIR)/game/character/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(SRCDIR)/game/item/%.o: $(SRCDIR)/game/item/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(SRCDIR)/game/quest/%.o: $(SRCDIR)/game/quest/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(SRCDIR)/game/ui/%.o: $(SRCDIR)/game/ui/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

# ---- Utility ----------------------------------------------
clean:
	rm -f $(OBJS_C) $(CRT0_O) $(ELF) $(GBA) $(TARGET).map

run: $(GBA)
	mgba-qt $(GBA)

# Show memory usage
size: $(ELF)
	$(PREFIX)size $(ELF)

# Disassemble for debugging
disasm: $(ELF)
	$(PREFIX)objdump -d $(ELF) | less
