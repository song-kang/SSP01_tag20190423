#ifndef PANEL_SIM_MMSSVR_H
#define PANEL_SIM_MMSSVR_H

#include <QWidget>
#include "ui_panel_sim_mmssvr.h"
#include "sim_manager.h"
#include "SQt.h"
#include "SimMmsServer.h"
class panel_sim_mmssvr : public QWidget
{
	Q_OBJECT

public:
	panel_sim_mmssvr(QWidget *parent = 0);
	~panel_sim_mmssvr();
	CSimManager *m_pSimManager;
	void Load();
public slots:
	void Save();
	void OnGlobalCtrlEnChg();
private:
	Ui::panel_sim_mmssvr ui;
};

#endif // PANEL_SIM_MMSSVR_H
