#ifndef PANEL_SIM_PARAM_SET_H
#define PANEL_SIM_PARAM_SET_H

#include <QWidget>
#include "ui_panel_sim_para_set.h"
//#include "sim_manager.h"
#include "ssp_baseview.h"
#include "sim_config.h"
#include "SQt.h"
#include "sim_manager.h"
////////////////////  class CSmvOutCtrlDelegate  /////////////////////////
class CSmvOutCtrlDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	CSmvOutCtrlDelegate(QObject *parent, stuSclVtIedDatasetFcda* pCur);
	~CSmvOutCtrlDelegate();
	void setRangeMin(QString min) { strMinValue = min; }
	void setRangeMax(QString max) { strMaxValue = max; }

	QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex & index) const;
	void setEditorData(QWidget *editor,const QModelIndex &index) const;
	void setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const;
private:
	//QWidget * setIntType(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	//QWidget * setFloatType(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
private:
	QString strValueType;
	stuSclVtIedDatasetFcda* pCurFcda;
	QString strMinValue;
	QString strMaxValue;
};
////////////////////  class CGooseOutCtrlDelegate  /////////////////////////
class CGooseOutCtrlDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	CGooseOutCtrlDelegate(QObject *parent, stuSclVtIedDatasetFcda* pCur);
	~CGooseOutCtrlDelegate();
	void setRangeMin(QString min) { strMinValue = min; }
	void setRangeMax(QString max) { strMaxValue = max; }

	QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex & index) const;
	void setEditorData(QWidget *editor,const QModelIndex &index) const;
	void setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const;
private:
	QWidget * setIntType(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	QWidget * setFloatType(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
private:
	QString strValueType;
	stuSclVtIedDatasetFcda* pCurFcda;
	QString strMinValue;
	QString strMaxValue;
};
////////////////////////////
class panel_sim_param_set : public CBaseView
{
	Q_OBJECT

public:
	panel_sim_param_set(QWidget *parent);
	~panel_sim_param_set();
	virtual void OnPreShow();
	virtual void OnAftShow();
	virtual void OnRefresh();
	void setCurTreeItem(QTreeWidgetItem* curItem){m_pCurTreeItem = curItem;}
private:
	void addTableWidgetOperItem(int row,stuSclVtIedDatasetFcda* pCurFcda);
	void addGooseOutRow(stuSclVtIedDatasetFcda* pCur,bool bAddRated = false);
	void addSmvOutRow(stuSclVtIedDatasetFcda* pCur,bool bAddRated = false);
	void onRefreshData();
	//void initTable();
	void initSmvTable();
	void initGooseTable();
	void initTableData();
	void initSmvOutData();

	void initGooseOutData();

	void doSendSmvOut(stuSclVtIedSmvOut* pCur);
	void doGooseOutItemChanged(QTableWidgetItem *item);
	void doSmvOutItemChanged(QTableWidgetItem *item);

	int	getRowByObject(QObject *object);
	void doSendGooseOut(stuSclVtIedGooseOut* pCur);
private:
	Ui::panel_sim_para_set ui;
	QTreeWidgetItem* m_pCurTreeItem;
	stuSclVtIedSmvOut* m_pCurStuSclVtIedSmvOut;
	stuSclVtIedGooseOut* m_pCurStuSclVtIedGooseOut;
	int m_iSimTreeNodeType;
	bool m_bHasChanged;
	CSimConfig *m_pSimConfig;
	CSimManager* m_pSimManager;
	stuSimSubnetwork *m_pCurSubNet;
private slots:
	void slotItemChanged(QTableWidgetItem *item);
	void slotDoGooseOutPushButtonClicked();
	void slotDoGooseOutComboxChanged(int index);
};

#endif // PANEL_SIM_PARAM_SET_H
