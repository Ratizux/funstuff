#ifndef ANALYZE_H
#define ANALYZE_H

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
#include "sell.h"

class ProductAnalyzeLabel : public QLabel
{
public:
	explicit ProductAnalyzeLabel(QString prefix,QWidget *parent=nullptr);
	QString prefixtext,realtext;
	void setRealText(const QString target);
	const QString getRealText();
};

class Analyze : public QDialog
{
public:
	explicit Analyze(QString uname,QString title,QWidget *parent = nullptr);
	QString username;
	QLabel *datefromprompt,*datetoprompt;
	QDateTimeEdit *datefrom,*dateto;
	ProductAnalyzeLabel *saleslabel,*turnoverlabel;
	QListWidget *goods;
	QPushButton *edit;
signals:

public slots:
	void onSelectionChange();
	void onUpdate();
	void onEdit();
private:
	void resizeEvent(QResizeEvent* event) override;
};

#endif // ANALYZE_H
