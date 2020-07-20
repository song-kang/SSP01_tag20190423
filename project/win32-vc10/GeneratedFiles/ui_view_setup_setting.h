/********************************************************************************
** Form generated from reading UI file 'view_setup_setting.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_SETUP_SETTING_H
#define UI_VIEW_SETUP_SETTING_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_view_setup_setting
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_typeAdd;
    QPushButton *pushButton_typeEdit;
    QPushButton *pushButton_typeDel;
    QSpacerItem *horizontalSpacer;
    QTabWidget *tabWidget;

    void setupUi(QWidget *view_setup_setting)
    {
        if (view_setup_setting->objectName().isEmpty())
            view_setup_setting->setObjectName(QString::fromUtf8("view_setup_setting"));
        view_setup_setting->resize(400, 300);
        gridLayout = new QGridLayout(view_setup_setting);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(4);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton_typeAdd = new QPushButton(view_setup_setting);
        pushButton_typeAdd->setObjectName(QString::fromUtf8("pushButton_typeAdd"));
        pushButton_typeAdd->setMaximumSize(QSize(16777215, 24));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165925169.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_typeAdd->setIcon(icon);

        horizontalLayout->addWidget(pushButton_typeAdd);

        pushButton_typeEdit = new QPushButton(view_setup_setting);
        pushButton_typeEdit->setObjectName(QString::fromUtf8("pushButton_typeEdit"));
        pushButton_typeEdit->setMaximumSize(QSize(16777215, 24));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165924420.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_typeEdit->setIcon(icon1);

        horizontalLayout->addWidget(pushButton_typeEdit);

        pushButton_typeDel = new QPushButton(view_setup_setting);
        pushButton_typeDel->setObjectName(QString::fromUtf8("pushButton_typeDel"));
        pushButton_typeDel->setMaximumSize(QSize(16777215, 24));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_typeDel->setIcon(icon2);

        horizontalLayout->addWidget(pushButton_typeDel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        tabWidget = new QTabWidget(view_setup_setting);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));

        verticalLayout->addWidget(tabWidget);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(view_setup_setting);

        QMetaObject::connectSlotsByName(view_setup_setting);
    } // setupUi

    void retranslateUi(QWidget *view_setup_setting)
    {
        view_setup_setting->setWindowTitle(QApplication::translate("view_setup_setting", "view_setup_setting", 0, QApplication::UnicodeUTF8));
        pushButton_typeAdd->setText(QApplication::translate("view_setup_setting", "\346\267\273\345\212\240\347\261\273\345\236\213(&A)", 0, QApplication::UnicodeUTF8));
        pushButton_typeEdit->setText(QApplication::translate("view_setup_setting", "\344\277\256\346\224\271\345\220\215\347\247\260(&E)", 0, QApplication::UnicodeUTF8));
        pushButton_typeDel->setText(QApplication::translate("view_setup_setting", "\345\210\240\351\231\244\347\261\273\345\236\213(&D)", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class view_setup_setting: public Ui_view_setup_setting {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_SETUP_SETTING_H
