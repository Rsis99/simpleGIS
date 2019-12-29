#include "MyOpenGLWidget.h"

MyOpenGLWidget::MyOpenGLWidget(QWidget* parent) : QOpenGLWidget(parent)
{
	//map = new CGeoMap("MyMap");
	
}

void MyOpenGLWidget::receiveMapData(CGeoMap* map)
{
	this->map = map;
	map->setMapExtent();
	map->getMapExtent(&geo_minX, &geo_minY, &geo_maxX, &geo_maxY);
	//��ʼ����map...
	int width = geo_maxX - geo_minX;
	int height = geo_maxY - geo_minY;
	this->width = this->size().width();
	this->height = this->size().height();
	this->resizeGL(width, height);
	//this->x_trans = int(-(geo_minX + geo_maxX) / 2);
	//this->y_trans = int(-(geo_minY + geo_maxY) / 2);
	//projection.translate(this->x_trans, this->y_trans, this->zoom);
	this->draw_flag = true;
	this->paintGL();
}

void MyOpenGLWidget::initializeGL()
{
	//�������ݳ�ʼ������
	initializeOpenGLFunctions();
	this->glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	program = new QOpenGLShaderProgram(this);

	if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, "./shader/vertexshader.vert")) {
		printf("no found vertexshader.vert");
		return;
	}
	if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, "./shader/fragmentshader.frag")) {
		printf("no found fragmentshader.frag");
		return;
	}
	if (!program->link()) {
		printf("no link");
		return;
	}
	if (!program->bind()) {
		printf("no bind");
		return;
	}

	// ��ȡshaderprogram��id�ţ�Ȼ�����ͨ��id�Ż�ȡһЩ����...
	programId = program->programId();
	printf("programId: %d", programId);
	// ��shaderprogram�����ȡ������ʶ
	matrixLocation = glGetUniformLocation(programId, "matrix");
	vertexLocation = glGetAttribLocation(programId, "vPosition");
	clorLocation = glGetAttribLocation(programId, "vColor");
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);

	// ģ�;�������
	projection.setToIdentity();
	// ͸��ͶӰ
	//qreal aspect = qreal(w) / qreal(h ? h : 1);
	//projection.perspective(90.0f, aspect, 1.0f, 100.0f);
	//����ͶӰ
	projection.ortho(geo_minX, geo_maxX, geo_minY, geo_maxY, -100, 100);
	//����ͶӰ��غ�������
	//projection.ortho(int left, int right, int bottom, int top, int far, int near)
	//��ʼ�������൱�ڸ�������һ������������崰�ڣ�ֻ��������֮�ڵĲ��ֲ�����ʾ
}
//��ʱ�ò�����������
void MyOpenGLWidget::drawMap(CGeoMap* map)
{
	for (int i = 0; i < map->getLayerCounts(); i++)
	{
		this->drawLayer(map->getLayerAt(i));
	}
}

void MyOpenGLWidget::drawLayer(CGeoLayer* layer)
{
	
}

void MyOpenGLWidget::paintGL()
{
	if (this->draw_flag == false)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		return;
	}
	//���֮ǰͼ�β�����������Ϊ��ɫ
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, projection.data());

	for (int m = 0; m < map->getLayerCounts(); m++)
	{
		//��ȡͼ������
		CGeoLayer* draw_layer = map->getLayerAt(m);
		int obj_length = draw_layer->getObjCounts();
		for (int k = 0; k < obj_length; k++)
		{
			//����Ҫ��������vertex�Լ�index,color�̶�Ϊ��ɫ
			GLfloat* vertex = NULL;
			GLuint* index = NULL;
			GLfloat* color = NULL;
			CGeoObject* obj = draw_layer->getObjAt(k);
			QString type = obj->getPropertyValue("type").toString();
			//̫����֮��Ҫ�ֿ�
			if (type == QString("Polygon"))
			{
				QList<CGeoPoint*> temp = static_cast<CGeoPolygon*>(obj)->getPointList();
				int point_num = temp.size();
				//����ռ�
				int vertex_size = point_num * 2;
				vertex = new GLfloat[vertex_size];
				index = new GLuint[point_num];
				int color_size = point_num * 3;
				color = new GLfloat[color_size];
				for (int j = 0; j < point_num; j++)
				{

					GLfloat x = (GLfloat)static_cast<CGeoPoint*>(temp.at(j))->getLongitude();
					GLfloat y = (GLfloat)static_cast<CGeoPoint*>(temp.at(j))->getLatitude();
					vertex[j * 2] = x;
					vertex[j * 2 + 1] = y;
					index[j] = j;
					color[j * 3] = 0.0;
					color[j * 3 + 1] = 0.0;
					color[j * 3 + 2] = 0.0;
				}

				//���Ʋ���
				initVbo(vertex, index, color, point_num);
				// shader����ģ����ͼ���� projection�������Ϊ������һ������ϵ�ռ䣬����������ռ�������ͼ��
				//transform����ͬ��

				//������֮ǰ�󶨵Ļ��������ж�����ɫװ��ȹ���
				// shader�󶨲�������ɫ����buffer
				glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
				glVertexAttribPointer(clorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(clorLocation);
				// ��ɫֵrgba������ÿ�ĸ�floatֵ��Ϊһ����ɫֵ�����ֻ��ϣ��rgb��ȡ����ֵ��Ϊ��ɫֵ����!


				// shader�󶨲����ö�������buffer
				glBindBuffer(GL_ARRAY_BUFFER, verVbo);
				glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(vertexLocation);

				// shader�󶨲�������������buffer - ������������
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
				glDrawElements(GL_LINE_LOOP, point_num, GL_UNSIGNED_INT, 0);

				// ���buffer���ر����ö��㡢��ɫ����
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glDisableVertexAttribArray(vertexLocation);
				glDisableVertexAttribArray(clorLocation);
				delete[]vertex;
				delete[]color;
				delete[]index;
			}
			if (type == QString("LineString"))
			{
				QList<CGeoPoint*> temp = static_cast<CGeoPolyline*>(obj)->getPointList();
				int point_num = temp.size();
				//����ռ�
				int vertex_size = point_num * 2;
				vertex = new GLfloat[vertex_size];
				index = new GLuint[point_num];
				int color_size = point_num * 3;
				color = new GLfloat[color_size];
				for (int j = 0; j < point_num; j++)
				{

					GLfloat x = (GLfloat)static_cast<CGeoPoint*>(temp.at(j))->getLongitude();
					GLfloat y = (GLfloat)static_cast<CGeoPoint*>(temp.at(j))->getLatitude();
					vertex[j * 2] = x;
					vertex[j * 2 + 1] = y;
					index[j] = j;
					color[j * 3] = 1.0;
					color[j * 3 + 1] = 1.0;
					color[j * 3 + 2] = 1.0;
				}

				//���Ʋ���
				initVbo(vertex, index, color, point_num);
				// shader����ģ����ͼ���� projection�������Ϊ������һ������ϵ�ռ䣬����������ռ�������ͼ��
				//transform����ͬ��

				//������֮ǰ�󶨵Ļ��������ж�����ɫװ��ȹ���
				// shader�󶨲�������ɫ����buffer
				glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
				glVertexAttribPointer(clorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(clorLocation);
				// ��ɫֵrgba������ÿ�ĸ�floatֵ��Ϊһ����ɫֵ�����ֻ��ϣ��rgb��ȡ����ֵ��Ϊ��ɫֵ����!


				// shader�󶨲����ö�������buffer
				glBindBuffer(GL_ARRAY_BUFFER, verVbo);
				glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(vertexLocation);

				// shader�󶨲�������������buffer - ������������
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
				glDrawElements(GL_LINE_STRIP, point_num, GL_UNSIGNED_INT, 0);

				// ���buffer���ر����ö��㡢��ɫ����
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glDisableVertexAttribArray(vertexLocation);
				glDisableVertexAttribArray(clorLocation);
				delete[]vertex;
				delete[]color;
				delete[]index;
			}
			if (type == QString("Point"))
			{
				int point_num = 1;
				//����ռ�
				int vertex_size = point_num * 2;
				vertex = new GLfloat[vertex_size];
				index = new GLuint[point_num];
				int color_size = point_num * 3;
				color = new GLfloat[color_size];
				GLfloat x = (GLfloat)((CGeoPoint*)obj)->getLongitude();
				GLfloat y = (GLfloat)((CGeoPoint*)obj)->getLatitude();
				vertex[0] = x;
				vertex[1] = y;
				index[0] = 0;
				color[0] = 1.0;
				color[1] = 1.0;
				color[2] = 1.0;
				

				//���Ʋ���
				initVbo(vertex, index, color, 1);
				// shader����ģ����ͼ���� projection�������Ϊ������һ������ϵ�ռ䣬����������ռ�������ͼ��
				//transform����ͬ��

				//������֮ǰ�󶨵Ļ��������ж�����ɫװ��ȹ���
				// shader�󶨲�������ɫ����buffer
				glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
				glVertexAttribPointer(clorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(clorLocation);
				// ��ɫֵrgba������ÿ�ĸ�floatֵ��Ϊһ����ɫֵ�����ֻ��ϣ��rgb��ȡ����ֵ��Ϊ��ɫֵ����!


				// shader�󶨲����ö�������buffer
				glBindBuffer(GL_ARRAY_BUFFER, verVbo);
				glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(vertexLocation);

				// shader�󶨲�������������buffer - ������������
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
				glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);

				// ���buffer���ر����ö��㡢��ɫ����
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glDisableVertexAttribArray(vertexLocation);
				glDisableVertexAttribArray(clorLocation);
				delete[]vertex;
				delete[]color;
				delete[]index;
			}
		}
	}
}

void MyOpenGLWidget::initVbo(GLfloat* vertex_, GLuint* triIndexs_, GLfloat* colors_, int size)
{

	// �ò������еİ��Ǹ��߻��������������һ��Ŀռ䣬�������ռ���verVbo�ȱ������а�
	// ��ʼ������buffer��װ�����ݵ��Դ�	
	int vertex_size = size * 2;
	glGenBuffers(1, &verVbo);
	glBindBuffer(GL_ARRAY_BUFFER, verVbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_size * sizeof(GLfloat), vertex_, GL_STATIC_DRAW);

	// ��ʼ������buffer��װ�����ݵ��Դ�
	glGenBuffers(1, &indexVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(GLuint), triIndexs_, GL_STATIC_DRAW);


	// ��ʼ����ɫbuffer��װ�����ݵ��Դ�
	int color_size = size * 3;
	glGenBuffers(1, &colorVbo);
	glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
	glBufferData(GL_ARRAY_BUFFER, color_size * sizeof(GLfloat), colors_, GL_STATIC_DRAW);
}


//�����¼�
void MyOpenGLWidget::wheelEvent(QWheelEvent* event)
{
	QOpenGLWidget::wheelEvent(event);
	if (draw_flag != 1)
	{
		return;
	}

	float scale_zoom = event->delta();
	//�ϻ��Ŵ�
	//|scale_zoom| = 120
	if (scale_zoom > 0)
	{
		//�൱�������ӽ�
		//this->projection.translate(0.0f, 0.0f, 0.5f);
		//�൱����С�ӿڣ��ﵽ�Ŵ��Ч��
		this->geo_maxX -= 1;
		this->geo_maxY -= 1;
		this->geo_minX += 1;
		this->geo_minY += 1;
		this->resizeGL(width, height);
	}
	//�»���С
	else
	{
		//�൱����Զ�ӽ�
		//this->projection.translate(0.0f, 0.0f, -0.5f);
		this->geo_maxX += 1;
		this->geo_maxY += 1;
		this->geo_minX -= 1;
		this->geo_minY -= 1;
		this->resizeGL(width, height);
	}
	update();
}

//��ק�¼�
void MyOpenGLWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		this->mouse_drag = true;
		this->start_point = event->globalPos();

	}
}
void MyOpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (this->mouse_drag)
	{
		this->end_point = event->globalPos();
		QPointF change_point = this->end_point - this->start_point;
		//this->projection.translate(change_point.x() / 10, -change_point.y() / 10, 0.0f);
		this->geo_maxX -= change_point.x() / 10;
		this->geo_minX -= change_point.x() / 10;
		this->geo_minY += change_point.y() / 10;
		this->geo_maxY += change_point.y() / 10;
		this->resizeGL(width, height);
		update();
		this->start_point = this->end_point;
	}
}
void MyOpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		this->mouse_drag = false;
	}
}
//���������¼�
void MyOpenGLWidget::resizeEvent(QResizeEvent* e)
{
	if (this->draw_flag == true)
	{
		//�߽�ķ�ΧҲ��Ҫ�ı�
		float x_scale = (float)e->size().width() / (float)this->width;
		float y_scale = (float)e->size().height() / (float)this->height;
		this->width = e->size().width();
		this->height = e->size().height();
		//��Χ�������
		float rect_W = geo_maxX - geo_minX;
		float rect_H = geo_maxY - geo_minY;
		//this->geo_maxX *= x_scale;
		this->geo_maxX = this->geo_minX + rect_W * x_scale;
		//this->geo_minX *= x_scale ;
		//this->geo_minY *= y_scale;
		this->geo_maxY = this->geo_minY + rect_H * y_scale;
		this->resizeGL(width, height);
		update();
	}
	//update();
}