TARGET  = jogo_da_velha
PREFIX  = arm-none-eabi-
CC      = $(PREFIX)gcc
AS      = $(PREFIX)as
OBJCOPY = $(PREFIX)objcopy

CFLAGS  = -mthumb-interwork -marm -O2 -fno-builtin -nostdlib \
          -march=armv4t -mcpu=arm7tdmi -Wall

LDFLAGS = -T gba.ld -nostdlib -lgcc

OBJS = crt0.o src/main.o

all: $(TARGET).gba

crt0.o: crt0.s
	$(CC) -x assembler-with-cpp -marm -march=armv4t -c $< -o $@

src/main.o: src/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(TARGET).gba: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	python3 fix_header.py $@
	@echo "ROM gerada: $(TARGET).gba"

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).gba
