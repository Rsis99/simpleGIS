#include "CGeoLayer.h"



CGeoLayer::CGeoLayer()
{
	for (int i = 0; i < obj_list.count(); i++)
	{
		delete obj_list[i];
	}
}

CGeoLayer::CGeoLayer(QString layer_name)
{
	layer_minX = DBL_MIN;
	layer_minY = DBL_MIN;
	layer_maxX = DBL_MAX;
	layer_maxY = DBL_MAX;
	properties.insert("layer_name", layer_name);
}

CGeoLayer::~CGeoLayer()
{
	for (int i = 0; i < obj_list.count(); i++)
	{
		delete obj_list[i];
	}
}

void CGeoLayer::setLayerExtent(double minX, double minY, double maxX, double maxY)
{
	this->layer_minX = minX;
	this->layer_minY = minY;
	this->layer_maxX = maxX;
	this->layer_maxY = maxY;
}

void CGeoLayer::getLayerExtent(double *minX, double *minY, double *maxX, double *maxY)
{
	if (layer_maxX == DBL_MAX || layer_maxY == DBL_MAX || layer_minX == DBL_MIN || layer_minY == DBL_MIN)
	{
		this->getLayerExtent();
	}
	else
	{
		*minX = layer_minX;
		*minY = layer_minY;
		*maxX = layer_maxX;
		*maxY = layer_maxY;
	}
}

void CGeoLayer::getLayerExtent()
{
	if (layer_maxX != DBL_MAX && layer_maxY != DBL_MAX && layer_minX != DBL_MIN && layer_minY != DBL_MIN) return;

	for (int i = 0; i < obj_list.count(); i++)
	{
		obj_list.at(i)->getObjExtent();
		if (layer_minX > obj_list.at(i)->obj_minX) layer_minX = obj_list.at(i)->obj_minX;
		if (layer_maxX < obj_list.at(i)->obj_maxX) layer_maxX = obj_list.at(i)->obj_maxX;
		if (layer_minY > obj_list.at(i)->obj_minY) layer_minY = obj_list.at(i)->obj_minY;
		if (layer_maxY < obj_list.at(i)->obj_maxY) layer_maxY = obj_list.at(i)->obj_maxY;

	}
}

bool CGeoLayer::isPropertyContained(QString key)
{
	if (this->properties.contains(key)) return true;
	else return false;
}

void CGeoLayer::addProperties(QString key, QVariant value)
{
	this->properties.insert(key, value);
}

QStringList CGeoLayer::getPropertyKeys()
{
	return this->properties.keys();

}

QVariant CGeoLayer::getPropertyValue(QString key)
{
	if (this->properties.contains(key))
		return this->properties.value(key);
	else return NULL;
}

void CGeoLayer::addObject(CGeoObject *obj)
{
	obj->addProperties("ID", this->obj_list.count());
	this->obj_list.append(obj);
	this->properties.insert("obj_ID", obj_list.count());
}

void CGeoLayer::deleObjectAt(int idx)
{
	if (idx < 0 || idx >= obj_list.count()) return;
	else {
		this->obj_list.removeAt(idx);
	}
}

void CGeoLayer::deleObjectAll()
{
	this->obj_list.clear();
}

int CGeoLayer::getObjCounts()
{
	return obj_list.count();
}

CGeoObject * CGeoLayer::getObjAt(int i)
{
	return obj_list.at(i);
}

QList<CGeoObject *> CGeoLayer::getObjAll()
{
	return obj_list;
}