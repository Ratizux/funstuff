#include "id_count.h"

int id_count_get()
{
	QFile file("id_count.dat");
	if(file.open(QIODevice::ReadWrite))
	{
		QTextStream in(&file);
		int id=-1;
		in>>id;
		return id;
	}
	return -1;
}

void id_count_set(int target)
{
	QFile file("id_count.dat");
	if(file.open(QIODevice::WriteOnly))
	{
		QTextStream out(&file);
		out<<target;
	}
}
