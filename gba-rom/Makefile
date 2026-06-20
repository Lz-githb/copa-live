TARGET = gba10games

CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-as
LD      = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

CFLAGS  = -mthumb-interwork -marm -march=armv4t -O2 -fno-builtin -nostdlib \
           -fomit-frame-pointer -Wall -Wno-unused-function
LIBGCC  = /usr/lib/gcc/arm-none-eabi/13.2.1/arm/v5te/softfp/libgcc.a
LDFLAGS = -T gba.ld

SRCS    = src/crt0.s src/main.c
OBJS    = crt0.o main.o

all: $(TARGET).gba

crt0.o: src/crt0.s
	$(CC) -c -marm -march=armv4t -o $@ $<

main.o: src/main.c src/gba.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET).elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBGCC)

$(TARGET).gba: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	@echo "ROM gerada: $(TARGET).gba"
	@ls -lh $(TARGET).gba

clean:
	rm -f *.o *.elf *.gba
