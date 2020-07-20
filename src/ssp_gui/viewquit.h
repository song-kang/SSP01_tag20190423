/**
 *
 * 文 件 名 : viewquit.h
 * 创建日期 : 2013-9-22 13:22
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 退出系统的几个功能
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2013-9-22	邵凯田　创建文件
 *
 **/

#ifndef VIEWQUIT_H
#define VIEWQUIT_H

#include <QWidget>
#include "ui_viewquit.h"
#include "ssp_baseview.h"

class CViewQuit : public CBaseView
{
	Q_OBJECT

public:
	CViewQuit(QWidget *parent = 0);
	virtual ~CViewQuit();

	virtual void OnRefresh(){};
public slots:
	void OnResetHmi();
	void OnQuitHmi();
	void OnResetSys();
	void OnHaltSys();

private:
	Ui::CViewQuit ui;
};

#endif // VIEWQUIT_H
