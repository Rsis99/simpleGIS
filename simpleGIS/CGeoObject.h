#pragma once
#include <qwidget.h>
#include <qmap.h>
#include <qvariant.h>
class CGeoObject
{
public:
	CGeoObject();
	~CGeoObject();

	double obj_minX, obj_minY, obj_maxX, obj_maxY;

public:

	virtual bool addProperties(QString key, QVariant value) = 0;
	virtual QStringList getPropertyKeys() = 0;
	virtual QVariant getPropertyValue(QString key) = 0;

	virtual void getObjExtent() = 0;

};

