/********************************************************************************
** Form generated from reading UI file 'dlg_login_bluestyle.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLG_LOGIN_BLUESTYLE_H
#define UI_DLG_LOGIN_BLUESTYLE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>

QT_BEGIN_NAMESPACE

class Ui_dlg_login_blueStyle
{
public:
    QToolButton *toolButtonOK;
    QLabel *label;
    QLineEdit *lineEditPwd;
    QLabel *labelError;
    QLabel *label_2;
    QToolButton *toolButtonCancel;
    QLineEdit *lineEditName;

    void setupUi(QDialog *dlg_login_blueStyle)
    {
        if (dlg_login_blueStyle->objectName().isEmpty())
            dlg_login_blueStyle->setObjectName(QString::fromUtf8("dlg_login_blueStyle"));
        dlg_login_blueStyle->resize(420, 290);
        dlg_login_blueStyle->setMinimumSize(QSize(420, 290));
        dlg_login_blueStyle->setMaximumSize(QSize(420, 290));
        dlg_login_blueStyle->setStyleSheet(QString::fromUtf8("QDialog#dlg_login_blueStyle{border-image: url(:/resource/Resource/login_bg.png);}"));
        toolButtonOK = new QToolButton(dlg_login_blueStyle);
        toolButtonOK->setObjectName(QString::fromUtf8("toolButtonOK"));
        toolButtonOK->setGeometry(QRect(106, 200, 84, 38));
        toolButtonOK->setStyleSheet(QString::fromUtf8("QToolButton#toolButtonOK{border-image: url(:/resource/Resource/login_btn.png);}\n"
"QToolButton#toolButtonOK:hover{border-image: url(:/resource/Resource/login_btn_hover.png);}\n"
""));
        label = new QLabel(dlg_login_blueStyle);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(46, 76, 60, 18));
        label->setStyleSheet(QString::fromUtf8("border-image: url(:/resource/Resource/username.png);"));
        lineEditPwd = new QLineEdit(dlg_login_blueStyle);
        lineEditPwd->setObjectName(QString::fromUtf8("lineEditPwd"));
        lineEditPwd->setGeometry(QRect(120, 116, 240, 30));
        lineEditPwd->setInputMethodHints(Qt::ImhHiddenText|Qt::ImhNoAutoUppercase|Qt::ImhNoPredictiveText);
        lineEditPwd->setEchoMode(QLineEdit::Password);
        labelError = new QLabel(dlg_login_blueStyle);
        labelError->setObjectName(QString::fromUtf8("labelError"));
        labelError->setGeometry(QRect(120, 157, 240, 23));
        labelError->setStyleSheet(QString::fromUtf8("border-image: url(:/resource/Resource/tips.png);"));
        label_2 = new QLabel(dlg_login_blueStyle);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(46, 122, 60, 18));
        label_2->setStyleSheet(QString::fromUtf8("border-image: url(:/resource/Resource/password.png);"));
        toolButtonCancel = new QToolButton(dlg_login_blueStyle);
        toolButtonCancel->setObjectName(QString::fromUtf8("toolButtonCancel"));
        toolButtonCancel->setGeometry(QRect(222, 200, 84, 38));
        toolButtonCancel->setStyleSheet(QString::fromUtf8("QToolButton#toolButtonCancel{border-image: url(:/resource/Resource/cancle_btn.png);}\n"
"QToolButton#toolButtonCancel:hover{border-image: url(:/resource/Resource/cancle_btn_hover.png);}\n"
"\n"
""));
        lineEditName = new QLineEdit(dlg_login_blueStyle);
        lineEditName->setObjectName(QString::fromUtf8("lineEditName"));
        lineEditName->setGeometry(QRect(120, 70, 240, 30));
        QWidget::setTabOrder(lineEditName, lineEditPwd);
        QWidget::setTabOrder(lineEditPwd, toolButtonOK);
        QWidget::setTabOrder(toolButtonOK, toolButtonCancel);

        retranslateUi(dlg_login_blueStyle);

        QMetaObject::connectSlotsByName(dlg_login_blueStyle);
    } // setupUi

    void retranslateUi(QDialog *dlg_login_blueStyle)
    {
        dlg_login_blueStyle->setWindowTitle(QApplication::translate("dlg_login_blueStyle", "dlg_login_blueStyle", 0, QApplication::UnicodeUTF8));
        toolButtonOK->setText(QString());
        label->setText(QString());
        labelError->setText(QString());
        label_2->setText(QString());
        toolButtonCancel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class dlg_login_blueStyle: public Ui_dlg_login_blueStyle {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLG_LOGIN_BLUESTYLE_H
