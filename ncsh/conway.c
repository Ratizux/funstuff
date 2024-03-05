#include "conway.h"

struct cells buffer;

struct cells simulate(struct cells current)
{
	int alive_count;
	struct cells next;
	clr_border(&next);
	for(int i=1;i<=30;i++)
	{
		for(int j=1;j<=40;j++)
		{
			alive_count=0;
			alive_count+=current.map[i-1][j-1];
			alive_count+=current.map[i-1][j];
			alive_count+=current.map[i-1][j+1];
			alive_count+=current.map[i][j-1];
			alive_count+=current.map[i][j+1];
			alive_count+=current.map[i+1][j-1];
			alive_count+=current.map[i+1][j];
			alive_count+=current.map[i+1][j+1];
			if(current.map[i][j]==1)
			{
				if(alive_count<2 || alive_count>3) next.map[i][j]=0;
				else next.map[i][j]=1;
			}
			else
			{
				if(alive_count==3) next.map[i][j]=1;
				else next.map[i][j]=0;
			}
		}
	}
	return next;
}

void clr_border(struct cells *target)
{
	for(int i=0;i<42;i++)
	{
		target->map[0][i]=0;
		target->map[31][i]=0;
	}
	for(int i=1;i<31;i++)
	{
		target->map[i][0]=0;
		target->map[i][41]=0;
	}
	return;
}

void print_cells(struct cells target)
{
	move(1,1);
	for(int i=1;i<=30;i++)
	{
		for(int j=1;j<=40;j++)
		{
			if(target.map[i][j]) attron(A_REVERSE);
			else attroff(A_REVERSE);
			printw("  ");
		}
		move(i+1,1);
	}
	refresh();
	return;
}

void nextframe()
{
	buffer=simulate(buffer);
	print_cells(buffer);
}

void movept(int *pty,int *ptx,char resp)
{	
	move(*pty,(*ptx-1)*2+1);
	if(buffer.map[*pty][*ptx]) attron(A_REVERSE);
	printw("  ");
	attroff(A_REVERSE);
	
	switch(resp)
	{
		case 2:(*pty)++;break;
		case 3:(*pty)--;break;
		case 4:(*ptx)--;break;
		case 5:(*ptx)++;break;
	}
	
	move(*pty,(*ptx-1)*2+1);
	if(buffer.map[*pty][*ptx]) attron(A_REVERSE);
	printw("##");
	move(LINES-1,COLS-1);
	attroff(A_REVERSE);
	return;
}

void status_pause()
{
	move(0,1);
	printw("[");
	attron(A_DIM|A_BLINK);
	printw("Pause");
	attroff(A_DIM|A_BLINK);
	printw("]--");
	move(LINES-1,COLS-1);
	return;
}

void status_running()
{
	move(0,1);
	printw("[Running]");
	move(LINES-1,COLS-1);
	return;
}

int conway()
{
	//this should have some annotation but I'm tooooooooooo lazy :-)
	wclear(stdscr);
	int unavailable=0;
	if(LINES<32 || COLS<82)
	{
		unavailable=1;
		move(0,0);
		printw("This requires a 82x32 (or larger) terminal.");
	}

	int paused=1;
	int pty=1,ptx=1;
	char resp;
	
	for(int i=0;i<32;i++)
	{
		for(int j=0;j<42;j++)
		{
			buffer.map[i][j]=0;
		}
	}
	
	if(!unavailable)
	{
		decoration();
		print_cells(buffer);
		move(1,1);
		printw("##");
		status_pause();
	}
	
	while(1)
	{
		resp=getch();
		if(paused && !unavailable)
		{
			switch(resp)
			{
				case 2:
				if(pty<30)
				{
					movept(&pty,&ptx,resp);
				}
				break;
				case 3:
				if(pty>1)
				{
					movept(&pty,&ptx,resp);
				}
				break;
				case 4:
				if(ptx>1)
				{
					movept(&pty,&ptx,resp);
				}
				break;
				case 5:
				if(ptx<40)
				{
					movept(&pty,&ptx,resp);
				}
				break;
				case 'z':
				case 'Z':
				move(pty,(ptx-1)*2+1);
				attron(A_REVERSE);
				printw("##");
				attroff(A_REVERSE);
				move(LINES-1,COLS-1);
				buffer.map[pty][ptx]=1;
				break;
				case 'x':
				case 'X':
				move(pty,(ptx-1)*2+1);
				printw("##");
				move(LINES-1,COLS-1);
				buffer.map[pty][ptx]=0;
				break;
			}
		}
		
		switch(resp)
		{
			case 'q':
			case 'Q':
			clean();
			return 0;
			break;
			case 'n':
			case 'N':
			nextframe();
			if(paused)
			{
				move(pty,(ptx-1)*2+1);
				if(buffer.map[pty][ptx]) attron(A_REVERSE);
				printw("##");
				attroff(A_REVERSE);
			}
			move(LINES-1,COLS-1);
			break;
			case -102:
			if(LINES<32 || COLS<82)
			{
				unavailable=1;
				paused=1;
				wclear(stdscr);
				move(0,0);
				printw("This requires a 82x32 (or larger) terminal.");
			}
			else if(unavailable)
			{
				wclear(stdscr);
				move(0,0);
				unavailable=0;
				decoration();
				print_cells(buffer);
				if(paused)
				{
				move(pty,(ptx-1)*2+1);
				if(buffer.map[pty][ptx]) attron(A_REVERSE);
				printw("##");
				attroff(A_REVERSE);
				status_pause();
				}
				else
				{
					status_running();
				}
			}
			move(LINES-1,COLS-1);
			break;
			case 'p':
			case 'P':
			if(paused)
			{
				paused=0;
				move(pty,(ptx-1)*2+1);
				if(buffer.map[pty][ptx]) attron(A_REVERSE);
				printw("  ");
				attroff(A_REVERSE);
				move(LINES-1,COLS-1);
				status_running();
			}
			else
			{
				paused=1;
				move(pty,(ptx-1)*2+1);
				if(buffer.map[pty][ptx]) attron(A_REVERSE);
				printw("##");
				attroff(A_REVERSE);
				move(LINES-1,COLS-1);
				status_pause();
			}
			break;
			case ERR:
			if(!paused && !unavailable)
			{
				nextframe();
				move(LINES-1,COLS-1);
			}
		}
	}
	return 0;
}
