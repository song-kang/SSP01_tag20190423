/**
 *
 * 文 件 名 : ssp_base.cpp
 * 创建日期 : 2015-11-9 19:14
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SSP软件平台基础类头文件
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-11-9	邵凯田　创建文件
 *
 **/

#include "ssp_base.h"
#include "SDateTime.h"

ssp_base* g_p_ssp_base = NULL;
ssp_base::ssp_base()
{
	m_iOperUserSn = m_iLoginUserSn = m_iGuarderUserSn = 0;
	m_SessionAttributes.setAutoDelete(true);
	m_SessionAttributes.setShared(true);
	g_p_ssp_base = this;
}


ssp_base::~ssp_base()
{
	m_SessionAttributes.clear();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取唯一实例指针
// 作    者:  邵凯田
// 创建时间:  2015-11-10 15:14
// 参数说明:  void
// 返 回 值:  ssp_base*
//////////////////////////////////////////////////////////////////////////
ssp_base* ssp_base::GetPtr()
{
	if(g_p_ssp_base == NULL)
	{
		static ssp_base base;
		g_p_ssp_base = &base;
	}
	return g_p_ssp_base;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  设备唯一实例指针（动态库中使用）
// 作    者:  邵凯田
// 创建时间:  2017-8-4 17:04
// 参数说明:  @ptr表示实例指针
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void ssp_base::SetPtr(ssp_base* ptr)
{
	g_p_ssp_base = ptr;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  添加新的系统日志记录到历史数据库
// 作    者:  邵凯田
// 创建时间:  2015-11-9 19:45
// 参数说明:  @iLogType表示日志类型
//         :  @iLogLevel表示日志级别
//         :  @sLogText表示日志内容字符串
//         :  @id1~id4表示4个标识符
// 返 回 值:  true表示成功，false表示失败
//////////////////////////////////////////////////////////////////////////
bool ssp_base::NewSysLog(int iLogType,int iLogLevel,const char* sLogText,int id1,int id2,int id3,int id4)
{
	int soc,usec;
	SDateTime::getSystemTime(soc,usec);
	SString sql;
	int oper_sn = m_iOperUserSn;
	if(oper_sn == 0)
		oper_sn = m_iLoginUserSn;
	sql.sprintf("insert into t_ssp_syslog (soc,usec,usr_sn,monitor_sn,log_type,log_level,id1,id2,id3,id4,log_text) "
		"values(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s')",
		soc,usec,oper_sn,m_iGuarderUserSn,iLogType,iLogLevel,id1,id2,id3,id4,sLogText);
	if(!DB->Execute(sql))
	{
		LOGERROR("插入系统日志时失败!sql=%s",sql.data());
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取全局配置值
// 作    者:  邵凯田
// 创建时间:  2016-2-22 9:48
// 参数说明:  @sCfgKey表示全局唯一的配置关键字
//         :  @sDefault表示缺省值
// 返 回 值:  配置值，如无配置项时返回缺省值
//////////////////////////////////////////////////////////////////////////
SString ssp_base::GetGlobalConfig(SString sCfgKey,SString sDefault/*=""*/)
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select set_value from t_ssp_setup where set_key='%s'",sCfgKey.data());
	DB->Retrieve(sql,rs);
	if(rs.GetRows() <= 0)
		return sDefault;
	else
		return rs.GetValue(0,0);
}

int ssp_base::GetGlobalConfigI(SString sCfgKey,int iDefault/*=0*/)
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select set_value from t_ssp_setup where set_key='%s'",sCfgKey.data());
	DB->Retrieve(sql,rs);
	if(rs.GetRows() <= 0)
		return iDefault;
	else
		return rs.GetValue(0,0).toInt();
}

float ssp_base::GetGlobalConfigF(SString sCfgKey,float fDefault/*=0*/)
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select set_value from t_ssp_setup where set_key='%s'",sCfgKey.data());
	DB->Retrieve(sql,rs);
	if(rs.GetRows() <= 0)
		return fDefault;
	else
		return rs.GetValue(0,0).toFloat();
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  设置全局配置值
// 作    者:  邵凯田
// 创建时间:  2016-2-22 9:54
// 参数说明:  @sCfgKey表示全局唯一的配置关键字
//         :  @sValue表示值内容
// 返 回 值:  true表示设置成功，false表示设置失败
//////////////////////////////////////////////////////////////////////////
bool ssp_base::SetGlobalConfig(SString sCfgKey,SString sValue)
{
	SString sql;
	sql.sprintf("update t_ssp_setup set set_value='%s' where set_key='%s'",sValue.data(),sCfgKey.data());
	return DB->Execute(sql);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  判断当然会话属性是否存在
// 作    者:  邵凯田
// 创建时间:  2015-11-10 11:30
// 参数说明:  @sAttrName为会话名称
// 返 回 值:  true表示属性存在，false表示属性不存在
//////////////////////////////////////////////////////////////////////////
bool ssp_base::IsSessionAttribute(const char* sAttrName)
{
	unsigned long pos;
	SString sName = sAttrName;
	sName.toLower();
	stuSessionAttribute *p = m_SessionAttributes.FetchFirst(pos);
	while(p)
	{
		if(p->m_sName == sName)
			return true;
		p = m_SessionAttributes.FetchNext(pos);
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取第idx个会话属性的名称
// 作    者:  邵凯田
// 创建时间:  2017-12-13 17:10
// 参数说明:  @idx
// 返 回 值:  SString
//////////////////////////////////////////////////////////////////////////
SString ssp_base::GetIdSessionAttributeName(int idx)
{
	stuSessionAttribute *p = m_SessionAttributes[idx];
	if(p == NULL)
		return "";
	return p->m_sName;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  取第idx个会话属性的名称和值
// 作    者:  邵凯田
// 创建时间:  2017-12-13 17:12
// 参数说明:  @idx
//         :  @name
//         :  @value
// 返 回 值:  true表示存在，false表示不存在
//////////////////////////////////////////////////////////////////////////
bool ssp_base::GetIdSessionAttribute(int idx,SString &name,SString &value)
{
	stuSessionAttribute *p = m_SessionAttributes[idx];
	if(p == NULL)
		return false;
	name = p->m_sName;
	value = p->m_sValue;
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取指定会话属性的值
// 作    者:  邵凯田
// 创建时间:  2015-11-10 11:31
// 参数说明:  @sAttrName为会话属性名称
// 返 回 值:  SString
//////////////////////////////////////////////////////////////////////////
SString ssp_base::GetSessionAttributeValue(const char* sAttrName)
{
	unsigned long pos;
	SString sName = sAttrName;
	sName.toLower();
	stuSessionAttribute *p = m_SessionAttributes.FetchFirst(pos);
	while(p)
	{
		if(p->m_sName == sName)
			return p->m_sValue;
		p = m_SessionAttributes.FetchNext(pos);
	}
	return "";
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  设置指定的会话属性值，如果不存在该属性则新建属性
// 作    者:  邵凯田
// 创建时间:  2015-11-10 11:32
// 参数说明:  @sAttrName为属性名称 
//         :  @sValue为属性值
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void ssp_base::SetSessionAttributeValue(const char* sAttrName,SString sValue)
{
	unsigned long pos;
	SString sName = sAttrName;
	sName.toLower();
	stuSessionAttribute *p = m_SessionAttributes.FetchFirst(pos);
	while(p)
	{
		if(p->m_sName == sName)
		{
			p->m_sValue = sValue;
			return;
		}
		p = m_SessionAttributes.FetchNext(pos);
	}
	p = new stuSessionAttribute();
	p->m_sName = sName;
	p->m_sValue = sValue;
	m_SessionAttributes.append(p);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  设置会话属性集
// 作    者:  邵凯田
// 创建时间:  2015-11-10 15:31
// 参数说明:  @sAttrs为属性集内容，格式如：name1=value1;name2=value2;
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void ssp_base::SetSessionAttributeValues(SString sAttrs)
{
	SString sLine,name,value;
	int i,cnt = SString::GetAttributeCount(sAttrs,";");
	for(i=1;i<=cnt;i++)
	{
		sLine = SString::GetIdAttribute(i,sAttrs,";");
		name  = SString::GetIdAttribute(1,sLine,"=");
		//忽略不关心的属性，如：各大种SVG图元协带的属性
		if(name == "type" ||
			name == "ds" ||
			name == "to" ||
			name == "row" ||
			name == "col" ||
			name == "dime" ||
			name == "fc" ||
			name == "lc" ||
			name.left(3) == "fc_" ||
			name.left(3) == "lc_" ||
			name == "userdraw" ||
			name == "t" ||
			name == "tcor" ||
			name == "lcor" 
			)
			continue;
		value = SString::GetIdAttribute(2,sLine,"=");
		SetSessionAttributeValue(name.data(),value);
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  删除指定的会话属性项
// 作    者:  邵凯田
// 创建时间:  2015-11-10 14:54
// 参数说明:  @sAttrName为待删除属性名称
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool ssp_base::RemoveSessionAttribute(const char* sAttrName)
{
	unsigned long pos;
	SString sName = sAttrName;
	sName.toLower();
	stuSessionAttribute *p = m_SessionAttributes.FetchFirst(pos);
	while(p)
	{
		if(p->m_sName == sName)
		{
			m_SessionAttributes.remove(p);
			return true;
		}
		p = m_SessionAttributes.FetchNext(pos);
	}
	return false;
}


