/**
 *
 * 文 件 名 : UnitCheckSelfSvr.cpp
 * 创建日期 : 2015-7-21 13:25
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 单元自检服务
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-21	邵凯田　创建文件
 *
 **/

#include "UnitCheckSelfSvr.h"

CUnitCheckSelfSvr::CUnitCheckSelfSvr()
{
	m_pUnitConfig = NULL;
	m_Analogs.setAutoDelete(true);
	m_Analogs.setShared(true);
	m_States.setAutoDelete(true);
	m_States.setShared(true);
	m_HashAnalog.SetHashSize(512);
	m_HashState.SetHashSize(512);
	m_pFunAnalogChg = NULL;
	m_pFunStateChg = NULL;
}

CUnitCheckSelfSvr::~CUnitCheckSelfSvr()
{

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  启动单元自检服务
// 作    者:  邵凯田
// 创建时间:  2015-7-21 13:33
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CUnitCheckSelfSvr::Start()
{
	S_CREATE_THREAD(ThreadCheckSelf,this);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  自检主线程
// 作    者:  邵凯田
// 创建时间:  2015-7-21 13:51
// 参数说明:  @lp为this
// 返 回 值:  NULL
//////////////////////////////////////////////////////////////////////////
void* CUnitCheckSelfSvr::ThreadCheckSelf(void *lp)
{
	CUnitCheckSelfSvr *pThis = (CUnitCheckSelfSvr*)lp;
	int i;
	pThis->BeginThread();

	//开机自检
	pThis->StartupCheckSelf();

	while(!pThis->IsQuit())
	{
		//实时自检
		pThis->RealtimeCheckSelf();

		//5秒执行一次基本自检，个别项目（如磁盘状态）在实时自检函数内再行控制执行频率
		for(i=0;i<50 && (!pThis->IsQuit());i++)
			SApi::UsSleep(100000);
	}

	pThis->EndThread();
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  模拟量值改变的突发处理
// 作    者:  邵凯田
// 创建时间:  2015-7-21 13:40
// 参数说明:  @pAnalog
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CUnitCheckSelfSvr::AnalogChg(stuUnitCheckSelfAnalog *pAnalog)
{
	if(m_pFunAnalogChg != NULL)
		m_pFunAnalogChg(m_pFunCbParam,pAnalog);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  状态量值突发改变的处理
// 作    者:  邵凯田
// 创建时间:  2015-7-21 13:40
// 参数说明:  @pState
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CUnitCheckSelfSvr::StateChk(stuUnitCheckSelfState *pState)
{
	if(m_pFunStateChg != NULL)
		m_pFunStateChg(m_pFunCbParam,pState);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  搜索模拟量
// 作    者:  邵凯田
// 创建时间:  2015-7-21 13:40
// 参数说明:  @list为引用返回的列表
//         :  @iFromId为起始ID，-1表示不限制起始ID
//         :  @iToId为结束ID，-1表示不限制结束ID
// 返 回 值:  搜索到的模拟量数据,<0表示失败
//////////////////////////////////////////////////////////////////////////
int CUnitCheckSelfSvr::SearchAnalog(SPtrList<stuUnitCheckSelfAnalog> &list,int iFromId/*=-1*/,int iToId/*=-1*/)
{
	if(iFromId == -1 && iToId == -1)
	{
		m_Analogs.copyto(list);
		return list.count();
	}
	unsigned long pos;
	stuUnitCheckSelfAnalog *p = m_Analogs.FetchFirst(pos);
	while(p)
	{
		if((iFromId == -1||p->m_iInfoId >= iFromId) && (iToId == -1 || p->m_iInfoId <= iToId))
			list.append(p);
		p = m_Analogs.FetchNext(pos);
	}
	return list.count();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  搜索状态量
// 作    者:  邵凯田
// 创建时间:  2015-7-21 13:40
// 参数说明:  @list为引用返回的列表
//         :  @iFromId为起始ID，-1表示不限制起始ID
//         :  @iToId为结束ID，-1表示不限制结束ID
// 返 回 值:  搜索到的状态量数据,<0表示失败
//////////////////////////////////////////////////////////////////////////
int CUnitCheckSelfSvr::SearchState(SPtrList<stuUnitCheckSelfState> &list,int iFromId/*=-1*/,int iToId/*=-1*/)
{
	if(iFromId == -1 && iToId == -1)
	{
		m_States.copyto(list);
		return list.count();
	}
	unsigned long pos;
	stuUnitCheckSelfState *p = m_States.FetchFirst(pos);
	while(p)
	{
		if((iFromId == -1||p->m_iInfoId >= iFromId) && (iToId == -1 || p->m_iInfoId <= iToId))
			list.append(p);
		p = m_States.FetchNext(pos);
	}
	return list.count();
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  开机自检，单元管理程序运行时一次性检测项目
// 作    者:  邵凯田
// 创建时间:  2015-7-21 13:53
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CUnitCheckSelfSvr::StartupCheckSelf()
{
	//先根据当前单元的硬件类型，创建支持的自检信息点
	NewAnalog(new stuUnitCheckSelfAnalog(CS_ANALOG_CPU_USED,"CPU利用率",0,"%",&m_pUnitConfig->m_ChkSelfCpu));
	if(m_pUnitConfig->m_ChkSelfAllDisk.m_bOpen)
	{
		NewAnalog(new stuUnitCheckSelfAnalog(CS_ANALOG_ALLDISK,"存储介质总容量",0,"GB"));
		NewAnalog(new stuUnitCheckSelfAnalog(CS_ANALOG_ALLDISK_USED,"存储介质总利用率",0,"%",&m_pUnitConfig->m_ChkSelfAllDisk));
	}
	//TODO:磁盘分区自检信息点

	if(m_pUnitConfig->m_ChkSelfTemp.m_bOpen)
		NewAnalog(new stuUnitCheckSelfAnalog(CS_ANALOG_TEMP,"装置温度",0,"℃",&m_pUnitConfig->m_ChkSelfTemp));
	if(m_pUnitConfig->m_ChkSelfHumi.m_bOpen)
		NewAnalog(new stuUnitCheckSelfAnalog(CS_ANALOG_HUMI,"装置湿度",0,"%",&m_pUnitConfig->m_ChkSelfHumi));
	if(m_pUnitConfig->m_ChkSelfPwr[0].m_bOpen)
		NewAnalog(new stuUnitCheckSelfAnalog(CS_ANALOG_PWR1,"装置电源电平1",0,"V",&m_pUnitConfig->m_ChkSelfPwr[0]));
	if(m_pUnitConfig->m_ChkSelfPwr[1].m_bOpen)
		NewAnalog(new stuUnitCheckSelfAnalog(CS_ANALOG_PWR2,"装置电源电平2",0,"V",&m_pUnitConfig->m_ChkSelfPwr[1]));
	if(m_pUnitConfig->m_ChkSelfPwr[2].m_bOpen)
		NewAnalog(new stuUnitCheckSelfAnalog(CS_ANALOG_PWR3,"装置电源电平3",0,"V",&m_pUnitConfig->m_ChkSelfPwr[2]));
	if(m_pUnitConfig->m_ChkSelfPwr[3].m_bOpen)
		NewAnalog(new stuUnitCheckSelfAnalog(CS_ANALOG_PWR4,"装置电源电平4",0,"V",&m_pUnitConfig->m_ChkSelfPwr[3]));
	//TODO:光功率自检信息点
	//TODO:硬盘温度自检信息点

	//TODO:执行开机自检
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  实时自检，即单元管理程序运行后定时执行的自检
// 作    者:  邵凯田
// 创建时间:  2015-7-21 13:54
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CUnitCheckSelfSvr::RealtimeCheckSelf()
{

	//TODO:
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  信息点散列值计算函数
// 作    者:  邵凯田
// 创建时间:  2015-7-21 14:44
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
int CUnitCheckSelfSvr::HashKey_Analog(void *cbParam,void *pItem)
{
	return ((stuUnitCheckSelfAnalog*)pItem)->m_iInfoId;
}

int CUnitCheckSelfSvr::HashKey_State(void *cbParam,void *pItem)
{
	return ((stuUnitCheckSelfState*)pItem)->m_iInfoId;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  添加新的信息点到列表和散列
// 作    者:  邵凯田
// 创建时间:  2015-7-21 14:44
// 参数说明:  @p
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CUnitCheckSelfSvr::NewAnalog(stuUnitCheckSelfAnalog *p)
{
	m_Analogs.append(p);
	m_HashAnalog.append(p);
}

void CUnitCheckSelfSvr::NewState(stuUnitCheckSelfState *p)
{
	m_States.append(p);
	m_HashState.append(p);
}

