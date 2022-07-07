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

struct PCB *construct_pcb(int start_pos, int length, FILE *fp);
struct PCB *insertPCB(struct PCB *head, struct PCB *newPCB);
struct PCB * sortLL2(struct PCB *head);
struct PCB * sortLL3(struct PCB *head);
struct PCB *moveToBack(struct PCB *head);