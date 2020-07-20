/********************************************************************************
** Form generated from reading UI file 'view_setup.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_SETUP_H
#define UI_VIEW_SETUP_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_view_setup
{
public:
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;

    void setupUi(QWidget *view_setup)
    {
        if (view_setup->objectName().isEmpty())
            view_setup->setObjectName(QString::fromUtf8("view_setup"));
        view_setup->resize(400, 300);
        gridLayout = new QGridLayout(view_setup);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tabWidget = new QTabWidget(view_setup);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));

        gridLayout->addWidget(tabWidget, 0, 0, 1, 1);


        retranslateUi(view_setup);

        QMetaObject::connectSlotsByName(view_setup);
    } // setupUi

    void retranslateUi(QWidget *view_setup)
    {
        view_setup->setWindowTitle(QApplication::translate("view_setup", "view_setup", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class view_setup: public Ui_view_setup {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_SETUP_H
