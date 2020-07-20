/********************************************************************************
** Form generated from reading UI file 'viewmoduleinfo.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWMODULEINFO_H
#define UI_VIEWMODULEINFO_H

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

class Ui_CViewModuleInfo
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButtonQuery;
    QPushButton *pushButtonSave;
    QPushButton *pushButtonPrint;
    QSpacerItem *horizontalSpacer;
    QTableWidget *tableWidget;

    void setupUi(QWidget *CViewModuleInfo)
    {
        if (CViewModuleInfo->objectName().isEmpty())
            CViewModuleInfo->setObjectName(QString::fromUtf8("CViewModuleInfo"));
        CViewModuleInfo->resize(529, 390);
        gridLayout = new QGridLayout(CViewModuleInfo);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButtonQuery = new QPushButton(CViewModuleInfo);
        pushButtonQuery->setObjectName(QString::fromUtf8("pushButtonQuery"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/hujiaozhuanyi.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonQuery->setIcon(icon);

        horizontalLayout->addWidget(pushButtonQuery);

        pushButtonSave = new QPushButton(CViewModuleInfo);
        pushButtonSave->setObjectName(QString::fromUtf8("pushButtonSave"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/export2.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonSave->setIcon(icon1);

        horizontalLayout->addWidget(pushButtonSave);

        pushButtonPrint = new QPushButton(CViewModuleInfo);
        pushButtonPrint->setObjectName(QString::fromUtf8("pushButtonPrint"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/ssp_gui/Resources/print.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonPrint->setIcon(icon2);

        horizontalLayout->addWidget(pushButtonPrint);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        tableWidget = new QTableWidget(CViewModuleInfo);
        if (tableWidget->columnCount() < 5)
            tableWidget->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

        verticalLayout->addWidget(tableWidget);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(CViewModuleInfo);

        QMetaObject::connectSlotsByName(CViewModuleInfo);
    } // setupUi

    void retranslateUi(QWidget *CViewModuleInfo)
    {
        CViewModuleInfo->setWindowTitle(QApplication::translate("CViewModuleInfo", "CViewModuleInfo", 0, QApplication::UnicodeUTF8));
        pushButtonQuery->setText(QApplication::translate("CViewModuleInfo", "\345\210\267\346\226\260(&R)", 0, QApplication::UnicodeUTF8));
        pushButtonSave->setText(QApplication::translate("CViewModuleInfo", "\345\257\274\345\207\272(&E)...", 0, QApplication::UnicodeUTF8));
        pushButtonPrint->setText(QApplication::translate("CViewModuleInfo", "\346\211\223\345\215\260(&P)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("CViewModuleInfo", "New Column", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("CViewModuleInfo", "New Column", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("CViewModuleInfo", "New Column", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("CViewModuleInfo", "New Column", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidget->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("CViewModuleInfo", "New Column", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CViewModuleInfo: public Ui_CViewModuleInfo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWMODULEINFO_H
