#ifndef UserMgrWindow_H
#define UserMgrWindow_H

#include <QDialog>
#include <QString>
#include <QPushButton>
#include "usermgr.h"

class UserMgrWindow : public QDialog
{
public:
	explicit UserMgrWindow(QString title,QWidget *parent = nullptr);
	UserMgr *mgrcore;
	QPushButton *useradd,*userdel,*usermod;
signals:

public slots:
	//
private:
	void resizeEvent(QResizeEvent* event) override;
};

#endif // UserMgrWindow_H
