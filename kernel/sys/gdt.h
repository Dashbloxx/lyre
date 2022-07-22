#ifndef _SYS__GDT_H
#define _SYS__GDT_H

#include <stdint.h>
#include <sys/cpu.h>

void gdt_init(void);
void gdt_reload(void);
void gdt_load_tss(struct tss *tss);

#endif
