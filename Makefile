OBJECTS = multiboot.asm.o fb.o loader.asm.o kernel.o \
					io.asm.o serial.o string.o interrupt.asm.o \
					descriptor_tables.o descriptor_tables.asm.o isr.o \
					apic.o keyboard.o paging.o paging.asm.o memory.o
CC = gcc
CFLAGS = -m32 -fno-stack-protector \
					-ffreestanding \
					-Wall -Wextra -Wno-unused -Werror -g -c \
					-DDEBUG=4
LDFLAGS = -T link.ld -melf_i386 -L/usr/lib/gcc/i686-linux-gnu/4.6 
AS = nasm
ASFLAGS = -f elf

all: kernel.elf os.iso

run: os.iso
	bochs -f bochsrc.txt -q

os.iso: kernel.elf kernel.sym
	cp kernel.elf iso/boot/
	genisoimage -R \
              -b boot/grub/stage2_eltorito    \
              -no-emul-boot                   \
              -boot-load-size 4               \
              -A os                           \
              -input-charset utf8             \
              -quiet                          \
              -boot-info-table                \
              -o os.iso                       \
              iso

kernel.elf: $(OBJECTS) log.h
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.asm.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

dump: kernel.elf
	readelf -WS kernel.elf
	objdump -D -Mintel-mnemonics kernel.elf

kernel.sym: kernel.elf
	objdump -D kernel.elf | grep -e '^[^ ]* <' | sed -e 's/<//' -e 's/>://' > kernel.sym

clean:
	rm -f kernel.elf kernel.sym iso/boot/kernel.elf *.o os.iso

