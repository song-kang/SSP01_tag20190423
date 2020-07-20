#include "SspUiFrameSubstation.h"
#include <QTimer>


CSspUiFrameSubstation::CSspUiFrameSubstation(QWidget *parent)
	: CSspUiFrame(parent), b_showLeft(true)
{
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::Widget);	

	user_menu_ = NULL;
	dt_timer_ = NULL;

	//setMouseTracking(true);
}


CSspUiFrameSubstation::~CSspUiFrameSubstation(void)
{
	
	if (user_menu_) {
		delete user_menu_;
		user_menu_ = NULL;
	}	
}

void CSspUiFrameSubstation::Start() {
	ssp_gui::GetPtr()->SetUiFramePtr(this);		

	this->setObjectName(QString("this"));
	this->setAutoFillBackground(true);
	QPalette palette;
	QDesktopWidget* desktopWidget = QApplication::desktop();
	QRect screenRect = desktopWidget->screenGeometry();
	if (screenRect.width() >= 1600 && screenRect.height() >= 1200)
		palette.setBrush(QPalette::Background, QBrush(QPixmap(":/resource/Resource/main_1600x1200.png")));
	else {
		//QPixmap pixmap = QPixmap(":/resource/Resource/subs_bg.png");
		//palette.setBrush(QPalette::Background, QBrush(pixmap));
		palette.setBrush(QPalette::Background, QBrush(QPixmap(":/resource/Resource/main.png")));
	}
	this->setPalette(palette);

	//总体界面layout
	frameVLayout = new QVBoxLayout(this);
	frameVLayout->setSpacing(0);
	frameVLayout->setObjectName(QString("frameVLayout"));
	frameVLayout->setContentsMargins(0, 0, 0, 0);	

	//top
	topWidget = new QWidget(this);
	topWidget->setObjectName(QString("topWidget"));
	topWidget->setMinimumSize(QSize(0, 80));
	topWidget->setMaximumSize(QSize(16777215, 80));
	//topWidget->setStyleSheet(QString("QWidget#topWidget{background:rgba(0,19,107,40)}"));	
	topWidget->setStyleSheet(QString("QWidget#topWidget{border-image: url(:/resource/Resource/subs_top.png);}"));
	
	//topWidget布局	
	topHLayout = new QHBoxLayout(topWidget);
	topHLayout->setObjectName(QString("topHLayout"));
	topHLayout->setSpacing(0);
	topHLayout->setContentsMargins(0, 0, 0, 0);		

	//显示用户名及时间
	//infoWidget = new QWidget(topWidget);
	//infoWidget->setObjectName(QString("infoWidget"));
	
	infoHLayout = new QHBoxLayout();
	infoHLayout->setObjectName(QString("infoHLayout"));
	infoHLayout->setSpacing(0);
	infoHLayout->setContentsMargins(0, 0, 0, 0);

	loginNameLbl = new QLabel();
	loginNameLbl->setObjectName("loginNameLbl");
	loginNameLbl->setStyleSheet(QString("QLabel#loginNameLbl{color:white; font-size:16px;}"));
	loginNameLbl->setText("");
	
	userToolButton = new QToolButton();
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

	connect(userToolButton, SIGNAL(clicked()), this, SLOT(slotUserToolButton()));
	
	//不需要显示时间
	dateTimeLbl = new QLabel();
	dateTimeLbl->setObjectName("dateTimeLbl");
	dateTimeLbl->setStyleSheet(QString("QLabel#dateTimeLbl{color:white; font-size:16px;}"));
	//slotTimeout();	
	//dt_timer_ = new QTimer(this);
	//connect(dt_timer_, SIGNAL(timeout()), this, SLOT(slotTimeout()));	
	//dt_timer_->start(60000);
	
	
	infoHLayout->addStretch();
	infoHLayout->addWidget(loginNameLbl);
	infoHLayout->addSpacing(5);
	infoHLayout->addWidget(userToolButton);
	infoHLayout->addSpacing(15);
	infoHLayout->addWidget(dateTimeLbl);

	//名称label
	subsNameLbl = new QLabel();
	subsNameLbl->setObjectName("subsNameLbl");
	subsNameLbl->setText("220kV南河变电站辅助设备监控系统");
	subsNameLbl->setStyleSheet(QString("QLabel#subsNameLbl{color:white; font-size:20px; font:bold;}"));

	subsNameHLayout = new QHBoxLayout();
	subsNameHLayout->setObjectName(QString("subsNameHLayout"));
	subsNameHLayout->setSpacing(0);
	subsNameHLayout->setContentsMargins(0, 0, 0, 0);

	subsNameHLayout->addStretch();
	subsNameHLayout->addWidget(subsNameLbl);
	subsNameHLayout->addStretch();		
		

	topVMainLayout = new QVBoxLayout();
	topVMainLayout->setSpacing(0);
	topVMainLayout->setObjectName(QString("frameVLayout"));
	topVMainLayout->setContentsMargins(0, 0, 0, 0);
	topVMainLayout->addSpacing(10);
	topVMainLayout->addLayout(infoHLayout);
	topVMainLayout->addLayout(subsNameHLayout);

	topHLayout->addLayout(topVMainLayout);

	frameVLayout->addWidget(topWidget);
	//frameVLayout->addLayout(topHLayout);	
	
	
	//middle widget
	middleWidget = new QWidget(this);
	middleWidget->setObjectName(QString("middleWidget"));	

	middleMainLayout = new QHBoxLayout(middleWidget);
	topVMainLayout->setSpacing(0);
	topVMainLayout->setObjectName(QString("middleMainLayout"));
	topVMainLayout->setContentsMargins(0, 0, 0, 0);

	//left
	leftWidget = new QWidget(middleWidget);
	leftWidget->setObjectName(QString("leftWidget"));
	//leftWidget->setMinimumSize(QSize(0, 50));
	//leftWidget->setMaximumSize(QSize(16777215, 50));	
	int leftMinHeight = screenRect.height() - 80 - 50 - 30;
	leftWidget->setMinimumHeight(leftMinHeight);
	leftWidget->setMaximumWidth(200);
	//leftWidget->setMaximumSize(180, leftMaxHeight);
	leftWidget->setStyleSheet(QString("QWidget#leftWidget{background-color:rgba(64,190,255,76);}"));	
	//leftWidget->setStyleSheet(QString("QWidget#leftWidget{border-image: url(:/resource/Resource/subs_bottom.png);}"));

	//隐藏或显示左边导航的小按钮
	leftRightButton = new QToolButton(leftWidget);
	leftRightButton->setObjectName(QString("leftRightButton"));
	//leftRightButton->setGeometry(QRect(0, 430, 16, 36));
	leftRightButton->setFixedSize(16, 36);
	leftRightButton->setCursor(QCursor(Qt::PointingHandCursor));
	leftRightButton->setStyleSheet(QString("QToolButton#leftRightButton{border-image: url(:/resource/Resource/slid_arrow.png);}\n"
		""));
	//leftRightButton->raise();
	connect(leftRightButton,SIGNAL(clicked()),this,SLOT(slotHideLeftWidget()));

	QVBoxLayout *leftRightBtnLayout = new QVBoxLayout();
	leftRightBtnLayout->setSpacing(0);
	leftRightBtnLayout->setObjectName(QString("leftRightBtnLayout"));
	leftRightBtnLayout->setContentsMargins(0, 0, 0, 0);
	leftRightBtnLayout->addWidget(leftRightButton);
	leftRightBtnLayout->addStretch();


	leftLayout = new QHBoxLayout();
	leftLayout->setSpacing(0);
	leftLayout->setObjectName(QString("leftLayout"));
	leftLayout->setContentsMargins(0, 0, 0, 0);

	leftLayout->addWidget(leftWidget);
	leftLayout->addLayout(leftRightBtnLayout);
	//leftLayout->addSpacing(30);	

	//middle
	centerWidget = new QWidget(middleWidget);
	centerWidget->setObjectName(QString("centerWidget"));
	//centerWidget->setMinimumSize(QSize(0, 50));
	//centerWidget->setMaximumSize(QSize(16777215, 50));
	centerWidget->setStyleSheet(QString("QWidget#centerWidget{background-color:rgba(64,190,255,38);}"));
	//centerWidget->setStyleSheet(QString("QWidget#centerWidget{border-image: url(:/resource/Resource/subs_bottom.png);}"));

	centerLayout = new QVBoxLayout();
	centerLayout->setSpacing(0);
	centerLayout->setObjectName(QString("centerLayout"));
	centerLayout->setContentsMargins(0, 0, 0, 0);

	centerLayout->addWidget(centerWidget);
	
	/*
	//right-top
	rightTopWidget = new QWidget(middleWidget);
	rightTopWidget->setObjectName(QString("rightTopWidget"));
	//rightTopWidget->setMinimumSize(QSize(0, 50));
	//rightTopWidget->setMaximumSize(QSize(16777215, 50));
	rightTopWidget->setMaximumWidth(180);
	int rightTopMinHeight = screenRect.height() - 80 - 50 - 200 - 15;
	rightTopWidget->setMinimumHeight(rightTopMinHeight);
	rightTopWidget->setStyleSheet(QString("QWidget#rightTopWidget{background-color:rgba(64,190,255,76);}"));
	//rightTopWidget->setStyleSheet(QString("QWidget#rightTopWidget{border-image: url(:/resource/Resource/subs_bottom.png);}"));

	//right-bottom
	rightBottomWidget = new QWidget(middleWidget);
	rightBottomWidget->setObjectName(QString("rightBottomWidget"));
	rightBottomWidget->setMinimumSize(QSize(0, 200));
	rightBottomWidget->setMaximumSize(QSize(180, 200));
	rightBottomWidget->setStyleSheet(QString("QWidget#rightBottomWidget{background-color:rgba(64,190,255,38);}"));
	//rightBottomWidget->setStyleSheet(QString("QWidget#rightBottomWidget{border-image: url(:/resource/Resource/subs_bottom.png);}"));

	rightLayout = new QVBoxLayout();
	rightLayout->setSpacing(0);
	rightLayout->setObjectName(QString("rightLayout"));
	rightLayout->setContentsMargins(0, 0, 0, 0);

	rightLayout->addWidget(rightTopWidget);
	rightLayout->addSpacing(15);
	rightLayout->addWidget(rightBottomWidget);
	rightLayout->addStretch();
	*/

	middleMainLayout->addLayout(leftLayout);
	middleMainLayout->addLayout(centerLayout);
	//middleMainLayout->addLayout(rightLayout);	

	frameVLayout->addWidget(middleWidget);
	
	//frameVLayout->addStretch();

	//bottom
	bottomWidget = new QWidget(this);
	bottomWidget->setObjectName(QString("bottomWidget"));
	bottomWidget->setMinimumSize(QSize(0, 50));
	bottomWidget->setMaximumSize(QSize(16777215, 50));
	bottomWidget->setStyleSheet(QString("QWidget#bottomWidget{border-image: url(:/resource/Resource/subs_bottom.png);}"));

	/*
	bottomWidget->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
	QPalette paletteBottom = bottomWidget->palette();
	paletteBottom.setBrush(QPalette::Background,
		QBrush(QPixmap(":/resource/Resource/subs_bottom.png").scaled(// 缩放背景图.
		bottomWidget->size(),
		Qt::IgnoreAspectRatio,
		Qt::SmoothTransformation)));            // 使用平滑的缩放方式
	bottomWidget->setPalette(paletteBottom); 	
	
	
	QFont font2;
	font2.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
	bottomWidget->setFont(font2);
	//bottomWidget->setStyleSheet(QString("QWidget#bottomWidget{background:rgba(64,190,255,.15)}"));	
	//bottomWidget->setStyleSheet(QString("QWidget#bottomWidget{background:rgba(0, 0, 0, 35)}"));
	*/
	bottomHLayout = new QHBoxLayout(bottomWidget);
	bottomHLayout->setObjectName(QString("bottomHLayout"));
	bottomHLayout->setSpacing(0);
	bottomHLayout->setContentsMargins(0, 0, 0, 0);

	frameVLayout->addWidget(bottomWidget);
	
}

void CSspUiFrameSubstation::InitModule(const char* sFunctionPointXml) {

}

void CSspUiFrameSubstation::resizeEvent(QResizeEvent *e) {
	QWidget::resizeEvent(e);

	frame_resize();
}

void CSspUiFrameSubstation::frame_resize() {
	
}

bool CSspUiFrameSubstation::GotoView(SString sFunName) {
	return false;
}

void CSspUiFrameSubstation::slotTimeout() {
	QTime qtimeObj = QTime::currentTime();  
	QString strTime = qtimeObj.toString("hh:mm");   
	QDate qdateObj = QDate::currentDate();  
	QString strDate = qdateObj.toString("yyyy/MM/dd"); //星期、月份、天、年   

	dateTimeLbl->setText(strDate + " " + strTime);
}

void CSspUiFrameSubstation::slotUserToolButton() {
	
	if (user_menu_)
	{
		user_menu_->clear();
	}
	else
	{
		user_menu_ = new QMenu(this);
	}

	user_menu_->setStyleSheet(
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

	QAction *tAction = new QAction(user_menu_);   
	tAction->setText(QString("注销"));
	tAction->setIcon(QIcon(":/resource/Resource/tc.png"));
	user_menu_->addAction(tAction);
	connect(tAction,SIGNAL(triggered()),this,SLOT(slotLogin()));
	
	
	QAction *tAction1 = new QAction(user_menu_);   
	tAction1->setText(QString("退出"));
	tAction1->setIcon(QIcon(":/resource/Resource/qhu.png"));
	user_menu_->addAction(tAction1);	
	connect(tAction1,SIGNAL(triggered()),this,SLOT(slotQuit()));	

	
	QPoint a = userToolButton->pos();
	int w = userToolButton->width();
	int h = userToolButton->height();
	int w1 = 0;//widget_2->width();
	QPoint bb = userToolButton->mapToGlobal(QPoint(a.x()+w,a.y()-h));
	user_menu_->exec(bb);	
}

void CSspUiFrameSubstation::slotLogin()
{
	if(ssp_gui::GetPtr()->IsLogin())
	{
		ssp_gui::GetPtr()->LogoutSubstation();
		loginNameLbl->setText(QString(""));		
		loginNameLbl->setVisible(false);
		userToolButton->setVisible(false);		
	}	

	bool tag = ssp_gui::GetPtr()->LoginSubstation(true);
	if (tag == true)
	{
		QString qstr = ssp_gui::GetPtr()->GetLoginUserName().data();
		loginNameLbl->setText(qstr);			
		loginNameLbl->setVisible(true);
		userToolButton->setVisible(true);			
	}
	else
	{			
		loginNameLbl->setVisible(false);
		userToolButton->setVisible(false);			
		slotQuit();
	}
}

void CSspUiFrameSubstation::slotQuit() {
	/*
	if (!ssp_gui::GetPtr()->IsGuarderLogin()) {
		if (!ssp_gui::GetPtr()->LoginSubstation(false)) {
			return;
		}
	}
	*/
	this->close();
}

void CSspUiFrameSubstation::slotHideLeftWidget() {
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