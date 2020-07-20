#ifndef VIEW_TEST1_H
#define VIEW_TEST1_H

#include "ssp_baseview.h"
#include "ui_view_test1.h"

class view_test1 : public CBaseView
{
	Q_OBJECT

public:
	view_test1(QWidget *parent = 0);
	~view_test1();

private:
	Ui::view_test1 ui;
};

#endif // VIEW_TEST1_H
