/**
 *
 * 文 件 名 : SimScript.cpp
 * 创建日期 : 2017-3-27 10:51
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

#include "SimScript.h"
#include <math.h>

//评分扩展函数
static const SScriptParser::stuExtFunction g_nam_eval_ext_fun[] = {
	{"$GooseChg",		CSimScriptParser::ExtFun_GooseChg,		CSimScriptParser::VAL_BOOL,	2,	"int appid,string vals",	"生成一次GOOSE变位，vals为ch_name1=val1;ch_name2=val2;"	},//val可以为true/false，也可以为now(表示当前时间)
	{"$SvChg",			CSimScriptParser::ExtFun_SvChg,			CSimScriptParser::VAL_BOOL,	4,	"int appid,int ch_idx,float rms,int q",	"修改SV值，ch_id为通道号(从1开始),rms为有效值,q为品质"	},
	{"$SetLeaf",		CSimScriptParser::ExtFun_SetLeaf,		CSimScriptParser::VAL_BOOL,	4,	"string ied_ld,string leaf_path,int vt,string val",	"修改LN值，ied_ldIED与LD名称，leaf_path为叶子节点路径,vt为值类型(1-bool,2-int,3-float,4-utctime)"	},
	{"$ReportProc",		CSimScriptParser::ExtFun_ReportProc,	CSimScriptParser::VAL_BOOL,	0,	"",	"执行一次MMS报告扫描操作"	},
// 
// 	{"$下一个历史状态量",	CSimScriptParser::ExtFun_NextHisState,		CSimScriptParser::VAL_BOOL,	0,	"",	"指向下一个历史状态量，$历史状态量统计()调用第一次使用，表示移动到第一个记录，返回类型bool(true表示记录存在，false表示记录不存在)"	},
// 
// 	{"$历史事件量统计",		CSimScriptParser::ExtFun_HisEventCnt,		CSimScriptParser::VAL_BOOL,	0,	"",	"提取当前厂站内所有未复归的事件量记录"	},
// 	{"$下一个历史事件量",	CSimScriptParser::ExtFun_NextHisEvent,		CSimScriptParser::VAL_BOOL,	0,	"",	"指向下一个历史事件量，$历史事件量统计()调用第一次使用，表示移动到第一个记录，返回类型bool(true表示记录存在，false表示记录不存在)"	},
// 	
// 	{"$当前未复归状态量",	CSimScriptParser::ExtFun_UnResumedStCnt,	CSimScriptParser::VAL_BOOL,	0,	"",	"提取当前厂站内所有未复归的状态量记录"	},
// 	{"$下一个未复归状态量",	CSimScriptParser::ExtFun_NextUnResumeSt,	CSimScriptParser::VAL_BOOL,	0,	"",	"指向下一个未复归状态量，$当前未复归状态量()调用第一次使用，表示移动到第一个记录，返回类型bool(true表示记录存在，false表示记录不存在)"	},
// 	{"$取当前冗余通信状态",	CSimScriptParser::ExtFun_ResunCommSt,		CSimScriptParser::VAL_INT,		0,	"",	"取当前未复归通信状态在冗余条件下的通信状态，0表示通信链路异常且无正常的冗余链路，1表示通信链路异常但有正常的冗余链路，2表示通信链路正常"	},
// 
// 	{"$扣当前设备分",		CSimScriptParser::ExtFun_CurrDevDeduct,	CSimScriptParser::VAL_VOID,	2,	"float 扣分值,string 可选扣分原因",	"对当前装置进行扣分"	},//参数：扣分值(float),string 扣分原因
// 	{"$扣当前厂站分",		CSimScriptParser::ExtFun_CurrSubDeduct,	CSimScriptParser::VAL_VOID,	2,	"float 扣分值,string 可选扣分原因",	"对当前厂站进行扣分"	},//参数：扣分值(float),string 扣分原因

	{NULL,						NULL,									CSimScriptParser::VAL_UNKNOWN,	0,	NULL,	NULL	}
};

//外部变量
static const SScriptParser::stuExtVariant g_nam_eval_ext_var[] = {
// 	{"@厂站编号",			SScriptParser::VAL_INT,			"",		""	},
// 	{"@设备编号",			SScriptParser::VAL_INT,			"",		""	},
// 	{"@设备名称",			SScriptParser::VAL_STRING,		"",		""	},
// 	{"@协议类型",			SScriptParser::VAL_INT,			"",		""	},
// 	{"@链接名称",			SScriptParser::VAL_STRING,		"",		""	},
// 	{"@信息名称",			SScriptParser::VAL_STRING,		"",		""	},
// 	{"@异常状态描述",		SScriptParser::VAL_STRING,		"",		""	},
// 	{"@历史状态量次数",		SScriptParser::VAL_INT,			"",		""	},
// 	{"@历史事件量次数",		SScriptParser::VAL_INT,			"",		""	},
// 	{"@未复归状态时长",		SScriptParser::VAL_INT,			"",		""	},

	{NULL,					SScriptParser::VAL_VOID,		NULL,	NULL}
};


CSimScriptParser::CSimScriptParser()
{
	m_pSimManager = NULL;
// 	m_vSubNo.SetName("@厂站编号");
// 	m_vDevSn.SetName("@设备编号");
// 	m_vDevName.SetName("@设备名称");
// 	m_vProtType.SetName("@协议类型");
// 	m_vClName.SetName("@链接名称");
// 	m_vInfName.SetName("@信息名称");
// 	m_vInfWarnDesc.SetName("@异常状态描述");
// 	m_vHisStAmount.SetName("@历史状态量次数");
// 	m_vHisEvtAmount.SetName("@历史事件量次数");
// 	m_vUnStSecs.SetName("@未复归状态时长");
// 	
// 	m_vSubNo.SetType(VAL_INT);
// 	m_vDevSn.SetType(VAL_INT);
// 	m_vDevName.SetType(VAL_STRING);
// 	m_vProtType.SetType(VAL_INT);
// 	m_vClName.SetType(VAL_STRING);
// 	m_vInfName.SetType(VAL_STRING);
// 	m_vInfWarnDesc.SetType(VAL_STRING);
// 	m_vHisStAmount.SetType(VAL_INT);
// 	m_vHisEvtAmount.SetType(VAL_INT);
// 	m_vUnStSecs.SetType(VAL_INT);
	
	int i,cnt;
	cnt = GetExtFunctionCount();
	for(i=0;i<cnt;i++)
	{
		const stuExtFunction *p = GetExtFunctionByIdx(i);
		this->RegisterFunction(p->sFunName,p->pFun,this);
	}
// 	RegisterInstVariant(&m_vSubNo);
// 	RegisterInstVariant(&m_vDevSn);
// 	RegisterInstVariant(&m_vDevName);
// 	RegisterInstVariant(&m_vProtType);
// 	RegisterInstVariant(&m_vClName);
// 	RegisterInstVariant(&m_vInfName);
// 	RegisterInstVariant(&m_vInfWarnDesc);
// 	RegisterInstVariant(&m_vHisStAmount);
// 	RegisterInstVariant(&m_vHisEvtAmount);
// 	RegisterInstVariant(&m_vUnStSecs);

}

CSimScriptParser::~CSimScriptParser()
{

}


//////////////////////////////////////////////////////////////////////////
// 描    述:  取扩展函数的数量
// 作    者:  邵凯田
// 创建时间:  2016-12-9 10:52
// 参数说明:  void
// 返 回 值:  返回有效扩展函数的数量
//////////////////////////////////////////////////////////////////////////
int CSimScriptParser::GetExtFunctionCount()
{
	return (sizeof(g_nam_eval_ext_fun)/sizeof(stuExtFunction))-1;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取指定序号的扩展函数指针
// 作    者:  邵凯田
// 创建时间:  2016-12-9 10:53
// 参数说明:  @idx为序号，从0开始
// 返 回 值:  stuExtFunction*, NULL表示不存在
//////////////////////////////////////////////////////////////////////////
const CSimScriptParser::stuExtFunction* CSimScriptParser::GetExtFunctionByIdx(int idx)
{
	if(idx < 0 || idx >= (sizeof(g_nam_eval_ext_fun)/sizeof(stuExtFunction))-1)
		return NULL;
	return &g_nam_eval_ext_fun[idx];
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  取扩展变量的数量 
// 作    者:  邵凯田
// 创建时间:  2016-12-9 17:18
// 参数说明:  void
// 返 回 值:  返回有效扩展变量的数量
//////////////////////////////////////////////////////////////////////////
int CSimScriptParser::GetExtVariantCount()
{
	return (sizeof(g_nam_eval_ext_var)/sizeof(stuExtVariant))-1;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取指定序号的扩展变量的指针
// 作    者:  邵凯田
// 创建时间:  2016-12-9 17:18
// 参数说明:  @idx为序号，从0开始
// 返 回 值:  stuExtVariant*， NULL表示不存在
//////////////////////////////////////////////////////////////////////////
const CSimScriptParser::stuExtVariant* CSimScriptParser::GetExtVariantByIdx(int idx)
{
	if(idx < 0 || idx >= (sizeof(g_nam_eval_ext_var)/sizeof(stuExtVariant))-1)
		return NULL;
	return &g_nam_eval_ext_var[idx];
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  触发GOOSE变位
// 作    者:  邵凯田
// 创建时间:  2017-03-27 16:30
// 参数说明:  @pCbParam表示回调参数(注册时指定的)
//         :  @ReturnVal表示返回值内容引用
//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
// 返 回 值:  true表示运行成功，false表示运行失败
//////////////////////////////////////////////////////////////////////////
bool CSimScriptParser::ExtFun_GooseChg(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
{
	CSimScriptParser *pThis = (CSimScriptParser*)pCbParam;
	if(pThis->m_pSimManager == 0 || pParamValues == NULL || pParamValues->count()<2)
	{
		ReturnVal.Set(VAL_BOOL,"0");
		return false;
	}
	bool ret = pThis->m_pSimManager->DoGooseChg(pParamValues->at(0)->GetValue().toInt(), pParamValues->at(1)->GetValue().data());
	ReturnVal.Set(VAL_BOOL,ret?"1":"0");
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  触发SV变化
// 作    者:  邵凯田
// 创建时间:  2017-03-27 16:30
// 参数说明:  @pCbParam表示回调参数(注册时指定的)
//         :  @ReturnVal表示返回值内容引用
//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
// 返 回 值:  true表示运行成功，false表示运行失败
//////////////////////////////////////////////////////////////////////////
bool CSimScriptParser::ExtFun_SvChg(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
{
	CSimScriptParser *pThis = (CSimScriptParser*)pCbParam;
	if(pThis->m_pSimManager == 0 || pParamValues == NULL || pParamValues->count()<4)
	{
		ReturnVal.Set(VAL_BOOL,"0");
		return false;
	}
	bool ret = pThis->m_pSimManager->DoSvChg(  pParamValues->at(0)->GetValue().toInt(), pParamValues->at(1)->GetValue().toInt(), 
										pParamValues->at(2)->GetValue().toFloat(), pParamValues->at(3)->GetValue().toInt() );
	ReturnVal.Set(VAL_BOOL,ret?"1":"0");
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  设置叶子节点
// 作    者:  邵凯田
// 创建时间:  2017-03-27 16:30
// 参数说明:  @pCbParam表示回调参数(注册时指定的)
//         :  @ReturnVal表示返回值内容引用
//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
// 返 回 值:  true表示运行成功，false表示运行失败
//////////////////////////////////////////////////////////////////////////
bool CSimScriptParser::ExtFun_SetLeaf(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
{
	CSimScriptParser *pThis = (CSimScriptParser*)pCbParam;
	if(pThis->m_pSimManager == 0 || pParamValues == NULL || pParamValues->count()<4)
	{
		ReturnVal.Set(VAL_BOOL,"0");
		return false;
	}
	bool ret = pThis->m_pSimManager->DoSetLeaf(  pParamValues->at(0)->GetValue().data(), pParamValues->at(1)->GetValue().data(), 
												 pParamValues->at(2)->GetValue().toInt(), pParamValues->at(3)->GetValue().data() );
	ReturnVal.Set(VAL_BOOL,ret?"1":"0");
	return true;
}

bool CSimScriptParser::ExtFun_ReportProc(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
{
	CSimScriptParser *pThis = (CSimScriptParser*)pCbParam;
	if(pThis->m_pSimManager == 0 )
	{
		ReturnVal.Set(VAL_BOOL,"0");
		return false;
	}
	pThis->m_pSimManager->m_MmsServer.NewReportProc();
	ReturnVal.Set(VAL_BOOL,"1");
	return true;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  当前厂站内历史状态量统计
// 作    者:  邵凯田
// 创建时间:  2016-12-8 15:40
// 参数说明:  @pCbParam表示回调参数(注册时指定的)
//         :  @ReturnVal表示返回值内容引用
//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
// 返 回 值:  true表示运行成功，false表示运行失败
//////////////////////////////////////////////////////////////////////////
// bool CSimScriptParser::ExtFun_HisStateCnt(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
// {
// 	CSimScriptParser *pThis = (CSimScriptParser*)pCbParam;
// 	SString sql;
// 	int soc_from;
// 	int soc_to = pThis->m_iBaseSoc;
// 	if(pThis->m_iEvalType == 1)
// 		soc_from = soc_to-300;
// 	else if(pThis->m_iEvalType == 2)
// 		soc_from = soc_to-3600;
// 	else if(pThis->m_iEvalType == 3)
// 		soc_from = soc_to-3600*24;
// 	else
// 	{
// 		ReturnVal.Set(VAL_BOOL,"0");
// 		return false;
// 	}
// 	SString sFrom = SDateTime::makeDateTime((time_t)soc_from).toString("yyyy-MM-dd hh:mm:ss");
// 	SString sTo = SDateTime::makeDateTime((time_t)soc_to).toString("yyyy-MM-dd hh:mm:ss");
// 	LOGDEBUG("正在检索厂站%d统计周期[%s ~ %s]内的历史状态量信息...",
// 		pThis->m_iSubNo,sFrom.data(),sTo.data());
// 	sql.sprintf("select v.dev_sn,v.dev_name,v.cl_name,v.inf_name,v.off_desc,h.cnt,v.prot_type from ( "
// 		"select stat_id,count(*)+sum(repeat_cnt) as cnt from t_nam_his_element_state "
// 		"where sub_no=%d and val=0 and rectime>=%s and  rectime<%s "
// 		"group by stat_id) h "
// 		"left join v_nam_real_state v on h.stat_id=v.stat_id",
// 		pThis->m_iSubNo,pThis->m_pEvalMgr->m_db_multi_sql.String2Date(sFrom).data(),pThis->m_pEvalMgr->m_db_multi_sql.String2Date(sTo).data());
// 	DB->Retrieve(sql,pThis->m_rsHisState);
// 	LOGDEBUG("厂站%d统计周期[%s ~ %s]内的历史状态量数量为%d",pThis->m_iSubNo,sFrom.data(),sTo.data(),pThis->m_rsHisState.GetRows());
// 	pThis->m_iCurrHisStateRow = -1;
// 	pThis->m_pCurrHisStateRow = NULL;
// 	ReturnVal.Set(VAL_BOOL,"1");
// 	return true;
// }
