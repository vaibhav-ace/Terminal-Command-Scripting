/**  smsh1.c  small-shell version 1
 **		first really useful version after prompting shell
 **		this one parses the command line into strings
 **		uses fork, exec, wait, and ignores signals
 **/

#include	<stdio.h>
#include	<stdlib.h>
#include    <string.h>
#include	<unistd.h>
#include	<signal.h>
#include	"smsh.h"

#define	DFL_PROMPT	"> "





//------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Function to check if a string contains the "*" or pointer symbol.
int contains_pointer(char* input){
    int size=strlen(input);
    int res=0;
    int i=0;
    for (i=0;i<size;i++){
        if(input[i]=='*'){
            res=1;
            break;
        }
    }
    return res;
}

//Function to check if a string contains the "|" or pipe symbol. If yes, then it is handled by a different function that ususal
int contains_pipe(char* input){
    int size=strlen(input);
    int res=0;
    int i=0;
    for (i=0;i<size;i++){
        if(input[i]=='|'){
            res=1;
            break;
        }
    }
    return res;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Function to check if a string contains the "<" or less than symbol. If yes, then input is handled by a different function that ususal
int less_than(char* input){
    int size=strlen(input); //string length
    int res=0;
    int i=0;
    //for loop to find the symbol
    for (i=0;i<size;i++){
        if(input[i]=='<'){
            res=1;
            break;
        }
    }
    return res;
}

//Function to check if a string contains the ">" or less than symbol. If yes, then input is handled by a different function that ususal
int greater_than(char* input){
    int size=strlen(input); //string length
    int res=0;
    int i=0;
    //for loop to find the symbol
    for (i=0;i<size;i++){
        if(input[i]=='>'){
            res=1;
            break;
        }
    }
    return res;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//Main function to mainly see the input and which function will execute the input
int main(){
	char *cmdline, *prompt, **arglist;
	void setup();

	prompt=DFL_PROMPT ;
	setup();

	while ((cmdline=next_cmd(prompt, stdin)) != NULL ){

	    //Checking to see if the input has the pointer character in it
	    if (contains_pointer(cmdline)){

	        //execute globbing function
	        char** temp=splitline(cmdline);
	        if(temp!=NULL){
	            final_glob_execution(temp);
	        }
	        freelist(temp);
	    }

		//Checking to see if the input has the pipe character in it
		else if (contains_pipe(cmdline)){
			//Pipe character found so a new function called pipe_execution will deal with this input
            pipe_execution(cmdline);//This function is in the 'mew_commands.c' file
		}

		//If less than or greater than exists, we execute it usinf redirection fucntion in new_commands.c
		else if (greater_than(cmdline)){
		    char* element=split_elements(cmdline,'>'); //function in new_commands.c file
            redirection_problem('>',cmdline,element);
		}

		else if (less_than(cmdline)){
		    char* element=split_elements(cmdline,'<'); //function in new_commands.c file
            redirection_problem('<',cmdline,element);
		}

		//Pipe execution is not found so this time the input is handled by the previously provided execute code
		else{
    		if ( (arglist = splitline(cmdline)) != NULL  ){
    			execute(arglist);//this function is in the 'execute.c' file
    			freelist(arglist);
    		}
		}
		free(cmdline);//freeing memory that was allocated
	}
	return 0;
}

//Previously provided functions
void setup()
/*
 * purpose: initialize shell
 * returns: nothing. calls fatal() if trouble
 */
{
	signal(SIGINT,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void fatal(char *s1, char *s2, int n)
{
	fprintf(stderr,"Error: %s,%s\n", s1, s2);
	exit(n);
}

//End of file