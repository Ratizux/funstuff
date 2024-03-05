#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QJsonArray>
#include "usermgrwindow.h"
#include "authmod.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
	QLabel *currentuser=nullptr;
	QPushButton *userloginout=nullptr;
	QString username;
	bool useradmin;
	QPushButton *goodsmanagebutton=nullptr;
	QPushButton *analyzebutton=nullptr;
	QPushButton *usermanagebutton=nullptr;
	QPushButton *importbutton=nullptr;
public slots:
	void onUserLogin();
	void onUserLogout();
	void onLoginKeyDown();
	void onLogoutKeyDown();
	void onGoodsManageKeyDown();
	void onAnalyzeKeyDown();
	void onUserManageKeyDown();
	void onImportKeyDown();
private:
	void resizeEvent(QResizeEvent* event) override;
};
#endif // MAINWINDOW_H
