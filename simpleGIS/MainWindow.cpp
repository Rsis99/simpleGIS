#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	//this->mapWidget =  ui->setupUi(this);
	ui->setupUi(this);
	this->mapWidget = this->ui->mapWidget;
	//��Ȼ�޷����ţ����ڻ���忴����ʲôԭ��
	this->setCentralWidget(this->mapWidget);
	map = new CGeoMap("map1");
	this->setWindowTitle(map->getPropertyValue("map_name").toString());

	//���Ӳ˵���ͬ���庯��
	connect(this->ui->action_geojson_J, SIGNAL(triggered(bool)), this, SLOT(action_geojson_J()));
	connect(this->ui->action_Shape_S, SIGNAL(triggered(bool)), this, SLOT(action_Shape_S()));
	connect(this->ui->action_postgis_P, SIGNAL(triggered(bool)), this, SLOT(action_postgis_P()));

	connect(ods_dialog, SIGNAL(emit_ods_settings(QString, QString)),this,SLOT(receive_ods_data(QString,QString)));
	this->connect(this, SIGNAL(emitMapData(CGeoMap*)), mapWidget, SLOT(receiveMapData(CGeoMap*)));
}

//geojson���Ӳ���
void MainWindow::action_geojson_J()
{
	//map = new CGeoMap("map1");
	//this->setWindowTitle(map->getMapName());

	//filename�����ļ�������·��url
	QString filename = QFileDialog::getOpenFileName(this, tr("�ļ�"), "D:/Wang/JUNIOR/FIRSR_SEMESTER", tr("�����ļ�( * json )"));

	//�����ļ�����
	QFile load_file;
	load_file.setFileName(filename);
	if (!load_file.open(QIODevice::ReadOnly))
	{
		qWarning() << "couldn't open projects json";
		return;
	}

	//����filename����Ϊlayername
	int index = filename.lastIndexOf('/');
	QString layer_name = filename.mid(index + 1);//layer_nameΪ���Ե��ļ���

	//map���layer
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

	//geojson���ݽ���
	docParser parser;
	parser.geoJsonDocParser(json_doc, layer);
	//qDebug() << layer->obj_list.count();
}

//shapefile�ļ��洢����
void MainWindow::action_Shape_S()
{

	QString filename = QFileDialog::getOpenFileName(this, tr("�ļ�"), "D:/Wang/JUNIOR/FIRSR_SEMESTER", tr("�����ļ�( * shp )"));
	//shpfile�ļ�����
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
