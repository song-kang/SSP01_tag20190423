/********************************************************************************
** Form generated from reading UI file 'view_singleuser.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_SINGLEUSER_H
#define UI_VIEW_SINGLEUSER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_view_singleUser
{
public:
    QFrame *frame;
    QLabel *label;
    QLineEdit *lineEdit;
    QLabel *label_2;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QWidget *view_singleUser)
    {
        if (view_singleUser->objectName().isEmpty())
            view_singleUser->setObjectName(QString::fromUtf8("view_singleUser"));
        view_singleUser->resize(400, 188);
        view_singleUser->setMinimumSize(QSize(400, 188));
        view_singleUser->setStyleSheet(QString::fromUtf8(""));
        frame = new QFrame(view_singleUser);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(10, 10, 381, 161));
        frame->setMinimumSize(QSize(260, 150));
        frame->setMaximumSize(QSize(999999, 999999));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Plain);
        frame->setLineWidth(0);
        frame->setMidLineWidth(0);
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(30, 30, 41, 20));
        lineEdit = new QLineEdit(frame);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(80, 30, 251, 20));
        label_2 = new QLabel(frame);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(30, 70, 41, 20));
        lineEdit_2 = new QLineEdit(frame);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(80, 69, 251, 21));
        lineEdit_2->setEchoMode(QLineEdit::Password);
        pushButton = new QPushButton(frame);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(80, 110, 100, 31));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/user1.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon);
        pushButton_2 = new QPushButton(frame);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(200, 110, 100, 31));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_2->setIcon(icon1);

        retranslateUi(view_singleUser);

        QMetaObject::connectSlotsByName(view_singleUser);
    } // setupUi

    void retranslateUi(QWidget *view_singleUser)
    {
        view_singleUser->setWindowTitle(QApplication::translate("view_singleUser", "view_singleUser", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("view_singleUser", "\347\224\250\346\210\267\345\220\215", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("view_singleUser", "\345\257\206  \347\240\201", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("view_singleUser", "\347\231\273\345\275\225(&L)", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("view_singleUser", "\345\217\226\346\266\210(&C)", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class view_singleUser: public Ui_view_singleUser {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_SINGLEUSER_H
