#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_5_Core>
class MyOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
	Q_OBJECT
public:
	explicit MyOpenGLWidget(QWidget* parent = nullptr);

protected:
	virtual void initializeGL() override;
	virtual void resizeGL(int w, int h) override;
	virtual void paintGL() override;

};