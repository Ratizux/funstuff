#ifndef SELL_H
#define SELL_H

#include <QLineEdit>
#include <QDialog>
#include <QString>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QMessageBox>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDateTimeEdit>
#include "id_count.h"

class ProductLabel : public QLabel
{
public:
	explicit ProductLabel(QString prefix,QWidget *parent=nullptr);
	QString prefixtext,realtext;
	void setRealText(const QString target);
	const QString getRealText();
};

class ProductItem : public QListWidgetItem
{
public:
	int id;
};

class SaleOperation : public QDialog
{
public:
	explicit SaleOperation(QString iname,QString uname,QString title,QWidget *parent = nullptr);
	QString itemname,username;
	ProductLabel *namelabel,*vendorlabel,*basepricelabel,*stocklabel,*saleslabel,*turnoverlabel,*currentearninglabel,*expectedearninglabel;
	QLineEdit *priceinput,*amountinput;
	QDateTimeEdit *dateinput;
	QPushButton *sellbutton;
	QLabel *recordtip;
	QListWidget *records;
	QPushButton *recordrevert,*recordproperties;
	QIntValidator *amountvalidator;
	QDoubleValidator *pricevalidator;
signals:

public slots:
	void onSelectionChange();
	void onUpdate();
	void onRevert();
	void onDisplayProperties();
	void onUserInput();
	void onSell();
private:
	void resizeEvent(QResizeEvent* event) override;
};

#endif // SELL_H
