/*
Filename: osh.c
Description: the main file that runs the shell
Author: duggano5
Date: 14:44 on Thursday, the 16th of March, 2023.
Last modified: 11:21 on Tuesday, the 11th of April, 2023.
*/

// include the common header and this programs header
#include "common.h"
#include "osh.h"

int main(int argc, char *argv[])
{
	// open the batch file it it exists
	FILE *batchfile;
	batchfile = argc > 1 ? fopen(argv[1], "r") : NULL;

	// pass the batchfile to the shell loop, or set it to NULL and start the loop if it wasn't provided
	argc > 1 ? osh_loop(batchfile) : osh_loop(NULL);

	// close the batch file if it exists
	if (batchfile) fclose(batchfile);

	return 0;
}

// path: osh.h
// name: osh_loop
// description: the main loop of the shell, and decides what to do depending on the input stream
// parameters: the batchfile, if it exists
// returns int, where 0 is success, and anything else is an error
int osh_loop(FILE *batchfile)
{
	// initialise variablese
    char buffer[MAX_BUFFER]; // where we store the line input
    char *args[MAX_ARGS]; // where we store the arguments
    char **arg; // used to iterate through the arguments
    char *error_msg = "\033[1;35muwehhhh an error >_<\033[0m"; // a pointer to the error message 

    // set an environment variable called shell to the full path of the executable
    setenv("shell", getcwd(NULL, 0), 1);

	// do the shell loop
	if (batchfile)
	{
		// TODO read the batch file line by line and execute it
		printf("batch file in woo\n");
	}

	// the regular shell loop 
	else
	{
		while(!feof(stdin))
		{
			char *pwd = getenv("PWD"); // get the pwd from the environment variables to include it in the prompt
			// print the pwd and then the prompt
			fputs(pwd, stdout);
			fputs(PROMPT, stdout);

			// read the line 
			if (fgets(buffer, MAX_BUFFER, stdin))
			{
				arg = args; // break the input down into usable arguments

				// get all the arguments into the array
				*arg++ = strtok(buffer, SEPARATORS);
				while ((*arg++ = strtok(NULL, SEPARATORS)));

				// pass the line to the parser
				parser(args, arg);
			}
		}
	}
}