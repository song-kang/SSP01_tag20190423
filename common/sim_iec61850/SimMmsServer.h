/**
 *
 * �� �� �� : SimMmsServer.h
 * �������� : 2016-9-2 14:27
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : MMS�����ʵ���࣬ʹ��SMmsServer��
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-9-2	�ۿ�������ļ�
 *
 **/
#ifndef __SIM_MMS_SERVER_H__
#define __SIM_MMS_SERVER_H__

#include "sim_config.h"
#include "SMmsServer.h"
#include "ssp_gui.h"
class CSimMmsServer : public SMmsServer
{
public:
	CSimMmsServer();
	virtual ~CSimMmsServer();

	////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��������,�麯�������������б����ȵ��ô˺���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-9-2 14:27
	// ����˵��:  void
	// �� �� ֵ:  true��ʾ���������ɹ�,false��ʾ��������ʧ��
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();


	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  �����ı��麯�������ط���ʱʹ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2017-4-13 15:10
	// ����˵��:  @sTipText
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnProgressText(SString sTipText)
	{
		SSP_GUI->SetStatusTip(sTipText);
		SQt::ProcessUiEvents();
	};

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ӳ��Ҷ�ӽڵ��û�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-9-2 14:27
	// ����˵��:  @sIedLdName��ʾIED+LD����
	//         :  @sLeafPath��ʾҶ�ӽڵ�·��
	//         :  @ppLeafMap��ʾҶ�ӽڵ���û�ӳ��ָ��ĵ�ַ��ӳ��ָ��Ϊ�գ��û�������Ƿ���Ҫӳ�䣬��Ҫʱ����stuLeafMap������дָ�룬MMS�����ڲ��������ͷ�
	// �� �� ֵ:  true��ʾ�ɹ�ӳ�䣬false��ʾ�޷�ӳ��
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnMapLeafToUser(char* sIedLdName, char* sLeafPath, stuLeafMap **ppLeafMap);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���ӷ��ʵ�IP��ַ
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-11-12 14:42
	// ����˵��:  void
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void AddApIpAddr();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ɾ�����ʵ�IP
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-11-12 14:42
	// ����˵��:  void
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void RemoveApIpAddr();

	SPtrList<stuLeafMap> m_StateLeafMap;//״̬��Ҷ��ӳ��
	SPtrList<stuLeafMap> m_AnalogLeafMap;//ģ����Ҷ��ӳ��

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ģ���̣߳���ʱ�����źŻ����ֵ�ı仯
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-9-2 14:39
	// ����˵��:  @lp
	// �� �� ֵ:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadSim(void *lp);
	CSimConfig *m_pSimConfig;
};
#endif