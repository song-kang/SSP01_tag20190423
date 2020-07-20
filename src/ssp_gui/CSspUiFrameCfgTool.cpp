#include "UKQtRccBuilder.h"
#include "ssp_funpoint.h"
#include "dlg_login_bluestyle.h"
#include "ssp_plugin_mgr.h"

#include "CSspUiFrameCfgTool.h"

CSspUiFrameCfgTool::CSspUiFrameCfgTool(QWidget *parent)
	: CSspUiFrame(parent), plugin_main_view(NULL)
{
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::Widget);

	b_showMax = true;	
	b_mousePressed = false;
	b_init = true;

	toolButton = NULL;
	maxToolButton = NULL;

	resizeBorderWidth = 5;
	setMouseTracking(true);

	tMenu = NULL;
}

CSspUiFrameCfgTool::~CSspUiFrameCfgTool()
{
	if (tMenu)
	{
		delete tMenu;
		tMenu  = NULL;
	}
}

void CSspUiFrameCfgTool::Start() {
	ssp_gui::GetPtr()->SetUiFramePtr(this);	
	this->setObjectName(QString("this"));
	this->setAutoFillBackground(true);
	QPalette palette;
	QDesktopWidget* desktopWidget = QApplication::desktop();
	QRect screenRect = desktopWidget->screenGeometry();
	if (screenRect.width() >= 1600 && screenRect.height() >= 1200)
		palette.setBrush(QPalette::Background, QBrush(QPixmap(":/resource/Resource/main_1600x1200.png")));
	else
		palette.setBrush(QPalette::Background, QBrush(QPixmap(":/resource/Resource/main.png")));
	this->setPalette(palette);

	verticalLayout_2 = new QVBoxLayout(this);
	verticalLayout_2->setSpacing(0);
	verticalLayout_2->setObjectName(QString("verticalLayout_2"));
	verticalLayout_2->setContentsMargins(0, 0, 0, 0);

	topWidget = new QWidget(this);
	topWidget->setObjectName(QString("topWidget"));
	topWidget->setMinimumSize(QSize(0, 50));
	topWidget->setMaximumSize(QSize(16777215, 50));
	topWidget->setStyleSheet(QString("QWidget#topWidget{background:rgba(0,19,107,40)}"));
	gridLayout = new QGridLayout(topWidget);
	gridLayout->setSpacing(0);
	gridLayout->setObjectName(QString("gridLayout"));
	gridLayout->setContentsMargins(0, 0, 0, 0);
	horizontalLayout_2 = new QHBoxLayout();
	horizontalLayout_2->setObjectName(QString("horizontalLayout_2"));
	horizontalLayout_2->setContentsMargins(10, -1, -1, -1);
	logoButton = new QPushButton(topWidget);
	logoButton->setObjectName(QString("logoButton"));

	horizontalLayout_2->addWidget(logoButton);

	horizontalSpacer = new QSpacerItem(458, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	horizontalLayout_2->addItem(horizontalSpacer);

	widget_5 = new QWidget(topWidget);
	widget_5->setObjectName(QString("widget_5"));
	horizontalLayout = new QHBoxLayout(widget_5);
	horizontalLayout->setSpacing(0);
	horizontalLayout->setObjectName(QString("horizontalLayout"));
	horizontalLayout->setContentsMargins(0, 0, 0, 0);
	widget_2 = new QWidget(widget_5);
	widget_2->setObjectName(QString("widget_2"));
	widget_2->setMinimumSize(QSize(120, 50));
	widget_2->setMaximumSize(QSize(120, 50));
	headLabel = new QLabel(widget_2);
	headLabel->setObjectName(QString("headLabel"));
	headLabel->setGeometry(QRect(0, 11, 28, 28));
	headLabel->setMinimumSize(QSize(28, 28));
	headLabel->setStyleSheet(QString("border-image: url(:/resource/Resource/t-mage.png);"));
	headLabel->setVisible(false);

	userNameLabel = new QLabel(widget_2);
	userNameLabel->setObjectName(QString("userNameLabel"));
	userNameLabel->setGeometry(QRect(33, 18, 60, 14));
	userNameLabel->setMinimumSize(QSize(60, 14));
	QFont font;
	userNameLabel->setFont(font);
	userNameLabel->setStyleSheet(QString("QLabel#label_2{font-size:14px;color:#ffecc8;}"));
	userNameLabel->setText(QString(""));
	userNameLabel->setVisible(false);
	userNameLabel->setAlignment(Qt::AlignCenter);

	userToolButton = new QToolButton(widget_2);
	userToolButton->setObjectName(QString("userToolButton"));
	userToolButton->setGeometry(QRect(94, 21, 14, 8));
	userToolButton->setMinimumSize(QSize(14, 8));
	userToolButton->setMaximumSize(QSize(14, 8));
	userToolButton->setCursor(QCursor(Qt::PointingHandCursor));
	userToolButton->setStyleSheet(QString("QToolButton#userToolButton{border-image: url(:/resource/Resource/arrow_d.png);}\n"
		"\n"
		"QToolButton#userToolButton:hover{border-image: url(:/resource/Resource/arrow_d_hov.png);}"));
	userToolButton->setIconSize(QSize(18, 4));
	userToolButton->setAutoRaise(true);
	userToolButton->setVisible(false);

	connect(userToolButton, SIGNAL(clicked()), this, SLOT(slot_userToolButton()));

	loginButton = new QToolButton(widget_2);
	loginButton->setObjectName(QString("loginButton"));
	loginButton->setGeometry(QRect(30, 10, 60, 28));
	loginButton->setMinimumSize(QSize(60, 28));
	loginButton->setMaximumSize(QSize(60, 28));
	loginButton->setStyleSheet(QString("QToolButton#loginButton{border-image: url(:/resource/Resource/login.png);}"
		"QToolButton#loginButton:hover{border-image: url(:/resource/Resource/login_hover.png);}"
		"QToolButton#loginButton:pressed{border-image: url(:/resource/Resource/login_click.png);"
		"}"));
	loginButton->setVisible(false);
	connect(loginButton,SIGNAL(clicked()),this,SLOT(slotLogin()));
	/*
	if(GET_DB_CFG != NULL && DB != NULL && DB->GetDatabasePool() != NULL)
	{
		loginButton->setVisible(true);
		connect(loginButton,SIGNAL(clicked()),this,SLOT(slot_login()));
	}
	*/

	horizontalLayout->addWidget(widget_2);

	/*
	不需要日期时间的展示
	widget_3 = new QWidget(widget_5);
	widget_3->setObjectName(QString("widget_3"));
	widget_3->setMinimumSize(QSize(120, 50));
	widget_3->setMaximumSize(QSize(120, 50));
	timeLabel = new QLabel(widget_3);
	timeLabel->setObjectName(QString("timeLabel"));
	timeLabel->setGeometry(QRect(0, 6, 120, 18));
	QFont font1;
	font1.setBold(true);
	font1.setWeight(75);
	timeLabel->setFont(font1);
	timeLabel->setStyleSheet(QString("QLabel#timeLabel{font-size:18px;font-weight:bold;color:#f8e05d;}"));
	timeLabel->setAlignment(Qt::AlignCenter);
	dateLabel = new QLabel(widget_3);
	dateLabel->setObjectName(QString("dateLabel"));
	dateLabel->setGeometry(QRect(0, 30, 120, 14));
	dateLabel->setFont(font);
	dateLabel->setStyleSheet(QString("QLabel#dateLabel{font-size:14px;color:#fff;}"));
	dateLabel->setAlignment(Qt::AlignCenter);

	horizontalLayout->addWidget(widget_3);
	*/

	widget_4 = new QWidget(widget_5);
	widget_4->setObjectName(QString("widget_4"));
	widget_4->setMinimumSize(QSize(180, 50));
	widget_4->setMaximumSize(QSize(180, 50));
	/*
	toolButton = new QToolButton(widget_4);
	toolButton->setObjectName(QString("toolButton"));
	toolButton->setGeometry(QRect(3, 0, 39, 24));
	toolButton->setMinimumSize(QSize(39, 24));
	toolButton->setCursor(QCursor(Qt::ArrowCursor));
	toolButton->setStyleSheet(QString("QToolButton#toolButton{border-image: url(:/resource/Resource/right_more.png);}\n"
		"\n"
		"\n"
		"QToolButton#toolButton:hover{border-image: url(:/resource/Resource/right_more_hover.png);}\n"
		"\n"
		"\n"
		"\n"
		""));
	toolButton->setIconSize(QSize(39, 24));
	toolButton->setAutoRaise(true);
	*/
	minToolButton = new QToolButton(widget_4);
	minToolButton->setObjectName(QString("minToolButton"));
	minToolButton->setGeometry(QRect(81, 0, 39, 24));
	minToolButton->setMinimumSize(QSize(39, 24));
	minToolButton->setCursor(QCursor(Qt::PointingHandCursor));
	minToolButton->setStyleSheet(QString("QToolButton#minToolButton{border-image: url(:/resource/Resource/right_sma.png);}\n"
		"QToolButton#minToolButton:hover{border-image: url(:/resource/Resource/right_sma_hover.png);}\n"
		"\n"
		""));
	minToolButton->setIconSize(QSize(37, 24));
	minToolButton->setAutoRaise(true);
	connect(minToolButton,SIGNAL(clicked()),this,SLOT(slot_min()));
	/*
	maxToolButton = new QToolButton(widget_4);
	maxToolButton->setObjectName(QString("maxToolButton"));
	maxToolButton->setGeometry(QRect(81, 0, 39, 24));
	maxToolButton->setMinimumSize(QSize(39, 24));
	maxToolButton->setCursor(QCursor(Qt::PointingHandCursor));
	maxToolButton->setStyleSheet(QString("QToolButton#maxToolButton{border-image: url(:/resource/Resource/right_big.png);}\n"
		"QToolButton#maxToolButton:hover{border-image: url(:/resource/Resource/right_big_hover.png);}\n"
		""));
	maxToolButton->setIconSize(QSize(37, 24));
	maxToolButton->setAutoRaise(true);
	connect(maxToolButton,SIGNAL(clicked()),this,SLOT(slot_max()));
	*/
	closeToolButton = new QToolButton(widget_4);
	closeToolButton->setObjectName(QString("closeToolButton"));
	closeToolButton->setGeometry(QRect(120, 0, 47, 24));
	closeToolButton->setCursor(QCursor(Qt::PointingHandCursor));
	closeToolButton->setStyleSheet(QString("QToolButton#closeToolButton{border-image: url(:/resource/Resource/right_close.png);}\n"
		"QToolButton#closeToolButton:hover{border-image: url(:/resource/Resource/right_close_hover.png);}\n"
		"\n"
		"\n"
		""));
	closeToolButton->setIconSize(QSize(47, 24));
	closeToolButton->setAutoRaise(true);
	connect(closeToolButton,SIGNAL(clicked()),this,SLOT(slot_close()));
	horizontalLayout->addWidget(widget_4);


	horizontalLayout_2->addWidget(widget_5);


	gridLayout->addLayout(horizontalLayout_2, 0, 0, 1, 1);


	verticalLayout_2->addWidget(topWidget);
	/*
	slot_time();
	timer = new QTimer(this);  
	connect(timer, SIGNAL(timeout()), this, SLOT(slot_time()));  
	timer->start(1000); 
	*/

	mainWidget = new QWidget(this);
	mainWidget->setObjectName(QString("mainWidget"));
	verticalLayout_3 = new QVBoxLayout(mainWidget);
	verticalLayout_3->setSpacing(0);
	verticalLayout_3->setObjectName(QString("verticalLayout_3"));
	verticalLayout_3->setContentsMargins(0, 0, 0, 0);
	mainWidget->setStyleSheet("QWidget#mainWidget{background-color:#f0f7ff;}");

	verticalLayout_2->addWidget(mainWidget);

	bottomWidget = new QWidget(this);
	bottomWidget->setObjectName(QString("bottomWidget"));
	bottomWidget->setMinimumSize(QSize(0, 30));
	bottomWidget->setMaximumSize(QSize(16777215, 30));
	QFont font2;
	font2.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
	bottomWidget->setFont(font2);
	bottomWidget->setStyleSheet(QString("QWidget#bottomWidget{background:rgba(0,0,0,35)}"));
	horizontalLayout_3 = new QHBoxLayout(bottomWidget);
	horizontalLayout_3->setObjectName(QString("horizontalLayout_3"));
	horizontalLayout_3->setSpacing(0);
	horizontalLayout_3->setContentsMargins(10, 0, 10, 0);
	bottomLabel = new QLabel(bottomWidget);
	bottomLabel->setObjectName(QString("bottomLabel"));
	bottomLabel->setMinimumSize(QSize(100, 14));
	bottomLabel->setMaximumSize(QSize(100, 14));
	bottomLabel->setFont(font);
	bottomLabel->setStyleSheet(QString("QLabel#label_3{font-size:12px; color:#fff}"));
	bottomLabel->setText(" ");

	horizontalLayout_3->addWidget(bottomLabel);

	horizontalSpacer_2 = new QSpacerItem(408, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	horizontalLayout_3->addItem(horizontalSpacer_2);

	verticalLayout_2->addWidget(bottomWidget);

	topWidget->setMouseTracking(true);
	mainWidget->setMouseTracking(true);

	static bool bInitPlugin=false;
	if(!bInitPlugin)
	{
		bInitPlugin = true;
		m_pluginMgr->Init();
	}
}

void CSspUiFrameCfgTool::InitModule( const char* sFunctionPointXml )
{
	m_FunMods.clear();
	Init(sFunctionPointXml, m_FunMods);
	initMainWidget();
}

void CSspUiFrameCfgTool::resizeEvent( QResizeEvent *e )
{
	QWidget::resizeEvent(e);

	frame_resize();
}

void CSspUiFrameCfgTool::initMainWidget()
{
	CUkQtRccBuilder builder1;
	builder1.SetPrefix("/style1");
	builder1.AddImage("top_logo.png",img_normal_buff,img_normal_len);
	if(!builder1.SaveRccToBuffer())
	{
		LOGERROR("Save rcc to buffer error!");
	}
	else
	{
		LOGDEBUG("save rcc to buffer: %p %d",builder1.GetRccBuffer(),builder1.GetRccBufferLen());
		QString root;
		if(!QResource::registerResource(builder1.GetRccBuffer(),root))
		{
			SQt::ShowErrorBox("tip","load rcc error!");
		}
	}
	logoButton->setStyleSheet(QString("QPushButton#logoButton{border-image: url(:/style1/top_logo.png);background-color: transparent;}\n"
		"\n"
		""));
	/*QPixmap pixmap;
	pixmap.load(":/style1/top_logo.png");
	logoButton->setFixedSize(pixmap.width(),38);*/
	QImage qimage;
	qimage.load(":/style1/top_logo.png");
	logoButton->setFixedSize(qimage.width(),38);

	//setMainWidget(0,0,0);
	//只需要设置Top下的一级菜单（模块），用于显示主界面
	CFrameMenuL2 frameMenuL2;
	frameMenuL2.m_sFunName = m_FunMods[0]->m_sFunName;

	plugin_main_view = NewView(&frameMenuL2,NULL);
	plugin_main_view->OnPreShow();
	addMainWidget(plugin_main_view);
}

void CSspUiFrameCfgTool::frame_resize()
{
	if (b_init)
	{
		i_x = this->x();
		i_y = this->y();
		i_w = this->width();
		i_h = this->height();
		b_init = false;

		//move(0,0);
		int aa = QApplication::desktop()->availableGeometry().top();
		int bb= QApplication::desktop()->availableGeometry().left();
		move(bb,aa);
		resize(QApplication::desktop()->availableGeometry().size());		
	}
}

bool CSspUiFrameCfgTool::GotoView(SString sFunName) {
	return false;
}

void CSspUiFrameCfgTool::slot_max()
{
	if (b_showMax)
	{
		b_showMax = false;
		//this->showNormal();
		move(i_x,i_y);
		resize(i_w,i_h);
	}
	else
	{
		b_showMax = true;
		//this->showMaximized();
		//move(0,0);
		int aa = QApplication::desktop()->availableGeometry().top();
		int bb= QApplication::desktop()->availableGeometry().left();
		move(bb,aa);
		resize(QApplication::desktop()->availableGeometry().size());
	}
	this->repaint();
}

void CSspUiFrameCfgTool::slot_min()
{
	this->showMinimized();
}

void CSspUiFrameCfgTool::slot_close()
{
	QMessageBox box(QMessageBox::Question, tr("退出"),tr("你确定要退出吗?"));
	box.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
	box.setButtonText (QMessageBox::Ok,QString("确 定"));
	box.setButtonText (QMessageBox::Cancel,QString("取 消"));	
	if(box.exec() == QMessageBox::Ok)
		this->close();	
}

void CSspUiFrameCfgTool::slot_userToolButton()
{
	if (tMenu)
	{
		tMenu->clear();
	}
	else
	{
		tMenu = new QMenu(this);
	}

	tMenu->setStyleSheet(
		" QMenu {\
		background-color: #1268B3; \
		border: #3c86cc 1px solid;\
		color:#f0f2ff;\
		width:120px;\
		}\
		QMenu::item {\
		background-color: transparent;\
		padding:8px 32px;\
		margin:0px 8px;\
		border-bottom:1px solid #DBDBDB;\
		font-size:14px;\
		width:120px;\
		}\
		QMenu::item:selected { \
		background-color: #2dabf9;\
		}"

		);
	QAction *tAction = new QAction(tMenu);   
	tAction->setText(QString("注销"));
	tAction->setIcon(QIcon(":/resource/Resource/tc.png"));
	tMenu->addAction(tAction);
	connect(tAction,SIGNAL(triggered()),this,SLOT(slotLogin()));
	QAction *tAction1 = new QAction(tMenu);   
	tAction1->setText(QString("退出"));
	tAction1->setIcon(QIcon(":/resource/Resource/qhu.png"));
	tMenu->addAction(tAction1);
	connect(tAction1,SIGNAL(triggered()),this,SLOT(slot_close()));
	QPoint a = userToolButton->pos();
	int w = userToolButton->width();
	int h = userToolButton->height();
	int w1 = widget_2->width();
	QPoint bb = userToolButton->mapToGlobal(QPoint(a.x()-w1+w+w,a.y()-h-h));
	tMenu->exec(bb);
}

void CSspUiFrameCfgTool::slot_userChange()
{
	bool tag = ssp_gui::GetPtr()->Login();
	if (tag == true)
	{
		QString qstr = ssp_gui::GetPtr()->GetLoginUserName().data();
		userNameLabel->setText(qstr);
	}
}

void CSspUiFrameCfgTool::slotLogin()
{
	if(ssp_gui::GetPtr()->IsLogin())
	{
		ssp_gui::GetPtr()->LogoutSubstation();
		userNameLabel->setText(QString(""));		
		userNameLabel->setVisible(false);
		userToolButton->setVisible(false);	

		loginButton->setVisible(true);

		//用于插件中主界面进行权限的更新等操作
		plugin_main_view->OnPreShow();
	}	
	//GET_FP_CFG->LoadByDb(DB,ssp_gui::GetPtr()->GetLoginUser());	

	bool tag = ssp_gui::GetPtr()->LoginSubstation(true);
	if (tag == true)
	{
		QString qstr = ssp_gui::GetPtr()->GetLoginUserName().data();
		userNameLabel->setText(qstr);			
		userNameLabel->setVisible(true);
		userToolButton->setVisible(true);

		loginButton->setVisible(false);

		plugin_main_view->OnPreShow();
	}
	else
	{	
		userNameLabel->setText("");
		userNameLabel->setVisible(false);
		userToolButton->setVisible(false);			
		
		loginButton->setVisible(true);

		plugin_main_view->OnPreShow();

		return;
	}

	//GET_FP_CFG->LoadByDb(DB,ssp_gui::GetPtr()->GetLoginUser());

	//m_FunMods.clear();
	//Init((const char*)GET_FP_CFG->GetFunPointXmlText(),m_FunMods);
	//initLeft();
	this->repaint();
}

void CSspUiFrameCfgTool::mousePressEvent( QMouseEvent *e )
{
	if (e->button() == Qt::LeftButton) {
		this->m_drag = true;
		this->dragPos = e->pos();
		this->resizeDownPos = e->globalPos();
		this->mouseDownRect = this->rect();

		//if (b_showMax)
		//{
		//	b_showMax = false;
		//	showNormal();
		//}
		bool tag = false;
		if (topWidget->pos().x()<= e->pos().x() && e->pos().x()<=(topWidget->pos().x()+topWidget->width()) && topWidget->pos().y()<=e->pos().y() &&e->pos().y()<=(topWidget->pos().y())+topWidget->height())
		{
			tag = true;
		}
		if (b_showMax && tag)
		{
			b_mousePressed = true;
		}


	}
	return CSspUiFrame::mousePressEvent(e);
}

void CSspUiFrameCfgTool::mouseMoveEvent( QMouseEvent *e )
{
	if (b_showMax && b_mousePressed)
	{
		b_showMax = false;
		b_mousePressed = true;
		//showNormal();
		//move(i_x,i_y);
		resize(i_w,i_h);
	}
	if (b_showMax && !b_mousePressed)
	{
		CSspUiFrame::mouseMoveEvent(e);
		return;
	}

	if (resizeRegion != Default)
	{
		handleResize();
		CSspUiFrame::mouseMoveEvent(e);
		return;
	}
	if(m_move) {
		move(e->globalPos() - dragPos);
		CSspUiFrame::mouseMoveEvent(e);
		return;
	}
	QPoint clientCursorPos = e->pos();
	QRect r = this->rect();
	QRect resizeInnerRect(resizeBorderWidth, resizeBorderWidth, r.width() - 2*resizeBorderWidth, r.height() - 2*resizeBorderWidth);
	if(r.contains(clientCursorPos) && !resizeInnerRect.contains(clientCursorPos)) { //调整窗体大小
		ResizeRegion resizeReg = getResizeRegion(clientCursorPos);
		setResizeCursor(resizeReg);
		if (m_drag && (e->buttons() & Qt::LeftButton)) {
			resizeRegion = resizeReg;
			handleResize();
		}
	}
	else { //移动窗体
		setCursor(Qt::ArrowCursor);
		if (m_drag && (e->buttons() & Qt::LeftButton)) {
			m_move = true;
			move(e->globalPos() - dragPos);
		}
	}
	CSspUiFrame::mouseMoveEvent(e);
}

void CSspUiFrameCfgTool::mouseReleaseEvent( QMouseEvent *e )
{
	if (!b_showMax)
	{
		if (!b_mousePressed)
		{
			i_x = this->x();
			i_y = this->y();
		}
		else
		{
			i_x = QApplication::desktop()->availableGeometry().left()+e->globalPos().x()-resizeDownPos.x();
			i_y = QApplication::desktop()->availableGeometry().top()+e->globalPos().y()-resizeDownPos.y();
		}

		i_w = this->width();
		i_h = this->height();
	}	

	b_mousePressed = false;
	m_drag = false;
	if(m_move) {
		m_move = false;
		handleMove(e->globalPos()); //鼠标放开后若超出屏幕区域自动吸附于屏幕顶部/左侧/右侧
	}
	resizeRegion = Default;
	setCursor(Qt::ArrowCursor);
	CSspUiFrame::mouseReleaseEvent(e);
}

void CSspUiFrameCfgTool::mouseDoubleClickEvent( QMouseEvent *e )
{
	if (e->button() == Qt::LeftButton) {
		bool tag = false;
		if (topWidget->pos().x()<= e->pos().x() && e->pos().x()<=(topWidget->pos().x()+topWidget->width()) && topWidget->pos().y()<=e->pos().y() &&e->pos().y()<=(topWidget->pos().y())+topWidget->height())
		{
			tag = true;
		}
		if (tag)
		{
			slot_max();
		}
	}
}

void CSspUiFrameCfgTool::addMainWidget( QWidget *p )
{
	verticalLayout_3->addWidget(p);
	//leftRightButton->raise();
}