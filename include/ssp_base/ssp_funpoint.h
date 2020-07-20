/**
 *
 * 文 件 名 : ssp_funpoint.h
 * 创建日期 : 2015-7-29 19:17
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 系统功能点定义
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-29	邵凯田　创建文件
 *
 **/

#ifndef __SSP_FUNCTION_POINT_H__
#define __SSP_FUNCTION_POINT_H__

#include "sp_config_mgr.h"
#include "SDatabaseOper.h"
#include "ssp_base_inc.h"

//取数据库配置实例
#define GET_FP_CFG ((CSsp_FunPointConfig*)CConfigMgr::GetConfig(SPCFG_FUN_POINT))

//////////////////////////////////////////////////////////////////////////
// 名    称:  CSsp_FunPointConfig
// 作    者:  邵凯田
// 创建时间:  2015-7-29 19:20
// 描    述:  功能点配置类
//////////////////////////////////////////////////////////////////////////
class SSP_BASE_EXPORT CSsp_FunPointConfig : public CConfigBase
{
public:
	CSsp_FunPointConfig();
	virtual ~CSsp_FunPointConfig();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  加载指定的配置文件,不能与LoadByDb混用
	// 作    者:  邵凯田
	// 创建时间:  2015-7-29 19:24
	// 参数说明:  @sPathFile为配置文件名
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Load(SString sPathFile);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从数据库加载功能点配置文件，不能与Load混用
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 16:37
	// 参数说明:  @pDbOper为数据库操作类
	//         :  @sUserCode为用户代码 
	// 返 回 值:  true表示加载成功，false表示加载失败
	//////////////////////////////////////////////////////////////////////////
	bool LoadByDb(SDatabaseOper *pDbOper,SString sUserCode);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取功能点的XML文本
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 16:39
	// 参数说明:  void
	// 返 回 值:  char*
	//////////////////////////////////////////////////////////////////////////
	inline char* GetFunPointXmlText(){return m_sFunPointXmlText.data();};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置功能点XML文本
	// 作    者:  邵凯田
	// 创建时间:  2015-12-20 10:20
	// 参数说明:  @sText
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetFunPointXmlText(SString &sText){m_sFunPointXmlText = sText;};

private:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  一条数据库功能点记录转为XML语法节点
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 16:53
	// 参数说明:  @pRecord为行
	//         :  @bLastLevel为true表示最后一级
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	SString DbNodeToXml(SRecord *pRecord,bool bLastLevel);

	SString m_sFunPointXmlText;//功能点XML配置内容
};

#endif//__SSP_FUNCTION_POINT_H__
