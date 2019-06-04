
#ifndef __SOS_BASE_H_
#define __SOS_BASE_H_

#include "types.h"

extern "C" int __attribute__((cdecl)) printf(const char *format, ...);
extern "C" void __attribute__((stdcall)) *memcpy(void *dst, void *src, uint32_t size);

#endif // __SOS_BASE_H_
