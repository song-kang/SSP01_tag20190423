/**
 *
 * 文 件 名 : sp_unit_checkself.h
 * 创建日期 : 2015-7-21 13:09
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 单元自检信息定义及调用方法封装
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-21	邵凯田　创建文件
 *
 **/

#ifndef __SP_UNIT_CHECK_SELF_H__
#define __SP_UNIT_CHECK_SELF_H__

#include "sp_unit_config.h"
#include "ssp_base_inc.h"

enum eUnitChkSelfAnalogId
{
	CS_ANALOG_CPU_USED=1,		//CPU利用率
	CS_ANALOG_ALLDISK=2,		//存储介质总容量
	CS_ANALOG_ALLDISK_USED=3,	//存储介质总利用率
								//存储介质X分区Y总容量
								//存储介质X分区Y利用率
								//网络存储器X分区Y总容量
								//网络存储器X分区Y利用率
	CS_ANALOG_TEMP=501,			//装置温度
	CS_ANALOG_HUMI=502,			//装置湿度
	CS_ANALOG_PWR1=503,			//装置电源电平1
	CS_ANALOG_PWR2=504,			//装置电源电平2
	CS_ANALOG_PWR3=505,			//装置电源电平3
	CS_ANALOG_PWR4=506,			//装置电源电平4
								//光口X的光功率
								//装置硬盘X的温度

};

//////////////////////////////////////////////////////////////////////////
// 名    称:  stuUnitCheckSelfAnalog
// 作    者:  邵凯田
// 创建时间:  2015-7-21 13:15
// 描    述:  单元自检信息点模拟量
//////////////////////////////////////////////////////////////////////////
struct SSP_BASE_EXPORT stuUnitCheckSelfAnalog
{
	stuUnitCheckSelfAnalog(int id, char* desc,int vt,char* dime,stuHwCheckSelfInfo *pMonitor=NULL)
	{
		m_iInfoId = id;
		strcpy(m_sInfoDesc, desc);
		m_iInfoValType = vt;
		strcpy(m_sInfoDime, dime);
		m_Value.iVal = 0;
		if(pMonitor != NULL)
			memcpy(&m_Monitor,pMonitor,sizeof(m_Monitor));
		else
		{
			m_Monitor.m_bOpen = false;
		}
	}
	int m_iInfoId;					//模拟量序号，从1开始不同信息点固定序号
	char m_sInfoDesc[32];			//模拟量名称描述
	int m_iInfoValType;				//模拟量值类型：0表示浮点数，1表示整数
	char m_sInfoDime[8];			//模拟量值单位
	union {
		float fVal;
		int iVal;
	}m_Value;						//模拟量当前值
	stuHwCheckSelfInfo m_Monitor;	//监视条件
};

struct SSP_BASE_EXPORT stuUnitCheckSelfState
{
	stuUnitCheckSelfState(int id,char* desc,int state=0)
	{
		m_iInfoId = id;
		strcpy(m_sInfoDesc, desc);
		m_iState = state;
	}
	int m_iInfoId;					//状态量序号,从1开始不同信息点固定序号
	char m_sInfoDesc[32];			//状态量名称描述
	int m_iState;					//状态，1表示描述对应的动作发生，0表示动作复归
};



#endif//__SP_UNIT_CHECK_SELF_H__
