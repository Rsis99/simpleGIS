#pragma once
#include <qfile.h>
#include <qfiledialog.h>
#include <qdebug.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
//#include <qgeocoordinate.h>

#include <gdal.h>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <iostream>
//#include "MainWindow.h"
#include "CGeoMap.h"
#include "CGeoPoint.h"
#include "CGeoPolyline.h"
#include "CGeoPolygon.h"
#include  "CGeoObject.h"
#include <qmessagebox.h>

class docParser :
	public QObject
{
public:
	docParser();
	~docParser();

	bool geoJsonDocParser(QJsonDocument json_doc, CGeoLayer * layer);
	bool geoJsonFeatureParser(const QJsonObject feature_obj, CGeoLayer * layer);
	CGeoPoint coordinatesForPoint(QVariant coords_value);

	bool shpDocParser(QString filename, CGeoMap * map);
	bool shpPorpertyParser(OGRFeatureDefn *poFDefn, OGRFeature *poFeature, CGeoObject *obj);
	bool odsDocParser(QString filename, QString layer_name, CGeoMap* map);
	//TODO:关于shp文件集以及文件路径的思考
};

