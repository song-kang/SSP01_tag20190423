/**
 *
 * 文 件 名 : view_sclimport.h
 * 创建日期 : 2015-12-23 13:17
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SCL文件导入
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-12-23	邵凯田　创建文件
 *
 **/

#ifndef VIEW_SCLIMPORT_H
#define VIEW_SCLIMPORT_H

#include <QWidget>
#include "ui_view_sclimport.h"
#include "ssp_baseview.h"
#include "sim_config.h"
#include "sp_sclparser.h"
#include "sp_virtual_terminal.h"

class view_sclimport : public CBaseView
{
	Q_OBJECT

public:
	view_sclimport(QWidget *parent = 0);
	~view_sclimport();

public slots:
	void OnOpen();
	void OnSelAll();
	void OnSelNone();
	void OnSelRev();
	void OnImport();

private:
	static void* ThreadLoadScl(void *lp);
	static void* ThreadImportScl(void *lp);

	SString m_sSclFile;
	int m_iSclLoadState;//0:loading 1:load error, 2:load failed

	SPtrList<CSpSclParser::stuSCL_IED> m_ImportIed;
	int m_iSclImportState;//0:importing, >0导入数量, <0表示失败
	int m_iSclImportPercent;

	Ui::view_sclimport ui;
	CSpSclParserEx* m_pScl;
	CSimConfig *m_pSimConfig;
	float m_fPrimaryVoltage;//一次额定电压
	float m_fSecondaryVoltage;//二次额定电压
	float m_fPriaryCurrent;//一次额定电流
	float m_fSecondaryCurrent;//二次额定电流
};

#endif // VIEW_SCLIMPORT_H
