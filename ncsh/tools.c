#ifndef NCSH_TOOLS
#define NCSH_TOOLS

void init()
{
	initscr();
	keypad(stdscr,TRUE);
	raw();
	noecho();
	return;
}

void clean()
{
	endwin();
	return;
}

void date_decoration()
{
	printw("[");
	attron(A_DIM);
	printw("    -  -           ");
	attroff(A_DIM);
	printw("]");
	return;
}

void dispdate(struct tm target)
{
	attron(A_DIM);
	move(0,1);
	printw("%.4d",target.tm_year+1900);
	move(0,6);
	printw("%.2d",target.tm_mon+1);
	move(0,9);
	printw("%.2d",target.tm_mday);
	move(0,12);
	printw("%.2d",target.tm_hour);
	move(0,15);
	printw("%.2d",target.tm_min);
	move(0,18);
	printw("%.2d",target.tm_sec);
	
	//unfortunately the blink character cannot blink exactly when the second changed.
	//we have to blink the indicator manually
	if(target.tm_sec%2)
	{
		move(0,14);
		printw(":");
		move(0,17);
		printw(".");
	}
	else
	{
		move(0,14);
		printw(" ");
		move(0,17);
		printw(" ");
	}
	
	attroff(A_DIM);
	move(LINES-1,COLS-1);
	refresh();
	return;
}

/*void run_program()
{
	int retvalue;
	char cmdline[128];
	wclear(stdscr);
	printw("Enter program and press enter: ");
	echo();
	attron(A_DIM);
	scanw("%s",cmdline);
	attroff(A_DIM);
	clean();
	retvalue=system(cmdline);
	
	init();
	printw("Program exited with value %d.\nPress any key to continue...",retvalue);
	refresh();
	getch();
}*/
//avoid the unsafe implementation.

void run_program()
{
	system("clear");
	int retvalue;
	char *cmdline;
	wclear(stdscr);
	printw("Enter program and press enter: ");
	echo();
	attron(A_DIM);
	if(scanw("%ms",&cmdline)==1)//this used the dynamic allocation conversion specifier. see EXAMPLE section in scanf(3).
	{
		attroff(A_DIM);
		clean();
		retvalue=system(cmdline);
		
		free(cmdline);
		init();
		printw("Program exited with value %d.\nPress any key to continue...",retvalue);
		refresh();
		getch();
	}
	else
	{
		attroff(A_DIM);
		printw("Invalid responce.\nPress any key to continue...");
		refresh();
		getch();
	}
	system("clear");
}

void motd(char path[])
{
	//this should be simplified. Consider this a shameful moment in my career in university.
	FILE *target;
	int lfcount;//This is required to record duplicate newlines.
	char buffer;
	target=fopen(path,"r");
	if(target==NULL)
	{
		printw("[WARN]Unable to open specified MOTD file.");
		refresh();
		return;
	}
	while((buffer=getc(target))!=EOF)
	{
		lfcount=0;
		if(buffer!='\n')
		{
			addch(buffer);
		}
		else
		{
			while(1)
			{
				buffer=getc(target);
				if(buffer==EOF) break;//newline duplicates at the end of file, output none of them
				else if(buffer!='\n')
				{
					while(lfcount--) addch('\n');
					addch('\n');
					addch(buffer);
					break;
				}
				else lfcount++;
			}
		}
	}
	refresh();
	return;
}
#endif
