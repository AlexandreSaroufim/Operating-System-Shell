#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "interpreter.h"
#include "shellmemory.h"
#include "shell.h"

struct PCB {
    int pid;
    int start_position;
    int length;
    int curr_instruction;
    int jobScore;
    struct PCB *next;
    int pageTable[100];
    int currPage;
    int curPageLine;
    int pagesInMemory;
    FILE *f;
};

// Construct PCB
struct PCB *construct_pcb(int start_pos, int length, FILE *fp) 
{

    struct PCB *pcb = malloc(sizeof(struct PCB));

	// if program runs out of memory when using malloc	
	if(pcb == NULL) exit(1); // terminate with code one
	
	pcb->pid = rand();
	pcb->start_position = start_pos;
	pcb->length = length;
    pcb->jobScore = length;
	pcb->next = NULL;
    pcb->currPage=0;
    pcb->curPageLine=0;
    pcb->pagesInMemory=1;
    pcb->f=fp;
    int counter=0;


    for ( int i=0; i< length ; i=i+3){
        pcb->pageTable[counter]=start_pos+i;
        counter++;
    }

    for ( int j =counter; counter<100;counter++){
        pcb->pageTable[counter]=-1;
    }

	return pcb;
}

// Add PCB 
struct PCB *insertPCB(struct PCB *head, struct PCB *newPCB) {

    if (head == NULL) {       // if head of list is null, add to the front of the list
        newPCB->next = head;
        head = newPCB;

    } else {
        struct PCB *temp = head; // temporary pointer that points to head

        // ITERATE UNTIL YOU FIND AN EMPTY SLOT
	    while(temp->next != NULL) {
		    temp = temp->next;
	    }
	    temp->next = newPCB;
        newPCB->next = NULL;
    }
    return head;
}

struct PCB * sortLL2(struct PCB *head){
    struct PCB * temp=head;
    if (temp->jobScore > temp->next->jobScore){

        head=head->next;
        head->next=temp;
        head->next->next=NULL;
	}
    return head;
}

struct PCB * sortLL3(struct PCB *head){
    struct PCB * temp=head;
    struct PCB * temp2;
    //printf("sort1\n");
    if (temp->jobScore > temp->next->jobScore){
        head=head->next;
        temp2=head->next;
        head->next=temp;
        head->next->next=temp2;
	}
    //printf("sort2\n");
    temp=head;
    if (temp->jobScore > temp->next->next->jobScore){
        head=head->next->next;
        head->next=temp->next;
        head->next->next=temp;
        head->next->next->next=NULL;
	}
    //printf("sort3\n");

    temp=head->next;
    if ( temp->jobScore > temp->next->jobScore){
        head->next=head->next->next;
        head->next->next=temp;
        head->next->next->next=NULL;
    }
    //printf("sort4\n");
    return head;
}

struct PCB *moveToBack(struct PCB *head)
{
    struct PCB *temp = head; // temporary pointer that points to head
    struct PCB *tail = NULL;
    struct PCB *temp2 = head;
    
    while(1)
    {
        //printf("TEMP id is : %d\n", head->pid);
        if (temp->next != NULL) {
            temp = temp->next;
        }
        tail = temp;
        break;
    }

    //printf("the length of head is : %d\n", head->length);
    //printf("the id of head is : %d\n", head->pid);
    temp = head->next;       // REMOVE HEAD, NEW HEAD IS NEXT PCB
    //printf("Next the length of head is : %d\n", temp->length);
    //printf("Next the id of head is : %d\n", temp->pid);
    tail->next = temp2;
    tail->next->next=NULL;

    return temp;
}