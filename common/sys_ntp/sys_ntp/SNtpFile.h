#include "SApi.h"
#include "SNtp.h"
#include "sp_config_mgr.h"
class SNtpFile : public CConfigBase
{
public:
	SNtpFile();
	virtual ~SNtpFile();


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  加载配置文件
	// 作    者:  高长春
	// 创建时间:  2019-4-1 9:03
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Load(SString sPathFile);

	static void* ThreadStart(void *lp);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据配置文件值选择是否运行ntp客户端、服务端
	// 作    者:  高长春
	// 创建时间:  2019-4-1 9:03
	// 参数说明:  
	// 返 回 值:  true表示正常守护运行，false表示需要中止运行
	//////////////////////////////////////////////////////////////////////////
	bool Run(int argc, char* argv[]);

	
	SNtpClient *m_SntpClient;
	SNtpServer *m_SntpServer;
	bool m_bOpenClient;
	bool m_bOpenServer;
	int m_iTimeSecs;//读取时间间隔，单位为秒
	SDateTime m_Now;
	SLock m_Lock;//配置锁
};