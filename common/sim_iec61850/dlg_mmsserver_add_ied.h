#ifndef DLG_MMSSERVER_ADD_IED_H
#define DLG_MMSSERVER_ADD_IED_H

#include <QDialog>
#include "ui_dlg_add_info_common.h"
#include "sim_config.h"
class dlg_mmsserver_add_ied : public QDialog
{
	Q_OBJECT

public:
	dlg_mmsserver_add_ied(CSimConfig *pCfg,QWidget *parent=0);
	~dlg_mmsserver_add_ied();
private:
	void initTable();
public:
	CSimConfig *m_pSimConfig;
	SPtrList<stuSclVtIed> m_SelectedIed;
private:
	Ui::dlg_add_info_common ui;


private slots:
	void OnOk();
	void OnCancel();
	void slotSelectInvBtn();
	void slotSelectAllBtn();
};

#endif // DLG_MMSSERVER_ADD_IED_H
