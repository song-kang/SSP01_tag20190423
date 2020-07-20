#include "dlgdelviewpoint.h"
#include <QMessageBox>

#define COLUMN_SCENE_ID		0
#define COLUMN_VP_ID		1
#define COLUMN_DEFAULT		2
#define COLUMN_NAME			3

DlgDelViewPoint::DlgDelViewPoint(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	init();

	connect(ui.pushButton_del,SIGNAL(clicked()),this,SLOT(slotOk()));
	connect(ui.pushButton_cancel,SIGNAL(clicked()),this,SLOT(close()));
	connect(ui.tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(slotItemClicked(QTableWidgetItem*)));
}

DlgDelViewPoint::~DlgDelViewPoint()
{

}

void DlgDelViewPoint::init()
{
	row = 0;
	initTableWidget();
}

void DlgDelViewPoint::initTableWidget()
{
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);		//整行选择模式
	ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);		//单选模式
	ui.tableWidget->setFocusPolicy(Qt::NoFocus);								//去除焦点，无虚框
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);			//设置充满表宽度
	ui.tableWidget->setStyleSheet("selection-background-color:lightblue;");		//设置选中背景色
	ui.tableWidget->verticalHeader()->setDefaultSectionSize(25);				//设置行高
	ui.tableWidget->horizontalHeader()->setHighlightSections(false);			//点击表时不对表头行光亮
	ui.tableWidget->setAlternatingRowColors(true);								//设置交替行色
	ui.tableWidget->verticalHeader()->setVisible(false);						//去除最前列

	ui.tableWidget->setColumnWidth(COLUMN_SCENE_ID,80);
	ui.tableWidget->setColumnWidth(COLUMN_VP_ID,80);
	ui.tableWidget->setColumnWidth(COLUMN_DEFAULT,80);
}

void DlgDelViewPoint::start()
{
	unsigned long pos = 0;
	CSsp3DSceneViewPoint *v = m_3DModel->m_SceneViewPoints.FetchFirst(pos);
	while (v)
	{
		if (v->m_iSceneId == m_sceneId)
		{
			unsigned long pos1 = 0;
			t_ssp_3d_scene_viewpoint *p = v->m_ViewPoints.FetchFirst(pos1);
			while (p)
			{
				ui.tableWidget->setRowCount(++row);

				QTableWidgetItem *item = new QTableWidgetItem(tr("%1").arg(p->scene_id));
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(item->flags() & (~Qt::ItemIsEditable));
				ui.tableWidget->setItem(row-1,COLUMN_SCENE_ID,item);

				item = new QTableWidgetItem(tr("%1").arg(p->vpoint_id));
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(item->flags() & (~Qt::ItemIsEditable));
				ui.tableWidget->setItem(row-1,COLUMN_VP_ID,item);

				item = new QTableWidgetItem(tr("%1").arg(p->def_vpoint == 1 ? tr("默认") : tr("非默认")));
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(item->flags() & (~Qt::ItemIsEditable));
				ui.tableWidget->setItem(row-1,COLUMN_DEFAULT,item);

				item = new QTableWidgetItem(tr("%1").arg(p->vpoint_name));
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(item->flags() & (~Qt::ItemIsEditable));
				ui.tableWidget->setItem(row-1,COLUMN_NAME,item);

				p = v->m_ViewPoints.FetchNext(pos1);
			}
		}
		v = m_3DModel->m_SceneViewPoints.FetchNext(pos);
	}
}

void DlgDelViewPoint::slotItemClicked(QTableWidgetItem *item)
{
	m_sceneId = ui.tableWidget->item(item->row(),COLUMN_SCENE_ID)->text().toInt();
	m_viewPointId = ui.tableWidget->item(item->row(),COLUMN_VP_ID)->text().toInt();
}

void DlgDelViewPoint::slotOk()
{
	SString sql;
	sql.sprintf("delete from t_ssp_3d_scene_viewpoint where scene_id=%d and vpoint_id=%d",m_sceneId,m_viewPointId);
	if (!DB->ExecuteSQL(sql))
	{
		QMessageBox::warning(this,tr("告警"),tr("实时库删除视点失败。"));
		return;
	}
	else
	{
		QMessageBox::information(this,tr("提示"),tr("实时库删除视点成功。"));
		if (!MDB->ExecuteSQL(sql))
			QMessageBox::information(this,tr("提示"),tr("内存库删除视点失败。"));
	}

	accept();
}
