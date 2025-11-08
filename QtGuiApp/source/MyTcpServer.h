#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QList>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit MyTcpServer(QObject* parent = nullptr);
    ~MyTcpServer();

    bool startServer(quint16 port);

signals:
    void clientConnected(QTcpSocket* client);
    void clientDisconnected(QTcpSocket* client);
    void dataReceived(QTcpSocket* client, const QByteArray& data);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    QList<QTcpSocket*> clients;
};