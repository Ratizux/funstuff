#ifndef NCSH
#define NCSH
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

void init();

void clean();

//Print decoration only, not real date
void date_decoration();

//Print specified date in the decoration, feels like [2016-08-10 12:10:34].
//This will NOT validate data, and will assume the decoration is on the first line.
void dispdate(struct tm);

//Run program specified by user
void run_program();

//Print message of the day in the following form:
//[MOTD]
//Messages...
//Messages...
//This should be able to clear duplicated newlines at the end of file
void motd(char []);
#include "tools.c"
#include "conway.h"
#endif
