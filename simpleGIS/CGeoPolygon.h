#pragma once
#include "CGeoObject.h"
#include "CGeoPoint.h"
class CGeoPolygon :
	public CGeoObject
{
public:
	CGeoPolygon();
	~CGeoPolygon();

private:
	QList<CGeoPoint *>polygon_pts;
	QMap<QString, QVariant>properties;

public:
	void add_point(CGeoPoint * pt_add);
	QList<CGeoPoint *> getPointList();

	bool addProperties(QString key, QVariant value);
	QStringList getPropertyKeys();
	QVariant getPropertyValue(QString key);

	void getObjExtent();
};

