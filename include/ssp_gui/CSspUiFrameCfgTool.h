#ifndef CSSPUIFRAMECFGTOOL_H
#define CSSPUIFRAMECFGTOOL_H

#include <QWidget>
#include "ssp_uiframe.h"

class SSP_GUI_EXPORT CSspUiFrameCfgTool : public CSspUiFrame
{
	Q_OBJECT

public:
	CSspUiFrameCfgTool(QWidget *parent);
	~CSspUiFrameCfgTool();

	virtual void Start();
	void initMainWidget();
	virtual void InitModule(const char* sFunctionPointXml);
	virtual void resizeEvent(QResizeEvent *e);
	void frame_resize();

	virtual bool GotoView(SString sFunName);

public slots:
	void slot_max();
	void slot_min();
	void slot_close();

	void slot_userToolButton();
	void slot_userChange();

	void slotLogin();	

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
	/*
	QWidget *widget_3;
	QLabel *timeLabel;
	QLabel *dateLabel;
	*/
	QWidget *widget_4;
	QToolButton *toolButton;
	QToolButton *minToolButton;
	QToolButton *maxToolButton;
	QToolButton *closeToolButton;

	QVBoxLayout *verticalLayout_3;//mainWidget²¼¾Ö
	QWidget *mainWidget;

	QWidget *bottomWidget;
	QHBoxLayout *horizontalLayout_3;//bottomWidget²¼¾Ö
	QLabel *bottomLabel;
	QSpacerItem *horizontalSpacer_2;

	bool b_showMax;	
	bool b_mousePressed;
	bool b_init;
	int i_x,i_y,i_w,i_h;

	QMenu *tMenu;

	void addMainWidget(QWidget *p);
	CBaseView* plugin_main_view;

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
#if 0//Îü¸½ÓÚÆÁÄ»
		QRect r = QApplication::desktop()->rect();
		QRect *desktop = &r;
		if(currentPos.x()<desktop->x()) { //Îü¸½ÓÚÆÁÄ»×ó²à
			currentPos.setX(desktop->x());
		}
		else if (currentPos.x()+this->width()>desktop->width()) { //Îü¸½ÓÚÆÁÄ»ÓÒ²à
			currentPos.setX(desktop->width()-this->width());
		}
		if(currentPos.y()<desktop->y()) { //Îü¸½ÓÚÆÁÄ»¶¥²¿
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

#endif // CSSPUIFRAMECFGTOOL_H
