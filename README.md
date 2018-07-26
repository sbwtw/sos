
# SOS - sbw's Operating System
It's just a toy for learning OS technology.

# Build from source

## Build dependencies
- g++
- cmake
- grub-mkrescue
- xorriso

## Build
__Must using g++ as compiler, clang++ or other else is NOT support yet.__ for using Ninja as generator, just add argument `-G "Ninja"` in cmake commandline.
```shell
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=g++ -DCMAKE_ASM_COMPILER=g++ ..
make # or ninja when using Ninja as generator
```

When build done, you can found `sos.iso` and `sos_kernel` in your build dir.

`sos.iso` is a bootable image so you can start from it using VirtualBox or QEMU.

# Try SOS in virtual machine

## Runtime dependencies
- QEMU

## Run
```shell
make launch-qemu
```

# Screenshots
![sos screenshots](screenshots/2018-07-21-115918_746x484_scrot.png)

# License
This project is licensed under the [MIT](LICENSE) license.
