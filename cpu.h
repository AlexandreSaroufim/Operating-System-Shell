#include "pcb.h"

struct ReadyQueue {
    struct PCB pcb;
    struct ReadyQueue *next;
};

int runCPU(struct ReadyQueue *pcbQueue);