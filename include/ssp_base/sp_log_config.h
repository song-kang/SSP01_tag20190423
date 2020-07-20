/**
 *
 * 文 件 名 : sp_log_config.h
 * 创建日期 : 2015-8-1 13:02
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 日志配置文件操作类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-8-1	邵凯田　创建文件
 *
 **/

#ifndef __SP_LOG_CONFIG_H__
#define __SP_LOG_CONFIG_H__

#include "sp_config_mgr.h"
#include "ssp_base_inc.h"

class SSP_BASE_EXPORT CLogConfig : public CConfigBase
{
public:
	CLogConfig();
	virtual ~CLogConfig();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  加载配置文件
	// 作    者:  邵凯田
	// 创建时间:  2015-8-1 13:04
	// 参数说明:  @sPathFile配置文件名
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Load(SString sPathFile);
};

#endif//__SP_LOG_CONFIG_H__
