/**
 *
 * 文 件 名 : view_user_mgr.h
 * 创建日期 : 2015-7-8 17:30
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 用户管理
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-8	邵凯田　创建文件
 *
 **/

#ifndef __VIEW_USER_MGR_H__
#define __VIEW_USER_MGR_H__

#include <QWidget>
#include "ui_view_user_mgr.h"
#include "ssp_baseview.h"
class view_user_mgr : public CBaseView
{
	Q_OBJECT

public:
	view_user_mgr(QWidget *parent = 0);
	virtual ~view_user_mgr();

private:
	Ui::view_user_mgr ui;
};

#endif // __VIEW_USER_MGR_H__
