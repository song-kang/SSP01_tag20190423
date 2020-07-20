#include "view_nocommon.h"
#include "ssp_gui.h"
#include "QFlag"
#include "view_nocommon_plugin.h"
#include <QDragEnterEvent>
#include <QMimeData>


view_nocommon::view_nocommon(QWidget *parent,SString sFunName)
	: CBaseView(parent),m_funName(sFunName)
{
	ui.setupUi(this);
	m_tree = new pcell_CTree<SString>;
	m_bconnect = false;
	QPalette pal;
	pal.setColor(QPalette::Base, QColor(255, 255, 255));
	pal.setColor(QPalette::AlternateBase, QColor(10, 10, 10,40));
	
	ui.treeWidget->setPalette(pal);
	ui.treeWidget->setAlternatingRowColors(true);
	ui.treeWidget->setDragDropMode(QAbstractItemView::DropOnly);
	setAcceptDrops(true);
	InitDb();
	//InitWidget(m_funName);
	
	QVector<QString> funList;
	GetSysFunName(funList);
	m_viewNoCommonPlugin = new ViewNoCommonPlugin(parent);
	m_viewNoCommonPlugin->hide();
	m_viewNoCommonPlugin->setSysFunNameList(funList);
}

view_nocommon::~view_nocommon()
{
	delete m_tree;
	delete m_viewNoCommonPlugin;
}

bool view_nocommon::InitWidget(SString sWname)
{
	SSP_GUI->ShowWaiting();
	m_funTop = PrepareForFunPoint("top");
	ClearUnusefulData(/*pdb*/);
	if (sWname=="sys.fun.point")
	{

		SetAsFunPoint();
		SSP_GUI->HideWaiting();
		return true;
	}
	if (sWname=="sys.grt.usr")
	{
		SetAsGrtUsr();
		SSP_GUI->HideWaiting();
		return true;
	}
	if (sWname=="sys.grt.grp")
	{
		SetAsGrtGrp();
		SSP_GUI->HideWaiting();
		return true;
	}
	SSP_GUI->HideWaiting();
	return false;
}
void view_nocommon::InitDb()
{
	m_pOper=DB;
}

SString view_nocommon::PrepareForFunPoint(SString sTop)
{
	if (sTop=="")
	{
		sTop="top";
	}
	SString sSql = "select fun_key,p_fun_key,name,idx,auth,type,ref_sn,dsc,path from t_ssp_fun_point where p_fun_key is null or p_fun_key='' ";
	SRecordset rs;
	m_pOper->RetrieveRecordset(sSql,rs);
	if (rs.GetRows()!=1)
	{
		m_pOper->ExecuteSQL("delete * from t_ssp_fun_point");
		m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_fun_point (fun_key)values('%s')",sTop.data()));
		return sTop;
	}else
	{
		return rs.GetValue(0,0);
	}
}

void view_nocommon::SetAsFunPoint()
{
	QStringList ql;
	if (ui.listWidget!=NULL)
	{
		delete ui.listWidget;
		ui.listWidget=NULL;
	}
	if (ui.label!=NULL)
	{
		delete ui.label;
		ui.label = NULL;
	}
	//ui.treeWidget->initUicfgAndReportRef();
	ui.treeWidget->setManual(false);
	m_tree->clear();
	ui.treeWidget->clear();
	ui.treeWidget->setStyleSheet("QTreeWidget::item{height:23px;border:0.5px solid gray;border-top-color:transparent;border-left-color:transparent;}"
		"QTreeView::item:selected{background-color: lightblue;color: black;}"
		"QTreeView::branch{selection-background-color: lightblue;}");
	ui.treeWidget->setRmenuAndCheck(true,false,true);
	ql<<tr("功能点")<<tr("名称")<<tr("序号")<<tr("权限认证")<<tr("类型")<<tr("引用序号")<<tr("说明")<<tr("关联路径")<<tr("正常图片")<<tr("按下状态图片")<<tr("鼠标移上图片")<<tr("不可用图片");
	ui.treeWidget->addTree("t_ssp_fun_point","fun_key","p_fun_key",m_funTop,ql,m_funFields);
	ui.treeWidget->header()->setResizeMode(QHeaderView::Interactive);
	ui.treeWidget->header()->setMovable(false);
	ui.treeWidget->resizeColumnToContents(2);
	ui.treeWidget->resizeColumnToContents(3);
	ui.treeWidget->resizeColumnToContents(5);
	if (!m_bconnect)
	{
		QObject::connect(ui.treeWidget,SIGNAL(addNode(QTreeWidgetItem*,QTreeWidgetItem*)),this,SLOT(OnAddNode(QTreeWidgetItem*,QTreeWidgetItem*)));
		QObject::connect(ui.treeWidget,SIGNAL(delNode(QTreeWidgetItem*,QTreeWidgetItem*)),this,SLOT(OnDelNode(QTreeWidgetItem*,QTreeWidgetItem*)));
		QObject::connect(ui.treeWidget,SIGNAL(editNode(QTreeWidgetItem*,QTreeWidgetItem*)),this,SLOT(OnEditNode(QTreeWidgetItem*,QTreeWidgetItem*)));
		QObject::connect(ui.treeWidget,SIGNAL(updateNode(QTreeWidgetItem*,SString,int)),this,SLOT(OnUpdateNode(QTreeWidgetItem*,SString,int)));
	m_bconnect = true;
	}
	ui.treeWidget->setCurrentItem(ui.treeWidget->topLevelItem(0));
	ui.treeWidget->setManual(true);
}
void view_nocommon::SetAsGrtUsr()
{
	ui.label->setText(tr("用户"));
	ui.treeWidget->setManual(false);
	m_tree->clear();
	ui.treeWidget->clear();
	ui.treeWidget->setStyleSheet("QTreeWidget::item{height:20px;}");
	ui.treeWidget->setRmenuAndCheck(false,true,false);
	QStringList sl1,sl2;
	ui.treeWidget->addTree("t_ssp_fun_point","fun_key","p_fun_key",m_funTop,sl1,sl2/*QStringList(),QStringList()*/);
	ui.treeWidget->headerItem()->setText(0,tr("功能点"));
	ui.treeWidget->headerItem()->setText(1,tr("权限"));
	ui.treeWidget->header()->setMovable(false);
	ui.listWidget->clear();
	SString sSql;
	sSql = "select usr_sn,t_ssp_user.grp_code,t_ssp_user_group.name,t_ssp_user.name from t_ssp_user join t_ssp_user_group on (t_ssp_user.grp_code = t_ssp_user_group.grp_code) order by t_ssp_user.grp_code,usr_sn";
	SRecordset rs;
	m_pOper->RetrieveRecordset(sSql,rs);
	for (int i = 0;i<rs.GetRows();i++)
	{
		QListWidgetItem *listItem = new QListWidgetItem();
		QVariant val,val1;
		val.setValue(QString(rs.GetValue(i,0).data()));
		val1.setValue(QString(rs.GetValue(i,1).data()));
		listItem->setData(3,val);
		listItem->setData(1,val1);
		listItem->setText((rs.GetValue(i,2)+" - " + rs.GetValue(i,3)).data());
		ui.listWidget->addItem(listItem);
	}


	if (!m_bconnect)
	{
		QObject::connect(ui.listWidget,SIGNAL(choosed(QListWidgetItem*)),this,SLOT(OnListItemChoosed_user(QListWidgetItem*)));
		QObject::connect(ui.treeWidget,SIGNAL(updateCheck(QTreeWidgetItem*,int)),this,SLOT(OnUpdateCheck_user(QTreeWidgetItem*,int)));
		m_bconnect = true;
	}
	ui.listWidget->setCurrentRow(0);
	ui.treeWidget->setManual(true);
}
void view_nocommon::SetAsGrtGrp()
{
	ui.label->setText(tr("群组"));
	ui.treeWidget->setManual(false);
	m_tree->clear();
	ui.treeWidget->clear();
	ui.treeWidget->setRmenuAndCheck(false,true,false);
	QStringList sl1,sl2;
	ui.treeWidget->addTree("t_ssp_fun_point","fun_key","p_fun_key",m_funTop,sl1,sl2/*QStringList(),QStringList()*/);
	ui.treeWidget->headerItem()->setText(0,tr("功能点"));
	ui.treeWidget->headerItem()->setText(1,tr("权限"));
	ui.treeWidget->header()->setMovable(false);
	ui.listWidget->clear();
	ui.treeWidget->setStyleSheet("QTreeWidget::item{height:20px;}");
	SString sSql;
	sSql = "select grp_code,name from t_ssp_user_group";
	SRecordset rs;
	m_pOper->RetrieveRecordset(sSql,rs);
	for (int i = 0;i<rs.GetRows();i++)
	{
		QListWidgetItem *listItem = new QListWidgetItem();
		QVariant val;
		val.setValue(QString(rs.GetValue(i,0).data()));
		listItem->setData(1,val);
		listItem->setText(rs.GetValue(i,1).data());
		listItem->setToolTip(QString(rs.GetValue(i,0).data()));
		ui.listWidget->addItem(listItem);
	}
	if (!m_bconnect)
	{
		QObject::connect(ui.listWidget,SIGNAL(choosed(QListWidgetItem*)),this,SLOT(OnListItemChoosed_group(QListWidgetItem*)));
		QObject::connect(ui.treeWidget,SIGNAL(updateCheck(QTreeWidgetItem*,int)),this,SLOT(OnUpdateCheck_group(QTreeWidgetItem*,int)));
		m_bconnect = true;
	}
	ui.listWidget->setCurrentRow(0);
	ui.treeWidget->setManual(true);
}

void view_nocommon::OnAddNode(QTreeWidgetItem * node,QTreeWidgetItem *parentNode)
{
	SString sSql;
	sSql = SString::toFormat("insert into t_ssp_fun_point(fun_key,p_fun_key,auth,type) values('%s','%s',0,0)",node->text(0).toStdString().c_str(),parentNode->text(0).toStdString().c_str());
	m_pOper->ExecuteSQL(sSql);
}
void view_nocommon::OnDelNode(QTreeWidgetItem * node,QTreeWidgetItem *parentNode)
{
	S_UNUSED(parentNode);
	SString sSql;
	sSql = SString::toFormat("delete from t_ssp_fun_point where fun_key='%s'",node->text(0).toStdString().c_str());
	m_pOper->ExecuteSQL(sSql);
	sSql = SString::toFormat("delete from t_ssp_fun_point where p_fun_key='%s'",node->text(0).toStdString().c_str());
	m_pOper->ExecuteSQL(sSql);

	m_viewNoCommonPlugin->onDeleteSysFun(node->text(0));
}

void view_nocommon::OnEditNode(QTreeWidgetItem * node,QTreeWidgetItem *parentNode)
{
	m_viewNoCommonPlugin->show();
}

void view_nocommon::OnUpdateNode(QTreeWidgetItem * node,SString sPre,int column)
{
	SString sSql,sSql1;
	SString sKey,sValue;

	if (column==3)
	{
		sValue=(node->checkState(3)==Qt::Unchecked)?"0":"1";
	}
	else if (column==4)
	{
		if (node->text(column)=="静态功能点")
			sValue="0";
		else if (node->text(column)=="动态菜单项")
			sValue="1";
		else if (node->text(column)=="组态界面功能点")
			sValue="2";
		else if (node->text(column)=="动态报表功能点")
			sValue="3";
		else if (node->text(column)=="隐式功能点")
			sValue="4";
		else if (node->text(column)=="WEB页面")
			sValue="5";
		else if (node->text(column)=="JAVA三维场景界面类")
			sValue="6";

	} else if (column == 5)
	{
		QString strRef = node->text(column);
		if (strRef.isEmpty())
		{
			sValue="NULL";
		} else {
			QString strType = node->text(4);
			QString strWndSn;
			if (strType == "动态报表功能点")
			{
				strWndSn = ui.treeWidget->m_reportInfoMap.key(strRef);
			} else {
				strWndSn = ui.treeWidget->m_uicfgWndMap.key(strRef);
			}
			 
			//QStringList tmp = strRef.split("-");
			//sValue="'"+tmp.at(0).toStdString()+"'";
			sValue="'"+strWndSn.toStdString()+"'";
		}
	}
	else
	{
		if (node->text(column)=="")
		{
			sValue="NULL";
		}else
		{
			sValue="'"+node->text(column).toStdString()+"'";

		}
	}
	if (column==0)
	{
		sKey = sPre;
		sSql1 = SString::toFormat("update t_ssp_fun_point set p_fun_key='%s' where p_fun_key='%s'",node->text(0).toStdString().c_str(),sPre.data());
		m_pOper->ExecuteSQL(sSql1);
	}else
	{
		sKey = node->text(0).toStdString();
	}
	sSql = SString::toFormat("update t_ssp_fun_point set %s=%s where fun_key='%s'",m_funFields[column].data(),sValue.data(),sKey.data());
	m_pOper->ExecuteSQL(sSql);

	if (column == 0)
	{
		m_viewNoCommonPlugin->onDeleteSysFun(sPre.data());
		m_viewNoCommonPlugin->onAddSysFun(node->text(0));
	}
}

void view_nocommon::OnListItemChoosed_group(QListWidgetItem *item)
{
	ui.treeWidget->setManual(false);
	m_choosedItem = item;
	ui.treeWidget->emptyCheck(0);
	SString sCode = item->data(1).toString().toStdString();
	SString sSql;
	sSql = SString::toFormat("select fun_key,auth from t_ssp_usergroup_auth join t_ssp_user_group on (t_ssp_usergroup_auth.grp_code = t_ssp_user_group.grp_code) where (t_ssp_user_group.grp_code = '%s')",sCode.data());
	SRecordset rs;
	m_pOper->RetrieveRecordset(sSql,rs);
	for (int i = 0;i<rs.GetRows();i++)
	{
// 		if(i%10==0)
// 			SQt::ProcessUiEvents();
		if (rs.GetValue(i,1)=="1")
		{
			QTreeWidgetItem *rItem;
			rItem = ui.treeWidget->chooseItem(0,rs.GetValue(i,0).data());
			if(rItem != NULL)
				rItem->setCheckState(1,Qt::Checked);
		}
	}
	ui.treeWidget->setManual(true);
}
void view_nocommon::OnUpdateCheck_group(QTreeWidgetItem* item,int column)
{
	SString sSql;
	SRecordset rs;
	SString sGrpCode = m_choosedItem->data(1).toString().toStdString();
	SString sFunKey = item->data(0,5).toString().toStdString();
	SString sAuth = item->checkState(column)==Qt::Checked?"1":"0";
	sSql =SString::toFormat("select grp_code,fun_key,auth from t_ssp_usergroup_auth where grp_code='%s' and fun_key='%s'",sGrpCode.data(),sFunKey.data());
	if (m_pOper->RetrieveRecordset(sSql,rs)<=0)
	{
		sSql = SString::toFormat("insert into t_ssp_usergroup_auth values('%s','%s',%s)",sGrpCode.data(),sFunKey.data(),sAuth.data());		

	}else
	{
		sSql = SString::toFormat("update t_ssp_usergroup_auth set auth=%s where grp_code='%s' and fun_key='%s'",sAuth.data(),sGrpCode.data(),sFunKey.data());		
	}
	m_pOper->ExecuteSQL(sSql);
}

void view_nocommon::OnListItemChoosed_user(QListWidgetItem *item)
{

	ui.treeWidget->setManual(false);
	m_choosedItem = item;
	ui.treeWidget->emptyCheck(0);
	SString sCode = item->data(1).toString().toStdString();
	SString sSql;
	sSql = SString::toFormat("select fun_key,auth from t_ssp_usergroup_auth join t_ssp_user_group on (t_ssp_usergroup_auth.grp_code = t_ssp_user_group.grp_code) where (t_ssp_user_group.grp_code = '%s')",sCode.data());
	SRecordset rs;
	m_pOper->RetrieveRecordset(sSql,rs);
	for (int i = 0;i<rs.GetRows();i++)
	{
// 		if(i%10==0)
// 			SQt::ProcessUiEvents();
		if (rs.GetValue(i,1)=="1")
		{
			QTreeWidgetItem *rItem  = ui.treeWidget->chooseItem(0,rs.GetValue(i,0).data());
			if(rItem != NULL)
			{
				rItem->setCheckState(1,Qt::Checked);
				rItem->setFlags(rItem->flags() &~Qt::ItemIsUserCheckable&~Qt::ItemIsSelectable);
				rItem->setTextColor(0,Qt::gray);
				rItem->setTextColor(1,Qt::gray);
			}
		}
	}
	sCode = item->data(3).toString().toStdString();
	sSql=SString::toFormat("select fun_key,auth from t_ssp_user_auth join t_ssp_user on (t_ssp_user_auth.usr_sn = t_ssp_user.usr_sn) where (t_ssp_user.usr_sn = '%s')",sCode.data());
	rs.clear();
	m_pOper->RetrieveRecordset(sSql,rs);
	for (int i = 0;i<rs.GetRows();i++)
	{
// 		if(i%10==0)
// 			SQt::ProcessUiEvents();
		if (rs.GetValue(i,1)=="1")
		{
			QTreeWidgetItem *rItem  = ui.treeWidget->chooseItem(0,rs.GetValue(i,0).data());
			if(rItem != NULL)
				rItem->setCheckState(1,Qt::Checked);
		}
	}
	ui.treeWidget->setManual(true);
}
void view_nocommon::OnUpdateCheck_user(QTreeWidgetItem* item,int column)
{
	SString sSql;
	SRecordset rs;
	SString sUsrsn = m_choosedItem->data(3).toString().toStdString();
	SString sFunKey = item->data(0,5).toString().toStdString();
	SString sAuth = item->checkState(column)==Qt::Checked?"1":"0";
	sSql =SString::toFormat("select usr_sn,fun_key,auth from t_ssp_user_auth where usr_sn='%s' and fun_key='%s'",sUsrsn.data(),sFunKey.data());
	if (m_pOper->RetrieveRecordset(sSql,rs)<=0)
	{
		sSql = SString::toFormat("insert into t_ssp_user_auth values('%s','%s',%s)",sUsrsn.data(),sFunKey.data(),sAuth.data());		

	}else
	{
		sSql = SString::toFormat("update t_ssp_user_auth set auth=%s where usr_sn='%s' and fun_key='%s'",sAuth.data(),sUsrsn.data(),sFunKey.data());		
	}


	m_pOper->ExecuteSQL(sSql);
}


void view_nocommon::OnPreShow()
{
	InitWidget(m_funName);

	if (m_funName == "sys.fun.point")
		m_viewNoCommonPlugin->show();
}

void view_nocommon::OnPreHide()
{
	if (m_funName == "sys.fun.point")
		m_viewNoCommonPlugin->hide();
}

void view_nocommon::GetSysFunName(QVector<QString> &funList)
{
	if (!funList.isEmpty())
		funList.clear();

	SString sql = "select fun_key from t_ssp_fun_point where type = 0";
	SRecordset rs;
	m_pOper->RetrieveRecordset(sql, rs);
	for (int row = 0; row < rs.GetRows(); ++row)
	{
		funList.push_back(rs.GetValue(row, 0).data());
	}
}

void view_nocommon::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasText()) {
		const QMimeData *mime = event->mimeData();
		qDebug() << "dragEnterEvent" << mime->text();
		event->accept();
	} else {
		event->ignore();
	}
}

void view_nocommon::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasText()) {
		const QMimeData *mime = event->mimeData();
		qDebug() << "drop data" << mime->text();

		QPoint pos = ui.treeWidget->mapFromGlobal(QCursor::pos());
		QTreeWidgetItem *item = static_cast<QTreeWidgetItem *>(ui.treeWidget->itemAt((const QPoint &)pos));
		if (item && item->parent() == NULL || item->parent()->parent() == NULL || item->parent()->parent()->parent() == NULL)
		{
			event->ignore();
		}
		else if (item)
		{
// 			int row = item->parent()->indexOfChild(item);
// 			QTreeWidgetItem *descItem = item->parent()->child(row, 1);
			QString preName = item->text(0);
			QString preDesc = item->text(1);
			QString preOrder = item->text(2);
			item->setText(0, mime->text());
			item->setText(1, mime->data("plugin/desc"));
			OnUpdateNode(item, preName.toAscii().data(), 0);
			OnUpdateNode(item, preDesc.toAscii().data(), 1);
			if (preOrder.isEmpty())
			{
				int childCount = item->parent()->childCount();
				QTreeWidgetItem *lastItem = item->parent()->child(childCount - 1);
				int order = lastItem->text(2).toInt();
				if (order == 0)
					order = childCount;
				else
					order++;
				item->setText(2, QString::number(order));
				OnUpdateNode(item, preOrder.toAscii().data(), 2);
			}

			event->accept();
		}
	} 
	else
	{
		event->ignore();
	}
}
