#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	//this->mapWidget =  ui->setupUi(this);
	ui->setupUi(this);
	this->mapWidget = this->ui->mapWidget;
	map = new CGeoMap("map1");
	this->setWindowTitle(map->getPropertyValue("map_name").toString());

	//连接菜单项同具体函数
	connect(this->ui->action_geojson_J, SIGNAL(triggered(bool)), this, SLOT(action_geojson_J()));
	connect(this->ui->action_Shape_S, SIGNAL(triggered(bool)), this, SLOT(action_Shape_S()));
	connect(this->ui->action_postgis_P, SIGNAL(triggered(bool)), this, SLOT(action_postgis_P()));

	connect(ods_dialog, SIGNAL(emit_ods_settings(QString, QString)),this,SLOT(receive_ods_data(QString,QString)));
	this->connect(this, SIGNAL(emitMapData(CGeoMap*)), mapWidget, SLOT(receiveMapData(CGeoMap*)));
}

//geojson连接部分
void MainWindow::action_geojson_J()
{
	//map = new CGeoMap("map1");
	//this->setWindowTitle(map->getMapName());

	//filename包含文件完整的路径url
	QString filename = QFileDialog::getOpenFileName(this, tr("文件"), "D:/Wang/JUNIOR/FIRSR_SEMESTER", tr("数据文件( * json )"));

	//加载文件部分
	QFile load_file;
	load_file.setFileName(filename);
	if (!load_file.open(QIODevice::ReadOnly))
	{
		qWarning() << "couldn't open projects json";
		return;
	}

	//利用filename后作为layername
	int index = filename.lastIndexOf('/');
	QString layer_name = filename.mid(index + 1);//layer_name为简略的文件名

	//map添加layer
	CGeoLayer* layer = new CGeoLayer(layer_name);
	map->addLayer(layer);

	//
	QByteArray all_data = load_file.readAll();
	load_file.close();
	QJsonParseError json_error;
	QJsonDocument json_doc(QJsonDocument::fromJson(all_data, &json_error));

	if (json_error.error != QJsonParseError::NoError)
	{
		qDebug() << "json error!";
		return;
	}

	//geojson数据解析
	docParser parser;
	parser.geoJsonDocParser(json_doc, layer);
	//qDebug() << layer->obj_list.count();
}

//shapefile文件存储部分
void MainWindow::action_Shape_S()
{

	QString filename = QFileDialog::getOpenFileName(this, tr("文件"), "D:/Wang/JUNIOR/FIRSR_SEMESTER", tr("数据文件( * shp )"));
	//shpfile文件解析
	docParser parser;
	parser.shpDocParser(filename, map);
	emit emitMapData(map);
}

void MainWindow::action_postgis_P()
{
	ods_dialog->show();
}

void MainWindow::receive_ods_data(QString ods_path, QString layer_name)
{
	ods_dialog->close();
	docParser parser;
	parser.odsDocParser(ods_path, layer_name, map);
	emit emitMapData(map);
}
