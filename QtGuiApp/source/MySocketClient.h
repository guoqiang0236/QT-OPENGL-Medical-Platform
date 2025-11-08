#ifndef MYSOCKETCLIENT_H
#define MYSOCKETCLIENT_H

#include <QDialog>
#include <QTcpSocket>

namespace Ui {
class MySocketClient;
}

class MySocketClient : public QDialog
{
    Q_OBJECT

public:
    explicit MySocketClient(QWidget *parent = nullptr);
    ~MySocketClient();

private:
    void InitSlots();
    void UpdateGUI();
    void UpdateSize();

private slots:
    void on_set_connect_clicked();
    void on_set_disconnect_clicked();
    void on_sendmsg_clicked();

private:
    Ui::MySocketClient *ui;
	QTcpSocket* m_tcpSocket; // TCP socket for client communication
};

#endif // MYSOCKETCLIENT_H
