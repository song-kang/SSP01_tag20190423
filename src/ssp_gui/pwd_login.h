/**
 *
 * 文 件 名 : pwd_login.h
 * 创建日期 : 2015-7-29 14:57
 * 作    者 : 褚冰冰
 * 修改日期 : 2015-7-29 14:57
 * 当前版本 : 1.0
 * 功能描述 : 密码验证接口封装类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-8	褚冰冰　创建文件
 *
 **/
#ifndef _PWD_LOGIN_H_
#define _PWD_LOGIN_H_
#include "db/SDatabase.h"
#include "include_macdef.h"
//////////////////////////////////////////////////////////////////////////
// 名    称:  pwd_login
// 作    者:  褚冰冰
// 创建时间:  2015-7-29 14:49
// 描    述:  密码验证接口类
//////////////////////////////////////////////////////////////////////////
class pwd_login
{
public:
	enum LOGSTATE
	{
		ONEIN,//单个登入
		ALLIN,//登录成功
		NOIN//登录失败
	};
public:
	pwd_login(int ncount=1);
	virtual ~pwd_login(void);
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  用户登录
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 16:37
	// 参数说明:  @pdb 数据库操作指针
	//         :  @s_user 用户名
	//         :  @pdb 密码
	// 返 回 值:  登录状态		ONEIN：单个登入 ALLIN：登录成功 NOIN：登录失败
	//////////////////////////////////////////////////////////////////////////
	virtual LOGSTATE logInSys(SDatabaseOper* pdb,SString s_user,SString s_pwd);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置登入个数
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 16:39
	// 参数说明:  @icount 登录个数
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void setLogCount(int icount)
	{
		m_nCount = icount;
	}
	SString getLogInfo(){return m_slogInfo;}
private:
	int m_nSucess;//成功验证成功
	int m_nCount;//登录个数
	SString m_slogInfo;//登录信息
};

#endif