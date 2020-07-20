/********************************************************************************
** Form generated from reading UI file 'ssp_3d_widget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SSP_3D_WIDGET_H
#define UI_SSP_3D_WIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>
#include "ssp_gui_3d_inc.h"

QT_BEGIN_NAMESPACE

class SSP_GUI_3D_EXPORT Ui_CSsp3DWidget
{
public:
    QGridLayout *gridLayout;
    QWidget *wndPannel;

    void setupUi(QWidget *CSsp3DWidget)
    {
        if (CSsp3DWidget->objectName().isEmpty())
            CSsp3DWidget->setObjectName(QString::fromUtf8("CSsp3DWidget"));
        CSsp3DWidget->resize(631, 300);
        gridLayout = new QGridLayout(CSsp3DWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(1, 1, 1, 1);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        wndPannel = new QWidget(CSsp3DWidget);
        wndPannel->setObjectName(QString::fromUtf8("wndPannel"));

        gridLayout->addWidget(wndPannel, 0, 0, 1, 1);


        retranslateUi(CSsp3DWidget);

        QMetaObject::connectSlotsByName(CSsp3DWidget);
    } // setupUi

    void retranslateUi(QWidget *CSsp3DWidget)
    {
        CSsp3DWidget->setWindowTitle(QApplication::translate("CSsp3DWidget", "CSsp3DWidget", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CSsp3DWidget: public Ui_CSsp3DWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SSP_3D_WIDGET_H
