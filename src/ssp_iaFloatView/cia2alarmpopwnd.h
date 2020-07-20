#ifndef CIA2ALARMPOPWND_H
#define CIA2ALARMPOPWND_H

#include <QWidget>
#include <QScrollBar>
#include <QItemDelegate>
#include <QPainter>
#include "ui_cia2alarmpopwnd.h"

#define SEVERITY_ALL			0
#define SEVERITY_GENEARL		1
#define SEVERITY_SECONDARY		2
#define SEVERITY_IMPORTANT		3
#define SEVERITY_CRITICAL		4

struct stuHisItem 
{
	int sn;
	int soc;
	int usec;
	int severity;
	int subNo;
	int bayNo;
	QString conclusion;
	bool isConfirm;
	QString confirmUser;
	int confirmSoc;
};

class ItemDelegate : public QItemDelegate  
{  
	Q_OBJECT

public:  
	ItemDelegate() {}
	~ItemDelegate() {}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const  
	{  
		QStyleOptionViewItem viewOption(option);

		viewOption.palette.setColor(QPalette::HighlightedText,index.data(Qt::ForegroundRole).value<QColor>());

		QItemDelegate::paint(painter,viewOption,index);
	}  
};

class ssp_iaFloatView;
class CIa2AlarmPopWnd : public QWidget
{
	Q_OBJECT

public:
	CIa2AlarmPopWnd(QWidget *parent = 0);
	~CIa2AlarmPopWnd();

	void Start();
	void SetPushButtonQss(QPushButton *btn,int radius = 5,int padding = 8,int width = 1,
		const QString &borderColor = "#000000",		  //边框颜色
		const QString &normalColor = "#FFFFFF",       //正常颜色
		const QString &normalTextColor = "#000000",   //文字颜色
		const QString &hoverColor = "#646464",        //悬停颜色
		const QString &hoverTextColor = "#000000",    //悬停文字颜色
		const QString &pressedColor = "#646464",      //按下颜色
		const QString &pressedTextColor = "#000000"); //按下文字颜色
	void InsertMapHisItem(stuHisItem *tItem);
	void InsertTableWidget(stuHisItem *tItem);
	void PopupWnd(stuHisItem *tItem);
	int GetCurrentSeverity() { return m_iCurrentSeverity; }
	void CallSlotBtnAllClicked() { SlotBtnAllClicked(); }
	bool OnConfirmHistory(int sn);
	void OnRefreshTableWidget();

private:
	Ui::CIa2AlarmPopWnd ui;

	ssp_iaFloatView *ssp_floatwnd;
	QMap<int,QString> m_mapSeveritys;
	QMap<int,QString> m_mapSubs;
	QMap<int,QString> m_mapBays;
	QMap<int,QString> m_mapFilterBays;
	QMap<int,stuHisItem*> m_mapHisItems;
	int m_iRow;

	QTimer *m_iRestoreTimer;
	ItemDelegate *m_itemDelegate;

	QMenu *m_popMenu;
	QAction *m_actConfirm;
	QAction *m_actConfirmAll;
	QAction *m_actClear;
	QAction *m_actClearAll;

	int m_iCurrentSeverity;
	bool m_bIsExtend;
	bool m_bIsStart;

protected:
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);

private:
	void Init();
	void InitMenu();
	void InitTableWidget();
	void InitTreeWidget();
	void InitMap();
	void ShowTableWidget(int severity = 0);
	void RefreshTableWidget(int severity = 0);
	void SetChildCheckState(QTreeWidgetItem *item,int column,Qt::CheckState state);
	void SetParentCheckState(QTreeWidgetItem *item);
	void SetMapFilterBays();
	void SaveCsvFile(QString fileName);
	void SavePdfFile(QString fileName);

private slots:
	void SlotTableWidgetItemClicked(QTableWidgetItem *item);
	void SlotTableWidgetItemDoubleClicked(QTableWidgetItem *item);
	void SlotTableWidgetClickedRightMenu(const QPoint &pos);
	void slotTreeWidgetItemChanged(QTreeWidgetItem *item,int column);
	void SlotRestoreTimeout();
	void SlotActConfirm();
	void SlotActConfirmAll();
	void SlotActClear();
	void SlotActClearAll();
	void SlotBtnAllClicked();
	void SlotBtnCritrialClicked();
	void SlotBtnImportantClicked();
	void SlotBtnSecondaryClicked();
	void SlotBtnGeneralClicked();
	void SlotBtnExtendClicked();
	void SlotBtnSettingClicked();
	void SlotBtnOkClicked();
	void SlotBtnConfirmAllClicked();
	void SlotBtnStartStopClicked();
	void SlotBtnSaveClicked();
	void SlotBtnQueryClicked();
	
};

#endif // CIA2ALARMPOPWND_H
