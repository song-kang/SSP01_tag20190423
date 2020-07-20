/**
 *
 * 文 件 名 : ssp_uiframe.h
 * 创建日期 : 2015-07-08 13:51
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 界面框架类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-07-08	邵凯田　创建文件
 *
 **/

#ifndef __UIFRAME_H__
#define __UIFRAME_H__

#include <QtGui>
#include "SLog.h"
#include "SList.h"
#include "SString.h"
#include "ssp_baseview.h"
#include "ssp_gui.h"
#include "ssp_gui_inc.h"
#include "BuddyWidgetServer.h"

class CModuleMgr;
class CSspUiFrame;
class CSspPluginMgr;

class SSP_GUI_EXPORT CUiFrameLoading : public QWidget
{
	Q_OBJECT
public:
	CUiFrameLoading(QWidget *parent = 0);
	virtual ~CUiFrameLoading();

	void SetProgress(SString sTip);
	void Stop();
	void postSetTipText()
	{
		emit sigSetTipText();
	}
signals:
	void sigSetTipText();
public slots:
	void OnSetTipText();
public:
	SString m_sTipText;
	SString m_sSysName;
private:
	QLabel *m_label;
	QMovie *m_movie;
	QLabel *m_labelSysName;
	//QLabel *m_labelTipWait;
	QLabel *m_labelTip;
};


class SSP_GUI_EXPORT CUiFrameWaiting : public QWidget
{
	Q_OBJECT
public:
	CUiFrameWaiting(QWidget *parent = 0);
	virtual ~CUiFrameWaiting();
	void ShowWaiting();
	void HideWaiting();
private:
	QLabel *m_label;
	QMovie *m_movie;
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CUiFrameTop
// 作    者:  邵凯田
// 创建时间:  2015-07-08 14:02
// 描    述:  框架顶部窗口
//////////////////////////////////////////////////////////////////////////
class SSP_GUI_EXPORT CUiFrameTop : public QWidget
{
	Q_OBJECT
public:
	friend class ssp_gui;
	friend class CSspUiFrame;
	CUiFrameTop(QWidget *parent = 0);
	virtual ~CUiFrameTop();

	void paintEvent(QPaintEvent *event);

	virtual void  mouseMoveEvent ( QMouseEvent * event ) ;
	virtual void  mousePressEvent ( QMouseEvent * event ) ;
	virtual void  mouseReleaseEvent ( QMouseEvent * event ) ;
public slots:
	void OnTimerUI();
	void OnLogin();
public:
	SString m_sCurrModDesc;//当前进入模块的描述
private:
	QPixmap m_bmpTop,m_bmpTopNoMenu,m_bmpTopFull,m_bmpTopRight,m_bmpButton,m_bmpButtonSel;
	QPen m_penTextMod,m_penLine;
	QFont m_ModFont;
	CSspUiFrame *m_pUiFrame;
	QTimer *m_ptimerUI;
	QVBoxLayout *m_verticalLayout_2;
	QSpacerItem *m_verticalSpacer;
	QWidget *m_pTopWidget;
	QHBoxLayout *m_horizontalLayout;
	QSpacerItem *m_horizontalSpacer;
	QLabel *m_plabLoginIfo;
	SQLabel *m_plabDoLogin;
// 	QLabel *m_plabTime;
// 	QLabel *m_plabTime2;
	int m_iModLeft;
};

class CFrameMenuL1;
class CFrameFunModule;


//////////////////////////////////////////////////////////////////////////
// 名    称:  CFrameMenuL2
// 作    者:  邵凯田
// 创建时间:  2015-07-08 14:58
// 描    述:  二级菜单
//////////////////////////////////////////////////////////////////////////
class SSP_GUI_EXPORT CFrameMenuL2
{
public:
	CFrameMenuL2();
	~CFrameMenuL2();

	SString m_sFunName;//功能视图名称，用于实例功能窗口时使用
	SString m_sTitle;//菜单标题
	SString m_sDesc;//菜单描述
	int m_iType;//功能点类型：
				// 		0-	表示静态功能点
				// 		1-	表示动态菜单项（必须包含子节点，不对应实际功能）
				// 		2-	表示组态界面功能点
				// 		3-	表示动态报表功能点
				//		4-	隐式功能点(不用于界面显示，用于隐式的功能点权限)
				//		5-WEB页面(path中为URL，URL中可以包含动态会话属性{@SESSION:属性名@}和全局配置{@SETUP:配置名@})
				//		6-JAVA三维场景界面类（在path中配置类名，包含完整包名称）
	int m_iRefSn;//功能点引用序号：
				// 		type=0/1时无效
				// 		type=2时对应T_UICFG_WND的wnd_sn
				// 		type=3时对应T_REPORT的rpt_sn
	SString m_sPath;//关联路径（备用）
	CBaseView *m_pFunView;//当前菜单对应的功能窗口
	CFrameMenuL1 *m_pL1;//上级菜单
	unsigned char* img_normal_buff;
	int img_normal_len;
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CFrameMenuL1
// 作    者:  邵凯田
// 创建时间:  2015-07-08 14:59
// 描    述:  一级菜单
//////////////////////////////////////////////////////////////////////////
class SSP_GUI_EXPORT CFrameMenuL1
{
public:
	CFrameMenuL1();
	~CFrameMenuL1();

	SPtrList<CFrameMenuL2> m_SubMenus;
	SString m_sTitle;//菜单标题
	bool m_bOpened;//二级菜单是否已经展开
	CFrameFunModule *m_pMod;//上级模块
	unsigned char* img_normal_buff;
	int img_normal_len;
	SString m_sFunName;
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CFrameFunModule
// 作    者:  邵凯田
// 创建时间:  2015-07-08 15:13
// 描    述:  功能模块定义，一个功能模块中对应若干个一级菜单
//////////////////////////////////////////////////////////////////////////
class SSP_GUI_EXPORT CFrameFunModule
{
public:
	CFrameFunModule();
	~CFrameFunModule();
	bool IsExistL1(CFrameMenuL1 *pL1);
	bool IsExistL2(CFrameMenuL2 *pL2);
	void CheckLastMenuL2Valid();
	SPtrList<CFrameMenuL1> m_Menus;
	CFrameMenuL2 *m_pLastMenuL2;//最后显示的二级菜单内容

	int m_iViewCount;//当前功能模块下的视图数量
	SString m_sTitle;//模块标题

	unsigned char* img_normal_buff;
	int img_normal_len;
	unsigned char* img_hover_buff;
	int img_hover_len;
	unsigned char* img_down_buff;
	int img_down_len;
	SString m_sFunName;
};


//////////////////////////////////////////////////////////////////////////
// 名    称:  CUiFrameLeft
// 作    者:  邵凯田
// 创建时间:  2015-07-08 14:49
// 描    述:  左边菜单窗口
//////////////////////////////////////////////////////////////////////////
class SSP_GUI_EXPORT CUiFrameLeft : public QWidget
{
	Q_OBJECT
public:
	friend class ssp_gui;
	friend class CSspUiFrame;
	CUiFrameLeft(QWidget *parent = 0);
	virtual ~CUiFrameLeft();

	void paintEvent(QPaintEvent *event);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  绘制菜单
	// 作    者:  邵凯田
	// 创建时间:  2015-07-08 13:15
	// 参数说明:  @pModule为当前模块指针
	//         :  @painter为画板
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void paintMenu(CFrameFunModule *pModule,QPainter &painter);

	virtual void  mouseMoveEvent ( QMouseEvent * event ) ;
	virtual void  mousePressEvent ( QMouseEvent * event ) ;
	virtual void  mouseReleaseEvent ( QMouseEvent * event ) ;
	virtual void  wheelEvent ( QWheelEvent * event );

private:
	QPixmap m_bmpMenuTop,m_bmpMenuL1Open,m_bmpMenuL1Close,m_bmpMenuBody,m_bmpMenuL2,m_bmpMenuL2Act,m_bmpMenuBottom;
	int m_iOffsetY;//Y方向的偏移量，
	int m_iNeedHeight;//Y方向需要的总高度
	CSspUiFrame *m_pUiFrame;
	QPen m_penTextL1,m_penTextL1Sel,m_penTextL2,m_penTextL2Sel;
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CUiFrameBottom
// 作    者:  邵凯田
// 创建时间:  2015-07-08 15:45
// 描    述:  框架底部
//////////////////////////////////////////////////////////////////////////
class SSP_GUI_EXPORT CUiFrameBottom : public QFrame
{
	Q_OBJECT
public:
	friend class ssp_gui;
	friend class CSspUiFrame;
	CUiFrameBottom(QWidget *parent = 0);
	virtual ~CUiFrameBottom();

	//void paintEvent(QPaintEvent *event);
	void SetTip(SString &tip);

private:
	QPixmap m_bmpBottom,m_bmpConnStation;
	CSspUiFrame *m_pUiFrame;
	QSpacerItem *horizontalSpacer_2;
	QHBoxLayout *horizontalLayout;
	QLabel *m_labTip/*,*m_labTipRight,*m_labNull*/;
	//QFrame *m_frmPicture;
};


//////////////////////////////////////////////////////////////////////////
// 名    称:  CUiFrameBody
// 作    者:  邵凯田
// 创建时间:  2013:9:2 15:25
// 描    述:  框架底部
//////////////////////////////////////////////////////////////////////////
class SSP_GUI_EXPORT CUiFrameBody : public QWidget
{
	Q_OBJECT
public:
	friend class ssp_gui;
	friend class CSspUiFrame;
	CUiFrameBody(QWidget *parent = 0);
	virtual ~CUiFrameBody();
	void AddWidget(QWidget *p);
private:
	CSspUiFrame *m_pUiFrame;
	QHBoxLayout *horizontalLayout;
};


//////////////////////////////////////////////////////////////////////////
// 名    称:  CUiFrame
// 作    者:  邵凯田
// 创建时间:  2015-07-08 14:02
// 描    述:  框架窗口
//////////////////////////////////////////////////////////////////////////
class SSP_GUI_EXPORT CSspUiFrame : public QWidget
{
	Q_OBJECT

public:
	friend class ssp_gui;
	friend class CUiFrameLeft;
	CSspUiFrame(QWidget *parent = 0);
	virtual ~CSspUiFrame();
	virtual void Start();
	bool WaitForBackStart();
	bool HideStartWnd();
	void SetStartSysName(SString sSysName);
	void SetStartTipText(SString sTipText);
	static CSspUiFrame* GetPtr();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  后台启动函数，不阻塞启动动画
	// 作    者:  邵凯田
	// 创建时间:  2016-1-19 17:07
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	virtual bool BackStart()
	{
		LOGDEBUG("into CSspUiFrame::BackStart()");
		SApplication::GetPtr()->BackStart();
		return true;
	};

	virtual void resizeEvent(QResizeEvent *e);
	void frame_resize();

	virtual void AddStatusWidget(QWidget *pWidget){m_pWndBottom->horizontalLayout->addWidget(pWidget);};

	////////////////////////////////////////////////////////////////////////
	// 描    述:  执行模块的点击操作
	// 作    者:  邵凯田
	// 创建时间:  2013:9:3 9:23
	// 参数说明:  @pMod为模块指针
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void OnFunModule(CFrameFunModule *pMod);


	////////////////////////////////////////////////////////////////////////
	// 描    述:  执行二级菜单的点击操作
	// 作    者:  邵凯田
	// 创建时间:  2013:9:2 15:00
	// 参数说明:  pMenuL2为二级菜指针
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void OnFunView(CFrameMenuL2 *pMenuL2);

	void paintall()
	{
		m_pWndLeft->repaint();
		m_pWndTop->repaint();
	}

	////////////////////////////////////////////////////////////////////////
	// 描    述:  设备状态栏提示信息
	// 作    者:  邵凯田
	// 创建时间:  2013:9:10 14:53
	// 参数说明:  @tip为提示文本
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetTip(SString &tip);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  刷新全部视图
	// 作    者:  邵凯田
	// 创建时间:  2013:10:9 14:12
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void RefreshAllView();


	////////////////////////////////////////////////////////////////////////
	// 描    述:  初始化模块及一二级菜单
	// 作    者:  邵凯田
	// 创建时间:  2015-07-08 13:12
	// 参数说明:  mods存储模块的引用变量
	// 返 回 值:  true表示成功，false表示失败
	//////////////////////////////////////////////////////////////////////////
	bool Init(const char* sFunctionPointXml,SPtrList<CFrameFunModule> &mods);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  根据功能名称创建新视图
	// 作    者:  邵凯田
	// 创建时间:  2015-07-08 14:56
	// 参数说明:  @pFunMenu表示功能菜单指针
	//         :  @parent为父窗口指针
	// 返 回 值:  CBaseView*
	//////////////////////////////////////////////////////////////////////////
	virtual CBaseView* NewView(CFrameMenuL2 *pFunMenu,QWidget *parent);


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  初始化模块
	// 作    者:  邵凯田
	// 创建时间:  2015-7-8 16:26
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	virtual void InitModule(const char* sFunctionPointXml);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据指定的名称查找二级菜单
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 16:30
	// 参数说明:  @sName为功能点名称
	// 返 回 值:  CFrameMenuL2*
	//////////////////////////////////////////////////////////////////////////
	CFrameMenuL2* SearchMenul2(SString sName);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  跳转到指定功能点的界面视图
	// 作    者:  邵凯田
	// 创建时间:  2015-8-5 14:41
	// 参数说明:  @sFunName为功能点名称，对应CFrameMenuL2的名称
	// 返 回 值:  true表示跳转成功,false表示跳转失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool GotoView(SString sFunName);
	void ShowWaiting(){m_Waiting.ShowWaiting();};
	void HideWaiting(){m_Waiting.HideWaiting();};
	CUiFrameWaiting* GetWaitingWidget(){return &m_Waiting;};
	CUiFrameLoading* GetLoadingWidget(){return &m_Loading;};

	CUiFrameBody* GetBodyWidget(){return m_pWndBody;};
private:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  初始化线程
	// 作    者:  邵凯田
	// 创建时间:  2015-11-22 17:05
	// 参数说明:  @lp为this指针
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadLoading(void* lp);
	bool m_bLoading;//是否正在加载
	bool m_bLoadSuccessed;//是否加载成功

	CUiFrameTop *m_pWndTop;
	CUiFrameLeft *m_pWndLeft;
	CUiFrameBottom *m_pWndBottom;
	CUiFrameBody *m_pWndBody;
	CUiFrameLoading m_Loading;
	QTimer m_TimerLoadHide;
	int m_iStartWndAlpha;
	CUiFrameWaiting m_Waiting;
	bool m_bOpenningView;
	SBuddyWidgetServer m_BuddyWidgetServer;
	int m_iStartState;//0:尚未启动 1:开始后台启动 2：后台启动完成
public slots:
	void OnTimerLoadHide();
public:
	CFrameFunModule *m_pThisFunModule;//当前界面打开的模块指针
	CFrameFunModule *m_pHotFunModule;//当前鼠标移动到的模块
	SPtrList<CFrameFunModule> m_FunMods;//各功能模块
	CFrameMenuL2 *m_pLastMenuL2;//最后显示的二级菜单内容
	SPtrList<CFrameMenuL2> m_MenuHistory;//打开界面的历史记录，通过它可以前进后退
//	BaseUpModule *m_pUpModule;//上行基类

	SString m_sUserCode;//当前登录的用户名
	time_t m_iLoginTime;//最后登录时间
	bool m_bFullScreen;//是否全屏显示，true表示全屏，false表示不全屏
	SString m_sSysName;
	SString m_sGotoFunPoint;//跳转用的临时功能点
	CSspPluginMgr *m_pluginMgr;
	unsigned char* img_normal_buff;
	int img_normal_len;
};

#endif // __UIFRAME_H__
