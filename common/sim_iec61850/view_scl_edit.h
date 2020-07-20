/**
 *
 * 文 件 名 : view_scl_edit.h
 * 创建日期 : 2015-12-29 17:23
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-12-29	邵凯田　创建文件
 *
 **/

#ifndef VIEW_SCL_EDIT_H
#define VIEW_SCL_EDIT_H

#include <QWidget>
#include "ui_view_scl_edit.h"
#include "ssp_baseview.h"
#include "sim_config.h"
#include "sp_sclparser.h"

class view_scl_edit : public CBaseView
{
	Q_OBJECT

public:
	view_scl_edit(QWidget *parent = 0);
	~view_scl_edit();

private:
	Ui::view_scl_edit ui;
};

#endif // VIEW_SCL_EDIT_H
