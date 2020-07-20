/********************************************************************************
** Form generated from reading UI file 'dlg_login.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLG_LOGIN_H
#define UI_DLG_LOGIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_dlg_login
{
public:
    QPushButton *pushButtonCancel;
    QPushButton *pushButtonOk;
    QGroupBox *groupBox;
    QLabel *label;
    QLineEdit *lineEditPassword;
    QLineEdit *lineEditName;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;

    void setupUi(QDialog *dlg_login)
    {
        if (dlg_login->objectName().isEmpty())
            dlg_login->setObjectName(QString::fromUtf8("dlg_login"));
        dlg_login->resize(383, 182);
        pushButtonCancel = new QPushButton(dlg_login);
        pushButtonCancel->setObjectName(QString::fromUtf8("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(190, 139, 100, 31));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonCancel->setIcon(icon);
        pushButtonOk = new QPushButton(dlg_login);
        pushButtonOk->setObjectName(QString::fromUtf8("pushButtonOk"));
        pushButtonOk->setGeometry(QRect(70, 139, 100, 31));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/SECUR08.ICO"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonOk->setIcon(icon1);
        pushButtonOk->setDefault(true);
        groupBox = new QGroupBox(dlg_login);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 361, 111));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(40, 30, 51, 20));
        lineEditPassword = new QLineEdit(groupBox);
        lineEditPassword->setObjectName(QString::fromUtf8("lineEditPassword"));
        lineEditPassword->setGeometry(QRect(90, 69, 241, 21));
        lineEditPassword->setEchoMode(QLineEdit::Password);
        lineEditName = new QLineEdit(groupBox);
        lineEditName->setObjectName(QString::fromUtf8("lineEditName"));
        lineEditName->setGeometry(QRect(90, 30, 241, 20));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(40, 70, 51, 20));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 30, 21, 20));
        label_3->setPixmap(QPixmap(QString::fromUtf8(":/ssp_gui/Resources/user2.gif")));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 70, 21, 20));
        label_4->setPixmap(QPixmap(QString::fromUtf8(":/ssp_gui/Resources/20070402165924420.gif")));
        QWidget::setTabOrder(lineEditName, lineEditPassword);
        QWidget::setTabOrder(lineEditPassword, pushButtonOk);
        QWidget::setTabOrder(pushButtonOk, pushButtonCancel);

        retranslateUi(dlg_login);

        QMetaObject::connectSlotsByName(dlg_login);
    } // setupUi

    void retranslateUi(QDialog *dlg_login)
    {
        dlg_login->setWindowTitle(QApplication::translate("dlg_login", "\347\231\273\345\275\225", 0, QApplication::UnicodeUTF8));
        pushButtonCancel->setText(QApplication::translate("dlg_login", "\345\217\226\346\266\210(&C)", 0, QApplication::UnicodeUTF8));
        pushButtonOk->setText(QApplication::translate("dlg_login", "\347\231\273\345\275\225(&L)", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("dlg_login", "\347\224\250\346\210\267\347\231\273\345\275\225", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("dlg_login", "\347\224\250\346\210\267\345\220\215\357\274\232", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("dlg_login", "\345\257\206  \347\240\201\357\274\232", 0, QApplication::UnicodeUTF8));
        label_3->setText(QString());
        label_4->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class dlg_login: public Ui_dlg_login {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLG_LOGIN_H
