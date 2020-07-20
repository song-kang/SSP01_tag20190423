/********************************************************************************
** Form generated from reading UI file 'view_plugin.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_PLUGIN_H
#define UI_VIEW_PLUGIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ViewPluginClass
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *editKeyword;
    QPushButton *btnQuery;
    QCheckBox *checkBoxShowFunPoint;
    QSpacerItem *horizontalSpacer;
    QTreeView *treeView;

    void setupUi(QWidget *ViewPluginClass)
    {
        if (ViewPluginClass->objectName().isEmpty())
            ViewPluginClass->setObjectName(QString::fromUtf8("ViewPluginClass"));
        ViewPluginClass->resize(842, 530);
        gridLayout = new QGridLayout(ViewPluginClass);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(ViewPluginClass);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        editKeyword = new QLineEdit(ViewPluginClass);
        editKeyword->setObjectName(QString::fromUtf8("editKeyword"));
        editKeyword->setMinimumSize(QSize(150, 0));
        editKeyword->setMaximumSize(QSize(150, 16777215));

        horizontalLayout->addWidget(editKeyword);

        btnQuery = new QPushButton(ViewPluginClass);
        btnQuery->setObjectName(QString::fromUtf8("btnQuery"));

        horizontalLayout->addWidget(btnQuery);

        checkBoxShowFunPoint = new QCheckBox(ViewPluginClass);
        checkBoxShowFunPoint->setObjectName(QString::fromUtf8("checkBoxShowFunPoint"));

        horizontalLayout->addWidget(checkBoxShowFunPoint);

        horizontalSpacer = new QSpacerItem(528, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        treeView = new QTreeView(ViewPluginClass);
        treeView->setObjectName(QString::fromUtf8("treeView"));
        treeView->setStyleSheet(QString::fromUtf8("QTreeView::item{height:23px;}"));

        gridLayout->addWidget(treeView, 1, 0, 1, 1);


        retranslateUi(ViewPluginClass);

        QMetaObject::connectSlotsByName(ViewPluginClass);
    } // setupUi

    void retranslateUi(QWidget *ViewPluginClass)
    {
        ViewPluginClass->setWindowTitle(QApplication::translate("ViewPluginClass", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ViewPluginClass", "\346\237\245\346\211\276", 0, QApplication::UnicodeUTF8));
        btnQuery->setText(QApplication::translate("ViewPluginClass", "\347\241\256\345\256\232", 0, QApplication::UnicodeUTF8));
        checkBoxShowFunPoint->setText(QApplication::translate("ViewPluginClass", "\346\230\276\347\244\272\345\212\237\350\203\275\347\202\271", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ViewPluginClass: public Ui_ViewPluginClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_PLUGIN_H
