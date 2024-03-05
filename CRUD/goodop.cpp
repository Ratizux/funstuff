#include "goodop.h"

GoodOperation::GoodOperation(QString operatorname,QString title,QWidget *parent)
	: username(operatorname),QDialog{parent}
{
	this->setMinimumSize(480,360);
	this->setWindowTitle(title);
	nameinput=new QLineEdit(this);
	nameinput->setPlaceholderText("Name");
	nameinput->setText("");
	vendorinput=new QLineEdit(this);
	vendorinput->setPlaceholderText("Vendor");
	vendorinput->setText("");
	searchstatus=new QLabel(this);
	searchresult=new QListWidget(this);
	itemnew=new QPushButton(this);
	itemnew->setText("New");
	itemnew->setDisabled(true);
	itemdelete=new QPushButton(this);
	itemdelete->setText("Delete");
	itemdelete->setDisabled(true);
	itemproperties=new QPushButton(this);
	itemproperties->setText("Properties");
	itemproperties->setDisabled(true);
	itemsell=new QPushButton(this);
	itemsell->setText("Sell");
	itemsell->setDisabled(true);
	//
	QObject::connect(nameinput,&QLineEdit::textChanged,this,&GoodOperation::onUserSearch);
	QObject::connect(vendorinput,&QLineEdit::textChanged,this,&GoodOperation::onUserSearch);
	QObject::connect(itemnew,&QPushButton::clicked,this,&GoodOperation::onProductCreate);
	QObject::connect(itemdelete,&QPushButton::clicked,this,&GoodOperation::onProductDelete);
	QObject::connect(itemproperties,&QPushButton::clicked,this,&GoodOperation::onProductModify);
	QObject::connect(itemsell,&QPushButton::clicked,this,&GoodOperation::onProductSell);
	QObject::connect(searchresult,&QListWidget::itemSelectionChanged,this,&GoodOperation::onSelectedChange);
	onUserSearch();
}

void GoodOperation::resizeEvent(QResizeEvent* event)
{
	nameinput->resize(240,36);
	nameinput->move(12,12);
	vendorinput->resize(240,36);
	vendorinput->move(12,12+36+6);
	searchstatus->resize(width()-12-12,16);
	searchstatus->move(12,12+36+6+36+6);
	searchresult->resize(width()-12-6-72-12,height()-12-36-6-36-24-12);
	searchresult->move(12,12+36+6+36+24);
	itemnew->resize(72,36);
	itemnew->move(width()-12-72,12+36+6+36+24);
	itemdelete->resize(72,36);
	itemdelete->move(width()-12-72,12+36+6+36+24+40);
	itemproperties->resize(72,36);
	itemproperties->move(width()-12-72,12+36+6+36+24+40+40);
	itemsell->resize(72,36);
	itemsell->move(width()-12-72,12+36+6+36+24+40+40+40);
	QDialog::resizeEvent(event);
}

void GoodOperation::onUserSearch()
{
	searchresult->clear();
	/*if(nameinput->text()=="" && vendorinput->text()=="")
	{
		//user entered nothing
		itemnew->setEnabled(false);
		searchstatus->setText("Enter product name or vendor to search.");
	}
	else*/
	{
		int entries=0;
		{
			QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","goods");
			db.setDatabaseName("goods.dat");
			db.open();
			QSqlQuery query(db);
			if(nameinput->text()=="" && vendorinput->text()=="")
			{
				//list user
				itemnew->setEnabled(true);
				searchstatus->setText("Enter product name or vendor to search.");
				query.prepare("SELECT name , vendor FROM goods");
			}
			else if(vendorinput->text()=="")
			{
				//search only by name
				query.prepare("SELECT name , vendor FROM goods WHERE name LIKE :name");
				query.bindValue(":name",QString("%%1%").arg(nameinput->text()));
			}
			else if(nameinput->text()=="")
			{
				//search only by vendor
				query.prepare("SELECT name , vendor FROM goods WHERE vendor LIKE :vendor");
				query.bindValue(":vendor",QString("%%1%").arg(vendorinput->text()));
			}
			else
			{
				//search by both parameters
				query.prepare("SELECT name , vendor FROM goods WHERE name LIKE :name AND vendor LIKE :vendor");
				query.bindValue(":name",QString("%%1%").arg(nameinput->text()));
				query.bindValue(":vendor",QString("%%1%").arg(vendorinput->text()));
			}
			if(query.exec())
			{
				Goods *tmp=nullptr;
				while(query.next())
				{
					entries++;
					tmp=new Goods;
					tmp->productname=query.value(0).toString();
					tmp->vendorname=query.value(1).toString();
					tmp->setText(tmp->productname+", "+tmp->vendorname);
					searchresult->addItem(tmp);
				}
			}
			else
			{
				entries=-1;
				searchstatus->setText("Search failed.");
				qDebug()<<query.lastQuery();
				qDebug()<<query.lastError();
			}
		}
		QSqlDatabase::removeDatabase("goods");
		if(entries==0)
		{
			searchstatus->setText("No entry found. Press \"New\" to import product.");
			itemnew->setEnabled(true);
		}
		else if(entries>=1)
		{
			itemnew->setEnabled(true);//itemnew->setEnabled(false);
			if(entries==1) searchstatus->setText("1 entry found.");
			else searchstatus->setText(QString::number(entries)+" entries found.");
		}
	}
	onSelectedChange();
}

void GoodOperation::onSelectedChange()
{
	Goods *target=dynamic_cast<Goods*>(searchresult->currentItem());
	if(target!=nullptr)
	{
		itemdelete->setEnabled(true);
		itemproperties->setEnabled(true);
		itemsell->setEnabled(true);
	}
	else
	{
		itemdelete->setEnabled(false);
		itemproperties->setEnabled(false);
		itemsell->setEnabled(false);
	}
}

void GoodOperation::onProductCreate()
{
	ProductOperation prod(false,username,nameinput->text(),vendorinput->text(),"Import Product",this);
	if(prod.exec()) onUserSearch();
	onSelectedChange();
}

void GoodOperation::onProductModify()
{
	Goods *target=dynamic_cast<Goods*>(searchresult->currentItem());
	if(target!=nullptr)
	{
		ProductOperation prod(true,username,target->productname,target->vendorname,"Modify Product",this);
		if(prod.exec()) onUserSearch();
	}
	onSelectedChange();
}

void GoodOperation::onProductDelete()
{
	if(QMessageBox::question(this,"Confirm","Are you sure? The product will be removed permanently!","No","Yes"))
	{
		Goods *target=dynamic_cast<Goods*>(searchresult->currentItem());
		{
			QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","goods");
			db.setDatabaseName("goods.dat");
			db.open();
			QSqlQuery query(db);
			query.prepare("DELETE FROM goods WHERE name = :name");
			query.bindValue(":name",target->productname);
			query.exec();
		}
		QSqlDatabase::removeDatabase("goods");
		{
			QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","operations");
			db.setDatabaseName("operations.dat");
			db.open();
			QSqlQuery query(db);
			query.prepare("DELETE FROM operations WHERE name = :name");
			query.bindValue(":name",target->productname);
			query.exec();
		}
		QSqlDatabase::removeDatabase("operations");
		searchresult->takeItem(searchresult->currentRow());
		onUserSearch();
	}
	onSelectedChange();
}

void GoodOperation::onProductSell()
{
	Goods *target=dynamic_cast<Goods*>(searchresult->currentItem());
	SaleOperation sell(target->productname,username,"Sell",this);
	sell.exec();
	onSelectedChange();
}
