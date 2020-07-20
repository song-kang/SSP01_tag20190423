/**
 *
 * 文 件 名 : SSnmp.cpp
 * 创建日期 : 2015-12-1 13:15
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SNMP客户端操作接口类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-12-1	邵凯田　创建文件
 *
 **/

#include "SSnmpClient.h"


#include <libsnmp.h>
#include "snmp_pp/snmp_pp.h"
#include <iostream>
using namespace std;

#ifdef WIN32
#define strcasecmp _stricmp
#endif

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

SString SSnmpClient_GetVbValueStr(Vb &vb)
{
	// 	if(vb.get_syntax() == sNMP_SYNTAX_OCTETS)
	// 	{
	// 		BYTE ptr[64];
	// 		memset(ptr,0,sizeof(ptr));
	// 		unsigned long len=0;
	// 		vb.get_value(ptr,len,sizeof(ptr));
	// 		return SString::HexToStr(ptr,len,false,sizeof(ptr));
	// 	}
	return vb.get_printable_value();
}

SString SSnmpClient_GetVbValueHexStr(Vb &vb)
{
// 	if(vb.get_syntax() == sNMP_SYNTAX_OCTETS)
// 	{
		BYTE ptr[128];
		memset(ptr,0,sizeof(ptr));
		unsigned long len=0;
		vb.get_value(ptr,len,sizeof(ptr));
		return SString::HexToStr(ptr,len,false,sizeof(ptr));
//	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  根据变量的值类型，将字符串型的值转换为对应的实际值类型
// 作    者:  邵凯田
// 创建时间:  2016-7-8 14:35
// 参数说明:  iValType表示值类型:
//								 "Integer:  " << sNMP_SYNTAX_INT
//								 "Bits:     " << sNMP_SYNTAX_BITS
//								 "STRING:   " << sNMP_SYNTAX_OCTETS
//								 "Oid:      " << sNMP_SYNTAX_OID
//								 "IpAddress:" << sNMP_SYNTAX_IPADDR
//         :  @sValue为字符串表示的值内容
//         :  @vb为引用的目标变量
// 返 回 值:  true表示成功，false表示失败
//////////////////////////////////////////////////////////////////////////
bool SSnmpClient_Determine_Vb(SmiUINT32 iValType, char* sValue, Vb &vb)
{
	// 	char buffer[255];
	// 
	// 	if (val == sNMP_SYNTAX_NOSUCHINSTANCE)
	// 	{
	// 		cout << "Instance does not exists but can be created.\n";
	// 		cout << "Please enter one of the following types:\n\n";
	// 		cout << "Integer:  " << sNMP_SYNTAX_INT << "\n";
	// 		cout << "Bits:     " << sNMP_SYNTAX_BITS << "\n";
	// 		cout << "STRING:   " << sNMP_SYNTAX_OCTETS << "\n";
	// 		cout << "Oid:      " << sNMP_SYNTAX_OID << "\n";
	// 		cout << "IpAddress:" << sNMP_SYNTAX_IPADDR << "\n\n";
	// 		cout << "Please choose value type: ";
	// 		cin >> val;
	// 		vb.set_syntax(val);
	// 	}
	// 	cout << "Value Type is ";
	vb.set_syntax(iValType);
	switch (iValType) 
	{		// octet string
	case sNMP_SYNTAX_OCTETS:
		{
			// 			cout << "Octet String\n";
			// 			cout << "Please enter new value: ";
			// 			cin >> buffer;
			//如果是MAC地址，则转换一下，
			int len = strlen(sValue)>>1;
			BYTE *pHex = new BYTE[len];
			if(strlen(sValue) == 17 && sValue[2]=='-' && sValue[5]=='-' && sValue[8]=='-' && sValue[11]=='-' && sValue[14]=='-')
			{
				len = 6;
				SString::MacStrToHex(sValue,pHex);
			}
			else
				SString::StrToHex(sValue,len<<1,pHex);
			OctetStr octetstr(pHex,len);
			if (octetstr.valid())
			{
				delete[] pHex;
				vb.set_value(octetstr);
				return true;
			}
			else
			{
				delete[] pHex;
				//cout << "Invalid OctetStr\n";
				return false;
			}
		}

		// IP Address
	case sNMP_SYNTAX_IPADDR:
		{
			// 			cout << "IP Address\n";
			// 			cout << "Please enter new value: ";
			// 			cin >> buffer;
			IpAddress ipaddress(sValue);
			if (ipaddress.valid())
			{
				vb.set_value(ipaddress);
				return true;
			}
			else
			{
				//cout << "Invalid IP Address\n";
				return false;
			}
		}

		// Oid
	case sNMP_SYNTAX_OID:
		{
			// 			cout << "Oid\n";
			// 			cout << "Please enter new value: ";
			// 			cin >> buffer;
			Oid oid(sValue);
			if (oid.valid()) {
				vb.set_value(oid);
				return true;
			}
			else {
				//cout << "Invalid Oid\n";
				return false;
			}
		}

		// TimeTicks
	case sNMP_SYNTAX_TIMETICKS:
		{
			// 			cout << "TimeTicks\n";
			// 			cout << "Please enter new value: ";
			// 			cin >> buffer;
			unsigned long i;
			i = atol(sValue);
			TimeTicks timeticks(i);
			if (timeticks.valid())
			{
				vb.set_value(timeticks);
				return true;
			}
			else
			{
				//cout << "Invalid TimeTicks\n";
				return false;
			}
		}

		// Gauge32
	case sNMP_SYNTAX_GAUGE32:
		{
			// 			cout << "Gauge32\n";
			// 			cout << "Please enter new value: ";
			// 			cin >> buffer;
			unsigned long i;
			i = atol(sValue);
			Gauge32 gauge32(i); 
			if (gauge32.valid())
			{
				vb.set_value(gauge32);
				return true;
			}
			else
			{
				//cout << "Invalid Gauge32\n";
				return false;
			}
		}

	case sNMP_SYNTAX_CNTR32:
		{
			// 			cout << "Counter32\n";
			// 			cout << "Please enter new value: ";
			// 			cin >> buffer;
			unsigned long i;
			i = atol(sValue);
			Counter32 counter32(i);
			if (counter32.valid())
			{
				vb.set_value(counter32);
				return true;
			}
			else
			{
				//cout << "Invalid Counter32\n";
				return false;
			}
		}

	case sNMP_SYNTAX_CNTR64:
		{
			// 			cout << "Counter64\n";
			// 			cout << "Please enter value (low 32 bit): ";
			// 			cin >> buffer;
			Counter64 counter64((pp_uint64)SString::toInt64(sValue));
			// 			unsigned long i;
			// 			i = atol(sValue);
			// 			Counter64 counter64;
			// 			counter64.set_low(i);
			// 
			// 			cout << "Please enter value (high 32 bit): ";
			// 			cin >> buffer;
			// 			i = atol(buffer);
			// 			counter64.set_high(i);
			// 
			if (counter64.valid()) {
				vb.set_value(counter64);
				return true;
			}
			else {
				//cout << "Invalid Counter64\n";
				return false;
			}
		}

	case sNMP_SYNTAX_INT:
		{
			// 			cout << "Integer\n";
			// 			cout << "Please enter new value: ";
			// 			cin >> buffer;
			// 			unsigned long i;
			// 			i = atol(sValue);
			// 			long l ;
			// 			l = (long) i;
			vb.set_value(atoi(sValue));
			return true;
		}

	case sNMP_SYNTAX_NOSUCHOBJECT:
		{
			// 			cout << "NO SUCH OBJECT\n";
			// 			cout << "Object cannot be created.\n";
			return false;
		}
	default:
		//		cout << "Unknown Data Type " << val << "\n";
		return false;
	}
}

SSnmpClient::SSnmpClient()
{
	m_sCommunity = "public";
	m_iPort = 161;
	m_Version = SSNMP_VER1;
	m_iRetryTimes = 1;
	m_iTimeoutMs = 1000;
	m_pUdpAddress = NULL;
	m_pCommunity = NULL;
	m_pSnmp = NULL;
	m_pCTarget = NULL;
	m_portinfo_last_soc = m_portinfo_last_usec = 0;
	m_iConnected = -1;
}

SSnmpClient::~SSnmpClient()
{
	if(m_pUdpAddress != NULL)
	{
		delete ((UdpAddress*)m_pUdpAddress);
		m_pUdpAddress = NULL;
	}
	if(m_pCommunity != NULL)
	{
		delete ((OctetStr*)m_pCommunity);
		m_pCommunity = NULL;
	}
	if(m_pSnmp != NULL)
	{
		delete ((Snmp*)m_pSnmp);
		m_pSnmp = NULL;
	}
	if(m_pCTarget != NULL)
	{
		delete ((CTarget*)m_pCTarget);
		m_pCTarget = NULL;
	}
}

bool g_bGlobalInited=false;
//////////////////////////////////////////////////////////////////////////
// 描    述:  全局初始化静态函数
// 作    者:  邵凯田
// 创建时间:  2015-12-1 13:54
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SSnmpClient::GlobalInit()
{
	if(g_bGlobalInited == true)
		return;
	g_bGlobalInited = true;
//#if defined(_DEBUG) && !defined(_NO_LOGGING) && !defined(WITH_LOG_PROFILES)
	// Set filter for logging
	DefaultLog::log()->set_filter(ERROR_LOG, 0);//7
	DefaultLog::log()->set_filter(WARNING_LOG, 0);
	DefaultLog::log()->set_filter(EVENT_LOG, 0);
	DefaultLog::log()->set_filter(INFO_LOG, 0);
	DefaultLog::log()->set_filter(DEBUG_LOG, 0);
//#endif

	Snmp::socket_startup();  // Initialize socket subsystem
}
//////////////////////////////////////////////////////////////////////////
// 描    述:  全局回收静态函数
// 作    者:  邵凯田
// 创建时间:  2015-12-1 13:54
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SSnmpClient::GlobalExit()
{
	if(g_bGlobalInited == false)
		return;
	Snmp::socket_cleanup();
	DefaultLog::cleanup();
	g_bGlobalInited = false;
}

void SSnmpClient::SetServerAddress(SString ip,int port)
{
	m_sServerIp = ip;
	m_iPort = port;
}

void SSnmpClient::SetCommunity(SString sCommunityName)
{
	m_sCommunity = sCommunityName;
	if(m_pCommunity != NULL)
	{
		delete ((OctetStr*)m_pCommunity);
		m_pCommunity = NULL;
	}
	OctetStr *pComm = new OctetStr(sCommunityName.data());
	m_pCommunity = pComm;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  根据类型名称返回对应的SNMP类型值
// 作    者:  邵凯田
// 创建时间:  2016-7-9 15:03
// 参数说明:  @name为类型名称，全部小写，如：int/octets/ipaddr/oid/timeticks/gauge32/cntr32/cntr64/bits/...
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
int SSnmpClient::GetSynTaxByName(SString name)
{
	if(SString::equals(name.data(),"int"))
		return sNMP_SYNTAX_INT;
	else if(SString::equals(name.data(),"bits"))
		return sNMP_SYNTAX_BITS;
	else if(SString::equals(name.data(),"octets"))
		return sNMP_SYNTAX_OCTETS;
	else if(SString::equals(name.data(),"null"))
		return sNMP_SYNTAX_NULL;
	else if(SString::equals(name.data(),"oid"))
		return sNMP_SYNTAX_OID;
	else if(SString::equals(name.data(),"int32"))
		return sNMP_SYNTAX_INT32;
	else if(SString::equals(name.data(),"ipaddr"))
		return sNMP_SYNTAX_IPADDR;
	else if(SString::equals(name.data(),"cntr32"))
		return sNMP_SYNTAX_CNTR32;
	else if(SString::equals(name.data(),"gauge32"))
		return sNMP_SYNTAX_GAUGE32;
	else if(SString::equals(name.data(),"timeticks"))
		return sNMP_SYNTAX_TIMETICKS;
	else if(SString::equals(name.data(),"opaque"))
		return sNMP_SYNTAX_OPAQUE;
	else if(SString::equals(name.data(),"cntr64"))
		return sNMP_SYNTAX_CNTR64;
	else if(SString::equals(name.data(),"uint32"))
		return sNMP_SYNTAX_UINT32;
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  将指定的10毫秒为单元的时间计数，转换为可视的字符串
// 作    者:  邵凯田
// 创建时间:  2016-7-11 14:09
// 参数说明:  @hsec_ticks表示装置上电或复位开始计数的时间，从0开始
// 返 回 值:  SString
//////////////////////////////////////////////////////////////////////////
SString SSnmpClient::HSecondsTickets2Str(unsigned long hsec_ticks)
{
	unsigned long hseconds, seconds, minutes, hours, days;
	unsigned long tt = hsec_ticks;

	days = tt / 8640000;
	tt %= 8640000;

	hours = tt / 360000;
	tt %= 360000;

	minutes = tt / 6000;
	tt %= 6000;

	seconds = tt / 100;
	tt %= 100;

	hseconds = tt;
	SString str;
	if (days == 0)
		str.sprintf("%lu:%02lu:%02lu.%02lu",
					hours, minutes, seconds, hseconds);
	else if (days == 1)
		str.sprintf("1 day %lu:%02lu:%02lu.%02lu",
					hours, minutes, seconds, hseconds);
	else
		str.sprintf("%lu days, %lu:%02lu:%02lu.%02lu",
					days, hours, minutes, seconds, hseconds);

	return str;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  新建会话
// 作    者:  邵凯田
// 创建时间:  2015-12-1 14:10
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool SSnmpClient::BeginSession()
{
// 	if(m_pSnmp != NULL)
// 		return false;
	if(m_pUdpAddress != NULL)
	{
		delete ((UdpAddress*)m_pUdpAddress);
		m_pUdpAddress = NULL;
	}
	if(m_pCommunity != NULL)
	{
		delete ((OctetStr*)m_pCommunity);
		m_pCommunity = NULL;
	}
	if(m_pSnmp != NULL)
	{
		delete ((Snmp*)m_pSnmp);
		m_pSnmp = NULL;
	}
	if(m_pCTarget != NULL)
	{
		delete ((CTarget*)m_pCTarget);
		m_pCTarget = NULL;
	}

	if(m_sServerIp.length() < 7 || m_sServerIp.find(".") < 0)
	{
		LOGERROR("无效的SNMP服务端IP:%s",m_sServerIp.data());
		SetConnected(false);
		return false;
	}
	m_pUdpAddress = new UdpAddress(m_sServerIp.data());
	if(((UdpAddress*)m_pUdpAddress)->valid() == false)
	{
		LOGERROR("无效的SNMP服务端IP:%s",m_sServerIp.data());
		SetConnected(false);
		return false;
	}
	((UdpAddress*)m_pUdpAddress)->set_port(m_iPort);
	if(m_pCommunity != NULL)
	{
		delete ((OctetStr*)m_pCommunity);
		m_pCommunity = NULL;
	}
	m_pCommunity = new OctetStr(m_sCommunity.data());
	int status;
	m_pSnmp = new Snmp(status,0,(((UdpAddress*)m_pUdpAddress)->get_ip_version() == Address::version_ipv6));
	if(status != SNMP_CLASS_SUCCESS)
	{
		LOGERROR("SNMP会话创建失败!%s",((Snmp*)m_pSnmp)->error_msg(status));
		EndSession();
		return false;
	}
	m_pCTarget = new CTarget(*((UdpAddress*)m_pUdpAddress));
	snmp_version version = version1;
	if(m_Version == SSNMP_VER1)
		version = version1;
	else if(m_Version == SSNMP_VER2C)
		version = version2c;
	else if(m_Version == SSNMP_VER3)
		version = version3;
	((CTarget*)m_pCTarget)->set_version(version);         // set the SNMP version SNMPV1 or V2
	((CTarget*)m_pCTarget)->set_retry(m_iRetryTimes);           // set the number of auto retries
	((CTarget*)m_pCTarget)->set_timeout(m_iTimeoutMs<10?1:(m_iTimeoutMs/10));         // set timeout
	((CTarget*)m_pCTarget)->set_readcommunity(*((OctetStr*)m_pCommunity)); // set the read community name
	((CTarget*)m_pCTarget)->set_writecommunity(*((OctetStr*)m_pCommunity)); // set the read community name

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  结束会话
// 作    者:  邵凯田
// 创建时间:  2015-12-1 14:10
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool SSnmpClient::EndSession()
{
	if(m_pSnmp == NULL)
		return false;
	SetConnected(false);
	//void *pSnmp = m_pSnmp;
	//m_pSnmp = NULL;
	//SApi::UsSleep(2000000);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  SNMP Get操作
// 作    者:  邵凯田
// 创建时间:  2015-12-1 14:12
// 参数说明:  oids为多个OID字符串，通过','分隔
//         :  @sValues表示引用返回的结果值，属性串方式,oid=value;多输出时用';'分隔
// 返 回 值:  >0表示成功，=0表示OID不存在，<0表示失败
//////////////////////////////////////////////////////////////////////////
int SSnmpClient::Get(SString oids,SString &sValues,SValueList<bool> *pRowHexList/*=NULL*/)
{
	sValues = "";
	if(m_pSnmp == NULL)
		return -10;
	Pdu pdu;                               // construct a Pdu object
	Vb vb;                                 // construct a Vb object
	SString oid;
	int i,cnt = SString::GetAttributeCount(oids,",");
	for(i=1;i<=cnt;i++)
	{
		oid = SString::GetIdAttribute(i,oids,",");
		Oid o(oid.data());
		vb.set_oid(o);                   // set the Oid portion of the Vb
		pdu += vb; 
	}
	SnmpTarget *target = (SnmpTarget*)m_pCTarget;
	int status;
	m_lock.lock();
	status = ((Snmp*)m_pSnmp)->get(pdu, *target);
	m_lock.unlock();
	if (status == SNMP_CLASS_SUCCESS)
	{
		for (i = 0; i < pdu.get_vb_count(); i++)
		{
			pdu.get_vb(vb, i);
			if(pRowHexList != NULL && i<(int)pRowHexList->count() && *pRowHexList->at(i) == true)//使用原始HEX类型格式化
				sValues += SString::toFormat("%s=%s;",vb.get_printable_oid(),SSnmpClient_GetVbValueHexStr(vb).data());
			else
				sValues += SString::toFormat("%s=%s;",vb.get_printable_oid(),SSnmpClient_GetVbValueStr(vb).data());

// 			LOGDEBUG("oid[%s]=%s (Syntax=%d) \n",
// 				vb.get_printable_oid(),
// 				SSnmpClient_GetVbValueStr(vb).data(),
// 				vb.get_syntax());
			if ((vb.get_syntax() == sNMP_SYNTAX_ENDOFMIBVIEW) ||
				(vb.get_syntax() == sNMP_SYNTAX_NOSUCHINSTANCE) ||
				(vb.get_syntax() == sNMP_SYNTAX_NOSUCHOBJECT))
			{
				LOGERROR("Exception:%d occured.", vb.get_syntax());
			}
		}
		SetConnected(true);
	}
	else
	{
		SetConnected(false);
		LOGERROR("Snmp Get Error!%s",((Snmp*)m_pSnmp)->error_msg( status));
		return -1;
	}
	return 1;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  SNMP Get操作，返回到记录对象中
// 作    者:  邵凯田
// 创建时间:  2016-7-8 10:52
// 参数说明:  oids为多个OID字符串，通过','分隔
//         :  @rec为记录对象
//         :  @pRowHexList表示是否将对应列值读取为原始的HEX字符，列表数量与oid数量一致
// 返 回 值:  >0表示成功，=0表示OID不存在，<0表示失败
//////////////////////////////////////////////////////////////////////////
int SSnmpClient::Get(SString oids,SRecord &rec,SValueList<bool> *pRowHexList/*=NULL*/)
{
	if(m_pSnmp == NULL)
		return -10;
	Pdu pdu;                               // construct a Pdu object
	Vb vb;                                 // construct a Vb object
	SString oid;
	int i,cnt = SString::GetAttributeCount(oids,",");
	for(i=1;i<=cnt;i++)
	{
		oid = SString::GetIdAttribute(i,oids,",");
		Oid o(oid.data());
		vb.set_oid(o);                   // set the Oid portion of the Vb
		pdu += vb; 
	}
	SnmpTarget *target = (SnmpTarget*)m_pCTarget;
	int status;
	m_lock.lock();
	status = ((Snmp*)m_pSnmp)->get(pdu, *target);
	m_lock.unlock();
	if (status == SNMP_CLASS_SUCCESS)
	{
		rec.SetColumns(pdu.get_vb_count());
		for (i = 0; i < pdu.get_vb_count(); i++)
		{
			pdu.get_vb(vb, i);
			if(pRowHexList != NULL && i<(int)pRowHexList->count() && *pRowHexList->at(i) == true)//使用原始HEX类型格式化
				rec.SetValue(i,SString::toFormat("%s=%s",vb.get_printable_oid(),SSnmpClient_GetVbValueHexStr(vb).data()));
			else
				rec.SetValue(i,SString::toFormat("%s=%s",vb.get_printable_oid(),SSnmpClient_GetVbValueStr(vb).data()));

			if ((vb.get_syntax() == sNMP_SYNTAX_ENDOFMIBVIEW) ||
				(vb.get_syntax() == sNMP_SYNTAX_NOSUCHINSTANCE) ||
				(vb.get_syntax() == sNMP_SYNTAX_NOSUCHOBJECT))
			{
				LOGERROR("Exception:%d occured.", vb.get_syntax());
			}
		}
		SetConnected(true);
	}
	else
	{
		SetConnected(false);
		LOGERROR("Snmp Get Error!%s",((Snmp*)m_pSnmp)->error_msg( status));
		return -1;
	}
	return 1;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  SNMP Set写操作
// 作    者:  邵凯田
// 创建时间:  2016-7-8 15:27
// 参数说明:  oids为多个OID字符串，通过','分隔
//         :  @sValues表示引用返回的结果值；多内容时用';'分隔，数量与oids对应
// 返 回 值:  >0表示成功，=0表示OID不存在，<0表示失败
//////////////////////////////////////////////////////////////////////////
int SSnmpClient::Set(SString oids,SString sValues)
{
	if(m_pSnmp == NULL)
		return -10;
	Pdu pdu;                               // construct a Pdu object
	Vb vb;                                 // construct a Vb object
	SString oid;
	int i,cnt = SString::GetAttributeCount(oids,",");
	for(i=1;i<=cnt;i++)
	{
		oid = SString::GetIdAttribute(i,oids,",");
		Oid o(oid.data());
		vb.set_oid(o);                   // set the Oid portion of the Vb
		pdu += vb; 
	}
	SnmpTarget *target = (SnmpTarget*)m_pCTarget;
	int status;
	m_lock.lock();
	status = ((Snmp*)m_pSnmp)->get(pdu, *target);
	m_lock.unlock();
	if (status == SNMP_CLASS_SUCCESS)
	{
		SetConnected(true);
		Pdu setpdu;
		for (i = 0; i < pdu.get_vb_count(); i++)
		{
			pdu.get_vb(vb, i);
			if ((vb.get_syntax() == sNMP_SYNTAX_ENDOFMIBVIEW) ||
				(vb.get_syntax() == sNMP_SYNTAX_NOSUCHINSTANCE) ||
				(vb.get_syntax() == sNMP_SYNTAX_NOSUCHOBJECT))
			{
				LOGERROR("Exception:%d occured.", vb.get_syntax());
				return -100;
			}
			SString sVal = SString::GetIdAttribute(i+1,sValues,";");
			if(vb.get_syntax() == sNMP_SYNTAX_OCTETS)
			{
				SString old_val = SSnmpClient_GetVbValueHexStr(vb);//vb.get_printable_value();
				if(old_val.length() > sVal.length())
					sVal += SString::toFillString("",old_val.length()-sVal.length(),'0');
			}
			SSnmpClient_Determine_Vb(vb.get_syntax(),sVal.data(),vb);
			setpdu += vb; 
		}
		m_lock.lock();
		status = ((Snmp*)m_pSnmp)->set(setpdu, *target);
		m_lock.unlock();
		if(status == SNMP_CLASS_SUCCESS)
			return 1;
		else
		{
			LOGERROR("Snmp Set Error!%s",((Snmp*)m_pSnmp)->error_msg( status));
			return -1;
		}
	}
	else
	{
		SetConnected(false);
		LOGERROR("Snmp Get Error!%s",((Snmp*)m_pSnmp)->error_msg( status));
		return -1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  根据给定的值类型写一个点的值
// 作    者:  邵凯田
// 创建时间:  2016-7-9 14:56
// 参数说明:  @oid表示路径
//         :  @sValue表示值内容
//         :  @iValType表示值类型
// 返 回 值:  >0表示成功，=0表示OID不存在，<0表示失败
//////////////////////////////////////////////////////////////////////////
int SSnmpClient::SetOneWithType(SString oid,SString sValue,int iValType)
{
	if(m_pSnmp == NULL)
		return -10;
	Pdu pdu;                               // construct a Pdu object
	Vb vb;                                 // construct a Vb object
	Oid o(oid.data());
	vb.set_oid(o);
	SSnmpClient_Determine_Vb((SmiUINT32)iValType,sValue.data(),vb);
	pdu += vb;

	SnmpTarget *target = (SnmpTarget*)m_pCTarget;
	int status;
	m_lock.lock();
	status = ((Snmp*)m_pSnmp)->set(pdu, *target);
	m_lock.unlock();
	if(status == SNMP_CLASS_SUCCESS)
	{
		SetConnected(true);
		return 1;
	}
	else
	{
		SetConnected(false);
		LOGERROR("Snmp Set Error!%s",((Snmp*)m_pSnmp)->error_msg( status));
		return -1;
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  SNMP GetNext操作
// 作    者:  邵凯田
// 创建时间:  2015-12-1 14:12
// 参数说明:  oids为单个OID字符串
//         :  @sValues表示引用返回的结果值，属性串方式,oid=value;
//         :  @bRawHexStr表示是否显示有16进制的格式化字符串
// 返 回 值:  >0表示成功，=0表示OID不存在，<0表示失败
//////////////////////////////////////////////////////////////////////////
int SSnmpClient::GetNext(SString oid,SString &sValues,bool bRawHexStr/*=false*/)
{
	sValues = "";
	if(m_pSnmp == NULL)
		return -10;
	Pdu pdu;                               // construct a Pdu object
	Vb vb;                                 // construct a Vb object
	Oid o(oid.data());
	vb.set_oid(o);                   // set the Oid portion of the Vb
	pdu += vb; 
	
	int i;
	SnmpTarget *target = (SnmpTarget*)m_pCTarget;
	int status;
	m_lock.lock();
	status = ((Snmp*)m_pSnmp)->get_next(pdu, *target);
	m_lock.unlock();
	if (status == SNMP_CLASS_SUCCESS)
	{
		for (i = 0; i < pdu.get_vb_count(); i++)
		{
			pdu.get_vb(vb, i);
			if(bRawHexStr)
				sValues += SString::toFormat("%s=%s;",vb.get_printable_oid(),SSnmpClient_GetVbValueHexStr(vb).data());
			else
				sValues += SString::toFormat("%s=%s;",vb.get_printable_oid(),SSnmpClient_GetVbValueStr(vb).data());

// 			LOGDEBUG("oid[%s]=%s (Syntax=%d) \n",
// 				vb.get_printable_oid(),
// 				SSnmpClient_GetVbValueStr(vb).data(),
// 				vb.get_syntax());
			if ((vb.get_syntax() == sNMP_SYNTAX_ENDOFMIBVIEW) ||
				(vb.get_syntax() == sNMP_SYNTAX_NOSUCHINSTANCE) ||
				(vb.get_syntax() == sNMP_SYNTAX_NOSUCHOBJECT))
			{
				LOGERROR("Exception:%d occured.", vb.get_syntax());
			}
		}
		SetConnected(true);
	}
	else
	{
		SetConnected(false);
		LOGERROR("Snmp Get Error!%s",((Snmp*)m_pSnmp)->error_msg( status));
		return -1;
	}
	return 1;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  SNMP多字段 GetNext操作
// 作    者:  邵凯田
// 创建时间:  2016-7-8 9:13
// 参数说明:  oids为多个OID字符串，通过','分隔
//         :  @rec为结果记录
//         :  @pRowHexList表示是否将对应列值读取为原始的HEX字符，列表数量与oid数量一致
// 返 回 值:  >0表示成功返回的记录数量，=0表示OID不存在，<0表示失败
//////////////////////////////////////////////////////////////////////////
int SSnmpClient::GetNextMulti(SString oids,SRecord &rec,SValueList<bool> *pRowHexList/*=NULL*/)
{
	
	if(m_pSnmp == NULL)
		return -10;
	Pdu pdu;                               // construct a Pdu object
	Vb vb;                                 // construct a Vb object
	SString oid;
	int i,cnt = SString::GetAttributeCount(oids,",");
	for(i=1;i<=cnt;i++)
	{
		oid = SString::GetIdAttribute(i,oids,",");
		Oid o(oid.data());
		vb.set_oid(o);						// set the Oid portion of the Vb
		pdu += vb; 
	}

	SnmpTarget *target = (SnmpTarget*)m_pCTarget;
	int status;
	SString sValues,newoid;
	m_lock.lock();
	status = ((Snmp*)m_pSnmp)->get_next(pdu, *target);
	m_lock.unlock();
	if (status == SNMP_CLASS_SUCCESS)
	{
		rec.SetColumns(pdu.get_vb_count());
		for (i = 0; i < pdu.get_vb_count(); i++)
		{
			pdu.get_vb(vb, i);
			if(pRowHexList != NULL && i<(int)pRowHexList->count() && *pRowHexList->at(i) == true)//使用原始HEX类型格式化
				sValues = SString::toFormat("%s=%s",vb.get_printable_oid(),SSnmpClient_GetVbValueHexStr(vb).data());
			else
				sValues = SString::toFormat("%s=%s",vb.get_printable_oid(),SSnmpClient_GetVbValueStr(vb).data());
			rec.SetValue(i,sValues);
//			LOGDEBUG("oid[%s]=%s (Syntax=%d) \n",vb.get_printable_oid(),SSnmpClient_GetVbValueStr(vb).data(),vb.get_syntax());
			if ((vb.get_syntax() == sNMP_SYNTAX_ENDOFMIBVIEW) ||
				(vb.get_syntax() == sNMP_SYNTAX_NOSUCHINSTANCE) ||
				(vb.get_syntax() == sNMP_SYNTAX_NOSUCHOBJECT))
			{
				LOGERROR("Exception:%d occured.", vb.get_syntax());
			}
		}
		SetConnected(true);
	}
	else
	{
		SetConnected(false);
		LOGERROR("Snmp Get Error!%s",((Snmp*)m_pSnmp)->error_msg( status));
		return -1;
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  SNMP遍历指定的OID所有值
// 作    者:  邵凯田
// 创建时间:  2015-12-2 8:58
// 参数说明:  @oid为单个OID字符串
//         :  @sValues表示引用返回的结果值，属性串方式,oid=value;oid2=value2;...
// 返 回 值:  >0表示值数量，=0表示OID不存在，<0表示失败
//////////////////////////////////////////////////////////////////////////
int SSnmpClient::FetchAll(SString oid,SString &sValues)
{
	int ret;
	SString suboid;
	SString sResult;
	sValues = "";
	ret = GetNext(oid,sResult);
	if(ret <= 0)
		return ret;
	int cnt = 0;
	while(ret > 0)
	{
		suboid = SString::GetIdAttribute(1,sResult,"=");
		if(suboid.find(oid) != 0)
			break;
		sValues += sResult;
		cnt ++;
		sResult = "";
		ret = GetNext(suboid,sResult);
	}
	return cnt;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  SNMP遍历指定的OID的值
// 作    者:  邵凯田
// 创建时间:  2015-12-2 8:58
// 参数说明:  @oid为单个OID字符串
//         :  @sValues表示引用返回的结果值，多个值采用';'分隔，最后一个元素后不放';'
// 返 回 值:  >0表示值数量，=0表示OID不存在，<0表示失败
//////////////////////////////////////////////////////////////////////////
int SSnmpClient::FetchValue(SString oid,SString &sValues)
{
	int ret;
	SString suboid;
	SString sResult;
	sValues = "";
	ret = GetNext(oid,sResult);
	if(ret <= 0)
		return ret;
	int cnt = 0;
	while(ret > 0)
	{
		sResult = SString::GetIdAttribute(1,sResult,";");
		suboid = SString::GetIdAttribute(1,sResult,"=");
		if(suboid.find(oid) != 0)
			break;
		if(sValues.length() > 0)
			sValues += ";";
		sValues += SString::GetIdAttribute(2,sResult,"=");
		cnt ++;
		sResult = "";
		ret = GetNext(suboid,sResult);
	}
	return cnt;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  读取设备描述：1.3.6.1.2.1.1.1
// 作    者:  邵凯田
// 创建时间:  2016-7-20 20:02
// 参数说明:  void
// 返 回 值:  SString
//////////////////////////////////////////////////////////////////////////
SString SSnmpClient::GetSwitchDescr()
{
	SString sValues;
	return GetNext("1.3.6.1.2.1.1.1",sValues);
	return sValues;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  根据给定的广播IP地址，探索子网内所有的支持SNMP协议的终端
// 作    者:  邵凯田
// 创建时间:  2016-7-11 8:47
// 参数说明:  @bcast_ip为广播地址
//         :  @slResult为结果IP地址集合,每个地址的格式为:IP地址/端口号
// 返 回 值:  >=0表示返回的终端数量，<0表示失败
//////////////////////////////////////////////////////////////////////////
int SSnmpClient::SnmpDiscover(SString bcast_ip,SStringList &slResult,SSnmpVersion ver)
{
	UdpAddress address(bcast_ip.data());      // make a SNMP++ Generic address
	if ( !address.valid())
	{
		LOGERROR("invalid bcast_ip=%s",bcast_ip.data());
		return -1;
	}
	snmp_version version=version1;                  // default is v1
	if(ver == SSNMP_VER2C)
		version = version2c;
	else if(ver == SSNMP_VER3)
		version = version3;
	int retries=1;                                  // default retries is 1
	int timeout=100;                                // default is 1 second
	u_short port=161;                               // default snmp port is 161
	OctetStr community("public");                   // community name
	//----------[ create a SNMP++ session ]-----------------------------------
	int status;
	// bind to any port and use IPv6 if needed
	Snmp snmp(status, 0, (address.get_ip_version() == Address::version_ipv6));

	if ( status != SNMP_CLASS_SUCCESS)
	{
		LOGERROR("SNMP++ Session Create Fail, %s",snmp.error_msg(status));
		return -2;
	}
	 
	//---------[ init SnmpV3 ]--------------------------------------------
#ifdef _SNMPv3
	if(ver == SSNMP_VER3)
	{
		static bool bOne = false;
		if(!bOne)
		{
			bOne = true;
			v3MP *v3_MP;
			// MUST create a dummy v3MP object if _SNMPv3 is enabled!
			int construct_status;
			v3_MP = new v3MP("dummy", 0, construct_status);
			if (construct_status != SNMPv3_MP_OK)
			{
				cout << "Error initializing v3MP: " << construct_status << endl;
				return -3;
			}
		}
	}
#endif

	//--------[ build up SNMP++ object needed ]-------------------------------
	address.set_port(port);

	UdpAddressCollection addresses;

	// do multiple loops as requested from "retries"
	for (int loops=1; loops<= retries + 1; ++loops)
	{
		status = snmp.broadcast_discovery(addresses,
			(timeout + 99) / 100,
			address, version, &community);

		if (status == SNMP_CLASS_SUCCESS)
		{
//			cout << "SNMP++ Success sending broadcast " << loops << "." << endl;
		}
		else
		{
			LOGERROR("SNMP++ Broadcast Error,%d, %s",status,snmp.error_msg( status));
		}
	}

	// filter out duplicates
	UdpAddressCollection filtered_addrs;
	int dummy_pos;

	for (int n=0; n < addresses.size(); ++n)
	{
		if (!filtered_addrs.find(addresses[n], dummy_pos))
			filtered_addrs += addresses[n];
	}

	// print out all addressess
	//cout << "Found " << filtered_addrs.size() << " agents." << endl;
	for (int m=0; m < filtered_addrs.size(); ++m)
	{
		SString str = filtered_addrs[m].get_printable();
		slResult.append(str);
// 		cout << "Answer received from: " << filtered_addrs[m].get_printable()
// 		<< endl;

	}
	return slResult.count();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取交换机信息
// 作    者:  邵凯田
// 创建时间:  2015-12-2 10:21
// 参数说明:  @pInfo为信息指针
// 返 回 值:  >0表示成功，=0表示没有有效端口，<0表示失败
//////////////////////////////////////////////////////////////////////////
int SSnmpClient::GetSwitchInfo(stuSnmpSwitchInfo *pInfo)
{
	int ret;
	SString sResult;
	ret = FetchValue(OID_sysUpTime,sResult);
	if(ret > 0)
		pInfo->up_time = sResult;
	else
		return ret;
	if(FetchValue(OID_sysDescr,sResult) > 0)
		pInfo->desc = sResult.trim();
	if(FetchValue(OID_sysObjectID,sResult) > 0)
		pInfo->objid = sResult.trim();
	if(FetchValue(OID_sysContact,sResult) > 0)
		pInfo->contact = sResult.trim();
	if(FetchValue(OID_sysName,sResult) > 0)
		pInfo->name = sResult.trim();
	if(FetchValue(OID_sysLocation,sResult) > 0)
		pInfo->location = sResult.trim();
	if(FetchValue(OID_ifNumber,sResult) > 0)
		pInfo->if_number = sResult.toInt();

	return 1;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取交换机端口信息
// 作    者:  邵凯田
// 创建时间:  2015-12-1 20:44
// 参数说明:  @SwitchPortInfo表示引用返回的结果列表指针
// 返 回 值:  >0表示返回端口数量，=0表示没有有效端口，<0表示失败
//////////////////////////////////////////////////////////////////////////
int SSnmpClient::GetSwitchPortInfo(SPtrList<stuSSnmpSwitchPortInfo>* &pSwitchPortInfo)
{
	int ret,ival;
	SString sResult;
	ret = FetchValue(OID_ifIndex,sResult);
	if(ret <= 0)
		return ret;
	stuSSnmpSwitchPortInfo *pPortInfo;
	int i,cnt = SString::GetAttributeCount(sResult,";");
	for(i=1;i<=cnt;i++)
	{
		if(i>m_SwitchPortInfo.count())
		{
			pPortInfo = new stuSSnmpSwitchPortInfo();
			m_SwitchPortInfo.append(pPortInfo);
		}
		else
			pPortInfo = m_SwitchPortInfo[i-1];
		pPortInfo->port_id = SString::GetIdAttribute(i,sResult,";").toInt();
		
	}

	//端口名
	ret = FetchValue(OID_ifDescr,sResult);
	cnt = SString::GetAttributeCount(sResult,";");
	if(cnt > m_SwitchPortInfo.count())
		cnt = m_SwitchPortInfo.count();
	for(i=1;i<=cnt;i++)
	{
		m_SwitchPortInfo[i-1]->port_name = SString::GetIdAttribute(i,sResult,";");
	}

	//端口速率
	ret = FetchValue(OID_ifSpeed,sResult);
	cnt = SString::GetAttributeCount(sResult,";");
	if(cnt > m_SwitchPortInfo.count())
		cnt = m_SwitchPortInfo.count();
	for(i=1;i<=cnt;i++)
	{
		pPortInfo = m_SwitchPortInfo[i-1];
		m_SwitchPortInfo[i-1]->port_speed = SString::GetIdAttribute(i,sResult,";").toInt();
	}


	//端口连接状态
	ret = FetchValue(OID_ifOperStatus,sResult);
	cnt = SString::GetAttributeCount(sResult,";");
	if(cnt > m_SwitchPortInfo.count())
		cnt = m_SwitchPortInfo.count();
	for(i=1;i<=cnt;i++)
	{
		m_SwitchPortInfo[i-1]->conn_stat = (SString::GetIdAttributeI(i,sResult,";")==1);
	}
	int soc,usec;
	SDateTime::getSystemTime(soc,usec);
	SString crc_error,big_error,short_error,in_byte,in_pkts,in_bcast_pkts,in_mcast_pkts;
	SString pkts_64,pkts_65_127,pkts_128_255,pkts_256_511,pkts_512_1023,pkts_1024_1518;
	FetchValue(OID_CrcError,crc_error);
	FetchValue(OID_BigFrameError,big_error);
	FetchValue(OID_ShortFrameError,short_error);
	FetchValue(OID_etherStatsOctets,in_byte);
	FetchValue(OID_etherStatsPkts,in_pkts);
	FetchValue(OID_etherStatsBroadcastPkts,in_bcast_pkts);
	FetchValue(OID_etherStatsMulticastPkts,in_mcast_pkts);
	FetchValue(OID_etherStatsPkts64Octets,pkts_64);
	FetchValue(OID_etherStatsPkts65to127Octets,pkts_65_127);
	FetchValue(OID_etherStatsPkts128to255Octets,pkts_128_255);
	FetchValue(OID_etherStatsPkts256to511Octets,pkts_256_511);
	FetchValue(OID_etherStatsPkts512to1023Octets,pkts_512_1023);
	FetchValue(OID_etherStatsPkts1024to1518Octets,pkts_1024_1518);

	pSwitchPortInfo = &m_SwitchPortInfo;
	//接收字节数
	if(m_portinfo_last_soc == 0)
	{
		m_portinfo_last_soc = soc;
		m_portinfo_last_usec = usec;
		for(i=1;i<=cnt;i++)
		{
			pPortInfo = m_SwitchPortInfo[i-1];
			ival = SString::GetIdAttribute(i,sResult,";").toInt();
			pPortInfo->sum_in_bytes = SString::GetIdAttribute(i,in_byte,";").toInt();//累计接收字节数
			pPortInfo->sum_in_pkgs = SString::GetIdAttribute(i,in_pkts,";").toInt();//累计接收包数
			pPortInfo->sum_in_bcast_pkgs = SString::GetIdAttribute(i,in_bcast_pkts,";").toInt();//累计广播包数
			pPortInfo->sum_in_mcast_pkgs = SString::GetIdAttribute(i,in_mcast_pkts,";").toInt();//累计多播包数
			pPortInfo->sum_in_pkts_64 = SString::GetIdAttribute(i,pkts_64,";").toInt();//64字节累计字节数
			pPortInfo->sum_in_pkts_65_127 = SString::GetIdAttribute(i,pkts_65_127,";").toInt();//65-127字节累计包数
			pPortInfo->sum_in_pkts_128_255 = SString::GetIdAttribute(i,pkts_128_255,";").toInt();//128-255字节累计包数
			pPortInfo->sum_in_pkts_256_511 = SString::GetIdAttribute(i,pkts_256_511,";").toInt();//256-511字节累计包数
			pPortInfo->sum_in_pkts_512_1023 = SString::GetIdAttribute(i,pkts_512_1023,";").toInt();//512-1023字节累计包数
			pPortInfo->sum_in_pkts_1024_1518 = SString::GetIdAttribute(i,pkts_1024_1518,";").toInt();//1024-1518字节累计包数
			pPortInfo->tmp_last_crc_error = SString::GetIdAttribute(i,crc_error,";").toInt();
			pPortInfo->tmp_last_short_error = SString::GetIdAttribute(i,short_error,";").toInt();
			pPortInfo->tmp_last_big_error = SString::GetIdAttribute(i,big_error,";").toInt();
		}
		return m_SwitchPortInfo.count();
	}

	float secs = (float)(soc-m_portinfo_last_soc)+((float)(usec-m_portinfo_last_usec))/1000000;
	m_portinfo_last_soc = soc;
	m_portinfo_last_usec = usec;
	for(i=1;i<=cnt;i++)
	{
		pPortInfo = m_SwitchPortInfo[i-1];
		ival = SString::GetIdAttribute(i,in_byte,";").toInt();
		//计算入口流量
		pPortInfo->in_real_flow = (float)(ival - pPortInfo->sum_in_bytes)/secs;
		//TODO:缺少out流量
		pPortInfo->all_real_flow = pPortInfo->in_real_flow + pPortInfo->out_real_flow;
		pPortInfo->crc_error = pPortInfo->tmp_last_crc_error != SString::GetIdAttribute(i,crc_error,";").toInt();
		pPortInfo->short_error = pPortInfo->tmp_last_crc_error != SString::GetIdAttribute(i,short_error,";").toInt();
		pPortInfo->big_error = pPortInfo->tmp_last_crc_error != SString::GetIdAttribute(i,big_error,";").toInt();

		pPortInfo->sum_in_bytes = SString::GetIdAttribute(i,in_byte,";").toInt();//累计接收字节数
		pPortInfo->sum_in_pkgs = SString::GetIdAttribute(i,in_pkts,";").toInt();//累计接收包数
		pPortInfo->sum_in_bcast_pkgs = SString::GetIdAttribute(i,in_bcast_pkts,";").toInt();//累计广播包数
		pPortInfo->sum_in_mcast_pkgs = SString::GetIdAttribute(i,in_mcast_pkts,";").toInt();//累计多播包数
		pPortInfo->sum_in_pkts_64 = SString::GetIdAttribute(i,pkts_64,";").toInt();//64字节累计字节数
		pPortInfo->sum_in_pkts_65_127 = SString::GetIdAttribute(i,pkts_65_127,";").toInt();//65-127字节累计包数
		pPortInfo->sum_in_pkts_128_255 = SString::GetIdAttribute(i,pkts_128_255,";").toInt();//128-255字节累计包数
		pPortInfo->sum_in_pkts_256_511 = SString::GetIdAttribute(i,pkts_256_511,";").toInt();//256-511字节累计包数
		pPortInfo->sum_in_pkts_512_1023 = SString::GetIdAttribute(i,pkts_512_1023,";").toInt();//512-1023字节累计包数
		pPortInfo->sum_in_pkts_1024_1518 = SString::GetIdAttribute(i,pkts_1024_1518,";").toInt();//1024-1518字节累计包数
		pPortInfo->tmp_last_crc_error = SString::GetIdAttribute(i,crc_error,";").toInt();
		pPortInfo->tmp_last_short_error = SString::GetIdAttribute(i,short_error,";").toInt();
		pPortInfo->tmp_last_big_error = SString::GetIdAttribute(i,big_error,";").toInt();
	}

	return m_SwitchPortInfo.count();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取交换机端口对应的MAC信息表
// 作    者:  邵凯田
// 创建时间:  2015-12-1 20:47
// 参数说明:  @SwitchPortMacInfo表示引用返回的结果列表
// 返 回 值:  >0表示返回MAC数量，=0表示没有有效MAC，<0表示失败
//////////////////////////////////////////////////////////////////////////
int SSnmpClient::GetSwitchPortMacInfo(SPtrList<stuSSnmpSwitchPortMacInfo> &SwitchPortMacInfo)
{
	int i,cnt;
	SString sMac,sPort,sSrc;
	cnt = FetchValue(OID_MACAddr,sMac);
	if(cnt <= 0)
		return cnt;
	SwitchPortMacInfo.clear();
	SwitchPortMacInfo.setAutoDelete(true);
	stuSSnmpSwitchPortMacInfo *pMac;
	SString str;
	FetchValue(OID_MACAddrPort,sPort);
	FetchValue(OID_MACAddrPortSrc,sSrc);
	for(i=1;i<=cnt;i++)
	{
		pMac = new stuSSnmpSwitchPortMacInfo();
		pMac->ip = 0;
		str = SString::GetIdAttribute(i,sMac,";").trim().left(17).toUpper();
		pMac->mac = str.replace((char*)" ",(char*)":");
		pMac->port_id = SString::GetIdAttributeI(i,sPort,";");
		pMac->is_learned = SString::GetIdAttribute(i,sSrc,";").toLower().find("learned")>=0;
		SwitchPortMacInfo.append(pMac);
	}
	return SwitchPortMacInfo.count();
}

void SSnmpClient::SetConnected(bool b)
{
	if((b && m_iConnected == 1) ||
		(!b && m_iConnected == 0))
		return;
	m_iConnected = b?1:0;
	if(b)
		OnConnected();
	else
		OnDisconnected();
}

SSnmpTrapListenner::SSnmpTrapListenner()
{
	m_pTrapFun = NULL;
	m_pTrapCbParam = NULL;
	m_pSnmp = NULL;
}

SSnmpTrapListenner::~SSnmpTrapListenner()
{
	if(m_pSnmp != NULL)
		StopTrapListen();
}


void SSnmp_Trap_callback( int reason, Snmp *snmp, Pdu &pdu, SnmpTarget &target, void *cd)
{
	Vb nextVb;
	int pdu_error;
	GenAddress addr;
	SSnmpTrapListenner *pThis = (SSnmpTrapListenner*)cd;
	if(pThis->m_pTrapFun == NULL)
	{
		LOGWARN("None callback funcation is active! Ignoro the Trap event!");
	}
	target.get_address(addr);
	UdpAddress from(addr);
	SString oids,sValues;

// 	cout << "reason: " << reason << endl
// 		<< "msg: " << snmp->error_msg(reason) << endl
// 		<< "from: " << from.get_printable() << endl;

	pdu_error = pdu.get_error_status();
	if (pdu_error){
		cout << "Response contains error: " 
			<< snmp->error_msg(pdu_error)<< endl;
	}
	Oid id,ent;
	TimeTicks ts;
	pdu.get_notify_timestamp(ts);
	unsigned long hsecs = (unsigned long)ts;
	pdu.get_notify_id(id);
// 	cout << "ID:  " << id.get_printable() << endl;
// 	cout << "Type:" << pdu.get_type() << endl;
	pdu.get_notify_enterprise(ent);
//	cout << "Enterprise ID:" << ent.get_printable() << endl;
	for (int i=0; i<pdu.get_vb_count(); i++)
	{
		pdu.get_vb(nextVb, i);
		if(i > 0)
		{
			oids += ",";
			sValues += ";";
		}
		oids += nextVb.get_printable_oid();
		sValues += SSnmpClient_GetVbValueStr(nextVb);

// 		cout << "Oid: " << nextVb.get_printable_oid() << endl
// 			<< "Val: " <<  nextVb.get_printable_value() << endl;
	}
	if (pdu.get_type() == sNMP_PDU_INFORM) {
		cout << "pdu type: " << pdu.get_type() << endl;
		cout << "sending response to inform: " << endl;
		nextVb.set_value("This is the response.");
		pdu.set_vb(nextVb, 0);
		snmp->response(pdu, target);
	}
	cout << endl;

	pThis->m_pTrapFun(pThis->m_pTrapCbParam,hsecs,from.get_printable(),id.get_printable(),ent.get_printable(),oids,sValues);
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  开始Trap接收侦听，一个进程只能使用一个Trap帧听
// 作    者:  邵凯田
// 创建时间:  2016-7-11 9:39
// 参数说明:  @pCallbackFun表示回调函数指针
//         :  @pCbParam表示回调参数指针
// 返 回 值:  >0表示成功，<=0表示失败
//////////////////////////////////////////////////////////////////////////
int SSnmpTrapListenner::StartTrapListen(CB_SSnmp_Trap pCallbackFun,void* pCbParam,int trap_port/*=162*/)
{
	if(m_pSnmp != NULL)
		StopTrapListen();
	//----------[ create a SNMP++ session ]-----------------------------------
	int status; 
	Snmp::socket_startup();  // Initialize socket subsystem
	m_pSnmp = new Snmp(status);                // check construction status
	Snmp &snmp = *((Snmp*)m_pSnmp);
	if ( status != SNMP_CLASS_SUCCESS)
	{
		LOGERROR("SSnmpTrapListenner::StartTrapListen error, SNMP++ Session Create Fail, %s",snmp.error_msg(status));
		return -1;
	}

	//---------[ init SnmpV3 ]--------------------------------------------
#if 0
#ifdef _SNMPv3
	const unsigned char engineId[] = 
	{ 0x80, 0x00, 0x13, 0x70, 0xC0, 0xA8, 0x01, 0x0D };
	OctetStr engineID(engineId, 8);
	const char *filename = "snmpv3_boot_counter";
	unsigned int snmpEngineBoots = 0;
	v3MP *v3_MP;

	status = getBootCounter(filename, engineID, snmpEngineBoots);
	if ((status != SNMPv3_OK) && (status < SNMPv3_FILEOPEN_ERROR))
	{
		cout << "Error loading snmpEngineBoots counter: " << status << endl;
		return -2;
	}
	snmpEngineBoots++;
	status = saveBootCounter(filename, engineID, snmpEngineBoots);
	if (status != SNMPv3_OK)
	{
		cout << "Error saving snmpEngineBoots counter: " << status << endl;
		return -3;
	}

	int construct_status;
	v3_MP = new v3MP(engineID, snmpEngineBoots, construct_status);
	if (construct_status != SNMPv3_MP_OK)
	{
		cout << "Error initializing v3MP: " << construct_status << endl;
		return -4;
	}

	USM *usm = v3_MP->get_usm();

	// users at UCD
	usm->add_usm_user("SHADESUser",
		SNMP_AUTHPROTOCOL_HMACSHA, SNMP_PRIVPROTOCOL_DES,
		"The UCD Demo Password", "The UCD Demo Password");

	usm->add_usm_user("SHAMD5User",
		SNMP_AUTHPROTOCOL_HMACMD5, SNMP_PRIVPROTOCOL_DES,
		"The UCD Demo Password", "The UCD Demo Password");

	usm->add_usm_user("noAuthUser",
		SNMP_AUTHPROTOCOL_HMACMD5, SNMP_PRIVPROTOCOL_DES,
		"Password", "Password");

	// testusers
	usm->add_usm_user("unsecureUser",
		SNMP_AUTHPROTOCOL_NONE, SNMP_PRIVPROTOCOL_NONE, 
		"", "");

	usm->add_usm_user("MD5",
		SNMP_AUTHPROTOCOL_HMACMD5, SNMP_PRIVPROTOCOL_NONE,
		"MD5UserAuthPassword", "");

	usm->add_usm_user("SHA",
		SNMP_AUTHPROTOCOL_HMACSHA, SNMP_PRIVPROTOCOL_NONE,
		"SHAUserAuthPassword", "");

	usm->add_usm_user("MD5DES",
		SNMP_AUTHPROTOCOL_HMACMD5, SNMP_PRIVPROTOCOL_DES,
		"MD5DESUserAuthPassword", "MD5DESUserPrivPassword");

	usm->add_usm_user("SHADES",
		SNMP_AUTHPROTOCOL_HMACSHA, SNMP_PRIVPROTOCOL_DES,
		"SHADESUserAuthPassword", "SHADESUserPrivPassword");

	usm->add_usm_user("MD5IDEA",
		SNMP_AUTHPROTOCOL_HMACMD5, SNMP_PRIVPROTOCOL_IDEA,
		"MD5IDEAUserAuthPassword", "MD5IDEAUserPrivPassword");

	usm->add_usm_user("SHAIDEA",
		SNMP_AUTHPROTOCOL_HMACSHA, SNMP_PRIVPROTOCOL_IDEA,
		"SHAIDEAUserAuthPassword", "SHAIDEAUserPrivPassword");
#endif
#endif
	OidCollection oidc;
	TargetCollection targetc;

	cout << "Trying to register for traps on port " << trap_port << "." << endl;
	snmp.notify_set_listen_port(trap_port);
	status = snmp.notify_register(oidc, targetc, SSnmp_Trap_callback, this);
	if (status != SNMP_CLASS_SUCCESS)
	{
		LOGERROR("Error register for notify (%d), %s",status,snmp.error_msg(status));
		return -100;
		//exit(1);
	}


	if(!snmp.start_poll_thread(1000))
	{
		LOGERROR("start_poll_thread error!");
		StopTrapListen();
		return -101;
	}
	m_pTrapFun = pCallbackFun;
	m_pTrapCbParam = pCbParam;
	return 1;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  结束Trap事件帧听
// 作    者:  邵凯田
// 创建时间:  2016-7-11 9:42
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SSnmpTrapListenner::StopTrapListen()
{
	if(m_pSnmp == NULL)
		return;
	Snmp &snmp = *((Snmp*)m_pSnmp);
	snmp.stop_poll_thread();
	delete (Snmp*)m_pSnmp;
	m_pSnmp = NULL;
}


