/********************************************************************************
** Form generated from reading UI file 'dlg_svg_dw_setup.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLG_SVG_DW_SETUP_H
#define UI_DLG_SVG_DW_SETUP_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_dlg_svg_dw_setup
{
public:
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_2;
    QTableWidget *twDW;
    QWidget *tab_2;
    QGridLayout *gridLayout_3;
    QTableWidget *twSession;

    void setupUi(QDialog *dlg_svg_dw_setup)
    {
        if (dlg_svg_dw_setup->objectName().isEmpty())
            dlg_svg_dw_setup->setObjectName(QString::fromUtf8("dlg_svg_dw_setup"));
        dlg_svg_dw_setup->resize(403, 344);
        gridLayout = new QGridLayout(dlg_svg_dw_setup);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tabWidget = new QTabWidget(dlg_svg_dw_setup);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout_2 = new QGridLayout(tab);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        twDW = new QTableWidget(tab);
        twDW->setObjectName(QString::fromUtf8("twDW"));
        twDW->setEditTriggers(QAbstractItemView::NoEditTriggers);
        twDW->setAlternatingRowColors(true);
        twDW->setSelectionBehavior(QAbstractItemView::SelectRows);

        gridLayout_2->addWidget(twDW, 0, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        gridLayout_3 = new QGridLayout(tab_2);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        twSession = new QTableWidget(tab_2);
        twSession->setObjectName(QString::fromUtf8("twSession"));
        twSession->setEditTriggers(QAbstractItemView::NoEditTriggers);
        twSession->setAlternatingRowColors(true);
        twSession->setSelectionBehavior(QAbstractItemView::SelectRows);

        gridLayout_3->addWidget(twSession, 0, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());

        gridLayout->addWidget(tabWidget, 0, 0, 1, 1);


        retranslateUi(dlg_svg_dw_setup);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(dlg_svg_dw_setup);
    } // setupUi

    void retranslateUi(QDialog *dlg_svg_dw_setup)
    {
        dlg_svg_dw_setup->setWindowTitle(QApplication::translate("dlg_svg_dw_setup", "\346\225\260\346\215\256\347\252\227\345\217\243\350\256\276\347\275\256", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("dlg_svg_dw_setup", "\346\225\260\346\215\256\347\252\227\345\217\243\346\235\241\344\273\266", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("dlg_svg_dw_setup", "\346\211\200\346\234\211\344\274\232\350\257\235\345\261\236\346\200\247", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class dlg_svg_dw_setup: public Ui_dlg_svg_dw_setup {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLG_SVG_DW_SETUP_H
