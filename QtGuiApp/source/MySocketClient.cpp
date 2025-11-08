#include "MySocketClient.h"
#include "MySocketClient-UI.h"
#include <QMessageBox>
#include <QNetworkProxy>
#include <QNetworkInterface>
MySocketClient::MySocketClient(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MySocketClient)
{
	m_tcpSocket = new QTcpSocket(this); // Initialize the TCP socket for client communication
    ui->setupUi(this);
    InitSlots();
    UpdateGUI();
    UpdateSize();
}

MySocketClient::~MySocketClient()
{
    delete ui;
}

void MySocketClient::InitSlots()
{
    connect(ui->pushButton_connect, &QPushButton::clicked, this, 
		&MySocketClient::on_set_connect_clicked);
	connect(ui->pushButton_disconnect, &QPushButton::clicked, this,
		&MySocketClient::on_set_disconnect_clicked);

	connect(m_tcpSocket, &QTcpSocket::readyRead, this, [this]() {
		QByteArray data = m_tcpSocket->readAll();
		ui->textEdit_history->append(QString("客户端说: %1").arg(QString(data)));
		});

	connect(m_tcpSocket, &QTcpSocket::disconnected, this, [this]() {
		m_tcpSocket->close();
		ui->textEdit_history->append("断开链接！");
		ui->pushButton_connect->setEnabled(true);
		ui->pushButton_disconnect->setDisabled(true);
		});

	connect(m_tcpSocket, &QTcpSocket::connected, this, [this]() {
		ui->textEdit_history->append("连接成功！");
		ui->pushButton_connect->setDisabled(true);
		ui->pushButton_disconnect->setEnabled(true);
		});

	connect(ui->pushButton_sendmsg, &QPushButton::clicked, this,
		&MySocketClient::on_sendmsg_clicked);
}

void MySocketClient::UpdateGUI()
{
	ui->lineEdit_port->setText("8899");
    sysconfig::GlobalConfig& config = sysconfig::GlobalConfig::get();
    //ui->lineEdit_IP->setText(config.getLocalIpAddress());
	QString localIp;
	const QList<QHostAddress>& addresses = QNetworkInterface::allAddresses();
	for (const QHostAddress& address : addresses) {
		if (address.protocol() == QAbstractSocket::IPv4Protocol &&
			address != QHostAddress::LocalHost) {
			localIp = address.toString();
			break;
		}
	}
	if (localIp.isEmpty())
		localIp = "127.0.0.1";
	ui->lineEdit_IP->setText(localIp);
	ui->pushButton_disconnect->setDisabled(true);
}

void MySocketClient::UpdateSize()
{
}

void MySocketClient::on_set_disconnect_clicked()
{
	m_tcpSocket->close();
	ui->pushButton_connect->setEnabled(true);
	ui->pushButton_disconnect->setDisabled(true);
}

void MySocketClient::on_sendmsg_clicked()
{
	QString msg = ui->textEdit_sendmsg->toPlainText();
	m_tcpSocket->write(msg.toUtf8());
	ui->textEdit_history->append(QString("客户端说: %1").arg(msg));
}

void MySocketClient::on_set_connect_clicked()
{
	QString ip = ui->lineEdit_IP->text();
	quint16 port = ui->lineEdit_port->text().toUShort();
	if (ip.isEmpty() || port == 0) {
		QMessageBox::warning(this, tr("Warning"), tr("IP address or port cannot be empty."));
		return;
	}
	m_tcpSocket->setProxy(QNetworkProxy::NoProxy);
	m_tcpSocket->connectToHost(ip, port);
	if (m_tcpSocket->waitForConnected(3000)) {
		QMessageBox::information(this, tr("Success"), tr("Connected to server successfully."));
	}
	else {
		QMessageBox::critical(this, tr("Error"), tr("Failed to connect to server: %1").arg(m_tcpSocket->errorString()));
	}
}
