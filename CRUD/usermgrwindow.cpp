#include "usermgrwindow.h"

UserMgrWindow::UserMgrWindow(QString title,QWidget *parent)
	: QDialog{parent}
{
	this->setWindowTitle(title);
	this->setMinimumSize(480,360);
	useradd=new QPushButton(this);
	useradd->setText("Add");
	userdel=new QPushButton(this);
	userdel->setText("Delete");
	usermod=new QPushButton(this);
	usermod->setText("Modify");
	mgrcore=new UserMgr(useradd,userdel,usermod,this);
}

void UserMgrWindow::resizeEvent(QResizeEvent* event)
{
	mgrcore->move(12,12);
	mgrcore->resize(width()-12-12-72-12,height()-12-12);
	useradd->resize(72,36);
	useradd->move(width()-72-18,12);
	userdel->resize(72,36);
	userdel->move(width()-72-18,12+40);
	usermod->resize(72,36);
	usermod->move(width()-72-18,12+40+40);
	QDialog::resizeEvent(event);
}

