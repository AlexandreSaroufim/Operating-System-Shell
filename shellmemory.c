#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "shell.h"
#include "shellmemory.h"


struct memory_struct{
	char *var;
	char *value;
};

struct memory_struct shellmemory[1000];

// Shell memory function

// shellmemory[900] -> [999] = variable 
// shellmemory[0] -> [998] = frame

void mem_init(){

	int i;
	for (i=0; i<1000; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

void memSetValue(char* str, int index){
	char position[3];
	sprintf(position, "%d", index);

	shellmemory[index].var=strdup(position);
	shellmemory[index].value=strdup(str);

	return;
}

// Set key value pair
void fs_set_value(char *var_in, char *value_in) {

	int i;

	for (i=0; i<FRAMESIZE; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=0; i<FRAMESIZE; i++){
		if (strcmp(shellmemory[i].var, "none") == 0){
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}

void vs_set_value(char *var_in, char *value_in) {

	int i;

	for (i=900; i<900+VARMEMSIZE; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=900; i<900+VARMEMSIZE; i++){
		if (strcmp(shellmemory[i].var, "none") == 0){
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}

void reset_VS()
{
	int i;

	for (i = 900; i < 999; i++) {
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}

}

void clear_mem() {
	int i;
	for (i = 0; i < 1000; i++) {
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

// returns 1 if a key exists in the shell memory, otherwise returns 0 indication key is not in shell memory
int mem_exists(char *var_in) {
	int i;

	for (i=0; i<1000; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){

			return 1;
		} 
	}
	return 0;
}

char* mem_get_value_at_index(int i) {

	char *line;
	char position[3];
	
	sprintf(position, "%d", i);
	line = mem_get_value(position);
	return line;

	// printf("%s", strdup(shellmemory[i].var));
	// printf("\n");
	// printf("%s", strdup(shellmemory[i].value));
	// printf("\n");
	// printf("----------\n");
}

int seememory(int i){
	printf("%s", strdup(shellmemory[i].var));
	printf("\n");
	printf("%s", strdup(shellmemory[i].value));
	printf("\n");
	printf("----------\n");
	return 0;

}

//get value based on input key
char *mem_get_value(char *var_in) {
	int i;

	for (i=0; i<1000; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			return strdup(shellmemory[i].value);
		} 
	}
	return "Variable does not exist";

}

int isPageAvailable(){
	for ( int i=0 ; i< FRAMESIZE-2 ; i++){
		if (shellmemory[i].var == "none" && shellmemory[i+1].var == "none" && shellmemory[i+2].var == "none"  ){
			return i;
		}
	}
	return -1;
}