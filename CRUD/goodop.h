#ifndef GOODOP_H
#define GOODOP_H

#include <QLineEdit>
#include <QDialog>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include "product.h"
#include "sell.h"

class Goods:public QListWidgetItem
{
public:
	QString productname,vendorname;
};

class GoodOperation : public QDialog
{
public:
	explicit GoodOperation(QString operatorname,QString title,QWidget *parent = nullptr);
	QString username;
	QLineEdit *nameinput,*vendorinput;
	QListWidget *searchresult;
	QLabel *searchstatus;
	QPushButton *itemnew,*itemdelete,*itemproperties,*itemsell;
signals:

public slots:
	void onUserSearch();
	void onSelectedChange();
	void onProductCreate();
	void onProductModify();
	void onProductDelete();
	void onProductSell();
private:
	void resizeEvent(QResizeEvent* event) override;
};

#endif // GOODOP_H
