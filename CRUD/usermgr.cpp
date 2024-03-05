#include "usermgr.h"

UserMgr::UserMgr(QPushButton *a,QPushButton *b,QPushButton *c,QWidget *parent)
	: useradd(a),userdel(b),usermod(c),QListWidget{parent}
{
	this->resize(100,100);//just debug purpose
	//make sure buttons are not nullptr, or something bad will happen!
	if(useradd!=nullptr) QObject::connect(useradd,&QPushButton::clicked,this,&UserMgr::onUserAddClick);
	if(userdel!=nullptr) QObject::connect(userdel,&QPushButton::clicked,this,&UserMgr::onUserDelClick);
	if(usermod!=nullptr) QObject::connect(usermod,&QPushButton::clicked,this,&UserMgr::onUserModClick);
	QObject::connect(this,&QListWidget::itemSelectionChanged,this,&UserMgr::onSelectedChange);
	//
	{
		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","userdata");
		db.setDatabaseName("user.dat");
		if(db.open())
		{
			bool sanity=true;
			QSqlQuery query(db);
			if(!db.tables().contains("users",Qt::CaseInsensitive))
			{
				sanity=false;
				//create table
				if(query.exec("CREATE TABLE users (name TEXT PRIMARY KEY NOT NULL,passwd TEXT NOT NULL,isadmin INTEGER NOT NULL);"))
				{
					if(query.exec("INSERT INTO users (name,passwd,isadmin) VALUES (\"root\",\"0x10c\",1)"))
					{
						qDebug()<<"Database initialized successfully.";
						sanity=true;
					}
					else qDebug()<<"Unable to create default root user.";
				}
				else qDebug()<<"Unable to create table.";
			}
			if(sanity)
			{
				//there is already existing userdata, so query and parse
				if(query.exec("SELECT * FROM users;"))
				{
					while(query.next())
					{
						UserItem *tmp=new UserItem(nullptr);
						tmp->setFlags(tmp->flags()&~Qt::ItemIsEditable);
						tmp->name=query.value(0).toString();
						tmp->setText(tmp->name);
						tmp->passwd=query.value(1).toString();
						tmp->admin=query.value(2).toBool();
						if(currentItem()==nullptr)
						{
							addItem(tmp);
						}
						else
						{
							insertItem(currentRow()+1,tmp);
						}
					}
				}
			}
		}
		else
		{
			qDebug()<<"Failed to open database \"user.dat\".";
		}
	}
	QSqlDatabase::removeDatabase("userdata");
	//
	onSelectedChange();
}

void UserMgr::onUserAddClick()
{
	UserItem *tmp=new UserItem(nullptr);
	tmp->setFlags(tmp->flags()&~Qt::ItemIsEditable);
	AuthMod auth("Create User",tmp,this);
	bool sanity=false;
	if(auth.exec())//user syntax ok
	{
		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","userdata");
		db.setDatabaseName("user.dat");
		db.open();
		QSqlQuery query(db);
		query.prepare("INSERT INTO users ( name , passwd , isadmin ) VALUES ( :name , :passwd , :isadmin )");
		query.bindValue(":name",tmp->name);
		query.bindValue(":passwd",tmp->passwd);
		query.bindValue(":isadmin",(tmp->admin)?1:0);
		sanity=query.exec();
		if(sanity)
		{
			if(currentItem()==nullptr)
			{
				addItem(tmp);
			}
			else
			{
				insertItem(currentRow()+1,tmp);
			}
		}
	}
	QSqlDatabase::removeDatabase("userdata");
	if(sanity==false) delete tmp;//give up
}

void UserMgr::onUserModClick()
{
	UserItem *target=dynamic_cast<UserItem*>(this->currentItem());
	if(target!=nullptr)
	{
		QString oldname=target->name;
		QString oldpasswd=target->passwd;
		AuthMod auth("Modify User",target,this);
		if(auth.exec())
		{
			bool sanity;
			{
				QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","userdata");
				db.setDatabaseName("user.dat");
				db.open();
				QSqlQuery query(db);
				query.prepare("UPDATE users SET name = :name, passwd = :passwd WHERE name = :oldname");
				query.bindValue(":name",target->name);
				query.bindValue(":passwd",target->passwd);
				query.bindValue(":oldname",oldname);
				sanity=query.exec();
				if(sanity==false) qDebug()<<query.lastError();
			}
			QSqlDatabase::removeDatabase("userdata");
			if(sanity==false)
			{
				target->name=oldname;
				target->passwd=oldpasswd;
				target->setText(target->name);
			}
		}
	}
}

void UserMgr::onUserDelClick()
{
	UserItem *target=dynamic_cast<UserItem*>(this->currentItem());
	if(target!=nullptr)
	if(!(target->admin))
	if(QMessageBox::question(this,"Confirm","Are you sure? This user will be removed permanently!","No","Yes"))
	{
		bool sanity;
		{
			QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","userdata");
			db.setDatabaseName("user.dat");
			db.open();
			QSqlQuery query(db);
			query.prepare("DELETE FROM users WHERE name = :name");
			query.bindValue(":name",target->name);
			sanity=query.exec();
			if(sanity==false) qDebug()<<query.lastError();
		}
		QSqlDatabase::removeDatabase("userdata");
		if(sanity)
		{
			this->takeItem(this->currentRow());
			onSelectedChange();
		}
	}
}

void UserMgr::onSelectedChange()
{
	bool stat=false;
	if(currentItem()==nullptr) stat=true;
	//
	if(userdel!=nullptr)
	{
		if(stat==false)
		{
			if((dynamic_cast<UserItem*>(currentItem()))->admin) userdel->setDisabled(true);
			else userdel->setDisabled(false);
		}
		else userdel->setDisabled(true);
	}
	if(usermod!=nullptr) usermod->setDisabled(stat);
}
