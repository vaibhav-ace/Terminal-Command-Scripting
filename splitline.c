/* splitline.c - commmand reading and parsing functions for smsh
 *
 *    char *next_cmd(char *prompt, FILE *fp) - get next command
 *    char **splitline(char *str);           - parse a string

 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"smsh.h"



//New function -> this function is called by a 'pipe_execution' from new_commands.c file
//Based on the next_cmd function that was previously provided, this function replaces the second argument parameter of a file pointer with a character
char** word_break (char* input_str,char symbol){


	//While the variables names are different in this function, the inspiration is basically taken from the 'next_cmd' function below to basically find the '|' character in the input
	//This function is used to basically help the input separate into smaller arguments
	//For example it will help "ls | wc" to become "ls" and "wc" which are passed back to the pipe_execution function as pointer of pointer (**)

	//Declaring variables
	char* newstr();	//this is a function in this file (provided below), this function's description is provided beforehand
	char** arguments;
	int	spots=0;			/* spots in table	*/
	int	bufspace=0;			/* bytes in table	*/
	int	argnum=0;			/* slots used		*/
	char* line=input_str;	/* pos in string	*/
	char* start;
	int	len;

	if (input_str==NULL){return NULL;} //if input is NULL, we can return NULL as nothing is present in the string                       /* handle special case	*/

	//Initializing array
	arguments= emalloc(BUFSIZ);
	bufspace = BUFSIZ;
	spots    = BUFSIZ/sizeof(char *);

	while(*line!='\0'){
		while (*line==symbol){line++;}     /* skip leading spaces	*/
		if (*line=='\0'){break;}  /* quit at end-o-string	*/

		/* make sure the array has room (+1 for NULL) */
		if ( argnum+1 >= spots ){
			arguments = erealloc(arguments,bufspace+BUFSIZ);
			bufspace += BUFSIZ;
			spots += (BUFSIZ/sizeof(char *));
		}

		/* mark start, then find end of word */
		start=line;
		len=1;
		while (*++line!='\0' && (*line!=symbol)){len++;}
		arguments[argnum++]=newstr(start, len);
	}

	//setting last argument in array as NULL (essential for execvp functions to execute properly)
	arguments[argnum]=NULL;


	//returning arguments
	return arguments;
}



//Previously provided functions

char * next_cmd(char *prompt, FILE *fp)
/*
 * purpose: read next command line from fp
 * returns: dynamically allocated string holding command line
 *  errors: NULL at EOF (not really an error)
 *          calls fatal from emalloc()
 *   notes: allocates space in BUFSIZ chunks.
 */
{
	char	*buf ; 				/* the buffer		*/
	int	bufspace = 0;			/* total size		*/
	int	pos = 0;			/* current position	*/
	int	c;				/* input char		*/

	printf("%s", prompt);				/* prompt user	*/
	while( ( c = getc(fp)) != EOF ) {

		/* need space? */
		if( pos+1 >= bufspace ){		/* 1 for \0	*/
			if ( bufspace == 0 )		/* y: 1st time	*/
				buf = emalloc(BUFSIZ);
			else				/* or expand	*/
				buf = erealloc(buf,bufspace+BUFSIZ);
			bufspace += BUFSIZ;		/* update size	*/
		}

		/* end of command? */
		if ( c == '\n' )
			break;

		/* no, add to buffer */
		buf[pos++] = c;
	}
	if ( c == EOF && pos == 0 )		/* EOF and no input	*/
		return NULL;			/* say so		*/
	buf[pos] = '\0';
	return buf;
}


/**
 **	splitline ( parse a line into an array of strings )
 **/
#define	is_delim(x) ((x)==' '||(x)=='\t')

char ** splitline(char *line)
/*
 * purpose: split a line into array of white-space separated tokens
 * returns: a NULL-terminated array of pointers to copies of the tokens
 *          or NULL if line if no tokens on the line
 *  action: traverse the array, locate strings, make copies
 *    note: strtok() could work, but we may want to add quotes later
 */
{
	char	*newstr();
	char	**args ;
	int	spots = 0;			/* spots in table	*/
	int	bufspace = 0;			/* bytes in table	*/
	int	argnum = 0;			/* slots used		*/
	char	*cp = line;			/* pos in string	*/
	char	*start;
	int	len;

	if ( line == NULL )			/* handle special case	*/
		return NULL;

	args     = emalloc(BUFSIZ);		/* initialize array	*/
	bufspace = BUFSIZ;
	spots    = BUFSIZ/sizeof(char *);

	while( *cp != '\0' )
	{
		while ( is_delim(*cp) )		/* skip leading spaces	*/
			cp++;
		if ( *cp == '\0' )		/* quit at end-o-string	*/
			break;

		/* make sure the array has room (+1 for NULL) */
		if ( argnum+1 >= spots ){
			args = erealloc(args,bufspace+BUFSIZ);
			bufspace += BUFSIZ;
			spots += (BUFSIZ/sizeof(char *));
		}

		/* mark start, then find end of word */
		start = cp;
		len   = 1;
		while (*++cp != '\0' && !(is_delim(*cp)) )
			len++;
		args[argnum++] = newstr(start, len);
	}
	args[argnum] = NULL;
	return args;
}


/*
 * purpose: constructor for strings
 * returns: a string, never NULL
 */
char *newstr(char *s, int l)
{
	char *rv = emalloc(l+1);

	rv[l] = '\0';
	strncpy(rv, s, l);
	return rv;
}

void
freelist(char **list)
/*
 * purpose: free the list returned by splitline
 * returns: nothing
 *  action: free all strings in list and then free the list
 */
{
	char	**cp = list;
	while( *cp )
		free(*cp++);
	free(list);
}

void * emalloc(size_t n)
{
	void *rv ;
	if ( (rv = malloc(n)) == NULL )
		fatal("out of memory","",1);
	return rv;
}
void * erealloc(void *p, size_t n)
{
	void *rv;
	if ( (rv = realloc(p,n)) == NULL )
		fatal("realloc() failed","",1);
	return rv;
}
