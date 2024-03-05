#include "product.h"

ProductOperation::ProductOperation(bool editmode,QString operatorname,QString itemname,QString itemvendor,QString title,QWidget *parent)
	: iseditmode(editmode),olditemname(itemname),username(operatorname),QDialog{parent}
{
	this->setMinimumSize(360,240);
	this->setWindowTitle(title);
	stockvalidator=new QIntValidator(0,1e7,this);
	basepricevalidator=new QDoubleValidator(0,1e7,2,this);
	nameinput=new QLineEdit(this);
	nameinput->setPlaceholderText("Name");
	nameinput->setToolTip("Input name of the product here.");
	nameinput->setText(itemname);
	vendorinput=new QLineEdit(this);
	vendorinput->setPlaceholderText("Vendor");
	vendorinput->setToolTip("Input manufacturer of the product here.");
	vendorinput->setText(itemvendor);
	basepriceinput=new QLineEdit(this);
	basepriceinput->setPlaceholderText("Base Price");
	basepriceinput->setToolTip("Base price, the price when we purchase it.");
	basepriceinput->setValidator(basepricevalidator);
	stockinput=new QLineEdit(this);
	stockinput->setPlaceholderText("Stock");
	stockinput->setToolTip("Stock, number of products in the warehouse.");
	stockinput->setValidator(stockvalidator);
	buttonbox=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,this);
	buttonbox->button(QDialogButtonBox::Ok)->setEnabled(false);
	buttonbox->button(QDialogButtonBox::Ok)->setText("OK");
	buttonbox->button(QDialogButtonBox::Cancel)->setText("Cancel");
	QObject::connect(nameinput,&QLineEdit::textChanged,this,&ProductOperation::onProductPropertyModify);
	QObject::connect(vendorinput,&QLineEdit::textChanged,this,&ProductOperation::onProductPropertyModify);
	QObject::connect(basepriceinput,&QLineEdit::textChanged,this,&ProductOperation::onProductPropertyModify);
	QObject::connect(stockinput,&QLineEdit::textChanged,this,&ProductOperation::onProductPropertyModify);
	if(iseditmode)
	{
		QObject::connect(buttonbox,&QDialogButtonBox::accepted,this,&ProductOperation::onUpdate);
		{
			bool sanity=false;
			QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","goods");
			db.setDatabaseName("goods.dat");
			db.open();
			QSqlQuery query(db);
			query.prepare("SELECT baseprice , stock FROM goods WHERE name = :name");
			query.bindValue(":name",olditemname);
			if(query.exec())
			{
				query.next();
				if(query.isValid())
				{
					basepriceinput->setText(query.value(0).toString());
					stockinput->setText(query.value(1).toString());
					oldstock=stockinput->text().toInt();
					sanity=true;
				}
			}
			if(!sanity)
			{
				QMessageBox::warning(this,"Unable to read database","Unable to read relevant information of product.");
				reject();
			}
		}
		QSqlDatabase::removeDatabase("goods");
	}
	else QObject::connect(buttonbox,&QDialogButtonBox::accepted,this,&ProductOperation::onSubmit);
	QObject::connect(buttonbox,&QDialogButtonBox::rejected,this,&QDialog::reject);
}

void ProductOperation::resizeEvent(QResizeEvent* event)
{
	nameinput->resize(240,36);
	nameinput->move(12,12);
	vendorinput->resize(240,36);
	vendorinput->move(12,12+42);
	basepriceinput->resize(120,36);
	basepriceinput->move(12,12+42+42);
	stockinput->resize(120,36);
	stockinput->move(12,12+42+42+42);
	if(buttonbox!=nullptr)
	{
		buttonbox->resize(width()-24,36);
		buttonbox->move(12,height()-12-36);
	}
	QDialog::resizeEvent(event);
}

void ProductOperation::onProductPropertyModify()
{
	if(nameinput->text()!="" && vendorinput->text()!="" && basepriceinput->text()!="" && stockinput->text()!="")
	{
		buttonbox->button(QDialogButtonBox::Ok)->setEnabled(true);
	}
	else
	{
		buttonbox->button(QDialogButtonBox::Ok)->setEnabled(false);
	}
}

void ProductOperation::onSubmit()
{
	bool dup=true;
	{
		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","goods");
		db.setDatabaseName("goods.dat");
		db.open();
		QSqlQuery query(db);
		query.prepare("SELECT name FROM goods WHERE name = :name");
		query.bindValue(":name",nameinput->text());
		if(query.exec())
		{
			query.next();
			if(query.isValid()==false) dup=false;
		}
	}
	QSqlDatabase::removeDatabase("goods");
	if(dup)
	{
		QMessageBox::warning(this,"Duplicated product name","There is alrealy a product called \""+nameinput->text()+"\" in the database.");
	}
	else
	{
		bool sanity=true;
		{
			QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","goods");
			db.setDatabaseName("goods.dat");
			db.open();
			QSqlQuery query(db);
			query.prepare("INSERT INTO goods ( name , vendor , baseprice , stock ) VALUES ( :name , :vendor , :baseprice , :stock )");
			query.bindValue(":name",nameinput->text());
			query.bindValue(":vendor",vendorinput->text());
			query.bindValue(":baseprice",basepriceinput->text());
			query.bindValue(":stock",stockinput->text());
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
			query.bindValue(":name",nameinput->text());
			query.bindValue(":amount",stockinput->text());
			query.bindValue(":price",basepriceinput->text());
			query.bindValue(":operator",username);
			query.bindValue(":description","Initial purchase");
			if(!query.exec())
			{
				qDebug()<<query.lastError();
				sanity=false;
			}
		}
		QSqlDatabase::removeDatabase("operations");
		if(sanity)
		{
			accept();
		}
		else
		{
			QMessageBox::warning(this,"Unable to insert record","Unable to insert record");
		}
	}
}

void ProductOperation::onUpdate()
{
	bool dup=true;
	{
		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","goods");
		db.setDatabaseName("goods.dat");
		db.open();
		QSqlQuery query(db);
		query.prepare("SELECT name FROM goods WHERE name = :name");
		query.bindValue(":name",nameinput->text());
		if(query.exec())
		{
			query.next();
			if(query.isValid()==false) dup=false;
			else if(nameinput->text()==olditemname) dup=false;
		}
	}
	QSqlDatabase::removeDatabase("goods");
	if(dup)
	{
		QMessageBox::warning(this,"Duplicated product name","There is alrealy a product called \""+nameinput->text()+"\" in the database.");
	}
	else
	{
		bool sanity=true;
		{
			QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","goods");
			db.setDatabaseName("goods.dat");
			db.open();
			QSqlQuery query(db);
			query.prepare("UPDATE goods SET name = :name , vendor = :vendor , baseprice = :baseprice , stock = :stock WHERE name = :oldname");
			query.bindValue(":name",nameinput->text());
			query.bindValue(":vendor",vendorinput->text());
			query.bindValue(":baseprice",basepriceinput->text());
			query.bindValue(":stock",stockinput->text());
			query.bindValue(":oldname",olditemname);
			if(!query.exec()) sanity=false;
		}
		QSqlDatabase::removeDatabase("goods");
		if(olditemname!=nameinput->text())
		{
			{
				QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","operations");
				db.setDatabaseName("operations.dat");
				db.open();
				QSqlQuery query(db);
				query.prepare("UPDATE operations SET name = :name WHERE name = :oldname");
				query.bindValue(":name",nameinput->text());
				query.bindValue(":oldname",olditemname);
				if(!query.exec()) sanity=false;
			}
			QSqlDatabase::removeDatabase("operations");
		}
		int delta=stockinput->text().toInt()-oldstock;
		if(delta!=0)
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
			query.bindValue(":name",nameinput->text());
			query.bindValue(":amount",delta);
			query.bindValue(":price",basepriceinput->text());
			query.bindValue(":operator",username);
			query.bindValue(":description","Stock update");
			if(!query.exec()) sanity=false;
		}
		QSqlDatabase::removeDatabase("operations");
		if(sanity)
		{
			accept();
		}
		else
		{
			QMessageBox::warning(this,"Unable to update database","Unable to update database");
		}
	}
}
