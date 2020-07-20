#include "pwd_login.h"
pwd_login::pwd_login(int ncount)
	:m_nCount(ncount)
{
	m_nSucess=0;
}
pwd_login::~pwd_login(void)
{
}
pwd_login::LOGSTATE pwd_login::logInSys(SDatabaseOper* pdb,SString s_user,SString s_pwd)
{
	if (s_user=="")
	{
		m_slogInfo=QObject::tr("请输入用户名!").toStdString().data();
		return NOIN;
	}
	if (s_pwd=="")
	{
		m_slogInfo=QObject::tr("请输入密码!").toStdString().data();
		return NOIN;
	}
	SString sSql;
	SRecordset rs;
	if(pdb==NULL)
	{
		m_slogInfo=QObject::tr("数据连接错误!").toStdString().data();
		return NOIN;
	}
	sSql=SString::toFormat("select usr_code,pwd from t_ssp_user where usr_code='%s'",s_user.data());
	pdb->Retrieve(sSql,rs);
	if (rs.GetRows()==1&&rs.GetValue(0,1)==s_pwd)
	{
			m_nSucess++;
	}else
	{
		m_slogInfo=QObject::tr("登录失败!").toStdString().data();
		m_nSucess=0;
		return NOIN;
	}
	if (m_nSucess==m_nCount)
	{
		m_slogInfo=QObject::tr("登入成功").toStdString().data();
		m_nSucess=0;
		return ALLIN;
	}
	m_slogInfo=QObject::tr("单个已登入").toStdString().data();
	return ONEIN;
}