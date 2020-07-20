#ifndef PANEL_SIM_MODIFY_DATASET_H
#define PANEL_SIM_MODIFY_DATASET_H

#include <QWidget>
#include "ui_panel_sim_para_set.h"
#include "ssp_baseview.h"
#include "sim_config.h"
#include "SQt.h"
#include "sim_manager.h"
////////////////////  class CDataSetCtrlDelegate  /////////////////////////
class CDataSetCtrlDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	CDataSetCtrlDelegate(QObject *parent,QTableWidget* pCur);
	~CDataSetCtrlDelegate();
	void setRangeMin(QString min) { strMinValue = min; }
	void setRangeMax(QString max) { strMaxValue = max; }

	QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex & index) const;
	void setEditorData(QWidget *editor,const QModelIndex &index) const;
	void setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const;

private:
	QString strValueType;
	//stuSclVtIedDatasetFcda* pCurFcda;
	QString strMinValue;
	QString strMaxValue;
	QTableWidget* m_pCurTableWidget;
};
/////////////////////////////
class panel_sim_modify_dataset : public CBaseView
{
	Q_OBJECT

public:
	panel_sim_modify_dataset(QWidget *parent);
	~panel_sim_modify_dataset();
	virtual void OnRefresh();
	void setCurTreeItem(QTreeWidgetItem* curItem){m_pCurTreeItem = curItem;}
	SString m_sMmsDsIedName;
private:
	//void onRefreshData();
	void initTable();
	void refreshData();
private:
	Ui::panel_sim_para_set ui;
	QTreeWidgetItem* m_pCurTreeItem;
	stuSclVtIedDataset* m_pCurIedDataSet;
	bool m_bHasChanged;
	CSimConfig *m_pSimConfig;
	CSimMmsServer *m_pSimMmsServer;
	//CSimManager* m_pSimManager;
private slots:
	void slotItemChanged(QTableWidgetItem *item);
};

#endif // PANEL_SIM_MODIFY_DATASET_H
