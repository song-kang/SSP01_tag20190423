#ifndef DLG_NEW_GSEOUT_H
#define DLG_NEW_GSEOUT_H

#include <QDialog>
#include "ui_dlg_new_gseout.h"
#include "sim_config.h"

class dlg_new_gseout : public QDialog
{
	Q_OBJECT

public:
	dlg_new_gseout(CSimConfig *pCfg,stuSimSubnetwork *pNet,QWidget *parent = 0);
	~dlg_new_gseout();
	SPtrList<stuSclVtIedGooseOut> m_SelectedGoose;
	CSimConfig *m_pSimConfig;
	stuSimSubnetwork *m_pThisNet;

public slots:
	void OnOk();
	void OnCancel();

private:
	Ui::dlg_new_gseout ui;
};

#endif // DLG_NEW_GSEOUT_H
