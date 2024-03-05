#ifndef MAZE_HPP
#define MAZE_HPP

#include <vector>
#include <queue>
#include <deque>
#include <stack>
#include <fstream>
#include <functional>

enum Direction{Up,Down,Left,Right};

class Block
{
public:
	enum Type{Path,Wall,Start,Destination};
	static Block *maze;
	Type type;
	static int maze_width,maze_height;
	Block(Type content,int position);
	Block();
	int pos;//self position
	static void status_reset();
	static bool dfs_start(bool a_star);
	template <typename container>
	static bool search_start(bool bi_direc,container pending_a,container pending_b);
	static std::deque<Block*> solution;
	static int src,dest;
	static void import_file(std::ifstream &file);
	static void export_file(std::ofstream &file);
	static void editor();
	int get_pos(Direction target);
	int y() const;
	int x() const;
private:
	static void random_pattern();
	int dfs(bool a_star);
	template <typename container>
	static int generic_search(bool bi_direc,container pending_a,container pending_b);
	bool discovered;//used in search
	bool bidirectional_discovered_a;
	bool bidirectional_discovered_b;
	int possibility;
	Block *from;
	void update_possibility();
};

class Display
{
public:
	Display();
	~Display();
	enum Color{Black_on_white,White_on_black,Cyan,Red,Green,Yellow};
	void print_maze();
	void print_block(Block *target,const char* content);
	void print_block(Block *target,Color color,int delay);
	void print_block_dim(Block *target);
	void print_color(const char* content,Color color);
	void print_cursor(Block *target,Direction direction);
	static int menu(std::vector<std::string>);
};

#endif
