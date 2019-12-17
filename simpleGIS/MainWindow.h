#pragma once
#pragma execution_character_set("utf-8")

#include <QtWidgets/QMainWindow>
#include "ui_simpleGIS.h"
#include "docParser.h"
#include "addODSDialog.h"
#include "qopenglfunctions.h"
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = Q_NULLPTR);
	//全局变量 
	CGeoMap* map;

private:
	Ui::MainWindow* ui;
	//对话框
	addODSDialog* ods_dialog = new addODSDialog();
	MyOpenGLWidget* mapWidget;
private slots:
	void action_geojson_J();//打开JSON文件动作对应的槽函数
	void action_Shape_S();//打开shape文件动作对应的槽函数
	void action_postgis_P();//打开postgis文件动作对应的槽函数
	void receive_ods_data(QString obs_path, QString layer_name);
signals:
	void emitMapData(CGeoMap* map);
};
