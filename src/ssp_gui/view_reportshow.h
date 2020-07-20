#ifndef VIEW_REPORTSHOW_H
#define VIEW_REPORTSHOW_H

#include <QWidget>
#include <QtCore>  
#include <QtGui>  
#include "ssp_baseview.h"
#include "ssp_database.h"
#include "SSvgWnd.h"
#include "ssp_datawindow.h"
#include "view_reportsetting.h"
#include "ui_view_reportshow.h"
#include "view_dw_condlg.h"
class PaintWidget;

QColor GetColorByAttr(SString sExtAttr,SString sAttrName);
class view_reportshow : public CBaseView
{
	Q_OBJECT

public:
	view_reportshow(int reportNo,QWidget *parent = 0);
	~view_reportshow();
	PaintWidget *paintWidget;
	QWidget *topWidget;
	QMap<int,CSsp_Dataset*> getdbSetMap();
	int getWidth(){return width;}
private:
	Ui::view_reportshow ui;
	int reportNo;
	QTimer *timer;
	QPrinter printer;
	QPushButton *conButton;
	QPushButton *printViewButton;
	QPushButton *printPDFButton;
	QString con;
	int width;
	int height;
	int dsNo;
	SPtrList<CSsp_DwColumn> *m_fieldlist;//所有字段信息
	CSsp_DatawindowMgr *m_dataWindow;
	SDatabaseOper *m_pOper;
	view_dw_condlg *condlg;
	QMap<int,CSsp_Dataset*>dbSetMap;//图表条件查询用
	QMap<int,view_dw_condlg *>condlgMap;//图表条件查询用
	SString dsName;
	CSsp_Dataset* dbSet;
	int LargeGap;
	
	SPtrList<CSsp_DwColumn> *getFieldInfo(SString sWname,SPtrList<CSsp_DwColumn> *cFinfo=0);
	void printPage(QPainter *painter,const QList<divStruct> &divList,SRecordset &reportItemSets);
	void initPage(QPainter *painter,const QList<divStruct> &divList,SRecordset &reportItemSets,int pageNum,int firstHeight);
	void printPDF(QPrinter &printer);
private slots:
	void onPushButton_con();
	void onPushButton_chartcon();
	void onPushButton_print();
	void onPushButton_printview();
	void onPushButton_printPDF();
	void timerOut();
	void printPages(QPrinter &printer);
	void printPages(QPrinter *printer);
};
////////////////////////////////////////////////////
class PaintWidget : public QWidget 
{ 
	Q_OBJECT 

public: 
	PaintWidget(QWidget *parent = 0);
	void setDbSet(CSsp_Dataset*dbSet);
	void setreportNo(int reportNo);
	void setCon(QString con){this->con=con;}
protected: 
	void paintEvent(QPaintEvent *event); 
private:
	SDatabaseOper *m_pOper;
	CSsp_Dataset* dbSet;
	int reportNo;
	QString con;
	CSsp_DatawindowMgr *m_dataWindow;
	view_reportshow *parent;
}; 
#endif // VIEW_REPORTSHOW_H
