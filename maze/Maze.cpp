#include "Maze.hpp"
#include <ncurses.h>
#include <algorithm>
#include <cmath>
#include <ctime>

Block* Block::maze;
int Block::maze_height,Block::maze_width,Block::src,Block::dest;
Display display;
std::deque<Block*> Block::solution;

Display::Display()
{
	initscr();
	keypad(stdscr,TRUE);
	raw();
	noecho();
	start_color();
	init_pair(Black_on_white,COLOR_WHITE,COLOR_BLACK);
	init_pair(White_on_black,COLOR_BLACK,COLOR_WHITE);
	init_pair(Cyan,COLOR_BLACK,COLOR_CYAN);
	init_pair(Red,COLOR_BLACK,COLOR_RED);
	init_pair(Green,COLOR_BLACK,COLOR_GREEN);
	init_pair(Yellow,COLOR_BLACK,COLOR_YELLOW);
	curs_set(0);//hide the cursor
	halfdelay(5);//500 ms timeout
}

Display::~Display()
{
	endwin();
}

void Display::print_color(const char* content,Color color)
{
	attron(COLOR_PAIR(color));
	printw(content);
	attroff(COLOR_PAIR(color));
}

void Display::print_block(Block *target,const char* content)
{
	Color color;
	switch(target->type)
	{
		case Block::Wall:color=Black_on_white;break;
		case Block::Path:color=White_on_black;break;
		case Block::Start:case Block::Destination:color=Green;break;
	}
	print_color(content,color);
}

void Display::print_cursor(Block *target,Direction direction)
{
	move(target->y(),target->x());
	if(target->type==Block::Start) print_block(target,"-A");
	else if(target->type==Block::Destination) print_block(target,"-B");
	else print_block(target,"  ");
	target=&Block::maze[target->get_pos(direction)];
	move(target->y(),target->x());
	print_block(target,"[]");
}

void Display::print_block(Block *target,Color color,int delay=0)
{
	move(target->y(),target->x());
	print_color("  ",color);
	while(delay--) getch();
}

void Display::print_block_dim(Block *target)
{
	move(target->y(),target->x());
	attron(COLOR_PAIR(Black_on_white));
	printw("::");
	attroff(COLOR_PAIR(Black_on_white));
}

void Display::print_maze()
{
	for(int i=0;i<Block::maze_height;i++)
	{
		move(i,0);
		for(int j=0;j<Block::maze_width;j++)
		{
			int target=i*Block::maze_width+j;
			if(Block::maze[target].type==Block::Start) print_block(&Block::maze[target],"-A");
			else if(Block::maze[target].type==Block::Destination) print_block(&Block::maze[target],"-B");
			else print_block(&Block::maze[target],"  ");
		}
	}
}

int Display::menu(std::vector<std::string> list)
{
	clear();
	int j=0;
	for(auto i:list)
	{
		mvprintw(j,0,"[%d]%s",j+1,i.c_str());
		j++;
	}
	int resp;
	while(true)
	{
		resp=getch()-48;
		if(resp<=j && resp>0) return resp;
	}
	clear();
}

int Block::y() const
{
	return pos/maze_width;
}

int Block::x() const
{
	return pos%maze_width*2;
}

int Block::get_pos(Direction target)
{
	int ans;
	switch(target)
	{
		case Up:ans=pos-maze_width;if(ans<0) ans=-1;break;
		case Down:ans=pos+maze_width;if(ans>(maze_height*maze_width-1)) ans=-1;break;
		case Left:ans=pos-1;if(pos%maze_width==0) ans=-1;break;
		case Right:ans=pos+1;if(ans%maze_width==0) ans=-1;break;
	}
	return ans;
}

bool blk_comp(const Block &a,const Block &b,const Block &std,bool less)
{
	auto distance=[](const Block &a,const Block &b)->double
	{
		auto v_module=[](double x,double y)->double
		{
			return sqrt(x*x+y*y);
		};
		return v_module(abs(a.y()-b.y()),abs(a.x()/2-b.x()/2));
	};
	if(less) return distance(a,std)<distance(b,std);
	return distance(a,std)>distance(b,std);
}

void Block::update_possibility()
{
	this->possibility--;
	if(this->possibility!=0) return;//if not return here, the current block has no access to destination
	if(this->type==Path) display.print_block_dim(this);//don't dim start or destination
	if(from!=nullptr) from->update_possibility();//inform the parent block
}

Block::Block(Type content,int position):type(content),pos(position),discovered(false),bidirectional_discovered_a(false),bidirectional_discovered_b(false),possibility(0),from(nullptr){}

void Block::status_reset()
{
	solution.clear();
	for(int i=0;i<maze_height*maze_width;i++) maze[i]=Block(maze[i].type,maze[i].pos);//reset all data used in search
}

int Block::dfs(bool a_star)
{
	getch();
	if(this->type==Destination) return 1;
	if(this->type==Path) display.print_block(this,Display::Cyan);
	this->discovered=true;
	std::vector<Block*> dfs_pending;
	for(auto i:{Up,Down,Left,Right})//push all fresh & discoverable adjacent block to dfs_pending
	{
		if(get_pos(i)<0) continue;
		Block *target=&maze[get_pos(i)];
		if(target->discovered==false && (target->type==Path || target->type==Destination)) dfs_pending.push_back(target);
	}
	if(a_star) std::sort(dfs_pending.begin(),dfs_pending.end(),[](Block *a,Block *b){return blk_comp(*a,*b,maze[dest],true);});
	for(auto i:dfs_pending)
	{
		if(i->dfs(a_star))
		{
			if(this->type!=Path) return 1;//don't change color of start & destination
			display.print_block(this,Display::Green);
			solution.push_front(this);
			return 1;
		}
	}
	if(this->type==Path) display.print_block_dim(this);
	return 0;
}

bool Block::dfs_start(bool a_star)
{
	status_reset();
	if(maze[src].dfs(a_star)) return true;
	return false;
}

template <typename container>
int Block::generic_search(bool bi_direc,container pending_a,container pending_b)
{
	bool sw=true;//if sw==true, search from the start side. otherwise, search from the destination side.
	while(pending_a.empty()==false || pending_b.empty()==false)
	{
		if(bi_direc==false && pending_a.empty()==true) return 2;//no valid solution
		if((sw?pending_a:pending_b).empty()==false)
		{
			Block *current=(sw?pending_a:pending_b).top();
			(sw?pending_a:pending_b).pop();//pick one candidate
			if(current->type==Path) display.print_block(current,(sw?Display::Cyan:Display::Yellow),1);
			(sw?current->bidirectional_discovered_a:current->bidirectional_discovered_b)=true;
			for(auto i:{Up,Down,Left,Right})
			{
				if(current->get_pos(i)<0) continue;
				Block *target=&maze[current->get_pos(i)];
				if((sw?target->bidirectional_discovered_b:target->bidirectional_discovered_a)==true)//found the destination
				{
					Block *trace=current;
					while(trace->from!=nullptr)
					{
						sw?solution.push_front(trace):solution.push_back(trace);
						display.print_block(trace,Display::Green);
						trace=trace->from;
					}
					trace=target;
					while(trace->from!=nullptr)
					{
						sw?solution.push_back(trace):solution.push_front(trace);
						display.print_block(trace,Display::Green);
						trace=trace->from;
					}
					return 1;
				}
				if(target->discovered==false && target->type==Path)//add this block to pending queue
				{
					(sw?pending_a:pending_b).push(target);
					target->from=current;
					(sw?target->bidirectional_discovered_a:target->bidirectional_discovered_b)=true;
					target->discovered=true;
					current->possibility++;
				}
			}
			if(current->possibility==0)//dead end
			{
				if(current->type==Path) display.print_block_dim(current);
				if(current->from!=nullptr) current->from->update_possibility();
			}
		}
		if(bi_direc) sw=!sw;
	}
	return 2;//no valid solution
}

template <typename container>
bool Block::search_start(bool bi_direc,container pending_a,container pending_b)
{
	status_reset();
	pending_a.push(&maze[src]);
	pending_b.push(&maze[dest]);
	maze[src].bidirectional_discovered_a=true;
	maze[src].discovered=true;
	maze[dest].bidirectional_discovered_b=true;
	if(bi_direc) maze[dest].discovered=true;
	if(generic_search(bi_direc,pending_a,pending_b)==1) return true;
	return false;
}


void explore_maze()
{
	class fifo_container:public std::queue<Block*>
	{
	public:
		Block* top()
		{
			return front();
		}
	};
	std::priority_queue<Block*,std::vector<Block*>,std::function<bool(Block*,Block*)>> heap_a([](Block *a,Block *b){return blk_comp(*a,*b,Block::maze[Block::dest],false);});
	std::priority_queue<Block*,std::vector<Block*>,std::function<bool(Block*,Block*)>> heap_b([](Block *a,Block *b){return blk_comp(*a,*b,Block::maze[Block::src],false);});
	int resp=Display::menu({"Plain DFS","A-Star DFS","Plain BFS","Bidirectional BFS","A-Star BFS-like","Bidirectional A-Star BFS-like"});
	display.print_maze();
	bool stat;
	switch(resp)
	{
		case 1:stat=Block::dfs_start(false);break;
		case 2:stat=Block::dfs_start(true);break;
		case 3:stat=Block::search_start(false,fifo_container(),fifo_container());break;
		case 4:stat=Block::search_start(true,fifo_container(),fifo_container());break;
		case 5:stat=Block::search_start(false,heap_a,heap_b);break;
		case 6:stat=Block::search_start(true,heap_a,heap_b);break;
	}
	if(stat) for(auto i:Block::solution)
	{
		move(i->y(),i->x());
		display.print_color("::",Display::Green);
	}
	else mvprintw(0,0,"No valid solution.");
	getch();getch();getch();getch();
}

Block::Block(){};

void Block::export_file(std::ofstream &file)
{
	file<<maze_width<<" "<<maze_height<<std::endl;
	file<<src<<" "<<dest<<std::endl;
	for(int i=0;i<maze_width*maze_height;i++)
	{
		switch(maze[i].type)
		{
			case Path:file<<'P';break;
			case Wall:file<<'W';break;
			case Start:file<<'S';break;
			case Destination:file<<'D';break;
		}
	}
}

void Block::import_file(std::ifstream &file)
{
	file>>maze_width>>maze_height;
	file>>src>>dest;
	maze=new Block[maze_width*maze_height];
	char c;
	for(int i=0;i<maze_width*maze_height;i++)
	{
		file>>c;
		switch(c)
		{
			case 'P':maze[i]=Block(Path,i);break;
			case 'W':maze[i]=Block(Wall,i);break;
			case 'S':maze[i]=Block(Start,i);break;
			case 'D':maze[i]=Block(Destination,i);break;
		}
	}
}

void Block::random_pattern()
{
	for(int i=0;i<maze_width*maze_height;i++) maze[i]=Block(Wall,i);
	std::vector<Block*> pending;
	maze[maze_width+3].from=&maze[maze_width+2];
	pending.push_back(&maze[maze_width+3]);
	maze[3*maze_width+1].from=&maze[2*maze_width+1];
	pending.push_back(&maze[3*maze_width+1]);
	while(pending.empty()==false)
	{
		int pos=rand()%pending.size();
		Block *target=pending[pos];
		pending.erase(pending.begin()+pos);
		target->from->type=Path;
		for(auto i:{Up,Down,Left,Right})
		{
			if(target->get_pos(i)<0) continue;
			Block *adjacent_block=&maze[target->get_pos(i)];
			if(adjacent_block->get_pos(i)<0) continue;
			Block *next_block=&maze[adjacent_block->get_pos(i)];
			if(next_block->type==Start) continue;
			next_block->type=Start;
			next_block->from=adjacent_block;
			pending.push_back(next_block);
		}
	}
	for(int i=0;i<maze_width*maze_height;i++)
	{
		if(maze[i].type!=Start) continue;
		if(rand()%10) continue;
		for(auto j:{Up,Down,Left,Right})
		{
			if(rand()%4) continue;
			if(maze[i].get_pos(j)<0) continue;
			Block *adjacent_block=&maze[maze[i].get_pos(j)];
			adjacent_block->type=Path;
		}
		if(rand()%10) continue;
		maze[i].type=Wall;
	}
	for(int i=0;i<maze_width*maze_height;i++) if(maze[i].type==Start) maze[i].type=Path;
	maze[rand()%(maze_height*maze_width)].type=Start;
	maze[rand()%(maze_height*maze_width)].type=Destination;
}

void Block::editor()
{
	clear();
	enum Operation{None,Move,Edit};
	Block *current=nullptr;
	auto refresh_display=[](Block *&current)->void
	{
		display.print_maze();
		move(LINES-2,0);display.print_color("Z",Display::Black_on_white);mvprintw(LINES-2,1,"=Wall ");
		move(LINES-2,7);display.print_color("X",Display::White_on_black);mvprintw(LINES-2,8,"=Path ");
		move(LINES-2,14);display.print_color("A",Display::Green);mvprintw(LINES-2,15,"=Start ");
		move(LINES-2,22);display.print_color("B",Display::Green);mvprintw(LINES-2,23,"=Destination");
		mvprintw(LINES-1,0,"Press Backspace/Q to save; Press R for a random pattern");
		current=&maze[0];
		move(0,0);
		display.print_block(current,"[]");
	};
	int resp;
	refresh_display(current);
	while(true)
	{
		map_edit_loop:
		Operation operation;
		Type blk_typ;
		resp=getch();
		if(resp=='R' || resp=='r')
		{
			random_pattern();
			refresh_display(current);
		}
		if(resp==KEY_BACKSPACE || resp=='Q' || resp=='q')
		{
			int new_start=-1,new_dest=-1;
			for(int i=0;i<maze_height;i++)
			{
				for(int j=0;j<maze_width;j++)
				{
					Block *target=&maze[i*maze_width+j];
					if(target->type==Start || target->type==Destination)
					{
						if((target->type==Start && new_start!=-1) || (target->type==Destination && new_dest!=-1))
						{
							mvprintw(0,0,"Multiple starting points/destinations is not allowed!");
							getch();getch();
							clear();refresh_display(current);
							goto map_edit_loop;
						}
						if(target->type==Start) new_start=target->pos;
						else new_dest=target->pos;
					}
				}
			}
			if(new_start==-1 || new_dest==-1)
			{
				mvprintw(0,0,"Please add both starting point and destination.");
				getch();getch();
				clear();refresh_display(current);
				goto map_edit_loop;
			}
			src=new_start;
			dest=new_dest;
			return;
		}
		Direction direction;
		switch(resp)
		{
			case KEY_UP:operation=Move;direction=Up;break;
			case KEY_DOWN:operation=Move;direction=Down;break;
			case KEY_LEFT:operation=Move;direction=Left;break;
			case KEY_RIGHT:operation=Move;direction=Right;break;
			case 'Z':case 'z':operation=Edit;blk_typ=Wall;break;
			case 'X':case 'x':operation=Edit;blk_typ=Path;break;
			case 'A':case 'a':operation=Edit;blk_typ=Start;break;
			case 'B':case 'b':operation=Edit;blk_typ=Destination;break;
			default:operation=None;break;
		}
		if(operation==Move && current->get_pos(direction)>=0)
		{
			display.print_cursor(current,direction);
			current=&maze[current->get_pos(direction)];
		}
		if(operation==Edit)
		{
			current->type=blk_typ;
			move(current->y(),current->x());
			display.print_block(current,"[]");
		}
	}
}

std::string read_filename()
{
	clear();
	mvprintw(0,0,"Enter filename:\nPress Enter or arrow key to confirm\n> _");
	std::string filename;
	int resp;
	while(true)
	{
		move(2,filename.length()+2);
		resp=getch();
		if(resp==10 || resp==KEY_LEFT || resp==KEY_RIGHT|| resp==KEY_UP || resp==KEY_DOWN) break;
		if(resp==KEY_BACKSPACE && filename.length()>0)
		{
			mvprintw(2,filename.length()+1,"_ ");
			filename.pop_back();
		}
		if(resp<32 || resp>126) continue;
		printw("%c_",resp);
		filename+=resp;
	}
	clear();
	return filename;
}

int import_maze()
{
	if(Block::maze!=nullptr) delete []Block::maze;
	Block::maze=nullptr;
	std::string filename=read_filename();
	std::ifstream file(filename);
	if(file.is_open()==false)
	{
		mvprintw(0,0,"Unable to open file.");
		getch();getch();
		return 1;
	}
	Block::import_file(file);
	return 0;
}

int export_maze()
{
	std::string filename=read_filename();
	std::ofstream file(filename);
	if(file.is_open()==false)
	{
		mvprintw(0,0,"Unable to open file.");
		getch();getch();
		return 1;
	}
	Block::export_file(file);
	return 0;
}

void open_editor()
{
	if(Block::maze==nullptr)
	{
		int resp=Display::menu({"10x10 maze","15x15 maze","25x25 maze","80x40 maze"});
		int size;
		switch(resp)
		{
			case 1:size=100;Block::maze_width=10;Block::maze_height=10;break;
			case 2:size=225;Block::maze_width=15;Block::maze_height=15;break;
			case 3:size=625;Block::maze_width=25;Block::maze_height=25;break;
			case 4:size=3200;Block::maze_width=80;Block::maze_height=40;break;
		}
		Block::maze=new Block[size];
		for(int i=0;i<size;i++) Block::maze[i]=Block(Block::Wall,i);
	}
	Block::editor();
}

void main_menu()
{
	while(true)
	{
		std::vector<std::string> list={"Exit","Import maze from file","Create an empty maze"};
		if(Block::maze!=nullptr)
		{
			list[2]="Open editor";
			list.push_back("Export maze to file");
			list.push_back("Explore maze");
		}
		int resp=Display::menu(list);
		switch(resp)
		{
			case 1:return;break;
			case 2:while(import_maze());break;
			case 3:open_editor();break;
			case 4:while(export_maze());break;
			case 5:explore_maze();break;
		}
	}
}

void init()
{
	srand((unsigned)clock());
	Block::maze=nullptr;
}

void postexec()
{
	if(Block::maze!=nullptr) delete []Block::maze;
}

int main()
{
	init();
	main_menu();
	postexec();
}
