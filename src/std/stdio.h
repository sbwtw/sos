//
// Created by sbw on 11/2/19.
//

#ifndef SOS_STDIO_H
#define SOS_STDIO_H

#include "types.h"

extern "C" int __attribute__((cdecl)) printf(const char *format, ...);
extern "C" void __attribute__((stdcall)) *memcpy(void *dst, void *src, uint32_t size);
extern "C" void __attribute__((stdcall)) *memset(void *dst, int c, uint32_t n);

#endif //SOS_STDIO_H
