#include "CGeoPoint.h"



CGeoPoint::CGeoPoint()
{
	this->latitude = 0;
	this->longitude = 0;
	//this->name = "";
}

CGeoPoint::CGeoPoint(double x, double y)
{
	this->longitude = x;
	this->latitude = y;
}

CGeoPoint::~CGeoPoint()
{
}

void CGeoPoint::setLatitude(double lat)
{
	this->latitude = lat;
}

void CGeoPoint::setLongitude(double lon)
{
	this->longitude = lon;
}

double CGeoPoint::getLatitude()
{
	return latitude;
}

double CGeoPoint::getLongitude()
{
	return longitude;
}

void CGeoPoint::setPoint(CGeoPoint pt)
{
	this->latitude = pt.latitude;
	this->longitude = pt.longitude;
}


bool CGeoPoint::addProperties(QString key, QVariant value)
{
	this->properties.insert(key, value);
	return true;
}

QStringList CGeoPoint::getPropertyKeys()
{
	return properties.keys();
}

QVariant CGeoPoint::getPropertyValue(QString key)
{
	if (properties.contains(key))
	{
		return properties.value(key);
	}
	else {
		return NULL;
	}
}

void CGeoPoint::getObjExtent()
{
	obj_minX = this->longitude;
	obj_maxX = this->longitude;
	obj_minY = this->latitude;
	obj_maxY = this->latitude;
}

