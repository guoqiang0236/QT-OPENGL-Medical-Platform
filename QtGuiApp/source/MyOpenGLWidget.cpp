#include "MyOpenGLWidget.h"

MyOpenGLWidget::MyOpenGLWidget(QWidget* parent):QOpenGLWidget(parent)
{
}

void MyOpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();
	
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}

void MyOpenGLWidget::paintGL()
{
	glClearColor(0.2f, 0.3f, 0.32f, 1.0f); //set方法【重点】如果没有 initializeGL，目前是一个空指针状态，没有指向显卡里面的函数，会报错
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); // use方法
}
