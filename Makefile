mysh: shell.c interpreter.c shellmemory.c cpu.c pcb.c framequeue.c
	gcc -D FRAMESIZE=$(framesize) -D VARMEMSIZE=$(varmemsize) -c shell.c interpreter.c shellmemory.c cpu.c pcb.c framequeue.c
	gcc -o mysh shell.o interpreter.o shellmemory.o cpu.o pcb.o framequeue.o

clean: 
	rm mysh; rm *.o