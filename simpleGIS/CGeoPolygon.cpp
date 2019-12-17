#include "CGeoPolygon.h"



CGeoPolygon::CGeoPolygon()
{
}


CGeoPolygon::~CGeoPolygon()
{
}

void CGeoPolygon::add_point(CGeoPoint * pt_add)
{
	this->polygon_pts.append(pt_add);
}

QList<CGeoPoint *> CGeoPolygon::getPointList()
{
	return this->polygon_pts;
}

bool CGeoPolygon::addProperties(QString key, QVariant value)
{
	this->properties.insert(key, value);
	return true;
}

QStringList CGeoPolygon::getPropertyKeys()
{
	return properties.keys();
}

QVariant CGeoPolygon::getPropertyValue(QString key)
{
	if (properties.contains(key))
	{
		return properties.value(key);
	}
	else return NULL;
}

void CGeoPolygon::getObjExtent()
{
	obj_minX = obj_maxX = polygon_pts.at(0)->getLongitude();
	obj_minY = obj_maxY = polygon_pts.at(0)->getLatitude();

	for (int i = 0; i < polygon_pts.count(); i++)
	{
		if (polygon_pts.at(i)->getLongitude() > obj_maxX)    obj_maxX = polygon_pts.at(i)->getLongitude();
		if (polygon_pts.at(i)->getLongitude() < obj_minX)    obj_minX = polygon_pts.at(i)->getLongitude();
		if (polygon_pts.at(i)->getLatitude() > obj_maxY)    obj_maxY = polygon_pts.at(i)->getLatitude();
		if (polygon_pts.at(i)->getLatitude() < obj_minY)    obj_minY = polygon_pts.at(i)->getLatitude();

	}
}