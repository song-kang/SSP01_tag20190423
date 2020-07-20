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
#include "view_xml2ssaxex.h"
#include "view_mysql_mdbcfg.h"
#include "view_oracle_mdbcfg.h"
#include "view_prj_hmiplugin.h"

CGuiFrame::CGuiFrame(QWidget *parent) : CSspUiFrame(parent)
{
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
	if(sFunName == "code.xml2saxex")
		return new CViewXml2SSaxEx(parent);
	else if(sFunName == "code.mysql.helper")
		return new view_mysql_mdbcfg(parent);
	else if(sFunName == "code.oracle.helper")
		return new view_oracle_mdbcfg(parent);
	else if(sFunName == "prj.new.plugin")
		return new view_prj_hmiplugin(parent);
	return pView;
}