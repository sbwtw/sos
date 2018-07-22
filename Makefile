
GPPPARAMS = -m32 -nostdlib -fno-use-cxa-atexit -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -fno-stack-protector
ASPARAMS = --32
LDPARAMS = -melf_i386

target = sos.iso
objects = loader.o kernel.o gdt.o port.o interrupts.o interrupt_stubs.o builtin_functions.o keyboard.o

%.o: %.cpp
	g++ $(GPPPARAMS) -o $@ -c $<

%.o: %.s
	as $(ASPARAMS) -o $@ $<

sos.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

$(target): sos.bin
	rm -rf iso
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot/

	echo 'set timeout=0' >> iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "sbw OS" {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/sos.bin' >> iso/boot/grub/grub.cfg
	echo '  boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg

	grub-mkrescue --output=$@ iso

	rm -rf iso

run: $(target)
	qemu-system-x86_64 -boot d -cdrom $< -m 512

.PHONY: clean

clean:
	rm -rf $(objects) $(target) sos.bin iso