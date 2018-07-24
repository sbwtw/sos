
cmake_minimum_required(VERSION 3.7)

project(sos CXX ASM)

# Sources files
file(GLOB ASM "*.s")
file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")
file(GLOB LINKER "linker.ld")

set(BIN_NAME sos_kernel)
set(ISO_NAME sos.iso)
#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "-Wall -m32 -nostdlib -fno-use-cxa-atexit -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -fno-stack-protector")
set(CMAKE_EXE_LINKER_FLAGS "-T ${LINKER}")

set_source_files_properties(${ASM} PROPERTIES COMPILE_FLAGS "-m32")

add_executable(${BIN_NAME} ${ASM} ${HEADERS} ${SOURCES})
add_custom_command(TARGET ${BIN_NAME} POST_BUILD
                   COMMAND mkdir -p iso/boot/grub
                   COMMAND cp ${BIN_NAME} iso/boot

                   COMMAND echo 'set timeout=0' >> iso/boot/grub/grub.cfg
                   COMMAND echo 'set default=0' >> iso/boot/grub/grub.cfg
                   COMMAND echo '' >> iso/boot/grub/grub.cfg
                   COMMAND echo 'menuentry "sbw OS" {' >> iso/boot/grub/grub.cfg
                   COMMAND echo '  multiboot /boot/${BIN_NAME}' >> iso/boot/grub/grub.cfg
                   COMMAND echo '  boot' >> iso/boot/grub/grub.cfg
                   COMMAND echo '}' >> iso/boot/grub/grub.cfg

                   COMMAND grub-mkrescue --output=${ISO_NAME} iso
                   COMMAND rm -rf iso)

add_custom_target(launch-qemu qemu-system-x86_64 -boot d -cdrom ${ISO_NAME} -m 512 & DEPENDS ${BIN_NAME})