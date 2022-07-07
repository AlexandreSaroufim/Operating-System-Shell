# ECSE-427: Assignment 3 (MEMORY MANAGEMENT)
 
## Author names and ID: (Mohammad Saeid Nafar, 260899098) (Alexandre Saroufim, 260886799)

Our code uses our Assignment 2 and 1 submissions as starter code for this assignment. 

##Assignment 1:

1.2.1. Enhance the set command: In interpreter.c file, we updated the interpreter function and the set function.

1.2.2. Add the echo command: In interpreter.c file, we created a new method echo and updated the interpreter function.

1.2.3. Enhance batch mode execution:In shell.c file, we extended the batch mode in the main function

1.2.4. Add the ls command: Added my_ls function in interpreter.c .

1.2.5. One-liners: Updated the parseInput function in order to allow one-liners.

##Assignment 2:

1.2.1. Implement the scheduling infrastructure:
We adjusted the run command such that instead of executing code line by line from a given input file,
it will instead read through the file and subsequenlty load each line from the input file to the shell
memory such that it could then execute each line of code using process control blocks (PCBs) in a single 
element ReadyQueue. After the SCRIPT terminates, the program removes the SCRIPT source code from the Shell memory.

1.2.2. Extend the OS Shell with the exec command:
We added the exec command to the OS Shell such that it can take up to 4 arguments (3 SCRIPTS and 1 POLICY) and 
run these scripts based on the given scheduling POLICY that was passed in.

1.2.3. Adding Scheduling Policies:
We implemented two more scheduling policies; namely, SJF (i.e. Shortest Job First), and RR (i.e. Round Robin).

1.2.4. SJF with job Aging: 
We first had an aging process that gave us the same answer as the output file given by the OS team. 
Then, we saw the discussion #405 on ED where Prof Oana states that the solution provided in this case "is closer to 
what actually happens in OS schedulers :)".
We wanted a challenge so we successfully implemented a new aging process that would have a closer output 
to a real OS scheduler.
The scheduler that would result in a solution similar to the OS team has the following logic: After the aging procedure,if there is a job 
in the queue that has a score lower to the current job it will be palced at the head of the queue and the old head 
will be placed at the end of the queue.

The scheduler that would result in a solution similar to a real OS scheduler ( and the one that we have as our solution ) 
has the following logic: After the aging procedure, we will sort the Queue from lower to higher score. 
In case of a tie, the process which is closer to the head will be placed at the top of the queue. So the old head is 
not always placed at the end of the queue ( which is the main differences between these 2 types of schedulers).

###Assignment 3:

1.2.1. Implementing The Paging Infrastructure:

We first adjusted the exec command such that it can now take multiple files that are the same and execute them.
Moreover, we set up a backing store directory that would enable us to perform the paging required for this assignment.
Upon opening the shell, if there is no backing store, the program creates one; and if there is already a backing store,
the program deletes all the contents in the backing store directory. Additionally, upon successfully quitting the 
OS, the program deletes the backing store directory. 

Furthermore, we split the shell memory into two parts; the frame store and the variable store which we would use in our
paging implementation later on. We also created a new command; "resetmem", which deletes the contents of the variable store. 

Additionalyy, we added a pageTable to each PCB in order to store the location of each page in the memory.

1.2.2. Extend the OS Shell with demand paging:

We updated the makefile in order to specify the frame size and variable memory size at compile time and 
included an extra printout in our shell that specifies the framse store size and variable store size.

We adjusted our code loading so that we just load the first 2 pages of each program.

If a program needs to execute a page that is not in memory we trigger a page fault that will 
stop the current process. If there is an available space in memory to add the new page, it will just be added and 
the next process will be executed, else we free the slot in memory depanding on the page replacement policy 
specified in 1.2.3 . 

1.2.3. Adding Page Replacement Policy:
We had to adjust the pages using the LRU policy.
So we created a new struct and a linkedlist of this struct in order to store the pages 
that are executed and when page fault is happening we just 
free the space that was allocated to the head of the linkedlist and make the according 
changes.

Thank you!
