#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QDebug>

void init()
{
	{
		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","userdata");
		db.setDatabaseName("user.dat");
		if(db.open())
		{
			QSqlQuery query(db);
			if(!db.tables().contains("users",Qt::CaseInsensitive))
			{
				//create table
				if(query.exec("CREATE TABLE users (name TEXT PRIMARY KEY NOT NULL,passwd TEXT NOT NULL,isadmin INTEGER NOT NULL);"))
				{
					if(query.exec("INSERT INTO users (name,passwd,isadmin) VALUES (\"root\",\"0x10c\",1)"))
					{
						qDebug()<<"Database initialized successfully.";
					}
					else qDebug()<<"Unable to create default root user.";
				}
				else qDebug()<<"Unable to create table.";
			}
		}
		else
		{
			qDebug()<<"Failed to open database \"user.dat\".";
		}
	}
	QSqlDatabase::removeDatabase("userdata");
	{
		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","goods");
		db.setDatabaseName("goods.dat");
		if(db.open())
		{
			QSqlQuery query(db);
			if(!db.tables().contains("goods",Qt::CaseInsensitive))
			{
				//create table
				if(query.exec("CREATE TABLE goods (name TEXT PRIMARY KEY NOT NULL,baseprice REAL NOT NULL,stock INTEGER NOT NULL,vendor TEXT NOT NULL);"))
				{
					qDebug()<<"Database initialized successfully.";
				}
				else qDebug()<<"Unable to create table.";
			}
		}
		else
		{
			qDebug()<<"Failed to open database \"goods.dat\".";
		}
	}
	QSqlDatabase::removeDatabase("goods");
	{
		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","operations");
		db.setDatabaseName("operations.dat");
		if(db.open())
		{
			QSqlQuery query(db);
			if(!db.tables().contains("operations",Qt::CaseInsensitive))
			{
				//create table
				if(query.exec("CREATE TABLE operations (id INTEGER PRIMARY KEY NOT NULL,name TEXT NOT NULL,amount INTEGER NOT NULL,price REAL NOT NULL,date INTEGER NOT NULL,operator TEXT NOT NULL,description TEXT);"))
				{
					qDebug()<<"Database initialized successfully.";
				}
				else qDebug()<<"Unable to create table.";
			}
		}
		else
		{
			qDebug()<<"Failed to open database \"operations.dat\".";
		}
	}
	QSqlDatabase::removeDatabase("operations");
}

int main(int argc, char *argv[])
{
	init();
	QApplication a(argc, argv);
	qDebug()<<QStyleFactory::keys();
	//a.setStyle(QStyleFactory::create("Fusion"));
	MainWindow w;
	w.show();
	return a.exec();
}
