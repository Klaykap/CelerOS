#!/bin/bash
nasm -f bin source/bootloader/bootloader.asm -o source/bootloader/bootloader.bin #bootloader

nasm -f elf32 source/celeros.asm -o source/compile/celeros_asm.bin #assembler part

gcc -m32 -c -fno-stack-protector -std=gnu99 -ffreestanding -fno-PIC source/celeros.c -o source/compile/celeros_c.bin

ld -m elf_i386 --oformat=binary -T source/compile/link.ld -o source/compile/celeros.bin source/compile/celeros_asm.bin source/compile/celeros_c.bin

dd if=source/bootloader/bootloader.bin of=celeros.img conv=notrunc seek=0
dd if=source/compile/celeros.bin of=celeros.img conv=notrunc seek=1

cp celeros.img cdiso/celeros.img
mkdir cdiso
mkisofs -o celeros.iso -b celeros.img cdiso/

#bochs -f /home/vendelin/bochs
#qemu-system-i386 -soundhw sb16 -boot a -fda celeros.img -hda hard_disc.img

sleep 50
