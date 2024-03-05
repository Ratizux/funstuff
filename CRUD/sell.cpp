#include "sell.h"

SaleOperation::SaleOperation(QString iname,QString uname,QString title,QWidget *parent)
	: itemname(iname),username(uname),QDialog{parent}
{
	this->setMinimumSize(480,360);
	this->setWindowTitle(title);
	namelabel=new ProductLabel("Name: ",this);
	vendorlabel=new ProductLabel("Vendor: ",this);
	basepricelabel=new ProductLabel("Base Price: $",this);
	stocklabel=new ProductLabel("Stock: ",this);
	saleslabel=new ProductLabel("Sales: ",this);
	turnoverlabel=new ProductLabel("Turnover: $",this);
	currentearninglabel=new ProductLabel("Current Earning: $",this);
	expectedearninglabel=new ProductLabel("Expected Earning: $",this);
	pricevalidator=new QDoubleValidator(0,1e9,2,this);
	priceinput=new QLineEdit(this);
	priceinput->setPlaceholderText("Price");
	priceinput->setValidator(pricevalidator);
	amountvalidator=new QIntValidator(1,1e7,this);
	amountinput=new QLineEdit(this);
	amountinput->setPlaceholderText("Amount");
	amountinput->setValidator(amountvalidator);
	QObject::connect(priceinput,&QLineEdit::textChanged,this,&SaleOperation::onUserInput);
	QObject::connect(amountinput,&QLineEdit::textChanged,this,&SaleOperation::onUserInput);
	dateinput=new QDateTimeEdit(QDateTime::currentDateTime(),this);
	dateinput->setDisplayFormat("yyyy/MM/d hh:mm");
	dateinput->setMinimumDateTime(QDateTime::fromTime_t(0));
	dateinput->setMaximumDateTime(QDateTime::fromTime_t(2147483647));
	sellbutton=new QPushButton(this);
	sellbutton->setText("Sell");
	QObject::connect(sellbutton,&QPushButton::clicked,this,&SaleOperation::onSell);
	recordtip=new QLabel(this);
	recordtip->setText("Records");
	records=new QListWidget(this);
	QObject::connect(records,&QListWidget::currentItemChanged,this,&SaleOperation::onSelectionChange);
	recordrevert=new QPushButton(this);
	recordrevert->setText("Revert");
	QObject::connect(recordrevert,&QPushButton::clicked,this,&SaleOperation::onRevert);
	recordproperties=new QPushButton(this);
	recordproperties->setText("Properties");
	QObject::connect(recordproperties,&QPushButton::clicked,this,&SaleOperation::onDisplayProperties);
	onSelectionChange();
	onUpdate();
	onUserInput();
}

void SaleOperation::resizeEvent(QResizeEvent* event)
{
	namelabel->resize((width()-12-12-6)/2,16);
	namelabel->move(12,12);
	vendorlabel->resize((width()-12-12-6)/2,16);
	vendorlabel->move(12,12+22);
	basepricelabel->resize((width()-12-12-6)/2,16);
	basepricelabel->move(12,12+22+22);
	stocklabel->resize((width()-12-12-6)/2,16);
	stocklabel->move(12,12+22+22+22);
	saleslabel->resize((width()-12-12-6)/2,16);
	saleslabel->move(12+(width()-12-12-6)/2+6,12);
	turnoverlabel->resize((width()-12-12-6)/2,16);
	turnoverlabel->move(12+(width()-12-12-6)/2+6,12+22);
	currentearninglabel->resize((width()-12-12-6)/2,16);
	currentearninglabel->move(12+(width()-12-12-6)/2+6,12+22+22);
	expectedearninglabel->resize((width()-12-12-6)/2,16);
	expectedearninglabel->move(12+(width()-12-12-6)/2+6,12+22+22+22);
	priceinput->resize(72,36);
	priceinput->move(12,12+22+22+22+16+12);
	amountinput->resize(72,36);
	amountinput->move(12+72+6,12+22+22+22+16+12);
	dateinput->resize(180,36);
	dateinput->move(12+72+6+72+6,12+22+22+22+16+12);
	sellbutton->resize(72,36);
	sellbutton->move(width()-72-12,12+22+22+22+16+12);
	recordtip->resize(120,16);
	recordtip->move(12,12+22+22+22+16+12+36+6);
	records->resize(width()-12-6-72-12,height()-(12+22+22+22+16+12+36+6+16+12));
	records->move(12,12+22+22+22+16+12+36+6+16);
	recordrevert->resize(72,36);
	recordrevert->move(width()-12-72,12+22+22+22+16+12+36+6+16);
	recordproperties->resize(72,36);
	recordproperties->move(width()-12-72,12+22+22+22+16+12+36+6+16+40);
	QDialog::resizeEvent(event);
}

void SaleOperation::onUpdate()
{
	records->clear();
	double baseprice=0;
	int stock=0;
	namelabel->setRealText(itemname);
	{
		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","goods");
		db.setDatabaseName("goods.dat");
		db.open();
		QSqlQuery query(db);
		query.prepare("SELECT vendor , baseprice , stock FROM goods WHERE name = :name");
		query.bindValue(":name",itemname);
		if(query.exec())
		{
			query.next();
			if(query.isValid())
			{
				vendorlabel->setRealText(query.value(0).toString());
				baseprice=query.value(1).toDouble();
				basepricelabel->setRealText(query.value(1).toString());
				stock=query.value(2).toInt();
				stocklabel->setRealText(query.value(2).toString());
			}
		}
	}
	amountvalidator->setTop(stock);
	QSqlDatabase::removeDatabase("goods");
	int sales=0;
	double turnover=0;
	{
		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","operations");
		db.setDatabaseName("operations.dat");
		db.open();
		QSqlQuery query(db);
		query.prepare("SELECT id , amount , price , datetime(date,'unixepoch','localtime') FROM operations WHERE name = :name AND amount < 0");
		query.bindValue(":name",itemname);
		if(query.exec())
		{
			ProductItem *tmp;
			while(query.next())
			{
				if(query.isValid())
				{
					tmp=new ProductItem();
					tmp->id=query.value(0).toInt();
					int amounti=-query.value(1).toInt();
					double pricei=query.value(2).toDouble();
					tmp->setText(QString::number(amounti)+"*$"+query.value(2).toString()+", "+query.value(3).toString());
					records->addItem(tmp);
					sales+=amounti;
					turnover+=(amounti*pricei);
				}
			}
		}
	}
	QSqlDatabase::removeDatabase("operations");
	saleslabel->setRealText(QString::number(sales));
	if(sales>0)
	{
		turnoverlabel->setRealText(QString::number(turnover));
		currentearninglabel->setRealText(QString::number(turnover-baseprice*sales));
		expectedearninglabel->setRealText(QString::number((turnover-baseprice*sales)*(((double)stock+sales)/sales)));
	}
	else
	{
		turnoverlabel->setRealText("0");
		currentearninglabel->setRealText("0");
		expectedearninglabel->setRealText("N/A");
	}
}

ProductLabel::ProductLabel(QString prefix,QWidget *parent)
	:prefixtext(prefix),QLabel(parent)
{
	setRealText("");
}

void ProductLabel::setRealText(const QString target)
{
	realtext=target;
	setText(prefixtext+realtext);
}

const QString ProductLabel::getRealText()
{
	return realtext;
}

void SaleOperation::onSelectionChange()
{
	ProductItem *target=dynamic_cast<ProductItem*>(records->currentItem());
	if(target)
	{
		recordrevert->setEnabled(true);
		recordproperties->setEnabled(true);
	}
	else
	{
		recordrevert->setEnabled(false);
		recordproperties->setEnabled(false);
	}
}

void SaleOperation::onRevert()
{
	//1 revert "amount" of operation to the product
	//2 delete operation
	ProductItem *target=dynamic_cast<ProductItem*>(records->currentItem());
	if(target)
	{
		if(QMessageBox::question(this,"Confirm","Are you sure? The operation will be reverted!","No","Yes"))
		{
			int amount,stock;
			QString productname;
			{
				QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","operations");
				db.setDatabaseName("operations.dat");
				db.open();
				QSqlQuery query(db);
				query.prepare("SELECT amount , name FROM operations WHERE id = :id");
				query.bindValue(":id",target->id);
				if(!query.exec()) qDebug()<<query.lastError();
				query.next();
				amount=-query.value(0).toInt();
				productname=query.value(1).toString();
				query.prepare("DELETE FROM operations WHERE id = :id");
				query.bindValue(":id",target->id);
				if(!query.exec()) qDebug()<<query.lastError();
			}
			QSqlDatabase::removeDatabase("operations");
			{
				QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","goods");
				db.setDatabaseName("goods.dat");
				db.open();
				QSqlQuery query(db);
				query.prepare("SELECT stock FROM goods WHERE name = :name");
				query.bindValue(":name",productname);
				if(!query.exec()) qDebug()<<query.lastError();
				query.next();
				stock=query.value(0).toInt();
				query.prepare("UPDATE goods SET stock = :stock WHERE name = :name");
				query.bindValue(":stock",stock+amount);
				query.bindValue(":name",productname);
				if(!query.exec()) qDebug()<<query.lastError();
			}
			QSqlDatabase::removeDatabase("goods");
			onSelectionChange();
			onUpdate();
		}
	}
}

void SaleOperation::onDisplayProperties()
{
	ProductItem *target=dynamic_cast<ProductItem*>(records->currentItem());
	if(target!=nullptr)
	{
		QString resp;
		{
			QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","operations");
			db.setDatabaseName("operations.dat");
			db.open();
			QSqlQuery query(db);
			query.prepare("SELECT amount , price , datetime(date,'unixepoch','localtime') , operator FROM operations WHERE id = :id");
			query.bindValue(":id",target->id);
			if(!query.exec()) qDebug()<<query.lastError();
			query.next();
			resp="Sold "+QString::number(-query.value(0).toInt())+"*$"+query.value(1).toString()+", at "+query.value(2).toString()+", operated by "+query.value(3).toString();
		}
		QSqlDatabase::removeDatabase("operations");
		QMessageBox::information(this,"Info",resp,"OK");
	}
}

void SaleOperation::onUserInput()
{
	if(priceinput->text()!="" && amountinput->text()!="")
	{
		sellbutton->setEnabled(true);
	}
	else
	{
		sellbutton->setEnabled(false);
	}
}

void SaleOperation::onSell()
{
	int amount=amountinput->text().toInt();
	if(amount<=0) return;
	if(amount>amountvalidator->top())
	{
		amount=amountvalidator->top();
		amountinput->setText(QString::number(amountvalidator->top()));
	}
	//
	{
		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","goods");
		db.setDatabaseName("goods.dat");
		db.open();
		QSqlQuery query(db);
		query.prepare("SELECT stock FROM goods WHERE name = :name");
		query.bindValue(":name",itemname);
		query.exec();
		query.next();
		int stock=query.value(0).toInt();
		query.prepare("UPDATE goods SET stock = :stock WHERE name = :name");
		query.bindValue(":name",itemname);
		query.bindValue(":stock",stock-amount);
		if(!query.exec()) qDebug()<<query.lastError();
	}
	QSqlDatabase::removeDatabase("goods");
	{
		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","operations");
		db.setDatabaseName("operations.dat");
		db.open();
		QSqlQuery query(db);
		query.prepare("INSERT INTO operations ( id , name , amount , price , date , operator , description ) VALUES ( :id , :name , :amount , :price , :date , :operator , :description )");
		int id=id_count_get();
		if(id<0)
		{
			id=0;
			id_count_set(0);
		}
		query.bindValue(":id",QString::number(id));
		id_count_set(++id);
		query.bindValue(":name",itemname);
		query.bindValue(":amount",-amountinput->text().toInt());
		query.bindValue(":price",priceinput->text());
		query.bindValue(":date",dateinput->dateTime().toTime_t());
		query.bindValue(":operator",username);
		query.bindValue(":description","Sell product");
		if(!query.exec()) qDebug()<<query.lastError();
	}
	QSqlDatabase::removeDatabase("operations");
	onSelectionChange();
	onUpdate();
}
