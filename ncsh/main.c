#include "ncsh.h"

void update_date()
{
	time_t current=time((NULL));
	dispdate(*localtime(&current));
}

void menu()
{	
	wclear(stdscr);
	halfdelay(5);
	
	move(0,0);
	date_decoration();
	update_date();
	
	move(0,COLS-6);
	printw("[");
	attron(A_BOLD);
	printw("Q");
	attroff(A_BOLD);
	printw("uit]");
	move(1,0);
	
	printw("[MOTD]\n");
	motd("/etc/motd");
	
	printw("\n[");//R -> Run program
	attron(A_BOLD);
	printw("R");
	attroff(A_BOLD);
	printw("un program]");
	
	printw("\n[");//C -> Command shell
	attron(A_BOLD);
	printw("C");
	attroff(A_BOLD);
	printw("ommand shell]");
	
	printw("\n[Conway's ");//G -> Game of life
	attron(A_BOLD);
	printw("G");
	attroff(A_BOLD);
	printw("ame of Life]");
	
	if(system("mail -e 2>/dev/null")==0)
	printw("\n[Mail]You have mail. You can read the mail via \'mail\' in the command shell.");
	
	move(LINES-1,0);
	printw("Press ");
	attron(A_BOLD);
	printw("highlighted");
	attroff(A_BOLD);
	printw(" keys to perform task.");
	
	move(LINES-1,COLS-1);
}

int main()
{
	init();
	menu();
	char resp;
	while(1)
	{
		resp=getch();
		switch(resp)
		{
			case 'r':
			case 'R':
			run_program();
			menu();
			break;
			
			case 'q':
			case 'Q':
			clean();
			system("clear");
			return 0;
			break;
			
			case 'c':
			case 'C':
			clean();
			system("clear ; $SHELL ; clear");
			init();
			menu();
			break;
			
			case 'g':
			case 'G':
			conway();
			menu();
			break;
			
			case -102:
			menu();
			break;
			
			default:
			update_date();
			break;
		}
	}
}
