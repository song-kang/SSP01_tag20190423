/**
 *
 * 文 件 名 : GuiFrame.h
 * 创建日期 : 2015-7-30 15:53
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 图形界面的显示框架派生类，用于框架窗口及所有子视窗的管理
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-30	邵凯田　创建文件
 *
 **/

#ifndef __TEST_GUI_FRAME_H__
#define __TEST_GUI_FRAME_H__

#include "ssp_uiframe.h"
class CGuiFrame : public CSspUiFrame
{
public:
	CGuiFrame(QWidget *parent=0);
	virtual ~CGuiFrame(void);

	virtual CBaseView* NewView(CFrameMenuL2 *pFunMenu,QWidget *parent);
};

#endif//__TEST_GUI_FRAME_H__
