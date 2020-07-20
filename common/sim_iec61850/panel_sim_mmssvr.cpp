#include "panel_sim_mmssvr.h"
#include "GuiApplication.h"
panel_sim_mmssvr::panel_sim_mmssvr(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.btnSave,SIGNAL(clicked()),this,SLOT(Save()));
	connect(ui.chkGlobalCtrl_Select,SIGNAL(stateChanged(int)),this,SLOT(OnGlobalCtrlEnChg()));
	connect(ui.chkGlobalCtrl_Oper  ,SIGNAL(stateChanged(int)),this,SLOT(OnGlobalCtrlEnChg()));
	connect(ui.chkGlobalCtrl_Cancel,SIGNAL(stateChanged(int)),this,SLOT(OnGlobalCtrlEnChg()));
}

panel_sim_mmssvr::~panel_sim_mmssvr()
{
	//
}
void panel_sim_mmssvr::Load()
{
	ui.chkMmsUsed->setCheckState(m_pSimManager->m_pSimConfig->m_bOpenMmsServer?Qt::Checked:Qt::Unchecked);
	ui.chkSimIpAddr->setCheckState(m_pSimManager->m_pSimConfig->m_bMmsSvrSimIpAddr?Qt::Checked:Qt::Unchecked);
	ui.edtStTimes->setText(SString::toString(m_pSimManager->m_pSimConfig->m_iMmsStTimes).data());
	ui.edtMxTimes->setText(SString::toString(m_pSimManager->m_pSimConfig->m_iMmsMxTimes).data());
	ui.edtExtAttr->setText(m_pSimManager->m_pSimConfig->m_sMmsExtAttribute.data());
}

void panel_sim_mmssvr::Save()
{
	m_pSimManager->m_pSimConfig->m_bOpenMmsServer = ui.chkMmsUsed->checkState() == Qt::Checked;
	m_pSimManager->m_pSimConfig->m_bMmsSvrSimIpAddr = ui.chkSimIpAddr->checkState() == Qt::Checked;
	m_pSimManager->m_pSimConfig->m_iMmsStTimes = ui.edtStTimes->text().toInt();
	m_pSimManager->m_pSimConfig->m_iMmsMxTimes = ui.edtMxTimes->text().toInt();
	m_pSimManager->m_pSimConfig->m_sMmsExtAttribute = ui.edtExtAttr->text().toStdString().data();
	if(!m_pSimManager->m_pSimConfig->Save())
	{
		SQt::ShowErrorBox("´íÎó","ÅäÖÃ±£´æÊ±Ê§°Ü!");
		return;
	}
}

void panel_sim_mmssvr::OnGlobalCtrlEnChg()
{
	m_pSimManager->m_MmsServer.SetGlobalCtrl_En(ui.chkGlobalCtrl_Select->checkState()==Qt::Checked,
		ui.chkGlobalCtrl_Oper->checkState()==Qt::Checked,
		ui.chkGlobalCtrl_Cancel->checkState()==Qt::Checked);
}