/********************************************************************************
** Form generated from reading UI file 'view_doubleuser.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_DOUBLEUSER_H
#define UI_VIEW_DOUBLEUSER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_view_doubleUser
{
public:
    QGroupBox *groupBox;
    QLineEdit *lineEdit_2;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *lineEdit;
    QGroupBox *groupBox_2;
    QLineEdit *lineEdit_3;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *lineEdit_4;
    QPushButton *pushButton_2;
    QPushButton *pushButton;
    QGroupBox *groupBox_3;
    QPlainTextEdit *plainTextTip;

    void setupUi(QWidget *view_doubleUser)
    {
        if (view_doubleUser->objectName().isEmpty())
            view_doubleUser->setObjectName(QString::fromUtf8("view_doubleUser"));
        view_doubleUser->resize(373, 299);
        groupBox = new QGroupBox(view_doubleUser);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 78, 351, 81));
        lineEdit_2 = new QLineEdit(groupBox);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(60, 48, 271, 21));
        lineEdit_2->setEchoMode(QLineEdit::Password);
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 20, 41, 20));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 49, 41, 20));
        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(60, 20, 271, 20));
        groupBox_2 = new QGroupBox(view_doubleUser);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 166, 351, 81));
        lineEdit_3 = new QLineEdit(groupBox_2);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(60, 49, 271, 21));
        lineEdit_3->setEchoMode(QLineEdit::Password);
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 21, 41, 20));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 50, 41, 20));
        lineEdit_4 = new QLineEdit(groupBox_2);
        lineEdit_4->setObjectName(QString::fromUtf8("lineEdit_4"));
        lineEdit_4->setGeometry(QRect(60, 21, 271, 20));
        pushButton_2 = new QPushButton(view_doubleUser);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(210, 257, 100, 31));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_2->setIcon(icon);
        pushButton = new QPushButton(view_doubleUser);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(55, 257, 100, 31));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165924420.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon1);
        groupBox_3 = new QGroupBox(view_doubleUser);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 7, 351, 65));
        plainTextTip = new QPlainTextEdit(groupBox_3);
        plainTextTip->setObjectName(QString::fromUtf8("plainTextTip"));
        plainTextTip->setGeometry(QRect(10, 15, 331, 40));
        plainTextTip->setStyleSheet(QString::fromUtf8("background-color: rgb(234, 234, 234);"));

        retranslateUi(view_doubleUser);

        QMetaObject::connectSlotsByName(view_doubleUser);
    } // setupUi

    void retranslateUi(QWidget *view_doubleUser)
    {
        view_doubleUser->setWindowTitle(QApplication::translate("view_doubleUser", "view_doubleUser", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("view_doubleUser", "\346\223\215\344\275\234\344\272\272", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("view_doubleUser", "\347\224\250\346\210\267\345\220\215", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("view_doubleUser", "\345\257\206  \347\240\201", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("view_doubleUser", "\347\233\221\346\212\244\344\272\272", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("view_doubleUser", "\347\224\250\346\210\267\345\220\215", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("view_doubleUser", "\345\257\206  \347\240\201", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("view_doubleUser", "\345\217\226\346\266\210(&C)", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("view_doubleUser", "\351\252\214\350\257\201(&S)", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("view_doubleUser", "\346\235\203\351\231\220\351\252\214\350\257\201", 0, QApplication::UnicodeUTF8));
        plainTextTip->setPlainText(QString());
    } // retranslateUi

};

namespace Ui {
    class view_doubleUser: public Ui_view_doubleUser {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_DOUBLEUSER_H
