#define	YES	1
#define	NO	0

char	*next_cmd();
char	**splitline(char *);
void	freelist(char **);
void	*emalloc(size_t);
void	*erealloc(void *, size_t);
int	execute(char **);
void	fatal(char *, char *, int );

int	process();


//New command made for executing code from various files to functions in other files
void pipe_execution(char*x);
char** word_break(char *x, char z);
char* newstr(char* x, int z);
char* split_elements(char* x, char z);
void redirection_problem(char x, char* y, char* z);
void final_glob_execution(char** x);