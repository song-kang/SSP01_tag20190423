#ifndef DLG_NEW_SMVOUT_H
#define DLG_NEW_SMVOUT_H

#include <QDialog>
#include "ui_dlg_new_smvout.h"
#include "sim_config.h"

class dlg_new_smvout : public QDialog
{
	Q_OBJECT

public:
	dlg_new_smvout(CSimConfig *pCfg,stuSimSubnetwork *pNet,QWidget *parent = 0);
	~dlg_new_smvout();
	SPtrList<stuSclVtIedSmvOut> m_SelectedSmv;
	CSimConfig *m_pSimConfig;
	stuSimSubnetwork *m_pThisNet;

public slots:
	void OnOk();
	void OnCancel();

private:
	Ui::dlg_new_smvout ui;
};

#endif // DLG_NEW_SMVOUT_H
