#include <ncurses.h>
#ifndef CONWAY
#define CONWAY
struct cells
{
	char map[32][42];
};

//show decoration
void decoration()
{
	for(int i=0;i<82;i++) printw("-");
	move(0,63);
	printw("[");
	attron(A_BOLD);
	printw("N");
	attroff(A_BOLD);
	printw("ext frame][");
	attron(A_BOLD);
	printw("Q");
	attroff(A_BOLD);
	printw("uit]");
	move(31,0);
	printw("-[");
	attron(A_BOLD);
	printw("Z");
	attroff(A_BOLD);
	printw("/");
	attron(A_BOLD);
	printw("X");
	attroff(A_BOLD);
	printw(" cell status][");
	attron(A_BOLD);
	printw("P");
	attroff(A_BOLD);
	printw("ause/Resume]");
	for(int i=32;i<82;i++) printw("-");
	for(int i=1;i<31;i++)
	{
		move(i,0);printw("|");
		move(i,81);printw("|");
	}
}

struct cells simulate(struct cells);

void clr_border(struct cells*);

void print_cells(struct cells);

//moves cursor to the specified direction. this will not check edge
void movept(int*,int*,char);

void status_pause();

void status_running();
#include "conway.c"
#endif
