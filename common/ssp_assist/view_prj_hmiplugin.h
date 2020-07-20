#ifndef VIEW_PRJ_HMIPLUGIN_H
#define VIEW_PRJ_HMIPLUGIN_H

#include "ssp_baseview.h"
#include "ui_view_prj_hmiplugin.h"

class view_prj_hmiplugin : public CBaseView
{
	Q_OBJECT

public:
	view_prj_hmiplugin(QWidget *parent = 0);
	~view_prj_hmiplugin();
public slots:
	void on_btnDir_clicked();
	void on_btnBuild_clicked();


private:
	Ui::view_prj_hmiplugin ui;
};

#endif // VIEW_PRJ_HMIPLUGIN_H
