#include "mainwindow.h"
#include "goodop.h"
#include "analyze.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	this->setMinimumSize(480,360);
	currentuser=new QLabel(this);
	currentuser->setStyleSheet("QLabel {font:24px;}");
	userloginout=new QPushButton(this);
	usermanagebutton=new QPushButton(this);
	usermanagebutton->setText("User Manage");
	goodsmanagebutton=new QPushButton(this);
	goodsmanagebutton->setText("Goods Manage");
	analyzebutton=new QPushButton(this);
	analyzebutton->setText("Analyze");
	importbutton=new QPushButton(this);
	importbutton->setText("Import");
	onUserLogout();
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	currentuser->resize(width()-12-64-72-12,36);
	currentuser->move(12,36);
	userloginout->resize(72,36);
	userloginout->move(width()-72-12,36);
	if(usermanagebutton!=nullptr)
	{
		usermanagebutton->resize(128,40);
		usermanagebutton->move(12,height()-72-48-48-48);
	}
	if(goodsmanagebutton!=nullptr)
	{
		goodsmanagebutton->resize(128,40);
		goodsmanagebutton->move(12,height()-72-48-48);
	}
	if(analyzebutton!=nullptr)
	{
		analyzebutton->resize(128,40);
		analyzebutton->move(12,height()-72-48);
	}
	if(importbutton!=nullptr)
	{
		importbutton->resize(128,40);
		importbutton->move(12,height()-72);
	}
	QMainWindow::resizeEvent(event);
}

void MainWindow::onUserLogin()
{
	currentuser->setText(username);
	userloginout->setText("Logout");
	QObject::disconnect(userloginout,&QPushButton::clicked,this,&MainWindow::onLoginKeyDown);
	QObject::connect(userloginout,&QPushButton::clicked,this,&MainWindow::onLogoutKeyDown);
	//
	goodsmanagebutton->setEnabled(true);
	goodsmanagebutton->show();
	QObject::connect(goodsmanagebutton,&QPushButton::clicked,this,&MainWindow::onGoodsManageKeyDown);
	analyzebutton->setEnabled(true);
	analyzebutton->show();
	QObject::connect(analyzebutton,&QPushButton::clicked,this,&MainWindow::onAnalyzeKeyDown);
	importbutton->setEnabled(true);
	importbutton->show();
	QObject::connect(importbutton,&QPushButton::clicked,this,&MainWindow::onImportKeyDown);
	if(useradmin)
	{
		usermanagebutton->setEnabled(true);
		usermanagebutton->show();
		QObject::connect(usermanagebutton,&QPushButton::clicked,this,&MainWindow::onUserManageKeyDown);
	}

}

void MainWindow::onUserLogout()
{
	currentuser->setText("Not logged in");
	userloginout->setText("Login");
	QObject::disconnect(userloginout,&QPushButton::clicked,this,&MainWindow::onLogoutKeyDown);
	QObject::connect(userloginout,&QPushButton::clicked,this,&MainWindow::onLoginKeyDown);
	//
	goodsmanagebutton->setDisabled(true);
	goodsmanagebutton->hide();
	QObject::disconnect(goodsmanagebutton,&QPushButton::clicked,this,&MainWindow::onGoodsManageKeyDown);
	analyzebutton->setDisabled(true);
	analyzebutton->hide();
	QObject::disconnect(analyzebutton,&QPushButton::clicked,this,&MainWindow::onAnalyzeKeyDown);
	importbutton->setDisabled(true);
	importbutton->hide();
	QObject::disconnect(importbutton,&QPushButton::clicked,this,&MainWindow::onImportKeyDown);
	usermanagebutton->setDisabled(true);
	usermanagebutton->hide();
	QObject::disconnect(usermanagebutton,&QPushButton::clicked,this,&MainWindow::onUserManageKeyDown);
}

void MainWindow::onLoginKeyDown()
{
	AuthMod auth("Login",&username,&useradmin,this);
	if(auth.exec()) onUserLogin();
}

void MainWindow::onLogoutKeyDown()
{
	onUserLogout();
}

void MainWindow::onGoodsManageKeyDown()
{
	GoodOperation a(username,"Goods Manage",this);
	a.exec();
}

void MainWindow::onAnalyzeKeyDown()
{
	Analyze a(username,"Analyze",this);
	a.exec();
}

void MainWindow::onUserManageKeyDown()
{
	UserMgrWindow mgr("User Manage",this);
	mgr.exec();
}

void MainWindow::onImportKeyDown()
{
	/* format
	{
		"goods":
		[
			"name",baseprice,stock,"vendor"
		],
		"operations":
		[
			"name",amount,price,date
		]
	}
	*/
	QString filename=QFileDialog::getOpenFileName(this,"Open",QDir::currentPath(),"JSON to be imported (*.json)");
	QFile import;
	import.setFileName(filename);
	import.open(QIODevice::ReadOnly|QIODevice::Text);
	QString json=import.readAll();
	QJsonDocument target=QJsonDocument::fromJson(json.toUtf8());
	QJsonObject obj=target.object();
	QJsonArray goods=obj.value("goods").toArray();
	if((!goods.empty()) && goods.count()%4==0)
	{
		int success=0,failure=0;
		int limit=goods.count();
		for(int i=0;i<limit;i+=4)
		{
			QString insertname=goods[i].toString();
			double insertprice=goods[i+1].toDouble();
			int insertstock=goods[i+2].toInt();
			QString insertvendor=goods[i+3].toString();
			//start inserting goods
			bool sanity=true;
			bool dup=true;
			{
				QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","goods");
				db.setDatabaseName("goods.dat");
				db.open();
				QSqlQuery query(db);
				query.prepare("SELECT name FROM goods WHERE name = :name");
				query.bindValue(":name",insertname);
				if(query.exec())
				{
					query.next();
					if(query.isValid()==false) dup=false;
				}
			}
			QSqlDatabase::removeDatabase("goods");
			if(dup)
			{
				failure++;
			}
			else
			{
				{
					QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","goods");
					db.setDatabaseName("goods.dat");
					db.open();
					QSqlQuery query(db);
					query.prepare("INSERT INTO goods ( name , vendor , baseprice , stock ) VALUES ( :name , :vendor , :baseprice , :stock )");
					query.bindValue(":name",insertname);
					query.bindValue(":vendor",insertvendor);
					query.bindValue(":baseprice",insertprice);
					query.bindValue(":stock",insertstock);
					sanity=query.exec();
					if(sanity==false) qDebug()<<query.lastError();
				}
				QSqlDatabase::removeDatabase("goods");
				{
					QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","operations");
					db.setDatabaseName("operations.dat");
					db.open();
					QSqlQuery query(db);
					query.prepare("INSERT INTO operations ( id , name , amount , price , date , operator , description ) VALUES ( :id , :name , :amount , :price , unixepoch() , :operator , :description )");
					int id=id_count_get();
					if(id<0)
					{
						id=0;
						id_count_set(0);
					}
					query.bindValue(":id",QString::number(id));
					id_count_set(++id);
					query.bindValue(":name",insertname);
					query.bindValue(":amount",insertstock);
					query.bindValue(":price",insertprice);
					query.bindValue(":operator",username);
					query.bindValue(":description","Initial purchase from file import");
					if(!query.exec())
					{
						qDebug()<<query.lastError();
						sanity=false;
					}
				}
				QSqlDatabase::removeDatabase("operations");
				if(sanity)
				{
					success++;
				}
				else
				{
					failure++;
				}
			}
			//stop inserting goods
		}
		QMessageBox::information(this,"Result",QString::number(success)+" success, "+QString::number(failure)+" failure.");
	}
}
