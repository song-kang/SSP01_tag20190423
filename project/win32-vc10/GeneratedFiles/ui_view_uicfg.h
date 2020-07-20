/********************************************************************************
** Form generated from reading UI file 'view_uicfg.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_UICFG_H
#define UI_VIEW_UICFG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "mtablewidget.h"

QT_BEGIN_NAMESPACE

class Ui_view_uicfg
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_copy;
    QLabel *label_cls;
    QComboBox *comboBox_cls;
    QCheckBox *checkBox_useCls;
    QSpacerItem *horizontalSpacer;
    mTableWidget *tableWidget;

    void setupUi(QWidget *view_uicfg)
    {
        if (view_uicfg->objectName().isEmpty())
            view_uicfg->setObjectName(QString::fromUtf8("view_uicfg"));
        view_uicfg->resize(507, 300);
        gridLayout = new QGridLayout(view_uicfg);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget = new QWidget(view_uicfg);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setMaximumSize(QSize(16777215, 30));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 5);
        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
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

        pushButton_copy = new QPushButton(widget);
        pushButton_copy->setObjectName(QString::fromUtf8("pushButton_copy"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165933688.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_copy->setIcon(icon2);

        horizontalLayout->addWidget(pushButton_copy);

        label_cls = new QLabel(widget);
        label_cls->setObjectName(QString::fromUtf8("label_cls"));

        horizontalLayout->addWidget(label_cls);

        comboBox_cls = new QComboBox(widget);
        comboBox_cls->setObjectName(QString::fromUtf8("comboBox_cls"));

        horizontalLayout->addWidget(comboBox_cls);

        checkBox_useCls = new QCheckBox(widget);
        checkBox_useCls->setObjectName(QString::fromUtf8("checkBox_useCls"));

        horizontalLayout->addWidget(checkBox_useCls);

        horizontalSpacer = new QSpacerItem(116, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(widget);

        tableWidget = new mTableWidget(view_uicfg);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

        verticalLayout->addWidget(tableWidget);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(view_uicfg);

        QMetaObject::connectSlotsByName(view_uicfg);
    } // setupUi

    void retranslateUi(QWidget *view_uicfg)
    {
        view_uicfg->setWindowTitle(QApplication::translate("view_uicfg", "view_uicfg", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("view_uicfg", "\346\226\260\345\242\236(&A)", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("view_uicfg", "\345\210\240\351\231\244(&D)", 0, QApplication::UnicodeUTF8));
        pushButton_copy->setText(QApplication::translate("view_uicfg", "\345\244\215\345\210\266(&C)", 0, QApplication::UnicodeUTF8));
        label_cls->setText(QApplication::translate("view_uicfg", "\345\210\206\347\261\273", 0, QApplication::UnicodeUTF8));
        checkBox_useCls->setText(QApplication::translate("view_uicfg", "\345\210\206\347\261\273\350\277\207\346\273\244\346\225\260\346\215\256\351\233\206\351\200\211\346\213\251", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class view_uicfg: public Ui_view_uicfg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_UICFG_H
