#include "systimer.h"
#include "base/port.h"

/**
 *  0x43:
 *  Bits        Usage
 *  6~7         Select channel:
 *                  0 0 = Channel 0
 *                  0 1 = Channel 1
 *                  1 0 = Channel 2
 *                  1 1 = Read-back command (8254 only)
 *
 *  4~5         Access model: 
 *                  0 0 = Latch count value command
 *                  0 1 = Access mode: lobyte only
 *                  1 0 = Access mode: hibyte only
 *                  1 1 = Access mode: lobyte/hibyte
 *
 *  1~3         Operating mode:
 *                  0 0 0 = Mode 0 (interrupt on terminal count)
 *                  0 0 1 = Mode 1 (hardware re-triggerable one-shot)
 *                  0 1 0 = Mode 2 (rate generator)
 *                  0 1 1 = Mode 3 (square wave generator)
 *                  1 0 0 = Mode 4 (software triggered strobe)
 *                  1 0 1 = Mode 5 (hardware triggered strobe)
 *                  1 1 0 = Mode 2 (rate generator, same as 010b)
 *                  1 1 1 = Mode 3 (square wave generator, same as 011b)
 *  0           BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
 * */

void init_timer(uint32_t frequency) {
    uint32_t divisor = 11931800 / frequency;

    // D7 D6 D5 D4 D3 D2 D1 D0
    // 0  0  1  1  0  1  1  0
    // 即 36 H
    // 设置 8253/8254 芯片工作在模式 3 下
    outb(0x43, 0x36);

    // 拆分低字节和高字节
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t hign = (uint8_t)((divisor >> 8) & 0xFF);

    // 分别写入低字节和高字节
    outb(0x40, low);
    outb(0x40, hign);
}
