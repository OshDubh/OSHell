/*
Filename: osh.c
Description: the header file for osh.c and associated programs that defines all the functions
Author: duggano5
Date: 14:45 on Thursday, the 16th of March, 2023.
Last modified: 23:21 on Sunday, the 19th of March, 2023.
*/

#include "common.h"

int osh_loop(FILE *batchfile); // the loop that passes input and does error control
int parser(char *buffer); // deals with the lines 
int close_shell(char **ptrs, FILE **files); // frees the memory and closes the shell
FILE *find_redirect_in(char **args); // finds the redirect in
FILE *find_redirect_out(char **args); // finds the redirect out
int find_background(char **args); // finds the ampersand
