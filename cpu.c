#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "interpreter.h"
#include "shellmemory.h"
#include "shell.h"
#include "pcb.h"


struct ReadyQueue {
    struct PCB pcb;
    struct ReadyQueue *next;
};

int runCPU(struct ReadyQueue *pcbQueue) { // RUNS PROCESSES ON CPU. YOU PASS IT A POINTER TO AN ALREADY BUILT READY QUEUE 
    int errCode = 0;
    struct ReadyQueue *temp = pcbQueue; // Declare temporary pointer to perform computations on QUEUE

    while (temp != NULL) { // WHILE THE QUEUE IS NOT NULL
        // need to extract start_position, length, curr_instruction in order to run a specific process
 
        char *line = malloc(256); // variable which will hold each instrctuion line in a given process

        int process_start_pos = temp->pcb.start_position; 
        int process_length = temp->pcb.length;
        int process_instruction = temp->pcb.curr_instruction;
        int end_of_process = process_length + process_start_pos;
        char location[3]; // ranges from 0 to 999 therefore can have a maximum length of 3

        while(process_instruction != end_of_process) { // WHILE CURR INSTRUCTRION HASN'T REACHED THE END OF THE PROCESS
            sprintf(location, "%d", process_instruction); // get process instruction number as a string
            line = mem_get_value(location); // get each instruction and store it in line
            // printf("%s\n", line);
            errCode = parseInput(line);
            memset(line, 0, sizeof(line)); // sets to NULL
            free(line);
            process_instruction = process_instruction + 1; // increment to move onto next instruction
        }

        // ONCE DONE WITH ONE SCRIPT, NEED TO CLEAR SCRIPT SOURCE CODE FROM MEMORY
        clear_mem();

        temp = temp->next; // move onto next process in pcbQueue

        
    }


    return 0;
}