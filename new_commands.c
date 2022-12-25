#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "smsh.h"
#include <stdlib.h>
#include <fcntl.h>
#include <glob.h>
#include <sys/wait.h>
// All necessary libraries included
//This file contains most of the functions for converting Bash input commands to C commands with similar output results using processes and forking



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Function to handle redirection requests
void redirection_problem (char symbol, char* word, char* element) {

	//Declaring variables
	int pipe; //used for redirection and duc() function
	int descriptor; //basically used like a file descriptor and reader in broad way
	char** elements; //this variable is used to extract individual symbols in an array form from the word using splitline command in the function
	char* temp; //used to store a pointer to element
	if (element!=NULL){

		//Process to store a value to temp later used in splitline
		char** temp2;
		if (symbol!='>' && symbol!='<'){temp=NULL;}
		else{
			//For greater than symbol
			if (symbol=='>'){
				temp2=word_break(word,'>');
				temp=*temp2;
			}

			//For less than symbol
			if (symbol=='<'){
				temp2=word_break(word,'<');
				temp=*temp2;
			}
		}

		//Function splitline() in file splitline.c
		elements=splitline(temp);

		if (symbol=='<'){//For less than symbol
			pipe=dup(0);//make pipe variable into stdin
			close(0); //close read
			descriptor=open(element,O_RDONLY); //opening and only reading into element

			if (descriptor<0){printf("Descriptor Error");}//handling errors

			if (elements!=NULL){
				//run execute command in execute.c file
				execute(elements);
			}

			//making pipe into stdin
			dup2(pipe,0);
			//Finally closing pipe
			close(pipe);
		}
		else if (symbol=='>'){//For greater than symbol
			//Make pipe variable into stdout so it can be written to
			pipe=dup(1);
			//close write
			close(1);
			descriptor=creat(element,S_IRUSR|S_IWUSR);

			if (descriptor<0){printf("Descriptor Error");}//handling errors

			if (elements!=NULL){
				//run execute command in execute.c file
				execute(elements);
			}

			//making pipe into stdout
			dup2(pipe,1);
			//Finally closing pipe
			close(pipe);
		}
	}

	//Returning nothing -> usually should return before reaching this line in function
	return;
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

char* split_elements(char *individual_word, char symbol){

	//Declaring variables
	char** element;
	char* temp;
	int size=0;
	char* output; //result variable

	//Base exception handler
	if (symbol!='<' && symbol!='>'){return NULL;}

	//Main execution of program
	if (symbol=='>'){//if greater than symbol is present
		element=word_break(individual_word,'>');
		if (element!=NULL){
			output=element[1];
			while (*output=='\t' || *output==' '){
					//we ignore the spaces before the first alphabet
					output=output+1; //output pointer set to non space value
			}
			size++;
			temp=output;

			while (!(*output==' ' || *output=='\t') && *++output!='\0'){size++;}
			output=newstr(temp, size);
			//Returning value
			return output;
		}
	}
	else if(symbol=='<'){//if less than symbol is present
		element = word_break(individual_word,'<');
		if (element!= NULL){
			output=element[1] ;
			while (*output=='\t' || *output==' '){
				//we ignore the spaces before the first alphabet
				output=output+1; //output pointer set to non space value
			}
			size++;
			temp=output;

			while (!(*output==' ' || *output=='\t') && *++output!='\0'){size++;}
			output=newstr(temp, size); //newstr() function is present in splitline.c
			//Returning value
			return output;
		}
	}

	//returning
	return NULL;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Function to check if a string contains the "<" or less than symbol. If yes, then input is handled by a different function that ususal
int contains_less_than(char* input){
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
int contains_greater_than(char* input){
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

//Function to check if a string contains the "*" or pointer symbol.
int contains_pointer2(char* input){
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
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//Function for executing commands with pipes
/*
This function uses pipes and the duc2() function to replace stdin and stdout with pipe input and output
The function follows a general rule where the pseudocode states that:
 - a for loop used is used to iterate over the various words in a line from the stdin
 - 2 pipes, basically a last and next are used to output the command from one command into the input of another command via pipe
 - the two pipes are closed after each iteration to ensure that no syntax errors are generated
 - use fork() to ensure that the execvp() fucntion are executed in the child process so that the parent does not kill the process, leaving the program hanging in between execution
 - close any pipes remaining and go to last iteration of for loop till the NULL keyword is reached, which states that the command input has ended

All referring function are either declared above in this same file or in other files - comments explain where each function is located!
*/
void pipe_execution(char* line){


    //Declaring variables
    //line is the input command as a string (character pointer in this case because of C)
    char** individual_words_list; // this is the pointer to the pointer to the list of indiviudal words from the line input without the pipe command -> '|' and in other cases without redirection commands -> '>','<',etc.
    char** word_arr; //this is the pointer to the pointer to the array of words without spaces so -> "hello world" would become "[{hello},{world}]"  ->  It is very useful for execvp() execution
    pid_t process_id; //process variable used to fork into child and parent


    //word_break function is in the splitline.c file - description of the function explained in the function implementation
	individual_words_list=word_break(line,'|');
	if (individual_words_list==NULL){return;}//exception handler


	//Pipe declaration for piping input and output to different processes - they store file descriptiors for two pipes
	int next[2]; int last[2];
	int i=-1; //variable for iteration in the while loop


    //Pipe failure handlers
	if (pipe(next)==-1){
		printf("next Pipe Error!");return;
	}
	if (pipe(last)==-1){
	    printf("last Pipe Error!");return;
	}

	//Start of a loop for execution of main code for piping
	while(individual_words_list[i]!=NULL){ // if NULL is reached we have successfully executed the command and piped everything that needs to be done
	    i++; //increasing i to traverse the arrays and otbain elements in the loop

		//Forking begins!
		process_id=fork();
		// Forking process error handler
		if (process_id<0){
			printf("Forking Error!");return;
		}


		//Parent Process
		if (process_id>0){
		    //If this is not the first command -> close previous pipe
			if(i!=0){
				close(last[0]);close(last[1]);
			}
            //If this is not the final command -> previous pipe = new pipe
			if(!(individual_words_list[i+1]==NULL)){
				last[0]=next[0];
				last[1]=next[1];
			}
			//Waiting for child to finish its processes
			wait(NULL);
		}


		//Child Process
		if(process_id==0){
		    //If this is not first command -> redirecting input to the previous pipe
			if(i!=0){
			    //closing previous input's write
				close(last[1]);
				//redirecting stdin to previous pipes read section
				dup2(last[0],0);
				//closing the read section of the previous pipe -> important to not leave any open pipes
				close(last[0]);
			}

            //If this is not the final command -> close previous pipe
			if(!(individual_words_list[i+1]==NULL)){
			    //closing previous pipe's read section
				close(next[0]);
				//redirecting file descriptior's of stdout to new pipe's write
				dup2(next[1], 1);
				//closind write of new pipe
				close(next[1]);
			}


			//Checking for redirection in piping execution function -> only the ">" symbol in this statement
			if (contains_greater_than(individual_words_list[i])){
				char* elements=split_elements(individual_words_list[i],'>'); //this function's output is used in redirection main function, function declared in this file
				redirection_problem('>',individual_words_list[i], elements); //redirection execution
			}

			else{
				//Checking for redirection in piping execution function -> only the "<" symbol in this statement
				if (contains_less_than (individual_words_list[i])){
					char* elements=split_elements(individual_words_list[i],'<'); //this function's output is used in redirection main function, function declared in this file
					redirection_problem('<',individual_words_list[i], elements); //redirection execution
				}


				//Executing the command
				else if((word_arr=splitline(individual_words_list[i]))!=NULL){ //parsing line into array of strings -> function splitline() is in splitline.c file
					//We can now finally execute our command using the execvp function
					execvp(word_arr[0],word_arr);

					//Freeing dynamically allocated memory
					freelist(word_arr);
				}
			}
		}// End of child process
	}
    //End of while loop


    //Closing any remaining pipes
	close(next[0]);close(next[1]);

	//Returning
    return;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void final_glob_execution(char** line){

	if (line[0]==NULL){return;} // exception handler

	//Declaring variables
	int counter=0; int i=0; int count=0; //counter and count are used as count variables and i is used for the for loop
	char** temp=line; //obtaining copy of line
	glob_t process;
	int k=0; //count variable as well

	for (i=0;temp[i]!=NULL;i++){

		//if * is the next word
		if(contains_pointer2(temp[i])){
			if (counter==0){
				//if first glob call is not yet done
				counter++;
				glob(temp[i],0,NULL,&process);
			}
			else{
				//if first glob call is already made then do the following
				glob(temp[i],GLOB_APPEND,NULL,&process);
			}
		}

		//if * is not the next word
		else{
			//no * symbol
			line[k]=temp[i];
			k=k+1;
		}
	}

	//Emptying the buffer
	for (count=0;count<process.gl_pathc;count++,k++){

		line[k]=process.gl_pathv[count];
	}
	//Executing command after setting final value to NULL
	line[k]=NULL; //setting final argument to be NULL to let the function that command has finished
	execute(line); // execute command present in execute.c file

	//Returning nothing
	return;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//End of file