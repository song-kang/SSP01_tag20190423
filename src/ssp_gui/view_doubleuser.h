/**
 *
 * 文 件 名 : view_doubleuser.h
 * 创建日期 : 2015-7-30 16:42
 * 作    者 : 褚冰冰
 * 修改日期 : 2015-7-30 16:42
 * 当前版本 : 1.0
 * 功能描述 : 双用户登录界面
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-30 	褚冰冰　双用户登录界面
 *
 **/
#ifndef VIEW_DOUBLEUSER_H
#define VIEW_DOUBLEUSER_H

#include <QWidget>
#include "ui_view_doubleuser.h"
#include "pwd_login.h"
class view_doubleUser : public QWidget,public pwd_login
{
	Q_OBJECT
signals:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  登录信息信号
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 16:44
	// 参数说明:  @pwd_login::LOGSTATE 登录状态
	//         :  @SString 登录信息
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void loginSucessed(pwd_login::LOGSTATE,SString);
public:
	view_doubleUser(QWidget *parent = 0);
	virtual ~view_doubleUser();
public slots:
	void on_pushButton_pressed();
	void on_pushButton_2_pressed()
	{
		((QWidget*)this->parent())->close();
	}
private:
	Ui::view_doubleUser ui;
	SDatabase*m_db;
	SString m_in[2];
};

#endif // VIEW_DOUBLEUSER_H
