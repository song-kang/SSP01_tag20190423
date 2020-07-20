/**
 *
 * 文 件 名 : ssp_base.h
 * 创建日期 : 2015-11-4 16:32
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SSP软件平台基础类头文件
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-11-4	邵凯田　创建文件
 *
 **/

#ifndef __SSP_BASE_H__
#define __SSP_BASE_H__

#include "ssp_database.h"
#include "ssp_base_inc.h"

#define SSP_BASE ssp_base::GetPtr()

enum SSP_BASE_EXPORT eSspSysLogType
{
	SSP_SYSLOG_TYPE_LOGIN=1,
	SSP_SYSLOG_TYPE_LOGOUT,
	SSP_SYSLOG_TYPE_OPERLOGIN,
	SSP_SYSLOG_TYPE_OPERLOGOUT,
};

enum SSP_BASE_EXPORT eSspSysLogLevel
{
	SSP_SYSLOG_LEVEL_TIP=1,
	SSP_SYSLOG_LEVEL_WARN,
	SSP_SYSLOG_LEVEL_ERROR,
	SSP_SYSLOG_LEVEL_FAULT,
};

class SSP_BASE_EXPORT ssp_base
{
public:
	struct SSP_BASE_EXPORT stuSessionAttribute
	{
		SString m_sName;
		SString m_sValue;
	};
	ssp_base();
	virtual ~ssp_base();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取唯一实例指针
	// 作    者:  邵凯田
	// 创建时间:  2015-11-10 15:14
	// 参数说明:  void
	// 返 回 值:  ssp_base*
	//////////////////////////////////////////////////////////////////////////
	static ssp_base* GetPtr();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设备唯一实例指针（动态库中使用）
	// 作    者:  邵凯田
	// 创建时间:  2017-8-4 17:04
	// 参数说明:  @ptr表示实例指针
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	static void SetPtr(ssp_base* ptr);

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
	bool NewSysLog(int iLogType,int iLogLevel,const char* sLogText,int id1=0,int id2=0,int id3=0,int id4=0);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取全局配置值
	// 作    者:  邵凯田
	// 创建时间:  2016-2-22 9:48
	// 参数说明:  @sCfgKey表示全局唯一的配置关键字
	//         :  @sDefault表示缺省值
	// 返 回 值:  配置值，如无配置项时返回缺省值
	//////////////////////////////////////////////////////////////////////////
	SString GetGlobalConfig(SString sCfgKey,SString sDefault="");
	int GetGlobalConfigI(SString sCfgKey,int iDefault=0);
	float GetGlobalConfigF(SString sCfgKey,float fDefault=0);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置全局配置值
	// 作    者:  邵凯田
	// 创建时间:  2016-2-22 9:54
	// 参数说明:  @sCfgKey表示全局唯一的配置关键字
	//         :  @sValue表示值内容
	// 返 回 值:  true表示设置成功，false表示设置失败
	//////////////////////////////////////////////////////////////////////////
	bool SetGlobalConfig(SString sCfgKey,SString sValue);

	inline int GetSessionAttributeCount(){return m_SessionAttributes.count();};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取第idx个会话属性的名称
	// 作    者:  邵凯田
	// 创建时间:  2017-12-13 17:10
	// 参数说明:  @idx
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	SString GetIdSessionAttributeName(int idx);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取第idx个会话属性的名称和值
	// 作    者:  邵凯田
	// 创建时间:  2017-12-13 17:12
	// 参数说明:  @idx
	//         :  @name
	//         :  @value
	// 返 回 值:  true表示存在，false表示不存在
	//////////////////////////////////////////////////////////////////////////
	bool GetIdSessionAttribute(int idx,SString &name,SString &value);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  判断当然会话属性是否存在
	// 作    者:  邵凯田
	// 创建时间:  2015-11-10 11:30
	// 参数说明:  @sAttrName为会话名称
	// 返 回 值:  true表示属性存在，false表示属性不存在
	//////////////////////////////////////////////////////////////////////////
	bool IsSessionAttribute(const char* sAttrName);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取指定会话属性的值
	// 作    者:  邵凯田
	// 创建时间:  2015-11-10 11:31
	// 参数说明:  @sAttrName为会话属性名称
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	SString GetSessionAttributeValue(const char* sAttrName);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置指定的会话属性值，如果不存在该属性则新建属性
	// 作    者:  邵凯田
	// 创建时间:  2015-11-10 11:32
	// 参数说明:  @sAttrName为属性名称 
	//         :  @sValue为属性值
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetSessionAttributeValue(const char* sAttrName,SString sValue);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置会话属性集
	// 作    者:  邵凯田
	// 创建时间:  2015-11-10 15:31
	// 参数说明:  @sAttrs为属性集内容，格式如：name1=value1;name2=value2;
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetSessionAttributeValues(SString sAttrs);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  删除指定的会话属性项
	// 作    者:  邵凯田
	// 创建时间:  2015-11-10 14:54
	// 参数说明:  @sAttrName为待删除属性名称
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool RemoveSessionAttribute(const char* sAttrName);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取当前登录操作员的用户号
	// 作    者:  邵凯田
	// 创建时间:  2016-2-22 10:48
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	inline int GetLoginUserSn(){return m_iLoginUserSn;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取当前登录操作员的用户号
	// 作    者:  邵凯田
	// 创建时间:  2016-2-22 10:48
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	inline int GetOperUserSn(){return m_iOperUserSn;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取当前监护用户的编号
	// 作    者:  邵凯田
	// 创建时间:  2016-2-22 22:23
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	inline int GetGuarderUserSn(){return m_iGuarderUserSn;};

	int m_iLoginUserSn;			//登录人用户ID，0表示未登录
	int m_iOperUserSn;			//操作员用户ID，0表示无操作员
	int m_iGuarderUserSn;		//监护用户ID，0表示无监护人
	SPtrList<stuSessionAttribute> m_SessionAttributes;
};

#endif//__SSP_BASE_H__
