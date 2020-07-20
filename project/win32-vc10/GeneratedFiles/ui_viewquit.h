/********************************************************************************
** Form generated from reading UI file 'viewquit.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWQUIT_H
#define UI_VIEWQUIT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CViewQuit
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QPushButton *pushButtonResetHmi;
    QLabel *label;
    QPushButton *pushButtonQuitHmi;
    QLabel *label_2;
    QGroupBox *groupBox_2;
    QPushButton *pushButtonResetSys;
    QLabel *label_5;
    QPushButton *pushButtonHaltSys;
    QLabel *label_6;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *CViewQuit)
    {
        if (CViewQuit->objectName().isEmpty())
            CViewQuit->setObjectName(QString::fromUtf8("CViewQuit"));
        CViewQuit->resize(739, 409);
        gridLayout = new QGridLayout(CViewQuit);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox = new QGroupBox(CViewQuit);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setMinimumSize(QSize(0, 100));
        pushButtonResetHmi = new QPushButton(groupBox);
        pushButtonResetHmi->setObjectName(QString::fromUtf8("pushButtonResetHmi"));
        pushButtonResetHmi->setGeometry(QRect(20, 22, 91, 31));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/hujiaozhuanyi.gif"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonResetHmi->setIcon(icon);
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(120, 72, 551, 16));
        pushButtonQuitHmi = new QPushButton(groupBox);
        pushButtonQuitHmi->setObjectName(QString::fromUtf8("pushButtonQuitHmi"));
        pushButtonQuitHmi->setGeometry(QRect(20, 60, 91, 31));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/W95MBX01.ICO"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonQuitHmi->setIcon(icon1);
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(120, 34, 551, 16));

        gridLayout->addWidget(groupBox, 0, 0, 1, 1);

        groupBox_2 = new QGroupBox(CViewQuit);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setMinimumSize(QSize(0, 100));
        pushButtonResetSys = new QPushButton(groupBox_2);
        pushButtonResetSys->setObjectName(QString::fromUtf8("pushButtonResetSys"));
        pushButtonResetSys->setGeometry(QRect(20, 22, 91, 31));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/ssp_gui/Resources/KEY04.ICO"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonResetSys->setIcon(icon2);
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(120, 72, 551, 16));
        pushButtonHaltSys = new QPushButton(groupBox_2);
        pushButtonHaltSys->setObjectName(QString::fromUtf8("pushButtonHaltSys"));
        pushButtonHaltSys->setGeometry(QRect(20, 60, 91, 31));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/ssp_gui/Resources/MSGBOX01.ICO"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonHaltSys->setIcon(icon3);
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(120, 34, 551, 16));

        gridLayout->addWidget(groupBox_2, 1, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 176, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 2, 0, 1, 1);


        retranslateUi(CViewQuit);

        QMetaObject::connectSlotsByName(CViewQuit);
    } // setupUi

    void retranslateUi(QWidget *CViewQuit)
    {
        CViewQuit->setWindowTitle(QApplication::translate("CViewQuit", "CViewQuit", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("CViewQuit", "\347\225\214\351\235\242\347\250\213\345\272\217", 0, QApplication::UnicodeUTF8));
        pushButtonResetHmi->setText(QApplication::translate("CViewQuit", "\351\207\215\345\220\257(&R)", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
        pushButtonQuitHmi->setText(QApplication::translate("CViewQuit", "\351\200\200\345\207\272(&Q)", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("CViewQuit", "\346\263\250\357\274\232\351\207\215\345\220\257\345\221\275\344\273\244\345\260\206\344\275\277\347\225\214\351\235\242\347\250\213\345\272\217\351\200\200\345\207\272\357\274\214\345\271\266\347\253\213\345\215\263\345\206\215\346\254\241\345\220\257\345\212\250", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("CViewQuit", "\346\223\215\344\275\234\347\263\273\347\273\237", 0, QApplication::UnicodeUTF8));
        pushButtonResetSys->setText(QApplication::translate("CViewQuit", "\351\207\215\345\220\257(&B)", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("CViewQuit", "\346\263\250\357\274\232\345\205\263\346\234\272\346\223\215\344\275\234\345\260\206\344\270\255\346\255\242\346\211\200\346\234\211\347\250\213\345\272\217\345\271\266\345\205\263\351\227\255\346\223\215\344\275\234\347\263\273\347\273\237\357\274\214\345\205\263\346\234\272\345\220\216\351\234\200\350\246\201\346\211\213\345\212\250\345\205\263\351\227\255\350\243\205\347\275\256\347\224\265\346\272\220", 0, QApplication::UnicodeUTF8));
        pushButtonHaltSys->setText(QApplication::translate("CViewQuit", "\345\205\263\346\234\272(&H)", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("CViewQuit", "\346\263\250\357\274\232\346\223\215\344\275\234\347\263\273\347\273\237\351\207\215\345\220\257\345\221\275\344\273\244\345\260\206\344\270\255\346\255\242\346\211\200\346\234\211\347\250\213\345\272\217\351\200\200\345\207\272\346\223\215\344\275\234\347\263\273\347\273\237\357\274\214\345\271\266\347\253\213\345\215\263\345\206\215\346\254\241\345\220\257\345\212\250\346\223\215\344\275\234\347\263\273\347\273\237", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CViewQuit: public Ui_CViewQuit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWQUIT_H
