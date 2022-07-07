#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <dirent.h>
#include <ftw.h>

#include "shellmemory.h"
#include "shell.h"
#include "cpu.h"
#include "interpreter.h"
#include "framequeue.h"



int MAX_ARGS_SIZE = 10;
int help();
int get_m();
int quit();
int isalphaNum(char *val);
int my_ls();
int badcommand();
int badcommandTooManyTokens();
int set(char *var, char *value);
int print(char *var);
int echo(char *var);
int run(char *script);
int exec_fcfs(int num_args, ...);
int badcommandFileDoesNotExist();
int get_num_lines(char *fileName);
int printFile(char *filename);
int run_RR(struct PCB *pcb_list[], int num_of_scripts);
int exec_RR(int num_args, ...);
struct PCB *load_to_mem(FILE *fp, int start_pos);
void freeList(struct PCB* head);
struct PCB* run_two_lines(struct PCB *pcb, struct Frame *frame);
int isDone(struct PCB *pcb);
int execAGING(int num_args, ...);
struct PCB* runOneline(struct PCB *pcb);
int get_m_index();
int set_fs(char *var, char *value);
int resetMem();
int badcommandIdenticalArgs();
struct PCB * createLL(struct PCB *pcb_list[], int num_of_scripts);
int LRU=0;

struct Frame *frame_head = NULL;

// Interpret commands and their arguments
int interpreter(char *command_args[], int args_size)
{
	int i;

	if (args_size < 1 || args_size > MAX_ARGS_SIZE)
	{
		return badcommand();
	}

	for (i = 0; i < args_size; i++)
	{ // strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help") == 0)
	{
		// help
		if (args_size != 1)
			return badcommand();
		return help();
	}
	else if (strcmp(command_args[0], "quit") == 0)
	{
		// quit
		if (args_size != 1)
			return badcommand();
		return quit();
	}
	else if (strcmp(command_args[0], "NoCommand") == 0)
	{
		return 0;
	}
	else if (strcmp(command_args[0], "set") == 0)
	{
		// set
		if (args_size < 3)
			return badcommand();
		if (args_size > 7)
			return badcommandTooManyTokens();
		char *value = (char *)calloc(1, 150);
		char spaceChar = ' ';

		for (int i = 2; i < args_size; i++)
		{
			strncat(value, command_args[i], 30);
			if (i < args_size - 1)
			{
				strncat(value, &spaceChar, 1);
			}
		}
		return set(command_args[1], value);
	}
	else if (strcmp(command_args[0], "print") == 0)
	{
		if (args_size != 2)
			return badcommand();
		return print(command_args[1]);
	}
	else if (strcmp(command_args[0], "run") == 0)
	{
		if (args_size != 2)
			return badcommand();

		return run(command_args[1]);
	}
	else if (strcmp(command_args[0], "echo") == 0)
	{
		// echo
		if (args_size != 2 || strlen(command_args[1]) > 99)
			return badcommand(); // Might have to fix this
		return echo(command_args[1]);
	}
	else if (strcmp(command_args[0], "my_ls") == 0)
	{
		// my_ls
		if (args_size != 1)
			return badcommand(); // Might have to fix this
		return my_ls();
	}
	else if (strcmp(command_args[0], "exec") == 0)
	{
		if (args_size < 3)
			return badcommand();
		if (args_size > 5)
			return badcommandTooManyTokens();
		char *policy = command_args[args_size - 1];

		//Aging
		if ((strcmp(policy, "AGING") == 0)){
			if (args_size == 3)
			{
				return exec_fcfs(args_size, command_args[1]);
			}

			if (args_size == 4)
			{ // IF TWO process are passed
				// CONSTRUCT PCB LINKED LIST WHICH WILL ACT AS READYQUEUE
				return execAGING(args_size, command_args[1], command_args[2]);
			}

			if (args_size == 5)
			{
				return execAGING(args_size, command_args[1], command_args[2], command_args[3]);
			}
		}


		// RR
		if ((strcmp(policy, "RR") == 0))
		{
			if (args_size == 3)
			{
				return run(command_args[1]);
			}

			if (args_size == 4)
			{ // IF TWO process are passed
				// CONSTRUCT PCB LINKED LIST WHICH WILL ACT AS READYQUEUE
				return exec_RR(args_size, command_args[1], command_args[2]);
			}

			if (args_size == 5)
			{
				return exec_RR(args_size, command_args[1], command_args[2], command_args[3]);
			}
		}

		// FCFS
		if (strcmp(policy, "FCFS") == 0)
		{
			if (args_size == 3)
			{
				return exec_fcfs(args_size, command_args[1]);
			}
			else if (args_size == 4)
			{
				return exec_fcfs(args_size, command_args[1], command_args[2]);
			}
			else
			{
				return exec_fcfs(args_size, command_args[1], command_args[2], command_args[3]);
			}
		}

		// SJF
		if ((strcmp(policy, "SJF") == 0))
		{
			if (args_size == 3)
			{ // 1 SCRIPT PASSED IN
				return exec_fcfs(args_size, command_args[1]);
			}
			else if (args_size == 4)
			{ // 2 SCRIPTS PASSED IN
				int num_of_lines1 = get_num_lines(command_args[1]);
				int num_of_lines2 = get_num_lines(command_args[2]);

				if (num_of_lines2 < num_of_lines1)
				{
					return exec_fcfs(args_size, command_args[2], command_args[1]);
				}
				else
				{
					return exec_fcfs(args_size, command_args[1], command_args[2]);
				}
			}
			else
			{ // 3 SCRIPTS PASSED IN

				int num_of_lines1 = get_num_lines(command_args[1]);
				int num_of_lines2 = get_num_lines(command_args[2]);
				int num_of_lines3 = get_num_lines(command_args[3]);

				if (num_of_lines2 < num_of_lines1 && num_of_lines3 < num_of_lines2)
				{ // 3 2 1
					return exec_fcfs(args_size, command_args[3], command_args[2], command_args[1]);
				}

				if (num_of_lines1 <= num_of_lines2 && num_of_lines3 < num_of_lines1)
				{ // 3 1 2
					return exec_fcfs(args_size, command_args[3], command_args[1], command_args[2]);
				}

				if (num_of_lines3 < num_of_lines1 && num_of_lines2 <= num_of_lines3)
				{ // 2 3 1
					return exec_fcfs(args_size, command_args[2], command_args[3], command_args[1]);
				}

				if (num_of_lines1 <= num_of_lines3 && num_of_lines2 < num_of_lines1)
				{ // 2 1 3
					return exec_fcfs(args_size, command_args[2], command_args[1], command_args[3]);
				}

				if (num_of_lines3 < num_of_lines2 && num_of_lines1 <= num_of_lines3)
				{ // 1 3 2
					return exec_fcfs(args_size, command_args[1], command_args[3], command_args[2]);
				}

				if (num_of_lines2 <= num_of_lines3 && num_of_lines1 <= num_of_lines2)
				{ // 1 2 3
					return exec_fcfs(args_size, command_args[1], command_args[2], command_args[3]);
				}
			}
		}
	}
	else if (strcmp(command_args[0], "print_file") == 0)
	{
		return printFile(command_args[1]);
	}
	else if (strcmp(command_args[0], "resetmem") == 0)
	{
		return resetMem();
	}
	else if (strcmp(command_args[0], "get_m") == 0)
	{
		return get_m_index();
	}
	else
		return badcommand();
}

int isalphaNum(char *val)
{
	char *temp;

	for (temp = val; *temp != '\0'; temp++)
	{
		if (isalnum(*temp))
		{
			continue;
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

int help()
{

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
	printf("%s\n", help_string);
	return 0;
}

int quit()
{
	printf("%s\n", "Bye!");
	system("rm -r ./backingStore");
	exit(0);
}

int badcommandIdenticalArgs()
{
	printf("%s\n", "Bad Command: At least 2 of the arguments passed are identical");
	return 1;
}

int badcommand()
{
	printf("%s\n", "Unknown Command");
	return 1;
}

int badcommandTooManyTokens()
{
	printf("%s\n", "Bad command: Too many tokens");
	return 1;
}

// For run command only
int badcommandFileDoesNotExist()
{
	printf("%s\n", "Bad command: File not found");
	return 3;
}

int set(char *var, char *value)
{

	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, value);

	vs_set_value(var, value);

	return 0;
}

int set_fs(char *var, char *value)
{

	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, value);

	fs_set_value(var, value);

	return 0;
}

int my_ls()
{
	char command[50];
	strcpy(command, "ls -1");
	system(command);

	return 0;
}

int echo(char *var)
{
	if (var[0] == '$')
	{
		var++;
		printf("%s\n", mem_get_value(var));
	}
	else
	{
		printf("%s\n", var);
	}
	return 0;
}

int resetMem()
{
	reset_VS();
	return 0;
}

int print(char *var)
{
	printf("%s\n", mem_get_value(var));
	return 0;
}

int exec_fcfs(int num_args, ...)
{ // CAN TAKE UP TO 4 ARGUMENTS
	va_list args;
	va_start(args, num_args);

	for (int i = 0; i < num_args - 2; i++)
	{
		char *process = va_arg(args, char *);
		run(process);
	}
	va_end(args);

	return 0;
}

int run(char *script)
{
	struct PCB *process_list[1];

	// LOAD SCRIPTS TO BACKING STORE DIRECTORY
	char f_command[50] = "cp ";
	char* backingStore = " backingStore/file1.txt";
	strcat(f_command, script);
	strcat(f_command, backingStore);    // need to do -> "cp prog1.txt backinStore"
	system(f_command);

	FILE *fp_1 = fopen("backingStore/file1.txt", "rt");

	process_list[0] = load_to_mem(fp_1, 0);
		
	run_RR(process_list, 1);

	fclose(fp_1);
	return 0;

}

struct PCB *load_to_mem(FILE *fp, int start_pos)
{
	// OPEN SCRIPT
	char line[100]; // EACH LINE WILL NOT BE LONGER THAN 100 CHARS

	fgets(line, 999, fp);
	// printf("FIRST LINE: %s\n", line);
	// exit(0);
	int i = start_pos;
	// printf("THIS IS THE VALUE OF I : %d\n", i);

	
	char position[3];

	int j=0;
	int alarm=0;

	//2 pages
	while (j<6)
	{
		sprintf(position, "%d", i); // get char equivalent of index
		set_fs(position, line);

		
		j++;
		i++;
		if (feof(fp))
		{
			alarm=-1;
			break;
		}
		
		fgets(line, 999, fp);
	}

	int k=i;
	while(j != 3 && j < 6){
		sprintf(position, "%d", k); // get char equivalent of index
		set_fs(position, "NoCommand"  );
		k++;
		j++;
	}

	//change
	if(j>=6 && alarm!=-1){
		while(1){
			i++;
			if (feof(fp))
			{
				break;
			}
			
			
			fgets(line, 999, fp);
		}
	}


	// CONSTRUCT A PCB ASSOCIATED TO THIS PROCESS AND RETURN IT
	int length = i - start_pos;
	
	// printf("File pointer value: %d\n", fp);
	struct PCB *pcb = construct_pcb(start_pos, length , fp);
	return pcb;
}

struct PCB *loadNewPage( struct PCB *pcb, int i)
{

	// GET NEW LINE TO ADD TO FRAME STORE IN MEM 
	// printf("pcb->currPage is %d\n",pcb->currPage );
	// printf("PCB FILE POINTER VALUE: %d\n", pcb->f);
	// printf("NEW PCB ID: %d\n", pcb->pid);  // SO PCB ID IS CORRECT 

	char line[100];
	rewind(pcb->f);

	for(int i=0; i<pcb->curr_instruction; i++ )
	{
		fgets(line, 999, pcb->f);
	}
	
	fgets(line, 999, pcb->f);

	// printf("FIRST LINE BACK: %s\n", line);

	// exit(0);
	
	char position[3];

	int j=0;
	int alarm=0;

	//1 pages
	while (j<3)
	{
		// printf("Here\n");
		sprintf(position, "%d", i); // get char equivalent of index
		// printf("position: %s\n", position);
		set_fs(position, line);
		
		if (j == 0)
		{
			pcb->pageTable[pcb->currPage] = i;
		}


		j++;
		i++;
		if (feof(pcb->f))
		{
			alarm=-1;
			break;
		}
		
		fgets(line, 999, pcb->f);
		// printf("PAGE FAULT -> NEW LINE: %s\n", line);
	}

	int k=i;
	while(j != 3 && j < 6){
		sprintf(position, "%d", k); // get char equivalent of index
		set_fs(position, "NoCommand"  );
		k++;
		j++;
	}

	//change
	if(j>=6 && alarm!=-1){
		while(1){
			i++;
			if (feof(pcb->f))
			{
				break;
			}
			
			
			fgets(line, 999, pcb->f);
		}
	}
	// get_m_index();

	pcb->curPageLine=0;
	pcb->pagesInMemory=pcb->pagesInMemory+1;
	return pcb;
}

int execAGING(int num_args, ...){
	// CAN TAKE UP TO 4 ARGUMENTS
	va_list args;
	va_start(args, num_args);
	const char *script_arr[2];
	struct PCB *process_list[num_args - 2];

	if (num_args == 4){
	
		for (int i = 0; i < num_args - 2; i++)
		{
			char *script = va_arg(args, char *);
			script_arr[i] = script;    // STORE IN SCRIPT ARRAY
		}
		va_end(args);

		// LOAD SCRIPT TO MEM
		FILE *fp_1 = fopen(script_arr[0], "rt");
		FILE *fp_2 = fopen(script_arr[1], "rt");

		process_list[0] = load_to_mem(fp_1, 0);
		process_list[1] = load_to_mem(fp_2, process_list[0]->length);

		// OKAY, SCRIPT LINES ARE IN MEMORY AND WE HAVE THE PCBS in process_list
		struct PCB *head = createLL(process_list, 2);
		//Now I should sort
		head=sortLL2(head);
		

		while (1) {
			if (isDone(head) == 1){
				break;
			}

			runOneline(head);
			// printf("%d" , head->jobScore);
			// break;
			if(head->next->jobScore>0){
				head->next->jobScore=head->next->jobScore-1;
			}
			head=sortLL2(head);
			
			
		}
		fclose(fp_1);
		fclose(fp_2);

	}

	if (num_args == 5){
		
		for (int i = 0; i < num_args - 2; i++)
		{
			char *process = va_arg(args, char *);
			script_arr[i] = process;
		}
		va_end(args);

		// LOAD SCRIPT TO MEM
		FILE *fp_1 = fopen(script_arr[0], "rt");
		FILE *fp_2 = fopen(script_arr[1], "rt");
		FILE *fp_3 = fopen(script_arr[2], "rt");

		process_list[0] = load_to_mem(fp_1, 0);
		process_list[1] = load_to_mem(fp_2, process_list[0]->length);
		process_list[2] = load_to_mem(fp_3, (process_list[1]->start_position + process_list[1]->length));

		// OKAY, SCRIPT LINES ARE IN MEMORY AND WE HAVE THE PCBS in process_lisT
		struct PCB *head = createLL(process_list, 3);

		//Now I should sort
		head=sortLL3(head);

		while (1) {

			if (isDone(head) == 1){
				break;
			}

			runOneline(head);
			// printf("%d" , head->jobScore);
			// break;
			if(head->next->jobScore>0){
				head->next->jobScore=head->next->jobScore-1;
			}
			if(head->next->next->jobScore>0){
				head->next->next->jobScore=head->next->next->jobScore-1;
			}

			head=sortLL3(head);
			//printf("Testing...\n");
			
		}

		fclose(fp_1);
		fclose(fp_2);
		fclose(fp_3);

	}
	

	return 0;

}

struct PCB* runOneline(struct PCB *pcb) 
{
	int i = 0;
	char *line;
	char position[3];
	while(i < 1 && pcb->curr_instruction < pcb->length) {
		// GET LINE AND PARSE IT
		int location = pcb->start_position + pcb->curr_instruction;
		sprintf(position, "%d", location);
		line = mem_get_value(position);  // line contains instruction that we need to execute
		parseInput(line);
		i++;
		pcb->curr_instruction = pcb->curr_instruction + 1;
	}
	if(pcb->curr_instruction == pcb->length){
		pcb->jobScore=1000;
	}
	return pcb;
}

struct PCB * createLL(struct PCB *pcb_list[], int num_of_scripts){
	struct PCB *head = NULL;
	for (int i=0; i < num_of_scripts; i++)
	{
		head = insertPCB(head, pcb_list[i]);
		// int test = head->pid;
	}
	return head;
}

int exec_RR(int num_args, ...)
{ // CAN TAKE UP TO 4 ARGUMENTS
	va_list args;
	va_start(args, num_args);
	const char *script_arr[2];
	struct PCB *process_list[num_args - 2];

	if (num_args == 4)
	{
		for (int i = 0; i < num_args - 2; i++)
		{
			char *script = va_arg(args, char *);
			script_arr[i] = script;    // STORE IN SCRIPT ARRAY --> CONTIANS NAMES OF SCRIPTS (I.E. PROG1.TXT)
		}
		va_end(args);

		// LOAD SCRIPTS TO BACKING STORE DIRECTORY
		for(int i=0; i<2; i++) {
			if (i == 0)
			{
				char f_command[50] = "cp ";
				char* backingStore = " backingStore/file1.txt";
				strcat(f_command, script_arr[i]);
				strcat(f_command, backingStore);    // need to do -> "cp prog1.txt backinStore"
				system(f_command);
			}

			if(i == 1)
			{
				char f_command[50] = "cp ";
				char* backingStore = " backingStore/file2.txt";
				strcat(f_command, script_arr[i]);
				strcat(f_command, backingStore);    // need to do -> "cp prog1.txt backinStore"
				system(f_command);
			}
			
		}

		FILE *fp_1 = fopen("backingStore/file1.txt", "rt");
		FILE *fp_2 = fopen("backingStore/file2.txt", "rt");

		process_list[0] = load_to_mem(fp_1, 0);
		if(process_list[0]->length<=3){
			process_list[1] = load_to_mem(fp_2, 3);
		}
		else{
			process_list[1] = load_to_mem(fp_2, 6);
		}
		
		
		// OKAY, SCRIPT LINES ARE IN MEMORY AND WE HAVE THE PCBS in process_list
		run_RR(process_list, 2);

		fclose(fp_1);
		fclose(fp_2);

		return 0;
	}

	if (num_args == 5)
	{
		
		for (int i = 0; i < num_args - 2; i++)
		{
			char *process = va_arg(args, char *);
			script_arr[i] = process;    // STORE IN SCRIPT ARRAY --> CONTIANS NAMES OF SCRIPTS (I.E. PROG1.TXT)
		}
		va_end(args);

		// LOAD SCRIPTS TO BACKING STORE DIRECTORY
		for(int i=0; i<3; i++) {
			if (i == 0)
			{
				char f_command[50] = "cp ";
				char* backingStore = " backingStore/file1.txt";
				strcat(f_command, script_arr[i]);
				strcat(f_command, backingStore);    // need to do -> "cp prog1.txt backinStore"
				system(f_command);
			}

			if(i == 1)
			{
				char f_command[50] = "cp ";
				char* backingStore = " backingStore/file2.txt";
				strcat(f_command, script_arr[i]);
				strcat(f_command, backingStore);    // need to do -> "cp prog1.txt backinStore"
				system(f_command);
			}

			if(i == 2)
			{
				char f_command[50] = "cp ";
				char* backingStore = " backingStore/file3.txt";
				strcat(f_command, script_arr[i]);
				strcat(f_command, backingStore);    // need to do -> "cp prog1.txt backinStore"
				system(f_command);
			}
		}

		// STEP 1: OPEN FILES IN BACKING STORE DIRECTORY

		FILE *fp_1 = fopen("backingStore/file1.txt", "rt");
		FILE *fp_2 = fopen("backingStore/file2.txt", "rt");
		FILE *fp_3 = fopen("backingStore/file3.txt", "rt");

		process_list[0] = load_to_mem(fp_1, 0);
		if(process_list[0]->length<=3) {
			process_list[1] = load_to_mem(fp_2, 3);
			if (process_list[1]->length <= 3) {
				process_list[2] = load_to_mem(fp_3, 6);
			} else {
				process_list[2] = load_to_mem(fp_3, 9);
			}
		}
		else{
			process_list[1] = load_to_mem(fp_2, 6);
			if (process_list[1]->length <= 3) {
				process_list[2] = load_to_mem(fp_3, 9);
			} else {
				process_list[2] = load_to_mem(fp_3, 12);
			}
		}

		// OKAY, SCRIPT LINES ARE IN MEMORY AND WE HAVE THE PCBS in process_list
		run_RR(process_list, 3);

		fclose(fp_1);
		fclose(fp_2);
		fclose(fp_3);

		return 0;
	}
}

int run_RR(struct PCB *pcb_list[], int num_of_scripts)
{
	struct PCB *head = NULL;
	for (int i=0; i < num_of_scripts; i++)
	{
		head = insertPCB(head, pcb_list[i]);
		// int test = head->pid;
	}
	// NOW WE HAVE OUR READY QUEUE (LINKED LIST OF PCBS)
	// NOW EXECUTE 2 LINES OF CODE PER SCRIPT

	
	while(1) {
		struct PCB *temp = head;

		//printf("the length of pcb is : %d\n", temp->length);
		if (isDone(temp) == 1)
		{
			break;
		}
		while(temp != NULL) {    // ITERATE ACROSS LINKED LIST AND PRINT OUT 2 LINES OF CODE FOR EACH PCB
			temp = run_two_lines(temp, frame_head);
			temp = temp->next;   // GO TO NEXT PCB
		}
	}
	freeList(head);
	return 0;
}

int isDone(struct PCB *pcb)
{
	struct PCB *temp1 = pcb;
	int num_pcbs = 0;
	int check = 0;

	while(temp1 != NULL) {
		num_pcbs++;
		temp1 = temp1->next;
	}

	struct PCB *temp2 = pcb;
	while(temp2 != NULL) {

		// IF ALL PCBS ARE DONE -> RETURN 1

		//change
		//if(temp2->curr_instruction == temp2->length || temp2->currPage>1)
		if(temp2->curr_instruction == temp2->length)
		check++;
		temp2 = temp2->next;
	}

	if (check == num_pcbs) {
		return 1;
	} else return 0;

}



struct PCB* run_two_lines(struct PCB *pcb, struct Frame *frame) 
{
	int i = 0;
	char *line;
	char position[3];

	
	while(i < 2 && pcb->curr_instruction < pcb->length ) {
		
		if (pcb->currPage > pcb->pagesInMemory){
			

			// IF THERE IS A PAGE AVAILABLE IN FRAME STORE
			// printf("page")
			if ( isPageAvailable()!=-1 ){
				
				struct PCB *temp=pcb;
				pcb=loadNewPage(temp, isPageAvailable() );
				
				// pcb=moveToBack(temp);
				
				break;
			}
			//if it is full remove the least recent frame store used and replace
			else{
				// printf("pcb length: %d\n", pcb->length);
				FILE *fp =pcb->f;
				char line[100];

				rewind(fp);
				for (int counter=0; counter<=pcb->currPage*3; counter++){
					fgets(line, 999, fp);
					// printf("LINE: %s\n", line);
				}

				printf("Page fault! Victim page contents:\n");
				LRU = frame_head->location;

				for(int counter=0; counter<3; counter++){

					if (counter == 0)
					{
						pcb->pageTable[pcb->currPage] = LRU;
					}

					char* victim;
					victim=mem_get_value_at_index(LRU);
					printf("%s\n", victim);

					// printf("CURRENT LINE: %s\n", line);
					memSetValue( line, LRU);
					
					LRU++;
					if (feof(fp))
					{
						strcpy(line, "NoCommand");
					} else {
						fgets(line, 999, fp);
					}
					
				}
				frame_head = frame_head->next;
				printf("End of victim page contents.\n");
				pcb->pagesInMemory=pcb->pagesInMemory+1;

				break;
				
			}
			pcb->pagesInMemory=pcb->pagesInMemory+1;

			break;
		}

		int location = pcb->pageTable[pcb->currPage]+pcb->curPageLine;
		
		if (pcb->curPageLine == 0)
		{
			struct Frame* newFrame = construct_FQ(location);
			frame_head = insertFrame(frame, newFrame);
		}
		
		sprintf(position, "%d", location);
		line = mem_get_value(position);  // line contains instruction that we need to execute
		parseInput(line);
		i++;
		pcb->curr_instruction = pcb->curr_instruction + 1;
		pcb->curPageLine= pcb->curPageLine+1;
		if (pcb->curPageLine==3){
			pcb->curPageLine=0;
			pcb->currPage=pcb->currPage+1;
		}
	}
	
	return pcb;
}

void freeList(struct PCB* head) 
{ // function that frees allocated memory

	struct PCB *temp = NULL; // temporary pointer
    while(head != NULL) 
	{ // iterate through linked list
        temp = head; // temporary pointer points to head 
        head = head->next; // go onto next node
        free(temp); // free memory
    }
}

int get_num_lines(char *fileName)
{
	FILE *fp = fopen(fileName, "r");
	int numOfLines = 0;
	char ch;

	if (fp == NULL)
	{
		printf("Could not open file %s", fileName);
		return 0;
	}

	ch = getc(fp);
	while (ch != EOF)
	{
		// Count whenever new line is encountered
		if (ch == '\n')
		{
			numOfLines++;
		}
		// take next character from file
		ch = getc(fp);
	}
	fclose(fp); // close file
	return numOfLines + 1;
}

int get_m()
{
	int i;
	char *line;
	char position[3];
	for (i = 0; i < 18; i++)
	{
		sprintf(position, "%d", i);
		line = mem_get_value(position);
		printf("%s\n", line);
	}
	return 0;
}

int get_m_index()
{
	int i;
	char* memValue;

	for (i=0; i<21; i++)
	{
		seememory(i);
	}

	return 0;
}

int printFile(char *filename)
{
	FILE *fptr = fopen(filename, "r");
	char c = fgetc(fptr);
	while (c != EOF)
	{
		printf("%c", c);
		c = fgetc(fptr);
	}
	return 0;
}