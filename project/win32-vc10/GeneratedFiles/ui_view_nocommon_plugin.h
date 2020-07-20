/********************************************************************************
** Form generated from reading UI file 'view_nocommon_plugin.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_NOCOMMON_PLUGIN_H
#define UI_VIEW_NOCOMMON_PLUGIN_H

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
#include <QtGui/QToolButton>
#include <QtGui/QTreeView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ViewNoCommonPluginClass
{
public:
    QGridLayout *gridLayoutMain;
    QGridLayout *gridLayout;
    QToolButton *toolButtonAll;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *editKeyword;
    QPushButton *btnQuery;
    QCheckBox *checkBoxShowFunPoint;
    QSpacerItem *horizontalSpacer;
    QTreeView *treeView;

    void setupUi(QWidget *ViewNoCommonPluginClass)
    {
        if (ViewNoCommonPluginClass->objectName().isEmpty())
            ViewNoCommonPluginClass->setObjectName(QString::fromUtf8("ViewNoCommonPluginClass"));
        ViewNoCommonPluginClass->resize(691, 494);
        gridLayoutMain = new QGridLayout(ViewNoCommonPluginClass);
        gridLayoutMain->setObjectName(QString::fromUtf8("gridLayoutMain"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setHorizontalSpacing(0);
        toolButtonAll = new QToolButton(ViewNoCommonPluginClass);
        toolButtonAll->setObjectName(QString::fromUtf8("toolButtonAll"));
        toolButtonAll->setMinimumSize(QSize(80, 25));

        gridLayout->addWidget(toolButtonAll, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 418, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 1, 0, 1, 1);


        gridLayoutMain->addLayout(gridLayout, 0, 0, 2, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(ViewNoCommonPluginClass);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        editKeyword = new QLineEdit(ViewNoCommonPluginClass);
        editKeyword->setObjectName(QString::fromUtf8("editKeyword"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(editKeyword->sizePolicy().hasHeightForWidth());
        editKeyword->setSizePolicy(sizePolicy);
        editKeyword->setMinimumSize(QSize(150, 0));
        editKeyword->setMaximumSize(QSize(150, 16777215));

        horizontalLayout->addWidget(editKeyword);

        btnQuery = new QPushButton(ViewNoCommonPluginClass);
        btnQuery->setObjectName(QString::fromUtf8("btnQuery"));

        horizontalLayout->addWidget(btnQuery);

        checkBoxShowFunPoint = new QCheckBox(ViewNoCommonPluginClass);
        checkBoxShowFunPoint->setObjectName(QString::fromUtf8("checkBoxShowFunPoint"));

        horizontalLayout->addWidget(checkBoxShowFunPoint);

        horizontalSpacer = new QSpacerItem(528, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        gridLayoutMain->addLayout(horizontalLayout, 0, 1, 1, 1);

        treeView = new QTreeView(ViewNoCommonPluginClass);
        treeView->setObjectName(QString::fromUtf8("treeView"));
        treeView->setStyleSheet(QString::fromUtf8("QTreeView::item{height:23px;}"));

        gridLayoutMain->addWidget(treeView, 1, 1, 1, 1);


        retranslateUi(ViewNoCommonPluginClass);

        QMetaObject::connectSlotsByName(ViewNoCommonPluginClass);
    } // setupUi

    void retranslateUi(QWidget *ViewNoCommonPluginClass)
    {
        ViewNoCommonPluginClass->setWindowTitle(QApplication::translate("ViewNoCommonPluginClass", "\345\212\237\350\203\275\346\217\222\344\273\266", 0, QApplication::UnicodeUTF8));
        toolButtonAll->setText(QApplication::translate("ViewNoCommonPluginClass", "\346\211\200\346\234\211", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ViewNoCommonPluginClass", "\346\237\245\346\211\276", 0, QApplication::UnicodeUTF8));
        btnQuery->setText(QApplication::translate("ViewNoCommonPluginClass", "\347\241\256\345\256\232", 0, QApplication::UnicodeUTF8));
        checkBoxShowFunPoint->setText(QApplication::translate("ViewNoCommonPluginClass", "\346\230\276\347\244\272\345\212\237\350\203\275\347\202\271", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ViewNoCommonPluginClass: public Ui_ViewNoCommonPluginClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_NOCOMMON_PLUGIN_H
