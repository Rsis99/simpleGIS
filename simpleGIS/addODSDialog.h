#pragma once
#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qstring.h>
//作为设置连接数据库参数的相关内容
class addODSDialog :
	public QDialog
{
	Q_OBJECT
public:
	addODSDialog(QWidget* parent = 0);
	QPushButton* ok_button;
private:
	QLineEdit* user_lineEdit = NULL;
	QLineEdit* password_lineEdit = NULL;
	QLineEdit* port_lineEdit = NULL;
	QLineEdit* db_lineEdit = NULL;
	QLineEdit* layer_lineEdit = NULL;

signals:
	void emit_ods_settings(QString obs_path, QString layer_name);

public slots:
	void ok_button_triggered();

};

