/**
 *
 * 文 件 名 : SimComm.h
 * 创建日期 : 2018-8-21 17:20
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 仿真器通信基础定义
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2018-8-21	邵凯田　创建文件
 *
 **/

#ifndef __SIM_COMMUNICATION_H__
#define __SIM_COMMUNICATION_H__

enum eSimPackageType
{
	PKG_REGISTER_REQ=1,
	PKG_REGISTER_RES=2,
	PKG_ABILITY_REQ=3,
	PKG_ABILITY_RES=4,
	PKG_START_REQ=5,
	PKG_START_RES=6,
	PKG_STOP_REQ=7,
	PKG_STOP_RES=8,
	PKG_JOB_REQ=9,
	PKG_JOB_RES=10,

	PKG_RUN_FUN_REQ=21,
	PKG_RUN_FUN_RES=22,
};

struct stuSimAbility
{
	int ctrl_id;//支持的控制编号
	char ctrl_name[32];//控制名称
	int param_num;//参数数目
	char param_names[128];//参数名称列表，不同名称用分号间隔
};



#endif//__SIM_COMMUNICATION_H__
