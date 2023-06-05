/*
By Oisín Duggan, s21328513 - Operating Systems Project 1
duggano5/2023-ca216-myshell

I acknowledge that this is my own work, except where I have clearly indicated that it is someone else's work.
I have read and agree to https://www.dcu.ie/policies/academic-integrity-plagiarism-policy 
*/

/*
Filename: osh.c
Description: the header file that contains shared definitions and includes required for all the programs
Author: duggano5
Date: 14:45 on Thursday, the 16th of March, 2023.
Last modified: 23:22 on Sunday, the 19th of March, 2023.
*/
//include necessary libraries
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <termios.h>

// definitions
#define MAX_BUFFER 1024 // max line buffer
#define MAX_ARGS 64 // max number of args
#define MAX_LINES 64 // max number of lines in a batch file
#define SEPARATORS " \t\n" // so we know when to split
#define PROMPT "\033[1;36m :3 \033[0m" // the prompt
#define ERROR_MSG "\033[1;35muwehhhh an error >_<\033[0m" // the error message in red
#define EXIT_MSG "\033[1;32mbye bye :3\033[0m\n" // the exit message in green
#define PAUSE_MSG "\033[1;35mCat got your tongue? Press enter for me~ :3 \033[0m" // the pause message in yellow
#define EXIT_CODE 2001 // so we know when to exit, numbers in errno past 2001 are safe to sue without conflicts
