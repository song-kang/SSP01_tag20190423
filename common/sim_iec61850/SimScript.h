/**
 *
 * 文 件 名 : SimScript.h
 * 创建日期 : 2017-3-27 10:52
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 模拟器脚本封装
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2017-3-27	邵凯田　创建文件
 *
 **/

#ifndef __NAM_SIM_SCRIPT_H__
#define __NAM_SIM_SCRIPT_H__

#include "ssp_base.h"
#include "SScriptParser.h"
#include <map>
#include "sim_manager.h"

//////////////////////////////////////////////////////////////////////////
// 名    称:  CEvalScriptParser
// 作    者:  邵凯田
// 创建时间:  2016-10-27 11:16
// 描    述:  运维评估脚本解翻译器类
//////////////////////////////////////////////////////////////////////////
class CSimScriptParser : public SScriptParser
{
public:

	CSimScriptParser();
	virtual ~CSimScriptParser();


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取扩展函数的数量
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 10:52
	// 参数说明:  void
	// 返 回 值:  返回有效扩展函数的数量
	//////////////////////////////////////////////////////////////////////////
	static int GetExtFunctionCount();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取指定序号的扩展函数指针
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 10:53
	// 参数说明:  @idx为序号，从0开始
	// 返 回 值:  stuExtFunction*, NULL表示不存在
	//////////////////////////////////////////////////////////////////////////
	static const stuExtFunction* GetExtFunctionByIdx(int idx);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取扩展变量的数量 
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 17:18
	// 参数说明:  void
	// 返 回 值:  返回有效扩展变量的数量
	//////////////////////////////////////////////////////////////////////////
	static int GetExtVariantCount();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取指定序号的扩展变量的指针
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 17:18
	// 参数说明:  @idx为序号，从0开始
	// 返 回 值:  stuExtVariant*， NULL表示不存在
	//////////////////////////////////////////////////////////////////////////
	static const stuExtVariant* GetExtVariantByIdx(int idx);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  触发GOOSE变位
	// 作    者:  邵凯田
	// 创建时间:  2017-03-27 16:30
	// 参数说明:  @pCbParam表示回调参数(注册时指定的)
	//         :  @ReturnVal表示返回值内容引用
	//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
	// 返 回 值:  true表示运行成功，false表示运行失败
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_GooseChg(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  触发SV变化
	// 作    者:  邵凯田
	// 创建时间:  2017-03-27 16:30
	// 参数说明:  @pCbParam表示回调参数(注册时指定的)
	//         :  @ReturnVal表示返回值内容引用
	//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
	// 返 回 值:  true表示运行成功，false表示运行失败
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_SvChg(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置叶子节点
	// 作    者:  邵凯田
	// 创建时间:  2017-03-27 16:30
	// 参数说明:  @pCbParam表示回调参数(注册时指定的)
	//         :  @ReturnVal表示返回值内容引用
	//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
	// 返 回 值:  true表示运行成功，false表示运行失败
	//////////////////////////////////////////////////////////////////////////
	static bool ExtFun_SetLeaf(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);
	static bool ExtFun_ReportProc(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);


public:
	void SetSimManagerPtr(CSimManager *p){m_pSimManager = p;};
protected:
	CSimManager *m_pSimManager;
};

#endif//__NAM_SIM_SCRIPT_H__
