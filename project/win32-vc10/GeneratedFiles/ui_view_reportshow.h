/********************************************************************************
** Form generated from reading UI file 'view_reportshow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_REPORTSHOW_H
#define UI_VIEW_REPORTSHOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_view_reportshow
{
public:

    void setupUi(QWidget *view_reportshow)
    {
        if (view_reportshow->objectName().isEmpty())
            view_reportshow->setObjectName(QString::fromUtf8("view_reportshow"));
        view_reportshow->resize(400, 300);

        retranslateUi(view_reportshow);

        QMetaObject::connectSlotsByName(view_reportshow);
    } // setupUi

    void retranslateUi(QWidget *view_reportshow)
    {
        view_reportshow->setWindowTitle(QApplication::translate("view_reportshow", "view_reportshow", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class view_reportshow: public Ui_view_reportshow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_REPORTSHOW_H
