
cmake_minimum_required(VERSION 3.7)

project(sos LANGUAGES ASM CXX)

# Sources files
file(GLOB ASM "asm/*.s")
file(GLOB INCLUDES "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.h" "src/*.cpp")
file(GLOB LINKER "linker.ld")

set_source_files_properties(${ASM} PROPERTIES COMPILE_FLAGS "-m32")

set(BIN_NAME sos_kernel)
set(ISO_NAME sos.iso)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-g -Wall -m32 -nostdlib -ffreestanding -fno-stack-check -mno-stack-arg-probe -fno-use-cxa-atexit -fno-builtin -fno-leading-underscore -fno-rtti -fno-exceptions -fno-stack-protector")
set(CMAKE_EXE_LINKER_FLAGS "-T${LINKER}")
#set(CMAKE_EXE_LINKER_FLAGS "-mi386pe -nostdlib --nmagic -T${LINKER}")
#set(CMAKE_CXX_LINK_EXECUTABLE "ld ${CMAKE_EXE_LINKER_FLAGS} <OBJECTS> -o <TARGET> ")
set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(${BIN_NAME} ${ASM} ${INCLUDES} ${SOURCES})
target_include_directories(${BIN_NAME} PUBLIC include src)
add_custom_command(TARGET ${BIN_NAME} POST_BUILD
        COMMAND objcopy -O elf32-i386 ${BIN_NAME} ${BIN_NAME}.elf
        COMMAND mkdir -p iso/boot/grub
        COMMAND cp ${BIN_NAME}.elf iso/boot

        COMMAND echo 'set timeout=0' >> iso/boot/grub/grub.cfg
        COMMAND echo 'set default=0' >> iso/boot/grub/grub.cfg
        COMMAND echo '' >> iso/boot/grub/grub.cfg
        COMMAND echo 'menuentry "sbw OS" {' >> iso/boot/grub/grub.cfg
        COMMAND echo '  multiboot /boot/${BIN_NAME}.elf' >> iso/boot/grub/grub.cfg
        COMMAND echo '  boot' >> iso/boot/grub/grub.cfg
        COMMAND echo '}' >> iso/boot/grub/grub.cfg

        COMMAND genisoimage -R -b boot/grub/grub.cfg -no-emul-boot -boot-load-size 4 -boot-info-table -o ${ISO_NAME} iso
        COMMAND rm -rf iso)
