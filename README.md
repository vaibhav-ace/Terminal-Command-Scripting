# Terminal-Command-Scripting

A program to input Bash commands from the terminal and execute them using C instructions.
The contents are C source files that implement a simple shell. The contents are:
- smsh.c - an implementation of a very simple shell program
- smsh.h - a header file with function prototypes
- execute.c - a set of helper functions for running processes within the program
- splitline.c - some text processing utilities

To compile these sources run:

gcc -o smsh1 smsh1.c splitline.c execute.c

To run the shell type:
./smsh1

A prompt of the form ">" will appear and then you can type commands like:
> ls

execute.c shell.tgz smsh.h smsh1 smsh1.c splitline.c

> wc execute.c

37 113 725 execute.c

The shell is terminated by typing the Control-D key (which signals end of input).

Piping can be observed by typing:
make part1

and it can then be can be run by typing:
./smsh2

Redirection can be observed by typing:
make part2

and it can then be run by typing:
./smsh3

Globbing can be observed by typing:
make part3

and it can then be run by typing:
./smsh4
