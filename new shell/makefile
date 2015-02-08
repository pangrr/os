all: shell

shell: main.c launch.c input_parser.c job_util.c process_util.c shell.h
	gcc main.c launch.c input_parser.c job_util.c process_util.c -g -Wall -o shell

clean:
	rm -f shell
