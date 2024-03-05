#ifndef USERMGR_H
#define USERMGR_H

#include <QListWidget>
#include <QPushButton>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include "authmod.h"

//QListWidget derived
//Have 3 QPushButtons:
//useradd userdel usermod
//can import from file
//can export to file
//use json as format since it is less possible to have millions of salesmen!

class UserMgr : public QListWidget
{
public:
	explicit UserMgr(QPushButton *a,QPushButton *b,QPushButton *c,QWidget *parent = nullptr);

signals:
	void onUserAddClick();
	void onUserDelClick();
	void onUserModClick();
	void onSelectedChange();
private:
	QPushButton *useradd,*userdel,*usermod;
};

#endif // USERMGR_H
