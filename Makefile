#Code to compile every part - not asked in assignment
all: part1 part2 part3

#Makefile code to compile part 1, handles piping
part1: smsh2.c splitline.c execute.c new_commands.c
	gcc -Wall smsh2.c splitline.c execute.c new_commands.c -o smsh2

#Makefile code to compile part2, handles redirection
part2: smsh3.c splitline.c execute.c new_commands.c
	gcc -Wall smsh3.c splitline.c execute.c new_commands.c -o smsh3

#Makefile code to compile part3, handles globbing
part3: smsh4.c splitline.c execute.c new_commands.c
	gcc -Wall smsh4.c splitline.c execute.c new_commands.c -o smsh4