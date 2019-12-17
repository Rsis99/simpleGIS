#pragma once
#include "CGeoObject.h"
#include "CGeoPoint.h"
#include <qlist.h>
class CGeoPolyline :
	public CGeoObject
{
public:
	CGeoPolyline();
	~CGeoPolyline();

private:
	QList<CGeoPoint*> line_pts;
	QMap<QString, QVariant>properties;

public:
	void add_point(CGeoPoint * pt_add);
	QList<CGeoPoint*> getPointList();

	bool addProperties(QString key, QVariant value);
	QStringList getPropertyKeys();
	QVariant getPropertyValue(QString keys);

	void getObjExtent();
};

