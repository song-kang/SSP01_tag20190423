#include "dlgsaveviewpoint.h"
#include <QMessageBox>

#define COLUMN_SCENE_ID		0
#define COLUMN_VP_ID		1
#define COLUMN_DEFAULT		2
#define COLUMN_NAME			3

DlgSaveViewPoint::DlgSaveViewPoint(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	init();

	connect(ui.pushButton_ok,SIGNAL(clicked()),this,SLOT(slotOk()));
	connect(ui.pushButton_cancel,SIGNAL(clicked()),this,SLOT(close()));
	connect(ui.radioButton_add,SIGNAL(clicked()),this,SLOT(slotRadioButtonAdd()));
	connect(ui.radioButton_replace,SIGNAL(clicked()),this,SLOT(slotRadioButtonReplace()));
	connect(ui.tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(slotItemClicked(QTableWidgetItem*)));
}

DlgSaveViewPoint::~DlgSaveViewPoint()
{

}

void DlgSaveViewPoint::init()
{
	row = 0;
	initTableWidget();
	
	ui.lineEdit->setEnabled(ui.radioButton_add->isChecked());
	ui.tableWidget->setEnabled(ui.radioButton_replace->isChecked());
}

void DlgSaveViewPoint::initTableWidget()
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

void DlgSaveViewPoint::start()
{
	setDefaultEnable();

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
				//item->setFlags(item->flags() & (~Qt::ItemIsEditable));
				ui.tableWidget->setItem(row-1,COLUMN_NAME,item);

				p = v->m_ViewPoints.FetchNext(pos1);
			}
		}
		v = m_3DModel->m_SceneViewPoints.FetchNext(pos);
	}

	m_viewPointId = 0;
	m_viewPointDefault = 0;
}

void DlgSaveViewPoint::slotItemClicked(QTableWidgetItem *item)
{
	m_sceneId = ui.tableWidget->item(item->row(),COLUMN_SCENE_ID)->text().toInt();
	m_viewPointId = ui.tableWidget->item(item->row(),COLUMN_VP_ID)->text().toInt();
	m_viewPointDefault = ui.tableWidget->item(item->row(),COLUMN_DEFAULT)->text() == tr("默认") ? 1 : 0;
	m_viewPointName = ui.tableWidget->item(item->row(),COLUMN_NAME)->text().toStdString().data();
}

void DlgSaveViewPoint::slotOk()
{
	if (ui.radioButton_add->isChecked())
	{
		if (ui.lineEdit->text().isEmpty())
		{
			QMessageBox::warning(this,tr("告警"),tr("请输入视点名称。"));
			return;
		}

		int maxVPId = getMaxViewPointId();
		if (maxVPId == -1)
			return;

		SString sql;
		sql.sprintf("insert into t_ssp_3d_scene_viewpoint"
			"(scene_id,vpoint_id,vpoint_name,def_vpoint,eye_x,eye_y,eye_z,eye_forw_x,eye_forw_y,eye_forw_z,cent2eye,x_angle,z_angle) values"
			"(%d,%d,'%s',%d,%f,%f,%f,%f,%f,%f,%f,%f,%f)",m_sceneId,++maxVPId,ui.lineEdit->text().toStdString().data(),m_viewPointDefault,
			eyeX,eyeY,eyeZ,centerX,centerY,centerZ,eye2center,xAngle,zAngle);
		if (!DB->ExecuteSQL(sql))
		{
			QMessageBox::warning(this,tr("告警"),tr("实时库增加视点失败。"));
			return;
		}
		else
		{
			QMessageBox::information(this,tr("提示"),tr("实时库增加视点成功。"));
			if (!MDB->ExecuteSQL(sql))
				QMessageBox::information(this,tr("提示"),tr("内存库增加视点失败。"));
		}
	}
	else if (ui.radioButton_replace->isChecked())
	{
		if (!m_sceneId || !m_viewPointId)
		{
			QMessageBox::warning(this,tr("告警"),tr("请选择需要替换的视点。"));
			return;
		}

		SString sql;
		sql.sprintf("update t_ssp_3d_scene_viewpoint set "
			"vpoint_name='%s',eye_x=%f,eye_y=%f,eye_z=%f,eye_forw_x=%f,eye_forw_y=%f,eye_forw_z=%f,cent2eye=%f,x_angle=%f,z_angle=%f "
			"where scene_id=%d and vpoint_id=%d and def_vpoint=%d",m_viewPointName.data(),eyeX,eyeY,eyeZ,centerX,centerY,centerZ,eye2center,xAngle,zAngle,
			m_sceneId,m_viewPointId,m_viewPointDefault);
		if (!DB->ExecuteSQL(sql))
		{
			QMessageBox::warning(this,tr("告警"),tr("实时库替换视点失败。"));
			return;
		}
		else
		{
			QMessageBox::information(this,tr("提示"),tr("实时库替换视点成功。"));
			if (!MDB->ExecuteSQL(sql))
				QMessageBox::information(this,tr("提示"),tr("内存库替换视点失败。"));
		}
	}

	accept();
}

void DlgSaveViewPoint::slotRadioButtonAdd()
{
	ui.lineEdit->setEnabled(true);
	ui.tableWidget->setEnabled(false);
	setDefaultEnable();
}

void DlgSaveViewPoint::slotRadioButtonReplace()
{
	ui.lineEdit->setEnabled(false);
	ui.checkBox->setEnabled(false);
	ui.tableWidget->setEnabled(true);
}

void DlgSaveViewPoint::setDefaultEnable()
{
	SRecordset rs;
	int cnt = DB->Retrieve(SString::toFormat("select count(*) from t_ssp_3d_scene_viewpoint where scene_id=%d and def_vpoint=1",m_sceneId),rs);
	if (cnt <= 0)
	{
		ui.pushButton_ok->setEnabled(false);
		return;
	}
	else
	{
		int count = rs.GetValue(0,0).toInt();
		if (count)
			ui.checkBox->setEnabled(false);
		else
			ui.checkBox->setEnabled(true);
	}
}

int DlgSaveViewPoint::getMaxViewPointId()
{
	SRecordset rs;
	int cnt = DB->Retrieve(SString::toFormat("select max(vpoint_id) from t_ssp_3d_scene_viewpoint where scene_id=%d",m_sceneId),rs);
	if (cnt <= 0)
	{
		QMessageBox::warning(this,tr("告警"),tr("获取场景中最大视点编号失败。"));
		return -1;
	}

	return rs.GetValue(0,0).toInt();
}
