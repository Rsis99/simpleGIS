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
	//开始绘制map...
	int width = geo_maxX - geo_minX;
	int height = geo_maxY - geo_minY;
	this->resizeGL(width, height);
	this->x_trans = int(-(geo_minX + geo_maxX) / 2);
	this->y_trans = int(-(geo_minY + geo_maxY) / 2);
	projection.translate(this->x_trans, this->y_trans, this->zoom);
	this->draw_flag = true;
	this->paintGL();
}

void MyOpenGLWidget::initializeGL()
{
	//调用内容初始化函数
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

	// 获取shaderprogram的id号，然后可以通过id号获取一些属性...
	programId = program->programId();
	printf("programId: %d", programId);
	// 从shaderprogram里面获取变量标识
	matrixLocation = glGetUniformLocation(programId, "matrix");
	vertexLocation = glGetAttribLocation(programId, "vPosition");
	clorLocation = glGetAttribLocation(programId, "vColor");
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);

	// 模型矩阵重置
	projection.setToIdentity();
	// 透视投影
	qreal aspect = qreal(w) / qreal(h ? h : 1);
	projection.perspective(90.0f, aspect, 1.0f, 100.0f);
}
//暂时用不到下面两个
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
	//清除之前图形并将背景设置为黑色
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, projection.data());

	for (int m = 0; m < map->getLayerCounts(); m++)
	{
		//获取图层内容
		CGeoLayer* draw_layer = map->getLayerAt(m);
		int obj_length = draw_layer->getObjCounts();
		for (int k = 0; k < obj_length; k++)
		{
			//首先要建立数据vertex以及index,color固定为白色
			GLfloat* vertex = NULL;
			GLuint* index = NULL;
			GLfloat* color = NULL;
			CGeoObject* obj = draw_layer->getObjAt(k);
			QString type = obj->getPropertyValue("type").toString();
			//太长了之后要分块
			if (type == QString("Polygon"))
			{
				QList<CGeoPoint*> temp = static_cast<CGeoPolygon*>(obj)->getPointList();
				int point_num = temp.size();
				//分配空间
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

				//绘制部分
				initVbo(vertex, index, color, point_num);
				// shader传入模型视图矩阵 projection可以理解为建立了一个坐标系空间，可以再这个空间内设置图形
				//transform部分同理

				//调用了之前绑定的缓冲区进行顶点颜色装配等过程
				// shader绑定并启用颜色数组buffer
				glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
				glVertexAttribPointer(clorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(clorLocation);
				// 颜色值rgba，所以每四个float值作为一个颜色值，如果只是希望rgb，取三个值作为颜色值即可!


				// shader绑定并启用顶点数组buffer
				glBindBuffer(GL_ARRAY_BUFFER, verVbo);
				glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(vertexLocation);

				// shader绑定并顶点索引数组buffer - 索引无需启用
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
				glDrawElements(GL_LINE_LOOP, point_num, GL_UNSIGNED_INT, 0);

				// 解绑buffer、关闭启用顶点、颜色数组
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
				//分配空间
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

				//绘制部分
				initVbo(vertex, index, color, point_num);
				// shader传入模型视图矩阵 projection可以理解为建立了一个坐标系空间，可以再这个空间内设置图形
				//transform部分同理

				//调用了之前绑定的缓冲区进行顶点颜色装配等过程
				// shader绑定并启用颜色数组buffer
				glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
				glVertexAttribPointer(clorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(clorLocation);
				// 颜色值rgba，所以每四个float值作为一个颜色值，如果只是希望rgb，取三个值作为颜色值即可!


				// shader绑定并启用顶点数组buffer
				glBindBuffer(GL_ARRAY_BUFFER, verVbo);
				glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(vertexLocation);

				// shader绑定并顶点索引数组buffer - 索引无需启用
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
				glDrawElements(GL_LINE_STRIP, point_num, GL_UNSIGNED_INT, 0);

				// 解绑buffer、关闭启用顶点、颜色数组
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
				//分配空间
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
				

				//绘制部分
				initVbo(vertex, index, color, 1);
				// shader传入模型视图矩阵 projection可以理解为建立了一个坐标系空间，可以再这个空间内设置图形
				//transform部分同理

				//调用了之前绑定的缓冲区进行顶点颜色装配等过程
				// shader绑定并启用颜色数组buffer
				glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
				glVertexAttribPointer(clorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(clorLocation);
				// 颜色值rgba，所以每四个float值作为一个颜色值，如果只是希望rgb，取三个值作为颜色值即可!


				// shader绑定并启用顶点数组buffer
				glBindBuffer(GL_ARRAY_BUFFER, verVbo);
				glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(vertexLocation);

				// shader绑定并顶点索引数组buffer - 索引无需启用
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
				glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);

				// 解绑buffer、关闭启用顶点、颜色数组
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

	// 该部分所有的绑定是告诉缓冲区，空余出这一块的空间，并将这块空间与verVbo等变量进行绑定
	// 初始化顶点buffer并装载数据到显存	
	int vertex_size = size * 2;
	glGenBuffers(1, &verVbo);
	glBindBuffer(GL_ARRAY_BUFFER, verVbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_size * sizeof(GLfloat), vertex_, GL_STATIC_DRAW);

	// 初始化索引buffer并装载数据到显存
	glGenBuffers(1, &indexVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(GLuint), triIndexs_, GL_STATIC_DRAW);


	// 初始化颜色buffer并装载数据到显存
	int color_size = size * 3;
	glGenBuffers(1, &colorVbo);
	glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
	glBufferData(GL_ARRAY_BUFFER, color_size * sizeof(GLfloat), colors_, GL_STATIC_DRAW);
}


//缩放事件
void MyOpenGLWidget::wheelEvent(QWheelEvent* event)
{
	QOpenGLWidget::wheelEvent(event);
	if (draw_flag != 1)
	{
		return;
	}

	float scale_zoom = event->delta();
	//上滑放大
	//|scale_zoom| = 120
	if (scale_zoom > 0)
	{
		//相当于拉进视角
		this->projection.translate(0.0f, 0.0f, 0.5f);
	}
	//下滑缩小
	else
	{
		//相当于拉远视角
		this->projection.translate(0.0f, 0.0f, -0.5f);
	}
	update();
}

//拖拽事件
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
		this->projection.translate(change_point.x() / 10, -change_point.y() / 10, 0.0f);
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
//窗口缩放事件
void MyOpenGLWidget::resizeEvent(QResizeEvent* e)
{

	//update();
}