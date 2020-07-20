#ifndef DLG_NEW_NET_H
#define DLG_NEW_NET_H

#include <QDialog>
#include "ui_dlg_new_net.h"
#include "SRawSocket.h"
#include "sim_config.h"

class dlg_new_net : public QDialog
{
	Q_OBJECT

public:
	dlg_new_net(CSimConfig *pCfg,QWidget *parent = 0);
	~dlg_new_net();
	SPtrList<SRawSocketRecv::stuPcapDevice> m_SelectedNetDev;
	CSimConfig *m_pSimConfig;

public slots:
	void OnOk();
	void OnCancel();

private:
	Ui::dlg_new_net ui;
};

#endif // DLG_NEW_NET_H
