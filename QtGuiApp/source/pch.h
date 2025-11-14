#ifndef PCH_H
#define PCH_H

// 添加常用的头文件
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

// Qt 相关头文件
#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QIcon>
#include <QSlider>
#include <QPushButton>
#include <QComboBox>
#include <QQuickWidget>
#include <QDateTime>
#include <QTimer>
#include <QThreadPool>
#include <QTcpSocket>
#include <QMessageBox>

//自定义Qt类
#include "MyThread.h"
#include "MyThread_Work.h"
#include "MyThread_QRunnable.h"
#include "MyProgressDialog.h"

//opengl
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>

//glm
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
//全局变量
#include "GlobalConfig.h"

#endif // PCH_H