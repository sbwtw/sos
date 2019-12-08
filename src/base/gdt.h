
#ifndef __GDT_H_
#define __GDT_H_

#include "std/types.h"

void gdt_init();

uint16_t get_code_segment();
uint16_t get_data_segment();

#endif // __GDT_H_
