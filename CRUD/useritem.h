#ifndef USERITEM_H
#define USERITEM_H

#include <QListWidgetItem>

//QListWidgetItem derived, used to storage necessary additional data in user management.
//Expected to be compatible with QListWidgetItem.

class UserItem : public QListWidgetItem
{
public:
	explicit UserItem(QListWidget *parent = nullptr);
	bool admin=false;//specify if the user has privilege i.e. can modify other users
	//note that admin and normal user will be exported seperately
	QString name="";//just name...
	QString passwd="";
signals:

};

#endif // USERITEM_H
