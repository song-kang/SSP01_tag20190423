#ifndef SIM_IEC61850_H
#define SIM_IEC61850_H

#include <QtGui/QMainWindow>
#include "ui_sim_iec61850.h"

class sim_iec61850 : public QMainWindow
{
	Q_OBJECT

public:
	sim_iec61850(QWidget *parent = 0, Qt::WFlags flags = 0);
	~sim_iec61850();

private:
	Ui::sim_iec61850Class ui;
};

#endif // SIM_IEC61850_H
