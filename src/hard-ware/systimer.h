#ifndef SOS_SYSTIMER_H
#define SOS_SYSTIMER_H

#include "std/types.h"

/**
 * 8253/8254 PIT 芯片
 * */

void init_timer(uint32_t frequency);

#endif //SOS_SYSTIMER_H
