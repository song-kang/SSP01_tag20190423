#include "ssp_uiframe_new.h"
#include "UKQtRccBuilder.h"
#include "ssp_funpoint.h"
#include "dlg_login_new.h"
#include "ssp_plugin_mgr.h"

CSspUiFrameNew::CSspUiFrameNew(QWidget *parent)
	: CSspUiFrame(parent)
{
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::Widget);
	b_showMax = true;
	b_showLeft = true;
	b_mousePressed = false;

	resizeBorderWidth = 5;
	setMouseTracking(true);

	tMenu = NULL;
	menu = NULL; 
	m_subMenu = NULL;
}

CSspUiFrameNew::~CSspUiFrameNew()
{

}

void CSspUiFrameNew::newStart()
{
	ssp_gui::GetPtr()->SetUiFramePtr(this);
	m_pLastMenuL2 = NULL;
	this->setObjectName(QString("this"));
	this->setAutoFillBackground(true);
	QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/resource/Resource/bg.jpg")));
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
	//QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	//sizePolicy.setHorizontalStretch(0);
	//sizePolicy.setVerticalStretch(0);
	//sizePolicy.setHeightForWidth(logoButton->sizePolicy().hasHeightForWidth());
	//logoButton->setSizePolicy(sizePolicy);
	logoButton->setMinimumSize(QSize(376, 38));
	logoButton->setMaximumSize(QSize(376, 38));
	logoButton->setStyleSheet(QString("QPushButton#logoButton{border-image: url(:/resource/Resource/logo.png);background-color: transparent;}\n"
		"\n"
		""));
	//logoButton->setStyleSheet(QString("QPushButton#logoButton{border-image: url(:/resource/Resource/logo.png);}\n"
	//	"\n"
	//	""));

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
	if(GET_DB_CFG != NULL && DB != NULL && DB->GetDatabasePool() != NULL)
	{
		loginButton->setVisible(true);
		connect(loginButton,SIGNAL(clicked()),this,SLOT(slot_login()));
	}	

	horizontalLayout->addWidget(widget_2);

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

	widget_4 = new QWidget(widget_5);
	widget_4->setObjectName(QString("widget_4"));
	widget_4->setMinimumSize(QSize(180, 50));
	widget_4->setMaximumSize(QSize(180, 50));
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
	minToolButton = new QToolButton(widget_4);
	minToolButton->setObjectName(QString("minToolButton"));
	minToolButton->setGeometry(QRect(42, 0, 39, 24));
	minToolButton->setMinimumSize(QSize(39, 24));
	minToolButton->setCursor(QCursor(Qt::PointingHandCursor));
	minToolButton->setStyleSheet(QString("QToolButton#minToolButton{border-image: url(:/resource/Resource/right_sma.png);}\n"
		"QToolButton#minToolButton:hover{border-image: url(:/resource/Resource/right_sma_hover.png);}\n"
		"\n"
		""));
	minToolButton->setIconSize(QSize(37, 24));
	minToolButton->setAutoRaise(true);
	connect(minToolButton,SIGNAL(clicked()),this,SLOT(slot_min()));
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
	slot_time();
	timer = new QTimer(this);  
	connect(timer, SIGNAL(timeout()), this, SLOT(slot_time()));  
	timer->start(1000); 

	middleWidget = new QWidget(this);
	middleWidget->setObjectName(QString("middleWidget"));
	gridLayout_4 = new QGridLayout(middleWidget);
	gridLayout_4->setSpacing(0);
	gridLayout_4->setObjectName(QString("gridLayout_4"));
	gridLayout_4->setContentsMargins(0, 0, 0, 0);
	leftWidget = new QWidget(middleWidget);
	leftWidget->setObjectName(QString("leftWidget"));
	leftWidget->setMinimumSize(QSize(120, 0));
	leftWidget->setMaximumSize(QSize(120, 16777215));
	leftWidget->setStyleSheet(QString("QWidget#leftWidget{border-image: url(:/resource/Resource/sid-bg1.png);}\n"
		""));
	gridLayout_2 = new QGridLayout(leftWidget);
	gridLayout_2->setSpacing(0);
	gridLayout_2->setObjectName(QString("gridLayout_2"));
	gridLayout_2->setContentsMargins(0, 0, 0, 0);
	upButton = new QToolButton(leftWidget);
	upButton->setObjectName(QString("upButton"));
	upButton->setEnabled(true);
	upButton->setMinimumSize(QSize(120, 20));
	upButton->setMaximumSize(QSize(120, 20));
	upButton->setCursor(QCursor(Qt::PointingHandCursor));
	upButton->setStyleSheet(QString("QToolButton#upButton{border-image: url(:/resource/Resource/change_btn_nor.png);}"
		"QToolButton#upButton:hover{border-image: url(:/resource/Resource/change_btn_hover.png);}"));
	connect(upButton,SIGNAL(clicked()),this,SLOT(slot_up()));
	gridLayout_2->addWidget(upButton, 0, 0, 1, 1);

	downButton = new QToolButton(leftWidget);
	downButton->setObjectName(QString("downButton"));
	downButton->setMinimumSize(QSize(120, 20));
	downButton->setMaximumSize(QSize(120, 20));
	downButton->setCursor(QCursor(Qt::PointingHandCursor));
	downButton->setStyleSheet(QString("QToolButton#downButton{border-image: url(:/resource/Resource/up_change_btn.png);}\n"
		"\n"
		"QToolButton#downButton:hover{border-image: url(:/resource/Resource/up_change_btn_hover.png);}\n"
		"\n"
		""));
	connect(downButton,SIGNAL(clicked()),this,SLOT(slot_down()));
	gridLayout_2->addWidget(downButton, 2, 0, 1, 1);

	scrollArea = new QScrollArea(leftWidget);
	scrollArea->setObjectName(QString("scrollArea"));
	scrollArea->setFrameShape(QFrame::NoFrame);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setWidgetResizable(true);
	scrollWidget = new QWidget();
	scrollWidget->setObjectName(QString("scrollWidget"));
	scrollWidget->setGeometry(QRect(0, 0, 120, 705));
	scrollWidget->setMinimumSize(QSize(120, 0));
	scrollWidget->setMaximumSize(QSize(120, 16777215));
	scrollWidget->setStyleSheet(QString("QWidget#scrollWidget{background-image: url(:/resource/Resource/sid-bg2.png);}\n"
		""));
	gridLayout_5 = new QGridLayout(scrollWidget);
	gridLayout_5->setSpacing(0);
	gridLayout_5->setObjectName(QString("gridLayout_5"));
	gridLayout_5->setContentsMargins(0, 0, 0, 0);


	scrollArea->setWidget(scrollWidget);

	gridLayout_2->addWidget(scrollArea, 1, 0, 1, 1);
	

	gridLayout_4->addWidget(leftWidget, 0, 0, 1, 1);

	rightWidget = new QWidget(middleWidget);
	rightWidget->setObjectName(QString("rightWidget"));
	QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(rightWidget->sizePolicy().hasHeightForWidth());
	rightWidget->setSizePolicy(sizePolicy1);

	verticalLayout = new QVBoxLayout(rightWidget);
	verticalLayout->setSpacing(0);
	verticalLayout->setObjectName(QString("verticalLayout"));
	verticalLayout->setContentsMargins(0, 0, 0, 0);

	barWidget = new QWidget(rightWidget);
	barWidget->setObjectName(QString("barWidget"));
	barWidget->setMinimumSize(QSize(0, 30));
	barWidget->setMaximumSize(QSize(16777215, 30));
	barWidget->setAutoFillBackground(true);
	barWidget->setStyleSheet(QString("border-image: url(:/resource/Resource/pos_bg.png);"));
	horizontalLayout_4 = new QHBoxLayout(barWidget);
	horizontalLayout_4->setSpacing(5);
	horizontalLayout_4->setObjectName(QString("horizontalLayout_4"));
	horizontalLayout_4->setContentsMargins(0, 0, 0, 0);

	/*toolButton_2 = new QToolButton(barWidget);
	toolButton_2->setObjectName(QString("toolButton_2"));
	toolButton_2->setMinimumSize(QSize(10, 14));
	toolButton_2->setStyleSheet(QString("QToolButton#toolButton_2{border-image: url(:/resource/Resource/posi_icon.png);}\n"
		"QToolButton#toolButton_2:hover{border-image: url(:/resource/Resource/posi_icon_bl.png);}"));
	toolButton_2->setIconSize(QSize(10, 14));

	horizontalLayout_4->addWidget(toolButton_2);*/

	label = new QLabel(barWidget);
	label->setObjectName(QString("label"));
	//label->setText(QString("<font color=#f0f2ff >&nbsp;&nbsp;您当前的位置：首页</font>"));
	label->setStyleSheet("background:transparent");

	horizontalLayout_4->addWidget(label);

	verticalLayout->addWidget(barWidget);


	leftRightButton = new QToolButton(rightWidget);
	leftRightButton->setObjectName(QString("leftRightButton"));
	leftRightButton->setGeometry(QRect(0, 430, 16, 36));
	leftRightButton->setCursor(QCursor(Qt::PointingHandCursor));
	leftRightButton->setStyleSheet(QString("QToolButton#leftRightButton{border-image: url(:/resource/Resource/slid_arrow.png);}\n"
		""));
	leftRightButton->raise();
	connect(leftRightButton,SIGNAL(clicked()),this,SLOT(slot_hideLeftWidget()));

	verticalLayout->addWidget(barWidget);

	mainWidget = new QWidget(rightWidget);
	mainWidget->setObjectName(QString("mainWidget"));
	verticalLayout_3 = new QVBoxLayout(mainWidget);
	verticalLayout_3->setSpacing(0);
	verticalLayout_3->setObjectName(QString("verticalLayout_3"));
	verticalLayout_3->setContentsMargins(0, 0, 0, 0);
	mainWidget->setStyleSheet("QWidget#mainWidget{background-color:#f0f7ff;}");


	verticalLayout->addWidget(mainWidget);

	gridLayout_4->addWidget(rightWidget, 0, 1, 1, 1);


	verticalLayout_2->addWidget(middleWidget);

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


	this->setMouseTracking(true);
	topWidget->setMouseTracking(true);
	middleWidget->setMouseTracking(true);
	bottomWidget->setMouseTracking(true);
	widget_5->setMouseTracking(true);
	widget_4->setMouseTracking(true);
	//leftWidget->setMouseTracking(true);
	logoButton->setMouseTracking(true);
	rightWidget->setMouseTracking(true);
	barWidget->setMouseTracking(true);
	mainWidget->setMouseTracking(true);

	static bool bInitPlugin=false;
	if(!bInitPlugin)
	{
		bInitPlugin = true;
		m_pluginMgr->Init();
	}	
}

void CSspUiFrameNew::resizeEvent( QResizeEvent *e )
{
	QWidget::resizeEvent(e);

	frame_resize();
}

void CSspUiFrameNew::frame_resize()
{
	int a = scrollArea->height();
	if (a >= m_FunMods.count()*100)
	{
		this->upButton->setHidden(true);
		this->downButton->setHidden(true);
	}
	else
	{
		this->upButton->setHidden(false);
		this->downButton->setHidden(false);
	}
	int b = rightWidget->height();
	int c = (b-36)/2;
	leftRightButton->setGeometry(QRect(0, c, 16, 36));
	/*QRect r = rect();
	int w = r.width();
	int h = r.height()-this->topWidget->height()-this->bottomWidget->height()-this->downButton->height()-this->upButton->height();
	this->leftMainwidget->setMaximumHeight(h);
	if (h>= m_FunMods.count()*100)
	{
		this->upButton->setHidden(true);
		this->downButton->setHidden(true);
	}
	else
	{
		this->upButton->setHidden(false);
		this->downButton->setHidden(false);
	}*/
}

void CSspUiFrameNew::slot_time()
{
	QTime qtimeObj = QTime::currentTime();  
	QString strTime = qtimeObj.toString("hh:mm:ss");   
	QDate qdateObj = QDate::currentDate();  
	QString strDate = qdateObj.toString("ddd yyyy/MM/dd"); //星期、月份、天、年   

	timeLabel->setText(strTime);
	dateLabel->setText(strDate);

	if(ssp_gui::GetPtr()->IsLoginTimeout())
		slot_login();
}

void CSspUiFrameNew::initLeft()
{	
	menu = new QMenu(this);
	menu->setStyleSheet(
		" QMenu {\
		background-color: #1268B3; \
		border: #3c86cc 1px solid;\
		margin:0px;\
		width:240px;\
		color:#f0f2ff;\
		}\
		QMenu::item {\
		background-color: transparent;\
		padding:8px 32px;\
		margin:0px 8px;\
		border-bottom:1px solid #DBDBDB;\
		font-size:14px;\
		width:158px;\
		}\
		QMenu::item:selected { \
		background-color: #2dabf9;\
		}"

		);
	/*menu->setStyleSheet(
		" QMenu {\
		background-color: #1268B3; \
		border: #3c86cc 1px solid;\
		margin:0px;\
		width:240px;\
		color:#f0f2ff;\
		}\
		QMenu::item {\
		background-color: transparent;\
		padding:8px 32px;\
		margin:0px 8px;\
		border-bottom:1px solid #DBDBDB;\
		font-size:14px;\
		width:158px;\
		}\
		QMenu::item:selected { \
		background-color: #2dabf9;\
		}"

		);*/
	m_subMenu = new QMenu(this);
	
	SString strName,strNameHover;
	int i=0;
	for (i=0;i<m_FunMods.count();i++)
	{
		QToolButton *toolButton_11 = new QToolButton(scrollWidget);
		toolButton_11->setObjectName(QString("toolButton_11"));
		toolButton_11->setMinimumSize(QSize(118, 100));
		toolButton_11->setMaximumSize(QSize(118, 100));
		toolButton_11->setCursor(QCursor(Qt::PointingHandCursor));
		strName = SString::toFormat("model_%s.png",m_FunMods.at(i)->m_sFunName.data());
		strNameHover = SString::toFormat("model_hover_%s.png",m_FunMods.at(i)->m_sFunName.data());
		CUkQtRccBuilder builder;
		builder.SetPrefix("/style1");
		builder.AddImage(strName,m_FunMods.at(i)->img_normal_buff,m_FunMods.at(i)->img_normal_len);
		
		builder.AddImage(strNameHover,m_FunMods.at(i)->img_hover_buff,m_FunMods.at(i)->img_hover_len);
		builder.SaveRccToBuffer();
		QString root;
		if(!QResource::registerResource(builder.GetRccBuffer(),root))
		{
			SQt::ShowErrorBox("tip","load rcc error!");
		}
		QString styleName = QString("QToolButton#toolButton_11{border-image: url(:/style1/%1);font-size:14px; color:#e0e5ff; font-weight:bold;line-height:130px;\n"
				"}\n"
				"QToolButton#toolButton_11:hover{border-image: url(:/style1/%2);}\n"
				"")
				.arg(strName.data()).arg(strNameHover.data());
		toolButton_11->setStyleSheet(styleName);
		
		/*toolButton_11->setStyleSheet(QString("QToolButton#toolButton_11{border-image: url(:/resource/Resource/home_icon.png);font-size:14px; color:#e0e5ff; font-weight:bold;line-height:130px;\n"
			"}\n"
			"QToolButton#toolButton_11:hover{border-image: url(:/resource/Resource/home_icon_hov.png);}\n"
			""));*/
		toolButton_11->setAutoRepeat(false);
		toolButton_11->setToolButtonStyle(Qt::ToolButtonIconOnly);
		toolButton_11->setAutoRaise(true);
		toolButton_11->setProperty("value",i);
		connect(toolButton_11, SIGNAL(clicked()), this, SLOT(slot_leftButton()));
		gridLayout_5->addWidget(toolButton_11,i, 0, 1, 1);
	}
	verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	gridLayout_5->addItem(verticalSpacer, i, 0, 1, 1);
	//setMainWidget(0,0,0);
	QWidget *p = gridLayout_5->itemAt(0)->widget();
	if (p!=NULL)
	{
		((QToolButton *)p)->click();
	}
	
}

void CSspUiFrameNew::slot_max()
{
	if (b_showMax)
	{
		b_showMax = false;
		this->showNormal();
	}
	else
	{
		b_showMax = true;
		//this->move(0,0);
		this->showMaximized();
	}
	
}

void CSspUiFrameNew::slot_min()
{
	this->showMinimized();
}

void CSspUiFrameNew::slot_close()
{
	this->close();
}

void CSspUiFrameNew::slot_hideLeftWidget()
{
	if (b_showLeft)
	{
		b_showLeft = false;
		leftWidget->hide();
		leftRightButton->setStyleSheet(QString("QToolButton#leftRightButton{border-image: url(:/resource/Resource/slid_do_arrow.png);}\n"
			""));
	}
	else
	{
		b_showLeft = true;
		leftWidget->show();
		leftRightButton->setStyleSheet(QString("QToolButton#leftRightButton{border-image: url(:/resource/Resource/slid_arrow.png);}\n"
			""));
	}
}

void CSspUiFrameNew::slot_up()
{
	int n = scrollArea->verticalScrollBar()->sliderPosition();
	if (n>=20)
	{
		scrollArea->verticalScrollBar()->setSliderPosition(n-20);
	}
	else
	{
		scrollArea->verticalScrollBar()->setSliderPosition(0);
	}
		
}
void CSspUiFrameNew::slot_down()
{
	int n = scrollArea->verticalScrollBar()->sliderPosition();
	if ((n+20)<= m_FunMods.count()*100)
	{
		scrollArea->verticalScrollBar()->setSliderPosition(n+20);
	}
	else
	{
		scrollArea->verticalScrollBar()->setSliderPosition(m_FunMods.count()*100);
	}
}

void CSspUiFrameNew::slot_leftButton()
{
	QToolButton *button = qobject_cast<QToolButton *>(sender());
	int n=0;
	
	if (button)
	{
		n = button->property("value").toInt();
		CFrameFunModule *model = m_FunMods[n];
		if(model->m_Menus.count()==1 && model->m_Menus.at(0)->m_SubMenus.count()==1 && model->m_sTitle == model->m_Menus.at(0)->m_SubMenus.at(0)->m_sTitle)
		{
			QString a = "<font color=#f0f2ff >&nbsp;&nbsp;您当前的位置：";
			a += QString(model->m_sTitle.data());
			a += "</font>";
			this->label->setText(a);
			setMainWidget(n,0,0);
			return;
		}
		menu->clear();
		m_subMenu->clear();
		
		button->setContextMenuPolicy(Qt::ActionsContextMenu);
		
		for (int i=0;i<model->m_Menus.count();i++)
		{
			SString strName = SString::toFormat("model_%s_CFrameMenuL1_%s.png",model->m_sFunName.data(),model->m_Menus.at(i)->m_sFunName.data());
			CUkQtRccBuilder builder;
			builder.SetPrefix("/style1");
			builder.AddImage(strName,model->m_Menus.at(i)->img_normal_buff,model->m_Menus.at(i)->img_normal_len);
			//builder.SetPrefix("/style1");
			//builder.AddImage(strNameHover,m_FunMods.at(i)->img_hover_buff,m_FunMods.at(i)->img_hover_len);
			builder.SaveRccToBuffer();
			QString root;
			if(!QResource::registerResource(builder.GetRccBuffer(),root))
			{
				SQt::ShowErrorBox("tip","load rcc error!");
			}
			QIcon button_ico(QString(":/style1/%1").arg(strName.data()));
			

			QAction *action = new QAction(menu);   
			action->setText(model->m_Menus.at(i)->m_sTitle.data());
			action->setIcon(button_ico);
			
			m_subMenu = new QMenu(this);  
			m_subMenu->setStyleSheet(
				" QMenu {\
				background-color: #1268B3; \
				border: #3c86cc 1px solid;\
				width:200px;\
				color:#f0f2ff;\
				}\
				QMenu::item {\
				background-color: transparent;\
				padding:8px 32px;\
				margin:0px 8px;\
				border-bottom:1px solid #DBDBDB;\
				font-size:14px;\
				width:200px;\
				}\
				QMenu::item:selected { \
				background-color: #2dabf9;\
				}"

				);
			for (int j=0;j<model->m_Menus.at(i)->m_SubMenus.count();j++)
			{
				QAction *action1 = new QAction(m_subMenu); 
				SString str = model->m_Menus.at(i)->m_SubMenus.at(j)->m_sTitle;
				action1->setText(str.data());

				SString strName = SString::toFormat("model_%s_CFrameMenuL1_%s_CFrameMenuL2_%s.png",model->m_sFunName.data(),model->m_Menus.at(i)->m_sFunName.data(),model->m_Menus.at(i)->m_SubMenus.at(j)->m_sFunName.data());
				CUkQtRccBuilder builder;
				builder.SetPrefix("/style1");
				builder.AddImage(strName,model->m_Menus.at(i)->m_SubMenus.at(j)->img_normal_buff,model->m_Menus.at(i)->m_SubMenus.at(j)->img_normal_len);
				//builder.SetPrefix("/style1");
				//builder.AddImage(strNameHover,m_FunMods.at(i)->img_hover_buff,m_FunMods.at(i)->img_hover_len);
				builder.SaveRccToBuffer();
				QString root;
				if(!QResource::registerResource(builder.GetRccBuffer(),root))
				{
					SQt::ShowErrorBox("tip","load rcc error!");
				}
				QIcon button_ico(QString(":/style1/%1").arg(strName.data()));
				action1->setIcon(button_ico);

				QString str1=QString::fromLocal8Bit("%1/%2/%3").arg(n).arg(i).arg(j);
				action1->setProperty("value",str1);
				connect(action1,SIGNAL(triggered()),this,SLOT(slot_setBarTitle()));
				m_subMenu->addAction(action1);
			}
			if (model->m_Menus.at(i)->m_SubMenus.count() == 0)
			{
				QString str1=QString::fromLocal8Bit("%1/%2/%3").arg(n).arg(i).arg(-1);
				action->setProperty("value",str1);
				connect(action,SIGNAL(triggered()),this,SLOT(slot_setBarTitle()));
			}
			else
			{
				action->setMenu(m_subMenu);
			}
			menu->addAction(action); 
		}
		
		//menu->exec(this->cursor().pos());
		QPoint a = button->pos();
		int h = scrollArea->verticalScrollBar()->sliderPosition();
		menu->exec(this->mapToGlobal(QPoint(a.x()+button->width(),a.y()+button->height()-upButton->height()-h)));
	}
}


void CSspUiFrameNew::slot_setBarTitle()
{
	QAction *action  = qobject_cast<QAction *>(sender());
	QString str= "";
	QString text="";
	if (action)
	{
		str = action->property("value").toString();
		QStringList strlist=str.split("/");
		int n = strlist.at(0).toInt();
		int i = strlist.at(1).toInt();
		int j = strlist.at(2).toInt();
        /*SString str1 = "您当前的位置：";
		str1 +=	m_FunMods[n]->m_sTitle;
		str1 += " > ";
		str1 += m_FunMods[n]->m_Menus.at(i)->m_sTitle;
		str1 += " > ";
		str1 += m_FunMods[n]->m_Menus.at(i)->m_SubMenus.at(j)->m_sTitle;
		this->label->setText(str1.data());*/

		SString str;
		if(j == -1)
		{
			str.sprintf("<font color=#f0f2ff >&nbsp;&nbsp;您当前的位置：%s ></font><font color=#fcbd61 > %s</font>",m_FunMods[n]->m_sTitle.data(),m_FunMods[n]->m_Menus.at(i)->m_sTitle.data());
		}
		else if(m_FunMods[n]->m_Menus.at(i)->m_SubMenus.count() == 1)
		{
			if(m_FunMods[n]->m_sTitle == m_FunMods[n]->m_Menus.at(i)->m_SubMenus.at(j)->m_sTitle)
				str.sprintf("<font color=#f0f2ff >&nbsp;&nbsp;您当前的位置：%s</font>",m_FunMods[n]->m_sTitle.data());
			else
				str.sprintf("<font color=#f0f2ff >&nbsp;&nbsp;您当前的位置：%s ></font><font color=#fcbd61 > %s</font>",m_FunMods[n]->m_sTitle.data(), m_FunMods[n]->m_Menus.at(i)->m_SubMenus.at(j)->m_sTitle.data());
		}
		else
			str.sprintf("<font color=#f0f2ff >&nbsp;&nbsp;您当前的位置：%s > %s ></font><font color=#fcbd61 > %s</font>",m_FunMods[n]->m_sTitle.data(),m_FunMods[n]->m_Menus.at(i)->m_sTitle.data(), m_FunMods[n]->m_Menus.at(i)->m_SubMenus.at(j)->m_sTitle.data());
		this->label->setText(str.data());

		setMainWidget(n,i,j);
		/*CFrameMenuL2 *pMenuL2 = m_FunMods[n]->m_Menus.at(i)->m_SubMenus.at(j);
		CBaseView *pShow = NULL;
		CFrameFunModule *pModule = m_FunMods[n];

		if((pModule->m_pLastMenuL2 != NULL && pModule->m_pLastMenuL2 == pMenuL2 && 
			pModule->m_pLastMenuL2->m_pFunView != NULL && 
			pModule->m_pLastMenuL2->m_pFunView->isVisible()) ||
			pMenuL2 == NULL)
		{
			//忽略相同的视图
			if(m_pLastMenuL2 != NULL && m_pLastMenuL2->m_pFunView != NULL && m_pLastMenuL2 != pMenuL2)
			{
				m_pLastMenuL2->m_pFunView->OnPreShow();
				m_pLastMenuL2->m_pFunView->setVisible(false);
				m_pLastMenuL2->m_pFunView->OnAftShow();
				m_pLastMenuL2 = NULL;
			}
			return;
		}
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
			//addMainWidget(pMenuL2->m_pFunView);
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
				addMainWidget(pMenuL2->m_pFunView);
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
		if(pShow != NULL)
			pShow->OnAftShow();*/

	}
		
}

void CSspUiFrameNew::addMainWidget( QWidget *p )
{
	verticalLayout_3->addWidget(p);
	leftRightButton->raise();
}

void CSspUiFrameNew::setMainWidget( int n,int i,int j )
{
	bool tag = m_FunMods.count()>n && m_FunMods[n]!= NULL && m_FunMods[n]->m_Menus.count()>i &&m_FunMods[n]->m_Menus.at(i)!=NULL && m_FunMods[n]->m_Menus.at(i)->m_SubMenus.count()>j &&m_FunMods[n]->m_Menus.at(i)->m_SubMenus.at(j)!= NULL;
	if (!tag)
	{
		return;
	}
	CFrameMenuL2 *pMenuL2 = m_FunMods[n]->m_Menus.at(i)->m_SubMenus.at(j);
	CBaseView *pShow = NULL;
	CFrameFunModule *pModule = m_FunMods[n];

	if((pModule->m_pLastMenuL2 != NULL && pModule->m_pLastMenuL2 == pMenuL2 && 
		pModule->m_pLastMenuL2->m_pFunView != NULL && 
		pModule->m_pLastMenuL2->m_pFunView->isVisible()) ||
		pMenuL2 == NULL)
	{
		//忽略相同的视图
		if(m_pLastMenuL2 != NULL && m_pLastMenuL2->m_pFunView != NULL && m_pLastMenuL2 != pMenuL2)
		{
			m_pLastMenuL2->m_pFunView->OnPreShow();
			m_pLastMenuL2->m_pFunView->setVisible(false);
			m_pLastMenuL2->m_pFunView->OnAftShow();
			m_pLastMenuL2 = NULL;
		}
		return;
	}
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
		//addMainWidget(pMenuL2->m_pFunView);
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
			addMainWidget(pMenuL2->m_pFunView);
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
	if(pShow != NULL)
		pShow->OnAftShow();
}

void CSspUiFrameNew::mousePressEvent( QMouseEvent *e )
{
	/*last=e->globalPos();*/ 
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

void CSspUiFrameNew::mouseMoveEvent( QMouseEvent *e )
{
	/*int dx = e->globalX() - last.x();  
	int dy = e->globalY() - last.y();  
	last = e->globalPos();  
	move(x()+dx, y()+dy); */
	if (b_showMax && b_mousePressed)
	{
		b_showMax = false;
		b_mousePressed = false;
		showNormal();
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

void CSspUiFrameNew::mouseReleaseEvent( QMouseEvent *e )
{
	/*int dx = e->globalX() - last.x();  
	int dy = e->globalY() - last.y();  
	move(x()+dx, y()+dy);*/
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

QWidget* CSspUiFrameNew::returnBottomWidget()
{
	return bottomWidget;
}

QHBoxLayout * CSspUiFrameNew::returnBottomLayout()
{
	return horizontalLayout_3;
}

void CSspUiFrameNew::slot_login()
{
	if(ssp_gui::GetPtr()->IsLogin())
	{
		ssp_gui::GetPtr()->Logout();
		userNameLabel->setText(QString(""));
		headLabel->setVisible(false);
		userNameLabel->setVisible(false);
		userToolButton->setVisible(false);
		loginButton->setVisible(true);
	}		
	else
	{
		bool tag = ssp_gui::GetPtr()->Login();
		if (tag == true)
		{
			QString qstr = ssp_gui::GetPtr()->GetLoginUserName().data();
			userNameLabel->setText(qstr);
			headLabel->setVisible(true);
			userNameLabel->setVisible(true);
			userToolButton->setVisible(true);
			loginButton->setVisible(false);
		}
		else
		{
			headLabel->setVisible(false);
			userNameLabel->setVisible(false);
			userToolButton->setVisible(false);
			loginButton->setVisible(true);
			return;
		}
		
	}
	int n = gridLayout_5->count();
	for (int i =n-1;i>=0;i--)
	{
		QWidget *p = gridLayout_5->itemAt(i)->widget();
		gridLayout_5->removeWidget(p);
		delete p;
		p= NULL;
	}
	/*int m = verticalLayout_3->count();
	for (int i =m-1;i>=0;i--)
	{
		QWidget *p = verticalLayout_3->itemAt(i)->widget();
		verticalLayout_3->removeWidget(p);
		delete p;
		p= NULL;
	}*/


	GET_FP_CFG->LoadByDb(DB,ssp_gui::GetPtr()->GetLoginUser());



	m_FunMods.clear();
	Init((const char*)GET_FP_CFG->GetFunPointXmlText(),m_FunMods);
	initLeft();
	this->repaint();	
}

void CSspUiFrameNew::slot_logout()
{

}

void CSspUiFrameNew::slot_userToolButton()
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
	connect(tAction,SIGNAL(triggered()),this,SLOT(slot_login()));
	QAction *tAction1 = new QAction(tMenu);   
	tAction1->setText(QString("切换用户"));
	tAction1->setIcon(QIcon(":/resource/Resource/qhu.png"));
	tMenu->addAction(tAction1);
	connect(tAction1,SIGNAL(triggered()),this,SLOT(slot_userChange()));
	QPoint a = userToolButton->pos();
	int w = userToolButton->width();
	int h = userToolButton->height();
	int w1 = widget_2->width();
	QPoint bb = userToolButton->mapToGlobal(QPoint(a.x()-w1+w+w,a.y()-h-h));
	tMenu->exec(bb);
}

void CSspUiFrameNew::slot_userChange()
{
	bool tag = ssp_gui::GetPtr()->Login();
	if (tag == true)
	{
		QString qstr = ssp_gui::GetPtr()->GetLoginUserName().data();
		userNameLabel->setText(qstr);
	}
}

void CSspUiFrameNew::mouseDoubleClickEvent( QMouseEvent *e )
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


