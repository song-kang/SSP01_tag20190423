/********************************************************************************
** Form generated from reading UI file 'dlg_svg_datawindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLG_SVG_DATAWINDOW_H
#define UI_DLG_SVG_DATAWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QTableWidget>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_dlg_svg_datawindow
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QComboBox *cmbClsName;
    QLineEdit *edtFilter;
    QComboBox *cmbName;
    QToolButton *btnCfg;
    QTableWidget *twDataset;

    void setupUi(QWidget *dlg_svg_datawindow)
    {
        if (dlg_svg_datawindow->objectName().isEmpty())
            dlg_svg_datawindow->setObjectName(QString::fromUtf8("dlg_svg_datawindow"));
        dlg_svg_datawindow->resize(542, 532);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/NOTE06.ICO"), QSize(), QIcon::Normal, QIcon::Off);
        dlg_svg_datawindow->setWindowIcon(icon);
        gridLayout = new QGridLayout(dlg_svg_datawindow);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        cmbClsName = new QComboBox(dlg_svg_datawindow);
        cmbClsName->setObjectName(QString::fromUtf8("cmbClsName"));
        cmbClsName->setMinimumSize(QSize(60, 0));
        cmbClsName->setMaximumSize(QSize(100, 16777215));

        horizontalLayout->addWidget(cmbClsName);

        edtFilter = new QLineEdit(dlg_svg_datawindow);
        edtFilter->setObjectName(QString::fromUtf8("edtFilter"));
        edtFilter->setMinimumSize(QSize(0, 0));
        edtFilter->setMaximumSize(QSize(100, 16777215));

        horizontalLayout->addWidget(edtFilter);

        cmbName = new QComboBox(dlg_svg_datawindow);
        cmbName->setObjectName(QString::fromUtf8("cmbName"));
        cmbName->setMinimumSize(QSize(0, 0));
        cmbName->setMaximumSize(QSize(16777215, 16777215));
        cmbName->setEditable(false);

        horizontalLayout->addWidget(cmbName);

        btnCfg = new QToolButton(dlg_svg_datawindow);
        btnCfg->setObjectName(QString::fromUtf8("btnCfg"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/cfg.gif"), QSize(), QIcon::Normal, QIcon::Off);
        btnCfg->setIcon(icon1);

        horizontalLayout->addWidget(btnCfg);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 1);
        horizontalLayout->setStretch(2, 3);

        verticalLayout->addLayout(horizontalLayout);

        twDataset = new QTableWidget(dlg_svg_datawindow);
        twDataset->setObjectName(QString::fromUtf8("twDataset"));
        twDataset->setEditTriggers(QAbstractItemView::NoEditTriggers);
        twDataset->setAlternatingRowColors(true);

        verticalLayout->addWidget(twDataset);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(dlg_svg_datawindow);

        QMetaObject::connectSlotsByName(dlg_svg_datawindow);
    } // setupUi

    void retranslateUi(QWidget *dlg_svg_datawindow)
    {
        dlg_svg_datawindow->setWindowTitle(QApplication::translate("dlg_svg_datawindow", "\346\225\260\346\215\256\347\252\227\345\217\243\351\200\211\346\213\251", 0, QApplication::UnicodeUTF8));
        btnCfg->setText(QApplication::translate("dlg_svg_datawindow", "...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class dlg_svg_datawindow: public Ui_dlg_svg_datawindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLG_SVG_DATAWINDOW_H
