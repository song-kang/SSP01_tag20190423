#pragma once

#include "ssp_baseview.h"


namespace Ui
{
	class ViewNoCommonPluginClass;
}


class PluginTreeModel;
class ViewNoCommonPlugin : public CBaseView
{
	Q_OBJECT

	enum TreeType
	{
		Group = 1,
		Module,					// 模块
		ModuleFunPointSupport,	// 模块功能
	};

public:
	ViewNoCommonPlugin(QWidget *parent = 0);
	virtual ~ViewNoCommonPlugin();

public:
	void setSysFunNameList(QVector<QString> sysFunNameList);

public slots:
	void onAddSysFun(const QString &sysFunName);
	void onDeleteSysFun(const QString &sysFunName);

protected slots:
	void onQuery();
	void onCheckedShowFunPointChanged(int state);

protected:
	Ui::ViewNoCommonPluginClass *ui;
	PluginTreeModel *m_model;
	QVector<QStandardItem *> m_funPointItemList;
	QMap<QString, int> m_mapSysFunName;
	QMap<QString, QStandardItem *> m_mapFunPointItem;
};

class PluginTreeModel : public QStandardItemModel
{
	Q_OBJECT

public:
	virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
};
