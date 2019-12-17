#pragma once
#include "CGeoObject.h"
//#include <qgeocoordinate.h>
class CGeoPoint :
	public CGeoObject
{
public:
	CGeoPoint();
	CGeoPoint(double x, double y);
	~CGeoPoint();

private:
	double longitude;
	double latitude;
	QMap<QString, QVariant>properties;

	//QString name;
public:
	void setLatitude(double lat);
	void setLongitude(double lon);
	void setPoint(CGeoPoint pt);
	//CGeoPoint getPoint();
	double getLatitude();
	double getLongitude();

	bool addProperties(QString key, QVariant value);
	QStringList getPropertyKeys();
	QVariant getPropertyValue(QString key);

	void getObjExtent();
	
};

