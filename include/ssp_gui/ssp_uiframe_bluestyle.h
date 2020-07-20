#pragma once

#include <QWidget>
#include "ssp_uiframe.h"

class SSP_GUI_EXPORT CSspUiFrameBlueStyle : public CSspUiFrame
{
	Q_OBJECT

public:
	CSspUiFrameBlueStyle(QWidget *parent);
	~CSspUiFrameBlueStyle();
	virtual void Start();
	//virtual bool BackStart();
	void initLeft();
	virtual void InitModule(const char* sFunctionPointXml);
	virtual void resizeEvent(QResizeEvent *e);
	void frame_resize();
	virtual void AddStatusWidget(QWidget *pWidget)
	{
		pWidget->setParent(returnBottomWidget());
		returnBottomLayout()->addWidget(pWidget);
	};
	QWidget* returnBottomWidget();
	QHBoxLayout * returnBottomLayout();
	virtual bool GotoView(SString sFunName);
public slots:
		void slot_time();
		void slot_max();
		void slot_min();
		void slot_close();
		void slot_hideLeftWidget();
		void slot_up();
		void slot_down();
		void slot_leftButton();
		void slot_setBarTitle();
		void slot_login();
		void slot_userToolButton();
		void slot_userChange();
		void slot_btnBar1();
		void slot_btnBar2();
		void slot_btnBar3();
protected:  
	void mousePressEvent(QMouseEvent *e);  
	void mouseMoveEvent(QMouseEvent *e);  
	void mouseReleaseEvent(QMouseEvent *e); 
	void mouseDoubleClickEvent(QMouseEvent *e);
private:
	QVBoxLayout *verticalLayout_2;
	QWidget *topWidget;
	QGridLayout *gridLayout;
	QHBoxLayout *horizontalLayout_2;
	QPushButton *logoButton;
	QSpacerItem *horizontalSpacer;
	QWidget *widget_5;
	QHBoxLayout *horizontalLayout;
	QWidget *widget_2;
	QLabel *headLabel;
	QLabel *userNameLabel;
	QToolButton *userToolButton;
	QToolButton *loginButton;
	QWidget *widget_3;
	QLabel *timeLabel;
	QLabel *dateLabel;
	QWidget *widget_4;
	QToolButton *toolButton;
	QToolButton *minToolButton;
	QToolButton *maxToolButton;
	QToolButton *closeToolButton;
	QWidget *middleWidget;
	QGridLayout *gridLayout_4;
	QWidget *leftWidget;
	QGridLayout *gridLayout_2;//leftWidget布局
	QToolButton *upButton;
	QToolButton *downButton;
	QScrollArea *scrollArea;
	QWidget *scrollWidget;
	QGridLayout *gridLayout_5;//scrollWidget布局
	//QToolButton *toolButton_5;
	//QToolButton *toolButton_6;
	//QToolButton *toolButton_7;
	//QToolButton *toolButton_8;
	//QToolButton *toolButton_9;
	//QToolButton *toolButton_10;
	QSpacerItem *verticalSpacer;
	QWidget *rightWidget;
	QVBoxLayout *verticalLayout;//rightWidget布局
	//QGridLayout *gridLayout_3;
	//QStackedWidget *stackedWidget;
	QVBoxLayout *verticalLayout_3;//mainWidget布局
	QWidget *mainWidget;
	QToolButton *leftRightButton;
	QWidget *bottomWidget;
	QHBoxLayout *horizontalLayout_3;//bottomWidget布局
	QLabel *bottomLabel;
	QSpacerItem *horizontalSpacer_2;

	QWidget *barWidget;
	QHBoxLayout *horizontalLayout_4;//barWidget布局
	QLabel *label;
	QToolButton *btnBar1;
	QToolButton *btnBar2;
	QToolButton *btnBar3;
	QLabel *labelBar1;
	QLabel *labelBar2;


	QTimer *timer; 
	bool b_showMax;
	bool b_showLeft;
	bool b_mousePressed;
	bool b_init;
	int i_x,i_y,i_w,i_h;

	QMenu *menu; 
	QMenu *m_subMenu;
	QMenu *tMenu;
	QMenu *barMenu;


	void addMainWidget(QWidget *p);
	void setMainWidget( int n,int i,int j );
	//QPoint last; 
	void clearLeftButtonCheck();
	bool GetByMenul2(SString sName,int &n,int &i,int &j );
	void setBarTitle(int n,int i,int j );

	enum ResizeRegion
	{
		Default,
		North,
		NorthEast,
		East,
		SouthEast,
		South,
		SouthWest,
		West,
		NorthWest
	};
	bool m_drag, m_move;
	QPoint dragPos, resizeDownPos;
	int resizeBorderWidth;
	ResizeRegion resizeRegion;
	QRect mouseDownRect;
	void setResizeCursor(ResizeRegion region){
		switch (region)
		{
		case North:
		case South:
			setCursor(Qt::SizeVerCursor);
			break;
		case East:
		case West:
			setCursor(Qt::SizeHorCursor);
			break;
		case NorthWest:
		case SouthEast:
			setCursor(Qt::SizeFDiagCursor);
			break;
		default:
			setCursor(Qt::SizeBDiagCursor);
			break;
		}
	};
	ResizeRegion getResizeRegion(QPoint clientPos){
		if (clientPos.y() <= resizeBorderWidth) {
			if (clientPos.x() <= resizeBorderWidth)
				return NorthWest;
			else if (clientPos.x() >= this->width() - resizeBorderWidth)
				return NorthEast;
			else
				return North;
		}
		else if (clientPos.y() >= this->height() - resizeBorderWidth) {
			if (clientPos.x() <= resizeBorderWidth)
				return SouthWest;
			else if (clientPos.x() >= this->width() - resizeBorderWidth)
				return SouthEast;
			else
				return South;
		}
		else {
			if (clientPos.x() <= resizeBorderWidth)
				return West;
			else
				return East;
		}
	};
	void handleMove(QPoint pt)
	{
		QPoint currentPos = pt - dragPos;
#if 0//吸附于屏幕
		QRect r = QApplication::desktop()->rect();
		QRect *desktop = &r;
		if(currentPos.x()<desktop->x()) { //吸附于屏幕左侧
			currentPos.setX(desktop->x());
		}
		else if (currentPos.x()+this->width()>desktop->width()) { //吸附于屏幕右侧
			currentPos.setX(desktop->width()-this->width());
		}
		if(currentPos.y()<desktop->y()) { //吸附于屏幕顶部
			currentPos.setY(desktop->y());
		}
#endif
		move(currentPos);
	};
	void handleResize()
	{
		int xdiff = QCursor::pos().x() - resizeDownPos.x();
		int ydiff = QCursor::pos().y() - resizeDownPos.y();
		switch (resizeRegion)
		{
		case East:
			{
				resize(mouseDownRect.width()+xdiff, this->height());
				break;
			}
		case West:
			{
				resize(mouseDownRect.width()-xdiff, this->height());
				move(resizeDownPos.x()+xdiff, this->y());
				break;
			}
		case South:
			{
				resize(this->width(),mouseDownRect.height()+ydiff);
				break;
			}
		case North:
			{
				resize(this->width(),mouseDownRect.height()-ydiff);
				move(this->x(), resizeDownPos.y()+ydiff);
				break;
			}
		case SouthEast:
			{
				resize(mouseDownRect.width() + xdiff, mouseDownRect.height() + ydiff);
				break;
			}
		case NorthEast:
			{
				resize(mouseDownRect.width()+xdiff, mouseDownRect.height()-ydiff);
				move(this->x(), resizeDownPos.y()+ydiff);
				break;
			}
		case NorthWest:
			{
				resize(mouseDownRect.width()-xdiff, mouseDownRect.height()-ydiff);
				move(resizeDownPos.x()+xdiff, resizeDownPos.y()+ydiff);
				break;
			}
		case SouthWest:
			{
				resize(mouseDownRect.width()-xdiff, mouseDownRect.height()+ydiff);
				move(resizeDownPos.x()+xdiff, this->y());
				break;
			}
		}
	};
	
};

