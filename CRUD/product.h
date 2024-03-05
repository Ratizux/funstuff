#ifndef PRODUCT_H
#define PRODUCT_H

#include <QLineEdit>
#include <QDialog>
#include <QString>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QMessageBox>
#include "id_count.h"

class ProductOperation : public QDialog
{
public:
	explicit ProductOperation(bool editmode,QString operatorname,QString itemname,QString itemvendor,QString title,QWidget *parent = nullptr);
	bool iseditmode;
	QLineEdit *nameinput,*vendorinput,*basepriceinput,*stockinput;
	QDialogButtonBox *buttonbox;
	QIntValidator *stockvalidator;
	QString username;
	QString olditemname;
	QDoubleValidator *basepricevalidator;
	int oldstock;
signals:

public slots:
	void onProductPropertyModify();
	void onSubmit();
	void onUpdate();
private:
	void resizeEvent(QResizeEvent* event) override;
};

#endif // PRODUCT_H
