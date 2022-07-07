int interpreter(char* command_args[], int args_size);
int help();
int badcommand();
struct PCB *loadNewPage( struct PCB *pcb, int i);