/**
 *
 * 文 件 名 : ssp_baseview.cpp
 * 创建日期 : 2015-7-8 11:07
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SSP图形人机对话程序（工作站）框架，功能窗口基础类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-8	邵凯田　创建文件
 *
 **/

#include "ssp_baseview.h"
#include "ssp_uiframe.h"
//#include "uiframe.h"
// class CUiFrame;
// extern CUiFrame *g_pUiFrame;

//////////////////////////////////////////////////////////////////////////
// 名    称:  CBaseView
// 作    者:  邵凯田
// 创建时间:  2015-7-8 11:11
// 描    述:  各功能视图基类，所有功能窗口需要从此派生
//////////////////////////////////////////////////////////////////////////
CBaseView::CBaseView(QWidget *parent) : QWidget(parent)
{
	if(SApplication::GetPtr() != NULL)
	{
		LOGDEBUG("SApplication::GetPtr()->m_pUserPtr=%p",SApplication::GetPtr()->m_pUserPtr);
	}
	if(SApplication::GetPtr() != NULL && (SPtrList<CBaseView>*)(SApplication::GetPtr()->m_pUserPtr) != NULL)
		((SPtrList<CBaseView>*)(SApplication::GetPtr()->m_pUserPtr))->append(this);
}

CBaseView::~CBaseView()
{
	if(SApplication::GetPtr() != NULL && (SPtrList<CBaseView>*)(SApplication::GetPtr()->m_pUserPtr) != NULL)
		((SPtrList<CBaseView>*)(SApplication::GetPtr()->m_pUserPtr))->remove(this);
}


////////////////////////////////////////////////////////////////////////
// 描    述:  当窗口显示后调用虚函数
// 作    者:  邵凯田
// 创建时间:  2015-7-8 11:11
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CBaseView::OnAftShow()
{
	this->setEnabled(true);
	if(m_sFunCode.length() == 0)
		return;
	if(!CheckThisUserGrant(m_sFunCode))
	{
		this->setEnabled(false);
	}
}

////////////////////////////////////////////////////////////////////////
// 描    述:  检查当前用户的指定功能点权限，未登录时要先登录
// 作    者:  邵凯田
// 创建时间:  2015-7-8 11:11
// 参数说明:  
// 返 回 值:  true表示允许，false表示拒绝
//////////////////////////////////////////////////////////////////////////
bool CBaseView::CheckThisUserGrant(SString sFunCode)
{
	if(!SSP_GUI->CheckFunPointAuth(sFunCode))
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  设置提示文本
// 作    者:  邵凯田
// 创建时间:  2015-7-8 11:11
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
void CBaseView::SetTip(SString tip)
{
	SSP_GUI->SetStatusTip(tip);
}


////////////////////////////////////////////////////////////////////////
// 描    述:  刷新全部打开的视图
// 作    者:  邵凯田
// 创建时间:  2015-7-8 11:11
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
void CBaseView::RefreshAllView()
{
	QWidget *p = parentWidget();
	if(p == NULL)
		return;
	p = p->parentWidget();
	if(p == NULL)
		return;
	CSspUiFrame *pFrame = (CSspUiFrame*)p;
	pFrame->RefreshAllView();
}

void CBaseView::slotGotoFunPoint()
{
	CSspUiFrame::GetPtr()->GotoView(CSspUiFrame::GetPtr()->m_sGotoFunPoint);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  跳转到指定的功能点窗口
// 作    者:  邵凯田
// 创建时间:  2015-8-5 14:35
// 参数说明:  @sFunKey为功能点名称
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CBaseView::GotoFunPoint(SString sFunKey)
{
	CSspUiFrame::GetPtr()->m_sGotoFunPoint = sFunKey;
	emit slotGotoFunPoint();
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  取当前登录用户
// 作    者:  邵凯田
// 创建时间:  2015-7-8 11:12
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
SString CBaseView::GetLoginUser()
{
	return SSP_GUI->GetOperUser();
}

////////////////////////////////////////////////////////////////////////
// 描    述:  取当前登录时间
// 作    者:  邵凯田
// 创建时间:  2015-7-8 11:12
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
time_t CBaseView::GetLoginTime()
{
	return SSP_GUI->GetLoginTime();
}

