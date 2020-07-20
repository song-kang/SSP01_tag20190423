#include "view_nocommon_plugin.h"
#include "ui_view_nocommon_plugin.h"
#include <QStringListModel>
#include "ssp_uiframe.h"
#include "ssp_plugin_mgr.h"
#include <QPixmap>


ViewNoCommonPlugin::ViewNoCommonPlugin(QWidget *parent)
	: CBaseView(parent)
	, ui(new Ui::ViewNoCommonPluginClass)
{
	ui->setupUi(this);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::Tool);

	m_model = new PluginTreeModel();
	m_model->setHorizontalHeaderLabels(QStringList() << "模块名" << "版本" << "描述");
	ui->treeView->setModel(m_model);
	ui->treeView->setColumnWidth(0, 250);
	ui->treeView->setColumnWidth(1, 80);
	ui->treeView->setColumnWidth(2, 300);
	ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->treeView->setAlternatingRowColors(true);
	ui->treeView->setDragDropMode(QAbstractItemView::DragOnly);
	QStandardItem* itemGroup = new QStandardItem("所有");
	itemGroup->setData(Group);
	m_model->appendRow(itemGroup);

	CSspUiFrame *uiFrame = CSspUiFrame::GetPtr();
	SPtrList<CSspPlugin> pluginList;
	uiFrame->m_pluginMgr->m_Plugins.copyto(pluginList);
	for (int i = 0; i < pluginList.count(); ++i)
	{
		CSspPlugin *plugin = pluginList.at(i);
		QFileInfo fileInfo(plugin->m_sPluginFile.data());

		int row = itemGroup->rowCount();

		// name
		QStandardItem *itemModuleName = new QStandardItem(fileInfo.fileName());
		itemGroup->insertRow(row, itemModuleName);
		itemModuleName->setData(Module);

		// version
		QString version = plugin->m_pSSP_GetPluginVer();
		QStandardItem *itemVersion = new QStandardItem(version);
		itemGroup->setChild(row, 1, itemVersion);

		// desc
		QString desc = plugin->m_pSSP_GetPluginName();
		QStandardItem *itemDesc = new QStandardItem(desc);
		itemGroup->setChild(row, 2, itemDesc);

		// module fun point support
		QString funPointSupport = plugin->m_pSSP_FunPointSupported();
		QStringList funPointList = funPointSupport.split(";");
		for (QStringList::iterator iter = funPointList.begin(); iter != funPointList.end(); ++iter)
		{
			if (iter->length() == 0)
				continue;

			QStringList valueList = iter->split("=");
			if (valueList.size() != 2)
			{
				LOGERROR("错误的功能支持类型,%s", iter->toAscii().data());
				continue;
			}

			QString name = valueList.at(0);
			QString desc = valueList.at(1);

			int funPointRow = itemModuleName->rowCount();

			QStandardItem *itemName = new QStandardItem(name);
			QStandardItem *itemDesc = new QStandardItem(desc);
			itemName->setData(ModuleFunPointSupport);

			itemModuleName->insertRow(funPointRow, itemName);
			itemModuleName->setChild(funPointRow, 2, itemDesc);
			m_funPointItemList.push_back(itemName);
			m_mapFunPointItem[name] = itemName;
		}
	}

	ui->checkBoxShowFunPoint->setChecked(true);
	ui->treeView->expandAll();
	ui->toolButtonAll->setVisible(false);

	connect(ui->btnQuery, SIGNAL(clicked()), this, SLOT(onQuery()));
	connect(ui->checkBoxShowFunPoint, SIGNAL(stateChanged(int)), this, SLOT(onCheckedShowFunPointChanged(int)));
}

ViewNoCommonPlugin::~ViewNoCommonPlugin()
{
	delete ui;
	delete m_model;
}

void ViewNoCommonPlugin::setSysFunNameList(QVector<QString> sysFunNameList)
{
	QMap<QString, QStandardItem *>::iterator iterTreeItem;

	for (QVector<QString>::iterator iter = sysFunNameList.begin(); iter != sysFunNameList.end(); ++iter)
	{
		QString pluginName = *iter;
		QStringList strlist = pluginName.split("=");
		if (strlist.size() > 1)
			pluginName = strlist.at(0);

		m_mapSysFunName[pluginName] = 1;

		iterTreeItem = m_mapFunPointItem.find(pluginName);
		if (iterTreeItem != m_mapFunPointItem.end())
		{
			iterTreeItem.value()->setBackground(QBrush(0x778899));
		}
	}
}

void ViewNoCommonPlugin::onAddSysFun(const QString &sysFunName)
{
	QString pluginName = sysFunName;
	QStringList strlist = pluginName.split("=");
	if (strlist.size() > 1)
		pluginName = strlist.at(0);

	QMap<QString, QStandardItem *>::iterator iterTreeItem;

	iterTreeItem = m_mapFunPointItem.find(pluginName);
	if (iterTreeItem != m_mapFunPointItem.end())
	{
		iterTreeItem.value()->setBackground(QBrush(0x778899));
		m_mapSysFunName[sysFunName]++;
	}
}

void ViewNoCommonPlugin::onDeleteSysFun(const QString &sysFunName)
{
	QString pluginName = sysFunName;
	QStringList strlist = pluginName.split("=");
	if (strlist.size() > 1)
		pluginName = strlist.at(0);

	m_mapSysFunName[pluginName]--;
	QMap<QString, QStandardItem *>::iterator iterTreeItem;
	QMap<QString, int>::iterator iterSysFun;
	iterSysFun = m_mapSysFunName.find(pluginName);
	iterTreeItem = m_mapFunPointItem.find(pluginName);

	if (iterTreeItem != m_mapFunPointItem.end() &&
		iterSysFun != m_mapSysFunName.end())
	{
		if (iterSysFun.value() == 0)
			iterTreeItem.value()->setBackground(QBrush(Qt::white));
	}
}

void ViewNoCommonPlugin::onQuery()
{
	QString retrievalText = ui->editKeyword->text();
	retrievalText.trimmed();
	QStandardItemModel *devModel = qobject_cast<QStandardItemModel *>(ui->treeView->model());

	int rowCount = devModel->rowCount();
	for (int row = 0; row < rowCount; ++row)
	{
		QStandardItem *rootItem = devModel->item(row, 0);
		if (!rootItem)
			continue;

		int pluginRowCount = rootItem->rowCount();
		for (int pluRow = 0; pluRow < pluginRowCount; ++pluRow)
		{
			QStandardItem *pluginItem = rootItem->child(pluRow, 0);
			if (!pluginItem)
				continue;

			int showCount = 0;
			int childRowCount = pluginItem->rowCount();
			for (int childRow = 0; childRow < childRowCount; ++childRow)
			{
				QStandardItem *childItem = pluginItem->child(childRow, 0);
				if (!childItem)
					continue;

				QStandardItem *descItem = pluginItem->child(childRow, 2);
				if (!descItem)
					continue;

				if (retrievalText.isEmpty() || childItem->text().indexOf(retrievalText) >= 0
					|| descItem->text().indexOf(retrievalText) >= 0)
				{
					ui->treeView->setRowHidden(childItem->row(), childItem->index().parent(), false);
					++showCount;
				}
				else
					ui->treeView->setRowHidden(childItem->row(), childItem->index().parent(), true);
			}

			ui->treeView->setRowHidden(pluginItem->row(), pluginItem->index().parent(), showCount > 0 ? false : true);
		}
	}
}

void ViewNoCommonPlugin::onCheckedShowFunPointChanged(int state)
{
	for (QVector<QStandardItem *>::iterator iter = m_funPointItemList.begin(); iter != m_funPointItemList.end(); ++iter)
	{
		QStandardItem *item = *iter;

		if (state == Qt::Checked)
			ui->treeView->setExpanded(item->index().parent(), true);
		else
			ui->treeView->setExpanded(item->index().parent(), false);
	}
}

//////////////////////////////////////////////////////////////////////////
QMimeData * PluginTreeModel::mimeData(const QModelIndexList &indexes) const
{
	if (indexes.count() <= 0)
		return 0;

	QByteArray name = indexes.at(0).data(Qt::DisplayRole).toByteArray();
	QByteArray desc;
	if (indexes.count() >= 2)
		desc = indexes.at(2).data(Qt::DisplayRole).toByteArray();
	QMimeData *data = new QMimeData;
	data->setText(name);
	data->setData("plugin/name", name);
	data->setData("plugin/desc", desc);
	return data;
}

