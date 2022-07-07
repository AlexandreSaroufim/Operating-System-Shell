#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>

#include "interpreter.h"
#include "shellmemory.h"
#include "cpu.h"
#include "shell.h"


int MAX_USER_INPUT = 1000;

int main(int argc, char *argv[]) {

	printf("%s\n", "Shell version 1.3 Created April 2022");
	printf("\n");

	help();
	
	// METHOD TO CREATE BACKING STORE
	createBackingStore();
	printf("\n");

	printf("Frame Store Size = %d; Variable Store Size = %d\n", FRAMESIZE, VARMEMSIZE);
	printf("\n");

	char prompt = '$';  				// Shell prompt
	char userInput[MAX_USER_INPUT];		// user's input stored here
	int errorCode = 0;					// zero means no error, default


	//init user input
	for (int i=0; i<MAX_USER_INPUT; i++)
		userInput[i] = '\0';
	
	//init shell memory
	mem_init();

	while(1) {						
		printf("%c ",prompt);
		fgets(userInput, MAX_USER_INPUT-1, stdin);

		if (feof(stdin)){
			freopen("/dev/tty", "r", stdin);
		}	
		
		errorCode = parseInput(userInput);
		if (errorCode == -1) exit(99);	// ignore all other errors
		memset(userInput, 0, sizeof(userInput));
	}

	return 0;

}

int createBackingStore() {
	int check ;
	char* dir= "backingStore";

	struct stat stats;
	stat(dir, &stats);

	// IF backingStore exists --> we delete content in backingStore
	
	if (S_ISDIR(stats.st_mode)) {
		deleteBsContent();
		printf("Backing Store content deleted\n");
	}
	else if (!(mkdir(dir, S_IRWXU))) {
		printf("Backing store created successfully\n");
	}
	else {
		printf("Unable to create backing store\n");
	}
	return 0;
}

int deleteBsContent() {
	system("rm -r ./backingStore/*");
}

int parseInput(char ui[]) {
	char tmp[200];
	char *words[100];							
	int a = 0;
	int b;							
	int w=0; // wordID	
	int errorCode;
	for(a=0; ui[a]==' ' && a<1000; a++);		// skip white spaces

	while(ui[a] != '\n' && ui[a] != '\0' && a<1000) {
		for(b=0; ui[a]!=';' && ui[a]!='\0' && ui[a]!='\n' && ui[a]!=' ' && a<1000; a++, b++)
			tmp[b] = ui[a];						// extract a word
		tmp[b] = '\0';

		words[w] = strdup(tmp);

		if(ui[a]==';'){
			w++;

			errorCode = interpreter(words, w);
			if(errorCode == -1){
				return errorCode;
			}

			a++;
			w = 0;
			for(; ui[a]==' ' && a<1000; a++);		// skip white spaces
			continue;
		}
		a++; 
		w++;
	}
	errorCode = interpreter(words, w);

	return errorCode;
}