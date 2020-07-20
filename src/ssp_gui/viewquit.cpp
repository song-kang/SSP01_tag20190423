/**
 *
 * 文 件 名 : viewquit.cpp
 * 创建日期 : 2013-9-22 13:22
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 退出系统的几个功能
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2013-9-22	邵凯田　创建文件
 *
 **/

#include "viewquit.h"
#include "SApplication.h"

CViewQuit::CViewQuit(QWidget *parent) : CBaseView(parent)
{
	ui.setupUi(this);
	//SetFunCode("sys.quit");

	connect(ui.pushButtonResetHmi,SIGNAL(clicked()),this,SLOT(OnResetHmi()));
	connect(ui.pushButtonQuitHmi,SIGNAL(clicked()),this,SLOT(OnQuitHmi()));
	connect(ui.pushButtonResetSys,SIGNAL(clicked()),this,SLOT(OnResetSys()));
	connect(ui.pushButtonHaltSys,SIGNAL(clicked()),this,SLOT(OnHaltSys()));


#ifdef WIN32
//	ui.pushButtonResetHmi->setEnabled(false);
	ui.pushButtonResetSys->setEnabled(false);
	ui.pushButtonHaltSys->setEnabled(false);
#endif
}

CViewQuit::~CViewQuit()
{

}

void CViewQuit::OnResetHmi()
{
	if(QMessageBox::question(NULL,"提示","您确定要重启HMI界面程序吗?",QDialogButtonBox::Yes,QDialogButtonBox::No) != QDialogButtonBox::Yes)
		return ;
	SApplication *pApp = SApplication::GetPtr();
	if(pApp != NULL)
	{
#ifdef WIN32
		SString sLautch = "launch.exe";
#else
		SString sLautch = "launch";
#endif
		SString sBinPath = pApp->GetBinPath()+pApp->GetModule();
		SApi::SystemExec(pApp->GetBinPath()+sLautch,sLautch,sBinPath);
	}
	if(pApp != NULL)
	{
		pApp->Stop();
		pApp->Quit();
	}
	exit(0);
	abort();
}

void CViewQuit::OnQuitHmi()
{
	if(QMessageBox::question(NULL,"提示","您确定要退出HMI界面程序吗?",QDialogButtonBox::Yes,QDialogButtonBox::No) != QDialogButtonBox::Yes)
		return ;
#ifndef WIN32
	SFile::touchFile("/hmi_stop.flg");
#endif
	SApplication *pApp = SApplication::GetPtr();
	if(pApp != NULL)
	{
		pApp->Stop();
		pApp->Quit();
	}
	exit(0);
	abort();
}

void CViewQuit::OnResetSys()
{
	if(QMessageBox::question(NULL,"提示","您确定要重启操作系统吗?",QDialogButtonBox::Yes,QDialogButtonBox::No) != QDialogButtonBox::Yes)
		return ;

	SApi::SystemReboot();
}

void CViewQuit::OnHaltSys()
{
	if(QMessageBox::question(NULL,"提示","您确定要关闭操作系统吗?",QDialogButtonBox::Yes,QDialogButtonBox::No) != QDialogButtonBox::Yes)
		return ;

	SApi::SystemHalt();
}




