
cmake_minimum_required(VERSION 3.7)

project(sos LANGUAGES ASM CXX)

# Sources files
file(GLOB ASM "asm/*.s")
file(GLOB_RECURSE SRCS "src/*.h" "src/*.cpp")
file(GLOB LINKER_SCRIPT "linker.ld")

set_source_files_properties(${ASM} PROPERTIES COMPILE_FLAGS "-m32")

set(BIN_NAME sos_kernel)
set(IMAGE_NAME "${BIN_NAME}.img")
set(ISO_NAME sos.iso)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-g -Wall -m32 -nostdlib -ffreestanding -fno-stack-check -mno-stack-arg-probe -fno-use-cxa-atexit -fno-builtin -fno-leading-underscore -fno-rtti -fno-exceptions -fno-stack-protector")
set(CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT}")
set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(${BIN_NAME} ${ASM} ${SRCS} src/std/stdio.h)
target_include_directories(${BIN_NAME} PUBLIC include src)

# create kernel image
add_custom_command(TARGET ${BIN_NAME} POST_BUILD
        COMMAND objcopy -O elf32-i386 ${BIN_NAME} ${IMAGE_NAME}
        COMMAND mkdir -p iso/boot/grub
        COMMAND cp ${IMAGE_NAME} iso/boot
        )

# create bootable iso image
if (WIN32)
    add_custom_command(TARGET ${BIN_NAME} POST_BUILD
            COMMAND cp ../boot/grub/stage2_eltorito iso/boot/grub

            COMMAND echo 'default=0' >> iso/boot/grub/menu.lst
            COMMAND echo 'timeout=0' >> iso/boot/grub/menu.lst
            COMMAND echo 'title SOS' >> iso/boot/grub/menu.lst
            COMMAND echo 'kernel /boot/${IMAGE_NAME}' >> iso/boot/grub/menu.lst

            COMMAND mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o ${ISO_NAME} iso
            )
else ()
    add_custom_command(TARGET ${BIN_NAME} POST_BUILD
            COMMAND echo 'set timeout=0' >> iso/boot/grub/grub.cfg
            COMMAND echo 'set default=0' >> iso/boot/grub/grub.cfg
            COMMAND echo '' >> iso/boot/grub/grub.cfg
            COMMAND echo 'menuentry "sbw OS" {' >> iso/boot/grub/grub.cfg
            COMMAND echo '  multiboot /boot/${IMAGE_NAME}' >> iso/boot/grub/grub.cfg
            COMMAND echo '  boot' >> iso/boot/grub/grub.cfg
            COMMAND echo '}' >> iso/boot/grub/grub.cfg

            COMMAND grub-mkrescue --output=${ISO_NAME} iso
            )
endif()

# clear
add_custom_command(TARGET ${BIN_NAME} POST_BUILD COMMAND rm -rf iso ${IMAGE_NAME})

