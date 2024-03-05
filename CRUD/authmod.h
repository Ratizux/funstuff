#ifndef AUTHMOD_H
#define AUTHMOD_H

#include <QDialog>
#include <QDialogButtonBox>
#include "useritem.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QString>
#include <QRegularExpressionValidator>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
//authenticator module
//

class AuthMod : public QDialog
{
public:
	explicit AuthMod(QString ti,UserItem *t,QWidget *parent = nullptr);
	explicit AuthMod(QString ti,QString *nm,bool *ad,QWidget *parent=nullptr);
	QDialogButtonBox *buttonbox;
	UserItem *target;
	QString title,origname;
	QLabel *nameprompt,*passwdprompt;
	QLineEdit *nameinput,*passwdinput;
	QRegularExpressionValidator *validator;
	bool *admin;
	QString *name;
signals:

public slots:
	void onUserAddInput();
	void onUserAddSave();
	void onLoginTry();
	void onLoginInput();
private:
	void resizeEvent(QResizeEvent* event) override;
};

#endif // AUTHMOD_H
