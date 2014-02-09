shell: shell.o shell_functions.o
	gcc shell.o shell_functions.o -o shell

shell.o: shell.c shell_functions.h
	gcc -c shell.c

shell_functions.o: shell_functions.c shell_functions.h
	gcc -c shell_functions.c

clean:
	rm -rf *o
