#ifndef KBENCH_H
#define KBENCH_H
#include "kbench.h"
#include <time.h>

int main(int argc, char *argv[]);
void testInsert();
void testGet(phashmap &map);
void testRemove(phashmap &map);
void testRemove2(phashmap &map);
double diffclock(clock_t clock1, clock_t clock2);

#endif
