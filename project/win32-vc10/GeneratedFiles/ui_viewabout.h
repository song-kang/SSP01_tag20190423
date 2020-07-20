/********************************************************************************
** Form generated from reading UI file 'viewabout.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWABOUT_H
#define UI_VIEWABOUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CViewAbout
{
public:
    QGridLayout *gridLayout_2;
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *labelBuild;
    QLabel *labelDbVer;
    QLabel *labelCRC;
    QLabel *label;
    QSpacerItem *verticalSpacer_2;
    QLabel *label_2;
    QLabel *labelVer;
    QLabel *label_7;
    QLabel *labelDbInst;
    QLabel *labelDbInst_2;
    QLabel *labelMDbInst;
    QLabel *label_3;
    QLabel *label_4;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *CViewAbout)
    {
        if (CViewAbout->objectName().isEmpty())
            CViewAbout->setObjectName(QString::fromUtf8("CViewAbout"));
        CViewAbout->resize(630, 328);
        gridLayout_2 = new QGridLayout(CViewAbout);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        widget = new QWidget(CViewAbout);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setMinimumSize(QSize(100, 100));
        widget->setMaximumSize(QSize(100, 100));

        gridLayout_2->addWidget(widget, 0, 0, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        labelBuild = new QLabel(CViewAbout);
        labelBuild->setObjectName(QString::fromUtf8("labelBuild"));

        gridLayout->addWidget(labelBuild, 6, 0, 1, 1);

        labelDbVer = new QLabel(CViewAbout);
        labelDbVer->setObjectName(QString::fromUtf8("labelDbVer"));

        gridLayout->addWidget(labelDbVer, 7, 0, 1, 1);

        labelCRC = new QLabel(CViewAbout);
        labelCRC->setObjectName(QString::fromUtf8("labelCRC"));

        gridLayout->addWidget(labelCRC, 5, 0, 1, 1);

        label = new QLabel(CViewAbout);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 14, 0, 1, 1);

        label_2 = new QLabel(CViewAbout);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 3, 0, 1, 1);

        labelVer = new QLabel(CViewAbout);
        labelVer->setObjectName(QString::fromUtf8("labelVer"));

        gridLayout->addWidget(labelVer, 4, 0, 1, 1);

        label_7 = new QLabel(CViewAbout);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 10, 0, 1, 1);

        labelDbInst = new QLabel(CViewAbout);
        labelDbInst->setObjectName(QString::fromUtf8("labelDbInst"));

        gridLayout->addWidget(labelDbInst, 11, 0, 1, 1);

        labelDbInst_2 = new QLabel(CViewAbout);
        labelDbInst_2->setObjectName(QString::fromUtf8("labelDbInst_2"));

        gridLayout->addWidget(labelDbInst_2, 12, 0, 1, 1);

        labelMDbInst = new QLabel(CViewAbout);
        labelMDbInst->setObjectName(QString::fromUtf8("labelMDbInst"));

        gridLayout->addWidget(labelMDbInst, 13, 0, 1, 1);

        label_3 = new QLabel(CViewAbout);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        label_4 = new QLabel(CViewAbout);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 2, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 1, 2, 1);

        verticalSpacer = new QSpacerItem(20, 201, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 1, 0, 1, 1);


        retranslateUi(CViewAbout);

        QMetaObject::connectSlotsByName(CViewAbout);
    } // setupUi

    void retranslateUi(QWidget *CViewAbout)
    {
        CViewAbout->setWindowTitle(QApplication::translate("CViewAbout", "\345\205\263\344\272\216", 0, QApplication::UnicodeUTF8));
        labelBuild->setText(QApplication::translate("CViewAbout", "HMI\347\250\213\345\272\217\347\224\237\346\210\220\346\227\245\346\234\237\357\274\232", 0, QApplication::UnicodeUTF8));
        labelDbVer->setText(QApplication::translate("CViewAbout", "\346\225\260\346\215\256\345\272\223\347\211\210\346\234\254\357\274\232", 0, QApplication::UnicodeUTF8));
        labelCRC->setText(QApplication::translate("CViewAbout", "HMI\347\250\213\345\272\217\346\240\241\351\252\214\347\240\201\357\274\232", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("CViewAbout", "SSP01\347\263\273\347\273\237", 0, QApplication::UnicodeUTF8));
        label_2->setText(QString());
        labelVer->setText(QApplication::translate("CViewAbout", "HMI\347\250\213\345\272\217\347\211\210\346\234\254\357\274\232", 0, QApplication::UnicodeUTF8));
        label_7->setText(QString());
        labelDbInst->setText(QApplication::translate("CViewAbout", "\345\216\206\345\217\262\344\270\273\346\225\260\346\215\256\345\272\223\345\256\236\344\276\213\357\274\232", 0, QApplication::UnicodeUTF8));
        labelDbInst_2->setText(QApplication::translate("CViewAbout", "\345\216\206\345\217\262\345\244\207\346\225\260\346\215\256\345\272\223\345\256\236\344\276\213\357\274\232", 0, QApplication::UnicodeUTF8));
        labelMDbInst->setText(QApplication::translate("CViewAbout", "\345\206\205\345\255\230\346\225\260\346\215\256\345\272\223\345\256\236\344\276\213\357\274\232", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("CViewAbout", "\346\261\237\350\213\217\351\207\221\346\231\272\347\247\221\346\212\200\350\202\241\344\273\275\346\234\211\351\231\220\345\205\254\345\217\270", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("CViewAbout", "\345\215\227\344\272\254\346\202\240\351\230\224\347\224\265\346\260\224\347\247\221\346\212\200\346\234\211\351\231\220\345\205\254\345\217\270", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CViewAbout: public Ui_CViewAbout {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWABOUT_H
