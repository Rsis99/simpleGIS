#pragma once
#include "CGeoLayer.h"

class CGeoMap
{
public:
	CGeoMap();
	CGeoMap(QString map_name);
	~CGeoMap();
private:
	double map_minX, map_minY, map_maxX, map_maxY;
	QList<CGeoLayer *>layer_list;
	QMap<QString, QVariant> properties;

public:

	void setMapExtent(double min_x, double min_y, double max_x, double max_y);
	void getMapExtent(double *min_x, double *min_y, double *max_x, double *max_y);
	void setMapExtent();

	bool isPropertyContained(QString key);
	void addProperties(QString key, QVariant value);
	QStringList getPropertyKeys();
	QVariant getPropertyValue(QString key);

	void addLayer(CGeoLayer * layer);
	void deleLayerAt(int idx);
	void deleLayerAll();

	int getLayerCounts();
	CGeoLayer * getLayerAt(int i);
	QList<CGeoLayer *> getAllLayers();


};

