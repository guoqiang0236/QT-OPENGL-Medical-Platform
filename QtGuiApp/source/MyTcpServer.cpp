#include "MyTcpServer.h"
#include <QDebug>

MyTcpServer::MyTcpServer(QObject* parent)
    : QTcpServer(parent)
{
}

MyTcpServer::~MyTcpServer()
{
    for (QTcpSocket* client : clients) {
        client->disconnectFromHost();
        client->deleteLater();
    }
}

bool MyTcpServer::startServer(quint16 port)
{
    if (this->listen(QHostAddress::Any, port)) {
        qDebug() << "Server started on port" << port;
        return true;
    } else {
        qWarning() << "Server failed to start:" << this->errorString();
        return false;
    }
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket* client = new QTcpSocket(this);
    if (client->setSocketDescriptor(socketDescriptor)) {
        clients.append(client);
        connect(client, &QTcpSocket::readyRead, this, &MyTcpServer::onReadyRead);
        connect(client, &QTcpSocket::disconnected, this, &MyTcpServer::onDisconnected);
        emit clientConnected(client);
        qDebug() << "New client connected:" << client->peerAddress().toString();
    } else {
        client->deleteLater();
    }
}

void MyTcpServer::onReadyRead()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        QByteArray data = client->readAll();
        emit dataReceived(client, data);
        qDebug() << "Received data from client:" << data;
    }
}

void MyTcpServer::onDisconnected()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        clients.removeAll(client);
        emit clientDisconnected(client);
        client->deleteLater();
        qDebug() << "Client disconnected";
    }
}