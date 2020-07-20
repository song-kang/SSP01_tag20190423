#ifndef SSP_ASSIST_H
#define SSP_ASSIST_H

#include <QtGui/QMainWindow>
#include "ui_ssp_assist.h"

class ssp_assist : public QMainWindow
{
	Q_OBJECT

public:
	ssp_assist(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ssp_assist();

private:
	Ui::ssp_assistClass ui;
};

#endif // SSP_ASSIST_H
