all: shell

shell: main.c execute.c pcs_manager.c input_parser.c shell.h
	gcc main.c execute.c pcs_manager.c input_parser.c -g -Wall -o shell

clean:
	rm -f shell
