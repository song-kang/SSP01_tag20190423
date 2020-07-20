/**
 *
 * 文 件 名 : ssp_gui.cpp
 * 创建日期 : 2015-7-8 11:02
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SSP图形人机对话程序（工作站）软件平台接口类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-8	邵凯田　创建文件
 *
 **/

#include "ssp_gui.h"
#include "qt/SQt.h"
#include "view_login.h"
#include "ssp_uiframe.h"
#include "view_mdb_svg.h"
#include "view_common.h"
#include "view_query.h"
#include "dlg_login.h"
#include "dlg_operlogin.h"
#include "ssp_uiframe_bluestyle.h"
#include "dlg_login_bluestyle.h"
#include "dlg_login_substation.h"

ssp_gui g_ssp_gui;

ssp_gui::ssp_gui()
{
	LOGERROR("new ssp_gui!!!!!!");
	m_plabStatusTip = NULL;
	m_GeneralActions.setAutoDelete(true);
	m_ViewFloat.setAutoDelete(true);
}

ssp_gui::~ssp_gui()
{
	m_ViewFloat.clear();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取当前类的静态实例，一个进程只有一个实例（平台自动实例）
// 作    者:  邵凯田
// 创建时间:  2015-8-3 16:07
// 参数说明:  void
// 返 回 值:  ssp_gui*
//////////////////////////////////////////////////////////////////////////
ssp_gui* ssp_gui::GetPtr()
{
	return &g_ssp_gui;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  加载平台图形化相关配置,图形工作站系统启动时一次性调用
// 作    者:  邵凯田
// 创建时间:  2015-11-22 17:14
// 参数说明:  @sGuiStyle表示图形样式,default:缺省
//         :  @sExtAttribute表示扩展属性
// 返 回 值:  true表示启动成功，false表示启动失败
//////////////////////////////////////////////////////////////////////////
bool ssp_gui::StartGui(SString sGuiStyle,SString sExtAttribute)
{
	S_UNUSED(sGuiStyle);
	S_UNUSED(sExtAttribute);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  执行登录操作，弹出登录窗口
// 作    者:  邵凯田
// 创建时间:  2015-8-3 16:13
// 参数说明:  void
// 返 回 值:  true表示登录成功，false表示登录失败
//////////////////////////////////////////////////////////////////////////
bool ssp_gui::Login()
{
	//if(m_sLoginUser.length() > 0)
		//return true;

	//dlg_login dlg;
	//dlg_login_new dlg;
	dlg_login_blueStyle dlg;
	if(dlg.exec() == QDialog::Rejected)
		return false;

	m_sLoginUser = dlg.m_sUser;
	SRecordset rs;
	DB->Retrieve(SString::toFormat("select usr_sn,name,login_timeout from t_ssp_user where usr_code='%s'",m_sLoginUser.data()),rs);
	m_iLoginUserSn = rs.GetValue(0,0).toInt();
	m_sLoginUserName = rs.GetValue(0,1);
	m_iTimeoutSecs =  rs.GetValue(0,2).toInt();
	m_dtLoginTime = SDateTime::currentDateTime();
	NewSysLog(SSP_SYSLOG_TYPE_LOGIN,SSP_SYSLOG_LEVEL_TIP,
		SString::toFormat("用户(%s:%s)在主机(%s)登录工作站HMI",m_sLoginUser.data(),m_sLoginUserName.data(),SSocket::GetLocalIp().data()).data());
	return true;
}

bool ssp_gui::LoginSubstation(bool forOper) {
	dlg_login_substation dlg(forOper);
	if(dlg.exec() == QDialog::Rejected)
		return false;

	if (forOper) {
		m_sLoginUser = dlg.m_sUser;
	}	
	else {
		m_sGuarderUser = dlg.m_sUser;
	}
	SRecordset rs;
	DB->Retrieve(SString::toFormat("select usr_sn,name,login_timeout from t_ssp_user where usr_code='%s'",m_sLoginUser.data()),rs);
	m_iLoginUserSn = rs.GetValue(0,0).toInt();
	m_sLoginUserName = rs.GetValue(0,1);
	if (forOper)
		m_iTimeoutSecs =  rs.GetValue(0,2).toInt();
	m_dtLoginTime = SDateTime::currentDateTime();
	/*
	NewSysLog(SSP_SYSLOG_TYPE_LOGIN,SSP_SYSLOG_LEVEL_TIP,
		SString::toFormat("用户(%s:%s)在主机(%s)登录工作站HMI",m_sLoginUser.data(),m_sLoginUserName.data(),SSocket::GetLocalIp().data()).data());
	*/
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  退出登录
// 作    者:  邵凯田
// 创建时间:  2015-8-3 16:18
// 参数说明:  @bOnlyGuarder为true表示仅退出监护人的登录
// 返 回 值:  true表示退出成功，false表示退出失败
//////////////////////////////////////////////////////////////////////////
void ssp_gui::Logout()
{
	OperatorLogout();
	if(m_iLoginUserSn != 0)
	{
		SString sTip;
		sTip.sprintf("用户(%s:%s)退出工作站HMI的登录",m_sLoginUser.data(),m_sLoginUserName.data());
		NewSysLog(SSP_SYSLOG_TYPE_OPERLOGOUT,SSP_SYSLOG_LEVEL_TIP,sTip.data());
	}
	m_iLoginUserSn = 0;
	m_sLoginUser = m_sLoginUserName = "";
}

void ssp_gui::LogoutSubstation()
{
	//OperatorLogout();
	if(m_iLoginUserSn != 0)
	{
		SString sTip;
		sTip.sprintf("用户(%s:%s)退出工作站HMI的登录",m_sLoginUser.data(),m_sLoginUserName.data());
		NewSysLog(SSP_SYSLOG_TYPE_OPERLOGOUT,SSP_SYSLOG_LEVEL_TIP,sTip.data());
	}
	m_iLoginUserSn = 0;
	m_sLoginUser = m_sLoginUserName = "";

	m_sGuarderUser = m_sGuarderUserName = "";
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  操作员方式登录,用于登录操作人及监护人
// 作    者:  邵凯田
// 创建时间:  2016-2-22 22:48
// 参数说明:  @sFunPointKey表示功能点类型
//         :  @sOperTip表示操作提示文本
//         :  @bDoubleMode表示是双用户登录（即是否含监护人）
// 返 回 值:  true表示权限已具备，false表示权限不具备
//////////////////////////////////////////////////////////////////////////
bool ssp_gui::OperatorLogin(SString sFunPointKey, SString sOperTip, bool bDoubleMode/*=false*/)
{
begin:
	if(m_sOperUser.length() > 0)
	{
		if(!CheckUserFunPointAuth(m_sOperUser,sFunPointKey))
		{
			SQt::ShowInfoBox(tr("提示"),tr("当前操作人不具备当前操作的权限!\r\n请更换用户!"));
			OperatorLogout();
			goto begin;
		}
		if(bDoubleMode && !CheckUserFunPointAuth(m_sGuarderUser,sFunPointKey))
		{
			SQt::ShowInfoBox(tr("提示"),tr("当前监护人不具备当前操作的权限!\r\n请更换用户!"));
			OperatorLogout();
			goto begin;
		}
	}

	{
		dlg_operlogin dlg(bDoubleMode,sOperTip);
		if(dlg.exec() == QDialog::Rejected)
			return false;
		m_sOperUser = dlg.m_sUser;
		m_sGuarderUser = dlg.m_sMonUser;
	}
	if(!CheckUserFunPointAuth(m_sOperUser,sFunPointKey))
	{
		SQt::ShowInfoBox(tr("提示"),tr("当前操作人不具备当前操作的权限!\r\n请更换用户!"));
		OperatorLogout();
		goto begin;
	}
	if(bDoubleMode && !CheckUserFunPointAuth(m_sGuarderUser,sFunPointKey))
	{
		SQt::ShowInfoBox(tr("提示"),tr("当前监护人不具备当前操作的权限!\r\n请更换用户!"));
		OperatorLogout();
		goto begin;
	}
	SRecordset rs;
	DB->Retrieve(SString::toFormat("select usr_sn,name from t_ssp_user where usr_code='%s'",m_sOperUser.data()),rs);
	m_iOperUserSn = rs.GetValue(0,0).toInt();
	m_sOperUserName = rs.GetValue(0,1);
	if(m_sGuarderUser.length() > 0)
	{
		DB->Retrieve(SString::toFormat("select usr_sn,name from t_ssp_user where usr_code='%s'",m_sGuarderUser.data()),rs);
		m_iGuarderUserSn = rs.GetValue(0,0).toInt();
		m_sGuarderUserName = rs.GetValue(0,1);
	}
	SString sTip;
	sTip.sprintf("操作人(%s:%s)在主机(%s)通过(%s:%s)功能点权限验证",m_sOperUser.data(),m_sOperUserName.data(),SSocket::GetLocalIp().data(),
		sFunPointKey.data(),DB->SelectInto(SString::toFormat("select name from t_ssp_fun_point where fun_key='%s'",sFunPointKey.data())).data());
	if(m_sGuarderUser.length() > 0)
		sTip += SString::toFormat(",监护人(%s:%s)",m_sGuarderUser.data(),m_sGuarderUserName.data());
	NewSysLog(SSP_SYSLOG_TYPE_OPERLOGIN,SSP_SYSLOG_LEVEL_TIP,sTip.data());

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  操作人及监护人退出登录
// 作    者:  邵凯田
// 创建时间:  2016-2-22 22:50
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void ssp_gui::OperatorLogout()
{
	//退出操作人和监护人的登录
	if(m_iOperUserSn != 0)
	{
		SString sTip;
		sTip.sprintf("操作人(%s:%s)退出功能点权限验证",m_sOperUser.data(),m_sOperUserName.data());
		if(m_iGuarderUserSn != 0)
			sTip += SString::toFormat(",监护人(%s:%s)",m_sGuarderUser.data(),m_sGuarderUserName.data());
		NewSysLog(SSP_SYSLOG_TYPE_OPERLOGOUT,SSP_SYSLOG_LEVEL_TIP,sTip.data());
	}
	m_iOperUserSn = m_iGuarderUserSn = 0;
	m_sOperUser = m_sOperUserName = "";
	m_sGuarderUser = m_sGuarderUserName = "";
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  检查当前用户是否具备指针功能点的权限，当无用户操作时弹出登录窗口提示用户登录
//            当需要双用户认证，且未登录双用户时应进行双用户登录提示
// 作    者:  邵凯田
// 创建时间:  2015-8-3 16:12
// 参数说明:  @sFunPointKey表示待验证权限的功能点名
//         :  @bDoubleMode表示是否为双用户模式，false表示单操作员登录，true表示操作员、监护人双用户登录
// 返 回 值:  true给具备权限，false表示不具备权限
//////////////////////////////////////////////////////////////////////////
bool ssp_gui::CheckFunPointAuth(SString sFunPointKey,bool bDoubleMode/*=false*/)
{
	S_UNUSED(bDoubleMode);
	if(!IsLogin())
	{
		if(!Login())
			return false;
	}
	if(!CheckUserFunPointAuth(m_sLoginUser,sFunPointKey))
	{
		SQt::ShowInfoBox(tr("提示"),SQt::qsprintf(tr("当前登录用户[%s-%s]不具备本功能的使用权限!"),m_sLoginUser.data(),m_sLoginUserName.data()));
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  检查指定用户是否具有指定功能点的权限
// 作    者:  邵凯田
// 创建时间:  2015-8-3 16:39
// 参数说明:  @sUserCode为用户代码
//         :  @sFunPointKey为功能点代码
// 返 回 值:  true表示具备权限,false表示不具备权限
//////////////////////////////////////////////////////////////////////////
bool ssp_gui::CheckUserFunPointAuth(SString sUserCode,SString sFunPointKey)
{
	SString sql;
	sql.sprintf("select auth from t_ssp_usergroup_auth where fun_key='%s' and grp_code in (select grp_code from t_ssp_user where usr_code='%s')",
		sFunPointKey.data(),sUserCode.data());
	int auth = DB->SelectIntoI(sql);
	if(auth == 1)
		return true;
	sql.sprintf("select auth from t_ssp_user_auth where fun_key='%s' and usr_sn in (select usr_sn from t_ssp_user where usr_code='%s')",
		sFunPointKey.data(),sUserCode.data());
	auth = DB->SelectIntoI(sql);
	if(auth == 1)
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取登录信息描述字符串，用于界面显示
// 作    者:  邵凯田
// 创建时间:  2015-8-3 16:04
// 参数说明:  void
// 返 回 值:  SString，包括登录人、登录时间、超时退出时间（如果超时时间不为0）
//////////////////////////////////////////////////////////////////////////
SString ssp_gui::GetLoginInfoStr()
{
	SString str;
	if(m_sLoginUser.length() == 0)
		return Q2S(tr("尚未登录! "));
	else
		str += Q2C(SQt::qsprintf(tr("登录用户:%s "),m_sLoginUserName.data()));
// 	if(m_sOperUser.length() != 0)
// 		str += Q2C(SQt::qsprintf(tr("操作人:%s "),m_sOperUserName.data()));
// 	if(m_sGuarderUser.length() != 0)
// 		str += Q2C(SQt::qsprintf(tr("监护人:%s "),m_sGuarderUserName.data()));
	str += m_dtLoginTime.toString("登录时间:yyyy-MM-dd hh:mm:ss ");
	if(m_iTimeoutSecs != 0)
	{
		int sec = m_iTimeoutSecs - m_dtLoginTime.secsTo(SDateTime::currentDateTime());
		
		if(sec > 3600*24)
			str += Q2C(SQt::qsprintf(tr("%d天后登录超时! "),sec / 3600*24));
		else if(sec > 3600)
			str += Q2C(SQt::qsprintf(tr("%d小时后登录超时! "),sec / 3600));
		else if(sec > 60)
			str += Q2C(SQt::qsprintf(tr("%d分钟后登录超时! "),sec / 60));
		else if(sec <= 0)
		{
			str += "即将自动注销!";
		}
		else
			str += Q2C(SQt::qsprintf(tr("%d秒钟后登录超时! "),sec));
	}
	return str;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  是否登录已超时，超时则由上层注销登录
// 作    者:  邵凯田
// 创建时间:  2015-8-4 12:23
// 参数说明:  void
// 返 回 值:  true表示超时，false表示未超时
//////////////////////////////////////////////////////////////////////////
bool ssp_gui::IsLoginTimeout()
{
	if(m_iTimeoutSecs == 0 || m_sLoginUser.length() == 0)
		return false;
	return (m_iTimeoutSecs - m_dtLoginTime.secsTo(SDateTime::currentDateTime())) <= 0;	
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  设置状态条的提示信息
// 作    者:  邵凯田
// 创建时间:  2015-11-10 8:50
// 参数说明:  @sText表示提示文本
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void ssp_gui::SetStatusTip(SString sText)
{
	if(m_plabStatusTip == NULL)
		return;
	m_plabStatusTip->setText(sText.data());
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  打开浮动的组态窗口
// 作    者:  邵凯田
// 创建时间:  2015-12-30 9:52
// 参数说明:  @wnd_sn窗口序号
//         :  @w表示窗口宽度
//         :  @h表示窗口高度
//         :  @alpha表示透明度（含alpha通道的32位色彩深度时才有效）
//         :  @title表示窗口标题
// 返 回 值:  <0表示失败,>0表示成功创建新窗口，=0表示显示已打开过的窗口并刷新
//////////////////////////////////////////////////////////////////////////
int ssp_gui::OpenFloatWnd(int wnd_sn,int w,int h,int alpha,SString title)
{
	SString sql;
	int wnd_type=DB->SelectIntoI(SString::toFormat("select wnd_type from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn));
	unsigned long pos=0;
	float falpha = ((float)alpha)/100.0f;
	if(falpha <= 0.0001)
		falpha = 1;
	else if(falpha < 0.1)
		falpha = 0.1;
	else if(falpha > 1)
		falpha = 1;
	CBaseView *pView = m_ViewFloat.FetchFirst(pos);
	while(pView)
	{
		if(pView->m_iWndSn == wnd_sn)
		{
			if(title.length() == 0)
				pView->setWindowTitle(title.data());
			pView->setWindowOpacity(falpha);
			pView->OnPreShow();
			pView->show();
			pView->OnAftShow();
			return 0;
		}
		pView = m_ViewFloat.FetchNext(pos);
	}

	if(wnd_type == 1)//1-	映射到内存数据表的SVG图形监视窗口
	{
		//指定的组态窗口存在
		//先从已创建的窗口中查找

		//找不到，则创建新的窗口
		sql.sprintf("select dataset,ds_cond,wnd_name from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn);
		CViewMdbSvg *pNew = new CViewMdbSvg(wnd_sn,NULL,true/*,&sExtAttr*/);
		pView = pNew;
		if(title.length() == 0)
			title = DB->SelectInto(sql,2);
		pNew->setWindowTitle(title.data());
		//pNew->m_pParentView = this;
		pNew->SetDatasetName(DB->SelectInto(sql));
		pNew->SetCondition(DB->SelectInto(sql,1));
	}
	else if(wnd_type == 2 || wnd_type == 4)//2-	映射到内存数据表的动态表格  4-	映射到历史数据表的数据窗口，查询窗口
	{
		bool bMdb = DB->SelectIntoI("select count(*) from t_ssp_data_window where name='%s' and dbtype=3")>0;
		sql.sprintf("select dataset,ds_cond from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn);
		SString ds = DB->SelectInto(sql);
		view_query *pNew = new view_query(NULL,ds,bMdb);
		pView = pNew;
		pNew->m_iWndSn = wnd_sn;
		if(title.length() == 0)
			title = DB->SelectInto(SString::toFormat("select dw_desc from t_ssp_data_window where name='%s' ",ds.data()));
		pNew->setWindowTitle(title.data());
	}
	else if(wnd_type == 3)//3-	映射到历史数据表的数据窗口，对应数据窗口配置中的某一数据集，可以配置若干过滤条件，可以分别指定查询/修改/添加/删除功能
	{
		sql.sprintf("select dataset,ds_cond from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn);
		SString ds = DB->SelectInto(sql);
		view_common *pNew = new view_common(NULL,ds);
		pView = pNew;
		pNew->m_iWndSn = wnd_sn;
		if(title.length() == 0)
			title = DB->SelectInto(SString::toFormat("select dw_desc from t_ssp_data_window where name='%s' ",ds.data()));
		pNew->setWindowTitle(title.data());
	}
	else
	{
		return -1;
	}

	//显示新窗口
	pView->setWindowOpacity(falpha);
	pView->OnPreShow();
	if(w == -1 && h == -1)
	{
		pView->showMaximized();
	}
	else
	{
#ifdef WIN32
		pView->setWindowFlags( Qt::Dialog | Qt::Tool/* | Qt::WindowStaysOnTopHint*/);
		pView->setWindowModality(Qt::ApplicationModal);
#else
		pView->setWindowFlags( Qt::Dialog );
#endif
// 		if(w <= 0)
// 			w = 300;
// 		if(h <= 0)
// 			h = 200;
		pView->resize(w,h);
		pView->show();
	}				
	pView->OnAftShow();
	pView->setFocus();
	m_ViewFloat.append(pView);

	return 1;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  打开对话框式的组态窗口
// 作    者:  邵凯田
// 创建时间:  2015-12-31 8:50
// 参数说明:  @wnd_sn窗口序号
//         :  @w表示窗口宽度
//         :  @h表示窗口高度
//         :  @alpha表示透明度（含alpha通道的32位色彩深度时才有效）
//         :  @title表示窗口标题
// 返 回 值:  <0表示失败,>0表示成功创建新窗口，=0表示显示已打开过的窗口并刷新
//////////////////////////////////////////////////////////////////////////
int ssp_gui::OpenDialogWnd(int wnd_sn,int w,int h,int alpha,SString title)
{
	SString sql;
	int wnd_type=DB->SelectIntoI(SString::toFormat("select wnd_type from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn));
	//unsigned long pos=0;
	float falpha = ((float)alpha)/100.0f;
	if(falpha <= 0.0001)
		falpha = 1;
	else if(falpha < 0.1)
		falpha = 0.1;
	else if(falpha > 1)
		falpha = 1;
	CBaseView *pView;
	QDialog dlg;
	if(wnd_type == 1)//1-	映射到内存数据表的SVG图形监视窗口
	{
		//指定的组态窗口存在
		//先从已创建的窗口中查找

		//找不到，则创建新的窗口
		sql.sprintf("select dataset,ds_cond,wnd_name from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn);
		CViewMdbSvg *pNew = new CViewMdbSvg(wnd_sn,&dlg,true/*,&sExtAttr*/);
		pView = pNew;
		if(title.length() == 0)
			title = DB->SelectInto(sql,2);
		pNew->setWindowTitle(title.data());
		//pNew->m_pParentView = this;
		pNew->SetDatasetName(DB->SelectInto(sql));
		pNew->SetCondition(DB->SelectInto(sql,1));
	}
	else if(wnd_type == 2 || wnd_type == 4)//2-	映射到内存数据表的动态表格  4-	映射到历史数据表的数据窗口，查询窗口
	{
		bool bMdb = DB->SelectIntoI("select count(*) from t_ssp_data_window where name='%s' and dbtype=3")>0;
		sql.sprintf("select dataset,ds_cond from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn);
		SString ds = DB->SelectInto(sql);
		view_query *pNew = new view_query(&dlg,ds,bMdb);
		pView = pNew;
		pNew->m_iWndSn = wnd_sn;
		if(title.length() == 0)
			title = DB->SelectInto(SString::toFormat("select dw_desc from t_ssp_data_window where name='%s' ",ds.data()));
		pNew->setWindowTitle(title.data());
	}
	else if(wnd_type == 3)//3-	映射到历史数据表的数据窗口，对应数据窗口配置中的某一数据集，可以配置若干过滤条件，可以分别指定查询/修改/添加/删除功能
	{
		sql.sprintf("select dataset,ds_cond from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn);
		SString ds = DB->SelectInto(sql);
		view_common *pNew = new view_common(&dlg,ds);
		pView = pNew;
		pNew->m_iWndSn = wnd_sn;
		if(title.length() == 0)
			title = DB->SelectInto(SString::toFormat("select dw_desc from t_ssp_data_window where name='%s' ",ds.data()));
		pNew->setWindowTitle(title.data());
	}
	else
	{
		return -1;
	}
	QHBoxLayout *horizontalLayout = new QHBoxLayout(&dlg);
	horizontalLayout->setSpacing(1);
	horizontalLayout->setContentsMargins(11, 11, 11, 11);
	horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
	horizontalLayout->setContentsMargins(1, 1, 1, 1);

	horizontalLayout->addWidget(pView);

	dlg.setWindowTitle(title.data());
	dlg.setWindowOpacity(falpha);
	//dlg.setWindowFlags(Qt::WindowMinMaxButtonsHint);
	if(w == -1 && h == -1)
		dlg.showMaximized();
	else
		dlg.resize(w,h);
	pView->OnPreShow();
	pView->OnAftShow();
#ifdef WIN32
	//dlg.setWindowFlags( Qt::Dialog | Qt::Tool | Qt::WindowStaysOnTopHint|Qt::WindowMinMaxButtonsHint);
	dlg.setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint|Qt::WindowCloseButtonHint);
#else
	//dlg.setWindowFlags( Qt::Dialog);
	dlg.setWindowFlags( Qt::Dialog  | Qt::WindowMinMaxButtonsHint|Qt::WindowCloseButtonHint);
#endif
	dlg.setWindowModality(Qt::ApplicationModal);
	dlg.exec();

	return 1;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  创建一个嵌入式窗口
// 作    者:  邵凯田
// 创建时间:  2017-7-26 10:33
// 参数说明:  @wnd_sn组态窗口序号
//         :  @parent为父窗口指针
//         :  @title表示窗口标题
// 返 回 值:  QWidget*， 表示新创建的子窗口指针，NULL表示创建失败
//////////////////////////////////////////////////////////////////////////
QWidget* ssp_gui::NewEmbeddedWnd(int wnd_sn,QWidget *parent,SString title)
{
	SString sql;
	int wnd_type=DB->SelectIntoI(SString::toFormat("select wnd_type from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn));
	CBaseView *pView;
	if(wnd_type == 1)//1-	映射到内存数据表的SVG图形监视窗口
	{
		//指定的组态窗口存在
		//先从已创建的窗口中查找

		//找不到，则创建新的窗口
		sql.sprintf("select dataset,ds_cond,wnd_name from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn);
		CViewMdbSvg *pNew = new CViewMdbSvg(wnd_sn,parent,true/*,&sExtAttr*/);
		pView = pNew;
		if(title.length() == 0)
			title = DB->SelectInto(sql,2);
		pNew->setWindowTitle(title.data());
		pNew->SetDatasetName(DB->SelectInto(sql));
		pNew->SetCondition(DB->SelectInto(sql,1));
	}
	else if(wnd_type == 2 || wnd_type == 4)//2-	映射到内存数据表的动态表格  4-	映射到历史数据表的数据窗口，查询窗口
	{
		bool bMdb = DB->SelectIntoI("select count(*) from t_ssp_data_window where name='%s' and dbtype=3")>0;
		sql.sprintf("select dataset,ds_cond from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn);
		SString ds = DB->SelectInto(sql);
		view_query *pNew = new view_query(parent,ds,bMdb);
		pView = pNew;
		pNew->m_iWndSn = wnd_sn;
		if(title.length() == 0)
			title = DB->SelectInto(SString::toFormat("select dw_desc from t_ssp_data_window where name='%s' ",ds.data()));
		pNew->setWindowTitle(title.data());
	}
	else if(wnd_type == 3)//3-	映射到历史数据表的数据窗口，对应数据窗口配置中的某一数据集，可以配置若干过滤条件，可以分别指定查询/修改/添加/删除功能
	{
		sql.sprintf("select dataset,ds_cond from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn);
		SString ds = DB->SelectInto(sql);
		view_common *pNew = new view_common(parent,ds);
		pView = pNew;
		pNew->m_iWndSn = wnd_sn;
		if(title.length() == 0)
			title = DB->SelectInto(SString::toFormat("select dw_desc from t_ssp_data_window where name='%s' ",ds.data()));
		pNew->setWindowTitle(title.data());
	}
	else
	{
		return NULL;
	}
	pView->OnPreShow();
	pView->OnAftShow();

	return (QWidget*)pView;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  创建一个新的功能点窗口
// 作    者:  邵凯田
// 创建时间:  2017-8-8 13:54
// 参数说明:  @sFunName为功能点名称
//         :  @parent为父窗口指针
//         :  @sExtAttr为扩展属性
// 返 回 值:  CBaseView*, NULL表示创建失败
//////////////////////////////////////////////////////////////////////////
CBaseView* ssp_gui::NewView(SString sFunName,QWidget *parent,SString sExtAttr/*=""*/)
{
	CFrameMenuL2 l2;
	l2.m_iType = 0;
	l2.m_sFunName = sFunName;
	CBaseView *pView = ((CSspUiFrame*)m_pSspUiFrame)->NewView(&l2,parent);
	if(pView == NULL)
		return NULL;
	if(sExtAttr.length() > 0)
		pView->SetExtAttribute(sExtAttr);
	return pView;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  执行通用动作
// 作    者:  邵凯田
// 创建时间:  2015-12-30 9:38
// 参数说明:  @act_sn为动作编号
// 返 回 值:  <0表示失败(-1000表示尚未注册)，=0表示动作取消，>0表示动作执行成功
//////////////////////////////////////////////////////////////////////////
int ssp_gui::RunGeneralAction(int act_sn)
{
	SString sql;
	sql.sprintf("select count(*) from t_ssp_general_action where act_sn=%d",act_sn);
	if(act_sn > 0 && DB->SelectIntoI(sql) > 0)
	{
		//执行指定的动作逻辑
		sql.sprintf("select act_type,act_name,act_desc,act_tip,act_sql,succ_tip,fail_tip from t_ssp_general_action where act_sn=%d",act_sn);
		SRecordset rs;
		DB->Retrieve(sql,rs);
		if(rs.GetRows() <= 0)
			return -1001;
		int act_type = rs.GetValue(0,0).toInt();
		SString name = rs.GetValue(0,1);
		SString desc = rs.GetValue(0,2);
		SString act_tip = rs.GetValue(0,3);
		SString act_sql = rs.GetValue(0,4);
		SString succ_tip = rs.GetValue(0,5);
		SString fail_tip = rs.GetValue(0,6);
		if(act_tip.length() > 0)
		{
			if(!SQt::ShowQuestionBox(desc.data(),act_tip.data(),NULL))
				return 0;
		}
		bool ret = false;
		if(act_type == 1 || act_type == 2)
		{
			//处理动态会话属性条件 {@SESSION:sub_no@}
			int p1,p2;
			SString sFldKey;
			p1 = act_sql.find("{@SESSION:");
			while(p1 >= 0)
			{
				p2 = act_sql.find("@}",p1);
				if(p2 < 0)
					break;
				sFldKey = act_sql.mid(p1+10,p2-p1-10);
				act_sql = act_sql.left(p1)+SSP_BASE->GetSessionAttributeValue(sFldKey.data())+act_sql.mid(p2+2);
				p1 = act_sql.find("{@SESSION:");
			}

			if(act_type == 1)
				ret = MDB->Execute(act_sql);
			else
				ret = DB->Execute(act_sql);
		}
		else if(act_type == 3)
		{
			int iret = SSP_GUI->RunRegistedCustomAction(name);
			if(iret < 0)
			{
				LOGERROR("通用动作(%s:%s)执行失败:ret=%d",name.data(),desc.data(),iret);
			}
			if(iret == 0)
				return 0;//cancel
			ret = iret > 0;
		}
		if(ret)
		{
			if(succ_tip.length() > 0)
				SQt::ShowInfoBox(desc.data(),succ_tip.data(),NULL);
		}
		else
		{
			if(fail_tip.length() > 0)
				SQt::ShowErrorBox(desc.data(),fail_tip.data(),NULL);
		}
		return ret;
	}
	return -1002;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  执行应用层已注册的自定义动作
// 作    者:  邵凯田
// 创建时间:  2015-11-21 15:30
// 参数说明:  @sActName为动作名称
// 返 回 值:  <0表示失败(-1000表示尚未注册)，=0表示动作取消，>0表示动作执行成功
//////////////////////////////////////////////////////////////////////////
int ssp_gui::RunRegistedCustomAction(SString sActName)
{
	SSP_GeneralAction p = SearchGeneranActionFun(sActName);
	if(p == NULL)
	{
		LOGERROR("指定的通用动作(%s)尚未注册!已注册函数%d个",sActName.data(),m_GeneralActions.count());
		unsigned long pos;
		stuGeneralActionParam *p0 = m_GeneralActions.FetchFirst(pos); 
		while(p0)
		{
			LOGERROR("已注册函数:%s -> %p",p0->sActName.data(),p0->pFun);
			p0 = m_GeneralActions.FetchNext(pos);
		}
		LOGERROR("指定的通用动作(%s)尚未注册!已注册函数%d个",sActName.data(),m_GeneralActions.count());
		return -1;
	}
	return p();
}
//////////////////////////////////////////////////////////////////////////
// 描    述:  注册通用动作函数入口
// 作    者:  邵凯田
// 创建时间:  2015-11-21 15:37
// 参数说明:  @sActName为全局唯一的动作关键字
//         :  @pFun为动作入口的静态回调函数
// 返 回 值:  true表示注册成功，false表示注册失败（如关键字已经被注册过）
//////////////////////////////////////////////////////////////////////////
bool ssp_gui::RegisterGeneralAction(SString sActName,SSP_GeneralAction pFun)
{
	SSP_GeneralAction p = SearchGeneranActionFun(sActName);
	if(p != NULL)
	{
		LOGERROR("注册通用动作函数已存在!%s -> %p",sActName.data(),pFun);
		return false;
	}
	LOGERROR("注册通用动作函数:%s -> %p, 已有%d个函数!",sActName.data(),pFun,m_GeneralActions.count());
	stuGeneralActionParam *pNew = new stuGeneralActionParam();
	pNew->sActName = sActName;
	pNew->pFun = pFun;
	m_GeneralActions.append(pNew);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  通过关键字搜索通用动作入口回调函数
// 作    者:  邵凯田
// 创建时间:  2015-11-21 15:41
// 参数说明:  @sActName为动作关键字
// 返 回 值:  SSP_GeneralAction,NULL表示指定动作尚未注册
//////////////////////////////////////////////////////////////////////////
SSP_GeneralAction ssp_gui::SearchGeneranActionFun(SString sActName)
{
	unsigned long pos;
	stuGeneralActionParam *p = m_GeneralActions.FetchFirst(pos);
	while(p)
	{
		if(p->sActName == sActName)
			return p->pFun;
		p = m_GeneralActions.FetchNext(pos);
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  取指定的SVG提示文本内容
// 作    者:  邵凯田
// 创建时间:  2015-12-8 14:50
// 参数说明:  @pSvgObj为图元对象指针
//         :  @sText为引用返回的文本内容
// 返 回 值:  true表示提取成功，false表示提取失败
//////////////////////////////////////////////////////////////////////////
bool ssp_gui::GetSvgObjectTipText(SSvgObject *pSvgObj,SString &sText)
{
	unsigned long pos=0;
	SSP_GetSvgObjectTipText* ppFun = m_GetSvgObjectTipTextFun.FetchFirst(pos);
	while(ppFun)
	{
		if((*ppFun)(pSvgObj,sText))
			return true;
		ppFun = m_GetSvgObjectTipTextFun.FetchNext(pos);
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  注册取指定的SVG提示文本内容的静态回调函数
// 作    者:  邵凯田
// 创建时间:  2015-12-8 14:56
// 参数说明:  @pFun
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void ssp_gui::RegisterGetSvgobjectTipCallback(SSP_GetSvgObjectTipText pFun)
{
	m_GetSvgObjectTipTextFun.append(pFun);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  向主界面框架的状态条右侧添加自定义WIDGET控件，应用层可以控制该控件的属性以及获取其消息
// 作    者:  邵凯田
// 创建时间:  2016-1-11 11:21
// 参数说明:  @pWidget
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void ssp_gui::AddStatusWidget(QWidget *pWidget)
{
	((CSspUiFrame*)m_pSspUiFrame)->AddStatusWidget(pWidget);
	//pWidget->setParent(((CSspUiFrame*)m_pSspUiFrame)->m_pWndBottom);
	//((CSspUiFrame*)m_pSspUiFrame)->m_pWndBottom->horizontalLayout->addWidget(pWidget);

	
	//pWidget->setParent(((CSspUiFrameBlueStyle*)m_pSspUiFrame)->returnBottomWidget());
	//((CSspUiFrameBlueStyle*)m_pSspUiFrame)->returnBottomLayout()->addWidget(pWidget);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  显示请稍候窗口
// 作    者:  邵凯田
// 创建时间:  2016-1-20 16:35
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
void ssp_gui::ShowWaiting()
{
	((CSspUiFrame*)m_pSspUiFrame)->ShowWaiting();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  关闭请稍候窗口
// 作    者:  邵凯田
// 创建时间:  2016-1-20 16:36
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
void ssp_gui::HideWaiting()
{
	((CSspUiFrame*)m_pSspUiFrame)->HideWaiting();
}

#include "view_mdb_svg.h"
//////////////////////////////////////////////////////////////////////////
// 描    述:  将最后一次点击的SVG组态窗口重定向到新的窗口号
// 作    者:  邵凯田
// 创建时间:  2016-3-1 18:19
// 参数说明:  @wnd_sn为组态窗口号
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void ssp_gui::JumpToSvgUiWnd(int wnd_sn)
{
	CViewMdbSvg::JumpWnd(wnd_sn);
}

