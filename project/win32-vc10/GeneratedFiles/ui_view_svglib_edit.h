/********************************************************************************
** Form generated from reading UI file 'view_svglib_edit.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_SVGLIB_EDIT_H
#define UI_VIEW_SVGLIB_EDIT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_view_svglib_edit
{
public:
    QGridLayout *gridLayout_5;
    QWidget *widget;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_import;
    QPushButton *pushButton_export;
    QPushButton *pushButton_app;
    QSpacerItem *horizontalSpacer;
    QSplitter *splitter;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_3;
    QTableWidget *tableWidget_type;
    QWidget *widget_2;
    QGridLayout *gridLayout_3;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton_add;
    QPushButton *pushButton_edit;
    QPushButton *pushButton_del;
    QPushButton *pushButton_copy;
    QPushButton *pushButton_up;
    QPushButton *pushButton_down;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_4;
    QTableWidget *tableWidget_svg;
    QWidget *widget_3;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButton_add_svg;
    QPushButton *pushButton_del_svg;
    QPushButton *pushButton_edit_svg;
    QPushButton *pushButton_copy_svg;
    QPushButton *pushButton_up_svg;
    QPushButton *pushButton_down_svg;
    QPushButton *pushButton_othtype_svg;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *view_svglib_edit)
    {
        if (view_svglib_edit->objectName().isEmpty())
            view_svglib_edit->setObjectName(QString::fromUtf8("view_svglib_edit"));
        view_svglib_edit->resize(774, 543);
        gridLayout_5 = new QGridLayout(view_svglib_edit);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_5->setContentsMargins(4, 4, 4, 4);
        widget = new QWidget(view_svglib_edit);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setMaximumSize(QSize(16777215, 30));
        layoutWidget = new QWidget(widget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 0, 245, 31));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 6);
        pushButton_import = new QPushButton(layoutWidget);
        pushButton_import->setObjectName(QString::fromUtf8("pushButton_import"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/pic.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_import->setIcon(icon);

        horizontalLayout->addWidget(pushButton_import);

        pushButton_export = new QPushButton(layoutWidget);
        pushButton_export->setObjectName(QString::fromUtf8("pushButton_export"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/export2.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_export->setIcon(icon1);

        horizontalLayout->addWidget(pushButton_export);

        pushButton_app = new QPushButton(layoutWidget);
        pushButton_app->setObjectName(QString::fromUtf8("pushButton_app"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/ssp_gui/Resources/hujiaozhuanyi.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_app->setIcon(icon2);

        horizontalLayout->addWidget(pushButton_app);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        gridLayout_5->addWidget(widget, 0, 0, 1, 1);

        splitter = new QSplitter(view_svglib_edit);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        groupBox = new QGroupBox(splitter);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setMaximumSize(QSize(500, 16777215));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(4, 4, 4, 4);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setSizeConstraint(QLayout::SetNoConstraint);
        horizontalLayout_2->setContentsMargins(4, 4, 4, 4);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(4);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        tableWidget_type = new QTableWidget(groupBox);
        tableWidget_type->setObjectName(QString::fromUtf8("tableWidget_type"));
        tableWidget_type->setMaximumSize(QSize(16777215, 16777215));
        tableWidget_type->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_3->addWidget(tableWidget_type);


        horizontalLayout_2->addLayout(verticalLayout_3);

        widget_2 = new QWidget(groupBox);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        widget_2->setMaximumSize(QSize(80, 16777215));
        gridLayout_3 = new QGridLayout(widget_2);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setHorizontalSpacing(4);
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        pushButton_add = new QPushButton(widget_2);
        pushButton_add->setObjectName(QString::fromUtf8("pushButton_add"));
        pushButton_add->setMaximumSize(QSize(16777215, 24));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165925169.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_add->setIcon(icon3);

        verticalLayout->addWidget(pushButton_add);

        pushButton_edit = new QPushButton(widget_2);
        pushButton_edit->setObjectName(QString::fromUtf8("pushButton_edit"));
        pushButton_edit->setMaximumSize(QSize(16777215, 24));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165925590.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_edit->setIcon(icon4);

        verticalLayout->addWidget(pushButton_edit);

        pushButton_del = new QPushButton(widget_2);
        pushButton_del->setObjectName(QString::fromUtf8("pushButton_del"));
        pushButton_del->setMaximumSize(QSize(16777215, 24));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_del->setIcon(icon5);

        verticalLayout->addWidget(pushButton_del);

        pushButton_copy = new QPushButton(widget_2);
        pushButton_copy->setObjectName(QString::fromUtf8("pushButton_copy"));
        pushButton_copy->setMaximumSize(QSize(16777215, 24));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165933688.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_copy->setIcon(icon6);

        verticalLayout->addWidget(pushButton_copy);

        pushButton_up = new QPushButton(widget_2);
        pushButton_up->setObjectName(QString::fromUtf8("pushButton_up"));
        pushButton_up->setMaximumSize(QSize(16777215, 24));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/ssp_gui/Resources/up.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_up->setIcon(icon7);

        verticalLayout->addWidget(pushButton_up);

        pushButton_down = new QPushButton(widget_2);
        pushButton_down->setObjectName(QString::fromUtf8("pushButton_down"));
        pushButton_down->setMaximumSize(QSize(16777215, 24));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/ssp_gui/Resources/down.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_down->setIcon(icon8);

        verticalLayout->addWidget(pushButton_down);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        gridLayout_3->addLayout(verticalLayout, 0, 0, 1, 1);


        horizontalLayout_2->addWidget(widget_2);


        gridLayout->addLayout(horizontalLayout_2, 0, 0, 1, 1);

        splitter->addWidget(groupBox);
        groupBox_2 = new QGroupBox(splitter);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_4 = new QGridLayout(groupBox_2);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(4, 4, 4, 4);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(4, 4, 4, 4);
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(4);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        tableWidget_svg = new QTableWidget(groupBox_2);
        tableWidget_svg->setObjectName(QString::fromUtf8("tableWidget_svg"));
        tableWidget_svg->horizontalHeader()->setStretchLastSection(true);
        tableWidget_svg->verticalHeader()->setStretchLastSection(false);

        verticalLayout_4->addWidget(tableWidget_svg);


        horizontalLayout_3->addLayout(verticalLayout_4);

        widget_3 = new QWidget(groupBox_2);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        widget_3->setMaximumSize(QSize(80, 16777215));
        gridLayout_2 = new QGridLayout(widget_3);
        gridLayout_2->setSpacing(4);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        pushButton_add_svg = new QPushButton(widget_3);
        pushButton_add_svg->setObjectName(QString::fromUtf8("pushButton_add_svg"));
        pushButton_add_svg->setMaximumSize(QSize(16777215, 24));
        pushButton_add_svg->setIcon(icon3);

        verticalLayout_2->addWidget(pushButton_add_svg);

        pushButton_del_svg = new QPushButton(widget_3);
        pushButton_del_svg->setObjectName(QString::fromUtf8("pushButton_del_svg"));
        pushButton_del_svg->setMaximumSize(QSize(16777215, 24));
        pushButton_del_svg->setIcon(icon5);

        verticalLayout_2->addWidget(pushButton_del_svg);

        pushButton_edit_svg = new QPushButton(widget_3);
        pushButton_edit_svg->setObjectName(QString::fromUtf8("pushButton_edit_svg"));
        pushButton_edit_svg->setMaximumSize(QSize(16777215, 24));
        pushButton_edit_svg->setIcon(icon4);

        verticalLayout_2->addWidget(pushButton_edit_svg);

        pushButton_copy_svg = new QPushButton(widget_3);
        pushButton_copy_svg->setObjectName(QString::fromUtf8("pushButton_copy_svg"));
        pushButton_copy_svg->setMaximumSize(QSize(16777215, 24));
        pushButton_copy_svg->setIcon(icon6);

        verticalLayout_2->addWidget(pushButton_copy_svg);

        pushButton_up_svg = new QPushButton(widget_3);
        pushButton_up_svg->setObjectName(QString::fromUtf8("pushButton_up_svg"));
        pushButton_up_svg->setMaximumSize(QSize(16777215, 24));
        pushButton_up_svg->setIcon(icon7);

        verticalLayout_2->addWidget(pushButton_up_svg);

        pushButton_down_svg = new QPushButton(widget_3);
        pushButton_down_svg->setObjectName(QString::fromUtf8("pushButton_down_svg"));
        pushButton_down_svg->setMaximumSize(QSize(16777215, 24));
        pushButton_down_svg->setIcon(icon8);

        verticalLayout_2->addWidget(pushButton_down_svg);

        pushButton_othtype_svg = new QPushButton(widget_3);
        pushButton_othtype_svg->setObjectName(QString::fromUtf8("pushButton_othtype_svg"));
        pushButton_othtype_svg->setMaximumSize(QSize(16777215, 24));
        pushButton_othtype_svg->setIcon(icon1);

        verticalLayout_2->addWidget(pushButton_othtype_svg);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);


        gridLayout_2->addLayout(verticalLayout_2, 0, 0, 1, 1);


        horizontalLayout_3->addWidget(widget_3);


        gridLayout_4->addLayout(horizontalLayout_3, 0, 0, 1, 1);

        splitter->addWidget(groupBox_2);

        gridLayout_5->addWidget(splitter, 1, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(1, 20, QSizePolicy::Ignored, QSizePolicy::Minimum);

        gridLayout_5->addItem(horizontalSpacer_2, 1, 1, 1, 1);

        splitter->raise();
        widget->raise();

        retranslateUi(view_svglib_edit);

        QMetaObject::connectSlotsByName(view_svglib_edit);
    } // setupUi

    void retranslateUi(QWidget *view_svglib_edit)
    {
        view_svglib_edit->setWindowTitle(QApplication::translate("view_svglib_edit", "view_svglib_edit", 0, QApplication::UnicodeUTF8));
        pushButton_import->setText(QApplication::translate("view_svglib_edit", "\345\257\274\345\205\245(I)", 0, QApplication::UnicodeUTF8));
        pushButton_export->setText(QApplication::translate("view_svglib_edit", "\345\257\274\345\207\272(O)", 0, QApplication::UnicodeUTF8));
        pushButton_app->setText(QApplication::translate("view_svglib_edit", "\345\272\224\347\224\250(&P)", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("view_svglib_edit", "\345\233\276\345\205\203\347\261\273\345\236\213", 0, QApplication::UnicodeUTF8));
        pushButton_add->setText(QApplication::translate("view_svglib_edit", "\345\242\236\345\212\240(&N)", 0, QApplication::UnicodeUTF8));
        pushButton_edit->setText(QApplication::translate("view_svglib_edit", "\347\274\226\350\276\221(&T)", 0, QApplication::UnicodeUTF8));
        pushButton_del->setText(QApplication::translate("view_svglib_edit", "\345\210\240\351\231\244(&L)", 0, QApplication::UnicodeUTF8));
        pushButton_copy->setText(QApplication::translate("view_svglib_edit", "\345\244\215\345\210\266(&Y)", 0, QApplication::UnicodeUTF8));
        pushButton_up->setText(QApplication::translate("view_svglib_edit", "\344\270\212\347\247\273", 0, QApplication::UnicodeUTF8));
        pushButton_down->setText(QApplication::translate("view_svglib_edit", "\344\270\213\347\247\273", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("view_svglib_edit", "\345\233\276\345\205\203\351\233\206", 0, QApplication::UnicodeUTF8));
        pushButton_add_svg->setText(QApplication::translate("view_svglib_edit", "\345\242\236\345\212\240(&A)", 0, QApplication::UnicodeUTF8));
        pushButton_del_svg->setText(QApplication::translate("view_svglib_edit", "\345\210\240\351\231\244(&D)", 0, QApplication::UnicodeUTF8));
        pushButton_edit_svg->setText(QApplication::translate("view_svglib_edit", "\347\274\226\350\276\221(&E)", 0, QApplication::UnicodeUTF8));
        pushButton_copy_svg->setText(QApplication::translate("view_svglib_edit", "\345\244\215\345\210\266(&C)", 0, QApplication::UnicodeUTF8));
        pushButton_up_svg->setText(QApplication::translate("view_svglib_edit", "\344\270\212\347\247\273", 0, QApplication::UnicodeUTF8));
        pushButton_down_svg->setText(QApplication::translate("view_svglib_edit", "\344\270\213\347\247\273", 0, QApplication::UnicodeUTF8));
        pushButton_othtype_svg->setText(QApplication::translate("view_svglib_edit", "\347\247\273\345\212\250", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class view_svglib_edit: public Ui_view_svglib_edit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_SVGLIB_EDIT_H
