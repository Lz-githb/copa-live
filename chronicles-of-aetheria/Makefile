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
DEVKITPRO ?= /opt/devkitpro
DEVKITARM ?= $(DEVKITPRO)/devkitARM

PREFIX  := $(DEVKITARM)/bin/arm-none-eabi-
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
           $(wildcard $(SRCDIR)/game/*.c)          \
           $(wildcard $(SRCDIR)/game/battle/*.c)  \
           $(wildcard $(SRCDIR)/game/character/*.c) \
           $(wildcard $(SRCDIR)/game/item/*.c)

OBJS := $(SRCS_C:.c=.o)

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
            -I$(DEVKITPRO)/libgba/include

# ---- Linker Flags -----------------------------------------
# gba.ld    : devkitPro linker script placing code at 0x08000000
# --gc-sections : remove unused code (essential for size)
LDSCRIPT := $(DEVKITPRO)/libgba/gba.ld

LDFLAGS := $(ARCH)                    \
            -T$(LDSCRIPT)             \
            -Wl,--gc-sections         \
            -Wl,-Map,$(TARGET).map    \
            -nostartfiles             \
            -L$(DEVKITPRO)/libgba/lib \
            -lgba

# ---- GBA ROM Header fix -----------------------------------
# gbafix patches the ROM header checksum so hardware boots it
GBAFIX  := $(DEVKITPRO)/tools/bin/gbafix

# ---- Build Rules ------------------------------------------
.PHONY: all clean run

all: $(GBA)

$(GBA): $(ELF)
	$(OBJCOPY) -O binary $< $@
	@if [ -f "$(GBAFIX)" ]; then $(GBAFIX) $@ -t"Aetheria" -c"CAEGBA" -m96; fi
	@echo ""
	@echo "  Built: $@"
	@ls -lh $@

$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

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

# ---- Utility ----------------------------------------------
clean:
	rm -f $(OBJS) $(ELF) $(GBA) $(TARGET).map

run: $(GBA)
	mgba-qt $(GBA)

# Show memory usage
size: $(ELF)
	$(PREFIX)size $(ELF)

# Disassemble for debugging
disasm: $(ELF)
	$(PREFIX)objdump -d $(ELF) | less
