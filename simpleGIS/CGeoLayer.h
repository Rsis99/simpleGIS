#pragma once
#include "CGeoObject.h"

class CGeoLayer
{
public:
	CGeoLayer();
	CGeoLayer(QString layer_name);
	~CGeoLayer();

//private:
	double layer_minX;
	double layer_minY;
	double layer_maxX;
	double layer_maxY;

public:

	void setLayerExtent(double minX, double minY, double maxX, double maxY);
	void getLayerExtent(double *minX, double *minY, double *maxX, double *maxY);
	void getLayerExtent();

	bool isPropertyContained(QString key);
	void addProperties(QString key, QVariant value);
	QStringList getPropertyKeys();
	QVariant getPropertyValue(QString key);

	void addObject(CGeoObject *obj);
	void deleObjectAt(int idx);
	void deleObjectAll();
	int getObjCounts();
	CGeoObject * getObjAt(int i);
	QList<CGeoObject*> getObjAll();

private:
	QList<CGeoObject *> obj_list;
	QMap<QString, QVariant> properties;
};

