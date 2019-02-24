#include "cpu.h"

void cpu__wait(void)
{
    __asm__ volatile ("wfi");
}
