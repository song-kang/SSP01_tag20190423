#include "extattributedlg.h"
#include <QComboBox>
#include <QFont>
#include <QColorDialog>


DynamicSSvgEditorWnd::DynamicSSvgEditorWnd(QWidget *parent)
	:QDialog(parent)
{
	m_memwnd=new memSSvgEditorWnd(this);
	m_btn_ok=new QPushButton(this);
	m_btn_ok->setText(tr("保存(&S)"));
	m_btn_cancel=new QPushButton(this);
	m_btn_cancel->setText(tr("取消(&C)"));
	m_gridLayout=new QGridLayout(this);
	m_gridLayout->setSpacing(6);
	m_gridLayout->setContentsMargins(0, 0, 0, -1);
	m_verticalLayout=new QVBoxLayout();
	m_verticalLayout->setSpacing(6);
	m_horizontalLayout=new QHBoxLayout();
	m_horizontalSpacer=new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	m_horizontalLayout->addItem(m_horizontalSpacer);
	m_horizontalLayout->addWidget(m_btn_ok);
	m_horizontalLayout->addWidget(m_btn_cancel);
	m_horizontalLayout->setSpacing(6);
	m_verticalLayout->addWidget(m_memwnd);
	m_verticalLayout->addLayout(m_horizontalLayout);
	m_gridLayout->addLayout(m_verticalLayout,0,0,1,1);
	QObject::connect(m_btn_ok,SIGNAL(pressed()),this,SLOT(OnOkPressed()));
	QObject::connect(m_btn_cancel,SIGNAL(pressed()),this,SLOT(OnCancelPressed()));
	m_save=false;
	//m_memwnd->GetSvgWnd()->SetOffset(0,0);
	//m_memwnd->GetSvgWnd()->RedrawWindow();
	//m_memwnd->LoadLibraryByFile("D:\\softPlat\\trunk\\SSP01\\out_debug\\conf\\sys_svg_library.xml");
	this->setWindowTitle(tr("svg编辑窗口"));
}
DynamicSSvgEditorWnd::~DynamicSSvgEditorWnd()
{

}
void DynamicSSvgEditorWnd::setData(SDatabaseOper * dboper,SString swndref,char * membuf)
{
	m_memwnd->getBbOper(dboper);
	m_memwnd->getWndRef(swndref);
	m_memwnd->LoadSvgByText(membuf);
}
void DynamicSSvgEditorWnd::getSaveBuff(SString & sbuff)
{
	if (m_save)
	m_memwnd->save(sbuff);
}
void DynamicSSvgEditorWnd::OnOkPressed()
{
	m_save=true;
	this->close();
}
void DynamicSSvgEditorWnd::OnCancelPressed()
{
	m_save=false;
	this->close();
}


void memSSvgEditorWnd::save(SString & sbuff)
{
	SSvgEditorWnd::GetSvgWnd()->m_Document.SaveSvgFileToText(sbuff);
}
void memSSvgEditorWnd::slotDatasetCell(const QString &dsname, int row,int col)
{
	unsigned long pos;
	SString sExtAttr,type;
	row++;
	col++;
	SSvgObject *pObj = GetSvgWnd()->m_SelectedObjList.FetchFirst(pos);
	while(pObj)
	{
		sExtAttr = pObj->GetAttribute("ext-attribute");
		type = SString::GetAttributeValue(sExtAttr,"type");
		if(type.length() == 0)
		{
			if(pObj->GetType() == SSvgObject::TYPE_TEXT)
			{
				//type=text;ds=ds_substation;row=1;col=4;flash=0;
				sExtAttr.sprintf("type=text;ds=%s;row=%d;col=%d;flash=0;",dsname.toStdString().data(),row,col);
			}
			else
			{
				//type=state;ds=ds_substation;row=1;col=4;fc=rgb(0,0,0);lc=rgb(0,0,0);flash=0;
				sExtAttr.sprintf("type=state;ds=%s;row=%d;col=%d;fc=rgb(0,0,0);lc=rgb(0,0,0);flash=0;",dsname.toStdString().data(),row,col);
			}
		}
		else if(type == "text" || type == "state")
		{
			SString::SetAttributeValue(sExtAttr,"ds",dsname.toStdString().data());
			SString::SetAttributeValueI(sExtAttr,"row",row);
			SString::SetAttributeValueI(sExtAttr,"col",col);
		}
		pObj->SetAttributeValue("ext-attribute",sExtAttr,false,false);

		//在当前图元外面临时显示一个边框
		SSvgObject obj;
		obj.SetType(SSvgObject::TYPE_RECT);
		obj.SetAttributeValue("x",SString::toString(pObj->GetRect()->left));
		obj.SetAttributeValue("y",SString::toString(pObj->GetRect()->top));
		obj.SetAttributeValue("width",SString::toString(pObj->GetRect()->width()));
		obj.SetAttributeValue("height",SString::toString(pObj->GetRect()->height()));
		obj.SetAttributeValue("z","255");
		obj.SetAttributeValue("stroke","#ff0000");
		obj.SetAttributeValue("stroke-width","2");
		obj.SetAttributeValue("stroke-dasharray","2");
		obj.SetAttributeValue("fill","none");
		obj.CalcObjectRect();

		GetSvgWnd()->SetTempCopyObject(&obj);
		GetSvgWnd()->Render();
		GetSvgWnd()->RedrawWindow();
		SQt::ProcessUiEvents();
		SApi::UsSleep(50000);

		GetSvgWnd()->SetTempCopyObject(NULL);
		GetSvgWnd()->Render();
		GetSvgWnd()->RedrawWindow();
		SQt::ProcessUiEvents();
		SApi::UsSleep(50000);

		GetSvgWnd()->SetTempCopyObject(&obj);
		GetSvgWnd()->Render();
		GetSvgWnd()->RedrawWindow();
		SQt::ProcessUiEvents();
		SApi::UsSleep(50000);		

		GetSvgWnd()->SetTempCopyObject(NULL);
		GetSvgWnd()->Render();
		GetSvgWnd()->RedrawWindow();

		this->GetAttributeWnd()->LoadObject(pObj);
		pObj = GetSvgWnd()->m_SelectedObjList.FetchNext(pos);
	}
}
bool memSSvgEditorWnd::EditExtAttribute(SSvgObject *pObj,SString &sExtAttr)
{
	if(m_pSvgDataWindow == NULL)
	{
		m_pSvgDataWindow = new CDlgSvgDataWindow(this);
		connect(m_pSvgDataWindow,SIGNAL(sigDatasetCell(const QString&,int,int)),this,SLOT(slotDatasetCell(const QString&,int,int)));
	}
	m_pSvgDataWindow->ShowDlg();
	ExtAttributeDlg e(pObj,sExtAttr,m_dboper,m_wndRef);
	e.exec();
	return true;
}
ExtAttributeDlg::ExtAttributeDlg(QWidget *parent)
	: QDialog(parent)
{
	init();
}
ExtAttributeDlg::ExtAttributeDlg(SSvgObject *pObj,SString &sExtAttr,SDatabaseOper *dboper,SString swndRef,QWidget *parent)
	: QDialog(parent)
{
	m_dboper = dboper;
	m_wndRef = swndRef;
	m_svgObject=pObj;
	m_attriStr=sExtAttr;
	m_pattriStr=&sExtAttr;
	init();
}

ExtAttributeDlg::~ExtAttributeDlg()
{
	delete m_delegate;
	delete m_delegate2;
	delete m_delegate3;
}
void ExtAttributeDlg::init()
{
	m_retainElement.clear();
	m_menu=NULL;
	m_citem=NULL;
	m_typeMap.insert("text",tr("文本图元"));
	m_typeMap.insert("state",tr("状态图元"));
	m_actMap.insert("jump_fun",tr("功能跳转点"));
	m_actMap.insert("jump_wnd",tr("组态窗口跳转点"));
	m_actMap.insert("float_wnd",tr("弹出浮动窗口"));
	m_actMap.insert("float_view",tr("弹出视图窗口"));
	m_actMap.insert("general_act",tr("通用动作图元"));
	m_typeMap.insert("chart_pie",tr("饼图图元"));
	m_typeMap.insert("chart_col",tr("柱图图元"));
	m_typeMap.insert("chart_line",tr("折线图图元"));
	m_typeMap.insert("dyn_rs_rect",tr("动态图元"));
	m_typeMap.insert("chart_panel",tr("表盘图元"));
	m_retainElement.append("text");
	m_retainElement.append("state");
	m_retainElement.append("jump_fun");
	m_retainElement.append("jump_wnd");
	m_retainElement.append("chart_pie");
	m_retainElement.append("chart_col");
	m_retainElement.append("chart_line");
	m_retainElement.append("dyn_rs_rect");
	m_retainElement.append("chart_panel");

	m_showMap.insert("type",tr("类型"));
	m_showMap.insert("ds",tr("数据引用"));
	m_showMap.insert("pos",tr("引用行列"));
	m_showMap.insert("dime",tr("单位"));
	m_showMap.insert("row",tr("行"));
	m_showMap.insert("col",tr("列"));
	m_showMap.insert("fc",tr("缺省填充色"));
	m_showMap.insert("lc",tr("缺省线条色"));
	m_showMap.insert("to",tr("动作名称"));
	m_showMap.insert("to1",tr("窗口序号"));
	m_showMap.insert("ds",tr("数据引用"));
	m_showMap.insert("userdraw",tr("是否手绘"));
	m_showMap.insert("cond",tr("条件"));
	m_showMap.insert("tcor",tr("文本颜色"));
	m_showMap.insert("lcor",tr("线条颜色"));
	m_showMap.insert("t",tr("标题"));
	m_showMap.insert("cols",tr("列数"));
	m_showMap.insert("alpha",tr("透明度百分比"));
	m_showMap.insert("act",tr("动作"));
	m_showMap.insert("w",tr("宽"));
	m_showMap.insert("h",tr("高"));
	m_showMap.insert("title",tr("窗口标题"));
	m_showMap.insert("view",tr("视图窗口名称"));

	m_showMap.insert("flash",tr("闪烁"));
	m_showMap.insert("f_ds",tr("闪烁引用"));
	m_showMap.insert("f_row",tr("闪烁行"));
	m_showMap.insert("f_col",tr("闪烁列"));

	//m_showMap.insert("p_row",tr("所在行"));
	//m_showMap.insert("p_col",tr("所在列"));
	m_showMap.insert("p_start",tr("起始值"));
	m_showMap.insert("p_end",tr("结束值"));
	m_showMap.insert("p_cor",tr("仪表盘色"));
	m_showMap.insert("p_corcnt",tr("仪表盘色范围"));
	m_showMap.insert("p_bakgcor",tr("仪表盘背景色"));

	m_retainElement.append("type");
	m_retainElement.append("ds");
	m_retainElement.append("pos");
	m_retainElement.append("dime");
	m_retainElement.append("row");
	m_retainElement.append("col");
	m_retainElement.append("fc");
	m_retainElement.append("lc");
	m_retainElement.append("to");
	m_retainElement.append("to1");
	m_retainElement.append("ds");
	m_retainElement.append("userdraw");
	m_retainElement.append("cond");
	m_retainElement.append("tcor");
	m_retainElement.append("lcor");
	m_retainElement.append("t");
	m_retainElement.append("cols");
	m_retainElement.append("alpha");
	m_retainElement.append("w");
	m_retainElement.append("h");
	m_retainElement.append("act");
	m_retainElement.append("title");
	m_retainElement.append("view");
	m_retainElement.append("flash");
	m_retainElement.append("f_ds");
	m_retainElement.append("f_row");
	m_retainElement.append("f_col");
	//m_retainElement.append("p_row");
	//m_retainElement.append("p_col");
	m_retainElement.append("p_start");
	m_retainElement.append("p_end");
	m_retainElement.append("p_cor");
	m_retainElement.append("p_corcnt");
	m_retainElement.append("p_bakgcor");

	m_nameMap.insert(tr("类型"),"type");
	m_nameMap.insert(tr("数据引用"),"ds");
	m_nameMap.insert(tr("引用行列"),"pos");
	m_nameMap.insert(tr("行"),"row");
	m_nameMap.insert(tr("列"),"col");
	m_nameMap.insert(tr("单位"),"dime");
	m_nameMap.insert(tr("缺省填充色"),"fc");
	m_nameMap.insert(tr("缺省线条色"),"lc");
	m_nameMap.insert(tr("动作名称"),"to");
	m_nameMap.insert(tr("窗口序号"),"to");
	m_nameMap.insert(tr("数据引用"),"ds");
	m_nameMap.insert(tr("是否手绘"),"userdraw");
	m_nameMap.insert(tr("条件"),"cond");
	m_nameMap.insert(tr("文本颜色"),"tcor");
	m_nameMap.insert(tr("线条颜色"),"lcor");
	m_nameMap.insert(tr("标题"),"t");
	m_nameMap.insert(tr("列数"),"cols");
	m_nameMap.insert(tr("透明度百分比"),"alpha");
	m_nameMap.insert(tr("动作"),"act");
	m_nameMap.insert(tr("宽"),"w");
	m_nameMap.insert(tr("高"),"h");
	m_nameMap.insert(tr("窗口标题"),"title");
	m_nameMap.insert(tr("视图窗口名称"),"view");
	m_nameMap.insert(tr("闪烁"),"flash");
	m_nameMap.insert(tr("闪烁引用"),"f_ds");
	m_nameMap.insert(tr("闪烁行"),"f_row");
	m_nameMap.insert(tr("闪烁列"),"f_col");

	//m_nameMap.insert(tr("所在行"),"p_row");
	//m_nameMap.insert(tr("所在列"),"p_col");
	m_nameMap.insert(tr("起始值"),"p_start");
	m_nameMap.insert(tr("结束值"),"p_end");
	m_nameMap.insert(tr("仪表盘色"),"p_cor");
	m_nameMap.insert(tr("仪表盘色范围"),"p_corcnt");
	m_nameMap.insert(tr("仪表盘背景色"),"p_bakgcor");

	this->resize(420, 400);
	m_btn_ok=new QPushButton(this);
	m_btn_ok->setText(tr("确定(&O)"));
	m_btn_cancel=new QPushButton(this);
	m_btn_cancel->setText(tr("取消(&C)"));
	m_tab=new QTabWidget(this);
	m_tree=new QTreeWidget(this);
	m_tree2=new QTreeWidget(this);
	m_tree3=new QTreeWidget(this);
	m_tab->addTab(m_tree,tr("类型"));
	m_tab->addTab(m_tree2,tr("动作"));
	m_tab->addTab(m_tree3,tr("闪烁"));
	m_gridLayout=new QGridLayout(this);
	m_gridLayout->setSpacing(6);
	m_verticalLayout=new QVBoxLayout();
	m_verticalLayout->setSpacing(6);
	m_verticalLayout->addWidget(m_tab);
	m_horizontalLayout=new QHBoxLayout();
	m_horizontalSpacer=new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	m_horizontalLayout->addItem(m_horizontalSpacer);
	m_horizontalLayout->addWidget(m_btn_ok);
	m_horizontalLayout->addWidget(m_btn_cancel);
	m_horizontalLayout->setSpacing(6);
	m_verticalLayout->addLayout(m_horizontalLayout);
	m_gridLayout->addLayout(m_verticalLayout,0,0,1,1);

	QObject::connect(m_btn_ok,SIGNAL(pressed()),this,SLOT(OnOk()));
	QObject::connect(m_btn_cancel,SIGNAL(pressed()),this,SLOT(OnCalcel()));

	initTree1();
	initTree2();
	initTree3();

	this->setWindowTitle(tr("扩展属性编辑窗口"));
}
void ExtAttributeDlg::clearAttri()
{
	//m_tree->setRowCount(1);
	int n=m_tree->topLevelItemCount();
	for (int i=0;i<n;i++)
	{
		m_tree->takeTopLevelItem(n);
	}
}
void ExtAttributeDlg::clearAttri2()
{
	int n=m_tree2->topLevelItemCount();
	for (int i=0;i<n;i++)
	{
		m_tree2->takeTopLevelItem(n);
	}
}
void ExtAttributeDlg::clearAttri3()
{
	int n=m_tree3->topLevelItemCount();
	for (int i=0;i<n;i++)
	{
		m_tree3->takeTopLevelItem(n);
	}
}
void ExtAttributeDlg::initTree1()
{
	m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
	QObject::connect(m_tree,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(OnTreeItemClicked(QTreeWidgetItem*,int)));
	QObject::connect(m_tree,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(OnTreeCustomContextMenuRequested(const QPoint&)));
	QStringList lheader;
	lheader<<tr("属性")<<tr("值")<<tr("操作");
	m_tree->setColumnCount(3);
	m_tree->setHeaderLabels(lheader);
	QTreeWidgetItem *item=new QTreeWidgetItem();
	item->setText(0,m_showMap["type"]);
	QComboBox *cbo=new QComboBox();
	QMapIterator<QString, QString> imap(m_typeMap);
	m_stype= SString::GetAttributeValue(m_attriStr,"type");
	QString str=m_typeMap.value(m_stype.data());
	int cindex=-1;
	int count=0;
	while (imap.hasNext()) 
	{
		QString tmp1=imap.next().value();
		cbo->addItem(tmp1);
		if(tmp1==str)
			cindex=count;
		count++;
	}
	cbo->setCurrentIndex(cindex);
	connect(cbo,SIGNAL(currentIndexChanged(QString)),this,SLOT(onTree1CboChanged(QString)));

	QTreeWidgetItem *item1=new QTreeWidgetItem();
	item1->setText(0,m_showMap["ds"]);
	SRecordset rs;
	QComboBox *cbox=new QComboBox();
	RefData::getRefData(m_dboper,rs,1);
	cindex=-1;
	m_sds= SString::GetAttributeValue(m_attriStr,"ds");
	if (m_sds=="")
	{
		m_sds=m_wndRef;
	}
	for(int i=0;i<rs.GetRows();i++)
	{
		cbox->addItem(rs.GetValue(i,1).data());
		cbox->setItemData(i,QVariant(rs.GetValue(i,0).data()));
		if (m_sds==rs.GetValue(i,0))
		{
			cindex=i;
		}
	}
	if (cindex==-1)
	{
		cbox->setItemData(-1,QVariant(m_sds.data()));
	}
	cbox->setCurrentIndex(cindex);
	m_tree->addTopLevelItem(item);
	m_tree->addTopLevelItem(item1);
	m_tree->setItemWidget(item,1,cbo);
	m_tree->setItemWidget(item1,1,cbox);
	m_tree->setSelectionMode(QAbstractItemView::NoSelection);
	initAttri(m_attriStr,"NULL");
	m_tree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_tree->header()->setMovable(false);
	m_tree->header()->setResizeMode(QHeaderView::Interactive);
	m_tree->header()->resizeSection(0,100);
	m_tree->header()->resizeSection(1,190);
	m_tree->header()->resizeSection(2,m_tree->width()-290);
	QPalette pal;
	pal.setColor(QPalette::Base, QColor(255, 255, 255));
	pal.setColor(QPalette::AlternateBase,QColor(10, 10, 10,40));
	m_tree->setPalette(pal);
	m_tree->setAlternatingRowColors(true);
	m_tree->setStyleSheet("QTreeWidget::item{height:22px;}");
	//delete m_delegate;
	m_delegate=new SpinBoxDelegate;
	m_stddelegate= static_cast<QItemDelegate*>(m_tree->itemDelegate());
}
void ExtAttributeDlg::initTree2()
{
	m_tree2->setContextMenuPolicy(Qt::CustomContextMenu);

	QObject::connect(m_tree2,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(OnTree2ItemClicked(QTreeWidgetItem*,int)));
	QObject::connect(m_tree2,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(OnTree2CustomContextMenuRequested(const QPoint&)));
	QStringList lheader;
	lheader<<tr("属性")<<tr("值")<<tr("操作");
	m_tree2->setColumnCount(3);
	m_tree2->setHeaderLabels(lheader);
	QTreeWidgetItem *item=new QTreeWidgetItem();
	item->setText(0,m_showMap["act"]);
	QComboBox *cbo=new QComboBox(this);
	QMapIterator<QString, QString> imap(m_actMap);
	QString str=m_actMap.value(m_sact.data());
	m_sact= SString::GetAttributeValue(m_attriStr,"act");
	int cindex=-1;
	int count=0;
	while (imap.hasNext()) 
	{
		QString tmp1=imap.next().value();
		cbo->addItem(tmp1);
 		if(tmp1==str)
 			cindex=count;
		count++;
	}
	cbo->setCurrentIndex(cindex);
	connect(cbo,SIGNAL(currentIndexChanged(QString)),this,SLOT(onTree2CboChanged(QString)));
	
	m_tree2->addTopLevelItem(item);
	m_tree2->setItemWidget(item,1,cbo);
	m_tree2->setSelectionMode(QAbstractItemView::NoSelection);
	initAttri2(m_attriStr,"NULL");
	m_tree2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_tree2->header()->setMovable(false);
	m_tree2->header()->setResizeMode(QHeaderView::Interactive);
	m_tree2->header()->resizeSection(0,100);
	m_tree2->header()->resizeSection(1,190);
	m_tree2->header()->resizeSection(2,m_tree2->width()-290);
	QPalette pal;
	pal.setColor(QPalette::Base, QColor(255, 255, 255));
	pal.setColor(QPalette::AlternateBase, QColor(10, 10, 10,40));
	m_tree2->setPalette(pal);
	m_tree2->setAlternatingRowColors(true);
	m_tree2->setStyleSheet("QTreeWidget::item{height:22px;}");

	m_delegate2=new SpinBoxDelegate;
	m_stddelegate2= static_cast<QItemDelegate*>(m_tree2->itemDelegate());
}


void ExtAttributeDlg::initTree3()
{
	m_tree3->setContextMenuPolicy(Qt::CustomContextMenu);
	QObject::connect(m_tree3,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(OnTree3ItemClicked(QTreeWidgetItem*,int)));
	QObject::connect(m_tree3,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(OnTree3CustomContextMenuRequested(const QPoint&)));
	QStringList lheader;
	lheader<<tr("属性")<<tr("值")<<tr("操作");
	m_tree3->setColumnCount(3);
	m_tree3->setHeaderLabels(lheader);
	QTreeWidgetItem *item=new QTreeWidgetItem();
	item->setText(0,m_showMap["flash"]);
	QComboBox *cbo=new QComboBox();
	cbo->addItem(tr("关闭闪烁"));
	cbo->addItem(tr("开启闪烁"));
	m_flash= SString::GetAttributeValue(m_attriStr,"flash");
	QString str=flashIntToStr(m_flash.toInt());

	cbo->setCurrentIndex(m_flash.toInt());
	//connect(cbo,SIGNAL(currentIndexChanged(QString)),this,SLOT(onTree3CboChanged(QString)));

	QTreeWidgetItem *item1=new QTreeWidgetItem();
	item1->setText(0,m_showMap["f_ds"]);
	SRecordset rs;
	QComboBox *cbox=new QComboBox();
	RefData::getRefData(m_dboper,rs,1);	
	int cindex=-1;
	//int count=0;
	m_fds= SString::GetAttributeValue(m_attriStr,"f_ds");
	if (m_fds=="")
	{
		m_fds=m_wndRef;
	}

	for(int i=0;i<rs.GetRows();i++)
	{
		cbox->addItem(rs.GetValue(i,1).data());
		cbox->setItemData(i,QVariant(rs.GetValue(i,0).data()));
		if (m_fds==rs.GetValue(i,0))
		{
			cindex=i;
		}
	}
	if (cindex==-1)
	{
		cbox->setItemData(-1,QVariant(m_fds.data()));
	}
	cbox->setCurrentIndex(cindex);
	m_tree3->addTopLevelItem(item);
	m_tree3->addTopLevelItem(item1);
	m_tree3->setItemWidget(item,1,cbo);
	m_tree3->setItemWidget(item1,1,cbox);
	m_tree3->setSelectionMode(QAbstractItemView::NoSelection);

	QTreeWidgetItem *item2=new QTreeWidgetItem();
	item2->setText(0,m_showMap["f_row"]);
	m_frow= SString::GetAttributeValue(m_attriStr,"f_row");
	item2->setText(1,m_frow.data());
	m_tree3->addTopLevelItem(item2);
	QTreeWidgetItem *item3=new QTreeWidgetItem();
	item3->setText(0,m_showMap["f_col"]);
	m_fcol= SString::GetAttributeValue(m_attriStr,"f_col");
	item3->setText(1,m_fcol.data());
	m_tree3->addTopLevelItem(item3);

	m_tree3->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_tree3->header()->setMovable(false);
	m_tree3->header()->setResizeMode(QHeaderView::Interactive);
	m_tree3->header()->resizeSection(0,100);
	m_tree3->header()->resizeSection(1,190);
	m_tree3->header()->resizeSection(2,m_tree->width()-290);
	QPalette pal;
	pal.setColor(QPalette::Base, QColor(255, 255, 255));
	pal.setColor(QPalette::AlternateBase, QColor(10, 10, 10,40));
	m_tree3->setPalette(pal);
	m_tree3->setAlternatingRowColors(true);
	m_tree3->setStyleSheet("QTreeWidget::item{height:22px;}");
	//delete m_delegate;
	m_delegate3=new SpinBoxDelegate;
	m_stddelegate3= static_cast<QItemDelegate*>(m_tree->itemDelegate());
}
void ExtAttributeDlg::onTree1CboChanged(QString type)
{
	initAttri(m_attriStr,type);
}
void ExtAttributeDlg::onTree2CboChanged(QString act)
{
	initAttri2(m_attriStr,act);
}
void ExtAttributeDlg::initAttri(SString m_attriStr,QString type)
{
	m_tree->blockSignals(true);
	m_stype = SString::GetAttributeValue(m_attriStr,"type");
	m_srow = SString::GetAttributeValue(m_attriStr,"row");
	m_scol = SString::GetAttributeValue(m_attriStr,"col");
	m_sdime = SString::GetAttributeValue(m_attriStr,"dime");
	m_sfc = SString::GetAttributeValue(m_attriStr,"fc");
	m_slc = SString::GetAttributeValue(m_attriStr,"lc");
	m_sto = SString::GetAttributeValue(m_attriStr,"to");
	m_suserdraw = SString::GetAttributeValue(m_attriStr,"userdraw");
	m_scond = SString::GetAttributeValue(m_attriStr,"cond");
	m_stcor = SString::GetAttributeValue(m_attriStr,"tcor");
	m_slcor = SString::GetAttributeValue(m_attriStr,"lcor");
	m_stitle= SString::GetAttributeValue(m_attriStr,"t");
	m_stitle1= SString::GetAttributeValue(m_attriStr,"title");
	m_sview= SString::GetAttributeValue(m_attriStr,"view");
	m_scolcount=SString::GetAttributeValue(m_attriStr,"cols");
	//m_pRow=SString::GetAttributeValue(m_attriStr,"p_row");
	//m_pCol=SString::GetAttributeValue(m_attriStr,"p_col");
	m_pStart=SString::GetAttributeValue(m_attriStr,"p_start");
	m_pEnd=SString::GetAttributeValue(m_attriStr,"p_end");
	m_pCor=SString::GetAttributeValue(m_attriStr,"p_cor");
	m_pbakCor=SString::GetAttributeValue(m_attriStr,"p_bakgcor");
	m_tree->clear();
	if(type!="NULL")
		m_stype=m_typeMap.key(type).toStdString().data();

	QTreeWidgetItem *item=new QTreeWidgetItem();
	item->setText(0,m_showMap["type"]);
	QComboBox *cbo=new QComboBox();
	QMapIterator<QString, QString> imap(m_typeMap);
	QString str1=m_typeMap.value(m_stype.data());
	int cindex=-1;
	int count=1;
	cbo->addItem("");
	while (imap.hasNext()) 
	{
		QString tmp1=imap.next().value();
		cbo->addItem(tmp1);
		if(tmp1==str1)
			cindex=count;
		count++;
	}
	cbo->setCurrentIndex(cindex);
	connect(cbo,SIGNAL(currentIndexChanged(QString)),this,SLOT(onTree1CboChanged(QString)));

	QTreeWidgetItem *item1=new QTreeWidgetItem();
	item1->setText(0,m_showMap["ds"]);
	SRecordset rs;
	QComboBox *cbox=new QComboBox();
	RefData::getRefData(m_dboper,rs,1);
	cindex=-1;
	if (m_sds=="")
	{
		m_sds=m_wndRef;
	}
	for(int i=0;i<rs.GetRows();i++)
	{
		cbox->addItem(rs.GetValue(i,1).data());
		cbox->setItemData(i,QVariant(rs.GetValue(i,0).data()));
		if (m_sds==rs.GetValue(i,0))
		{
			cindex=i;
		}
	}
	if (cindex==-1)
	{
		cbox->setItemData(-1,QVariant(m_sds.data()));
	}
	cbox->setCurrentIndex(cindex);
	m_tree->addTopLevelItem(item);
	m_tree->addTopLevelItem(item1);
	m_tree->setItemWidget(item,1,cbo);
	m_tree->setItemWidget(item1,1,cbox);

	QStringList spList=QString(m_attriStr.data()).split(";");		
	for(int j=0;j<spList.size();j++)
	{
		for(int i=0;i<m_retainElement.size();i++)
		{
			if(spList.at(j).split("=").at(0).contains("lc_"))
			{
				spList.removeOne(spList.at(j));
				j=-1;
				break;
			}
			if(spList.at(j).split("=").at(0).contains("fc_"))
			{
				spList.removeOne(spList.at(j));
				j=-1;
				break;
			}
			if(spList.at(j).split("=").at(0).contains("p_cor_"))
			{
				spList.removeOne(spList.at(j));
				j=-1;
				break;
			}
			if(spList.at(j).split("=").at(0).contains("p_corcnt_"))
			{
				spList.removeOne(spList.at(j));
				j=-1;
				break;
			}
			if(spList.at(j).split("=").at(0)==(m_retainElement.at(i)))
			{
				spList.removeOne(spList.at(j));
				j=-1;
				break;
			}
		}
	}

	QStringList leftList;
	QStringList rightList;
	for(int i=0;i<spList.size();i++)
	{
		QStringList strList=spList.at(i).split("=");
		if(strList.size()!=2)
			continue;
		leftList.append(strList.at(0));
		rightList.append(strList.at(1));
	}

	SString str="";
	for (int i=0;;i++)
	{
		str = SString::GetAttributeValue(m_attriStr,SString::toFormat("fc_%d",i));
		if (str!="")
			m_vfcext.push_back(str);
		else
			break;
	}
	str="";
	for (int i=0;;i++)
	{
		str = SString::GetAttributeValue(m_attriStr,SString::toFormat("lc_%d",i));
		if (str!="")
			m_vlcext.push_back(str);
		else
			break;
	}
	str="";
	for (int i=0;;i++)
	{
		str = SString::GetAttributeValue(m_attriStr,SString::toFormat("p_cor_%d",i));
		if (str!="")
			m_vpcorext.push_back(str);
		else
			break;
	}
	str="";
	for (int i=0;;i++)
	{
		str = SString::GetAttributeValue(m_attriStr,SString::toFormat("p_corcnt_%d",i));
		if (str!="")
			m_vpcorcntext.push_back(str);
		else
			break;
	}
	if (m_stype=="text")
	{
		setAsText();
	}
	if (m_stype=="state")
	{
		setAsState();
	}
	if (m_stype=="chart_pie")
	{
		setAsChartPie();
	}
	if (m_stype=="chart_col")
	{
		setAsChartCol();
	}
	if (m_stype=="chart_line")
	{
		setAsChartPie();
	}

	if (m_stype=="dyn_rs_rect")
	{
		setAsDynRsRect();
	}

	if (m_stype=="chart_panel")
	{
		setAsChartPanel();
	}
	QTreeWidgetItem *expanditem=new QTreeWidgetItem();
	expanditem->setText(0,tr("扩展属性"));
	expanditem->setText(1,tr("<右键添加或删除>"));
	for(int i=0;i<leftList.size();i++)
	{
		if(leftList.at(i)!=""&&rightList.at(i)!="")
		{
			QTreeWidgetItem *item=new QTreeWidgetItem(expanditem);
			item->setText(0,leftList.at(i));
			item->setText(1,rightList.at(i));
			item->setFlags(item->flags()|Qt::ItemIsEditable);
		}
	}

	m_tree->addTopLevelItem(expanditem);
	m_tree->expandAll();
	QObject::connect(m_tree,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(OnTreeItemChanged(QTreeWidgetItem*,int)));
	m_tree->blockSignals(false);
}

void ExtAttributeDlg::initAttri2(SString m_attriStr,QString act)
{
	m_tree2->blockSignals(true);
	m_tree2->clear();

	//m_stype = SString::GetAttributeValue(m_attriStr,"type");
	m_srow = SString::GetAttributeValue(m_attriStr,"row");
	m_scol = SString::GetAttributeValue(m_attriStr,"col");
	m_sdime = SString::GetAttributeValue(m_attriStr,"dime");
	m_sfc = SString::GetAttributeValue(m_attriStr,"fc");
	m_slc = SString::GetAttributeValue(m_attriStr,"lc");
	m_sto = SString::GetAttributeValue(m_attriStr,"to");
	m_suserdraw = SString::GetAttributeValue(m_attriStr,"userdraw");
	m_scond = SString::GetAttributeValue(m_attriStr,"cond");
	m_stcor = SString::GetAttributeValue(m_attriStr,"tcor");
	m_slcor = SString::GetAttributeValue(m_attriStr,"lcor");
	m_stitle= SString::GetAttributeValue(m_attriStr,"t");
	m_stitle1= SString::GetAttributeValue(m_attriStr,"title");
	m_sview= SString::GetAttributeValue(m_attriStr,"view");
	m_scolcount=SString::GetAttributeValue(m_attriStr,"cols");
	m_sact=SString::GetAttributeValue(m_attriStr,"act");
	m_sheight=SString::GetAttributeValue(m_attriStr,"h");
	m_swidth=SString::GetAttributeValue(m_attriStr,"w");
	m_salpha=SString::GetAttributeValue(m_attriStr,"alpha");

	if(act!="NULL")
		m_sact=m_actMap.key(act).toStdString().data();

	QTreeWidgetItem *item=new QTreeWidgetItem();
	item->setText(0,m_showMap["act"]);
	QComboBox *cbo=new QComboBox(this);
	QMapIterator<QString, QString> imap(m_actMap);
	QString str1=m_actMap.value(m_sact.data());
	int cindex=-1;
	int count=1;
	cbo->addItem("");
	while (imap.hasNext()) 
	{
		QString tmp1=imap.next().value();
		cbo->addItem(tmp1);
		if(tmp1==str1)
			cindex=count;
		count++;
	}
	cbo->setCurrentIndex(cindex);
	connect(cbo,SIGNAL(currentIndexChanged(QString)),this,SLOT(onTree2CboChanged(QString)));

	m_tree2->addTopLevelItem(item);
	m_tree2->setItemWidget(item,1,cbo);



	if (m_sact=="jump_fun")
	{
		setAsJumpFun();
	}
	else if (m_sact=="jump_wnd")
	{
		setAsJumpWnd();
	}
	else if (m_sact=="float_wnd")
	{
		setAsFloatWnd();
	}
	else if(m_sact == "float_view")
	{
		setAsFloatView();
	}
	else if (m_sact=="general_act")
	{
		setAsGenAction();
	}
	m_tree2->expandAll();
	QObject::connect(m_tree2,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(OnTreeItemChanged(QTreeWidgetItem*,int)));
	m_tree2->blockSignals(false);
}
void ExtAttributeDlg::setAsText()
{
	clearAttri();
	QFont fot;
	fot.setItalic(true);
	fot.setUnderline(true);
	QTreeWidgetItem *item2=new QTreeWidgetItem();
	item2->setText(0,m_showMap["pos"]);
	item2->setText(1,(m_srow+","+m_scol).data());
	item2->setText(2,tr("选择..."));
	item2->setFont(2,fot);
	item2->setTextColor(2,Qt::blue);
	QTreeWidgetItem *item2_1=new QTreeWidgetItem(item2);
	item2_1->setText(0,m_showMap["row"]);
	item2_1->setText(1,m_srow.data());
	QTreeWidgetItem *item2_2=new QTreeWidgetItem(item2);
	item2_2->setText(0,m_showMap["col"]);
	item2_2->setText(1,m_scol.data());
	QTreeWidgetItem * item3=new QTreeWidgetItem();
	item3->setText(0,m_showMap["dime"]);
	item3->setText(1,m_sdime.data());
	m_tree->addTopLevelItem(item2);
	m_tree->addTopLevelItem(item3);
}
void ExtAttributeDlg::setAsState()
{
	clearAttri();
	QFont fot;
	fot.setItalic(true);
	fot.setUnderline(true);
	QTreeWidgetItem *item2=new QTreeWidgetItem();
	item2->setText(0,m_showMap["pos"]);
	item2->setText(1,(m_srow+","+m_scol).data());
	item2->setText(2,tr("选择..."));
	item2->setFont(2,fot);
	item2->setTextColor(2,Qt::blue);
	QTreeWidgetItem *item2_1=new QTreeWidgetItem(item2);
	item2_1->setText(0,m_showMap["row"]);
	item2_1->setText(1,m_srow.data());
	QTreeWidgetItem *item2_2=new QTreeWidgetItem(item2);
	item2_2->setText(0,m_showMap["col"]);
	item2_2->setText(1,m_scol.data());
	QTreeWidgetItem * item4=new QTreeWidgetItem();
	item4->setText(0,m_showMap["fc"]);
	item4->setText(1,QcorToAttri(SrgbToAttri(m_sfc)));
	item4->setBackgroundColor(1,SrgbToAttri(m_sfc));
	item4->setText(2,tr("设置..."));
	item4->setFont(2,fot);
	item4->setTextColor(2,Qt::blue);
	for (int i=0;i<m_vfcext.size();i++)
	{
		QTreeWidgetItem * fcextitem=new QTreeWidgetItem(item4);
		QString pstr=item4->text(0);
		fcextitem->setText(0,pstr.right(pstr.size()-tr("缺省").size())+"_"+QString("%1").arg(i));
		fcextitem->setText(1,QcorToAttri(SrgbToAttri(m_vfcext[i])));
		fcextitem->setBackgroundColor(1,SrgbToAttri(m_vfcext[i]));
		fcextitem->setText(2,tr("设置..."));
		fcextitem->setFont(2,fot);
		fcextitem->setTextColor(2,Qt::blue);
	}
	QTreeWidgetItem * item5=new QTreeWidgetItem();
	item5->setText(0,m_showMap["lc"]);
	item5->setText(1,QcorToAttri(SrgbToAttri(m_slc)));
	item5->setBackgroundColor(1,SrgbToAttri(m_slc));
	item5->setText(2,tr("设置..."));
	item5->setFont(2,fot);
	item5->setTextColor(2,Qt::blue);

	for (int i=0;i<m_vlcext.size();i++)
	{
		QTreeWidgetItem * lcextitem=new QTreeWidgetItem(item5);
		QString pstr=item5->text(0);
		lcextitem->setText(0,pstr.right(pstr.size()-tr("缺省").size())+"_"+QString("%1").arg(i));
		lcextitem->setText(1,QcorToAttri(SrgbToAttri(m_vlcext[i])));
		lcextitem->setBackgroundColor(1,SrgbToAttri(m_vlcext[i]));
		lcextitem->setText(2,tr("设置..."));
		lcextitem->setFont(2,fot);
		lcextitem->setTextColor(2,Qt::blue);

	}
	m_tree->addTopLevelItem(item2);
	m_tree->addTopLevelItem(item4);
	m_tree->addTopLevelItem(item5);
}
void ExtAttributeDlg::setAsJumpFun()
{
	clearAttri2();
	//m_tree2->takeTopLevelItem(1);
	QTreeWidgetItem * item6=new QTreeWidgetItem();
	SRecordset rs;
	RefData::getFunPoint(m_dboper,rs);
	QComboBox *cbox=new QComboBox();
	int cindex=-1;
	for (int i=0;i<rs.GetRows();i++)
	{
		cbox->addItem(rs.GetValue(i,1).data());
		cbox->setItemData(i,QVariant(rs.GetValue(i,0).data()));
		if (rs.GetValue(i,0)==m_sto)
		{
			cindex=i;
		}
	}
		cbox->setCurrentIndex(cindex);
	item6->setText(0,m_showMap["to"]);
	m_tree2->addTopLevelItem(item6);
	m_tree2->setItemWidget(item6,1,cbox);
}
void ExtAttributeDlg::setAsJumpWnd()
{
	clearAttri2();
	QTreeWidgetItem * item6=new QTreeWidgetItem();
	SRecordset rs;
	RefData::getCfgWndNo(m_dboper,rs);
	QComboBox *cbox=new QComboBox();
	int cindex=-1;
	for (int i=0;i<rs.GetRows();i++)
	{
		cbox->addItem(rs.GetValue(i,1).data());
		cbox->setItemData(i,QVariant(rs.GetValue(i,0).data()));
		if (rs.GetValue(i,0)==m_sto)
		{
			cindex=i;
		}
	}
	cbox->setCurrentIndex(cindex);
	item6->setText(0,m_showMap["to"]);
	m_tree2->addTopLevelItem(item6);
	m_tree2->setItemWidget(item6,1,cbox);
}

void ExtAttributeDlg::setAsGenAction()
{
	clearAttri2();
	//m_tree2->takeTopLevelItem(1);
	QTreeWidgetItem * item6=new QTreeWidgetItem();
	SRecordset rs;
	RefData::getGenAction(m_dboper,rs);
	QComboBox *cbox=new QComboBox();
	int cindex=-1;
	for (int i=0;i<rs.GetRows();i++)
	{
		cbox->addItem(rs.GetValue(i,1).data());
		cbox->setItemData(i,QVariant(rs.GetValue(i,0).data()));
		if (rs.GetValue(i,0)==m_sto)
		{
			cindex=i;
		}
	}
	cbox->setCurrentIndex(cindex);
	item6->setText(0,m_showMap["to"]);
	m_tree2->addTopLevelItem(item6);
	m_tree2->setItemWidget(item6,1,cbox);
}

void ExtAttributeDlg::setAsFloatWnd()
{
	clearAttri2();
	QTreeWidgetItem * item6=new QTreeWidgetItem();
	QTreeWidgetItem * item7=new QTreeWidgetItem();
	QTreeWidgetItem * item8=new QTreeWidgetItem();
	QTreeWidgetItem * item9=new QTreeWidgetItem();
	QTreeWidgetItem * item10=new QTreeWidgetItem();
	SRecordset rs;
	RefData::getCfgWndNo(m_dboper,rs);
	QComboBox *cbox=new QComboBox();
	int cindex=-1;
	for (int i=0;i<rs.GetRows();i++)
	{
		cbox->addItem(rs.GetValue(i,1).data());
		cbox->setItemData(i,QVariant(rs.GetValue(i,0).data()));
		if (rs.GetValue(i,0)==m_sto)
		{
			cindex=i;
		}
	}
	cbox->setCurrentIndex(cindex);
	item6->setText(0,m_showMap["to"]);
	item7->setText(0,m_showMap["title"]);
	item7->setText(1,m_stitle1.data());
	item7->setFlags(item7->flags()|Qt::ItemIsEditable);
	item8->setText(0,m_showMap["h"]);
	item8->setText(1,m_sheight.data());
	item8->setFlags(item8->flags()|Qt::ItemIsEditable);
	item9->setText(0,m_showMap["w"]);
	item9->setText(1,m_swidth.data());
	item9->setFlags(item9->flags()|Qt::ItemIsEditable);
	item10->setText(0,m_showMap["alpha"]);
	item10->setText(1,m_salpha.data());
	item10->setFlags(item10->flags()|Qt::ItemIsEditable);
	m_tree2->addTopLevelItem(item6);
	m_tree2->setItemWidget(item6,1,cbox);
	m_tree2->addTopLevelItem(item7);
	m_tree2->addTopLevelItem(item8);
	m_tree2->addTopLevelItem(item9);
	m_tree2->addTopLevelItem(item10);
}

void ExtAttributeDlg::setAsFloatView()
{
	clearAttri2();
	QTreeWidgetItem * item6=new QTreeWidgetItem();
	QTreeWidgetItem * item7=new QTreeWidgetItem();
	QTreeWidgetItem * item8=new QTreeWidgetItem();
	QTreeWidgetItem * item9=new QTreeWidgetItem();
	QTreeWidgetItem * item10=new QTreeWidgetItem();
	
	item6->setText(0,m_showMap["view"]);
	item6->setText(1,m_sview.data());
	item7->setText(0,m_showMap["title"]);
	item7->setText(1,m_stitle1.data());
	item6->setFlags(item6->flags()|Qt::ItemIsEditable);
	item7->setFlags(item7->flags()|Qt::ItemIsEditable);
	item8->setText(0,m_showMap["h"]);
	item8->setText(1,m_sheight.data());
	item8->setFlags(item8->flags()|Qt::ItemIsEditable);
	item9->setText(0,m_showMap["w"]);
	item9->setText(1,m_swidth.data());
	item9->setFlags(item9->flags()|Qt::ItemIsEditable);
	item10->setText(0,m_showMap["alpha"]);
	item10->setText(1,m_salpha.data());
	item10->setFlags(item10->flags()|Qt::ItemIsEditable);
	m_tree2->addTopLevelItem(item6);
	m_tree2->addTopLevelItem(item7);
	m_tree2->addTopLevelItem(item8);
	m_tree2->addTopLevelItem(item9);
	m_tree2->addTopLevelItem(item10);
}
void ExtAttributeDlg::setAsChartPie()
{
	clearAttri();
	//m_tree->takeTopLevelItem(1);
	QFont fot;
	fot.setItalic(true);
	fot.setUnderline(true);
	QTreeWidgetItem * item7=new QTreeWidgetItem();
	item7->setText(0,m_showMap["userdraw"]);
	if (m_suserdraw=="false")
	{
		item7->setCheckState(1,Qt::Unchecked);
	}else
	{
		item7->setCheckState(1,Qt::Checked);
	}
	item7->setDisabled(true);
	QTreeWidgetItem * item8=new QTreeWidgetItem();
	item8->setText(0,m_showMap["cond"]);
	QTreeWidgetItem * item9=new QTreeWidgetItem();
	item9->setText(0,m_showMap["tcor"]);
	item9->setText(1,QcorToAttri(ScorToQcor(m_stcor)));
	//item9->setBackgroundColor(1,ScorToQcor(m_stcor));
	item9->setTextColor(1,ScorToQcor(m_stcor));
	item9->setText(2,tr("设置..."));
	item9->setFont(2,fot);
	item9->setTextColor(2,Qt::blue);

	QTreeWidgetItem * item10=new QTreeWidgetItem();
	item10->setText(0,m_showMap["lcor"]);
	item10->setText(1,QcorToAttri(ScorToQcor(m_slcor)));
	item10->setBackgroundColor(1,ScorToQcor(m_slcor));
	item10->setText(2,tr("设置..."));
	item10->setFont(2,fot);
	item10->setTextColor(2,Qt::blue);

	QTreeWidgetItem * item11=new QTreeWidgetItem();
	item11->setText(0,m_showMap["t"]);
	item11->setText(1,m_stitle.data());

	m_tree->addTopLevelItem(item7);
	m_tree->addTopLevelItem(item8);
	m_tree->addTopLevelItem(item9);
	m_tree->addTopLevelItem(item10);
	m_tree->addTopLevelItem(item11);
}
void ExtAttributeDlg::setAsChartCol()
{
	setAsChartPie();
}
void ExtAttributeDlg::setAsChartLine()
{
	setAsChartPie();
}
void ExtAttributeDlg::setAsChartPanel()
{
	clearAttri();
	QFont fot;
	fot.setItalic(true);
	fot.setUnderline(true);
	QTreeWidgetItem * item1=new QTreeWidgetItem();
	item1->setText(0,m_showMap["userdraw"]);
	if (m_suserdraw=="false")
	{
		item1->setCheckState(1,Qt::Unchecked);
	}else
	{
		item1->setCheckState(1,Qt::Checked);
	}
	item1->setDisabled(true);
	QTreeWidgetItem *item2=new QTreeWidgetItem();
	item2->setText(0,m_showMap["pos"]);
	item2->setText(1,(m_srow+","+m_scol).data());
	item2->setText(2,tr("选择..."));
	item2->setFont(2,fot);
	item2->setTextColor(2,Qt::blue);
	QTreeWidgetItem *item2_1=new QTreeWidgetItem(item2);
	item2_1->setText(0,m_showMap["row"]);
	item2_1->setText(1,m_srow.data());
	QTreeWidgetItem *item2_2=new QTreeWidgetItem(item2);
	item2_2->setText(0,m_showMap["col"]);
	item2_2->setText(1,m_scol.data());
	QTreeWidgetItem * item3=new QTreeWidgetItem();
	item3->setText(0,m_showMap["p_start"]);
	item3->setText(1,m_pStart.data());
	QTreeWidgetItem * item4=new QTreeWidgetItem();
	item4->setText(0,m_showMap["p_end"]);
	item4->setText(1,m_pEnd.data());
	QTreeWidgetItem * item5=new QTreeWidgetItem();
	item5->setText(0,m_showMap["p_cor"]);
	item5->setText(1,QcorToAttri(SrgbToAttri(m_pCor)));
	item5->setBackgroundColor(1,SrgbToAttri(m_pCor));
	item5->setText(2,tr("设置..."));
	item5->setFont(2,fot);
	item5->setTextColor(2,Qt::blue);
	for (int i=0;i<m_vpcorcntext.size();i++)
	{
		QTreeWidgetItem * pcorextitem=new QTreeWidgetItem(item5);
		QString pstr=item5->text(0);
		pcorextitem->setText(0,pstr.right(pstr.size()-tr("缺省").size())+"_"+QString("%1").arg(i));
		pcorextitem->setText(1,QcorToAttri(SrgbToAttri(m_vpcorext[i])));
		pcorextitem->setBackgroundColor(1,SrgbToAttri(m_vpcorext[i]));
		pcorextitem->setText(2,tr("设置..."));
		pcorextitem->setFont(2,fot);
		pcorextitem->setTextColor(2,Qt::blue);
	}
	QTreeWidgetItem * item6=new QTreeWidgetItem();
	item6->setText(0,m_showMap["p_corcnt"]);
	for (int i=0;i<m_vpcorcntext.size();i++)
	{
		SString rowcol=m_vpcorcntext.at(i);
		QTreeWidgetItem *item6_1=new QTreeWidgetItem(item6);
		QString pstr=item6->text(0);
		item6_1->setText(0,pstr+"_"+QString("%1").arg(i));
		item6_1->setText(1,rowcol.data());
	}
	QTreeWidgetItem * item7=new QTreeWidgetItem();
	item7->setText(0,m_showMap["p_bakgcor"]);
	item7->setText(1,QcorToAttri(SrgbToAttri(m_pbakCor)));
	item7->setBackgroundColor(1,SrgbToAttri(m_pbakCor));
	item7->setText(2,tr("设置..."));
	item7->setFont(2,fot);
	item7->setTextColor(2,Qt::blue);
	QTreeWidgetItem * item8=new QTreeWidgetItem();
	item8->setText(0,m_showMap["dime"]);
	item8->setText(1,m_sdime.data());
	m_tree->addTopLevelItem(item1);
	m_tree->addTopLevelItem(item2);
	m_tree->addTopLevelItem(item3);
	m_tree->addTopLevelItem(item4);
	m_tree->addTopLevelItem(item5);
	m_tree->addTopLevelItem(item6);
	m_tree->addTopLevelItem(item7);
	m_tree->addTopLevelItem(item8);
}
void ExtAttributeDlg::setAsDynRsRect()
{
	clearAttri();
	QTreeWidgetItem *item=new QTreeWidgetItem();
	item->setText(0,m_showMap["cols"]);
	QLineEdit *lineedit=new QLineEdit(m_scolcount.data());
	//item->setText(1,m_scolcount.data());
	//item->setFlags(item->flags()|Qt::ItemIsEditable);
	m_tree->addTopLevelItem(item);
	m_tree->setItemWidget(item,1,lineedit);
}
SString ExtAttributeDlg::QcorToSrgb(QColor cor)
{
	return SString::toFormat("rgb(%d,%d,%d)",cor.red(),cor.green(),cor.blue());
}
QColor ExtAttributeDlg::SrgbToAttri(SString srgb)
{
	if(srgb.length()<4)
		return qRgb(0,0,0);
	srgb = srgb.right(srgb.length()-4);
	srgb = srgb.left(srgb.length()-1);
	return qRgb(SString::GetIdAttribute(1,srgb,",").toInt(),SString::GetIdAttribute(2,srgb,",").toInt(),SString::GetIdAttribute(3,srgb,",").toInt());
}
SString ExtAttributeDlg::QcorToScor(QColor cor)
{
	return SString::toFormat("%d,%d,%d",cor.red(),cor.green(),cor.blue());
}
QColor ExtAttributeDlg::ScorToQcor(SString srgb)
{
	return qRgb(SString::GetIdAttribute(1,srgb,",").toInt(),SString::GetIdAttribute(2,srgb,",").toInt(),SString::GetIdAttribute(3,srgb,",").toInt());
}
QColor ExtAttributeDlg::AttriToQcor(QString sattri)
{
	sattri.replace("#","0x");
	return QRgb(sattri.toUInt(0,16));
}
QString ExtAttributeDlg::QcorToAttri(QColor cor)
{
	return QVariant(cor).toString();
}

SString ExtAttributeDlg::getAttriStr()
{
	QString sRet;
	QTreeWidgetItem * item;
	QString str,str1,svalue;
	for (int i=0;i<m_tree->topLevelItemCount();i++)
	{
		item=m_tree->topLevelItem(i);
		str=item->text(0);
		str1=m_nameMap[str];
		if (str1=="type"&&m_stype!="")
		{
			sRet+=str1+("="+m_stype+";").data();
			continue;
		}
		if ((str1=="ds"&&m_stype!=""))
		{
			int cindex=((QComboBox*)m_tree->itemWidget(item,1))->currentIndex();
			svalue=((QComboBox*)m_tree->itemWidget(item,1))->itemData(cindex).toString();
			if (svalue=="")
			{
// 				if (str1=="ds")
// 				{
// 					svalue=m_sds;
// 				}
			}
			if (svalue!="")
			{
				sRet+=str1+"="+svalue+";";
			}
			continue;
		}
		if (str1=="pos")
		{
			sRet+=m_nameMap[item->child(0)->text(0)]+"="+QString::number(item->child(0)->text(1).toInt())+";";
			sRet+=m_nameMap[item->child(1)->text(0)]+"="+QString::number(item->child(1)->text(1).toInt())+";";
			continue;
		}
		if (str1=="p_corcnt")
		{
			for (int i=0;i<item->childCount();i++)
			{
				sRet+=str1+QString("_%1").arg(i)+"="+item->child(i)->text(1)+";";
			}
			continue;
		}
		if (str1=="fc"||str1=="lc"||str1=="p_cor"||str1=="p_bakgcor")
		{
			sRet+=str1+"="+QcorToSrgb(AttriToQcor(item->text(1)))+";";
			for (int i=0;i<item->childCount();i++)
			{
				sRet+=str1+QString("_%1").arg(i)+"="+QcorToSrgb(AttriToQcor(item->child(i)->text(1)))+";";
			}
			continue;
		}
		if (str1=="userdraw")
		{
			sRet+=str1+"="+ ((item->checkState(1)==Qt::Checked)?"true;":"false;");
			continue;
		}
		if (str1=="tcor"||str1=="lcor")
		{
			sRet+=str1+"="+QcorToScor(AttriToQcor(item->text(1)))+";";
			continue;
		}
		if(item->text(1)!=""&&item->text(1)!=tr("<右键添加或删除>"))
		{
			sRet+=str1+"="+item->text(1)+";";
		}
		if (str1=="cols")
		{
			sRet+=str1+"="+QString::number(((QLineEdit*)m_tree->itemWidget(item,1))->text().toInt())+";";
		}
		if(str==tr("扩展属性"))
		{
			for(int i=0;i<item->childCount();i++)
			{
				if(item->child(i)->text(0)!=""&&item->child(i)->text(1)!="")
					sRet+=item->child(i)->text(0)+"="+item->child(i)->text(1)+";";
			}
		}
	}

	for (int i=0;i<m_tree2->topLevelItemCount();i++)
	{
		item=m_tree2->topLevelItem(i);
		str=item->text(0);
		str1=m_nameMap[str];
		if (str1=="act"&&m_sact!="")
		{
			sRet+=str1+("="+m_sact+";").data();
			continue;
		}
		if (str1=="to"&&m_sact!="")
		{
			int cindex=((QComboBox*)m_tree2->itemWidget(item,1))->currentIndex();
			svalue=((QComboBox*)m_tree2->itemWidget(item,1))->itemData(cindex).toString();
// 			if (svalue=="")
// 			{
// 				svalue=m_sto;
// 			}
			if (svalue!="")
			{
				sRet+=str1+"="+svalue+";";
			}
			continue;
		}
		if (item->text(1)!=""&&item->text(1)!=tr("<右键添加或删除>"))
		{
			sRet+=str1+"="+item->text(1)+";";
		}
	}

	for (int i=0;i<m_tree3->topLevelItemCount();i++)
	{
		item=m_tree3->topLevelItem(i);
		str=item->text(0);
		str1=m_nameMap[str];
		if (str1=="flash")
		{
			int cindex=((QComboBox*)m_tree3->itemWidget(item,1))->currentIndex();
			sRet+=str1+"="+QString::number(cindex)+";";
			continue;
		}
		if (str1=="f_ds"&&m_fds!="")
		{
			int cindex=((QComboBox*)m_tree3->itemWidget(item,1))->currentIndex();
			svalue=((QComboBox*)m_tree3->itemWidget(item,1))->itemData(cindex).toString();
			if (svalue!="")
			{
				sRet+=str1+"="+svalue+";";
			}
			continue;
		}
		if (item->text(1)!=""&&item->text(1)!=tr("<右键添加或删除>"))
		{
			sRet+=str1+"="+item->text(1)+";";
		}
	}
	return sRet.toStdString().data();
}

void ExtAttributeDlg::OnOk()
{
	SString s = getAttriStr();
	*m_pattriStr=s;
	this->done(1);
}
void ExtAttributeDlg::OnCalcel()
{
	this->done(0);
}
void ExtAttributeDlg::OnChangeMap(int index)
{
	switch(index)
	{
	case 0:
		setAsText();
		break;
	case 1:
		setAsState();
		break;
	case 2:
		setAsJumpFun();
		break;
	case 3:
		setAsJumpWnd();
		break;
	case 4:
		setAsChartPie();
		break;
	case 5:
		setAsChartCol();
		break;
	case 6:
		setAsChartLine();
		break;
	}
}
void ExtAttributeDlg::OnTreeItemClicked(QTreeWidgetItem* item,int column)
{
	m_citem=item;
	if (column==2&&item->text(2)==tr("设置..."))
	{
		QColor initcor=AttriToQcor(item->text(1));
		QColor color =  QColorDialog::getColor(initcor.isValid()?initcor:qRgb(255,255,255));
		if (color.isValid())
		{
			item->setText(1,QcorToAttri(color));

			if (item->text(0)==m_showMap["tcor"])
			{
				item->setTextColor(1,color);
			}else
			{
				item->setBackgroundColor(1,color);
			}
		}
		return;
	}
	if (column==2&&item->text(2)==tr("选择..."))
	{
		m_choosedlg = new QDialog(this);
		QGridLayout *lyt=new QGridLayout(m_choosedlg);
		QTableWidget *table=new QTableWidget();
		lyt->addWidget(table);
		m_choosedlg->resize(800,600);
		SRecordset rs;
		SString sds="";
		int cindex=((QComboBox*)m_tree->itemWidget(m_tree->topLevelItem(1),1))->currentIndex();
		sds=((QComboBox*)m_tree->itemWidget(m_tree->topLevelItem(1),1))->itemData(cindex).toString().toStdString().data();
		if (sds=="")
		{
			SQt::ShowErrorBox(tr("提示"),tr("请先选择有效数据引用!"),this);
			return;
		}
		RefData::getPosData(sds,rs);
		if (rs.GetRows()==0)
		{
			return;
		}
		table->setRowCount(rs.GetRows());
		table->setColumnCount(rs.GetColumns());
		QStringList ls;
		for (int i=0;i<rs.GetColumns();i++)
		{
			ls<<rs.GetColumnName(i).data();
		}
		table->setHorizontalHeaderLabels(ls);
		for(int i=0;i<rs.GetRows();i++)
		{
			for (int j=0;j<rs.GetColumns();j++)
			{
				table->setItem(i,j,new QTableWidgetItem(rs.GetValue(i,j).data()));
			}
		}

		QObject::connect(table,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(OnChoosed(QTableWidgetItem*)));
		m_choosedlg->setWindowFlags(m_choosedlg->windowFlags()|Qt::WindowMaximizeButtonHint);
		m_choosedlg->setWindowTitle(tr("行列选择"));
		m_choosedlg->exec();
		return;
	}
	QString str=item->text(0);
	if ((str==m_showMap["dime"]||str==m_showMap["cond"]||str==m_showMap["t"]||str==m_showMap["p_start"]||str==m_showMap["p_end"])&&column==1)
	{
		m_tree->setItemDelegateForColumn(1,m_stddelegate);
		item->setFlags(item->flags()|Qt::ItemIsEditable);
		m_tree->editItem(item,1);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
	}
	if ((str==m_showMap["row"]||str==m_showMap["col"]||str.contains("仪表盘色范围_"))&&column==1)
	{
		m_tree->setItemDelegateForColumn(1,m_delegate);
		item->setFlags(item->flags()|Qt::ItemIsEditable);
		m_tree->editItem(item,1);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
	}
}

void ExtAttributeDlg::OnTree2ItemClicked(QTreeWidgetItem* item,int column)
{
	m_citem=item;
	if (column==2&&item->text(2)==tr("设置..."))
	{
		QColor initcor=AttriToQcor(item->text(1));
		QColor color =  QColorDialog::getColor(initcor.isValid()?initcor:qRgb(255,255,255));
		if (color.isValid())
		{
			item->setText(1,QcorToAttri(color));

			if (item->text(0)==m_showMap["tcor"])
			{
				item->setTextColor(1,color);
			}else
			{
				item->setBackgroundColor(1,color);
			}
		}
		return;
	}
	if (column==2&&item->text(2)==tr("选择..."))
	{
		m_choosedlg = new QDialog(this);
		QGridLayout *lyt=new QGridLayout(m_choosedlg);
		QTableWidget *table=new QTableWidget();
		lyt->addWidget(table);
		m_choosedlg->resize(800,600);
		SRecordset rs;
		SString sds="";
		int cindex=((QComboBox*)m_tree2->itemWidget(m_tree2->topLevelItem(1),1))->currentIndex();
		sds=((QComboBox*)m_tree2->itemWidget(m_tree2->topLevelItem(1),1))->itemData(cindex).toString().toStdString().data();
		if (sds=="")
		{
			SQt::ShowErrorBox(tr("提示"),tr("请先选择有效数据引用!"),this);
			return;
		}
		RefData::getPosData(sds,rs);
		if (rs.GetRows()==0)
		{
			return;
		}
		table->setRowCount(rs.GetRows());
		table->setColumnCount(rs.GetColumns());
		QStringList ls;
		for (int i=0;i<rs.GetColumns();i++)
		{
			ls<<rs.GetColumnName(i).data();
		}
		table->setHorizontalHeaderLabels(ls);
		for(int i=0;i<rs.GetRows();i++)
		{
			for (int j=0;j<rs.GetColumns();j++)
			{
				table->setItem(i,j,new QTableWidgetItem(rs.GetValue(i,j).data()));
			}
		}

		QObject::connect(table,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(OnChoosed(QTableWidgetItem*)));
		m_choosedlg->setWindowFlags(m_choosedlg->windowFlags()|Qt::WindowMaximizeButtonHint);
		m_choosedlg->setWindowTitle(tr("行列选择"));
		m_choosedlg->exec();
		return;
	}
	QString str=item->text(0);
	if ((str==m_showMap["h"]||str==m_showMap["w"]||str==m_showMap["view"]||str==m_showMap["title"]||str==m_showMap["alpha"])&&column==1)
	{
		m_tree2->setItemDelegateForColumn(1,m_stddelegate2);
		item->setFlags(item->flags()|Qt::ItemIsEditable);
		m_tree2->editItem(item,1);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
	}
	if ((str==m_showMap["row"]||str==m_showMap["col"])&&column==1)
	{
		m_tree2->setItemDelegateForColumn(1,m_delegate2);
		item->setFlags(item->flags()|Qt::ItemIsEditable);
		m_tree2->editItem(item,1);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
	}
}
void ExtAttributeDlg::OnTree3ItemClicked(QTreeWidgetItem* item,int column)
{
	QString str=item->text(0);
	if ((str==m_showMap["f_row"]||str==m_showMap["f_col"])&&column==1)
	{
		m_tree3->setItemDelegateForColumn(1,m_stddelegate3);
		item->setFlags(item->flags()|Qt::ItemIsEditable);
		m_tree3->editItem(item,1);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
	}

}
void ExtAttributeDlg::OnTreeCustomContextMenuRequested(const QPoint&pos)
{
	QTreeWidgetItem * item = m_tree->itemAt(pos);
	if (item==NULL)
		return;
	m_citem=item;
	QString str = item->text(0);
	if (m_nameMap[str]=="fc"||m_nameMap[str]=="lc"||m_nameMap[str]=="p_cor")
	{
		if (m_menu==NULL)
		{
			m_menu=new QMenu(this);
			m_menu->addAction(tr("添加"));
			QObject::connect(m_menu,SIGNAL(triggered(QAction*)),this,SLOT(OnMenuTriggered(QAction*)));
		}else
		{
			m_menu->clear();
			m_menu->addAction(tr("添加"));
		}
		m_menu->exec(QCursor::pos());
	}
	if(m_nameMap[str]=="p_corcnt")
	{
		if (m_menu==NULL)
		{
			m_menu=new QMenu(this);
			m_menu->addAction(tr("添加范围"));
			QObject::connect(m_menu,SIGNAL(triggered(QAction*)),this,SLOT(OnMenuTriggered(QAction*)));
		}else
		{
			m_menu->clear();
			m_menu->addAction(tr("添加范围"));
		}
		m_menu->exec(QCursor::pos());
	}
	if(str==tr("扩展属性"))
	{
		if (m_menu==NULL)
		{
			m_menu=new QMenu(this);
			m_menu->addAction(tr("添加属性"));
			QObject::connect(m_menu,SIGNAL(triggered(QAction*)),this,SLOT(OnMenuTriggered(QAction*)));
		}else
		{
			m_menu->clear();
			m_menu->addAction(tr("添加属性"));
		}
		m_menu->exec(QCursor::pos());
	}
	QTreeWidgetItem *pitem=m_citem->parent();
	if (pitem!=NULL)
	{
		str=pitem->text(0);
		if (m_nameMap[str]=="fc"||m_nameMap[str]=="lc"||m_nameMap[str]=="p_cor")
		{
			if (m_citem->parent()->indexOfChild(m_citem)==m_citem->parent()->childCount()-1)
			{

				if (m_menu==NULL)
				{
					m_menu=new QMenu(this);
					m_menu->addAction(tr("删除"));
					QObject::connect(m_menu,SIGNAL(triggered(QAction*)),this,SLOT(OnMenuTriggered(QAction*)));
				}else
				{
					m_menu->clear();
					m_menu->addAction(tr("删除"));
				}
				m_menu->exec(QCursor::pos());
			}
		}
		if(m_nameMap[str]=="p_corcnt")
		{
			if (m_menu==NULL)
			{
				m_menu=new QMenu(this);
				m_menu->addAction(tr("删除范围"));
				QObject::connect(m_menu,SIGNAL(triggered(QAction*)),this,SLOT(OnMenuTriggered(QAction*)));
			}else
			{
				m_menu->clear();
				m_menu->addAction(tr("删除范围"));
			}
			m_menu->exec(QCursor::pos());
		}
		if(str==tr("扩展属性"))
		{
			if (m_menu==NULL)
			{
				m_menu=new QMenu(this);
				m_menu->addAction(tr("删除属性"));
				QObject::connect(m_menu,SIGNAL(triggered(QAction*)),this,SLOT(OnMenuTriggered(QAction*)));
			}else
			{
				m_menu->clear();
				m_menu->addAction(tr("删除属性"));
			}
			m_menu->exec(QCursor::pos());
		}
	}
}
void ExtAttributeDlg::OnTree2CustomContextMenuRequested(const QPoint&pos)
{
	QTreeWidgetItem * item = m_tree2->itemAt(pos);
	if (item==NULL)
		return;
	m_citem=item;
	QString str = item->text(0);
	if (m_nameMap[str]=="fc"||m_nameMap[str]=="lc"||m_nameMap[str]=="p_cor")
	{
		if (m_menu==NULL)
		{
			m_menu=new QMenu(this);
			m_menu->addAction(tr("添加"));
			QObject::connect(m_menu,SIGNAL(triggered(QAction*)),this,SLOT(OnMenuTriggered(QAction*)));
		}else
		{
			m_menu->clear();
			m_menu->addAction(tr("添加"));
		}
		m_menu->exec(QCursor::pos());
	}
	if(str==tr("扩展属性"))
	{
		if (m_menu==NULL)
		{
			m_menu=new QMenu(this);
			m_menu->addAction(tr("添加属性"));
			QObject::connect(m_menu,SIGNAL(triggered(QAction*)),this,SLOT(OnMenuTriggered(QAction*)));
		}else
		{
			m_menu->clear();
			m_menu->addAction(tr("添加属性"));
		}
		m_menu->exec(QCursor::pos());
	}
	QTreeWidgetItem *pitem=m_citem->parent();
	if (pitem!=NULL)
	{
		str=pitem->text(0);
		if (m_nameMap[str]=="fc"||m_nameMap[str]=="lc"||m_nameMap[str]=="p_cor")
		{
			if (m_citem->parent()->indexOfChild(m_citem)==m_citem->parent()->childCount()-1)
			{

				if (m_menu==NULL)
				{
					m_menu=new QMenu(this);
					m_menu->addAction(tr("删除"));
					QObject::connect(m_menu,SIGNAL(triggered(QAction*)),this,SLOT(OnMenuTriggered(QAction*)));
				}else
				{
					m_menu->clear();
					m_menu->addAction(tr("删除"));
				}
				m_menu->exec(QCursor::pos());
			}
		}
		if(str==tr("扩展属性"))
		{
			if (m_menu==NULL)
			{
				m_menu=new QMenu(this);
				m_menu->addAction(tr("删除属性"));
				QObject::connect(m_menu,SIGNAL(triggered(QAction*)),this,SLOT(OnMenuTriggered(QAction*)));
			}else
			{
				m_menu->clear();
				m_menu->addAction(tr("删除属性"));
			}
			m_menu->exec(QCursor::pos());
		}
	}

}

void ExtAttributeDlg::OnTreeItemChanged(QTreeWidgetItem* item,int column)
{
	QString str=item->text(0);
	QTreeWidgetItem * pitem=item->parent();
	if (pitem==NULL)
	{
		return;
	}
	if ((str==m_showMap["row"]||str==m_showMap["col"])&&column==1)
	{
		pitem->setText(1,QString("%1,%2").arg(pitem->child(0)->text(1)).arg(pitem->child(1)->text(1)));
	}
}
void ExtAttributeDlg::OnMenuTriggered(QAction* action)
{
	if (action->text()==tr("添加"))
	{
		QTreeWidgetItem * aitem=new QTreeWidgetItem();
		QString pstr=m_citem->text(0);
		QString str=pstr.right(pstr.size()-tr("缺省").size())+"_"+QString("%1").arg(m_citem->childCount());
		aitem->setText(0,str);
		m_citem->addChild(aitem);
		QFont fot;
		fot.setItalic(true);
		fot.setUnderline(true);
		aitem->setFont(2,fot);
		aitem->setText(2,tr("设置..."));
		aitem->setTextColor(2,Qt::blue);
	}
	if (action->text()==tr("删除"))
	{
		delete m_citem;
		m_citem=NULL;
	}
	if (action->text()==tr("添加范围"))
	{
		QTreeWidgetItem * aitem=new QTreeWidgetItem();
		QString pstr=m_citem->text(0);
		QString str=pstr+"_"+QString("%1").arg(m_citem->childCount());
		aitem->setText(0,str);
		m_citem->addChild(aitem);
	}
	if (action->text()==tr("删除范围"))
	{
		delete m_citem;
		m_citem=NULL;
	}
	if (action->text()==tr("添加属性"))
	{
		QTreeWidgetItem * aitem=new QTreeWidgetItem();
		aitem->setFlags(aitem->flags()|Qt::ItemIsEditable);
		QString pstr=m_citem->text(0);
		m_citem->addChild(aitem);
	}
	if (action->text()==tr("删除属性"))
	{
		delete m_citem;
		m_citem=NULL;
	}
}
int ExtAttributeDlg::flashStrToInt(QString str)
{
	if(str==tr("开启闪烁"))
		return 1;
	else if(str==tr("关闭闪烁"))
		return 0;
	return 0;
}
QString ExtAttributeDlg::flashIntToStr(int num)
{
	if(num==1)
		return tr("开启闪烁");
	else if(num==0)
		return tr("关闭闪烁");
	return tr("关闭闪烁");
}
void ExtAttributeDlg::OnChoosed(QTableWidgetItem*item)
{
	m_choosedlg->hide();
	//int row=((QTableWidget*)sender())->item(item->row(),0)->text().toInt();
	int row = item->row()+1;
	m_citem->setText(1,QString("%1,%2").arg(row).arg(item->column()+1));
	m_citem->child(0)->setText(1,QString::number(row));
	m_citem->child(1)->setText(1,QString::number(item->column()+1));
}


void RefData::getPosData(SString refName,SRecordset &rs)
{
	//SString sSql=SString::toFormat("select dw_select from t_ssp_data_window where name=%1",refName.data();
	CSsp_DatawindowMgr *dataWindow=GET_DW_CFG;
	CSsp_Dataset* dbSet=NULL;
	dbSet =  dataWindow->SearchDataset(refName);
	if (dbSet==NULL)
	{
		SQt::ShowErrorBox(QObject::tr("提示"),QObject::tr("获取数据集失败"));
		return;
	}
	if (dbSet->GetRecordset(&rs)==NULL)
	{
		SQt::ShowErrorBox(QObject::tr("提示"),QObject::tr("获取记录集失败"));
	}
}
void RefData::getCfgWndNo(SDatabaseOper *dboper,SRecordset &rs)
{
	SString sSql="select wnd_sn,wnd_name from t_ssp_uicfg_wnd";
	if (dboper->RetrieveRecordset(sSql,rs)<0)
	{
		SQt::ShowErrorBox(QObject::tr("提示"),QObject::tr("获取组态窗口编号失败"));
	}
}
void RefData::getFunPoint(SDatabaseOper *dboper,SRecordset &rs)
{
	SString sSql="select fun_key,name from t_ssp_fun_point where type<>1";
	if (dboper->RetrieveRecordset(sSql,rs)<0)
	{
		SQt::ShowErrorBox(QObject::tr("提示"),QObject::tr("获取功能点集失败"));
	}
}
void RefData::getGenAction(SDatabaseOper *dboper,SRecordset &rs)
{
	SString sSql="select act_sn,act_name from t_ssp_general_action";
	if (dboper->RetrieveRecordset(sSql,rs)<0)
	{
		SQt::ShowErrorBox(QObject::tr("提示"),QObject::tr("获取通用动作集失败"));
	}
}
void RefData::getRefData(SDatabaseOper *dboper,SRecordset &rs,int type,QString cls)
{
	SString tmp;
	if (type==2||type==1)
	{
		//type=3;
		tmp="dbtype<>1";
	}
	else if (type==3||type==4||type==5)
	{
		//type=2;
		tmp="dbtype=2";
	}
	if(cls!="")
		tmp+=cls.toStdString().data();
	SString sSql=SString::toFormat("select name,dw_desc from t_ssp_data_window where dstype=2 and ")+tmp;
	sSql+=" order by cls_name,dw_desc";
	if (dboper->RetrieveRecordset(sSql,rs)<0)
	{
		SQt::ShowErrorBox(QObject::tr("提示"),QObject::tr("获取引用集失败"));
	}
}


SpinBoxDelegate::SpinBoxDelegate(QObject *parent)
	: QItemDelegate(parent)
{
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
	const QStyleOptionViewItem &/* option */,
	const QModelIndex &/* index */) const
{
	QSpinBox *editor = new QSpinBox(parent);
	editor->setMinimum(0);
	editor->setMaximum(10000);

	return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor,
	const QModelIndex &index) const
{
	int value = index.model()->data(index, Qt::EditRole).toInt();

	QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
	spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
	const QModelIndex &index) const
{
	QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
	spinBox->interpretText();
	int value = spinBox->value();

	model->setData(index, value, Qt::EditRole);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
	const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}