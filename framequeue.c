#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "interpreter.h"
#include "shellmemory.h"
#include "shell.h"

struct Frame {
    int location;
    struct Frame *next;
};

// Construct PCB
struct Frame *construct_FQ(int loc) 
{

    struct Frame *frame = malloc(sizeof(struct Frame));

	// if program runs out of memory when using malloc	
	if(frame == NULL) exit(1); // terminate with code one

    frame->location = loc;
    frame->next = NULL;

	return frame;
}

// Add PCB 
struct Frame *insertFrame(struct Frame *head, struct Frame *newFrame) {

    if (head == NULL) {       // if head of list is null, add to the front of the list
        newFrame->next = head;
        head = newFrame;

    } else {
        struct Frame *temp = head; // temporary pointer that points to head

        // ITERATE UNTIL YOU FIND AN EMPTY SLOT
	    while(temp->next != NULL) {
		    temp = temp->next;
	    }
	    temp->next = newFrame;
        newFrame->next = NULL;
    }
    return head;
}


// struct PCB *moveToBack(struct PCB *head)
// {
//     struct PCB *temp = head; // temporary pointer that points to head
//     struct PCB *tail = NULL;
//     struct PCB *temp2 = head;
    
//     while(1)
//     {
//         //printf("TEMP id is : %d\n", head->pid);
//         if (temp->next != NULL) {
//             temp = temp->next;
//         }
//         tail = temp;
//         break;
//     }

//     //printf("the length of head is : %d\n", head->length);
//     //printf("the id of head is : %d\n", head->pid);
//     temp = head->next;       // REMOVE HEAD, NEW HEAD IS NEXT PCB
//     //printf("Next the length of head is : %d\n", temp->length);
//     //printf("Next the id of head is : %d\n", temp->pid);
//     tail->next = temp2;
//     tail->next->next=NULL;

//     return temp;
// }