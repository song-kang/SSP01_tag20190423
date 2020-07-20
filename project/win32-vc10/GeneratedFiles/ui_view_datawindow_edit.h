/********************************************************************************
** Form generated from reading UI file 'view_datawindow_edit.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_DATAWINDOW_EDIT_H
#define UI_VIEW_DATAWINDOW_EDIT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_view_datawindow_edit
{
public:
    QGridLayout *gridLayout_12;
    QGridLayout *gridLayout_11;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_add;
    QPushButton *pushButton_copy;
    QPushButton *pushButton_del;
    QPushButton *pushButton_view;
    QPushButton *pushButton_import;
    QPushButton *pushButton_export;
    QPushButton *pushButton_apply;
    QSpacerItem *horizontalSpacer;
    QTabWidget *tabWidget;
    QWidget *tab_1;
    QGridLayout *gridLayout_5;
    QSplitter *splitter_2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_ref_cls;
    QComboBox *comboBox_ref_cls;
    QSpacerItem *horizontalSpacer_5;
    QTableWidget *tableWidget_ref1;
    QSplitter *splitter;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QTextEdit *textEdit_ref;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QWidget *widget_2;
    QGridLayout *gridLayout_2;
    QPushButton *pushButton_ref2_add;
    QPushButton *pushButton_ref2_del;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_4;
    QTableWidget *tableWidget_ref2;
    QWidget *tab_2;
    QGridLayout *gridLayout_13;
    QSplitter *splitter_4;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_7;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_data_cls;
    QComboBox *comboBox_data_cls;
    QCheckBox *checkBox_useCls;
    QSpacerItem *horizontalSpacer_6;
    QTableWidget *tableWidget_data1;
    QSplitter *splitter_3;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_6;
    QTextEdit *textEdit_data;
    QTabWidget *tabWidget_2;
    QWidget *tab_4;
    QGridLayout *gridLayout_9;
    QVBoxLayout *verticalLayout_3;
    QWidget *widget_3;
    QGridLayout *gridLayout_10;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *pushButton_newcol;
    QPushButton *pushButton_data2_add;
    QPushButton *pushButton_data2_del;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_8;
    QTableWidget *tableWidget_data2;
    QWidget *tab_5;

    void setupUi(QWidget *view_datawindow_edit)
    {
        if (view_datawindow_edit->objectName().isEmpty())
            view_datawindow_edit->setObjectName(QString::fromUtf8("view_datawindow_edit"));
        view_datawindow_edit->resize(882, 612);
        gridLayout_12 = new QGridLayout(view_datawindow_edit);
        gridLayout_12->setSpacing(6);
        gridLayout_12->setContentsMargins(11, 11, 11, 11);
        gridLayout_12->setObjectName(QString::fromUtf8("gridLayout_12"));
        gridLayout_12->setContentsMargins(4, 4, 4, 4);
        gridLayout_11 = new QGridLayout();
        gridLayout_11->setSpacing(6);
        gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
        widget = new QWidget(view_datawindow_edit);
        widget->setObjectName(QString::fromUtf8("widget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        widget->setMinimumSize(QSize(0, 0));
        widget->setMaximumSize(QSize(16777215, 30));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(4);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 4);
        pushButton_add = new QPushButton(widget);
        pushButton_add->setObjectName(QString::fromUtf8("pushButton_add"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165925169.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_add->setIcon(icon);

        horizontalLayout->addWidget(pushButton_add);

        pushButton_copy = new QPushButton(widget);
        pushButton_copy->setObjectName(QString::fromUtf8("pushButton_copy"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165933688.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_copy->setIcon(icon1);

        horizontalLayout->addWidget(pushButton_copy);

        pushButton_del = new QPushButton(widget);
        pushButton_del->setObjectName(QString::fromUtf8("pushButton_del"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_del->setIcon(icon2);

        horizontalLayout->addWidget(pushButton_del);

        pushButton_view = new QPushButton(widget);
        pushButton_view->setObjectName(QString::fromUtf8("pushButton_view"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/ssp_gui/Resources/preview.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_view->setIcon(icon3);

        horizontalLayout->addWidget(pushButton_view);

        pushButton_import = new QPushButton(widget);
        pushButton_import->setObjectName(QString::fromUtf8("pushButton_import"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/ssp_gui/Resources/export.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_import->setIcon(icon4);

        horizontalLayout->addWidget(pushButton_import);

        pushButton_export = new QPushButton(widget);
        pushButton_export->setObjectName(QString::fromUtf8("pushButton_export"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/ssp_gui/Resources/export2.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_export->setIcon(icon5);

        horizontalLayout->addWidget(pushButton_export);

        pushButton_apply = new QPushButton(widget);
        pushButton_apply->setObjectName(QString::fromUtf8("pushButton_apply"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/ssp_gui/Resources/hujiaozhuanyi.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_apply->setIcon(icon6);

        horizontalLayout->addWidget(pushButton_apply);

        horizontalSpacer = new QSpacerItem(165, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        gridLayout_11->addWidget(widget, 0, 0, 1, 1);

        tabWidget = new QTabWidget(view_datawindow_edit);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab_1 = new QWidget();
        tab_1->setObjectName(QString::fromUtf8("tab_1"));
        gridLayout_5 = new QGridLayout(tab_1);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_5->setContentsMargins(4, 4, 4, 4);
        splitter_2 = new QSplitter(tab_1);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        splitter_2->setOrientation(Qt::Horizontal);
        groupBox = new QGroupBox(splitter_2);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(4, 4, 4, 4);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_ref_cls = new QLabel(groupBox);
        label_ref_cls->setObjectName(QString::fromUtf8("label_ref_cls"));

        horizontalLayout_3->addWidget(label_ref_cls);

        comboBox_ref_cls = new QComboBox(groupBox);
        comboBox_ref_cls->setObjectName(QString::fromUtf8("comboBox_ref_cls"));

        horizontalLayout_3->addWidget(comboBox_ref_cls);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_5);


        gridLayout->addLayout(horizontalLayout_3, 0, 0, 1, 1);

        tableWidget_ref1 = new QTableWidget(groupBox);
        tableWidget_ref1->setObjectName(QString::fromUtf8("tableWidget_ref1"));

        gridLayout->addWidget(tableWidget_ref1, 1, 0, 1, 1);

        splitter_2->addWidget(groupBox);
        splitter = new QSplitter(splitter_2);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Vertical);
        groupBox_2 = new QGroupBox(splitter);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_3 = new QGridLayout(groupBox_2);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(4, 4, 4, 4);
        textEdit_ref = new QTextEdit(groupBox_2);
        textEdit_ref->setObjectName(QString::fromUtf8("textEdit_ref"));

        gridLayout_3->addWidget(textEdit_ref, 1, 0, 1, 1);

        splitter->addWidget(groupBox_2);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget_2 = new QWidget(layoutWidget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        widget_2->setMaximumSize(QSize(16777215, 30));
        gridLayout_2 = new QGridLayout(widget_2);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        pushButton_ref2_add = new QPushButton(widget_2);
        pushButton_ref2_add->setObjectName(QString::fromUtf8("pushButton_ref2_add"));
        pushButton_ref2_add->setMaximumSize(QSize(16777215, 24));
        pushButton_ref2_add->setIcon(icon);

        gridLayout_2->addWidget(pushButton_ref2_add, 0, 0, 1, 1);

        pushButton_ref2_del = new QPushButton(widget_2);
        pushButton_ref2_del->setObjectName(QString::fromUtf8("pushButton_ref2_del"));
        pushButton_ref2_del->setMaximumSize(QSize(16777215, 24));
        pushButton_ref2_del->setIcon(icon2);

        gridLayout_2->addWidget(pushButton_ref2_del, 0, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(76, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_2, 0, 2, 1, 1);


        verticalLayout->addWidget(widget_2);

        groupBox_3 = new QGroupBox(layoutWidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        gridLayout_4 = new QGridLayout(groupBox_3);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(4, 4, 4, 4);
        tableWidget_ref2 = new QTableWidget(groupBox_3);
        tableWidget_ref2->setObjectName(QString::fromUtf8("tableWidget_ref2"));

        gridLayout_4->addWidget(tableWidget_ref2, 0, 0, 1, 1);


        verticalLayout->addWidget(groupBox_3);

        splitter->addWidget(layoutWidget);
        splitter_2->addWidget(splitter);

        gridLayout_5->addWidget(splitter_2, 0, 0, 1, 1);

        tabWidget->addTab(tab_1, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        gridLayout_13 = new QGridLayout(tab_2);
        gridLayout_13->setSpacing(6);
        gridLayout_13->setContentsMargins(11, 11, 11, 11);
        gridLayout_13->setObjectName(QString::fromUtf8("gridLayout_13"));
        gridLayout_13->setContentsMargins(4, 4, 4, 4);
        splitter_4 = new QSplitter(tab_2);
        splitter_4->setObjectName(QString::fromUtf8("splitter_4"));
        splitter_4->setOrientation(Qt::Horizontal);
        groupBox_4 = new QGroupBox(splitter_4);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        sizePolicy.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy);
        gridLayout_7 = new QGridLayout(groupBox_4);
        gridLayout_7->setSpacing(6);
        gridLayout_7->setContentsMargins(11, 11, 11, 11);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        gridLayout_7->setContentsMargins(4, 4, 4, 4);
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_data_cls = new QLabel(groupBox_4);
        label_data_cls->setObjectName(QString::fromUtf8("label_data_cls"));

        horizontalLayout_4->addWidget(label_data_cls);

        comboBox_data_cls = new QComboBox(groupBox_4);
        comboBox_data_cls->setObjectName(QString::fromUtf8("comboBox_data_cls"));

        horizontalLayout_4->addWidget(comboBox_data_cls);

        checkBox_useCls = new QCheckBox(groupBox_4);
        checkBox_useCls->setObjectName(QString::fromUtf8("checkBox_useCls"));

        horizontalLayout_4->addWidget(checkBox_useCls);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_6);


        gridLayout_7->addLayout(horizontalLayout_4, 0, 0, 1, 1);

        tableWidget_data1 = new QTableWidget(groupBox_4);
        tableWidget_data1->setObjectName(QString::fromUtf8("tableWidget_data1"));

        gridLayout_7->addWidget(tableWidget_data1, 1, 0, 1, 1);

        splitter_4->addWidget(groupBox_4);
        splitter_3 = new QSplitter(splitter_4);
        splitter_3->setObjectName(QString::fromUtf8("splitter_3"));
        splitter_3->setOrientation(Qt::Vertical);
        groupBox_5 = new QGroupBox(splitter_3);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        gridLayout_6 = new QGridLayout(groupBox_5);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        gridLayout_6->setContentsMargins(4, 4, 4, 4);
        textEdit_data = new QTextEdit(groupBox_5);
        textEdit_data->setObjectName(QString::fromUtf8("textEdit_data"));

        gridLayout_6->addWidget(textEdit_data, 0, 0, 1, 1);

        splitter_3->addWidget(groupBox_5);
        tabWidget_2 = new QTabWidget(splitter_3);
        tabWidget_2->setObjectName(QString::fromUtf8("tabWidget_2"));
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        gridLayout_9 = new QGridLayout(tab_4);
        gridLayout_9->setSpacing(6);
        gridLayout_9->setContentsMargins(11, 11, 11, 11);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        gridLayout_9->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(4, 4, 4, 4);
        widget_3 = new QWidget(tab_4);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        widget_3->setMaximumSize(QSize(16777215, 30));
        gridLayout_10 = new QGridLayout(widget_3);
        gridLayout_10->setSpacing(6);
        gridLayout_10->setContentsMargins(11, 11, 11, 11);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        gridLayout_10->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_3 = new QSpacerItem(132, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_10->addItem(horizontalSpacer_3, 0, 6, 1, 1);

        pushButton_newcol = new QPushButton(widget_3);
        pushButton_newcol->setObjectName(QString::fromUtf8("pushButton_newcol"));
        pushButton_newcol->setMaximumSize(QSize(16777215, 24));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165925152.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_newcol->setIcon(icon7);

        gridLayout_10->addWidget(pushButton_newcol, 0, 3, 1, 1);

        pushButton_data2_add = new QPushButton(widget_3);
        pushButton_data2_add->setObjectName(QString::fromUtf8("pushButton_data2_add"));
        pushButton_data2_add->setMaximumSize(QSize(16777215, 24));
        pushButton_data2_add->setIcon(icon);

        gridLayout_10->addWidget(pushButton_data2_add, 0, 4, 1, 1);

        pushButton_data2_del = new QPushButton(widget_3);
        pushButton_data2_del->setObjectName(QString::fromUtf8("pushButton_data2_del"));
        pushButton_data2_del->setMaximumSize(QSize(16777215, 24));
        pushButton_data2_del->setIcon(icon2);

        gridLayout_10->addWidget(pushButton_data2_del, 0, 5, 1, 1);


        verticalLayout_3->addWidget(widget_3);

        groupBox_6 = new QGroupBox(tab_4);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        gridLayout_8 = new QGridLayout(groupBox_6);
        gridLayout_8->setSpacing(6);
        gridLayout_8->setContentsMargins(11, 11, 11, 11);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        gridLayout_8->setContentsMargins(4, 4, 4, 4);
        tableWidget_data2 = new QTableWidget(groupBox_6);
        tableWidget_data2->setObjectName(QString::fromUtf8("tableWidget_data2"));

        gridLayout_8->addWidget(tableWidget_data2, 0, 0, 1, 1);


        verticalLayout_3->addWidget(groupBox_6);


        gridLayout_9->addLayout(verticalLayout_3, 0, 0, 1, 1);

        tabWidget_2->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        tabWidget_2->addTab(tab_5, QString());
        splitter_3->addWidget(tabWidget_2);
        splitter_4->addWidget(splitter_3);

        gridLayout_13->addWidget(splitter_4, 0, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());

        gridLayout_11->addWidget(tabWidget, 1, 0, 1, 1);


        gridLayout_12->addLayout(gridLayout_11, 0, 0, 1, 1);


        retranslateUi(view_datawindow_edit);

        tabWidget->setCurrentIndex(1);
        tabWidget_2->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(view_datawindow_edit);
    } // setupUi

    void retranslateUi(QWidget *view_datawindow_edit)
    {
        view_datawindow_edit->setWindowTitle(QApplication::translate("view_datawindow_edit", "view_datawindow_edit", 0, QApplication::UnicodeUTF8));
        pushButton_add->setText(QApplication::translate("view_datawindow_edit", "\345\242\236\345\212\240(&A)", 0, QApplication::UnicodeUTF8));
        pushButton_copy->setText(QApplication::translate("view_datawindow_edit", "\345\244\215\345\210\266(&C)", 0, QApplication::UnicodeUTF8));
        pushButton_del->setText(QApplication::translate("view_datawindow_edit", "\345\210\240\351\231\244(&D)", 0, QApplication::UnicodeUTF8));
        pushButton_view->setText(QApplication::translate("view_datawindow_edit", "\351\242\204\350\247\210(&V)", 0, QApplication::UnicodeUTF8));
        pushButton_import->setText(QApplication::translate("view_datawindow_edit", "\345\257\274\345\205\245(I)", 0, QApplication::UnicodeUTF8));
        pushButton_export->setText(QApplication::translate("view_datawindow_edit", "\345\257\274\345\207\272(O)", 0, QApplication::UnicodeUTF8));
        pushButton_apply->setText(QApplication::translate("view_datawindow_edit", "\345\272\224\347\224\250(&L)", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("view_datawindow_edit", "\344\270\273\350\241\250", 0, QApplication::UnicodeUTF8));
        label_ref_cls->setText(QApplication::translate("view_datawindow_edit", "\345\274\225\347\224\250\345\210\206\347\261\273", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("view_datawindow_edit", "SQL\350\257\255\345\217\245", 0, QApplication::UnicodeUTF8));
        pushButton_ref2_add->setText(QApplication::translate("view_datawindow_edit", "\345\242\236\345\212\240(&N)", 0, QApplication::UnicodeUTF8));
        pushButton_ref2_del->setText(QApplication::translate("view_datawindow_edit", "\345\210\240\351\231\244(&L)", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("view_datawindow_edit", "\345\257\271\345\272\224\350\241\214", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_1), QApplication::translate("view_datawindow_edit", "\345\274\225\347\224\250", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("view_datawindow_edit", "\344\270\273\350\241\250", 0, QApplication::UnicodeUTF8));
        label_data_cls->setText(QApplication::translate("view_datawindow_edit", "\346\225\260\346\215\256\351\233\206\345\210\206\347\261\273", 0, QApplication::UnicodeUTF8));
        checkBox_useCls->setText(QApplication::translate("view_datawindow_edit", "\345\210\206\347\261\273\350\277\207\346\273\244\345\274\225\347\224\250\351\200\211\346\213\251", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QApplication::translate("view_datawindow_edit", "SQL\350\257\255\345\217\245", 0, QApplication::UnicodeUTF8));
        pushButton_newcol->setText(QApplication::translate("view_datawindow_edit", "\345\210\235\345\247\213\345\210\227\347\224\237\346\210\220", 0, QApplication::UnicodeUTF8));
        pushButton_data2_add->setText(QApplication::translate("view_datawindow_edit", "\345\242\236\345\212\240(&N)", 0, QApplication::UnicodeUTF8));
        pushButton_data2_del->setText(QApplication::translate("view_datawindow_edit", "\345\210\240\351\231\244(&L)", 0, QApplication::UnicodeUTF8));
        groupBox_6->setTitle(QString());
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_4), QApplication::translate("view_datawindow_edit", "\345\210\227\345\256\232\344\271\211", 0, QApplication::UnicodeUTF8));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_5), QApplication::translate("view_datawindow_edit", "\346\235\241\344\273\266\345\256\232\344\271\211", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("view_datawindow_edit", "\346\225\260\346\215\256\351\233\206", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class view_datawindow_edit: public Ui_view_datawindow_edit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_DATAWINDOW_EDIT_H
