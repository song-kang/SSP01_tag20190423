/**
 *
 * 文 件 名 : viewabout.h
 * 创建日期 : 2013-9-13 8:52
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 关于窗口
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2013-9-13	邵凯田　创建文件
 *
 **/

#ifndef VIEWABOUT_H
#define VIEWABOUT_H

#include <QWidget>
#include <QWebView>
#include "ui_viewabout.h"
#include "ssp_baseview.h"

class CAboutPic : public QWidget
{
	Q_OBJECT
public:
	CAboutPic(QWidget *parent = 0);
	virtual ~CAboutPic();
	void paintEvent(QPaintEvent *event);

	QPixmap m_bmpAbout;

};

class CViewAbout : public CBaseView
{
	Q_OBJECT

public:
	CViewAbout(QWidget *parent = 0);
	virtual ~CViewAbout();
	virtual void OnPreShow();
	virtual void OnRefresh();
private:
	Ui::CViewAbout ui;
	CAboutPic *m_pAboutPic;
};


class CViewHelp : public CBaseView
{
	Q_OBJECT

public:
	CViewHelp(QWidget *parent = 0);
	virtual ~CViewHelp();
	virtual void OnPreShow();
	virtual void OnRefresh();
private:
	QWebView *m_pWebView;
	QGridLayout *m_gridLayout;
};

#endif // VIEWABOUT_H
