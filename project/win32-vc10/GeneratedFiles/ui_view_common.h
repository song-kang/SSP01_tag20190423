/********************************************************************************
** Form generated from reading UI file 'view_common.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_COMMON_H
#define UI_VIEW_COMMON_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "mtablewidget.h"

QT_BEGIN_NAMESPACE

class Ui_view_common
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QSpacerItem *horizontalSpacer;
    mTableWidget *tableWidget;

    void setupUi(QWidget *view_common)
    {
        if (view_common->objectName().isEmpty())
            view_common->setObjectName(QString::fromUtf8("view_common"));
        view_common->resize(484, 330);
        gridLayout = new QGridLayout(view_common);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget = new QWidget(view_common);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setMaximumSize(QSize(16777215, 30));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 5);
        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165925169.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon);

        horizontalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(widget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_2->setIcon(icon1);

        horizontalLayout->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(widget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/ssp_gui/Resources/save.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_3->setIcon(icon2);

        horizontalLayout->addWidget(pushButton_3);

        pushButton_4 = new QPushButton(widget);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/ssp_gui/Resources/print.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_4->setIcon(icon3);

        horizontalLayout->addWidget(pushButton_4);

        pushButton_5 = new QPushButton(widget);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/ssp_gui/Resources/export2.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_5->setIcon(icon4);

        horizontalLayout->addWidget(pushButton_5);

        horizontalSpacer = new QSpacerItem(116, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(widget);

        tableWidget = new mTableWidget(view_common);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

        verticalLayout->addWidget(tableWidget);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(view_common);

        QMetaObject::connectSlotsByName(view_common);
    } // setupUi

    void retranslateUi(QWidget *view_common)
    {
        view_common->setWindowTitle(QApplication::translate("view_common", "view_common", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("view_common", "\346\226\260\345\242\236(&A)", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("view_common", "\345\210\240\351\231\244(&D)", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("view_common", "\344\277\235\345\255\230(&S)", 0, QApplication::UnicodeUTF8));
        pushButton_4->setText(QApplication::translate("view_common", "\346\211\223\345\215\260(&P)", 0, QApplication::UnicodeUTF8));
        pushButton_5->setText(QApplication::translate("view_common", "\345\257\274\345\207\272(&O)", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class view_common: public Ui_view_common {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_COMMON_H
