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

//GLM]

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
//全局变量
#include "GlobalConfig.h"

#endif // PCH_H