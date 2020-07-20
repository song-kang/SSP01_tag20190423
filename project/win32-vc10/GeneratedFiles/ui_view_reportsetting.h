/********************************************************************************
** Form generated from reading UI file 'view_reportsetting.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_REPORTSETTING_H
#define UI_VIEW_REPORTSETTING_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_view_reportsetting
{
public:

    void setupUi(QWidget *view_reportsetting)
    {
        if (view_reportsetting->objectName().isEmpty())
            view_reportsetting->setObjectName(QString::fromUtf8("view_reportsetting"));
        view_reportsetting->resize(400, 300);

        retranslateUi(view_reportsetting);

        QMetaObject::connectSlotsByName(view_reportsetting);
    } // setupUi

    void retranslateUi(QWidget *view_reportsetting)
    {
        view_reportsetting->setWindowTitle(QApplication::translate("view_reportsetting", "view_reportsetting", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class view_reportsetting: public Ui_view_reportsetting {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_REPORTSETTING_H
