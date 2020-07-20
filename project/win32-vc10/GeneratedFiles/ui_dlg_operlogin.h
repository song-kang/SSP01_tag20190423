/********************************************************************************
** Form generated from reading UI file 'dlg_operlogin.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLG_OPERLOGIN_H
#define UI_DLG_OPERLOGIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_dlg_operlogin
{
public:
    QPushButton *pushButtonCancel;
    QGroupBox *groupBox_2;
    QLineEdit *lineEditMonPassword;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *lineEditMonUser;
    QGroupBox *groupBox;
    QLineEdit *lineEditPassword;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *lineEditUser;
    QGroupBox *groupBox_3;
    QPlainTextEdit *plainTextTip;
    QPushButton *pushButtonOk;

    void setupUi(QDialog *dlg_operlogin)
    {
        if (dlg_operlogin->objectName().isEmpty())
            dlg_operlogin->setObjectName(QString::fromUtf8("dlg_operlogin"));
        dlg_operlogin->resize(375, 301);
        pushButtonCancel = new QPushButton(dlg_operlogin);
        pushButtonCancel->setObjectName(QString::fromUtf8("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(212, 260, 100, 31));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonCancel->setIcon(icon);
        groupBox_2 = new QGroupBox(dlg_operlogin);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(12, 169, 351, 81));
        lineEditMonPassword = new QLineEdit(groupBox_2);
        lineEditMonPassword->setObjectName(QString::fromUtf8("lineEditMonPassword"));
        lineEditMonPassword->setGeometry(QRect(57, 49, 271, 21));
        lineEditMonPassword->setEchoMode(QLineEdit::Password);
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(7, 21, 51, 20));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(7, 50, 51, 20));
        lineEditMonUser = new QLineEdit(groupBox_2);
        lineEditMonUser->setObjectName(QString::fromUtf8("lineEditMonUser"));
        lineEditMonUser->setGeometry(QRect(57, 21, 271, 20));
        groupBox = new QGroupBox(dlg_operlogin);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(12, 81, 351, 81));
        lineEditPassword = new QLineEdit(groupBox);
        lineEditPassword->setObjectName(QString::fromUtf8("lineEditPassword"));
        lineEditPassword->setGeometry(QRect(57, 48, 271, 21));
        lineEditPassword->setEchoMode(QLineEdit::Password);
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(7, 20, 51, 20));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(7, 49, 51, 20));
        lineEditUser = new QLineEdit(groupBox);
        lineEditUser->setObjectName(QString::fromUtf8("lineEditUser"));
        lineEditUser->setGeometry(QRect(57, 20, 271, 20));
        groupBox_3 = new QGroupBox(dlg_operlogin);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(12, 10, 351, 65));
        plainTextTip = new QPlainTextEdit(groupBox_3);
        plainTextTip->setObjectName(QString::fromUtf8("plainTextTip"));
        plainTextTip->setEnabled(true);
        plainTextTip->setGeometry(QRect(7, 15, 331, 40));
        plainTextTip->setStyleSheet(QString::fromUtf8("background-color: rgb(234, 234, 234);"));
        plainTextTip->setReadOnly(true);
        pushButtonOk = new QPushButton(dlg_operlogin);
        pushButtonOk->setObjectName(QString::fromUtf8("pushButtonOk"));
        pushButtonOk->setGeometry(QRect(57, 260, 100, 31));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165924420.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonOk->setIcon(icon1);
        pushButtonOk->setDefault(true);
        QWidget::setTabOrder(lineEditUser, lineEditPassword);
        QWidget::setTabOrder(lineEditPassword, lineEditMonUser);
        QWidget::setTabOrder(lineEditMonUser, lineEditMonPassword);
        QWidget::setTabOrder(lineEditMonPassword, pushButtonOk);
        QWidget::setTabOrder(pushButtonOk, pushButtonCancel);
        QWidget::setTabOrder(pushButtonCancel, plainTextTip);

        retranslateUi(dlg_operlogin);

        QMetaObject::connectSlotsByName(dlg_operlogin);
    } // setupUi

    void retranslateUi(QDialog *dlg_operlogin)
    {
        dlg_operlogin->setWindowTitle(QApplication::translate("dlg_operlogin", "\346\223\215\344\275\234\346\235\203\351\231\220\351\252\214\350\257\201", 0, QApplication::UnicodeUTF8));
        pushButtonCancel->setText(QApplication::translate("dlg_operlogin", "\345\217\226\346\266\210(&C)", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("dlg_operlogin", "\347\233\221\346\212\244\344\272\272", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("dlg_operlogin", "\347\224\250\346\210\267\345\220\215\357\274\232", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("dlg_operlogin", "\345\257\206  \347\240\201\357\274\232", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("dlg_operlogin", "\346\223\215\344\275\234\344\272\272", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("dlg_operlogin", "\347\224\250\346\210\267\345\220\215\357\274\232", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("dlg_operlogin", "\345\257\206  \347\240\201\357\274\232", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("dlg_operlogin", "\346\235\203\351\231\220\351\252\214\350\257\201", 0, QApplication::UnicodeUTF8));
        plainTextTip->setPlainText(QString());
        pushButtonOk->setText(QApplication::translate("dlg_operlogin", "\351\252\214\350\257\201(&S)", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class dlg_operlogin: public Ui_dlg_operlogin {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLG_OPERLOGIN_H
