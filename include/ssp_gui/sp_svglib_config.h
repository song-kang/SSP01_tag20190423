/**
 *
 * 文 件 名 : sp_svglib_config.h
 * 创建日期 : 2015-10-26 10:39
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SVG图元库配置文件类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-10-26	邵凯田　创建文件
 *
 **/

#ifndef __SP_SVGLIB_CONFIG_H__
#define __SP_SVGLIB_CONFIG_H__

#include "sp_config_mgr.h"
#include "ssp_gui_inc.h"

class SSP_GUI_EXPORT CSp_SvgLibConfig : public CConfigBase
{
public:
	CSp_SvgLibConfig();
	virtual ~CSp_SvgLibConfig();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  加载配置文件
	// 作    者:  邵凯田
	// 创建时间:  2015-8-1 13:04
	// 参数说明:  @sPathFile配置文件名
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Load(SString sPathFile);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从数据库加载图元库配置
	// 作    者:  邵凯田
	// 创建时间:  2015-11-9 13:28
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool LoadByDb();
};

#endif//__SP_SVGLIB_CONFIG_H__
