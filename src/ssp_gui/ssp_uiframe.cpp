/**
 *
 * 文 件 名 : ssp_uiframe.cpp
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
#include "ssp_uiframe.h"
#include "view_user_mgr.h"
#include "view_common.h"
#include "view_nocommon.h"
#include "view_query.h"
#include "ssp_gui.h"
#include "ssp_datawindow.h"
#include "ssp_funpoint.h"
#include "view_mdb_svg.h"
#include "view_uicfg.h"
#include "view_datawindow_edit.h"
#include "view_svglib_edit.h"
#include "view_setup.h"
#include "view_setup_setting.h"
#include "view_reportsetting.h"
#include "view_reportshow.h"
#include "viewabout.h"
#include "viewmoduleinfo.h"
#include "viewquit.h"
#include "ssp_plugin_mgr.h"
#include "view_plugin.h"

CUiFrameLoading::CUiFrameLoading(QWidget *parent)
{
	S_UNUSED(parent);
	/* m_label = new QLabel(this);
	 this->setFixedSize(547,317);
	 QDesktopWidget *desk = QApplication::desktop();
	 move((desk->width()-width())/2,(desk->height()-height())/2);
	 //move((desk->width()-1000)/2>0?(desk->width()-1000)/2:0,desk->height()-400);

	 //this->setWindowFlags(Qt::SplashScreen|Qt::Tool|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
	 this->setWindowFlags(Qt::Tool|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
	 this->setAutoFillBackground(true);
	 this->setContentsMargins(0,0,0,0);
	 m_label->setContentsMargins(0,0,0,0);
	 m_movie=new QMovie(":/ssp_gui/Resources/load.gif");
	 m_label->setMovie(m_movie);
	 m_movie->start();
	 m_labelTipWait = new QLabel(this);
	 m_labelTipWait->setText(tr("正在加载，请稍候..."));
	 m_labelTipWait->setGeometry(QRect(175, 163, 350, 16));
	 QFont font;
	 font.setFamily(tr("黑体"));
	 font.setPointSize(12);
	 m_labelTipWait->setFont(font);	 
	 m_labelTipWait->setStyleSheet(QString::fromUtf8("color: rgb(20, 93, 25);"));

	 m_labelTip = new QLabel(this);
	 m_labelTip->setText(tr("系统启动准备..."));
	 m_labelTip->setGeometry(QRect(175, 189, 350, 16));
	 QFont font2;
	 font2.setFamily(tr("宋体"));
	 font2.setPointSize(11);
	 m_labelTip->setFont(font2);	 
	 m_labelTip->setStyleSheet(QString::fromUtf8("color: rgb(66, 141, 199);"));
	 
	 m_labelSysName = new QLabel(this);
	 m_labelSysName->setText(tr("SSP01系列软件平台"));
	 m_labelSysName->setGeometry(QRect(12, 280, 500, 21));
	 QFont font1;
	 font1.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
	 font1.setPointSize(14);
	 font1.setBold(true);
	 font1.setWeight(75);
	 m_labelSysName->setFont(font1);
	 m_labelSysName->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 127);"));
	 connect(this,SIGNAL(sigSetTipText()),this,SLOT(OnSetTipText()));
	 show();*/


	//----------new modify
	m_sTipText = "";
	m_sSysName = "";
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::Widget);
	this->setMinimumSize(QSize(550, 320));
	this->setMaximumSize(QSize(550, 320));
	QDesktopWidget *desk = QApplication::desktop();
	move((desk->width()-width())/2,(desk->height()-height())/2);

	this->setAutoFillBackground(true);
	QPalette palette;
	palette.setBrush(QPalette::Background, QBrush(QPixmap(":/resource/Resource/bg.png")));
	this->setPalette(palette);

	QPushButton *pushButton;

	pushButton = new QPushButton(this);
	pushButton->setObjectName(QString::fromUtf8("pushButton"));
	pushButton->setGeometry(QRect(10, 10, 128, 40));
	pushButton->setStyleSheet(QString::fromUtf8("QPushButton{border-image: url(:/resource/Resource/logo1.png);background-color: transparent;}\n"
		""));
	m_label = new QLabel(this);
	m_label->setContentsMargins(0,0,0,0);
	m_label->setMinimumSize(QSize(24, 24));
	m_label->setMaximumSize(QSize(24, 24));
	m_movie=new QMovie(":/resource/Resource/load.gif");
	m_label->setMovie(m_movie);
	m_label->setGeometry(QRect(30, 154, 24, 24));
	m_label->setScaledContents(true);
	m_movie->start();
	
	m_labelTip = new QLabel(this);
	m_labelTip->setText(tr("正在加载，请稍候..."));
	m_labelTip->setGeometry(QRect(60, 158, 131, 16));
	QFont font;
	m_labelTip->setFont(font);
	m_labelTip->setStyleSheet(QString::fromUtf8("font-size:12px;\n"
		"color:rgb(55,148,255);"));
	m_labelSysName = new QLabel(this);
	m_labelSysName->setText(tr("SSP01系列软件平台"));
	m_labelSysName->setGeometry(QRect(30, 120, 272, 18));
	QFont font1;
	font1.setBold(true);
	font1.setWeight(75);
	m_labelSysName->setFont(font1);
	m_labelSysName->setStyleSheet(QString::fromUtf8("font-size:16px;font-weight:bold;color:#052370;"));

	//设置关于窗体为圆角

	QBitmap bmp(this->size());

	bmp.fill();

	QPainter p(&bmp);

	p.setPen(Qt::NoPen);

	p.setBrush(Qt::black);

	p.drawRoundedRect(bmp.rect(),10,10);

	setMask(bmp);

	connect(this,SIGNAL(sigSetTipText()),this,SLOT(OnSetTipText()));
	show();
}
void CUiFrameLoading::OnSetTipText()
{
	m_labelTip->setText(m_sTipText.data());
	m_labelSysName->setText(m_sSysName.data());
}


CUiFrameLoading::~CUiFrameLoading()
{
	delete m_movie;
}

void CUiFrameLoading::SetProgress(SString sTip)
{
	S_UNUSED(sTip);
}

void CUiFrameLoading::Stop()
{

}


CUiFrameWaiting::CUiFrameWaiting(QWidget *parent)
{
	S_UNUSED(parent);
	m_label = new QLabel(this);
	this->setFixedSize(100,100);
	QDesktopWidget *desk = QApplication::desktop();
	move((desk->width()-width())/2,(desk->height()-height())/2);
	//move((desk->width()-1000)/2>0?(desk->width()-1000)/2:0,desk->height()-400);

	this->setWindowFlags(Qt::Tool|Qt::Dialog | Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);///*Qt::SplashScreen|*/Qt::Tool|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
	//this->setAutoFillBackground(true);
	setAttribute(Qt::WA_TranslucentBackground, true);
	this->setContentsMargins(0,0,0,0);
	m_label->setContentsMargins(0,0,0,0);
	//this->setWindowOpacity(0.8);
	//setStyleSheet("background-color: rgb(200,200,200);");//55, 135, 215
	setStyleSheet("background-color: transparent;");
	m_label->setStyleSheet("background-color: transparent;");
	m_movie=new QMovie(":/ssp_gui/Resources/loading.gif");
	m_label->setMovie(m_movie);	
	hide();
}

CUiFrameWaiting::~CUiFrameWaiting()
{
	delete m_movie;
}
void CUiFrameWaiting::ShowWaiting()
{
	m_movie->start();
	show();
	SQt::ProcessUiEvents();
}

void CUiFrameWaiting::HideWaiting()
{
	m_movie->stop();
	hide();
	SQt::ProcessUiEvents();
}

//////////////////////////////////////////////////////////////////////////
// 名    称:  CUiFrameTop
// 作    者:  邵凯田
// 创建时间:  2015-07-08 14:02
// 描    述:  框架顶部窗口
//////////////////////////////////////////////////////////////////////////
CUiFrameTop::CUiFrameTop(QWidget *parent) : QWidget(parent)
{
	if(!m_bmpTop.load(":/ssp_gui/Resources/top.bmp"))
	{
		LOGWARN("load Resources/top.bmp error!");
	}
	if(!m_bmpTopNoMenu.load(":/ssp_gui/Resources/top_nomenu.bmp"))
	{
		LOGWARN("load Resources/top_nomenu.bmp error!");
	}
	if(!m_bmpTopFull.load(":/ssp_gui/Resources/top_full.bmp"))
	{
		LOGWARN("load Resources/top_full.bmp error!");
	}
	if(!m_bmpTopRight.load(":/ssp_gui/Resources/top_right.bmp"))
	{
		LOGWARN("load Resources/top_right.bmp error!");
	}
	if(!m_bmpButton.load(":/ssp_gui/Resources/button.png"))
	{
		LOGWARN("load Resources/button.png error!");
	}
	if(!m_bmpButtonSel.load(":/ssp_gui/Resources/button_sel.png"))
	{
		LOGWARN("load Resources/button_sel.png error!");
	}
	m_penTextMod = QPen(QColor(50,50,50));
	m_penLine = QPen(QColor(0,0,0));
	m_ModFont = QFont(tr("黑体"),10,QFont::Bold);//QFont::Bold
	setMouseTracking(true);
	//m_plabTime = new QLabel(this);

	m_verticalLayout_2 = new QVBoxLayout(this);
	m_verticalLayout_2->setSpacing(0);
	m_verticalLayout_2->setContentsMargins(11, 11, 11, 11);
	m_verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
	m_verticalLayout_2->setContentsMargins(0, 0, 0, 0);

	m_pTopWidget = new QWidget(this);
	m_pTopWidget->setObjectName(QString::fromUtf8("topwidget"));
	m_pTopWidget->setMinimumSize(QSize(0, 15));
	//m_pTopWidget->setGeometry(QRect(10, 20, 541, 41));
	m_horizontalLayout = new QHBoxLayout(m_pTopWidget);
	m_horizontalLayout->setSpacing(6);
	m_horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
	m_horizontalLayout->setContentsMargins(0, 0, 0, 0);
	m_horizontalSpacer = new QSpacerItem(262, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_horizontalLayout->addItem(m_horizontalSpacer);

	m_plabLoginIfo = new QLabel(m_pTopWidget);
	m_plabLoginIfo->setObjectName(QString::fromUtf8("labelLoginInfo"));
	m_plabLoginIfo->setAlignment(Qt::AlignRight|Qt::AlignTop);
	m_horizontalLayout->addWidget(m_plabLoginIfo);

	if(GET_DB_CFG != NULL && DB != NULL && DB->GetDatabasePool() != NULL)
	{
		m_plabDoLogin = new SQLabel("",m_pTopWidget,true);
		m_plabDoLogin->setObjectName(QString::fromUtf8("labelDoLogin"));
		m_plabDoLogin->setAlignment(Qt::AlignRight|Qt::AlignTop);

		m_horizontalLayout->addWidget(m_plabDoLogin);
	}

// 	m_plabTime = new QLabel(m_pTopWidget);
// 	m_plabTime->setObjectName(QString::fromUtf8("labelTime"));
// 	m_plabTime->setStyleSheet("font: 87 18pt \"Arial Black\";color:rgb(0, 0, 127);");
// 	m_plabTime->setAlignment(Qt::AlignRight|Qt::AlignTop);
// 	m_horizontalLayout->addWidget(m_plabTime);
// 
// 	m_plabTime2 = new QLabel(m_pTopWidget);
// 	m_plabTime2->setObjectName(QString::fromUtf8("labelTime"));
// 	m_plabTime2->setStyleSheet("font: 50 9pt \"Arial\";color:rgb(0, 0, 127);");
// 	m_plabTime2->setWordWrap(true);
// 	m_plabTime2->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
// 	m_horizontalLayout->addWidget(m_plabTime2);

	m_verticalLayout_2->addWidget(m_pTopWidget);
	m_verticalSpacer = new QSpacerItem(20, 189, QSizePolicy::Minimum, QSizePolicy::Expanding);
	m_verticalLayout_2->addItem(m_verticalSpacer);

	m_ptimerUI = new QTimer(this);
	connect(m_ptimerUI,SIGNAL(timeout()),this,SLOT(OnTimerUI()),Qt::DirectConnection);
	m_ptimerUI->start(500);
	if(GET_DB_CFG != NULL && DB != NULL && DB->GetDatabasePool() != NULL)
		connect(m_plabDoLogin,SIGNAL(clicked()),this,SLOT(OnLogin()));
}

CUiFrameTop::~CUiFrameTop()
{

}

void CUiFrameTop::OnLogin()
{
// 	SString sql = SQt::GetInput("input","Please input SQL to Execute(Empty for cancel):","",this);
// 	if(DB->Execute(sql))
// 		SQt::ShowInfoBox("tip","SQL Execute OK!");
// 	else
// 		SQt::ShowErrorBox("tip","SQL Execute Failed!");
	if(ssp_gui::GetPtr()->IsLogin())
		ssp_gui::GetPtr()->Logout();
	else
		ssp_gui::GetPtr()->Login();
	//重新加载功能模块
	GET_FP_CFG->LoadByDb(DB,ssp_gui::GetPtr()->GetLoginUser());
	m_pUiFrame->InitModule((const char*)GET_FP_CFG->GetFunPointXmlText());
	m_pUiFrame->repaint();
	OnTimerUI();
}

void CUiFrameTop::OnTimerUI()
{
	QString qstr;
	//qstr = SQt::qsprintf(tr("当前时间:%s"),SDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ").data());
// 	SDateTime now = SDateTime::currentDateTime();
// 	qstr = now.toString("hh:mm:ss").data();
// 	m_plabTime->setText(qstr);
// 	static QString week_str[] = {tr("星期日"),tr("星期一"),tr("星期二"),tr("星期三"),tr("星期四"),tr("星期五"),tr("星期六")};
// 	int week = now.wday();
// 	if(week<0 || week>6)
// 		week=0;
// 	qstr = week_str[week]+" "+now.toString("yyyy-MM-dd ").data();
// 	m_plabTime2->setText(qstr);
	if(GET_DB_CFG != NULL && DB != NULL && DB->GetDatabasePool() != NULL)
	{
		qstr = ssp_gui::GetPtr()->GetLoginInfoStr().data();
		if(!ssp_gui::GetPtr()->IsLogin())
		{
			m_plabLoginIfo->setText("");
			if(GET_DB_CFG != NULL)
				m_plabDoLogin->setText(tr("[登录]"));
		}
		else
		{
			m_plabLoginIfo->setText(qstr);
			if(GET_DB_CFG != NULL)
				m_plabDoLogin->setText(tr("[注销]"));
		}
	}

	//CFrameFunModule *pOld = m_pUiFrame->m_pHotFunModule;
	if(m_pUiFrame->m_pHotFunModule != NULL)
	{
		QPoint mousePos = mapFromGlobal(QCursor::pos());
		int y = mousePos.y();
		if(y < 5 || y>height())
			m_pUiFrame->m_pHotFunModule = NULL;
	}
	//if(pOld != m_pUiFrame->m_pHotFunModule)
		repaint(0,0,width(),height());
	if(ssp_gui::GetPtr()->IsLoginTimeout())
		OnLogin();
}

//#define modmenu_left  300//模块菜单的左边距
#define modmenu_top   14//模块菜单的上边距14
#define modmenu_sep   8//模块菜单间的间距 

extern SString m_sDbIp_p,m_sDbInst_p;

void CUiFrameTop::paintEvent(QPaintEvent *event)
{
	S_UNUSED(event);
	QPainter painter(this);
	QRect r = rect();
	int bmpw = m_bmpTop.width();
	static QFont font;//(tr("黑体"),15);
	font.setPointSize(15);
	painter.setFont(font);
	painter.setPen(QColor(30,0,158));
	//painter.setPen(QColor(255,255,255));
	if(r.width() > bmpw)
	{
		int x = bmpw;
		r.setWidth(x);
		if(m_pUiFrame->m_bFullScreen)
			painter.drawPixmap(0,0,x,r.height(),m_bmpTopFull,0,0,m_bmpTopFull.width(),m_bmpTopFull.height() );
		else if(m_pUiFrame->m_pThisFunModule != NULL && m_pUiFrame->m_pThisFunModule->m_iViewCount == 1)
			painter.drawPixmap(0,0,x,r.height(),m_bmpTopNoMenu,0,0,m_bmpTopNoMenu.width(),m_bmpTopNoMenu.height() );
		else
			painter.drawPixmap(0,0,x,r.height(),m_bmpTop,0,0,m_bmpTop.width(),m_bmpTop.height() );
//		painter.drawPixmap(x,0,width()-x,r.height(),m_bmpTopRight,0,0,m_bmpTopRight.width(),m_bmpTopRight.height() );
		painter.drawPixmap(x,0,width()-x,r.height(),m_bmpTopRight,0,0,m_bmpTopRight.width(),m_bmpTopRight.height() );
	}
	else
	{
		if(m_pUiFrame->m_bFullScreen)
			painter.drawPixmap(0,0,bmpw,r.height(),m_bmpTopFull,0,0,m_bmpTopFull.width(),m_bmpTopFull.height() );
		else if(m_pUiFrame->m_pThisFunModule && m_pUiFrame->m_pThisFunModule->m_iViewCount == 1)
			painter.drawPixmap(0,0,bmpw,r.height(),m_bmpTopNoMenu,0,0,m_bmpTopNoMenu.width(),m_bmpTopNoMenu.height() );
		else
			painter.drawPixmap(0,0,bmpw,r.height(),m_bmpTop,0,0,m_bmpTop.width(),m_bmpTop.height() );
	}
	//绘制系统名称
	painter.drawText(65,30,m_pUiFrame->m_sSysName.data());

 	painter.setPen(m_penTextMod);
// 	QRectF rect(100,2,parentWidget()->width()-110,18);
// 	SString sTip = ssp_gui::GetPtr()->GetLoginInfoStr()+SDateTime::currentDateTime().toString(Q2C(tr("当前时间: yyyy-MM-dd hh:mm:ss")));
// 	//painter.drawText(parentWidget()->width()-190,18,sTip.data());
// 	painter.drawText(rect,Qt::AlignRight,sTip.data());
	
	int x = 0/*modmenu_left*/;

	QFontMetrics fm(painter.font());
	m_iModLeft = x = 65 + fm.width(m_pUiFrame->m_sSysName.data()) + 10;

	unsigned long pos;
	painter.setFont(m_ModFont);
	CFrameFunModule *pMod = m_pUiFrame->m_FunMods.FetchFirst(pos);
	while(pMod)
	{
		if(pMod == m_pUiFrame->m_pThisFunModule || m_pUiFrame->m_pHotFunModule == pMod)
			painter.drawPixmap(x,modmenu_top,m_bmpButtonSel.width(),m_bmpButtonSel.height(),m_bmpButtonSel,0,0,m_bmpButtonSel.width(),m_bmpButtonSel.height());
		else
			painter.drawPixmap(x,modmenu_top,m_bmpButton.width(),m_bmpButton.height(),m_bmpButton,0,0,m_bmpButton.width(),m_bmpButton.height());
		painter.drawText(x,modmenu_top,m_bmpButton.width(),m_bmpButton.height(),Qt::AlignCenter,G2U(pMod->m_sTitle.data()));
		x += modmenu_sep + m_bmpButton.width();
		pMod = m_pUiFrame->m_FunMods.FetchNext(pos);
	}
	if(width()-x > 190)
	{
		x = width()-190;
	}
// 	//绘制时间
	SDateTime now = SDateTime::currentDateTime();
	painter.setPen(QColor(0,0,127));
	static QFont font2(tr("Arial Black"),18,QFont::Black);
	painter.setFont(font2);
	QRect rect0(x,17,120,25);
	painter.drawText(rect0,Qt::AlignRight|Qt::AlignVCenter,now.toString("hh:mm:ss").data());
	static QFont font3(tr("Arial"),9);
	painter.setFont(font3);
	static QString week_str[] = {tr("星期日"),tr("星期一"),tr("星期二"),tr("星期三"),tr("星期四"),tr("星期五"),tr("星期六")};
	int week = now.wday();
	if(week<0 || week>6)
		week=0;
	QString sTime = week_str[week]+"\n"+now.toString("yyyy-MM-dd").data();
	QRect rect(x+120,16,70,28);
	painter.drawText(rect,Qt::AlignCenter,sTime);
}


void  CUiFrameTop::mouseMoveEvent ( QMouseEvent * event )
{
	int thisx = event->pos().x();
	int thisy = event->pos().y();
	int x = m_iModLeft;//modmenu_left;
	unsigned long pos;
	CFrameFunModule *hot = NULL;
	CFrameFunModule *pMod = m_pUiFrame->m_FunMods.FetchFirst(pos);
	while(pMod)
	{
		if(thisx >= x && thisx <= x+m_bmpButton.width() && thisy >= modmenu_top && thisy <= modmenu_top+m_bmpButton.height())
		{
			hot = pMod;
			break;
		}
		x += modmenu_sep + m_bmpButton.width();
		pMod = m_pUiFrame->m_FunMods.FetchNext(pos);
	}
	if(hot != m_pUiFrame->m_pHotFunModule)
	{
		m_pUiFrame->m_pHotFunModule = hot;
		repaint();
	}
}

void  CUiFrameTop::mousePressEvent ( QMouseEvent * event )
{
	int thisx = event->pos().x();
	int thisy = event->pos().y();
	int x = m_iModLeft;//modmenu_left;
	unsigned long pos;
	CFrameFunModule *pMod = m_pUiFrame->m_FunMods.FetchFirst(pos);
	while(pMod)
	{
		if(thisx >= x && thisx <= x+m_bmpButton.width() && thisy >= modmenu_top && thisy <= modmenu_top+m_bmpButton.height())
		{
			if(m_pUiFrame->m_pThisFunModule != pMod)
			{
				//切换到新的模块
				m_pUiFrame->OnFunModule(pMod);
			}
			break;
		}
		x += modmenu_sep + m_bmpButton.width();
		pMod = m_pUiFrame->m_FunMods.FetchNext(pos);
	}
}


void  CUiFrameTop::mouseReleaseEvent ( QMouseEvent * event )
{
	S_UNUSED(event);
}


//////////////////////////////////////////////////////////////////////////
// 名    称:  CFrameMenuL2
// 作    者:  邵凯田
// 创建时间:  2015-07-08 14:58
// 描    述:  二级菜单
//////////////////////////////////////////////////////////////////////////
CFrameMenuL2::CFrameMenuL2()
{
	m_pFunView = NULL;
	img_normal_buff = NULL;
	img_normal_len = 0;
};
CFrameMenuL2::~CFrameMenuL2()
{
	if(m_pFunView != NULL)
	{
		delete m_pFunView;
		m_pFunView = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// 名    称:  CFrameMenuL1
// 作    者:  邵凯田
// 创建时间:  2015-07-08 14:59
// 描    述:  一级菜单
//////////////////////////////////////////////////////////////////////////
CFrameMenuL1::CFrameMenuL1()
{
	m_SubMenus.setAutoDelete(true);
	m_bOpened = false;
	img_normal_buff = NULL;
	img_normal_len = 0;
}
CFrameMenuL1::~CFrameMenuL1()
{

}


//////////////////////////////////////////////////////////////////////////
// 名    称:  CFrameFunModule
// 作    者:  邵凯田
// 创建时间:  2015-07-08 15:13
// 描    述:  功能模块定义，一个功能模块中对应若干个一级菜单
//////////////////////////////////////////////////////////////////////////
CFrameFunModule::CFrameFunModule()
{
	m_Menus.setAutoDelete(true);
	m_pLastMenuL2 = NULL;

	img_normal_buff = NULL;
	img_normal_len = 0;
	img_hover_buff = NULL ;
	img_hover_len = 0;
	img_down_buff = NULL;
	img_down_len = 0;
}
CFrameFunModule::~CFrameFunModule()
{

}
bool CFrameFunModule::IsExistL1(CFrameMenuL1 *pL1)
{
	return m_Menus.exist(pL1);
}

bool CFrameFunModule::IsExistL2(CFrameMenuL2 *pL2)
{
	CFrameMenuL1 *pL1 = pL2->m_pL1;
	if(!IsExistL1(pL1))
		return false;
	return pL1->m_SubMenus.exist(pL2);
}

void CFrameFunModule::CheckLastMenuL2Valid()
{
	if(m_pLastMenuL2 == NULL || !IsExistL2(m_pLastMenuL2))
	{
		CFrameMenuL1 *pL1 = m_Menus[0];
		if(pL1 != NULL)
			m_pLastMenuL2 = pL1->m_SubMenus[0];
	}
}

//////////////////////////////////////////////////////////////////////////
// 名    称:  CUiFrameLeft
// 作    者:  邵凯田
// 创建时间:  2015-07-08 14:49
// 描    述:  左边菜单窗口
//////////////////////////////////////////////////////////////////////////
CUiFrameLeft::CUiFrameLeft(QWidget *parent) : QWidget(parent)
{
	//m_bmpMenuTop,m_bmpMenuL1Open,m_bmpMenuL1Close,m_bmpMenuBody,m_bmpMenuL2,m_bmpMenuBottom
	if(!m_bmpMenuTop.load(":/ssp_gui/Resources/menu_top.bmp"))
	{
		LOGWARN("load Resources/menu_top.bmp error!");
	}
	if(!m_bmpMenuL1Open.load(":/ssp_gui/Resources/menu_l1_open.bmp"))
	{
		LOGWARN("load Resources/menu_l1_open.bmp error!");
	}
	if(!m_bmpMenuL1Close.load(":/ssp_gui/Resources/menu_l1_close.bmp"))
	{
		LOGWARN("load Resources/menu_l1_close.bmp error!");
	}
	if(!m_bmpMenuBody.load(":/ssp_gui/Resources/menu_body.bmp"))
	{
		LOGWARN("load Resources/menu_body.bmp error!");
	}
	if(!m_bmpMenuL2.load(":/ssp_gui/Resources/menu_l2.bmp"))
	{
		LOGWARN("load Resources/menu_l2.bmp error!");
	}
	if(!m_bmpMenuL2Act.load(":/ssp_gui/Resources/menu_l2_act.bmp"))
	{
		LOGWARN("load Resources/menu_l2_act.bmp error!");
	}
	if(!m_bmpMenuBottom.load(":/ssp_gui/Resources/menu_bottom.bmp"))
	{
		LOGWARN("load Resources/menu_bottom.bmp error!");
	}
	m_pUiFrame = NULL;
	m_iOffsetY = 0;
	m_iNeedHeight = 0;

	m_penTextL1 = QPen(QColor(255,255,255));
	m_penTextL1Sel = QPen(QColor(255,255,255),2);
	m_penTextL2 = QPen(QColor(0,0,0));
	m_penTextL2Sel = QPen(QColor(255,255,255),2);
}

CUiFrameLeft::~CUiFrameLeft()
{

}
void CUiFrameLeft::paintEvent(QPaintEvent *event)
{
	S_UNUSED(event);
	QPainter painter(this);
	QRect r = rect();
	//int y = 0;
	int w = m_bmpMenuTop.width();
	int height = r.height();
	//先画一个空的框
	painter.drawPixmap(0,0,w,height,m_bmpMenuBody,0,0,m_bmpMenuBody.width(),m_bmpMenuBody.height() );

	//画菜单项
	if(m_pUiFrame->m_pThisFunModule != NULL)
	{
		paintMenu(m_pUiFrame->m_pThisFunModule, painter);
	}

	//画首尾图角框
// 	painter.drawPixmap(0,0,w,m_bmpMenuTop.height(),m_bmpMenuTop,0,0,m_bmpMenuTop.width(),m_bmpMenuTop.height() );
// 	painter.drawPixmap(0,height-m_bmpMenuBottom.height(),w,m_bmpMenuBottom.height(),m_bmpMenuBottom,0,0,m_bmpMenuBottom.width(),m_bmpMenuBottom.height() );
}


void  CUiFrameLeft::mouseMoveEvent ( QMouseEvent * event )
{
	S_UNUSED(event);
// 	int x = event->pos().x() + this->x();
// 	if(m_iCursorType != 0 && x>m_pWaveInfo->m_iLeftWidth)
// 	{
// 		m_pWaveInfo->MoveCursorPos(m_iCursorType-1,x);
// 	}

}

#define textleft  50//菜单文字左边距
#define textheigt 30//菜单文字高度
#define texttop   8//菜单文字上边距
void  CUiFrameLeft::mousePressEvent ( QMouseEvent * event )
{
//	int h = height();
	int thisy = event->pos().y();
// 	if(thisy <= m_bmpMenuTop.height())
// 		return;
// 	if(thisy >= h-m_bmpMenuBottom.height())
// 		return;
	if(m_pUiFrame->m_bOpenningView)
		return;//前一个界面尚未打开完毕

	CFrameMenuL1 *pL1;
	CFrameMenuL2 *pL2;
	unsigned long p1,p2;
	CFrameFunModule *pModule = m_pUiFrame->m_pThisFunModule;
	if (pModule == NULL)
		return QWidget::mousePressEvent(event);

	pL1 = pModule->m_Menus.FetchFirst(p1);
	int y = this->m_iOffsetY/*+m_bmpMenuTop.height()*/;
//	int w = m_bmpMenuTop.width();
	while(pL1)
	{
		//绘制当前一级菜单
		if(pL1->m_bOpened)
		{
			if(thisy > y && thisy < y + m_bmpMenuL1Open.height())
			{
				//点击了一级菜单
				pL1->m_bOpened = !pL1->m_bOpened;
				repaint();
				return;
			}
			y += m_bmpMenuL1Open.height();

			//当前一级菜单下的二级菜单
			pL2 = pL1->m_SubMenus.FetchFirst(p2);
			while(pL2)
			{
				if(thisy > y && thisy < y + m_bmpMenuL2.height())
				{
					//点击了二级菜单
					m_pUiFrame->OnFunView(pL2);
					repaint();
					return;
				}
				y += m_bmpMenuL2.height();

				pL2 = pL1->m_SubMenus.FetchNext(p2);
			}
		}
		else
		{
			if(thisy > y && thisy < y + m_bmpMenuL1Close.height())
			{
				//点击了一级菜单
				pL1->m_bOpened = !pL1->m_bOpened;
				repaint();
				return;
			}
			y += m_bmpMenuL1Close.height();
		}


		pL1 = pModule->m_Menus.FetchNext(p1);
	}

}


void  CUiFrameLeft::mouseReleaseEvent ( QMouseEvent * event )
{
	S_UNUSED(event);
//	m_iCursorType = 0;
}

void  CUiFrameLeft::wheelEvent ( QWheelEvent * event )
{
	int h = height();
	int y = -m_iOffsetY;
	if(h < m_iNeedHeight)
	{
		int d = event->delta()>0?20:-20;
		y -= d;
		if(y < 0)
			y = 0;
		else if(y > m_iNeedHeight-h)
			y = m_iNeedHeight-h;
	}
	m_iOffsetY = -y;
	repaint();
}

////////////////////////////////////////////////////////////////////////
// 描    述:  绘制菜单
// 作    者:  邵凯田
// 创建时间:  2013:9:1 13:15
// 参数说明:  @pModule为当前模块指针
//         :  @painter为画板
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CUiFrameLeft::paintMenu(CFrameFunModule *pModule, QPainter &painter)
{
	CFrameMenuL1 *pL1;
	CFrameMenuL2 *pL2;
	unsigned long p1,p2;
	pL1 = pModule->m_Menus.FetchFirst(p1);
	int y = this->m_iOffsetY/*+m_bmpMenuTop.height()*/;
	int w = m_bmpMenuTop.width();
	while(pL1)
	{
		//绘制当前一级菜单
		if(pL1->m_bOpened)
		{
			painter.drawPixmap(0,y,w,m_bmpMenuL1Open.height(),m_bmpMenuL1Open,0,0,w,m_bmpMenuL1Open.height());
			painter.setPen(m_penTextL1Sel);
			painter.drawText(textleft,y+texttop,w-textleft,textheigt,0,G2U(pL1->m_sTitle.data()));
			y += m_bmpMenuL1Open.height();

			//绘制当前一级菜单下的二级菜单
			pL2 = pL1->m_SubMenus.FetchFirst(p2);
			while(pL2)
			{
				//绘制二级菜单
				if(pL2 == pModule->m_pLastMenuL2)
				{
					painter.setPen(m_penTextL2Sel);
					painter.drawPixmap(0,y,w,m_bmpMenuL2Act.height(),m_bmpMenuL2Act,0,0,w,m_bmpMenuL2Act.height());
				}
				else
				{
					painter.setPen(m_penTextL2);
					painter.drawPixmap(0,y,w,m_bmpMenuL2.height(),m_bmpMenuL2,0,0,w,m_bmpMenuL2.height());
				}
				painter.drawText(textleft,y+texttop,w-textleft,textheigt,0,G2U(pL2->m_sTitle.data()));
				y += m_bmpMenuL2.height();

				pL2 = pL1->m_SubMenus.FetchNext(p2);
			}
		}
		else
		{
			painter.drawPixmap(0,y,w,m_bmpMenuL1Close.height(),m_bmpMenuL1Close,0,0,w,m_bmpMenuL1Close.height());
			painter.setPen(m_penTextL1);
			painter.drawText(textleft,y+texttop,w-textleft,textheigt,0,G2U(pL1->m_sTitle.data()));
			y += m_bmpMenuL1Close.height();
		}


		pL1 = pModule->m_Menus.FetchNext(p1);
	}
	m_iNeedHeight = y-this->m_iOffsetY;
}


//////////////////////////////////////////////////////////////////////////
// 名    称:  CUiFrameBottom
// 作    者:  邵凯田
// 创建时间:  2015-07-08 15:45
// 描    述:  框架底部
//////////////////////////////////////////////////////////////////////////
CUiFrameBottom::CUiFrameBottom(QWidget *parent) : QFrame(parent)
{
	if(!m_bmpBottom.load(":/ssp_gui/Resources/bottom.bmp"))
	{
		LOGWARN("load Resources/bottom.bmp error!");
	}
	if(!m_bmpConnStation.load(":/ssp_gui/Resources/hujiaozhuanyi.gif"))
	{
		LOGWARN("load Resources/hujiaozhuanyi.gif error!");
	}
	setAutoFillBackground(true);
	setStyleSheet(QString::fromUtf8("background-color: rgb(118,153,208);"));
	setContentsMargins(10, 0, 10, 0);
	horizontalLayout = new QHBoxLayout(this);
	this->setLayout(horizontalLayout);
	horizontalLayout->setContentsMargins(0, 0, 0, 0);
	horizontalLayout->setSpacing(0);
//	m_labNull = new QLabel(this);
	m_labTip = new QLabel(this);
// 	m_labTipRight = new QLabel();
// 	m_frmPicture = new QFrame();
	m_labTip->setText(" ");
	m_labTip->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
// 	m_labTipRight->setText("连接：本地");
// 	m_labTipRight->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
// 	m_labTipRight->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
// 	m_frmPicture->setStyleSheet(QString::fromUtf8("image: url(:/ssp_gui/Resources/hujiaozhuanyi.gif);"));

//	m_labNull->setMaximumWidth(10);
//	m_frmPicture->setMaximumWidth(40);
	horizontalSpacer_2 = new QSpacerItem(408, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
//	horizontalLayout->addWidget(m_labNull);
	horizontalLayout->addWidget(m_labTip);
	horizontalLayout->addItem(horizontalSpacer_2);
// 	horizontalLayout->addWidget(m_labTipRight);
// 	horizontalLayout->addWidget(m_frmPicture);	

}

CUiFrameBottom::~CUiFrameBottom()
{

}

void CUiFrameBottom::SetTip(SString &tip)
{
	m_labTip->setText(tip.data());
	qApp->processEvents();
}

// void CUiFrameBottom::paintEvent(QPaintEvent *event)
// {
// 	QPainter painter(this);
// 	QRect r = rect();
// 	//painter.drawPixmap(0,0,width(),height(),m_bmpBottom,0,0,m_bmpBottom.width(),m_bmpBottom.height() );
// // 	int h = m_bmpConnStation.height();
// // 	int w = m_bmpConnStation.width();
// // 	painter.drawPixmap(width()-r.width(),3,w,h,m_bmpConnStation,0,0,m_bmpConnStation.width(),m_bmpConnStation.height() );
// }


//////////////////////////////////////////////////////////////////////////
// 名    称:  CUiFrameBody
// 作    者:  邵凯田
// 创建时间:  2013:9:2 15:25
// 描    述:  框架底部
//////////////////////////////////////////////////////////////////////////
CUiFrameBody::CUiFrameBody(QWidget *parent) : QWidget(parent)
{
	horizontalLayout = new QHBoxLayout(this);
	horizontalLayout->setSpacing(0);
	horizontalLayout->setContentsMargins(0, 0, 0, 0);
	horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
}

CUiFrameBody::~CUiFrameBody()
{

}

void CUiFrameBody::AddWidget(QWidget *p)
{
	horizontalLayout->addWidget(p);
}

CSspUiFrame* g_SSpUiFrame = NULL;

//////////////////////////////////////////////////////////////////////////
// 名    称:  CUiFrame
// 作    者:  邵凯田
// 创建时间:  2015-07-08 14:02
// 描    述:  框架窗口
//////////////////////////////////////////////////////////////////////////
CSspUiFrame::CSspUiFrame(QWidget *parent) : QWidget(parent)
{
	LOGDEBUG("into CSspUiFrame::CSspUiFrame");
	m_iStartState = 0;
	m_bLoading = true;
	m_pThisFunModule = NULL;
	m_pHotFunModule = NULL;
	m_pLastMenuL2 = NULL;
	m_pWndTop = NULL;
	m_pWndLeft = NULL;
	m_pWndBottom = NULL;
	m_pWndBody = NULL;
	bool bFirst = (g_SSpUiFrame==NULL);
	g_SSpUiFrame = this;
	m_bOpenningView = false;
	m_pluginMgr = new CSspPluginMgr();
	img_normal_buff = NULL;
	img_normal_len = 0;
	LOGDEBUG("create ThreadLoading");
	SKT_CREATE_THREAD(ThreadLoading,this);
	m_Loading.show();

	if(bFirst)
	{
		m_BuddyWidgetServer.setUdpListenPort(5677);
		m_BuddyWidgetServer.Start();
	}
}

CSspUiFrame::~CSspUiFrame()
{
	m_BuddyWidgetServer.Stop();
	m_pluginMgr->Exit();
	delete m_pluginMgr;
	SLog::quitLog();
}

CSspUiFrame* CSspUiFrame::GetPtr()
{
	return g_SSpUiFrame;
}

void CSspUiFrame::SetStartSysName(SString sSysName)
{
	m_Loading.m_sSysName = sSysName;
	m_Loading.postSetTipText();
}

void CSspUiFrame::SetStartTipText(SString sTipText)
{
	m_Loading.m_sTipText = sTipText;
	m_Loading.postSetTipText();
}

bool CSspUiFrame::HideStartWnd()
{
	m_iStartWndAlpha = 100;
	connect(&m_TimerLoadHide,SIGNAL(timeout()),this,SLOT(OnTimerLoadHide()));
	m_TimerLoadHide.start(50);
	return true;
}
void CSspUiFrame::OnTimerLoadHide()
{
	if(m_iStartWndAlpha < 5)
	{
		m_Loading.hide();
		m_TimerLoadHide.stop();
	}
	m_Loading.setWindowOpacity(((float)m_iStartWndAlpha)/100);
	m_iStartWndAlpha -= 8;
}
bool CSspUiFrame::WaitForBackStart()
{
	m_iStartState = 1;
	while(m_bLoading)
	{
		SApi::UsSleep(50000);
		SQt::ProcessUiEvents();
	}
	return m_bLoadSuccessed;
} 
void CSspUiFrame::Start()
{
	this->setWindowTitle("SSP人机工作站软件");
	m_bFullScreen = false;
	//linux环境下去除标题栏
#ifndef WIN32
	setWindowFlags(Qt::Window|Qt::FramelessWindowHint|Qt::WindowTitleHint);
#endif
	m_pWndTop = new CUiFrameTop(this);
	m_pWndBottom = new CUiFrameBottom(this);
	m_pWndLeft = new CUiFrameLeft(this);
	m_pWndBody = new CUiFrameBody(this);

	m_pWndTop->m_pUiFrame = this;
	m_pWndBottom->m_pUiFrame = this;
	m_pWndLeft->m_pUiFrame = this;
	m_FunMods.setAutoDelete(true);
	//setStyleSheet("background-color: rgb(255, 255, 255);");
	ssp_gui::GetPtr()->SetUiFramePtr(this);
	ssp_gui::GetPtr()->SetStatusTipQLabel(m_pWndBottom->m_labTip);
	frame_resize();

	static bool bInitPlugin=false;
	if(!bInitPlugin)
	{
		bInitPlugin = true;
		m_pluginMgr->Init();
	}	
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  初始化线程
// 作    者:  邵凯田
// 创建时间:  2015-11-22 17:05
// 参数说明:  @lp为this指针
// 返 回 值:  NULL
//////////////////////////////////////////////////////////////////////////
void* CSspUiFrame::ThreadLoading(void* lp)
{
	LOGDEBUG("into  ThreadLoading");
	CSspUiFrame *pThis = (CSspUiFrame*)lp;
	while(pThis->m_iStartState == 0)
		SApi::UsSleep(1000);
	pThis->m_bLoadSuccessed = pThis->BackStart();
	pThis->m_iStartState = 2;
	pThis->m_bLoading = false;
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  根据指定的名称查找二级菜单
// 作    者:  邵凯田
// 创建时间:  2015-7-30 16:30
// 参数说明:  @sName为功能点名称
// 返 回 值:  CFrameMenuL2*
//////////////////////////////////////////////////////////////////////////
CFrameMenuL2* CSspUiFrame::SearchMenul2(SString sName)
{
	unsigned long pos,p2,p3;
	CFrameFunModule *pMod = m_FunMods.FetchFirst(pos);
	while(pMod)
	{
		CFrameMenuL1 *pL1 = pMod->m_Menus.FetchFirst(p2);
		while(pL1)
		{
			CFrameMenuL2 *pL2 = pL1->m_SubMenus.FetchFirst(p3);
			while(pL2)
			{

				if(pL2->m_sFunName == sName)
					return pL2;
				pL2 = pL1->m_SubMenus.FetchNext(p3);
			}
			pL1 = pMod->m_Menus.FetchNext(p2);
		}
		pMod = m_FunMods.FetchNext(pos);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  跳转到指定功能点的界面视图
// 作    者:  邵凯田
// 创建时间:  2015-8-5 14:41
// 参数说明:  @sFunName为功能点名称，对应CFrameMenuL2的名称
// 返 回 值:  true表示跳转成功,false表示跳转失败
//////////////////////////////////////////////////////////////////////////
bool CSspUiFrame::GotoView(SString sFunName)
{
	CFrameMenuL2 *pL2 = SearchMenul2(sFunName);
	if(pL2 == NULL)
		return false;
	OnFunModule(pL2->m_pL1->m_pMod);
	qApp->processEvents();
	OnFunView(pL2);
	m_pWndLeft->repaint();
	qApp->processEvents();
	return true;
}

void CSspUiFrame::InitModule(const char* sFunctionPointXml)
{
	SString sLastMenuL2;
	if(m_pLastMenuL2 != NULL)
	{
		sLastMenuL2 = m_pLastMenuL2->m_sFunName;
		if(m_pLastMenuL2->m_pFunView != NULL)
		{
			m_pLastMenuL2->m_pFunView->OnPreHide();
			m_pLastMenuL2->m_pFunView->setVisible(false);
			m_pLastMenuL2->m_pFunView->OnAftHide();
		}
	}
	m_FunMods.clear();
	m_pLastMenuL2 = NULL;
	m_pThisFunModule = NULL;
	m_pHotFunModule = NULL;
	Init(sFunctionPointXml, m_FunMods);
	if(sLastMenuL2.length() > 0)
	{
		//还原最后菜单配置
		CFrameMenuL2 *pLast = SearchMenul2(sLastMenuL2);
		if(pLast != NULL)
		{
			//模块权限仍存在，则保留在当前窗口不变
			m_pThisFunModule = pLast->m_pL1->m_pMod;
			m_pThisFunModule->m_pLastMenuL2 = pLast;
			OnFunModule(m_pThisFunModule);
			return;
		}
	}

	m_pThisFunModule = m_FunMods[0];
	if(m_pThisFunModule != NULL)
		OnFunModule(m_pThisFunModule);
}

void CSspUiFrame::frame_resize()
{
	QRect r = rect();
	int top_h = m_pWndTop->m_bmpTop.height();
	if(m_bFullScreen)
		top_h = 36;
	bool bFull = false;
	if(m_FunMods.count() == 0 || (m_FunMods.count() == 1 && m_FunMods[0]->m_Menus.count() <= 1))
		bFull = true;
	m_pWndTop->setFixedWidth(r.width());
	m_pWndTop->setFixedHeight(bFull?0:top_h);
	m_pWndTop->move(0,0);

	int left_width = m_pWndLeft->m_bmpMenuTop.width();
	if((m_pThisFunModule != NULL && m_pThisFunModule->m_iViewCount <= 1) || m_bFullScreen)
		left_width = 0;

	m_pWndBottom->setFixedWidth(r.width());
	m_pWndBottom->setFixedHeight(bFull?0:m_pWndBottom->m_bmpBottom.height());
	m_pWndBottom->move(0,r.height()-m_pWndBottom->m_bmpBottom.height());

	m_pWndLeft->setFixedWidth(bFull?0:left_width);
	m_pWndLeft->setFixedHeight(r.height()-m_pWndTop->height()-m_pWndBottom->height());
	m_pWndLeft->move(0,m_pWndTop->height());

	m_pWndBody->setFixedWidth(width()-m_pWndLeft->width());
	m_pWndBody->setFixedHeight(r.height()-m_pWndTop->height()-m_pWndBottom->height());
	m_pWndBody->move(m_pWndLeft->width(),m_pWndTop->height());


}
void CSspUiFrame::resizeEvent( QResizeEvent *e )
{
	QWidget::resizeEvent(e);

	frame_resize();
}

////////////////////////////////////////////////////////////////////////
// 描    述:  执行模块的点击操作
// 作    者:  邵凯田
// 创建时间:  2015-07-08 9:23
// 参数说明:  @pMod为模块指针
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSspUiFrame::OnFunModule(CFrameFunModule *pMod)
{
	static int cnt=0;
	m_pWndLeft->m_iOffsetY = 0;
	m_pThisFunModule = pMod;
	OnFunView(m_pThisFunModule->m_pLastMenuL2);
	int c=1;
	if(pMod->m_iViewCount == 0)
		c = 0;
	else if(pMod->m_iViewCount > 1)
		c = 2;
	if(cnt != c)
	{
		cnt = c;
		frame_resize();
	}
	paintall();
}


////////////////////////////////////////////////////////////////////////
// 描    述:  执行二级菜单的点击操作
// 作    者:  邵凯田
// 创建时间:  2013:9:2 15:00
// 参数说明:  pMenuL2为二级菜指针
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSspUiFrame::OnFunView(CFrameMenuL2 *pMenuL2)
{
	if(m_bOpenningView)
		return;
	m_bOpenningView = true;
	m_pWndBody->setEnabled(false);
	if(pMenuL2 != NULL && pMenuL2->m_pL1 != NULL && pMenuL2->m_pL1->m_bOpened == false)
		pMenuL2->m_pL1->m_bOpened = true;
	CFrameMenuL1 *pL1;
	//CFrameMenuL2 *pL2;
	unsigned long p1;//,p2;
	//CBaseView *pHide = NULL;
	CBaseView *pShow = NULL;
	CFrameFunModule *pModule = m_pThisFunModule;

	if((pModule->m_pLastMenuL2 != NULL && pModule->m_pLastMenuL2 == pMenuL2 && 
		pModule->m_pLastMenuL2->m_pFunView != NULL && 
		pModule->m_pLastMenuL2->m_pFunView->isVisible()) ||
		pMenuL2 == NULL)
	{
		//忽略相同的视图
		m_pWndBody->setEnabled(true);
		if(m_pLastMenuL2 != NULL && m_pLastMenuL2->m_pFunView != NULL && m_pLastMenuL2 != pMenuL2)
		{
			m_pLastMenuL2->m_pFunView->OnPreShow();
			m_pLastMenuL2->m_pFunView->setVisible(false);
			m_pLastMenuL2->m_pFunView->OnAftShow();
			m_pLastMenuL2 = NULL;
		}
		m_bOpenningView = false;
		return;
	}

	pL1 = pModule->m_Menus.FetchFirst(p1);
	while(pL1)
	{
		if(pL1->m_SubMenus.exist(pMenuL2))
			break;
		pL1 = pModule->m_Menus.FetchNext(p1);
	}
	if(pL1 == NULL)
	{
		//无效的二级菜单
		m_pWndBody->setEnabled(true);
		m_bOpenningView = false;
		return;
	}
	m_pWndBody->setEnabled(true);
	if(m_pLastMenuL2 != NULL && m_pLastMenuL2->m_pFunView != NULL)
	{
		m_pLastMenuL2->m_pFunView->OnPreHide();
		//m_pLastMenuL2->m_pFunView->setVisible(false);
		//m_pLastMenuL2->m_pFunView->OnAftHide();
	}

	if(pMenuL2->m_pFunView != NULL)
	{
		pShow = pMenuL2->m_pFunView;
		pMenuL2->m_pFunView->OnPreShow();
		pMenuL2->m_pFunView->setVisible(true);
		//pMenuL2->m_pFunView->OnAftShow();
	}
	else
	{
		pMenuL2->m_pFunView = NewView(pMenuL2,NULL);
		if(pMenuL2->m_pFunView != NULL)
		{
			pShow = pMenuL2->m_pFunView;
			pMenuL2->m_pFunView->OnPreShow();
			m_pWndBody->AddWidget(pMenuL2->m_pFunView);
			//pMenuL2->m_pFunView->OnAftShow();
		}
	}
	if(m_pLastMenuL2 != NULL && m_pLastMenuL2->m_pFunView != NULL)
	{
		//m_pLastMenuL2->m_pFunView->OnPreHide();
		m_pLastMenuL2->m_pFunView->setVisible(false);
		m_pLastMenuL2->m_pFunView->OnAftHide();
	}
	m_pLastMenuL2 = pModule->m_pLastMenuL2 = pMenuL2;
	m_pWndBody->setEnabled(true);

	if(pShow != NULL)
		pShow->OnAftShow();

	SString str;
	if(pL1->m_SubMenus.count() == 1)
	{
		if(pModule->m_sTitle == pMenuL2->m_sTitle)
			str.sprintf("%s",pModule->m_sTitle.data());
		else
			str.sprintf("%s > %s",pModule->m_sTitle.data(), pMenuL2->m_sTitle.data());
	}
	else
		str.sprintf("%s > %s > %s",pModule->m_sTitle.data(),pL1->m_sTitle.data(), pMenuL2->m_sTitle.data());
	m_pWndTop->m_sCurrModDesc = str;
	m_pWndTop->repaint();
	//SetTip(str);
	m_bOpenningView = false;
}

void CSspUiFrame::SetTip(SString &tip)
{
	m_pWndBottom->SetTip(tip);
}

////////////////////////////////////////////////////////////////////////
// 描    述:  刷新全部视图
// 作    者:  邵凯田
// 创建时间:  2013:10:9 14:12
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
void CSspUiFrame::RefreshAllView()
{
	unsigned long p1,p2,p3;
	CFrameFunModule *pMod = this->m_FunMods.FetchFirst(p1);
	while(pMod)
	{
		CFrameMenuL1 *pL1 = pMod->m_Menus.FetchFirst(p2);
		while(pL1)
		{
			CFrameMenuL2 *pL2 = pL1->m_SubMenus.FetchFirst(p3);
			while(pL2)
			{
				if(pL2->m_pFunView != NULL)
				{
					pL2->m_pFunView->OnRefresh();
				}
				pL2 = pL1->m_SubMenus.FetchNext(p3);
			}
			pL1 = pMod->m_Menus.FetchNext(p2);
		}
		pMod = m_FunMods.FetchNext(p1);
	}
}


////////////////////////////////////////////////////////////////////////
// 描    述:  初始化模块及一二级菜单
// 作    者:  邵凯田
// 创建时间:  2015-07-08 13:12
// 参数说明:  mods存储模块的引用变量
// 返 回 值:  true表示成功，false表示失败
//////////////////////////////////////////////////////////////////////////
SString g_sSystemName;
bool CSspUiFrame::Init(const char* sFunctionPointXml, SPtrList<CFrameFunModule> &mods)
{
	CFrameFunModule *pMod;
	CFrameMenuL1 *pL1;
	CFrameMenuL2 *pL2;
	SBaseConfig *pCfgMod,*pCfgL1,*pCfgL2;

	SXmlConfig xml;
	if(!xml.ReadText((char*)sFunctionPointXml))
	{
		LOGERROR("Error in function point xml text read! XML:%s",sFunctionPointXml);
		return false;
	}

	int i,j,k,cnt1,cnt2,cnt3;
	SString str;
	SBaseConfig *pRoot = &xml;
	if(pRoot->GetNodeName() != "fun_point")
	{
		LOGERROR("no fun_point found in function point");
		return false;
	}
	g_sSystemName = m_sSysName = str = pRoot->GetAttribute("name");
	if(str.length() > 0)
		setWindowTitle(str.data());

	cnt1 = pRoot->GetChildCount("fun_point");
	SDatabaseOper *m_dbOper = GET_DB_CFG ? DB : NULL;
	if (m_dbOper)
		m_dbOper->ReadLobToMem("t_ssp_fun_point","img_normal","fun_key='top'",img_normal_buff,img_normal_len);
	for(i=0;i<cnt1;i++)
	{
		pCfgMod = pRoot->GetChildNode(i,"fun_point");
		if(pCfgMod == NULL)
			continue;
		pMod = new CFrameFunModule();
		pMod->m_iViewCount = 0;
		pMod->m_sTitle = pCfgMod->GetAttribute("name");
		pMod->m_sFunName = pCfgMod->GetAttribute("key");
				
		if (m_dbOper)
		{
			SString sWhere = SString::toFormat("fun_key='%s'",pCfgMod->GetAttribute("key").data());
			m_dbOper->ReadLobToMem("t_ssp_fun_point","img_normal",sWhere,pMod->img_normal_buff,pMod->img_normal_len);
			m_dbOper->ReadLobToMem("t_ssp_fun_point","img_hover",sWhere,pMod->img_hover_buff,pMod->img_hover_len);
			m_dbOper->ReadLobToMem("t_ssp_fun_point","img_down",sWhere,pMod->img_down_buff,pMod->img_down_len);
		}
		mods.append(pMod);

		cnt2 = pCfgMod->GetChildCount("fun_point");
		for(j=0;j<cnt2;j++)
		{
			pCfgL1 = pCfgMod->GetChildNode(j,"fun_point");
			if(pCfgL1 == NULL)
				continue;
			pL1 = new CFrameMenuL1();
			pL1->m_pMod = pMod;
			pL1->m_sTitle = pCfgL1->GetAttribute("name");
			pL1->m_sFunName = pCfgL1->GetAttribute("key");

			if (m_dbOper)
			{
				SString sWhere = SString::toFormat("fun_key='%s'",pCfgL1->GetAttribute("key").data());
				m_dbOper->ReadLobToMem("t_ssp_fun_point","img_normal",sWhere,pL1->img_normal_buff,pL1->img_normal_len);
			}

			if(j == 0)
				pL1->m_bOpened = true;
			pMod->m_Menus.append(pL1);

			cnt3 = pCfgL1->GetChildCount("fun_point");
			for(k=0;k<cnt3;k++)
			{
				pCfgL2 = pCfgL1->GetChildNode(k,"fun_point");
				if(pCfgL2 == NULL)
					continue;
				pL2 = new CFrameMenuL2();
				pL2->m_pL1 = pL1;
				pL2->m_sTitle	= pCfgL2->GetAttribute("name");
				pL2->m_sFunName = pCfgL2->GetAttribute("key");
				pL2->m_sDesc	= pCfgL2->GetAttribute("desc");
				pL2->m_iType	= pCfgL2->GetAttributeI("type");
				pL2->m_iRefSn	= pCfgL2->GetAttributeI("ref_sn");
				pL2->m_sPath	= pCfgL2->GetAttribute("path");
				
				if (m_dbOper)
				{
					SString sWhere = SString::toFormat("fun_key='%s'",pCfgL2->GetAttribute("key").data());
					m_dbOper->ReadLobToMem("t_ssp_fun_point","img_normal",sWhere,pL2->img_normal_buff,pL2->img_normal_len);
				}
				if(pMod->m_pLastMenuL2 == NULL)
					pMod->m_pLastMenuL2 = pL2;
				pMod->m_iViewCount++;
				pL1->m_SubMenus.append(pL2);
			}
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  根据功能名称创建新视图
// 作    者:  邵凯田
// 创建时间:  2015-07-08 14:56
// 参数说明:  @pFunMenu表示功能菜单指针
//         :  @parent为父窗口指针
// 返 回 值:  CBaseView*
//////////////////////////////////////////////////////////////////////////
CBaseView* CSspUiFrame::NewView(CFrameMenuL2 *pFunMenu,QWidget *parent)
{
	CBaseView *pView = NULL;
	if(pFunMenu->m_sFunName == "sys.fun.point")
		return new view_nocommon(parent,pFunMenu->m_sFunName);
	else if(pFunMenu->m_sFunName == "sys.grt.grp")
		return new view_nocommon(parent,pFunMenu->m_sFunName);
	else if(pFunMenu->m_sFunName == "sys.grt.usr")
		return new view_nocommon(parent,pFunMenu->m_sFunName);
	else if(pFunMenu->m_sFunName == "sys.fun.wnd")
		return new view_uicfg(parent);
	else if(pFunMenu->m_sFunName == "sys.dw.edit")
		return new view_datawindow_edit(parent);
	else if(pFunMenu->m_sFunName == "sys.svglib.edit")
		return new view_svglib_edit(parent);
	else if(pFunMenu->m_sFunName == "sys.gset.tmpl")
		return new view_setup_setting(parent);
	else if(pFunMenu->m_sFunName == "sys.gset.edit")
		return new view_setup(parent);
	else if(pFunMenu->m_sFunName == "sys.rpt.set")
		return new view_reportsetting(parent);
	else if(pFunMenu->m_sFunName == "sys.about")
		return new CViewAbout(parent);
	else if(pFunMenu->m_sFunName == "sys.help")
		return new CViewHelp(parent);
	else if(pFunMenu->m_sFunName == "sys.modinfo")
		return new CViewModuleInfo(parent);
	else if(pFunMenu->m_sFunName == "sys.quit")
		return new CViewQuit(parent);
	else if(pFunMenu->m_sFunName == "sys.viewplugin")
		return new ViewPlugin(parent);
	else if((pView = m_pluginMgr->NewView(pFunMenu->m_sFunName,parent)) != NULL)
		return pView;
	else if(pFunMenu->m_iType == 2)//表示组态界面功能点
	{
		//从t_ssp_uicfg_wnd加载指定的组态窗口
		SString sql;
		sql.sprintf("select wnd_name,wnd_type,dataset,ds_cond from t_ssp_uicfg_wnd where wnd_sn=%d",pFunMenu->m_iRefSn);
		SRecordset rs;
		int ret = DB->Retrieve(sql,rs);
		if(ret <= 0)
		{
			SQt::ShowErrorBox(tr("提示"),tr("功能点对应的组态窗口不存在!\n想要解决此问题请与系统管理员联系!"));
			return NULL;
		}
		int wnd_sn = pFunMenu->m_iRefSn;
		SString wnd_name = rs.GetValue(0,0);
		int wnd_type = rs.GetValue(0,1).toInt();
		SString ds_name = rs.GetValue(0,2);
		SString cond = rs.GetValue(0,3);
		if(wnd_type == 1)//1-	映射到内存数据表的SVG图形监视窗口
		{
			CViewMdbSvg *pSvg = new CViewMdbSvg(wnd_sn,parent);
			pSvg->SetDatasetName(ds_name);
			pSvg->SetCondition(cond);
			return pSvg;
		}
		else if(wnd_type == 2)//2-	映射到内存数据表的动态表格
		{
			SString dsName = ds_name;//数据集名称
			if(dsName.length() == 0)
				SQt::ShowErrorBox(tr("提示"),tr("未在关联路径中配置数据窗口对应的数据集名称!\n想要解决此问题请与系统管理员联系!"));
			else if(GET_DW_CFG->SearchDataset(dsName) == NULL)
				SQt::ShowErrorBox(tr("提示"),SQt::qsprintf(tr("关联数据集名称[%s]未在数据窗口中定义!\n想要解决此问题请与系统管理员联系!"),dsName.data()));
			else
				return new view_query(parent,dsName,true);
			return NULL;
		}
		else if(wnd_type == 3)//3-	映射到历史数据表的数据窗口，对应数据窗口配置中的某一数据集，可以配置若干过滤条件，可以分别指定查询/修改/添加/删除功能
		{
			SString dsName = ds_name;//数据集名称
			if(dsName.length() == 0)
				SQt::ShowErrorBox(tr("提示"),tr("未在关联路径中配置数据窗口对应的数据集名称!\n想要解决此问题请与系统管理员联系!"));
			else if(GET_DW_CFG->SearchDataset(dsName) == NULL)
				SQt::ShowErrorBox(tr("提示"),SQt::qsprintf(tr("关联数据集名称[%s]未在数据窗口中定义!\n想要解决此问题请与系统管理员联系!"),dsName.data()));
			else
				return new view_common(parent,dsName);
			return NULL;
		}
		else if(wnd_type == 4)//4-	映射到历史数据表的数据窗口，查询窗口
		{
			SString dsName = ds_name;//数据集名称
			if(dsName.length() == 0)
				SQt::ShowErrorBox(tr("提示"),tr("未在关联路径中配置数据窗口对应的数据集名称!\n想要解决此问题请与系统管理员联系!"));
			else if(GET_DW_CFG->SearchDataset(dsName) == NULL)
				SQt::ShowErrorBox(tr("提示"),SQt::qsprintf(tr("关联数据集名称[%s]未在数据窗口中定义!\n想要解决此问题请与系统管理员联系!"),dsName.data()));
			else
				return new view_query(parent,dsName,false);
			return NULL;
		}
		else if(wnd_type == 5)//5-	映射到报表
		{
			SString dsName = ds_name;
			//int dsNo=DB->SelectIntoI(SString::toFormat("select dw_sn from t_ssp_data_window where name='%s'",dsName.data()));
			//int reportNo=DB->SelectIntoI(SString::toFormat("select report_no from t_ssp_report_info where ds_no=%d",dsNo));
			int reportNo=dsName.toInt();
  			if(reportNo>0)
 				return new view_reportshow(reportNo,parent);
			return NULL;
		}
		else
		{
			SQt::ShowErrorBox(tr("提示"),SQt::qsprintf(tr("关联组态窗口[%s]定义了无效的窗口类型[%d]!\n想要解决此问题请与系统管理员联系!"),
														wnd_name.data(),wnd_type));
			return NULL;
		}
	}
	else if(pFunMenu->m_iType == 3)//表示动态报表功能点
	{
		int reportNo=pFunMenu->m_iRefSn;
		if(reportNo>0)
			return new view_reportshow(reportNo,parent);
		return NULL;
	}
	return NULL;
}

