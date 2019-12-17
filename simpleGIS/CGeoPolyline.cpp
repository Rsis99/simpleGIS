#include "CGeoPolyline.h"



CGeoPolyline::CGeoPolyline()
{
}


CGeoPolyline::~CGeoPolyline()
{
}

void CGeoPolyline::add_point(CGeoPoint * pt_add)
{
	line_pts.append(pt_add);
}

QList<CGeoPoint *> CGeoPolyline::getPointList()
{
	return this->line_pts;
}

bool CGeoPolyline::addProperties(QString key, QVariant value)
{
	this->properties.insert(key, value);
	return true;
}

QStringList CGeoPolyline::getPropertyKeys()
{
	return properties.keys();
}

QVariant CGeoPolyline::getPropertyValue(QString key)
{
	return properties.value(key);
}

void CGeoPolyline::getObjExtent()
{
	obj_minX = obj_maxX = line_pts.at(0)->getLongitude();
	obj_minY = obj_maxY = line_pts.at(0)->getLatitude();

	for (int i = 0; i < line_pts.count(); i++)
	{
		if (line_pts.at(i)->getLongitude() > obj_maxX)    obj_maxX = line_pts.at(i)->getLongitude();
		if (line_pts.at(i)->getLongitude() < obj_minX)    obj_minX = line_pts.at(i)->getLongitude();
		if (line_pts.at(i)->getLatitude() > obj_maxY)    obj_maxY = line_pts.at(i)->getLatitude();
		if (line_pts.at(i)->getLatitude() < obj_minY)    obj_minY = line_pts.at(i)->getLatitude();

	}
}