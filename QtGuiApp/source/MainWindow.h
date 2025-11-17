#ifndef MED_IMG_MAINWINDOW_H
#define MED_IMG_MAINWINDOW_H
#include "pch.h"
#include <QNetworkInterface>
class QVTKOpenGLNativeWidget;
class vtkRenderer;
class DcmCStoreSender;
class MySocketServer;
class MySocketClient;
class MyThread_DCMTK_SCP_Work;
namespace Ui {
    class MainWindow_UI;
}
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();


protected:
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;

private slots:
    void StyleChanged(const QString& style);
    void ShutDown();
    void Change_CurrentTime();
	void ProgressChanged(int value, int max); // 进度条更新
    void OpenImg();
 
    // 处理 DICOM 加载完成
    void onDicomLoaded(int totalSlices, int currentSlice);

    // 处理滑块值变化
    void onSliceChanged(int value);

    void Freeze();

signals:

private:
    void InitSlots();
    void UpdateGUI();
    void UpdateSize();
    void loadStyleSheet(const QString& path); // 动态加载 QSS
    void InitThread();
   
	
private:
    std::mutex m_mutex; // 互斥锁
    std::unique_ptr<Ui::MainWindow_UI> m_ui; 
    MyThread* m_thread; 
    MyThread_Work* m_thread_work; 
    MyThread_DCMTK_SCP_Work* m_scpWorkThread; // DICOM SCP 工作线程
    MyThread_Runnable* m_thread_runnable; 
    QTimer* m_current_time; 
    QThread* m_DCMScpsub;
    QThread* m_numsub; 
    std::unique_ptr<MyProgressDialog> m_progressDialog; // 进度对话框指针
    QString m_localIpAddress;//ip地址
    //Dcmtk SCP

	bool m_isListening = false; // 是否正在监听
    //Socket
	MySocketServer* m_socketServer; 
	MySocketClient* m_socketClient;
    QTcpSocket* m_tcpSocket;
};

#endif // MED_IMG_MAINWINDOW_H