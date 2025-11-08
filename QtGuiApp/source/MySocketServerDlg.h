#pragma once

#include <QDialog>
#include "MySocketServer-UI.h"
#include "MyTcpServer.h"
#include "pch.h"
#include <QTcpSocket>

class MySocketServer : public QDialog
{
    Q_OBJECT

public:
    explicit MySocketServer(QWidget* parent = nullptr);
    ~MySocketServer();

private:
    void InitSlots();
    void UpdateGUI();
    void UpdateSize();

private slots:
    void on_set_listen_clicked();
    void on_sendmsg_clicked();

private:
    Ui::MYSocketServer* ui;
	MyTcpServer* m_tcpServer; // TCP服务器对象
    QTcpSocket* m_tcp; 
};

