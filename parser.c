/*
By Oisín Duggan, s21328513 - Operating Systems Project 1
duggano5/2023-ca216-myshell

I acknowledge that this is my own work, except where I have clearly indicated that it is someone else's work.
I have read and agree to https://www.dcu.ie/policies/academic-integrity-plagiarism-policy 
*/

/*
Filename: parser.c
Description: the program to deal with the command line input. Takes a line and decides what do do with it
Author: duggano5
Date: 15:28 on Thursday, the 16th of March, 2023.
Last modified: 23:37 on Sunday, the 19th of March, 2023.
*/

//include necessary libraries
#include "common.h"
#include "osh.h"

// name: parser
// description: takes a line and decides what to do with it
// parameters: char *buffer, char **args, char **arg, char *error_msg
// returns: int error code so we know if something goes wrong
int parser(char *buffer)
{
    // turn the buffer into usable arguments#
    char *args[MAX_ARGS]; // where we store the arguments
    char **arg = args; // used to iterate through the arguments
    FILE *redirect_out = NULL, *redirect_in = NULL; // where we store the file to redirect to and from
    int redirect_out_append = 0; // whether we are appending to the file or not
    int background; // whether we are running in the background or not

    // get all the arguments into the array
    *arg++ = strtok(buffer, SEPARATORS);
    while ((*arg++ = strtok(NULL, SEPARATORS)));

    // redirects and backgrounds
    redirect_out = find_redirect_out(args); // get the file to redirect to, if any
    redirect_in = find_redirect_in(args); // get the file to get input from, if any
    background = find_background(args); // get whether we are running in the background or not

    // if we have got a file to redirect to, we need to remove the arguments from the array and set stdout to the file
    if (redirect_out)
    {
        args[arg - args - 3] = NULL; // remove the last two args from args

        // set stdout to the already opened files
        if (redirect_out_append) freopen(NULL, "a", stdout);
        else freopen(NULL, "w", stdout);
    }


    // process the arguments if we get anything
    if (args[0])
    {
        // check if the arg is a builtin
        // cd
        if (strcmp(args[0], "cd") == 0)
        {
            if (!args[1]) // we have got no other args, so print the current dir
            {   
                fputs(getcwd(NULL, 0), stdout);
                fputs("\n", stdout);
            }

            else if (args[2]) // error if too many arguments
            {
                errno = E2BIG; // the standard "too many arguments" error code
            }

            else // go to the right dir
            {
                chdir(args[1]); // if no dir exists, the error handling will be handled
                setenv("PWD", getcwd(NULL, 0), 1); // set the new pwd in the environment variables
            }
        }

        // clear through communicating with the ansi terminal
        else if (strcmp(args[0], "clr") == 0)
        {
            fputs("\033[2J", stdout); // send the clear command
            fputs("\033[0;0H", stdout); // reposition the cursor at the top
        }

        // dir
        else if (strcmp(args[0], "dir") == 0)
        {
            if (args[2]) return E2BIG; // return if too many args

            // fork and then run ls (error handling is done already since fork will set errno)
            pid_t pid = fork();

            if (pid == 0) // child
            {
                if (args[1]) // print the dir we specified if it exists
                {
                    // return ENOTDIR if it won't work
                    struct stat st;
                    if (stat(args[1], &st) == -1) return ENOTDIR;
                    execlp("ls", "ls", "-al", args[1], NULL); // ls the specified dir
                }

                else // print the current one
                {
                    execlp("ls", "ls", "-al", NULL);
                }
            }

            else // parent
            {
                int status; // for storing the status of the child
                if (background == 0) waitpid(pid, &status, 0); // wait for the child to finish if we are running 
                if(status != 0) return ENOTDIR;
            }

        }

        // environ
        else if (strcmp(args[0], "environ") == 0)
        {
            if (args[1]) return E2BIG; // return if too many args 

            // fork and then run env (error handling is done already since fork will set errno)
            pid_t pid = fork();

            if (pid == 0) execlp("env", "env", NULL); // the child, list the environment variables
            else
            {
                if (background == 0) waitpid(pid, NULL, 0); // the parent, wait for the child to finish
            }
        }

        // echo
        else if (strcmp(args[0], "echo") == 0)
        {
            // print the arguments
            for (int i = 1; args[i]; i++)
            {
                fputs(args[i], stdout);
                fputs(" ", stdout);
            }
            fputs("\n", stdout);
        }

        // help
        else if (strcmp(args[0], "help") == 0)
        {
            // more the readme file
            pid_t pid = fork();

            if (pid == 0) execlp("more", "more", "./manual/readme", NULL); // child
            else
            {
                if (background == 0) waitpid(pid, NULL, 0); // child
            }
        }

        // pause
        else if (strcmp(args[0], "pause") == 0)
        {
            pid_t pid = fork();

            if (pid == 0)
            {
                fputs(PAUSE_MSG, stdout); // print the pause message
                while (getchar() != '\n'); // wait for the user to press enter
            }
            else
            {
                if (background == 0) waitpid(pid, NULL, 0); // wait for the child to finish
            }

        }

        // quit
        else if (strcmp(args[0], "quit") == 0)
        {
            fputs(EXIT_MSG, stdout); // print the quit message

            // build an array of all the pointers to free
            char **ptrs = malloc(sizeof(char *) * (MAX_ARGS + 1));	
            ptrs[0] = buffer;
            for (int i = 1; args[i]; i++)
            {
                ptrs[i] = args[i];
            }

            // build an array of all the files to close
            FILE **files = malloc(sizeof(FILE *) * 2);
            files[0] = redirect_out;
            files[1] = redirect_in;

            close_shell(ptrs, files); // close the shell
        }

        // else send the command to the system
        else
        {
            pid_t pid = fork();

            if (pid == 0) // child
            {
                execvp(args[0], args); // execute the command
            }
            else
            {
                waitpid(pid, NULL, 0); // wait for the child to finish
            }
        }
    }


    return errno;
}

// name: close_shell
// description: closes the program, kills all child processes and frees up memory
// parameters: an array of all pointers to free and an array of all files to close
// returns: the code to know we want to exit the program
int close_shell(char **ptrs, FILE **files)
{
    // kill all the child processes
    pid_t pid = fork();
    if (pid == 0) execlp("killall", "killall", "osh", NULL); // child
    else waitpid(pid, NULL, 0); // parent

    // free all the pointers
    for (int i = 0; ptrs[i]; i++)
    {
        free(ptrs[i]);
    }

    // close all the files
    for (int i = 0; files[i]; i++)
    {
        fclose(files[i]);
    }

    // exit the program
    return EXIT_CODE;
}

// name: find_redirect_out
// description: finds the file to redirect to, if any
// parameters: the arguments
// returns: the file to redirect to, if any, either in overwite mode or append mode
FILE *find_redirect_out(char **args)
{
    // find the file to redirect to
    for (int i = 0; args[i]; i++)
    {
        if(strcmp(args[i], ">") == 0) // overwrite mode
        {
            if (args[i + 1]) // if there is a file to redirect to
            {
                return fopen(args[i + 1], "w"); // open the file in overwrite mode
            }
            else // if there is no file to redirect to
            {
                errno = ENOENT; // the standard "no such file or directory" error code
                return NULL;
            }
        }

        else if (strcmp(args[i], ">>") == 0) // append mode
        {
            if (args[i + 1]) // if there is a file to redirect to
            {
                return fopen(args[i + 1], "a"); // open the file in append mode
            }
            else // if there is no file to redirect to
            {
                errno = ENOENT; // the standard "no such file or directory" error code
                return NULL;
            }
        }
    }

    return NULL;
}

// name: find_redirect_in
// description: finds the file to get input from, if any
// parameters: the arguments
// returns: the file to get input from, if any
FILE *find_redirect_in(char **args)
{
    // find the file to get input from
    for (int i = 0; args[i]; i++)
    {
        if (strcmp(args[i], "<") == 0) // if there is a file to get input from
        {
            if (args[i + 1]) // if there is a file to get input from
            {
                return fopen(args[i + 1], "r"); // open the file in read mode
            }
            else // if there is no file to get input from
            {
                errno = ENOENT; // the standard "no such file or directory" error code
                return NULL;
            }
        }
    }

    return NULL;
}

// name: find_ampersand
// description: finds the ampersand, if any
// parameters: the arguments
// returns: 1 if there is an ampersand, NULL if there is not
int find_background(char **args)
{
    // find the ampersand
    for (int i = 0; args[i]; i++)
    {
        if (strcmp(args[i], "&") == 0) return 1; // if there is an ampersand
    }

    return 0; // if there is not an ampersand
}