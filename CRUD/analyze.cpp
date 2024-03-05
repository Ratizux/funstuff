#include "analyze.h"

Analyze::Analyze(QString uname,QString title,QWidget *parent)
	: username(uname),QDialog{parent}
{
	this->setMinimumSize(480,360);
	this->setWindowTitle(title);
	datefromprompt=new QLabel(this);
	datefromprompt->setText("From");
	datetoprompt=new QLabel(this);
	datetoprompt->setText("To");
	datefrom=new QDateTimeEdit(QDateTime::currentDateTime(),this);
	datefrom->setDisplayFormat("yyyy/MM/d hh:mm");
	datefrom->setMinimumDateTime(QDateTime::fromTime_t(0));
	datefrom->setMaximumDateTime(QDateTime::fromTime_t(2147483647));
	dateto=new QDateTimeEdit(QDateTime::currentDateTime(),this);
	dateto->setDisplayFormat("yyyy/MM/d hh:mm");
	dateto->setMinimumDateTime(QDateTime::fromTime_t(0));
	dateto->setMaximumDateTime(QDateTime::fromTime_t(2147483647));
	QObject::connect(datefrom,&QDateTimeEdit::dateTimeChanged,this,&Analyze::onUpdate);
	QObject::connect(dateto,&QDateTimeEdit::dateTimeChanged,this,&Analyze::onUpdate);
	saleslabel=new ProductAnalyzeLabel("Sales: ",this);
	turnoverlabel=new ProductAnalyzeLabel("Turnover: $",this);
	goods=new QListWidget(this);
	QObject::connect(goods,&QListWidget::currentItemChanged,this,&Analyze::onSelectionChange);
	edit=new QPushButton(this);
	edit->setText("Edit");
	QObject::connect(edit,&QPushButton::clicked,this,&Analyze::onEdit);
	onSelectionChange();
	onUpdate();
}

void Analyze::resizeEvent(QResizeEvent* event)
{
	datefromprompt->resize(36,16);
	datefromprompt->move(12,16);
	datetoprompt->resize(36,16);
	datetoprompt->move(12,16+36);
	datefrom->resize(180,36);
	datefrom->move(12+36,12);
	dateto->resize(180,36);
	dateto->move(12+36,12+36);
	saleslabel->resize(width()-12-12,16);
	saleslabel->move(12+240,12+16);
	turnoverlabel->resize(width()-12-12,16);
	turnoverlabel->move(12+240,12+20+16);
	goods->resize(width()-12-6-72-12,height()-(12+36+36+6+12));
	goods->move(12,12+36+36+6);
	edit->resize(72,36);
	edit->move(width()-72-12,12+36+36+6);
	QDialog::resizeEvent(event);
}

void Analyze::onUpdate()
{
	goods->clear();
	int sales=0;
	double turnover=0;
	int unixfrom=datefrom->dateTime().toTime_t();
	int unixto=dateto->dateTime().toTime_t();
	if(unixfrom<=unixto)
	{
		{
			QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","operations");
			db.setDatabaseName("operations.dat");
			db.open();
			QSqlQuery query(db);
			query.prepare("SELECT DISTINCT name FROM operations WHERE amount < 0 AND date >= :datefrom AND date <= :dateto");
			query.bindValue(":datefrom",unixfrom);
			query.bindValue(":dateto",unixto);
			query.exec();
			while(query.next())
			{
				new QListWidgetItem(query.value(0).toString(),goods);
			}
			query.prepare("SELECT amount , price FROM operations WHERE amount < 0 AND date >= :datefrom AND date <= :dateto");
			query.bindValue(":datefrom",unixfrom);
			query.bindValue(":dateto",unixto);
			query.exec();
			while(query.next())
			{
				int delta=-(query.value(0).toInt());
				turnover+=query.value(1).toDouble()*delta;
				sales+=delta;
			}
		}
		QSqlDatabase::removeDatabase("operations");
		saleslabel->setRealText(QString::number(sales));
		if(sales>0)
		{
			turnoverlabel->setRealText(QString::number(turnover));
		}
		else
		{
			turnoverlabel->setRealText("0");
		}
	}
	else
	{
		saleslabel->setRealText("0");
		turnoverlabel->setRealText("0");
	}
	onSelectionChange();
}

ProductAnalyzeLabel::ProductAnalyzeLabel(QString prefix,QWidget *parent)
	:prefixtext(prefix),QLabel(parent)
{
	setRealText("");
}

void ProductAnalyzeLabel::setRealText(const QString target)
{
	realtext=target;
	setText(prefixtext+realtext);
}

const QString ProductAnalyzeLabel::getRealText()
{
	return realtext;
}

void Analyze::onSelectionChange()
{
	QListWidgetItem *target=goods->currentItem();
	if(target)
	{
		edit->setEnabled(true);
	}
	else
	{
		edit->setEnabled(false);
	}
}

void Analyze::onEdit()
{
	QListWidgetItem *target=goods->currentItem();
	if(target)
	{
		SaleOperation sell(target->text(),username,"Edit",this);
		sell.exec();
		onUpdate();
	}
}
