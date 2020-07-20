/**
 *
 * 文 件 名 : GooseSimulator.h
 * 创建日期 : 2016-1-6 9:17
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : GOOSE模拟器
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-1-6	邵凯田　创建文件
 *
 **/

#if !defined(AFX_GOOSESIMULATOR_H__038486BD_7706_423C_A600_91A3B616B8ED__INCLUDED_)
#define AFX_GOOSESIMULATOR_H__038486BD_7706_423C_A600_91A3B616B8ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SApi.h"
#include "SRawSocket.h"
#include <math.h>
#include "sp_virtual_terminal.h"

//一个报文的最大长度
#define C_MAX_GOOSE_FRAME_LEN 1400

#pragma pack(1)
struct stuGoose_Head1
{
	BYTE DstMac[6];
	BYTE SrcMac[6];
	BYTE VLan[2];//0x81 00
	BYTE VLan_Val[2];//0x80 08
	WORD Type;//0x88B8
	WORD AppID;
	WORD AppLen;
	WORD Res1;
	WORD Res2;
	BYTE PDU_Len[4];//61 82 xx xx
// 	BYTE NumberOfAsdu[3];//80 01 01
// 	BYTE SeqOfAsdu[4];//A2 82 xx xx
// 	//BYTE Asdu_Head[4];//30 82 00 AC
// 	BYTE Asdu_Head[4];//30 82 XX xx
// 	BYTE SvId_Empty[2];//80 0? 后跟SVID字符串
};

// struct stuSV92_Head2
// {
// 	BYTE SampCnt[4];//82 02 00 00
// 	BYTE ConfRev[6];//83 04 00 00 00 01
// 	BYTE Sync[3];//85 01 01
// 	//BYTE Data_Head[4];//87 82 00 90
// 	BYTE Data_Head[4];//87 82 xx xx
// };
#pragma pack(4)

//////////////////////////////////////////////////////////////////////////
// 名    称:  CGooseSimulator
// 作    者:  邵凯田
// 创建时间:  2016-01-07 09:42
// 描    述:  GOOSE模拟器
//////////////////////////////////////////////////////////////////////////
class CGooseSimulator  
{
public:
	/*
	{   1, "array" },
	{   2, "structure" },
	{   3, "boolean" },
	{   4, "bit-string" },
	{   5, "integer" },
	{   6, "unsigned" },
	{   7, "floating-point" },
	{   9, "octet-string" },
	{  10, "visible-string" },
	{  12, "binary-time" },
	{  13, "bcd" },
	{  14, "booleanArray" },
	{  15, "objId" },
	{  16, "mMSString" },
	{  17, "utc-time" },
	};

	*/
	enum eGooseValType
	{
		GOOSE_CHN_UNKNOWN = 0,
		GOOSE_CHN_BOOL    = 3,
		GOOSE_CHN_INT     = 5,
		GOOSE_CHN_UINT    = 6,
		GOOSE_CHN_UTC     = 17,
		GOOSE_CHN_FLOAT   = 7,
		GOOSE_CHN_BITSTR  = 4,
		GOOSE_CHN_STRING  = 10,
		GOOSE_CHN_Q		  = 12,//?
	};
	class CChannel
	{
	public:
		CChannel();
		~CChannel();
		int m_iOffsetInFrameBuffer;//在发送报文缓冲区中的偏移位置
		eGooseValType m_ValType;
		SString m_sChnName;
		SString m_sValue;
		SString m_sNewValue;
	};

	CGooseSimulator(stuSclVtIedGooseOut *pGooseOut,SString srcmac);
	virtual ~CGooseSimulator();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加一个新的通道
	// 作    者:  邵凯田
	// 创建时间:  2016-01-07 09:42
	// 参数说明:  @eType为类型
	//            @iValue为当前值
	//            @q为品质
	// 返 回 值:  新通道指针
	//////////////////////////////////////////////////////////////////////////
	CChannel* AddChannel(SString chnname,eGooseValType vtype,SString sValue);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  重新构建缓冲区
	// 作    者:  邵凯田
	// 创建时间:  2016-01-07 09:42
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void RebuildBuffer();

	void AddNextSendMs(int msec)
	{
		m_iNextSendTimeUSec += msec*1000;
		if(m_iNextSendTimeUSec >= 1000000)
		{
			m_iNextSendTimeSoc += m_iNextSendTimeUSec/1000000;
			m_iNextSendTimeUSec %= 1000000;
		}
	}
protected:


public:
	stuSclVtIedGooseOut *m_pGooseOut;

	BYTE m_SrcMac[6];//源MAC地址
	BYTE m_DstMac[6];//目标MAC地址
	int m_iVLanId,m_iVLanP;
	WORD m_iAppid;//APPID
	WORD m_iReserver1,m_iReserver2;//两个保留字
	SString m_gocb,m_dataset,m_goID;
	int m_iTTL;
	int m_iMaxSendMsec;
	int m_iEventTimeSoc,m_iEventTimeNSec;
	int m_iEventTimeQ;
	int m_iST;
	int m_iSQ;
	bool m_bTest;
	int m_iConfRev;
	int m_iNdsCom;
	int m_iChns;
	SPtrList<CChannel> m_Chns;//全部的通道

	BYTE *m_pBuffer;//发送缓冲区大小
	int m_iFrameLen;//一条报文的长度，单位字节
	int m_iEventTimePos;//事件时间所在偏移位置
	int m_iStPos;//ST偏移位置，初始为1
	int m_iSqPos;//SQ偏移位置，初始为1
	int m_iTestPos;//测试位偏移位置
	int m_iReSendTimes;//重发次数，变位时归0
	int m_iNextSendTimeSoc,m_iNextSendTimeUSec;//下一次的发送时间
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CGooseSimPort
// 作    者:  邵凯田
// 创建时间:  2016-01-07 09:57
// 描    述:  MU模拟器端口，一个端口可以发送多个MU
//////////////////////////////////////////////////////////////////////////
class CGooseSimPort : public SService
{
public:
	CGooseSimPort(SString sDevName);
	~CGooseSimPort();
	////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
	// 作    者:  邵凯田
	// 创建时间:  2016-01-07 09:57
	// 参数说明:  void
	// 返 回 值:  true表示服务启动成功,false表示服务启动失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务，派生函数中必须先调用此函数
	// 作    者:  邵凯田
	// 创建时间:  2016-01-07 09:57
	// 参数说明:  void
	// 返 回 值:  true表示服务停止成功,false表示服务停止失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	static void* ThreadSend(void *lp);

	bool CheckGooseOutIsExist(stuSclVtIedGooseOut* pCur);
	SString m_sDevName;//发送网卡名
	SRawSocketSend m_Sender;
	SPtrList<CGooseSimulator> m_Gooses;//Goose列表
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CGooseSimManager
// 作    者:  邵凯田
// 创建时间:  2016-01-07 10:01
// 描    述:  GOOSE模拟器管理器
//////////////////////////////////////////////////////////////////////////
class CGooseSimManager
{
public:
	CGooseSimManager();
	virtual ~CGooseSimManager();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  查找指针网卡名的端口实例，不存在则新创建
	// 作    者:  邵凯田
	// 创建时间:  2016-01-07 10:01
	// 参数说明:  @sDevName为网卡名
	// 返 回 值:  CGooseSimPort*
	//////////////////////////////////////////////////////////////////////////
	CGooseSimPort* SearchPort(SString sDevName);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加一个新的GOOSE
	// 作    者:  邵凯田
	// 创建时间:  2016-01-07 10:01
	// 参数说明:  sDevName为网卡名
	//            @pGoose为初始化后的GOOSE实例
	// 返 回 值:  CGooseSimulator*
	//////////////////////////////////////////////////////////////////////////
	CGooseSimulator* AddGoose(SString sDevName,CGooseSimulator *pGoose);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  停止全部发送，释放对象
	// 作    者:  邵凯田
	// 创建时间:  2016-01-07 10:01
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void Clear();

	SPtrList<CGooseSimPort> m_Ports;
};

#endif // !defined(AFX_GOOSESIMULATOR_H__038486BD_7706_423C_A600_91A3B616B8ED__INCLUDED_)
