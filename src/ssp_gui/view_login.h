/**
 *
 * 文 件 名 : view_login.h
 * 创建日期 : 2015-7-30 16:46
 * 作    者 : 褚冰冰
 * 修改日期 : 2015-7-30 16:46
 * 当前版本 : 1.0
 * 功能描述 : 登录界面
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-30 	褚冰冰　登录界面
 *
 **/
#ifndef VIEW_LOGIN_H
#define VIEW_LOGIN_H
#include "SApi.h"
#include "db/SDatabase.h"
#include "pwd_login.h"
#include "include_macdef.h"
#include <QDialog>

class view_login : public QDialog
{
	Q_OBJECT
//signals:
//	//////////////////////////////////////////////////////////////////////////
//	// 描    述:  登录信息信号
//	// 作    者:  褚冰冰
//	// 创建时间:  2015-7-30 16:44
//	// 参数说明:  @pwd_login::LOGSTATE 登录状态
//	//         :  @SString 登录信息
//	// 返 回 值:  
//	//////////////////////////////////////////////////////////////////////////
//	void loginSucessed(pwd_login::LOGSTATE,SString);
public:
	enum LOGINWINDOWTYPE
	{
		LOGIN_WND_TYPE_LOGIN,//用户登录
		LOGIN_WND_TYPE_CHECK//权限验证
	};
public:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  view_login构造
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 16:47
	// 参数说明:  @pdb 数据库操作指针
	//         :  @usertype 登录模式默认为单人
	//		   :  @parent 父窗口
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	view_login(LOGINWINDOWTYPE usertype=LOGIN_WND_TYPE_LOGIN,QWidget *parent=0);
	virtual ~view_login();
	bool exec(SString &sUser,SString &sMonitor)
	{
		QDialog::exec();

		if (m_bLogin)
		{
			sUser=m_sUser;
			sMonitor=m_sMonitor;
		}
		return m_bLogin;
	}
public slots:
	void onLoginSucessed(pwd_login::LOGSTATE state,SString sInfo);
private:
	QWidget * m_logWidget;//登录窗口
	bool m_bLogin;//是否登录成功
	SString m_sUser;//用户名
	SString m_sMonitor;//监护人名
	bool m_bSingle;//单人还是多人
};

#endif // VIEW_LOGIN_H
