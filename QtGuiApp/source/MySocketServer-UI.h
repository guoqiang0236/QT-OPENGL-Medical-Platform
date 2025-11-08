/********************************************************************************
** Form generated from reading UI file 'MySocketServer-UI.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MYSOCKETSERVER_2D_UI_H
#define MYSOCKETSERVER_2D_UI_H

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

class Ui_MYSocketServer
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit_port;
    QPushButton *pushButton_listen;
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

    void setupUi(QDialog *MYSocketServer)
    {
        if (MYSocketServer->objectName().isEmpty())
            MYSocketServer->setObjectName("MYSocketServer");
        MYSocketServer->resize(458, 586);
        gridLayout = new QGridLayout(MYSocketServer);
        gridLayout->setObjectName("gridLayout");
        frame = new QFrame(MYSocketServer);
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

        pushButton_listen = new QPushButton(frame);
        pushButton_listen->setObjectName("pushButton_listen");

        horizontalLayout->addWidget(pushButton_listen);


        verticalLayout->addLayout(horizontalLayout);

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

        buttonBox = new QDialogButtonBox(MYSocketServer);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        gridLayout->addWidget(buttonBox, 1, 0, 1, 1);


        retranslateUi(MYSocketServer);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, MYSocketServer, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, MYSocketServer, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(MYSocketServer);
    } // setupUi

    void retranslateUi(QDialog *MYSocketServer)
    {
        MYSocketServer->setWindowTitle(QCoreApplication::translate("MYSocketServer", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("MYSocketServer", "\347\253\257\345\217\243\357\274\232", nullptr));
        pushButton_listen->setText(QCoreApplication::translate("MYSocketServer", "\345\220\257\345\212\250\347\233\221\345\220\254\346\234\215\345\212\241", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MYSocketServer", "\345\216\206\345\217\262\344\277\241\346\201\257", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MYSocketServer", "\345\217\221\351\200\201\347\232\204\344\277\241\346\201\257", nullptr));
        pushButton_sendmsg->setText(QCoreApplication::translate("MYSocketServer", "\345\217\221\351\200\201\344\277\241\346\201\257", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MYSocketServer: public Ui_MYSocketServer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MYSOCKETSERVER_2D_UI_H
