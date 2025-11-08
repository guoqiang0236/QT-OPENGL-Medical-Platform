/********************************************************************************
** Form generated from reading UI file 'MySocketClient-UI.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MYSOCKETCLIENT_2D_UI_H
#define MYSOCKETCLIENT_2D_UI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_MySocketClient
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit_port;
    QPushButton *pushButton_connect;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_2;
    QLineEdit *lineEdit_IP;
    QPushButton *pushButton_disconnect;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_2;
    QTextEdit *textEdit_history;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_3;
    QTextEdit *textEdit_sendmsg;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_sendmsg;
    QSpacerItem *horizontalSpacer_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *MySocketClient)
    {
        if (MySocketClient->objectName().isEmpty())
            MySocketClient->setObjectName("MySocketClient");
        MySocketClient->resize(713, 817);
        gridLayout = new QGridLayout(MySocketClient);
        gridLayout->setObjectName("gridLayout");
        frame = new QFrame(MySocketClient);
        frame->setObjectName("frame");
        frame->setFrameShape(QFrame::Shape::StyledPanel);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(frame);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        lineEdit_port = new QLineEdit(frame);
        lineEdit_port->setObjectName("lineEdit_port");

        horizontalLayout->addWidget(lineEdit_port);

        pushButton_connect = new QPushButton(frame);
        pushButton_connect->setObjectName("pushButton_connect");

        horizontalLayout->addWidget(pushButton_connect);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        label_2 = new QLabel(frame);
        label_2->setObjectName("label_2");

        horizontalLayout_5->addWidget(label_2);

        lineEdit_IP = new QLineEdit(frame);
        lineEdit_IP->setObjectName("lineEdit_IP");

        horizontalLayout_5->addWidget(lineEdit_IP);

        pushButton_disconnect = new QPushButton(frame);
        pushButton_disconnect->setObjectName("pushButton_disconnect");

        horizontalLayout_5->addWidget(pushButton_disconnect);


        verticalLayout->addLayout(horizontalLayout_5);

        groupBox = new QGroupBox(frame);
        groupBox->setObjectName("groupBox");
        horizontalLayout_2 = new QHBoxLayout(groupBox);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        textEdit_history = new QTextEdit(groupBox);
        textEdit_history->setObjectName("textEdit_history");

        horizontalLayout_2->addWidget(textEdit_history);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(frame);
        groupBox_2->setObjectName("groupBox_2");
        horizontalLayout_3 = new QHBoxLayout(groupBox_2);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        textEdit_sendmsg = new QTextEdit(groupBox_2);
        textEdit_sendmsg->setObjectName("textEdit_sendmsg");

        horizontalLayout_3->addWidget(textEdit_sendmsg);


        verticalLayout->addWidget(groupBox_2);

        frame_2 = new QFrame(frame);
        frame_2->setObjectName("frame_2");
        frame_2->setFrameShape(QFrame::Shape::StyledPanel);
        frame_2->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_4 = new QHBoxLayout(frame_2);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalSpacer = new QSpacerItem(138, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        pushButton_sendmsg = new QPushButton(frame_2);
        pushButton_sendmsg->setObjectName("pushButton_sendmsg");

        horizontalLayout_4->addWidget(pushButton_sendmsg);

        horizontalSpacer_2 = new QSpacerItem(137, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(frame_2);


        gridLayout->addWidget(frame, 0, 0, 1, 1);

        buttonBox = new QDialogButtonBox(MySocketClient);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        gridLayout->addWidget(buttonBox, 1, 0, 1, 1);


        retranslateUi(MySocketClient);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, MySocketClient, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, MySocketClient, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(MySocketClient);
    } // setupUi

    void retranslateUi(QDialog *MySocketClient)
    {
        MySocketClient->setWindowTitle(QCoreApplication::translate("MySocketClient", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("MySocketClient", "\347\253\257\345\217\243\357\274\232", nullptr));
        pushButton_connect->setText(QCoreApplication::translate("MySocketClient", "\351\223\276\346\216\245\346\234\215\345\212\241\345\231\250", nullptr));
        label_2->setText(QCoreApplication::translate("MySocketClient", "IP\357\274\232", nullptr));
        pushButton_disconnect->setText(QCoreApplication::translate("MySocketClient", "\346\226\255\345\274\200\351\223\276\346\216\245", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MySocketClient", "\345\216\206\345\217\262\344\277\241\346\201\257", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MySocketClient", "\345\217\221\351\200\201\347\232\204\344\277\241\346\201\257", nullptr));
        pushButton_sendmsg->setText(QCoreApplication::translate("MySocketClient", "\345\217\221\351\200\201\344\277\241\346\201\257", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MySocketClient: public Ui_MySocketClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MYSOCKETCLIENT_2D_UI_H
