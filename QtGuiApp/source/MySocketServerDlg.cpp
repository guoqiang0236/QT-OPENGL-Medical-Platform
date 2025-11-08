#include "MySocketServerDlg.h"


MySocketServer::MySocketServer(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MYSocketServer)
	, m_tcpServer(new MyTcpServer(this)) // 初始化 TCP 服务器
{
    ui->setupUi(this);
	m_tcp = nullptr; // 初始化 TCP 客户端指针为 nullptr

    InitSlots();
    UpdateGUI();
    //UpdateSize();
}

MySocketServer::~MySocketServer()
{
    delete ui;
}

void MySocketServer::InitSlots()
{
    // 监听“监听”按钮点击事件，启动服务器监听
    connect(ui->pushButton_listen, &QPushButton::clicked, this, &MySocketServer::on_set_listen_clicked);

    // 当有新的客户端连接到服务器时
  // 在 InitSlots() 里添加
    connect(m_tcpServer, &MyTcpServer::dataReceived, this, [this](QTcpSocket* client, const QByteArray& data) {
        ui->textEdit_history->append(QString("客户端说: %1").arg(QString::fromUtf8(data)));
        });


    // 监听“发送”按钮点击事件，向客户端发送消息
    connect(ui->pushButton_sendmsg, &QPushButton::clicked, this, &MySocketServer::on_sendmsg_clicked);
}


void MySocketServer::UpdateGUI()
{
    ui->lineEdit_port->setText("8899");
}

void MySocketServer::UpdateSize()
{
    sysconfig::GlobalConfig& config = sysconfig::GlobalConfig::get();
    QSize windowsize = config.getAdjustedSize();
    this->resize(windowsize);
}

void MySocketServer::on_sendmsg_clicked()
{
    QString msg = ui->textEdit_sendmsg->toPlainText();
    if (m_tcp) {
        m_tcp->write(msg.toUtf8());
    }
    else {
        qWarning() << "m_tcp is null";
    }
    ui->textEdit_history->append(QString("服务器说: %1").arg(msg));
}

void MySocketServer::on_set_listen_clicked()
{
	unsigned short port = ui->lineEdit_port->text().toUShort();
    bool ok = m_tcpServer->listen(QHostAddress::Any, port);
    if (ok) {
        ui->pushButton_listen->setDisabled(true);
        ui->textEdit_history->append("服务器监听成功");
    }
    else {
        ui->textEdit_history->append("服务器监听失败: " + m_tcpServer->errorString());
    }
}