/********************************************************************************
** Form generated from reading UI file 'view_user_mgr.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_USER_MGR_H
#define UI_VIEW_USER_MGR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_view_user_mgr
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QSpacerItem *horizontalSpacer;
    QTableWidget *tableWidget;

    void setupUi(QWidget *view_user_mgr)
    {
        if (view_user_mgr->objectName().isEmpty())
            view_user_mgr->setObjectName(QString::fromUtf8("view_user_mgr"));
        view_user_mgr->resize(400, 300);
        gridLayout = new QGridLayout(view_user_mgr);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget = new QWidget(view_user_mgr);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(widget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        horizontalLayout->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(widget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

        horizontalLayout->addWidget(pushButton_3);

        horizontalSpacer = new QSpacerItem(116, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(widget);

        tableWidget = new QTableWidget(view_user_mgr);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

        verticalLayout->addWidget(tableWidget);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(view_user_mgr);

        QMetaObject::connectSlotsByName(view_user_mgr);
    } // setupUi

    void retranslateUi(QWidget *view_user_mgr)
    {
        view_user_mgr->setWindowTitle(QApplication::translate("view_user_mgr", "Form", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("view_user_mgr", "\346\226\260\345\242\236(&A)", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("view_user_mgr", "\345\210\240\351\231\244(&D)", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("view_user_mgr", "\344\277\235\345\255\230(&S)", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class view_user_mgr: public Ui_view_user_mgr {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_USER_MGR_H
