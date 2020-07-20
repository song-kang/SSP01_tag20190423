#ifndef LOGABOUT_H
#define LOGABOUT_H

#include <QDialog>
#include "ui_logabout.h"

class logabout : public QDialog
{
	Q_OBJECT

public:
	logabout(QWidget *parent = 0);
	~logabout();

private:
	Ui::logabout ui;
};

#endif // LOGABOUT_H
