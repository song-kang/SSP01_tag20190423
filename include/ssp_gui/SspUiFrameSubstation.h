#ifndef SSP_UIFRAME_SUBSTATION_H_
#define SSP_UIFRAME_SUBSTATION_H_

#include <QWidget>
#include "ssp_uiframe.h"
#include <QToolButton>
#include <QMenu>

class SSP_GUI_EXPORT CSspUiFrameSubstation : public CSspUiFrame
{
	Q_OBJECT
public:
	CSspUiFrameSubstation(QWidget *parent);
	~CSspUiFrameSubstation(void);

	virtual void Start();
	virtual void InitModule(const char* sFunctionPointXml);
	virtual void resizeEvent(QResizeEvent *e);
	void frame_resize();

	virtual bool GotoView(SString sFunName);

private:
	QVBoxLayout* frameVLayout;

	QVBoxLayout* topVMainLayout;
	QHBoxLayout *topHLayout;
	QHBoxLayout* subsNameHLayout;
	QWidget* topWidget;	
	QHBoxLayout *infoHLayout;
	QLabel* loginNameLbl;
	QToolButton *userToolButton;
	QLabel* dateTimeLbl;
	QLabel* subsNameLbl;

	
	QHBoxLayout* middleMainLayout;	
	QWidget* middleWidget;
	QHBoxLayout *leftLayout;
	QWidget* leftWidget;
	QWidget* centerWidget;
	QVBoxLayout* centerLayout;
	
	/*
	QVBoxLayout* rightLayout;
	QWidget* rightTopWidget;
	QWidget* rightBottomWidget;
	*/

	/*
	QWidget* winCtrlWidget;
	QToolButton *minToolButton;
	QToolButton *maxToolButton;
	QToolButton *closeToolButton;
	*/

	QHBoxLayout *bottomHLayout;
	QWidget* bottomWidget;

	//隐藏或显示左边导航的小按钮
	QToolButton *leftRightButton;

	//显示时间
	QTimer *dt_timer_; 
	//用户注销菜单
	QMenu *user_menu_; 

	bool b_showLeft;

public slots:
	void slotTimeout();
	void slotUserToolButton();
	void slotLogin();
	void slotQuit();

	void slotHideLeftWidget();
};

#endif //SSP_UIFRAME_SUBSTATION_H_