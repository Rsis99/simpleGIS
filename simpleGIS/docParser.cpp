#include "docParser.h"
#include <qmessagebox.h>
#include <QtWidgets/qmessagebox.h>

docParser::docParser()
{
}


docParser::~docParser()
{
}

//读取json数据内容
bool docParser::geoJsonDocParser(QJsonDocument json_doc, CGeoLayer *layer)
{
	//QJsonObject root_obj = json_doc.object();
	//QJsonValue root_value = root_obj.value("features");
	QJsonValue root_value = json_doc.object().value("features");
	QJsonArray root_array = root_value.toArray();
	//qDebug() << root_value.isArray() << endl;

	for (int i = 0; i < root_array.size(); i++)
	{
		QJsonValue feature_val = root_array.at(i);
		QJsonObject feature_obj = feature_val.toObject();
		geoJsonFeatureParser(feature_obj, layer);
		//qDebug()<<feature_val.isObject()<<endl<<feature_obj.keys();
	}
	return true;
}

//添加feature
bool docParser::geoJsonFeatureParser(const QJsonObject feature_obj, CGeoLayer * layer)
{
	//deal with geometry and coordinates
	CGeoObject * obj = nullptr;
	QString geojsonTypes[] = { "Point",
						   "Multipoint",
						   "LineString",
						   "MultiLineString",
						   "Polygon",
						   "MultiPolygon",
	};
	enum geo_types { Point, MultiPoint, LineString, MultiLineString, Polygon, MultiPolygon };

	QJsonValue geometry_val = feature_obj.value("geometry");
	QJsonObject geometry_obj = geometry_val.toObject();

	QVariant type_key = geometry_obj.value("type");
	if (type_key == QVariant::Invalid) {
		qWarning() << "Type check failed!";
		return false;
	}
	QString obj_type = type_key.toString();

	int len = sizeof(geojsonTypes) / sizeof(*geojsonTypes);
	int i = 0;
	for (i = 0; i < len; i++)
	{
		if (obj_type == geojsonTypes[i]) {
			qDebug() << "geometry type:" << obj_type << "; type check passed! i=" << i;
			break;
		}
		else if (i == len) {
			qDebug() << "type check failed!";
			return false;
		}
	}

	//根据geometry type选择相应的对象进行存储
	switch (i) {

	case Point:
	{
		obj = new CGeoPoint;
		obj->addProperties("type",i);

		QVariant coords_value = geometry_obj.value("coordinates");
		CGeoPoint pt = coordinatesForPoint(coords_value);
		((CGeoPoint *)obj)->setPoint(pt);

		/*
		QJsonValue property_val = feature_obj.value("properties");
		QString obj_name = property_val.toObject().value("name").toString();
		obj->setObjName(obj_name);
		*/

		layer->addObject(obj);
		//qDebug()<<"Set point successed!";
		break;
	}

	case MultiPoint:
	{
		break;
	}

	case LineString:
	{
		obj = new CGeoPolyline;
		obj->addProperties("type",i);

		QVariant coords_value = geometry_obj.value("coordinates");
		//QJsonArray coords_array = coords_value.toArray();
		QVariantList list2 = coords_value.value<QVariantList>();
		QVariantList::iterator iter;

		for (iter = list2.begin(); iter != list2.end(); ++iter)
		{
			CGeoPoint *point_for_multi = new CGeoPoint;
			*point_for_multi = coordinatesForPoint(*iter);
			((CGeoPolyline *)obj)->add_point(point_for_multi);
			//qDebug()<<"Succeed in adding point in POLYLINE!";
		}

		layer->addObject(obj);
		break;
	}

	case MultiLineString:
	{
		break;
	}

	case Polygon:
	{
		obj = new CGeoPolygon;
		obj->addProperties("type",i);

		QJsonArray coords_array = geometry_obj.value("coordinates").toArray();
		QVariant coords_value = coords_array.at(0);
		QVariantList list3 = coords_value.value<QVariantList>();


		QVariantList::iterator iter;

		for (iter = list3.begin(); iter != list3.end(); ++iter)
		{
			CGeoPoint* point_for_multi = new CGeoPoint;
			*point_for_multi = coordinatesForPoint(*iter);
			((CGeoPolygon *)obj)->add_point(point_for_multi);
			//qDebug()<<"Succeed in adding point in POLYGON!";
		}

		layer->addObject(obj);
		break;
	}

	case MultiPolygon:
	{
		break;
	}

	default:
	{
		qWarning() << obj_type << "It's not a valid value!";
	}

	}
	qDebug() << "Succeed in adding point in POLYGON!";
	return true;
}

//读取坐标系
CGeoPoint docParser::coordinatesForPoint(QVariant coords_value)
{
	CGeoPoint coord ;
	QVariantList list = coords_value.value<QVariantList>();
	QVariantList::iterator iter;

	for (iter = list.begin(); iter != list.end(); ++iter)
	{
		QString element_type = iter->typeName();

		switch (list.indexOf(*iter)) {
		case 0:
			coord.setLatitude(iter->toDouble());
			break;
		case 1:
			coord.setLongitude(iter->toDouble());
			break;
		}
	}
	return coord;
}


bool docParser::shpDocParser(QString filename, CGeoMap * map)
{ 
	GDALAllRegister();
	GDALDataset *poDS;
	CPLSetConfigOption("SHAPE_ENCODING", "");//解决中文乱码问题
	
	//读取shp文件
	const char * filename_str = filename.toStdString().c_str();
	poDS = (GDALDataset*)GDALOpenEx(filename.toUtf8(), GDAL_OF_VECTOR, NULL, NULL, NULL);

	if (poDS == NULL)
	{
		qWarning() << "Open failed!";
		return false;
	}

	OGRLayer *poLayer;
	poLayer = poDS->GetLayer(0);
	//poLayer = poDS->GetLayerByName("point");

	//layer属性设置
	int index = filename.lastIndexOf('/');
	QString layer_name = filename.mid(index + 1);
	CGeoLayer *layer = new CGeoLayer(layer_name);//layer_name
	//layer->setLayerName(QString (poLayer->GetName()));
	
	OGREnvelope *poEnvelope = new OGREnvelope;
	poLayer->GetExtent(poEnvelope);
	layer->setLayerExtent(poEnvelope->MinX, poEnvelope->MinY, poEnvelope->MaxX, poEnvelope->MaxY);
	delete poEnvelope;

	map->addLayer(layer);

	OGRFeature *poFeature;
	OGRFeatureDefn *poFDefn;
	OGRGeometry *poGeometry;

	poFDefn = poLayer->GetLayerDefn();
	poLayer->ResetReading();
	while ((poFeature = poLayer->GetNextFeature()) != NULL)
	{
		poGeometry = poFeature->GetGeometryRef();
		if (poGeometry == NULL) {
			qWarning() << "File cannot open! Lack Geometry Type!";
			return false;
		}
		else
		{
			OGRwkbGeometryType pGeoType = poGeometry->getGeometryType();
			if (pGeoType == wkbPoint)
			{
				OGRPoint *poPoint = static_cast<OGRPoint*>(poGeometry);
				CGeoObject *obj = new CGeoPoint(poPoint->getX(), poPoint->getY());
				this->shpPorpertyParser(poFDefn, poFeature, obj);
				obj->addProperties("type", "Point");
				layer->addObject(obj);
				qDebug() << "Success in ADDing POINT!";
			}
			else if (pGeoType == wkbLineString)
			{
				OGRLineString *poLineString = static_cast<OGRLineString*>(poGeometry);
				CGeoObject *obj = new CGeoPolyline;
				for (int ipoint = 0; ipoint<poLineString->getNumPoints(); ipoint++)
				{
					CGeoPoint *line_pt = new CGeoPoint(poLineString->getX(ipoint), poLineString->getY(ipoint));
					((CGeoPolyline *)obj)->add_point(line_pt);
				}
				this->shpPorpertyParser(poFDefn, poFeature, obj);
				obj->addProperties("type", "LineString");
				layer->addObject(obj);
				qDebug() << "Success in ADDing LINESTRING!";
			}
			else if (pGeoType == wkbPolygon)
			{
				OGRPolygon * poPolygon = static_cast<OGRPolygon*>(poGeometry);
				CGeoObject *obj = new CGeoPolygon;
				OGRLinearRing * poLinearRing = poPolygon->getExteriorRing();
				for (int ipoint = 0; ipoint < poLinearRing->getNumPoints(); ipoint++)
				{
					CGeoPoint *polygon_pt = new CGeoPoint(poLinearRing->getX(ipoint), poLinearRing->getY(ipoint));
					((CGeoPolygon *)obj)->add_point(polygon_pt);
				}
				this->shpPorpertyParser(poFDefn, poFeature, obj);
				obj->addProperties("type", "Polygon");
				layer->addObject(obj);
				qDebug() << "Success in ADDing POLYGON!";
			}
			else {
				qWarning() << "SORRY!功能尚在开发~";
			}
		}
		OGRFeature::DestroyFeature(poFeature);
	}
	GDALClose(poDS);
	OGRCleanupAll();
	return true;
}

bool docParser::shpPorpertyParser(OGRFeatureDefn *poFDefn, OGRFeature *poFeature, CGeoObject *obj)
{
	for (int iField = 0; iField < poFDefn->GetFieldCount(); iField++)
	{
		OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(iField);
		OGRFieldType field_type = poFieldDefn->GetType();
		if (field_type == OFTInteger)
		{
			obj->addProperties(poFieldDefn->GetNameRef(), poFeature->GetFieldAsInteger(iField));
		}
		else if (field_type == OFTReal)
		{
			obj->addProperties(poFieldDefn->GetNameRef(), poFeature->GetFieldAsDouble(iField));
		}
		else if (field_type == OFTString)
		{
			obj->addProperties(poFieldDefn->GetNameRef(), poFeature->GetFieldAsString(iField));
		}
		else
		{
			obj->addProperties(poFieldDefn->GetNameRef(), poFeature->GetFieldAsString(iField));
		}

	}
	return true;
}

bool docParser::odsDocParser(QString ods_path, QString layer_name, CGeoMap* map)
{
	std::string s_obs_path = ods_path.toStdString();
	const char* c_obs_path = s_obs_path.c_str();
	std::string s_layer_name = layer_name.toStdString();
	const char* c_layer_name = s_layer_name.c_str();
	//连接数据库部分
	GDALAllRegister();
	GDALDataset* p_ods = NULL;
	p_ods = (GDALDataset*)GDALOpenEx(c_obs_path, GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
	if (p_ods == NULL)
	{
		//QMessageBox::critical(this, "error", QString(QStringLiteral("数据库未连接")));
		return false;
	}
	OGRLayer* poLayer;
	poLayer = p_ods->GetLayerByName(c_layer_name);
	//poLayer = poDS->GetLayerByName("point");

	//layer属性设置
	CGeoLayer* layer = new CGeoLayer(layer_name);//layer_name
	//layer->setLayerName(QString (poLayer->GetName()));

	OGREnvelope* poEnvelope = new OGREnvelope;
	poLayer->GetExtent(poEnvelope);
	layer->setLayerExtent(poEnvelope->MinX, poEnvelope->MinY, poEnvelope->MaxX, poEnvelope->MaxY);
	delete poEnvelope;

	map->addLayer(layer);

	OGRFeature* poFeature;
	OGRFeatureDefn* poFDefn;
	OGRGeometry* poGeometry;

	poFDefn = poLayer->GetLayerDefn();
	poLayer->ResetReading();
	while ((poFeature = poLayer->GetNextFeature()) != NULL)
	{
		poGeometry = poFeature->GetGeometryRef();
		if (poGeometry == NULL) {
			qWarning() << "File cannot open! Lack Geometry Type!";
			return false;
		}
		else
		{
			OGRwkbGeometryType pGeoType = poGeometry->getGeometryType();
			if (pGeoType == wkbPoint)
			{
				OGRPoint* poPoint = static_cast<OGRPoint*>(poGeometry);
				CGeoObject* obj = new CGeoPoint(poPoint->getX(), poPoint->getY());
				this->shpPorpertyParser(poFDefn, poFeature, obj);
				obj->addProperties("type", "Point");
				layer->addObject(obj);
				qDebug() << "Success in ADDing POINT!";
			}
			else if (pGeoType == wkbLineString)
			{
				OGRLineString* poLineString = static_cast<OGRLineString*>(poGeometry);
				CGeoObject* obj = new CGeoPolyline;
				for (int ipoint = 0; ipoint < poLineString->getNumPoints(); ipoint++)
				{
					CGeoPoint* line_pt = new CGeoPoint(poLineString->getX(ipoint), poLineString->getY(ipoint));
					((CGeoPolyline*)obj)->add_point(line_pt);
				}
				this->shpPorpertyParser(poFDefn, poFeature, obj);
				obj->addProperties("type", "LineString");
				layer->addObject(obj);
				qDebug() << "Success in ADDing LINESTRING!";
			}
			else if (pGeoType == wkbPolygon)
			{
				OGRPolygon* poPolygon = static_cast<OGRPolygon*>(poGeometry);
				CGeoObject* obj = new CGeoPolygon;
				OGRLinearRing* poLinearRing = poPolygon->getExteriorRing();
				for (int ipoint = 0; ipoint < poLinearRing->getNumPoints(); ipoint++)
				{
					CGeoPoint* polygon_pt = new CGeoPoint(poLinearRing->getX(ipoint), poLinearRing->getY(ipoint));
					((CGeoPolygon*)obj)->add_point(polygon_pt);
				}
				this->shpPorpertyParser(poFDefn, poFeature, obj);
				obj->addProperties("type", "Polygon");
				layer->addObject(obj);
				qDebug() << "Success in ADDing POLYGON!";
			}
			else if (pGeoType == wkbMultiPolygon)
			{
				OGRMultiPolygon* poMultiPolygon = (OGRMultiPolygon*)poGeometry;
				OGRPolygon* poPolygon = NULL;
				int polygonNum = poMultiPolygon->getNumGeometries();
				for (int i = 0; i < polygonNum; i++)
				{
					CGeoObject* obj = new CGeoPolygon;
					poPolygon = (OGRPolygon*)poMultiPolygon->getGeometryRef(i);
					OGRLinearRing* poLinerRing = poPolygon->getExteriorRing();
					for (int ipoint = 0; ipoint < poLinerRing->getNumPoints(); ipoint++)
					{
						CGeoPoint* polygon_pt = new CGeoPoint(poLinerRing->getX(ipoint), poLinerRing->getY(ipoint));
						((CGeoPolygon*)obj)->add_point(polygon_pt);
					}
					this->shpPorpertyParser(poFDefn, poFeature, obj);
					obj->addProperties("type", "Polygon");
					layer->addObject(obj);
					qDebug() << "Success in ADDing POLYGON!";
				}
			}
			else {
				qWarning() << "SORRY!功能尚在开发~";
			}
		}
		OGRFeature::DestroyFeature(poFeature);
	}
	GDALClose(p_ods);
	OGRCleanupAll();
	return true;
}
