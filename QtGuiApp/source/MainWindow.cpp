#include "MainWindow.h"
// 移除不存在的头文件
#include <QDebug>
#include <QIcon>
#include <QFileDialog>
#include <memory>
#include "MainWindow-UI.h"
#include <thread>
#include <iostream>

#include "MySocketServerDlg.h"
#include "MySocketClient.h"
#include <QNetworkProxy> 
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    m_ui(std::make_unique<Ui::MainWindow_UI>()),
    m_thread(new MyThread(this)),
    m_thread_work(new MyThread_Work(this)),
	m_thread_runnable(new MyThread_Runnable(this)),
    m_DCMScpsub(new QThread(this)),
    m_numsub(new QThread(this))
{
   
    m_socketServer = new MySocketServer(this);
	m_socketClient = new MySocketClient(this);
	m_tcpSocket = new QTcpSocket(this);
    //setWindowFlags(Qt::FramelessWindowHint);
   
    m_ui->setupUi(this);
    UpdateGUI();
    InitSlots();
    UpdateSize();
    
    InitThread();
}

MainWindow::~MainWindow() 
{
    if (m_DCMScpsub)
    {
        if (m_DCMScpsub->isRunning())
        {
            m_DCMScpsub->quit();
            m_DCMScpsub->wait();
        }
    }
    if (m_numsub)
    {
        if (m_numsub->isRunning())
        {
            m_numsub->quit();
            m_numsub->wait();
        }
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
		QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty())
        {
            QString filePath = urls.first().toLocalFile();
            QFileInfo fileInfo(filePath);
            QString suffix = fileInfo.suffix().toLower();
            // 检查文件类型是否符合要求
            if (suffix == "png" || suffix == "jpg" || suffix == "bmp" || suffix == "dcm")
            {
                event->acceptProposedAction(); 
                return;
			}
        }
    }

    event->ignore();
}

void MainWindow::dropEvent(QDropEvent* event)
{

    if (event->mimeData()->hasUrls())
    {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty())
        {
            QString filePath = urls.first().toLocalFile();
            QFileInfo fileInfo(filePath);

            // 验证文件格式
            QString suffix = fileInfo.suffix().toLower();
            if (suffix == "png" || suffix == "jpg" || suffix == "jpeg" ||
                suffix == "bmp" || suffix == "dcm")
            {
                qDebug() << "成功拖放图片:" << filePath;
                std::string imagePath = std::string(filePath.toLocal8Bit());
                if(m_ui->openGLWidget)
                    m_ui->openGLWidget->switchTexture(imagePath);
                event->acceptProposedAction();
                return;
            }
            else
            {
                qWarning() << "不支持的文件格式:" << suffix;
            }
        }
    }

    event->ignore();
}

;




void MainWindow::StyleChanged(const QString& style)
{
    qDebug() << "当前选中文本：" << style;
    QString styledir = ":/res/QSS/" + style + ".qss";
    loadStyleSheet(styledir);
}



void MainWindow::ShutDown()
{
    QApplication::quit();
}



void MainWindow::Change_CurrentTime()
{
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
	m_ui->label_currenttime->setText(currentTime);
}



void MainWindow::ProgressChanged(int value, int max)
{
	if (m_progressDialog)
	{
		m_progressDialog->setProgress(value);
		m_progressDialog->setMaximumValue(max);
		m_progressDialog->setLabelText(QString("正在处理... %1/%2").arg(value).arg(max));
		qApp->processEvents();
	}
}

void MainWindow::OpenImg()
{
	QPushButton* senderBtn = qobject_cast<QPushButton*>(sender());


	QString ImgsDir = QCoreApplication::applicationDirPath()+"/../../QtGuiApp/assets/textures";
	QDir dir(ImgsDir);
    QString initialDir = dir.absolutePath();

    QString path;
    if (senderBtn == m_ui->pushButton_openimg)
    {
        path = QFileDialog::getOpenFileName(this, "选择图像文件", initialDir, "图像文件 (*.png *.jpg *.bmp *.dcm)");
    }
    else if(senderBtn == m_ui->pushButton_opendicomdir)
    {
        path = QFileDialog::getExistingDirectory(this, "选择 DICOM 文件夹", initialDir,
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	}
    
    if(!path.isEmpty())
    {
		std::string imagePath =  std::string(path.toLocal8Bit());
		m_ui->openGLWidget->switchTexture(imagePath);
	}
}

void MainWindow::onDicomLoaded(int totalSlices, int currentSlice)
{
    qDebug() << "🎯 MainWindow 收到 DICOM 加载信号: 总切片=" << totalSlices
        << " 当前切片=" << currentSlice;

    if (totalSlices > 1) {
        // 启用滑块
        m_ui->horizontalSlider_slice->setEnabled(true);

        // 设置范围 (0 到 totalSlices-1)
        m_ui->horizontalSlider_slice->setMinimum(0);
        m_ui->horizontalSlider_slice->setMaximum(totalSlices - 1);

        // 设置当前值
        m_ui->horizontalSlider_slice->setValue(currentSlice);

        // 更新显示文本
        m_ui->label_showthreadnum->setText(QString("%1/%2").arg(currentSlice + 1).arg(totalSlices));

        qDebug() << "✅ 滑块已启用,范围: 0 -" << (totalSlices - 1);
    }
    else {
        // 单张图像,禁用滑块
        m_ui->horizontalSlider_slice->setEnabled(false);
        m_ui->label_showthreadnum->setText("1/1");
    }
}


void MainWindow::onSliceChanged(int value)
{
    qDebug() << "🔄 滑块值变化:" << value;

    // 通知 OpenGL 控件切换切片
    m_ui->openGLWidget->setCurrentSlice(value);

    // 更新显示文本
    int totalSlices = m_ui->horizontalSlider_slice->maximum() + 1;
    m_ui->label_showthreadnum->setText(QString("%1/%2").arg(value + 1).arg(totalSlices));
}

void MainWindow::Freeze()
{
 
    // 构造 JSON 消息
    QJsonObject data;
    data["value"] = 0;

    QJsonObject root;
    root["action"] = "freeze";
    root["data"] = data;

    QJsonDocument doc(root);
    QByteArray jsonMsg = doc.toJson(QJsonDocument::Compact);

    qDebug() << "Sending JSON message -" << jsonMsg;

    // 通过 socket 发送
    if (m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
        m_tcpSocket->write(jsonMsg);
        m_tcpSocket->flush();
    }
  
}





void MainWindow::InitSlots()
{
	m_current_time = new QTimer(this);
	connect(m_current_time, &QTimer::timeout, this, &MainWindow::Change_CurrentTime);
    m_current_time->start(1000);
    
    connect(m_ui->comboBox, &QComboBox::currentTextChanged, this, &MainWindow::StyleChanged);
    connect(m_ui->pushButton_shutdown, &QPushButton::clicked, this, &MainWindow::ShutDown);
    
	connect(m_ui->pushButton_openimg, &QPushButton::clicked, this, &MainWindow::OpenImg);
    
    connect(m_ui->pushButton_opendicomdir, &QPushButton::clicked, this, &MainWindow::OpenImg);


    // 新增:连接 OpenGL 控件的 DICOM 加载信号
    connect(m_ui->openGLWidget, &MyOpenGLWidget::dicomLoaded,
        this, &MainWindow::onDicomLoaded);

    // 新增:连接滑块值变化信号
    connect(m_ui->horizontalSlider_slice, &QSlider::valueChanged,
        this, &MainWindow::onSliceChanged);
   
}

void MainWindow::UpdateGUI()
{
    if (!m_ui )
        return;
	m_ui->label_hospital->setText("二维图渲染demo");
    m_ui->comboBox->setCurrentIndex(2);
    setWindowIcon(QIcon(":/res/icon/logo.ico")); // 覆盖可能的默认值
    setWindowTitle("上海泊维胜科技有限公司");

    // 初始化滑块为禁用状态
    m_ui->horizontalSlider_slice->setEnabled(false);
    m_ui->horizontalSlider_slice->setMinimum(0);
    m_ui->horizontalSlider_slice->setMaximum(0);
    m_ui->horizontalSlider_slice->setValue(0);
}

void MainWindow::UpdateSize()
{
	sysconfig::GlobalConfig& config = sysconfig::GlobalConfig::get();
    QSize windowsize = config.getAdjustedSize();
    this->resize(windowsize);
}

void MainWindow::loadStyleSheet(const QString& path)
{
    QFile qssFile(path);
    if (!qssFile.exists()) {
        qWarning() << "QSS 文件不存在:" << path;
        return;
    }
    if (qssFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(qssFile.readAll());
        qApp->setStyleSheet(styleSheet); // 全局应用样式
        qssFile.close();
    }
    else {
        qWarning() << "无法打开 QSS 文件:" << path;
    }
}

void MainWindow::InitThread()
{
    //方式一QThread
    //int idealThreads = MyThread::idealThreadCount(); // 获取硬件支持的最大线程数
    //int activeThreads = MyThread::getActiveThreadCount();
    //qDebug() << "硬件支持的最大线程数: " << idealThreads;
    //qDebug() << "现在活跃的线程数: " << activeThreads;

    //if (!m_thread->isRunning())
    //{
    //    m_thread->setPriority(QThread::LowestPriority); // 设置线程优先级
    //    m_thread->start(); // 启动线程
    //}
    //else
    //{
    //    qWarning() << "线程正在运行，无法启动新任务！";
    //}
  /*  connect(m_thread, &MyThread::numberGenerated, this, [this](int num) {
        m_ui->label_showthreadnum->setText(QString::number(num));
        });*/

    

    //方式二QObject 创建线程对象
    
    if (m_thread_work)
    {
        m_thread_work->moveToThread(m_numsub);
    }
    connect(m_thread_work, &MyThread_Work::numberGenerated, this, [this](int num) {
        m_ui->label_showthreadnum->setText(QString::number(num));
        });
    connect(m_numsub, &QThread::started, m_thread_work, &MyThread_Work::working);
    m_numsub->start();
	
        
   

    //方式三 线程池
    /*QThreadPool::globalInstance()->setMaxThreadCount(4);
	if (m_thread_runnable)
	{
		QThreadPool::globalInstance()->start(m_thread_runnable);
	}
    connect(m_thread_runnable, &MyThread_Runnable::numberGenerated, this, [this](int num) {

        m_ui->label_showthreadnum->setText(QString::number(num));
        });
    */

    //if (!m_scpWorkThread)
    //    return;
    //m_scpWorkThread->moveToThread(m_DCMScpsub);
    //connect(m_DCMScpsub, &QThread::started, m_scpWorkThread, &MyThread_DCMTK_SCP_Work::Working);

    ////m_scpWorkThread->GetSCP().setPort(11112); // 设置端口号
    //m_DCMScpsub->start();

}





