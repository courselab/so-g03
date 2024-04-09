as --32 eg-02_att.S -o eg-02_att.o

ld -melf_i386 --oformat=binary -e begin -Ttext=0x7c00 eg-02_att.o -o eg-02_att.bin

qemu-system-i386 -drive format=raw,file=eg-02_att.bin -net none