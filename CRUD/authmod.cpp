#include "authmod.h"
#include "useritem.h"

AuthMod::AuthMod(QString ti, UserItem *t,QWidget *parent)
	: title(ti),target(t),QDialog{parent}
{
	//constructor used to create or modify user
	//*parent must be UserMgr
	origname=target->name;
	this->setWindowTitle(title);
	this->setMinimumSize(240,240);
	buttonbox=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,this);
	QObject::connect(buttonbox,&QDialogButtonBox::accepted,this,&AuthMod::onUserAddSave);
	QObject::connect(buttonbox,&QDialogButtonBox::rejected,this,&QDialog::reject);
	//
	validator=new QRegularExpressionValidator(QRegularExpression("[A-Za-z0-9]{0,24}"),this);
	nameprompt=new QLabel(this);
	passwdprompt=new QLabel(this);
	nameinput=new QLineEdit(this);
	nameinput->setMaxLength(16);
	nameinput->setText(target->name);
	nameinput->setPlaceholderText("Username");
	nameinput->setValidator(validator);

	passwdinput=new QLineEdit(this);
	passwdinput->setMaxLength(24);
	passwdinput->setEchoMode(QLineEdit::Password);
	passwdinput->setText(target->passwd);
	passwdinput->setPlaceholderText("Password");
	passwdinput->setValidator(validator);
	buttonbox->button(QDialogButtonBox::Ok)->setEnabled(false);
	buttonbox->button(QDialogButtonBox::Ok)->setText("OK");
	buttonbox->button(QDialogButtonBox::Cancel)->setText("Cancel");
	//
	QObject::connect(nameinput,&QLineEdit::textChanged,this,&AuthMod::onUserAddInput);
	QObject::connect(passwdinput,&QLineEdit::textChanged,this,&AuthMod::onUserAddInput);
	QObject::connect(nameinput,&QLineEdit::returnPressed,[&]{passwdinput->setFocus();});
	onUserAddInput();
	nameinput->setFocus();
}

void AuthMod::onUserAddInput()
{
	bool sanity=true;
	if(nameinput->text()=="")
	{
		nameprompt->setText("Provide username.");
		sanity=false;
	}
	else if((nameinput->text()!=origname)&& !(((QListWidget*)parent() )->findItems(nameinput->text(),Qt::MatchExactly).isEmpty()))
	{
		nameprompt->setText("Duplicated username.");
		sanity=false;
	}
	else nameprompt->setText("");
	if(passwdinput->text()=="")
	{
		passwdprompt->setText("Provide password.");
		sanity=false;
	}
	else passwdprompt->setText("");
	buttonbox->button(QDialogButtonBox::Ok)->setEnabled(sanity);
}

void AuthMod::onUserAddSave()
{
	target->name=nameinput->text();
	target->setText(target->name);
	target->passwd=passwdinput->text();
	accept();
}

void AuthMod::resizeEvent(QResizeEvent* event)
{
	if(buttonbox!=nullptr)
	{
		buttonbox->resize(width()-24,36);
		buttonbox->move(12,height()-12-36);
	}
	if(nameprompt!=nullptr)
	{
		nameprompt->resize(width()-24,20);
		nameprompt->move(12,24);
	}
	if(nameinput!=nullptr)
	{
		nameinput->resize(width()-24,24);
		nameinput->move(12,24+20);
	}
	if(passwdprompt!=nullptr)
	{
		passwdprompt->resize(width()-24,20);
		passwdprompt->move(12,24+20+24);
	}
	if(passwdinput!=nullptr)
	{
		passwdinput->resize(width()-24,24);
		passwdinput->move(12,24+20+24+20);
	}
	QDialog::resizeEvent(event);
}

AuthMod::AuthMod(QString ti,QString *nm,bool *ad,QWidget *parent)
	: title(ti),name(nm),admin(ad),QDialog{parent}
{
	//constructor used to login
	target=nullptr;
	this->setWindowTitle(title);
	this->setMinimumSize(240,240);
	buttonbox=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,this);
	QObject::connect(buttonbox,&QDialogButtonBox::accepted,this,&AuthMod::onLoginTry);
	QObject::connect(buttonbox,&QDialogButtonBox::rejected,this,&QDialog::reject);
	//
	validator=new QRegularExpressionValidator(QRegularExpression("[A-Za-z0-9]{0,24}"),this);
	nameprompt=new QLabel(this);
	passwdprompt=new QLabel(this);
	nameinput=new QLineEdit(this);
	nameinput->setMaxLength(16);
	nameinput->setPlaceholderText("Username");
	nameinput->setValidator(validator);

	passwdinput=new QLineEdit(this);
	passwdinput->setMaxLength(24);
	passwdinput->setEchoMode(QLineEdit::Password);
	passwdinput->setPlaceholderText("Password");
	passwdinput->setValidator(validator);
	buttonbox->button(QDialogButtonBox::Ok)->setEnabled(false);
	buttonbox->button(QDialogButtonBox::Ok)->setText("OK");
	buttonbox->button(QDialogButtonBox::Cancel)->setText("Cancel");
	//
	QObject::connect(nameinput,&QLineEdit::textChanged,this,&AuthMod::onLoginInput);
	QObject::connect(passwdinput,&QLineEdit::textChanged,this,&AuthMod::onLoginInput);
	QObject::connect(nameinput,&QLineEdit::returnPressed,[&]{passwdinput->setFocus();});
	onUserAddInput();
	nameinput->setFocus();
}

void AuthMod::onLoginTry()
{
	{
		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","userdata");
		db.setDatabaseName("user.dat");
		db.open();
		QSqlQuery query(db);
		query.prepare("SELECT passwd , isadmin FROM users WHERE name = :name");
		query.bindValue(":name",nameinput->text());
		if(query.exec())
		{
			query.next();
			if(query.isValid() && (query.value(0).toString()==passwdinput->text()))
			{
				*name=nameinput->text();
				*admin=query.value(1).toBool();
				accept();
			}
			else
			{
				nameprompt->setText("Login Incorrect");
				buttonbox->button(QDialogButtonBox::Ok)->setEnabled(false);
				nameinput->setFocus();
			}
		}
	}
	QSqlDatabase::removeDatabase("userdata");
}

void AuthMod::onLoginInput()
{
	bool sanity=true;
	if(nameinput->text()=="")
	{
		nameprompt->setText("Provide username.");
		sanity=false;
	}
	else nameprompt->setText("");
	if(passwdinput->text()=="")
	{
		passwdprompt->setText("Provide password.");
		sanity=false;
	}
	else passwdprompt->setText("");
	buttonbox->button(QDialogButtonBox::Ok)->setEnabled(sanity);
}
