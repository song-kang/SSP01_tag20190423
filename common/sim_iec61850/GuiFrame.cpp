/**
 *
 * 文 件 名 : GuiFrame.cpp
 * 创建日期 : 2015-7-30 15:55
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 图形界面的显示框架派生类，用于框架窗口及所有子视窗的管理
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-30	邵凯田　创建文件
 *
 **/

#include "GuiFrame.h"
#include "view_home.h"
#include "view_sclimport.h"
#include "view_scl_edit.h"
#include "view_visual_ap.h"
#include "msgreplay.h"
CGuiFrame::CGuiFrame(QWidget *parent) : CSspUiFrame(parent)
{
	WaitForBackStart();
}


CGuiFrame::~CGuiFrame(void)
{
}

CBaseView* CGuiFrame::NewView(CFrameMenuL2 *pFunMenu,QWidget *parent)
{
	CBaseView *pView = CSspUiFrame::NewView(pFunMenu,parent);
	if(pView != NULL)
		return pView;
	//TODO:添加自己的视图窗口
	SString sFunName = pFunMenu->m_sFunName;
 	if(sFunName == "home")
 		return new view_home(parent);
	else if(sFunName == "mod.scd.import")
		return new view_sclimport(parent);
	else if(sFunName == "mod.scd.edit")
		return new view_scl_edit(parent);
	else if(sFunName == "vis.ap")
		return new view_visual_ap(parent);
	else if(sFunName == "pcap.replay")
		return new MsgReplay(parent);
	return pView;
}