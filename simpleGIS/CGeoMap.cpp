#include "CGeoMap.h"
#include <gdal_priv.h>



CGeoMap::CGeoMap()
{
}

CGeoMap::CGeoMap(QString map_name)
{
	//小笨蛋这个别设颠倒了
	map_minX = DBL_MAX;
	map_minY = DBL_MAX;
	map_maxX = DBL_MIN;
	map_maxY = DBL_MIN;
	properties.insert("map_name", map_name);
}

CGeoMap::~CGeoMap()
{
	for (int i = 0; i < layer_list.count(); i++)
	{
		delete layer_list[i];
	}
}


void CGeoMap::setMapExtent(double min_x, double min_y, double max_x, double max_y)
{
	this->map_minX = min_x;
	this->map_minY = min_y;
	this->map_maxX = max_x;
	this->map_maxY = max_y;
}

void CGeoMap::getMapExtent(double *min_x, double *min_y, double *max_x, double *max_y)
{
	*min_x = map_minX;
	*min_y = map_minY;
	*max_x = map_maxX;
	*max_y = map_maxY;
}

void CGeoMap::setMapExtent()
{
	for (int i = 0; i < layer_list.count(); i++)
	{
		layer_list.at(i)->getLayerExtent();
		map_maxX = MAX(map_maxX, layer_list.at(i)->layer_maxX);
		map_minX = MIN(map_minX, layer_list.at(i)->layer_minX);
		map_maxY = MAX(map_maxY, layer_list.at(i)->layer_maxY);
		map_minY = MIN(map_minY, layer_list.at(i)->layer_minY);
	}
}

void CGeoMap::addProperties(QString key, QVariant value)
{
	this->properties.insert(key, value);
}

QStringList CGeoMap::getPropertyKeys()
{
	return properties.keys();
}

QVariant CGeoMap::getPropertyValue(QString key)
{
	if (isPropertyContained(key) == true)
	{
		return properties.value(key);
	}
	else return NULL;
	
}

bool CGeoMap::isPropertyContained(QString key)
{
	return properties.contains(key);
}

void CGeoMap::addLayer(CGeoLayer *layer)
{
	layer->addProperties("ID", layer_list.count());
	this->layer_list.append(layer);
	this->properties.insert("layer_ID", layer_list.count());
}

void CGeoMap::deleLayerAt(int idx)
{
	this->layer_list.removeAt(idx);
}

void CGeoMap::deleLayerAll()
{
	this->layer_list.clear();
}

int CGeoMap::getLayerCounts()
{
	return layer_list.count();
}

CGeoLayer * CGeoMap::getLayerAt(int i)
{
	return layer_list.at(i);
}

QList<CGeoLayer *> CGeoMap::getAllLayers()
{
	return this->layer_list;
}
