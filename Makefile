CC = gcc




all: myshell



myshell: myshell.c stackCommands.c var_table.c
	$(CC) myshell.c -o myshell


clean:
	rm -f *.o myshell