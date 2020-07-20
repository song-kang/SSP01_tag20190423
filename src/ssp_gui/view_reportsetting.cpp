#include "view_reportsetting.h"
static void qt_graphicsItem_highlightSelected(QGraphicsItem *item, QPainter *painter, const QStyleOptionGraphicsItem *option);
static QString setValue(QString info,QString name,QString value);
static QString getAtu(QString info,QString name);
view_reportsetting::view_reportsetting(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	sql_dlg=new settingSQL_dlg(this);
	group_dlg=NULL;
	page_dlg=NULL;
	topWidget=new QWidget(this);
	leftWidget=new QWidget(this);
	m_dataWindow=GET_DW_CFG;
	width=0;height=0;
	m_scene=new MyGraphicsScene(this);
	connect(m_scene,SIGNAL(changeHW(int,int)),this,SLOT(changeHW(int,int)));
	m_pOper=DB;
	dsNo=-1;
 	m_view = new QGraphicsView(m_scene); 
	m_view->centerOn(0,0);
	QHBoxLayout *layouttop = new QHBoxLayout();
	newreport=new QPushButton(tr("新建报表(&N)"),this);
	chgName=new QPushButton(tr("报表重命名(&R)"),this);
	selreport=new QComboBox(this);
	selreport->addItem("未选择");
	delreport=new QPushButton(tr("删除当前报表"),this);
	delElement=new QPushButton(tr("删除元素(&D)"),this);
	sqlElement=new QPushButton(tr("数据源设置(&S)"),this);
	groupElement=new QPushButton(tr("分组(&G)"),this);
	pageSetting=new QPushButton(tr("配置页设置(&P)"),this);
	//pageSetting->hide();
	layouttop->addWidget(newreport);
	layouttop->addWidget(selreport);
	layouttop->addWidget(chgName);
	layouttop->addWidget(delreport);
	layouttop->addWidget(delElement);
	layouttop->addWidget(sqlElement);
	layouttop->addWidget(groupElement);
	layouttop->addWidget(pageSetting);
	layouttop->addStretch();
	layouttop->setMargin(2);
	topWidget->setLayout(layouttop);
	
	QVBoxLayout *layoutleft = new QVBoxLayout();
	rectDraw=new QPushButton(this);
	rectDraw->setText("矩形");
	rectDraw->hide();
	lineHDraw=new QPushButton(this);
	lineHDraw->setToolTip("横线");
	lineHDraw->setMinimumHeight(32);
	lineHDraw->setMaximumHeight(32);
	lineHDraw->setMinimumWidth(32);
	lineHDraw->setMaximumWidth(32);
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/rpt_lineH.png"), QSize(), QIcon::Normal, QIcon::Off);
	lineHDraw->setIcon(icon1);

	lineVDraw=new QPushButton(this);
	lineVDraw->setToolTip("竖线");
	lineVDraw->setMinimumHeight(32);
	lineVDraw->setMaximumHeight(32);
	lineVDraw->setMinimumWidth(32);
	lineVDraw->setMaximumWidth(32);
	QIcon icon2;
	icon2.addFile(QString::fromUtf8(":/ssp_gui/Resources/rpt_lineV.png"), QSize(), QIcon::Normal, QIcon::Off);
	lineVDraw->setIcon(icon2);

	textDraw=new QPushButton(this);
	textDraw->setToolTip("文本");
	textDraw->setMinimumHeight(32);
	textDraw->setMaximumHeight(32);
	textDraw->setMinimumWidth(32);
	textDraw->setMaximumWidth(32);
	QIcon icon3;
	icon3.addFile(QString::fromUtf8(":/ssp_gui/Resources/rpt_txt.png"), QSize(), QIcon::Normal, QIcon::Off);
	textDraw->setIcon(icon3);

	lineDraw=new QPushButton(this);
	lineDraw->setToolTip("斜线");
	lineDraw->setMinimumHeight(32);
	lineDraw->setMaximumHeight(32);
	lineDraw->setMinimumWidth(32);
	lineDraw->setMaximumWidth(32);
	QIcon icon4;
	icon4.addFile(QString::fromUtf8(":/ssp_gui/Resources/rpt_line.png"), QSize(), QIcon::Normal, QIcon::Off);
	lineDraw->setIcon(icon4);

	expDraw=new QPushButton(this);
	expDraw->setToolTip("表达式");
	expDraw->setMinimumHeight(32);
	expDraw->setMaximumHeight(32);
	expDraw->setMinimumWidth(32);
	expDraw->setMaximumWidth(32);
	QIcon icon5;
	icon5.addFile(QString::fromUtf8(":/ssp_gui/Resources/rpt_grp.png"), QSize(), QIcon::Normal, QIcon::Off);
	expDraw->setIcon(icon5);

	chartDraw=new QPushButton(this);
	chartDraw->setToolTip("图表");
	chartDraw->setMinimumHeight(32);
	chartDraw->setMaximumHeight(32);
	chartDraw->setMinimumWidth(32);
	chartDraw->setMaximumWidth(32);
	QIcon icon6;
	icon6.addFile(QString::fromUtf8(":/ssp_gui/Resources/rpt_chart.png"), QSize(), QIcon::Normal, QIcon::Off);
	chartDraw->setIcon(icon6);

	Halign=new QPushButton(this);
	Halign->setToolTip("横对齐");
	Halign->setMinimumHeight(32);
	Halign->setMaximumHeight(32);
	Halign->setMinimumWidth(32);
	Halign->setMaximumWidth(32);
	QIcon icon7;
	icon7.addFile(QString::fromUtf8(":/ssp_gui/Resources/rpt_Halign.png"), QSize(), QIcon::Normal, QIcon::Off);
	Halign->setIcon(icon7);

	Valign=new QPushButton(this);
	Valign->setToolTip("竖对齐");
	Valign->setMinimumHeight(32);
	Valign->setMaximumHeight(32);
	Valign->setMinimumWidth(32);
	Valign->setMaximumWidth(32);
	QIcon icon8;
	icon8.addFile(QString::fromUtf8(":/ssp_gui/Resources/rpt_Valign.png"), QSize(), QIcon::Normal, QIcon::Off);
	Valign->setIcon(icon8);

	sizeAlign=new QPushButton(this);
	sizeAlign->setToolTip("大小对齐");
	sizeAlign->setMinimumHeight(32);
	sizeAlign->setMaximumHeight(32);
	sizeAlign->setMinimumWidth(32);
	sizeAlign->setMaximumWidth(32);
	QIcon icon9;
	icon9.addFile(QString::fromUtf8(":/ssp_gui/Resources/rpt_Salign.png"), QSize(), QIcon::Normal, QIcon::Off);
	sizeAlign->setIcon(icon9);

	layoutleft->addWidget(rectDraw);
	layoutleft->addWidget(lineHDraw);
	layoutleft->addWidget(lineVDraw);
	layoutleft->addWidget(lineDraw);
	layoutleft->addWidget(textDraw);
	layoutleft->addWidget(expDraw);
	layoutleft->addWidget(chartDraw);
	layoutleft->addWidget(Halign);
	layoutleft->addWidget(Valign);
	layoutleft->addWidget(sizeAlign);
	layoutleft->addStretch();
	layoutleft->setMargin(2);
	leftWidget->setLayout(layoutleft);

	QHBoxLayout *layout1 = new QHBoxLayout();
	layout1->addWidget(leftWidget);
	layout1->addWidget(m_view);
	//layout1->addStretch();
	layout1->setMargin(2);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(topWidget);
	layout->addLayout(layout1);
	layout->addStretch();
	setLayout(layout);

	SRecordset reports;
	m_pOper->RetrieveRecordset("select report_no,report_name from t_ssp_report_info order by report_no",reports);

	reportCount=reports.GetRows();
	for(int i=0;i<reportCount;i++)
	{
		selreport->addItem(QString((reports.GetValue(i,0)+":"+reports.GetValue(i,1)).data()));
	}

	connect(newreport,SIGNAL(clicked()),this,SLOT(onPushButton_new()));
	connect(selreport,SIGNAL(currentIndexChanged(int)),this,SLOT(initPaint(int)));
	connect(chgName,SIGNAL(clicked()),this,SLOT(onPushButton_chgName()));
	connect(delreport,SIGNAL(clicked()),this,SLOT(onPushButton_delreport()));
	connect(delElement,SIGNAL(clicked()),this,SLOT(onPushButton_delElm()));
	connect(sqlElement,SIGNAL(clicked()),this,SLOT(onPushButton_sql()));
	connect(groupElement,SIGNAL(clicked()),this,SLOT(onPushButton_group()));
	connect(pageSetting,SIGNAL(clicked()),this,SLOT(onPushButton_page()));

	connect(rectDraw,SIGNAL(clicked()),this,SLOT(onPushButton_rect()));
	connect(lineHDraw,SIGNAL(clicked()),this,SLOT(onPushButton_lineH()));
	connect(lineVDraw,SIGNAL(clicked()),this,SLOT(onPushButton_lineV()));
	connect(lineDraw,SIGNAL(clicked()),this,SLOT(onPushButton_line()));
	connect(textDraw,SIGNAL(clicked()),this,SLOT(onPushButton_text()));
	connect(chartDraw,SIGNAL(clicked()),this,SLOT(onPushButton_chart()));
	connect(expDraw,SIGNAL(clicked()),this,SLOT(onPushButton_exp()));
	connect(Halign,SIGNAL(clicked()),this,SLOT(onPushButton_Halign()));
	connect(Valign,SIGNAL(clicked()),this,SLOT(onPushButton_Valign()));
	connect(sizeAlign,SIGNAL(clicked()),this,SLOT(onPushButton_setSize()));
	connect(m_scene,SIGNAL(itemInserted(QGraphicsRectItem*)),this,SLOT(itemInserted(QGraphicsRectItem*)));
	
}

view_reportsetting::~view_reportsetting()
{

}
//改变页面高度
void view_reportsetting::changeHW(int height,int width)
{
	this->height=height;
	this->width=width;
	m_view->setSceneRect(QRectF(0,0,width,height));
	int realX=m_view->size().width();
	int realY=m_view->size().height();
	int tranX=0;int tranY=0;
	if(realX>width)
		tranX=realX-width;
	if(realY>height)
		tranY=realY-height;
	m_view->translate(tranX,tranY);
	m_scene->setSceneRect(QRectF(0,0,width,height));
}
//新建报表
void view_reportsetting::onPushButton_new()
{
	bool ok;

	QString name=QInputDialog::getText(0,tr("报表名称"),tr("请输入名称:"),QLineEdit::Normal,"",&ok,0);  
	int newReportNo=m_pOper->SelectInto("select max(report_no) from t_ssp_report_info").toInt()+1;
	if(!name.isEmpty()) 
	{
		reportCount++;
		selreport->addItem(QString::number(newReportNo)+QString(":")+name);
	}
	else
		return;
	m_scene->clear();
	selreport->setCurrentIndex(selreport->count()-1);
	
	QString size="1200,800";
	width=1200;
	height=800;
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_info (report_no,report_name,rpt_size) \
										  values (%d,'%s','%s')",newReportNo,name.toStdString().data(),size.toStdString().data()));
	m_view->setSceneRect(QRectF(0,0,width,height));
	reportNo=newReportNo;
	initNewPage(reportNo);
}
void view_reportsetting::initNewPage(int reportNo)
{
	m_scene->initScreen();
	m_scene->setNo(reportNo);
	m_scene->initNewPage(selreport->currentText());
}
void view_reportsetting::onPushButton_chgName()
{
	if(reportNo<1)
		return;
	QString curName=m_pOper->SelectInto(SString::toFormat("select report_name from t_ssp_report_info where report_no=%d",reportNo)).data();
	bool ok;
	QString name=QInputDialog::getText(0,tr("修改文本"),tr("请输入新文本:"),QLineEdit::Normal,curName,&ok,0);  
	if(!name.isEmpty())
	{
		m_pOper->Execute(SString::toFormat("update t_ssp_report_info set report_name='%s'where report_no=%d",name.toStdString().data(),reportNo));
		selreport->setItemText(selreport->currentIndex(),QString::number(reportNo)+QString(":")+name);
	}
}
void view_reportsetting::initPaint(int cont)
{
	SRecordset reports,items;
	m_scene->clear();
	if(cont==0)
	{
		reportNo=0;
		return;
	}
	m_pOper->RetrieveRecordset("select report_no,report_name,ds_no,rpt_size from t_ssp_report_info order by report_no",reports);
	reportNo=reports.GetValue(cont-1,0).toInt();//第一行是默认空
	dsNo=reports.GetValue(cont-1,2).toInt();
	SString s13=reports.GetValue(cont-1,3);
	width=SString::GetIdAttributeI(1,s13,",");
	height=SString::GetIdAttributeI(2,s13,",");
// 	m_scene->setSceneRect(QRectF(0,0,width,height));
// 	m_view->setSceneRect(QRectF(0,0,width,height));

	int realX=m_view->size().width();
	int realY=m_view->size().height();
	int tranX=width;int tranY=height;
	if(realX>width)
		tranX=realX;
	if(realY>height)
		tranY=realY;
	m_scene->setSceneRect(QRectF(0,0,tranX,tranY));
	m_view->setSceneRect(QRectF(0,0,tranX,tranY));

	m_pOper->RetrieveRecordset(SString::toFormat("select item_no,report_no,pos,type,rect,info from t_ssp_report_item where report_no=%d order by seg+0",reportNo),items);
	m_scene->setNo(reportNo);

	CSsp_Dataset* dbSet=NULL;
	SString dsName=m_pOper->SelectInto(SString::toFormat("select name from t_ssp_data_window where dw_sn=%d",dsNo));
	dbSet=m_dataWindow->SearchDataset(dsName);

	m_scene->setDsNo(dbSet);
	m_scene->saveAllDs(dbSet);
	for(int i=0;i<items.GetRows();i++)
	{
		m_scene->initPaint(items.GetValue(i,0).toInt(),items.GetValue(i,3).toInt(),items.GetValue(i,2).data(),items.GetValue(i,4).data(),items.GetValue(i,5).data());
	}
	
}
//删除当前保镖
void view_reportsetting::onPushButton_delreport()
{
	if (QMessageBox::No==QMessageBox::warning(this,tr("警告"),tr("是否删除当前图表？"),QMessageBox::Yes,QMessageBox::No))
		return;
	if(selreport->currentIndex()==0)
		return;
	int tmpNo=reportNo;
	int selNo=selreport->currentIndex();
	selreport->setCurrentIndex(0);
	selreport->removeItem(selNo);
	m_scene->initScreen();
	m_pOper->Execute(SString::toFormat("delete from t_ssp_report_info where report_no=%d",tmpNo));
	m_pOper->Execute(SString::toFormat("delete from t_ssp_report_item where report_no=%d",tmpNo));

}
//删除选择元素
void view_reportsetting::onPushButton_delElm()
{
	if (QMessageBox::No==QMessageBox::warning(this,tr("警告"),tr("是否删除所选元素？"),QMessageBox::Yes,QMessageBox::No))
		return;
	m_scene->delSelElm();
}
//选择数据集
void view_reportsetting::onPushButton_sql()
{
	if(selreport->currentIndex()==0)
		return;
	sql_dlg->setDsNo(dsNo);
	sql_dlg->exec();
	dsNo=sql_dlg->getNo();
	if(dsNo!=0)
	{
		m_pOper->Execute(SString::toFormat("update t_ssp_report_info set ds_no=%d where report_no=%d",dsNo,reportNo));
		CSsp_Dataset* dbSet=NULL;//是否要delete
		SString dsName=m_pOper->SelectInto(SString::toFormat("select name from t_ssp_data_window where dw_sn=%d",dsNo));
		dbSet=m_dataWindow->SearchDataset(dsName);
		m_scene->setDsNo(dbSet);
		m_scene->saveAllDs(dbSet);
	}

}
//分组（只支持一重）
void view_reportsetting::onPushButton_group()
{
	if(selreport->currentIndex()==0)
		return;
	if(dsNo==-1)
		return;
	if(group_dlg==NULL)
	{
		group_dlg=new settingGroup_dlg(dsNo,this);
		connect(group_dlg,SIGNAL(deleteGrp()),m_scene,SLOT(deleteGrp()));
	}
	SRecordset grpSets;
	m_pOper->RetrieveRecordset("select item_no,info from t_ssp_report_item where seg='100'",grpSets);//暂时只允许一个分组

	QString info=grpSets.GetValue(0,1).data();
	QString fontSize=getAtu(info,"fontsize");//字体大小
	group_dlg->setFontSize(fontSize);

	QString fontFamily=getAtu(info,"fontfamily");//字体
	group_dlg->setFontFamily(fontFamily);

	QString colorStr=getAtu(info,"color");//字体颜色
	QStringList lstColorStr=colorStr.split(",");
	if(lstColorStr.size()>=3)
	{
		QColor color(lstColorStr.at(0).toInt(),lstColorStr.at(1).toInt(),lstColorStr.at(2).toInt());
		group_dlg->setColor(color);
	}
	else
	{
		group_dlg->setColor(QColor(0,0,0));
	}

	QString fontType=getAtu(info,"fonttype");//字体类型（加粗斜体）
	group_dlg->setFontType(fontType.toInt());

	QString txtop=getAtu(info,"txtop");//中左右
	group_dlg->setTxtOp(txtop.toInt());

	QString delLine=getAtu(info,"delline");//删除线
	if(delLine=="true")
		group_dlg->setFontDelLine(true);
	else
		group_dlg->setFontDelLine(false);

	QString undLine=getAtu(info,"undline");//下划线
	if(undLine=="true")
		group_dlg->setFontUndLine(true);
	else
		group_dlg->setFontUndLine(false);

	QString isRect=getAtu(info,"isrect");//外框
	group_dlg->setIsRect(isRect.toInt());

	QString rectPx=getAtu(info,"rectpx");//外框粗细
	group_dlg->setRectpx(rectPx.toInt());

	QString rectColorStr=getAtu(info,"rectcor");//外框颜色，没有color可能报错
	QStringList lstRectColorStr=rectColorStr.split(",");
	if(lstRectColorStr.size()>=3)
	{
		QColor rectColor(lstRectColorStr.at(0).toInt(),lstRectColorStr.at(1).toInt(),lstRectColorStr.at(2).toInt());
		group_dlg->setRectColor(rectColor);
	}
	else
	{
		group_dlg->setRectColor(QColor(0,0,0));
	}
	QString txt=getAtu(info,"txt");//外框粗细
	group_dlg->setTxt(txt);

	QString grp=getAtu(info,"grp");//外框粗细
	group_dlg->setGrp(grp);

	group_dlg->setDs(dsNo);
	group_dlg->initDlg();
	group_dlg->exec();
	//从对话框读取属性
	QString newInfo;
	if(group_dlg->getTxt()!="")
	{
		QString txt=group_dlg->getTxt();
		QString newInfo=setValue(info,"txt",txt);
		info=newInfo;
	}
	fontSize=group_dlg->getFontSize();
	newInfo=setValue(info,"fontsize",fontSize);
	info=newInfo;

	fontFamily=group_dlg->getFontFamily();
	newInfo=setValue(info,"fontfamily",fontFamily);
	info=newInfo;

	QColor color=group_dlg->getColor();
	colorStr=QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue());
	newInfo=setValue(info,"color",colorStr);
	info=newInfo;

	fontType=QString::number(group_dlg->getFontType());
	newInfo=setValue(info,"fonttype",fontType);
	info=newInfo;

	txtop=QString::number(group_dlg->getTxtOp());
	newInfo=setValue(info,"txtop",txtop);
	info=newInfo;

	bool delline=group_dlg->getFontDelLine();
	if(delline)
		newInfo=setValue(info,"delline","true");
	else
		newInfo=setValue(info,"delline","false");
	info=newInfo;

	bool undline=group_dlg->getFontUndLine();
	if(undline)
		newInfo=setValue(info,"undline","true");
	else
		newInfo=setValue(info,"undline","false");
	info=newInfo;

	isRect=QString::number(group_dlg->getIsRect());
	newInfo=setValue(info,"isrect",isRect);
	info=newInfo;

	rectPx=QString::number(group_dlg->getRectpx());
	newInfo=setValue(info,"rectpx",rectPx);
	info=newInfo;

	QColor rectColor=group_dlg->getRectColor();
	rectColorStr=QString::number(rectColor.red())+","+QString::number(rectColor.green())+","+QString::number(rectColor.blue());
	newInfo=setValue(info,"rectcor",rectColorStr);
	info=newInfo;

	if(group_dlg->getGrp()!="")
	{
		QString grp=group_dlg->getGrp();
		m_scene->drawDiv(grp,dsNo,info);
	}
}
//设置高宽
void view_reportsetting::onPushButton_page()
{
	if(selreport->currentIndex()==0)
		return;
	if(page_dlg==NULL)
		page_dlg=new pagesetting_dlg(this);
	page_dlg->setWH(width,height);
	page_dlg->exec();
	page_dlg->getWH(width,height);
	//m_scene->setSceneRect(QRectF(0,0,width,height));
	QString size=QString::number(width)+","+QString::number(height);
	m_pOper->Execute(SString::toFormat("update t_ssp_report_info set rpt_size='%s' where report_no=%d",size.toStdString().data(),reportNo));
	//m_view->setSceneRect(QRectF(0,0,width,height));
	int realX=m_view->size().width();
	int realY=m_view->size().height();
	int tranX=width;int tranY=height;
	if(realX>width)
		tranX=realX;
	if(realY>height)
		tranY=realY;
	m_scene->setSceneRect(QRectF(0,0,tranX,tranY));
	m_view->setSceneRect(QRectF(0,0,tranX,tranY));
	//m_view->translate(tranX*2,tranY);
	//m_view->centerOn(0,0);
}
//画矩形（unuse）
void view_reportsetting::onPushButton_rect()
{
	if(selreport->currentIndex()!=0)
		m_scene->setMode(MyGraphicsScene::InsertRect);
}
//横线
void view_reportsetting::onPushButton_lineH()
{	
	if(selreport->currentIndex()!=0)
		m_scene->setMode(MyGraphicsScene::InsertHLine);
}
//竖线
void view_reportsetting::onPushButton_lineV()
{
	if(selreport->currentIndex()!=0)
		m_scene->setMode(MyGraphicsScene::InsertVLine);
}
//文本
void view_reportsetting::onPushButton_text()
{
	if(selreport->currentIndex()!=0)
		m_scene->setMode(MyGraphicsScene::InsertText);
}
//折线
void view_reportsetting::onPushButton_line()
{
	if(selreport->currentIndex()!=0)
		m_scene->setMode(MyGraphicsScene::InsertLine);
}
//表达式
void view_reportsetting::onPushButton_exp()
{
	if(selreport->currentIndex()!=0)
		m_scene->setMode(MyGraphicsScene::InsertExp);
}
//图标
void view_reportsetting::onPushButton_chart()
{
	if(selreport->currentIndex()!=0)
		m_scene->setMode(MyGraphicsScene::InsertChart);
}
//横对其
void view_reportsetting::onPushButton_Halign()
{
	if(selreport->currentIndex()!=0)
		m_scene->setHalign();
}
//竖对其
void view_reportsetting::onPushButton_Valign()
{
	if(selreport->currentIndex()!=0)
		m_scene->setValign();
}
//大小对其
void view_reportsetting::onPushButton_setSize()
{
	if(selreport->currentIndex()!=0)
		m_scene->setSameSize();
}
///////////////////////////////////////////////////////////////////////////////////
MyGraphicsScene::MyGraphicsScene(QObject *parent)
	: QGraphicsScene(parent)
{
	setSceneRect(QRectF(0,0,1200,700));
	m_pOper=DB;
	myMode = MoveItem;
	line = 0;
	textItem = 0;
	divList=new QList<divStruct>();
	itemCount=0;
	myItemColor = Qt::white;
	myTextColor = Qt::black;
	myLineColor = Qt::black;
	dbSet=NULL;
	connect(this,SIGNAL(selectionChanged()),this,SLOT(selListChanged()));
}
MyGraphicsScene::~MyGraphicsScene()
{
	divList->clear();
	delete divList;
}
void MyGraphicsScene::initScreen()
{
	clear();
	itemCount=0;
	divList->clear();
}
//设置分组
void MyGraphicsScene::drawDiv(QString dsName,int dsNo,QString info)
{
	itemCount++;int count=0;
	m_pOper->ExecuteSQL("delete from t_ssp_report_item where seg='100'");
	int height=divList->at(count).height;
	QList<QGraphicsItem *>items=this->items();

	SString dsDesc=m_pOper->SelectInto(SString::toFormat("select col_name from t_ssp_data_window_col where col_sn=%d and dw_sn=%d",dsName.toInt()+1,dsNo));
/*	drawGraphics(QPointF(0,height+18),QRectF(0,0,5000,18),5,reportNo,itemCount,SString::toFormat("txt=分组:%s;fontsize=10;",dsDesc.data()).data());*/
	QString pos="0,"+QString::number(height+18);
	QString rect="0,0,5000,18";
	SString divNum="100";
	
	QString newInfo=setValue(info,"grp",dsName);

	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),5,divNum.data(),rect.toStdString().data(),newInfo.toStdString().data()));
	QString sInfo=m_pOper->SelectInto(SString::toFormat("select info from t_ssp_report_item where type='5' and seg='2' and report_no=%d",reportNo)).data();
	QString newsInfo=setValue(sInfo,"txt",QString("数据集+分组列：")+dsName);
	m_pOper->ExecuteSQL(SString::toFormat("update t_ssp_report_item set info='%s' where type='5' and seg='2'and report_no=%d",newsInfo.toStdString().data(),reportNo));

	int itemNo1=m_pOper->SelectIntoI(SString::toFormat("select item_no from t_ssp_report_item where type='5' and seg='2' and report_no=%d",reportNo));
	for(int i=0;i<items.size();i++)
	{
		int reportNo;int itemNo;
		((MyGraphicsItem*)items.at(i))->getNo(reportNo,itemNo);
		if(itemNo==itemNo1)
		{
			((MyGraphicsItem*)items.at(i))->setTxt(QString("数据集+分组列：")+dsName);
			break;
		}
	}
// 	divStruct div2(divNum.data(),itemCount,height+18);
// 	divList->insert(count+1,div2);
}
//绘制初始页面的元素
void MyGraphicsScene::initNewPage(QString curName)
{
	itemCount++;
	drawGraphics(QPointF(0,0),QRectF(0,0,5000,18),5,reportNo,itemCount,"txt=表头;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;");
	QString pos="0,0";
	QString rect="0,0,5000,18";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),5,"0",rect.toStdString().data(),"txt=表头;fontsize=10;"));
	divStruct div0("0",itemCount,0);
	divList->append(div0);

	itemCount++;
	drawGraphics(QPointF(406,18),QRectF(-62,1,217,44),4,reportNo,itemCount,"txt=变电站;fontsize=18;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;fonttype=1;");
	pos="406,18";
	rect="-62,1,217,44";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),4,"1",rect.toStdString().data(),"txt=变电站;fontsize=20;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;fonttype=1;"));

	itemCount++;
	drawGraphics(QPointF(462,63),QRectF(-117,2,217,37),4,reportNo,itemCount,QString("txt=%1;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;fonttype=1;").arg(curName));
	pos="462,63";
	rect="-117,2,217,37";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),4,"1",rect.toStdString().data(),QString("txt=%1;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;fonttype=1;").arg(curName).toStdString().data()));

	itemCount++;
	drawGraphics(QPointF(0,139),QRectF(0,0,5000,18),5,reportNo,itemCount,"txt=页头;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;");
	pos="0,139";
	rect="0,0,5000,18";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),5,"1",rect.toStdString().data(),"txt=页头;fontsize=10;"));
	divStruct div1("1",itemCount,139);
	divList->append(div1);

	itemCount++;
	drawGraphics(QPointF(12,101),QRectF(0,16,82,21),4,reportNo,itemCount,"txt=列1;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;");
	pos="12,101";
	rect="0,16,82,21";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),4,"1",rect.toStdString().data(),"txt=列1;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;"));

	itemCount++;
	drawGraphics(QPointF(117,101),QRectF(0,16,82,21),4,reportNo,itemCount,"txt=列2;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;");
	pos="117,101";
	rect="0,16,82,21";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),4,"1",rect.toStdString().data(),"txt=列2;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;"));

	itemCount++;
	drawGraphics(QPointF(222,101),QRectF(0,16,82,21),4,reportNo,itemCount,"txt=列3;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;");
	pos="222,101";
	rect="0,16,82,21";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),4,"1",rect.toStdString().data(),"txt=列3;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;"));

	itemCount++;
	drawGraphics(QPointF(0,182),QRectF(0,0,5000,18),5,reportNo,itemCount,"txt=数据集;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;");
	pos="0,182";
	rect="0,0,5000,18";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),5,"2",rect.toStdString().data(),"txt=数据集;fontsize=10;"));
	divStruct div2("2",itemCount,182);
	divList->append(div2);

	itemCount++;
	drawGraphics(QPointF(13,144),QRectF(0,16,82,21),6,reportNo,itemCount,"txt=表达式1;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;");
	pos="13,144";
	rect="0,16,82,21";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),6,"2",rect.toStdString().data(),"txt=表达式1;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;"));

	itemCount++;
	drawGraphics(QPointF(116,143),QRectF(0,16,82,21),6,reportNo,itemCount,"txt=表达式2;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;");
	pos="116,143";
	rect="0,16,82,21";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),6,"2",rect.toStdString().data(),"txt=表达式2;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;"));

	itemCount++;
	drawGraphics(QPointF(222,144),QRectF(0,16,82,21),6,reportNo,itemCount,"txt=表达式3;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;");
	pos="222,144";
	rect="0,16,82,21";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),6,"2",rect.toStdString().data(),"txt=表达式3;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;"));

	itemCount++;
	drawGraphics(QPointF(0,236),QRectF(0,0,5000,18),5,reportNo,itemCount,"txt=总计;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;");
	pos="0,236";
	rect="0,0,5000,18";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),5,"3",rect.toStdString().data(),"txt=总计;fontsize=10;"));
	divStruct div3("3",itemCount,236);
	divList->append(div3);

	itemCount++;
	drawGraphics(QPointF(14,210),QRectF(0,0,81,20),6,reportNo,itemCount,"txt=总计;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;");
	pos="14,210";
	rect="0,0,81,20";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),4,"3",rect.toStdString().data(),"txt=总计;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;"));

	itemCount++;
	drawGraphics(QPointF(0,479),QRectF(0,0,5000,18),5,reportNo,itemCount,"txt=页尾;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;");
	pos="0,479";
	rect="0,0,5000,18";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),5,"4",rect.toStdString().data(),"txt=页尾;fontsize=10;"));
	divStruct div4("4",itemCount,479);
	divList->append(div4);

	itemCount++;
	drawGraphics(QPointF(9,110),QRectF(0,0,566,1),2,reportNo,itemCount,"px=2;type=0;color=0,0,0;");
	pos="9,110";
	rect="0,0,566,1";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),1,"4",rect.toStdString().data(),"px=2;type=0;color=0,0,0;"));

	itemCount++;
	drawGraphics(QPointF(9,206),QRectF(0,0,566,1),2,reportNo,itemCount,"px=2;type=0;color=0,0,0;");
	pos="9,206";
	rect="0,0,566,1";
	m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,seg,rect,info) values (%d,%d,'%s',%d,'%s','%s','%s')",itemCount,reportNo,pos.toStdString().data(),1,"4",rect.toStdString().data(),"px=2;type=0;color=0,0,0;"));

	//dsNo=-1;
	m_pOper->ExecuteSQL(SString::toFormat("update t_ssp_report_info set ds_no=%d where report_no=%d",-1,reportNo));
}
//删除元素
void MyGraphicsScene::delSelElm()
{

	QList<QGraphicsItem *>items=this->selectedItems();
	for(int i=0;i<items.size();i++)
	{
		
		int reportNo;int itemNo;
		((MyGraphicsItem*)items.at(i))->getNo(reportNo,itemNo);
		if(((MyGraphicsItem*)items.at(i))->getType()==5)//不能删除分隔符
		{
			continue;
// 			for(int j=0;j<divList->size();j++)
// 			{
// 				if(itemNo==divList->at(j).itemNo)
// 				{
// 					divList->removeAt(j);
// 					break;
// 				}
// 			}
		}
		removeItem(items.at(i));
		delete items.at(i);
		m_pOper->Execute(SString::toFormat("delete from t_ssp_report_item where report_no=%d and item_no=%d",reportNo,itemNo));
	}
	update();
}
//设置横对其
void MyGraphicsScene::setHalign()
{
	//QList<QGraphicsItem *>items=this->selectedItems();
	if(selItems.size()>0)
	{
		QPointF point=selItems.at(0)->pos();
		qreal y=point.y();
		for(int i=0;i<selItems.size();i++)
		{
			selItems.at(i)->setPos(selItems.at(i)->pos().x(),y);
			int itemNo=0;int reportNo=0;
			((MyGraphicsItem*)selItems.at(i))->getNo(reportNo,itemNo);
			QString pos=QString::number(selItems.at(i)->pos().x())+","+QString::number(y);
			m_pOper->Execute(SString::toFormat("update t_ssp_report_item set pos='%s' where item_no=%d and report_no=%d",pos.toStdString().data(),itemNo,reportNo));
		}
	}
	update();
}
//竖对其
void MyGraphicsScene::setValign()
{
	//QList<QGraphicsItem *>items=this->selectedItems();
	if(selItems.size()>0)
	{
		QPointF point=selItems.at(0)->pos();
		qreal x=point.x();
		for(int i=0;i<selItems.size();i++)
		{
			selItems.at(i)->setPos(x,selItems.at(i)->pos().y());
			int itemNo=0;int reportNo=0;
			((MyGraphicsItem*)selItems.at(i))->getNo(reportNo,itemNo);
			QString pos=QString::number(x)+","+QString::number(selItems.at(i)->pos().y());
			m_pOper->Execute(SString::toFormat("update t_ssp_report_item set pos='%s' where item_no=%d and report_no=%d",pos.toStdString().data(),itemNo,reportNo));
		}
	}
	update();
}
//大小对其
void MyGraphicsScene::setSameSize()
{
	//QList<QGraphicsItem *>items=this->selectedItems();
	if(selItems.size()>0)
	{
		QRectF rect=((MyGraphicsItem*)selItems.at(0))->rect();
		for(int i=0;i<selItems.size();i++)
		{
			((MyGraphicsItem*)selItems.at(i))->ResetRect(rect);
			int itemNo=0;int reportNo=0;
			((MyGraphicsItem*)selItems.at(i))->getNo(reportNo,itemNo);
			QString rect1=QString::number(rect.x())+","+QString::number(rect.y())+","+QString::number(rect.width())+","+QString::number(rect.height());
			m_pOper->Execute(SString::toFormat("update t_ssp_report_item set rect='%s' where item_no=%d and report_no=%d",rect1.toStdString().data(),itemNo,reportNo));
		}
	}
	update();
}
//对其功能按第一个元素为标准，以此函数确定第一个元素
void MyGraphicsScene::selListChanged()
{
	QList<QGraphicsItem *>items=this->selectedItems();//无序，需要确定第一个
	for(int i=0;i<items.size();i++)
	{
		MyGraphicsItem* item=(MyGraphicsItem*)items.at(i);
		int itemNo=0;int reportNo=0;
		item->getNo(reportNo,itemNo);
		bool isExist=false;
		for(int j=0;j<selItems.size();j++)
		{
			MyGraphicsItem* selItem=(MyGraphicsItem*)selItems.at(j);
			int selItemNo=0;int selReportNo=0;
			selItem->getNo(selReportNo,selItemNo);
			if(selItemNo==itemNo)
			{
				isExist=true;
				break;
			}
		}
		if(isExist==false)
			selItems.append(item);
	}
	for(int i=0;i<selItems.size();i++)
	{			
		MyGraphicsItem* selItem=(MyGraphicsItem*)selItems.at(i);
		int selItemNo=0;int selReportNo=0;
		selItem->getNo(selReportNo,selItemNo);
		bool isExist=false;
		for(int j=0;j<items.size();j++)
		{
			MyGraphicsItem* item=(MyGraphicsItem*)items.at(j);
			int itemNo=0;int reportNo=0;
			item->getNo(reportNo,itemNo);
			if(selItemNo==itemNo)
			{
				isExist=true;
				break;
			}
		}
		if(isExist==false)
		{
			selItems.removeOne(selItem);
			i=0;
		}
	}
}
//绘制图形
void MyGraphicsScene::drawGraphics(QPointF pos,QRectF rect,int type,int reportNo,int itemNo,QString info)
{
	MyGraphicsItem *item= new MyGraphicsItem();
	addItem(item);
	QFont myFont;
	item->setType(type);
	item->setNo(reportNo,itemNo);
	item->setPos(pos);
	item->setDivList(divList);
	item->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	item->setFlag(QGraphicsItem::ItemIsSelectable, true);
	connect(item,SIGNAL(refresh()),this,SLOT(refresh()));
	connect(item,SIGNAL(saveAllPos()),this,SLOT(saveAllPos()));
	connect(item,SIGNAL(moveItems(float,float)),this,SLOT(moveItems(float,float)));
	if(type==2||type==3||type==7)
	{
		item->setLinePx(getAtu(info,"px").toInt());
		QString colorStr=getAtu(info,"color");
		QStringList lstColorStr=colorStr.split(",");
		if(lstColorStr.size()>=3)
		{
			QColor color(lstColorStr.at(0).toInt(),lstColorStr.at(1).toInt(),lstColorStr.at(2).toInt());
			item->setColor(color);
			item->setLineType(getAtu(info,"type").toInt());
		}
	}
	else if(type==4)
	{
		item->setTxt(getAtu(info,"txt"));
		QString colorStr=getAtu(info,"color");
		QStringList lstColorStr=colorStr.split(",");
		if(lstColorStr.size()>=3)
		{
			QColor color(lstColorStr.at(0).toInt(),lstColorStr.at(1).toInt(),lstColorStr.at(2).toInt());
			item->setColor(color);
		}
		QString txtOp=getAtu(info,"txtop");
		item->setTxtop(txtOp.toInt());
		myFont.setPointSize(getAtu(info,"fontsize").toInt());
		myFont.setFamily(getAtu(info,"fontfamily"));
		QString fontType=getAtu(info,"fonttype");
		if(fontType=="0")
		{
			myFont.setBold(false);
			myFont.setItalic(false);
		}
		else if(fontType=="1")
		{
			myFont.setBold(true);
			myFont.setItalic(false);
		}
		else if(fontType=="2")
		{
			myFont.setBold(false);
			myFont.setItalic(true);
		}
		else if(fontType=="3")
		{
			myFont.setBold(true);
			myFont.setItalic(true);
		}
		if(getAtu(info,"delline")=="true")
			myFont.setStrikeOut(true);
		else
			myFont.setStrikeOut(false);
		if(getAtu(info,"undline")=="true")
			myFont.setUnderline(true);
		else
			myFont.setUnderline(false);
		item->setFont(myFont);
	}
	else if(type==5)
	{
		item->setTxt(getAtu(info,"txt"));
		myFont.setPointSize(getAtu(info,"fontsize").toInt());
		myFont.setFamily(getAtu(info,"family"));
		item->setFont(myFont);
		connect(item,SIGNAL(divChanged(QPointF,int)),this,SLOT(divChanged(QPointF,int)));
		connect(item,SIGNAL(changeHeight(int)),this,SLOT(changeHeight(int)));
	}
	else if(type==6)
	{
		item->setTxt(getAtu(info,"txt"));
		QString colorStr=getAtu(info,"color");
		QStringList lstColorStr=colorStr.split(",");
		if(lstColorStr.size()>=3)
		{
			QColor color(lstColorStr.at(0).toInt(),lstColorStr.at(1).toInt(),lstColorStr.at(2).toInt());
			item->setColor(color);
		}
		QString txtOp=getAtu(info,"txtop");
		item->setTxtop(txtOp.toInt());
		myFont.setPointSize(getAtu(info,"fontsize").toInt());
		myFont.setFamily(getAtu(info,"fontfamily"));
		QString fontType=getAtu(info,"fonttype");
		if(fontType=="0")
		{
			myFont.setBold(false);
			myFont.setItalic(false);
		}
		else if(fontType=="1")
		{
			myFont.setBold(true);
			myFont.setItalic(false);
		}
		else if(fontType=="2")
		{
			myFont.setBold(false);
			myFont.setItalic(true);
		}
		else if(fontType=="3")
		{
			myFont.setBold(true);
			myFont.setItalic(true);
		}
		if(getAtu(info,"delline")=="true")
			myFont.setStrikeOut(true);
		else
			myFont.setStrikeOut(false);
		if(getAtu(info,"undline")=="true")
			myFont.setUnderline(true);
		else
			myFont.setUnderline(false);
		item->setFont(myFont);
	}
	else if(type==8)
	{
		item->setTxt(getAtu(info,"txt"));
		int type=getAtu(info,"charttype").toInt();
		item->setChartType(type);
		QString chartDs=getAtu(info,"chartds");
		item->setChartDs(chartDs);
	}
	item->setAtu(info);
	if(type!=5)
		item->ResetRect(rect);
	else
	{
		QRectF tmpRect;
		tmpRect.setX(rect.x());
		tmpRect.setWidth(width());
		tmpRect.setHeight(rect.height());
		tmpRect.setY(rect.y());
		item->ResetRect(tmpRect);
	}
	item->setDs(dbSet);
	myMode=MoveItem;
}
void MyGraphicsScene::deleteGrp()
{
	m_pOper->ExecuteSQL("delete from t_ssp_report_item where seg='100'");
	QString sInfo=m_pOper->SelectInto(SString::toFormat("select info from t_ssp_report_item where type='5' and seg='2' and report_no=%d",reportNo)).data();
	QString newsInfo=setValue(sInfo,"txt",QString("数据集"));
	m_pOper->ExecuteSQL(SString::toFormat("update t_ssp_report_item set info='%s'where where type='5' and seg='2'and report_no=%d",newsInfo.toStdString().data(),reportNo));

	int itemNo1=m_pOper->SelectIntoI(SString::toFormat("select item_no from t_ssp_report_item where type='5' and seg='2' and report_no=%d",reportNo));
	for(int i=0;i<this->items().size();i++)
	{
		int reportNo;int itemNo;
		((MyGraphicsItem*)this->items().at(i))->getNo(reportNo,itemNo);
		if(itemNo==itemNo1)
		{
			((MyGraphicsItem*)this->items().at(i))->setTxt(QString("数据集"));
			break;
		}
	}
}

//移动item（多选移动用）
void MyGraphicsScene::moveItems(float dx,float dy)
{
	QList<QGraphicsItem *>items=this->selectedItems();
	for(int i=0;i<items.size();i++)
	{
		((MyGraphicsItem*)items.at(i))->moveItem(dx,dy);
	}
}
//设置高度
void MyGraphicsScene::changeHeight(int height)
{
	QRectF rectf=sceneRect();
	rectf.setHeight(height);
	
	emit changeHW(height,rectf.width());
	//setSceneRect(rectf);
}
//单击响应（添加元素用）
void MyGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if (mouseEvent->button() != Qt::LeftButton)
		return;
	
	QString pos;
	QString rect;
	switch (myMode) {

	case InsertRect:
		itemCount++;
		drawGraphics(mouseEvent->scenePos(),QRectF(0,0,50,50),1,reportNo,itemCount,"");
		pos=QString::number(mouseEvent->scenePos().x())+","+QString::number(mouseEvent->scenePos().y());
		rect="0,0,50,50";
		m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,rect,info) values (%d,%d,'%s',%d,'%s','%s')",itemCount,reportNo,pos.toStdString().data(),1,rect.toStdString().data(),""));
		break;

	case InsertHLine:
		itemCount++;
		drawGraphics(mouseEvent->scenePos(),QRectF(0,0,100,1),2,reportNo,itemCount,"px=2;type=0;color=0,0,0;");
		pos=QString::number(mouseEvent->scenePos().x())+","+QString::number(mouseEvent->scenePos().y());
		rect="0,0,100,1";
		m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,rect,info) values (%d,%d,'%s',%d,'%s','%s')",itemCount,reportNo,pos.toStdString().data(),2,rect.toStdString().data(),"px=2;type=0;color=0,0,0;"));
		break;

	case InsertVLine:
		itemCount++;
		drawGraphics(mouseEvent->scenePos(),QRectF(0,0,1,100),3,reportNo,itemCount,"px=2;type=0;color=0,0,0;");
		pos=QString::number(mouseEvent->scenePos().x())+","+QString::number(mouseEvent->scenePos().y());
		rect="0,0,1,100";
		m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,rect,info) values (%d,%d,'%s',%d,'%s','%s')",itemCount,reportNo,pos.toStdString().data(),3,rect.toStdString().data(),"px=2;type=0;color=0,0,0;"));
		break;

 	case InsertText:
		itemCount++;
		drawGraphics(mouseEvent->scenePos(),QRectF(0,0,80,20),4,reportNo,itemCount,"txt=txt;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;");
		pos=QString::number(mouseEvent->scenePos().x())+","+QString::number(mouseEvent->scenePos().y());
		rect="0,0,80,20";
		m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,rect,info) values (%d,%d,'%s',%d,'%s','%s')",itemCount,reportNo,pos.toStdString().data(),4,rect.toStdString().data(),"txt=txt;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;"));
		break;

// 	case InsertDiv:
// 		itemCount++;
// 		drawGraphics(QPointF(0,mouseEvent->scenePos().y()),QRectF(0,0,5000,18),5,reportNo,itemCount,"txt=分割;");
// 		pos=QString::number(0)+","+QString::number(mouseEvent->scenePos().y());
// 		rect="0,0,5000,18";
// 		m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,rect,info) values (%d,%d,'%s',%d,'%s','%s')",itemCount,reportNo,pos.toStdString().data(),5,rect.toStdString().data(),"txt=分割;fontsize=10;"));
// 		break;

	case InsertExp:
		itemCount++;
		drawGraphics(mouseEvent->scenePos(),QRectF(0,0,65,20),6,reportNo,itemCount,"txt=表达式;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;");
		pos=QString::number(mouseEvent->scenePos().x())+","+QString::number(mouseEvent->scenePos().y());
		rect="0,0,80,20";
		m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,rect,info) values (%d,%d,'%s',%d,'%s','%s')",itemCount,reportNo,pos.toStdString().data(),6,rect.toStdString().data(),"txt=表达式;fontsize=10;fontfamily=宋体;color=0,0,0;fonttype=0;txtop=0;delline=false;undline=false;isrect=0;rectpx=1;rectcor=0,0,0;"));
		break;

	case InsertLine:
		itemCount++;
		drawGraphics(mouseEvent->scenePos(),QRectF(0,0,50,50),7,reportNo,itemCount,"px=2;type=0;color=0,0,0;");
		pos=QString::number(mouseEvent->scenePos().x())+","+QString::number(mouseEvent->scenePos().y());
		rect="0,0,1,50";
		m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,rect,info) values (%d,%d,'%s',%d,'%s','%s')",itemCount,reportNo,pos.toStdString().data(),7,rect.toStdString().data(),"px=2;type=0;color=0,0,0;"));
		break;

	case InsertChart:
		itemCount++;
		drawGraphics(mouseEvent->scenePos(),QRectF(0,0,100,100),8,reportNo,itemCount,"txt=图像;fontsize=10;");
		pos=QString::number(mouseEvent->scenePos().x())+","+QString::number(mouseEvent->scenePos().y());
		rect="0,0,100,100";
		m_pOper->ExecuteSQL(SString::toFormat("insert into t_ssp_report_item (item_no,report_no,pos,type,rect,info) values (%d,%d,'%s',%d,'%s','%s')",itemCount,reportNo,pos.toStdString().data(),8,rect.toStdString().data(),"txt=图像;fontsize=10;"));
		break;
	default:
		;
	}
	QGraphicsScene::mousePressEvent(mouseEvent);
}
//从info获得属性，格式"name=atu;"
QString getAtu(QString info,QString name)
{
	QList <QString> strList=info.split(";");
	for(int i=0;i<strList.size();i++)
	{
		QString tmp=strList.at(i);
		if(tmp.split("=").at(0)==name)
		{
			if(tmp.split("=").size()>1)
				return tmp.split("=").at(1);
			else
				return "";
		}
	}
	return "";
}
//根据数据库内容绘制
void MyGraphicsScene::initPaint(int itemNo,int type,QString pos,QString rect,QString info)
{
	if(itemNo>itemCount)
		itemCount=itemNo;
	QPointF point;
	point.setX(pos.split(",").at(0).toInt());
	point.setY(pos.split(",").at(1).toInt());
	QString txt;
	QRectF rect1(rect.split(",").at(0).toInt(),rect.split(",").at(1).toInt(),rect.split(",").at(2).toInt(),rect.split(",").at(3).toInt());
	SString div;
	divStruct div1;
	switch (type) 
	{
	case 1:
		drawGraphics(point,rect1,1,reportNo,itemNo,info);
		break;
	case 2:
		drawGraphics(point,rect1,2,reportNo,itemNo,info);
		break;
	case 3:
		drawGraphics(point,rect1,3,reportNo,itemNo,info);
		break;
	case 4:
		drawGraphics(point,rect1,4,reportNo,itemNo,info);
		break;
	case 5:
		{
			div=m_pOper->SelectInto(SString::toFormat("select seg from t_ssp_report_item where item_no=%d and report_no=%d",itemNo,reportNo));
			if(div!="100")
			{
				drawGraphics(point,rect1,5,reportNo,itemNo,info);
				div1.setDiv(div.data(),itemNo,point.y());
				divList->append(div1);
			}
		}
		break;
	case 6:
		drawGraphics(point,rect1,6,reportNo,itemNo,info);
		break;
	case 7:
		drawGraphics(point,rect1,7,reportNo,itemNo,info);
		break;
	case 8:
		drawGraphics(point,rect1,8,reportNo,itemNo,info);
		break;
	default:
		;
	}
}
void MyGraphicsScene::setMode(Mode mode)
{
	myMode=mode;
}
void MyGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void MyGraphicsScene::refresh()
{
	update();
}
//分隔符改变高度时其下所有元素同样改变
void MyGraphicsScene::divChanged(QPointF divR,int y)
{
	QList<QGraphicsItem *>items=this->items();
	int orgH=divR.y();
	for(int i=0;i<items.size();i++)
	{
		if(orgH<items.at(i)->pos().y())
		{
			((MyGraphicsItem*)items.at(i))->moveItem(0,y);

		}
	}
}
//保存位置
void MyGraphicsScene::saveAllPos()
{
	QList<QGraphicsItem *>items=this->items();
	for(int i=0;i<items.size();i++)
	{
		if(((MyGraphicsItem*)items.at(i))->getType()==5)
			((MyGraphicsItem*)items.at(i))->savePos();
	}
	for(int i=0;i<items.size();i++)
	{
		if(((MyGraphicsItem*)items.at(i))->getType()!=5)
			((MyGraphicsItem*)items.at(i))->savePos();
	}
}
//为item设置数据集
void MyGraphicsScene::saveAllDs(CSsp_Dataset *dbSet)
{
	QList<QGraphicsItem *>items=this->items();
	for(int i=0;i<items.size();i++)
	{
		((MyGraphicsItem*)items.at(i))->setDs(dbSet);
	}
}
void MyGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
//键盘响应事件
void MyGraphicsScene::keyPressEvent(QKeyEvent *keyEvent)
{
	if(keyEvent->key()==Qt::Key_Up)
	{
		moveItems(0,-1);
		return;
	}
	if(keyEvent->key()==Qt::Key_Down)
	{
		moveItems(0,1);
		return;
	}
	if(keyEvent->key()==Qt::Key_Left)
	{
		moveItems(-1,0);
		return;
	}
	if(keyEvent->key()==Qt::Key_Right)
	{
		moveItems(1,0);
		return;
	}
	if(keyEvent->key()==Qt::Key_Delete)
	{
		if (QMessageBox::No==QMessageBox::warning(NULL,tr("警告"),tr("是否删除所选元素？"),QMessageBox::Yes,QMessageBox::No))
			return;
		delSelElm();
	}
	QGraphicsScene::keyPressEvent(keyEvent);
}

/////////////////////////////////////////////////////////////////////////////
MyGraphicsItem::MyGraphicsItem()
{
	setZValue(0);
	setAcceptDrops(true);
	setFlags(ItemIsSelectable);
	setAcceptsHoverEvents(true);
	cScale=false;
	m_cursor=new QCursor;
	direction=0;
	txt="txt";
	expDlg=NULL;
	txtDlg=NULL;
	lineDlg=NULL;
	chartDlg=NULL;
	m_pOper=DB;
	dbSet=NULL;
	
}
MyGraphicsItem::~MyGraphicsItem()
{
	delete m_cursor;
	delete expDlg;
	delete txtDlg;
	delete lineDlg;
	delete chartDlg;
	
}
void MyGraphicsItem::ResetRect(QRectF rect)
{
	m_rect=rect;
	update(boundingRect());
	//setPos(rect.x(),rect.y());

}
//获得分隔符列表，改变方位时所属分隔符可能改变
void MyGraphicsItem::setDivList(QList<divStruct>*divList)
{
	this->divList=divList;
}
//类型
void MyGraphicsItem::setType(int type)
{
	this->type=type;
}
//显示的文本
void MyGraphicsItem::setTxt(QString txt)
{
	this->txt=txt;
}
QRectF MyGraphicsItem::boundingRect()const
{
	return QRectF(m_rect.x()-2,m_rect.y()-2,m_rect.width()+4,m_rect.height()+4);
}
QRectF MyGraphicsItem::rect()
{
	return m_rect;
}
void MyGraphicsItem::setFont(QFont font)
{
	if(type==4)
		m_font=font;
	if(type==5)
		m_font=font;
	if(type==6)
		m_font=font;
}
void MyGraphicsItem::setColor(QColor color)
{
	m_color=color;
}
//响应区域
QPainterPath MyGraphicsItem::shape()const
{
	QPainterPath path;
	if(type==1||type==7)
		path.addRect(m_rect);
	else if(type==2)//横线
	{
		//path.moveTo(m_rect.x(),m_rect.y());
		//path.lineTo(m_rect.x()+m_rect.width(),m_rect.y());
		path.addRect(m_rect.x(),m_rect.y()-1,m_rect.width(),2);//设为矩形，方便选中
	}
	else if(type==3)//竖线
	{
		path.addRect(m_rect.x()-1,m_rect.y(),2,m_rect.height());//设为矩形，方便选中
		//path.moveTo(m_rect.x(),m_rect.y());
		//path.lineTo(m_rect.x(),m_rect.y()+m_rect.height());
	}
	else if(type==4)//文本
	{
		path.addText(m_rect.x(),m_rect.y()+m_font.pointSize(),m_font,txt);
		path.addRect(m_rect);
	}
	else if(type==5)//区域分割
	{
		path.addText(m_rect.x(),m_rect.y()+m_font.pointSize(),m_font,txt);
		path.addRect(m_rect);
	}
	else if(type==6)//表达式
	{
		path.addText(m_rect.x(),m_rect.y()+m_font.pointSize(),m_font,txt);
		path.addRect(m_rect);
	}
	else if(type==8)//图像
	{
		path.addText(m_rect.x(),m_rect.y()+m_font.pointSize(),m_font,txt);
		path.addRect(m_rect);
	}
	return path;
}
//绘制元素
void MyGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	QStyleOptionGraphicsItem op(*option ); 
	op.state=QStyle::State_HasFocus;
	QTextOption o;
	o.setAlignment(Qt::AlignCenter);
	if(txtOption==0)
	{
		o.setAlignment(Qt::AlignCenter);
	}
	else if(txtOption==1)
	{
		o.setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
	}
	else if(txtOption==2)
	{
		o.setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	}

	if(type==1)//矩形
		painter->drawRect(m_rect);

	else if(type==2)//横线
	{
		QPen oldpen=painter->pen();
		QPen pen;
		pen.setColor(m_color);
		pen.setStyle(Qt::SolidLine);
		if(lineType==1)
			pen.setStyle(Qt::DashLine);
		else if(lineType==2)
			pen.setStyle(Qt::DotLine);
		else if(lineType==3)
			pen.setStyle(Qt::DashDotLine);
		pen.setWidth(linePx);
		painter->setPen(pen);
		painter->drawLine(m_rect.x(),m_rect.y(),m_rect.x()+m_rect.width(),m_rect.y());
		painter->setPen(oldpen);
	}
	else if(type==3)//竖线
	{
		QPen oldpen=painter->pen();
		QPen pen;
		pen.setColor(m_color);
		pen.setStyle(Qt::SolidLine);
		if(lineType==1)
			pen.setStyle(Qt::DashLine);
		else if(lineType==2)
			pen.setStyle(Qt::DotLine);
		else if(lineType==3)
			pen.setStyle(Qt::DashDotLine);
		pen.setWidth(linePx);
		painter->setPen(pen);
		painter->drawLine(m_rect.x(),m_rect.y(),m_rect.x(),m_rect.y()+m_rect.height());
		painter->setPen(oldpen);
	}
	else if(type==7)//斜线
	{
		QPen oldpen=painter->pen();
		QPen pen;
		pen.setColor(m_color);
		pen.setStyle(Qt::SolidLine);
		if(lineType==1)
			pen.setStyle(Qt::DashLine);
		else if(lineType==2)
			pen.setStyle(Qt::DotLine);
		else if(lineType==3)
			pen.setStyle(Qt::DashDotLine);
		pen.setWidth(linePx);
		painter->setPen(pen);
		painter->drawLine(m_rect.x(),m_rect.y(),m_rect.x()+m_rect.width(),m_rect.y()+m_rect.height());
		painter->setPen(oldpen);
	}
	else if(type==4)//文本
	{
		painter->setFont(m_font);
		QPen oldpen=painter->pen();
		QPen pen;
		pen.setColor(m_color);
		painter->setPen(pen);
		painter->drawText(m_rect,txt,o);
		painter->setPen(oldpen);
		painter->drawRect(m_rect);
	}
	else if(type==5)//分割
	{
		painter->setFont(m_font);
		painter->setBrush(Qt::yellow);
		painter->drawRect(m_rect);
		QPen oldpen=painter->pen();
		QPen pen;
		pen.setColor(m_color);
		painter->setPen(pen);
		//painter->drawText(m_rect.x(),m_rect.y()+m_font.pointSize(),txt,o);
		QTextOption o;
		o.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
		painter->drawText(m_rect,txt,o);
		painter->setPen(oldpen);
	}
	else if(type==6)//表达式
	{
		painter->setFont(m_font);
		painter->drawRect(m_rect);
		QPen oldpen=painter->pen();
		QPen pen;
		pen.setColor(m_color);
		painter->setPen(pen);
		painter->drawText(m_rect,txt,o);
		painter->setPen(oldpen);
	}
	else if(type==8)//图像
	{
		QFont font;
		font.setPointSize(18);
		painter->setFont(font);
		painter->drawText(m_rect,txt,o);
		painter->drawRect(m_rect);
	}
	if (option->state & QStyle::State_Selected)//选中在外层绘制虚线框
	{
		qt_graphicsItem_highlightSelected(this, painter, option);
		//setFlags(flags()|QGraphicsItem::ItemIsFocusable);
	}
	//emit refresh();
}
//鼠标焦点进入item，形状改变
void MyGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	QPointF pos1=event->scenePos();
	QPointF lt=this->scenePos()+QPointF(m_rect.x(),m_rect.y());
	QPointF lb=this->scenePos()+QPointF(m_rect.x(),m_rect.y()+m_rect.height());
	QPointF rt=this->scenePos()+QPointF(m_rect.x()+m_rect.width(),m_rect.y());
	QPointF rb=this->scenePos()+QPointF(m_rect.x()+m_rect.width(),m_rect.y()+m_rect.height());
	if((pos1.x()<=lt.x()+2&&pos1.y()<=lt.y()+2)||((pos1.x()>=rb.x()-2&&pos1.y()>=rb.y()-2)&&type!=2&&type!=3))
	{
		m_cursor->setShape(Qt::SizeFDiagCursor);
	}
	else if((pos1.x()<=lb.x()+2&&pos1.y()>=lb.y()-2)||((pos1.x()>=rt.x()-2&&pos1.y()<=rt.y()+2)&&type!=2&&type!=3))
	{
		m_cursor->setShape(Qt::SizeBDiagCursor);
	}
	else if((pos1.x()<=lt.x()+2||pos1.x()>=rt.x()-2)&&(pos1.y()<=lb.y()&&pos1.y()>=lt.y())&&type!=3)
	{
		m_cursor->setShape(Qt::SizeHorCursor);
	}
	else if((pos1.y()<=lt.y()+2||pos1.y()>=lb.y()-2)&&(pos1.x()>=lt.x()&&pos1.x()<=rt.x())&&type!=2)
	{
		m_cursor->setShape(Qt::SizeVerCursor);
	}
	else
	{
		m_cursor->setShape(Qt::OpenHandCursor);
	}
	this->setCursor(*m_cursor);
	update();
	QGraphicsItem::hoverEnterEvent(event);
}
//鼠标焦点移动，形状改变
void MyGraphicsItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
	QPointF pos1=event->scenePos();
	QPointF lt=this->scenePos()+QPointF(m_rect.x(),m_rect.y());
	QPointF lb=this->scenePos()+QPointF(m_rect.x(),m_rect.y()+m_rect.height());
	QPointF rt=this->scenePos()+QPointF(m_rect.x()+m_rect.width(),m_rect.y());
	QPointF rb=this->scenePos()+QPointF(m_rect.x()+m_rect.width(),m_rect.y()+m_rect.height());
	if(((pos1.x()<=lt.x()+2&&pos1.y()<=lt.y()+2)||(pos1.x()>=rb.x()-2&&pos1.y()>=rb.y()-2))&&type!=2&&type!=3)//横竖线屏蔽
	{
		m_cursor->setShape(Qt::SizeFDiagCursor);
	}
	else if(((pos1.x()<=lb.x()+2&&pos1.y()>=lb.y()-2)||(pos1.x()>=rt.x()-2&&pos1.y()<=rt.y()+2))&&type!=2&&type!=3)
	{
		m_cursor->setShape(Qt::SizeBDiagCursor);
	}
	else if((pos1.x()<=lt.x()+2||pos1.x()>=rt.x()-2)&&(pos1.y()<=lb.y()&&pos1.y()>=lt.y())&&type!=3)
	{
		m_cursor->setShape(Qt::SizeHorCursor);
	}
	else if((pos1.y()<=lt.y()+2||pos1.y()>=lb.y()-2)&&(pos1.x()>=lt.x()&&pos1.x()<=rt.x())&&type!=2)
	{
		m_cursor->setShape(Qt::SizeVerCursor);
	}
	else
	{
		m_cursor->setShape(Qt::ClosedHandCursor);
	}
	this->setCursor(*m_cursor);
	update();
	QGraphicsItem::hoverMoveEvent(event);
}

void MyGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	m_cursor->setShape(Qt::ArrowCursor);
	this->setCursor(*m_cursor);
	update();
	QGraphicsItem::hoverLeaveEvent(event);
}

void MyGraphicsItem::keyPressEvent(QKeyEvent *event)
{
	QGraphicsItem::keyPressEvent(event);
}
void MyGraphicsItem::keyReleaseEvent(QKeyEvent *event)
{
	QGraphicsItem::keyReleaseEvent(event);
}
//812
//7 3
//654
//鼠标点击item，判断哪个方向拉伸
void MyGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if(event->button()==Qt::LeftButton){
 		start=event->scenePos();
		cScale=true;
		cMove=false;
		setFlag(QGraphicsItem::ItemIsMovable,false);
		QPointF pos1=event->scenePos();
		QPointF lt=this->scenePos()+QPointF(m_rect.x(),m_rect.y());
		QPointF lb=this->scenePos()+QPointF(m_rect.x(),m_rect.y()+m_rect.height());
		QPointF rt=this->scenePos()+QPointF(m_rect.x()+m_rect.width(),m_rect.y());
		QPointF rb=this->scenePos()+QPointF(m_rect.x()+m_rect.width(),m_rect.y()+m_rect.height());
		if(((pos1.x()<=lt.x()+2&&pos1.y()<=lt.y()+2)||(pos1.x()>=rb.x()-2&&pos1.y()>=rb.y()-2))&&type!=2&&type!=3)
		{
			if(pos1.x()<=lt.x()+2)
				direction=8;
			else 
				direction=4;
		}else if(((pos1.x()<=lb.x()+2&&pos1.y()>=lb.y()-2)||(pos1.x()>=rt.x()-2&&pos1.y()<=rt.y()+2))&&type!=2&&type!=3)
		{
			if(pos1.x()<=lb.x()+2)
				direction=6;
			else 
				direction=2;
		}
		else if((pos1.x()<=lt.x()+2||pos1.x()>=rt.x()-2)&&(pos1.y()<=lb.y()&&pos1.y()>=lt.y())&&type!=3)
		{
			if(pos1.x()<=lt.x()+2)
				direction=7;
			else 
				direction=3;
		}
		else if((pos1.y()<=lt.y()+2||pos1.y()>=lb.y()-2)&&(pos1.x()>=lt.x()&&pos1.x()<=rt.x())&&type!=2)
		{
			if(pos1.y()<=lt.y()+2)
				direction=1;
			else 
				direction=5;
		}
		else
		{
			cScale=false;
			cMove=true;
			m_cursor->setShape(Qt::ArrowCursor);
		}
	}
	if(type==2)//横线垂直方向屏蔽
	{
		if(direction==1||direction==5)
		{
			cScale=false;
			cMove=true;
		}
	}
	if(type==3)//竖线水平方向屏蔽
	{
		if(direction==3||direction==7)
		{
			cScale=false;
			cMove=true;
		}
	}
	if(type==5)//分割线使用自定义move
	{
		cScale=false;
		cMove=false;
		setFlag(QGraphicsItem::ItemIsMovable,false);
	}
	update();
	QGraphicsItem::mousePressEvent(event);
}
//鼠标双击item，打开设置框
void MyGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)//双击
{
	S_UNUSED(event);
	if(type==2||type==3||type==7)//3种线
	{
		if(lineDlg==NULL)
			lineDlg=new linesetting_dialog(0);
		QString info=m_pOper->SelectInto(SString::toFormat("select info from t_ssp_report_item where item_no=%d and report_no=%d",itemNo,reportNo)).data();
		//读取属性设置进对话框
		QString px=getAtu(info,"px");//粗细
		lineDlg->setpx(px.toInt());

		QString colorStr=getAtu(info,"color");//颜色
		QStringList lstColorStr=colorStr.split(",");
		if(lstColorStr.size()>=3)
		{
			QColor color(lstColorStr.at(0).toInt(),lstColorStr.at(1).toInt(),lstColorStr.at(2).toInt());
			lineDlg->setColor(color);
		}

		QString type=getAtu(info,"type");//类型（实线虚线）
		lineDlg->setType(type.toInt());

		lineDlg->exec();//打开属性对话框

		//从对话框读取设置
		QColor color=lineDlg->getColor();
		colorStr=QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue());
		QString newInfo=setValue(atu,"color",colorStr);
		m_color=color;
		atu=newInfo;

		type=QString::number(lineDlg->getType());
		newInfo=setValue(atu,"type",type);
		lineType=type.toInt();
		atu=newInfo;

		px=QString::number(lineDlg->getpx());
		newInfo=setValue(atu,"px",px);
		linePx=px.toInt();
		atu=newInfo;

		m_pOper->Execute(SString::toFormat("update t_ssp_report_item set info='%s' where item_no=%d and report_no=%d",newInfo.toStdString().data(),itemNo,reportNo));

		emit refresh();
	}
	else if(type==4)//文本
	{
		if(txtDlg==NULL)
			txtDlg=new txtsetting_dialog(0);
		QString info=m_pOper->SelectInto(SString::toFormat("select info from t_ssp_report_item where item_no=%d and report_no=%d",itemNo,reportNo)).data();
		//读取属性设置进对话框
		QString fontSize=getAtu(info,"fontsize");//字体大小
		txtDlg->setFontSize(fontSize);

		QString fontFamily=getAtu(info,"fontfamily");//字体
		txtDlg->setFontFamily(fontFamily);

		QString colorStr=getAtu(info,"color");//颜色
		QStringList lstColorStr=colorStr.split(",");
		if(lstColorStr.size()>=3)
		{
			QColor color(lstColorStr.at(0).toInt(),lstColorStr.at(1).toInt(),lstColorStr.at(2).toInt());
			txtDlg->setColor(color);
		}

		QString fontType=getAtu(info,"fonttype");//字体类型（倾斜加粗）
		txtDlg->setFontType(fontType.toInt());

		QString txtop=getAtu(info,"txtop");//左右中
		txtDlg->setTxtOp(txtop.toInt());

		QString delLine=getAtu(info,"delline");//删除线
		if(delLine=="true")
			txtDlg->setFontDelLine(true);
		else
			txtDlg->setFontDelLine(false);

		QString undLine=getAtu(info,"undline");//下划线
		if(undLine=="true")
			txtDlg->setFontUndLine(true);
		else
			txtDlg->setFontUndLine(false);

		QString isRect=getAtu(info,"isrect");//外框
		txtDlg->setIsRect(isRect.toInt());

		QString rectPx=getAtu(info,"rectpx");//外框粗细
		txtDlg->setRectpx(rectPx.toInt());

		QString rectColorStr=getAtu(info,"rectcor");//外框颜色
		QStringList lstRectColorStr=rectColorStr.split(",");
		if(lstRectColorStr.size()>=3)
		{
			QColor rectColor(lstRectColorStr.at(0).toInt(),lstRectColorStr.at(1).toInt(),lstRectColorStr.at(2).toInt());
			txtDlg->setRectColor(rectColor);
		}

		txtDlg->setTxt(txt);
		//txtDlg->initDlg();
		txtDlg->exec();//打开属性对话框
		//从对话框读取设置
		if(txtDlg->getTxt()!="")
		{
			txt=txtDlg->getTxt();
			QString newInfo=setValue(atu,"txt",txt);
			m_pOper->Execute(SString::toFormat("update t_ssp_report_item set info='%s' where item_no=%d and report_no=%d",newInfo.toStdString().data(),itemNo,reportNo));
			atu=newInfo;
		}
		fontSize=txtDlg->getFontSize();
		QString newInfo=setValue(atu,"fontsize",fontSize);
		atu=newInfo;

		fontFamily=txtDlg->getFontFamily();
		newInfo=setValue(atu,"fontfamily",fontFamily);
		atu=newInfo;


		QColor color=txtDlg->getColor();
		colorStr=QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue());
		newInfo=setValue(atu,"color",colorStr);
		m_color=color;
		atu=newInfo;

		fontType=QString::number(txtDlg->getFontType());
		newInfo=setValue(atu,"fonttype",fontType);
		atu=newInfo;

		txtop=QString::number(txtDlg->getTxtOp());
		newInfo=setValue(atu,"txtop",txtop);
		txtOption=txtop.toInt();
		atu=newInfo;

		bool delline=txtDlg->getFontDelLine();
		if(delline)
			newInfo=setValue(atu,"delline","true");
		else
			newInfo=setValue(atu,"delline","false");
		atu=newInfo;

		bool undline=txtDlg->getFontUndLine();
		if(undline)
			newInfo=setValue(atu,"undline","true");
		else
			newInfo=setValue(atu,"undline","false");
		atu=newInfo;

		isRect=QString::number(txtDlg->getIsRect());
		newInfo=setValue(atu,"isrect",isRect);
		isrect=isRect.toInt();
		atu=newInfo;

		rectPx=QString::number(txtDlg->getRectpx());
		newInfo=setValue(atu,"rectpx",rectPx);
		rectpx=rectPx.toInt();
		atu=newInfo;

		QColor rectColor=txtDlg->getRectColor();
		rectColorStr=QString::number(rectColor.red())+","+QString::number(rectColor.green())+","+QString::number(rectColor.blue());
		newInfo=setValue(atu,"rectcor",rectColorStr);
		rect_color=rectColor;
		atu=newInfo;

		m_pOper->Execute(SString::toFormat("update t_ssp_report_item set info='%s' where item_no=%d and report_no=%d",newInfo.toStdString().data(),itemNo,reportNo));

		QFont curfont=m_font;
		curfont.setPointSize(fontSize.toInt());
		curfont.setFamily(fontFamily);
		if(fontType=="0")
		{
			curfont.setBold(false);
			curfont.setItalic(false);
		}
		else if(fontType=="1")
		{
			curfont.setBold(true);
			curfont.setItalic(false);
		}
		else if(fontType=="2")
		{
			curfont.setBold(false);
			curfont.setItalic(true);
		}
		else if(fontType=="3")
		{
			curfont.setBold(true);
			curfont.setItalic(true);
		}
		if(delline)
			curfont.setStrikeOut(true);
		else
			curfont.setStrikeOut(false);
		if(undline)
			curfont.setUnderline(true);
		else
			curfont.setUnderline(false);
		m_font=curfont;
		setFont(curfont);

 		emit refresh();
	}
	else if(type==5)//分隔符
	{
// 		bool ok;
// 		QString name=QInputDialog::getText(0,tr("修改文本"),tr("请输入新文本:"),QLineEdit::Normal,"",&ok,0);  
// 		if(!name.isEmpty()) 
// 			txt=name; 
// 		QString newInfo=setValue(atu,"txt",txt);
// 		m_pOper->Execute(SString::toFormat("update t_ssp_report_item set info='%s' where item_no=%d and report_no=%d",newInfo.toStdString().data(),itemNo,reportNo));
// 		atu=newInfo;
	}
	else if(type==6)//表达式
	{
		if(dbSet==NULL)
			return;
		if(expDlg==NULL)
			expDlg=new settingExpress_dlg(txt,exp,0);
		expDlg->setDs(dbSet);
		//读取属性设置金对话框
		QString info=m_pOper->SelectInto(SString::toFormat("select info from t_ssp_report_item where item_no=%d and report_no=%d",itemNo,reportNo)).data();
		QString fontSize=getAtu(info,"fontsize");//字体大小
		expDlg->setFontSize(fontSize);

		QString fontFamily=getAtu(info,"fontfamily");//字体
		expDlg->setFontFamily(fontFamily);

		QString colorStr=getAtu(info,"color");//字体颜色
		QStringList lstColorStr=colorStr.split(",");
		if(lstColorStr.size()>=3)
		{
			QColor color(lstColorStr.at(0).toInt(),lstColorStr.at(1).toInt(),lstColorStr.at(2).toInt());
			expDlg->setColor(color);
		}

		QString fontType=getAtu(info,"fonttype");//字体类型（加粗斜体）
		expDlg->setFontType(fontType.toInt());
		
		QString txtop=getAtu(info,"txtop");//中左右
		expDlg->setTxtOp(txtop.toInt());

		QString delLine=getAtu(info,"delline");//删除线
		if(delLine=="true")
			expDlg->setFontDelLine(true);
		else
			expDlg->setFontDelLine(false);

		QString undLine=getAtu(info,"undline");//下划线
		if(undLine=="true")
			expDlg->setFontUndLine(true);
		else
			expDlg->setFontUndLine(false);

		QString isRect=getAtu(info,"isrect");//外框
		expDlg->setIsRect(isRect.toInt());

		QString rectPx=getAtu(info,"rectpx");//外框粗细
		expDlg->setRectpx(rectPx.toInt());

		QString rectColorStr=getAtu(info,"rectcor");//外框颜色，没有color可能报错
		QStringList lstRectColorStr=rectColorStr.split(",");
		if(lstRectColorStr.size()>=3)
		{
			QColor rectColor(lstRectColorStr.at(0).toInt(),lstRectColorStr.at(1).toInt(),lstRectColorStr.at(2).toInt());
			expDlg->setRectColor(rectColor);
		}

		QString exp=getAtu(info,"exp");//数据集
		expDlg->setExp(exp);

		QString fun=getAtu(info,"fun");//函数
		expDlg->setFun(fun);

		expDlg->setTxt(txt);

		expDlg->initDlg();
		expDlg->exec();//打开属性对话框
		//从对话框读取属性
		if(expDlg->getTxt()!="")
		{
			txt=expDlg->getTxt();
			QString newInfo=setValue(atu,"txt",txt);
			m_pOper->Execute(SString::toFormat("update t_ssp_report_item set info='%s' where item_no=%d and report_no=%d",newInfo.toStdString().data(),itemNo,reportNo));
			atu=newInfo;
		}
		if(expDlg->getExp()!="")
		{
			exp=expDlg->getExp();
			QString newInfo=setValue(atu,"exp",exp);
			m_pOper->Execute(SString::toFormat("update t_ssp_report_item set info='%s' where item_no=%d and report_no=%d",newInfo.toStdString().data(),itemNo,reportNo));
			atu=newInfo;
		}
		fun=expDlg->getFun();
		QString newInfo=setValue(atu,"fun",fun);
		atu=newInfo;

		fontSize=expDlg->getFontSize();
		newInfo=setValue(atu,"fontsize",fontSize);
		atu=newInfo;

		fontFamily=expDlg->getFontFamily();
		newInfo=setValue(atu,"fontfamily",fontFamily);
		atu=newInfo;
		
		QColor color=expDlg->getColor();
		colorStr=QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue());
		newInfo=setValue(atu,"color",colorStr);
		m_color=color;
		atu=newInfo;

		fontType=QString::number(expDlg->getFontType());
		newInfo=setValue(atu,"fonttype",fontType);
		atu=newInfo;

		txtop=QString::number(expDlg->getTxtOp());
		newInfo=setValue(atu,"txtop",txtop);
		txtOption=txtop.toInt();
		atu=newInfo;

		bool delline=expDlg->getFontDelLine();
		if(delline)
			newInfo=setValue(atu,"delline","true");
		else
			newInfo=setValue(atu,"delline","false");
		atu=newInfo;

		bool undline=expDlg->getFontUndLine();
		if(undline)
			newInfo=setValue(atu,"undline","true");
		else
			newInfo=setValue(atu,"undline","false");
		atu=newInfo;

		isRect=QString::number(expDlg->getIsRect());
		newInfo=setValue(atu,"isrect",isRect);
		isrect=isRect.toInt();
		atu=newInfo;

		rectPx=QString::number(expDlg->getRectpx());
		newInfo=setValue(atu,"rectpx",rectPx);
		rectpx=rectPx.toInt();
		atu=newInfo;

		QColor rectColor=expDlg->getRectColor();
		rectColorStr=QString::number(rectColor.red())+","+QString::number(rectColor.green())+","+QString::number(rectColor.blue());
		newInfo=setValue(atu,"rectcor",rectColorStr);
		rect_color=rectColor;
		atu=newInfo;

		m_pOper->Execute(SString::toFormat("update t_ssp_report_item set info='%s' where item_no=%d and report_no=%d",newInfo.toStdString().data(),itemNo,reportNo));

		QFont curfont=m_font;
		curfont.setPointSize(fontSize.toInt());
		curfont.setFamily(fontFamily);
		if(fontType=="0")
		{
			curfont.setBold(false);
			curfont.setItalic(false);
		}
		else if(fontType=="1")
		{
			curfont.setBold(true);
			curfont.setItalic(false);
		}
		else if(fontType=="2")
		{
			curfont.setBold(false);
			curfont.setItalic(true);
		}
		else if(fontType=="3")
		{
			curfont.setBold(true);
			curfont.setItalic(true);
		}
		if(delline)
			curfont.setStrikeOut(true);
		else
			curfont.setStrikeOut(false);
		if(undline)
			curfont.setUnderline(true);
		else
			curfont.setUnderline(false);
		m_font=curfont;
		setFont(curfont);
		emit refresh();
	}
	else if(type==8)//图标
	{
		if(chartDlg==NULL)
			chartDlg=new chartsetting_dialog(0);
		QString info=m_pOper->SelectInto(SString::toFormat("select info from t_ssp_report_item where item_no=%d and report_no=%d",itemNo,reportNo)).data();
		QString txt=getAtu(info,"txt");//显示文本（图表标题）
		chartDlg->setTxt(txt);

		QString chartDs=getAtu(info,"chartds");//数据集号
		chartDlg->setDs(chartDs.toInt());

		QString extAtu=getAtu(info,"extatu");//扩展属性
		chartDlg->setExtAtu(extAtu);

		QString chartType=getAtu(info,"charttype");//图表类型
		chartDlg->setType(chartType.toInt());

		//chartDlg->initDlg();
		chartDlg->exec();//打开对话框

		txt=chartDlg->getTxt();
		QString newInfo=setValue(atu,"txt",txt);
		atu=newInfo;

		chartDs=QString::number(chartDlg->getDs());
		newInfo=setValue(atu,"chartds",chartDs);
		atu=newInfo;

		extAtu=chartDlg->getExtAtu();
		newInfo=setValue(atu,"extatu",extAtu);
		atu=newInfo;

		chartType=QString::number(chartDlg->getType());
		newInfo=setValue(atu,"charttype",chartType);
		atu=newInfo;

		m_pOper->Execute(SString::toFormat("update t_ssp_report_item set info='%s' where item_no=%d and report_no=%d",newInfo.toStdString().data(),itemNo,reportNo));

		emit refresh();
	}
}
void MyGraphicsItem::setDs(CSsp_Dataset* dbSet)
{
	this->dbSet=dbSet;
}
//鼠标点击后移动
void MyGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if(cScale)
	{
		QPointF dis;
		end=event->scenePos();
		dis=end-start;
		start=end;
		switch(direction)
		{
		case 1://上
			{
				QRectF tem=QRectF(m_rect.x(),m_rect.y()+dis.y(),m_rect.width(),m_rect.height()-dis.y());
				if(tem.height()>0)
					this->ResetRect(tem);
				break;
			}
		case 2://右上
			{
				QRectF tem=QRectF(m_rect.x(),m_rect.y()+dis.y(),
					m_rect.width()+dis.x(),m_rect.height()-dis.y());
				if(tem.height()>0&&tem.width()>0)
					this->ResetRect(tem);
				break;
			}
		case 3://右
			{
				QRectF tem=QRectF(m_rect.x(),m_rect.y(),m_rect.width()+dis.x(),m_rect.height());
				if(tem.width()>0)
					this->ResetRect(tem);
				break;
			}
		case 4://右下
			{
				QRectF tem=QRectF(m_rect.x(),m_rect.y(),m_rect.width()+dis.x(),m_rect.height()+dis.y());
				if(tem.height()>0&&tem.width()>0)
					this->ResetRect(tem);
				break;
			}
		case 5://下
			{
				QRectF tem=QRectF(m_rect.x(),m_rect.y(),m_rect.width(),m_rect.height()+dis.y());
				if(tem.height()>0)
					this->ResetRect(tem);
				break;
			}
		case 6://左下
			{
				QRectF tem=QRectF(m_rect.x()+dis.x(),m_rect.y(),m_rect.width()-dis.x(),m_rect.height()+dis.y());
				if(tem.height()>0&&tem.width()>0)
					this->ResetRect(tem);
				break;
			}
		case 7://左
			{
				QRectF tem=QRectF(m_rect.x()+dis.x(),m_rect.y(),m_rect.width()-dis.x(),m_rect.height());
				if(tem.width()>0)
					this->ResetRect(tem);
				break;
			}
		case 8://左上
			{
				QRectF tem=QRectF(m_rect.x()+dis.x(),m_rect.y()+dis.y(),m_rect.width()-dis.x(),m_rect.height()-dis.y());
				if(tem.height()>0&&tem.width()>0)
					this->ResetRect(tem);
				break;
			}
		}
	}
	else if(cMove==true)//整体移动
	{
		QPointF dis;
		end=event->scenePos();
 		dis=end-start;
//		start=end;
// 		setPos(pos().x()+dis.x(),pos().y()+dis.y());
		emit moveItems(dis.x(),dis.y());
	}
	else if(type==5)//分隔符时下方元素一起改变位置
	{
		QPointF dis;
		end=event->scenePos();
		dis=end-start;
		start=end;
		setPos(pos().x(),pos().y()+dis.y());
		//QRectF tem=QRectF(m_rect.x(),m_rect.y()+dis.y(),
		//	m_rect.width(),m_rect.height());
		//this->ResetRect(tem);
		emit divChanged(pos(),dis.y());
	}
	QGraphicsItem::mouseMoveEvent(event);
}
void MyGraphicsItem::moveItem(int rx,int ry)
{
	QPointF tmpS=start;
	start.setX(tmpS.x()+rx);
	start.setY(tmpS.y()+ry);
	setPos(pos().x()+rx,pos().y()+ry);
}
void MyGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	cScale=false;
	cMove=false;
	update();
// 	if(type==5)
// 	{
// 		for(int i=0;i<divList->size();i++)
// 		{
// 			if(divList->at(i).itemNo==itemNo)
// 			{
// 				(*divList)[i].height=pos().y();
// 			}
// 		}
// 	}
// 	else
// 	{
// 		for(int i=0;i<divList->size();i++)
// 		{
// 			if(divList->at(i).height>=pos().y())
// 			{
// 				m_pOper->Execute(SString::toFormat("update t_ssp_report_item set seg='%s' where item_no=%d and report_no=%d",divList->at(i).no.data(),itemNo,reportNo));
// 				break;
// 			}
// 		}
// 	}
	emit saveAllPos();//保存位置
	QGraphicsItem::mouseReleaseEvent(event);
}
//保存位置，根据分隔符位置改变分隔符所属
void MyGraphicsItem::savePos()
{
	QRectF rect=m_rect;
	QString pos1=QString::number(pos().x())+","+QString::number(pos().y());
	QString rectStr=QString::number(rect.x())+","+QString::number(rect.y())+","+QString::number(rect.width())+","+QString::number(rect.height());
	if(type==5)
	{
		for(int i=0;i<divList->size();i++)
		{
			if(divList->at(i).itemNo==itemNo)
			{
				(*divList)[i].height=pos().y();
			}
		}
	}
	else
	{
		for(int i=0;i<divList->size();i++)
		{
			if(divList->at(i).height>=pos().y())
			{
				m_pOper->Execute(SString::toFormat("update t_ssp_report_item set seg='%s' where item_no=%d and report_no=%d",divList->at(i).no.data(),itemNo,reportNo));
				break;
			}
		}
	}
	m_pOper->Execute(SString::toFormat("update t_ssp_report_item set pos='%s',rect='%s' where item_no=%d and report_no=%d",pos1.toStdString().data(),rectStr.toStdString().data(),itemNo,reportNo));
}
//在info添加属性
QString setValue(QString info,QString name,QString value)
{
	QList <QString> strList=info.split(";");
	for(int i=0;i<strList.size();i++)
	{
		QString tmp=strList.at(i);
		if(tmp=="")
			continue;
		if(tmp.split("=").at(0)==name)
		{
			if(tmp.split("=").size()>1)
			{
				QString newInfo=name+"="+value+";";
				for(int j=0;j<strList.size();j++)
				{
					if(i!=j&&strList.at(j)!="")
					{
						newInfo+=strList.at(j)+";";
					}
				}
				return newInfo;
			}
			else
				return info;
		}
	}
	return name+"="+value+";"+info;//为空
}
/////////////////////////////////////////////////////////////////////////////////////////
settingSQL_dlg::settingSQL_dlg(QWidget *parent)
	: QDialog(parent)
{
	//报表数据源设置
	setWindowTitle(tr("数据窗口选择"));
	dsLabel=new QLabel(this);
	dsLabel->setText(tr("选择数据窗口"));
	curDsLabel=new QLabel(this);
	tableWidget=new QTableWidget(this);

	Ok=new QPushButton(this);
	Ok->setText(tr("确定(&O)"));
	Ok->setMaximumHeight(24);
	Ok->setDefault(true);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/image/ok"), QSize(), QIcon::Normal, QIcon::Off);
	Ok->setIcon(icon);
	cancel=new QPushButton(this);
	cancel->setText(tr("取消(&C)"));
	cancel->setMaximumHeight(24);
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/image/cancel"), QSize(), QIcon::Normal, QIcon::Off);
	cancel->setIcon(icon1);

	m_pOper=DB;
	QVBoxLayout *labelsqllayout=new QVBoxLayout();
	QVBoxLayout *sqllayout=new QVBoxLayout();
	QHBoxLayout *btnlayout=new QHBoxLayout();

	labelsqllayout->addWidget(dsLabel);
	labelsqllayout->addStretch();
	labelsqllayout->addWidget(curDsLabel);
	labelsqllayout->setMargin(2);

	sqllayout->addLayout(labelsqllayout);
	sqllayout->addWidget(tableWidget);
	sqllayout->setMargin(2);

	btnlayout->addStretch();
	btnlayout->addWidget(Ok);
	btnlayout->addWidget(cancel);
	btnlayout->setMargin(2);

	QVBoxLayout *layout=new QVBoxLayout();
	layout->addLayout(sqllayout);
	layout->addLayout(btnlayout);
	layout->setMargin(2);

	setLayout(layout);
	initDlg();
}
settingSQL_dlg::~settingSQL_dlg()
{

}
void settingSQL_dlg::initDlg()//设置表格内容
{
	tableWidget->clear();
	QStringList header;
	header<<tr("序号")<<tr("名称")<<tr("描述");
	tableWidget->setColumnCount(3);
	tableWidget->setRowCount(0);
	tableWidget->setHorizontalHeaderLabels(header);
	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	tableWidget->setColumnWidth(0,50);
	tableWidget->setColumnWidth(1,100);
	tableWidget->setColumnWidth(2,100);
	tableWidget->setMinimumWidth(350);
	tableWidget->verticalHeader()->setDefaultSectionSize(17);
	tableWidget->horizontalHeader()->setStretchLastSection(true);
	tableWidget->horizontalHeader()->setAutoScroll(true);
	tableWidget->verticalHeader()->setAutoScroll(true);
	SRecordset dsSet;
	m_pOper->RetrieveRecordset("select dw_sn,dstype,name,dw_desc from t_ssp_data_window order by cls_name desc,dw_desc",dsSet);

	for(int i=0;i<dsSet.GetRows();i++)
	{
		if(dsSet.GetValue(i,1).toInt()!=1)
		{
			int rowCount=tableWidget->rowCount();
			tableWidget->insertRow(rowCount);

			SString tmp=dsSet.GetValue(i,0).data();
			QTableWidgetItem *item=new QTableWidgetItem();
			item->setText(tmp.data());
			item->setFlags(item->flags()&~Qt::ItemIsEditable);
			tableWidget->setItem(rowCount,0,item);

			tmp=dsSet.GetValue(i,2).data();
			item=new QTableWidgetItem();
			item->setText(tmp.data());
			item->setFlags(item->flags()&~Qt::ItemIsEditable);
			tableWidget->setItem(rowCount,1,item);

			tmp=dsSet.GetValue(i,3).data();
			item=new QTableWidgetItem();
			item->setText(tmp.data());
			item->setFlags(item->flags()&~Qt::ItemIsEditable);
			tableWidget->setItem(rowCount,2,item);
		}
	}
	connect(Ok,SIGNAL(clicked()),this,SLOT(onOk()));
	connect(cancel,SIGNAL(clicked()),this,SLOT(onCancel()));
}
void settingSQL_dlg::onOk()
{
	if(tableWidget->selectedItems().size()>0)
	{
		int row=tableWidget->selectedItems().at(0)->row();
		dsNo=tableWidget->item(row,0)->text().toInt();
	}
	hide();
}
void settingSQL_dlg::onCancel()
{
	hide();
}
int settingSQL_dlg::getNo()
{
	return dsNo;
}
void settingSQL_dlg::setDsNo(int ds)
{
	dsNo=ds;
	curDsLabel->setText(QString("当前序号:%1").arg(dsNo));
}

/////////////////////////////////////////////////////////////////////////////////////////
settingExpress_dlg::settingExpress_dlg(QString txt,QString exp,QWidget *parent)
	: QDialog(parent)
{
	S_UNUSED(exp);
	//表达式元素属性设置
	setWindowTitle(tr("表达式设置"));
	funLabel=new QLabel(this);
	funLabel->setText("函数设置");
	curFunLabel=new QLabel(this);
	funList=new QTableWidget(this);

	colLabel=new QLabel(this);
	colLabel->setText("列设置");
	curColLabel=new QLabel(this);
	colList=new QTableWidget(this);

	tabWidget=new QTabWidget(this);
	tab1=new QWidget(this);
	tab2=new txtsetting_widget(this);

	Ok=new QPushButton(this);
	Ok->setText(tr("确定(&O)"));
	Ok->setMaximumHeight(24);
	Ok->setDefault(true);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/image/ok"), QSize(), QIcon::Normal, QIcon::Off);
	Ok->setIcon(icon);
	cancel=new QPushButton(this);
	cancel->setText(tr("取消(&C)"));
	cancel->setMaximumHeight(24);
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/image/cancel"), QSize(), QIcon::Normal, QIcon::Off);
	cancel->setIcon(icon1);

	QHBoxLayout *labelfunlayout=new QHBoxLayout();
	QVBoxLayout *funlayout=new QVBoxLayout();
	QHBoxLayout *labelcollayout=new QHBoxLayout();
	QVBoxLayout *collayout=new QVBoxLayout();
	QHBoxLayout *bottomlayout=new QHBoxLayout();
	QVBoxLayout *tab1layout=new QVBoxLayout();
	QHBoxLayout *btnlayout=new QHBoxLayout();

	labelfunlayout->addWidget(funLabel);
	labelfunlayout->addStretch();
	labelfunlayout->addWidget(curFunLabel);
	labelfunlayout->setMargin(2);

	funlayout->addLayout(labelfunlayout);
	funlayout->addWidget(funList);
	funlayout->setMargin(2);

	labelcollayout->addWidget(colLabel);
	labelcollayout->addStretch();
	labelcollayout->addWidget(curColLabel);
	labelcollayout->setMargin(2);

	collayout->addLayout(labelcollayout);
	collayout->addWidget(colList);
	collayout->setMargin(2);

	bottomlayout->addLayout(funlayout);
	bottomlayout->addLayout(collayout);
	bottomlayout->setMargin(2);

	tab1layout->addLayout(bottomlayout);
	tab1layout->setMargin(2);
	tab1->setLayout(tab1layout);
	tabWidget->addTab(tab1,"表达式设置");
	tabWidget->addTab(tab2,"文本设置");

	btnlayout->addStretch();
	btnlayout->addWidget(Ok);
	btnlayout->addWidget(cancel);
	btnlayout->setMargin(2);

	m_pOper=DB;
	this->txt=txt;
	this->col=col;

	QVBoxLayout *layout=new QVBoxLayout();
	layout->addWidget(tabWidget);
	layout->addLayout(btnlayout);
	layout->setMargin(2);
	setLayout(layout);

	//initDlg();
	connect(Ok,SIGNAL(clicked()),this,SLOT(onOk()));
	connect(cancel,SIGNAL(clicked()),this,SLOT(onCancel()));
}
settingExpress_dlg::~settingExpress_dlg()
{

}
void settingExpress_dlg::initDlg()
{
	colList->clear();
	funList->clear();
	
	funList->setColumnCount(1);
	funList->setRowCount(0);
	funList->insertRow(0);
	QTableWidgetItem *item=new QTableWidgetItem();
	item->setText("无");
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	funList->setItem(0,0,item);

	funList->insertRow(1);
	item=new QTableWidgetItem();
	item->setText("序号");
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	funList->setItem(1,0,item);

	funList->insertRow(2);
	item=new QTableWidgetItem();
	item->setText("求和");
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	funList->setItem(2,0,item);

	funList->insertRow(3);
	item=new QTableWidgetItem();
	item->setText("时间");
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	funList->setItem(3,0,item);

	funList->insertRow(4);
	item=new QTableWidgetItem();
	item->setText("查询条件");
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	funList->setItem(4,0,item);

	funList->insertRow(5);
	item=new QTableWidgetItem();
	item->setText("行数");
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	funList->setItem(5,0,item);

	funList->insertRow(6);
	item=new QTableWidgetItem();
	item->setText("作者");
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	funList->setItem(6,0,item);

	if(fun!="")
		curFunLabel->setText(QString("当前函数:")+fun);
	else
		curFunLabel->setText("");
	funList->verticalHeader()->setDefaultSectionSize(17);
	funList->horizontalHeader()->setHidden(true);
	funList->verticalHeader()->setHidden(true);
	funList->horizontalHeader()->setStretchLastSection(true);
	funList->setSelectionMode(QAbstractItemView::SingleSelection);

	colList->setColumnCount(2);
	colList->setRowCount(0);
	colList->horizontalHeader()->setHidden(true);
	colList->verticalHeader()->setHidden(true);
	colList->setColumnHidden(1,true);
	colList->verticalHeader()->setDefaultSectionSize(17);
	colList->horizontalHeader()->setStretchLastSection(true);
	colList->setSelectionMode(QAbstractItemView::SingleSelection);
	//int count=dbSet->GetColumnCount();
	for(int i=0;i<dbSet->GetColumnCount();i++)
	{
		colList->insertRow(i);
		QTableWidgetItem *item=new QTableWidgetItem();
		item->setText(dbSet->GetColumn(i)->m_sName.data());
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		colList->setItem(i,0,item);

		item=new QTableWidgetItem();
		item->setText(dbSet->GetRecordset()->GetColumnName(i).data());
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		colList->setItem(i,1,item);
	}
	if(col.toInt()>=0)
	{
		QTableWidgetItem *item = colList->item(col.toInt(),0);
		if(item == NULL)
			curColLabel->setText(QString("当前列:未设置"));
		else
			curColLabel->setText(QString("当前列:")+item->text());
	}
	else
		curColLabel->setText("");
}
void settingExpress_dlg::onOk()
{
	QString sColText="";
	if(colList->currentRow()>=0)
	{
		int row=colList->currentItem()->row();
		col=QString::number(row);
		sColText = colList->item(row,0)->text();
		//tab2->setTxt(col+":"+colList->currentItem()->text());
	}
	else
		col="";
	//0 无 1 序号 2 求和 3 当前时间 4 查询条件 5 条目行数 6 作者
	if(funList->selectedRanges().size()!=0&&funList->currentRow()>=0)
	{
		int row=funList->currentItem()->row();
		switch (row)
		{
		case 0:
			fun="";
			break;
		case 1:
			fun="no";
			break;
		case 2:
			fun="sum";
			break;
		case 3:
			fun="time";
			break;
		case 4:
			fun="selcon";
			break;
		case 5:
			fun="count";
			break;
		case 6:
			fun="author";
			break;
		default:
			break;
		}
	}
// 	else
// 		fun="";
	txt=tab2->getTxt();
	if(sColText.length()>0)
		txt = sColText;
	font=tab2->getFont();
	color=tab2->getColor();
	txtop=tab2->getTxtOp();
	isRect=tab2->getIsRect();
	rectPx=tab2->getRectpx();
	rectColor=tab2->getRectColor();
	hide();
}
void settingExpress_dlg::onCancel()
{
	col="";
	hide();
}
void settingExpress_dlg::setTxt(QString txt)
{
	this->txt=txt;
	tab2->setTxt(this->txt);
}
QString settingExpress_dlg::getTxt()
{
	return txt;
}
void settingExpress_dlg::setDs(CSsp_Dataset* dbSet)
{
	this->dbSet=dbSet;
}
void settingExpress_dlg::setFun(QString fun)
{
	this->fun=fun;
}
QString settingExpress_dlg::getFun()
{
	return fun;
}
void settingExpress_dlg::setExp(QString col)
{
	this->col=col;
}
QString settingExpress_dlg::getExp()
{
	return col;
}
void settingExpress_dlg::setFontSize(QString fontSize)
{
	font.setPointSize(fontSize.toInt());
	tab2->setFont(font);
}
QString settingExpress_dlg::getFontSize()
{
	int tmp=font.pointSize();
	return QString::number(tmp);
}
void settingExpress_dlg::setFontFamily(QString family)
{
	font.setFamily(family);
	tab2->setFont(font);
}
QString settingExpress_dlg::getFontFamily()
{
	return font.family();
}
void settingExpress_dlg::setColor(QColor color)
{
	this->color=color;
	tab2->setColor(color);
}
QColor settingExpress_dlg::getColor()
{
	return color;
}
void settingExpress_dlg::setFontType(int type)//0 normal 1 bold 2 italic 3 bolditalic
{
	if(type==0)
	{
		font.setBold(false);
		font.setItalic(false);
	}
	else if(type==1)
	{
		font.setBold(true);
		font.setItalic(false);
	}
	else if(type==2)
	{
		font.setBold(false);
		font.setItalic(true);
	}
	else if(type==3)
	{
		font.setBold(true);
		font.setItalic(true);
	}
}
int settingExpress_dlg::getFontType()
{
	if(!font.bold()&&!font.italic())
		return 0;
	else if(font.bold()&&!font.italic())
		return 1;
	else if(!font.bold()&&font.italic())
		return 2;
	else if(font.bold()&&font.italic())
		return 3;
	return 0;
}
void settingExpress_dlg::setTxtOp(int op)//0 center 1 left 2 right
{
	txtop=op;
	tab2->setTxtOp(op);
}
int settingExpress_dlg::getTxtOp()
{
	return txtop;
}
void settingExpress_dlg::setFontDelLine(bool delLine)
{
	if(delLine)
		font.setStrikeOut(true);
	else
		font.setStrikeOut(false);
	tab2->setFont(font);
}
bool settingExpress_dlg::getFontDelLine()
{
	return font.strikeOut();
}
void settingExpress_dlg::setFontUndLine(bool undLine)
{
	if(undLine)
		font.setUnderline(true);
	else
		font.setUnderline(false);
	tab2->setFont(font);
}
bool settingExpress_dlg::getFontUndLine()
{
	return font.underline();
}
int settingExpress_dlg::getIsRect()
{
	return isRect;
}
void settingExpress_dlg::setIsRect(int isRect)
{
	this->isRect=isRect;
	tab2->setIsRect(isRect);
}
int settingExpress_dlg::getRectpx()
{
	return rectPx;
}
void settingExpress_dlg::setRectpx(int px)
{
	rectPx=px;
	tab2->setRectpx(rectPx);
}
QColor settingExpress_dlg::getRectColor()
{
	return rectColor;
}
void settingExpress_dlg::setRectColor(QColor color)
{
	rectColor=color;
	tab2->setRectColor(color);
}
////////////////////////////////////////////////////////////////////////////////////
//虚线框
static void qt_graphicsItem_highlightSelected(QGraphicsItem *item, QPainter *painter, const QStyleOptionGraphicsItem *option)
{
	const QRectF murect = painter->transform().mapRect(QRectF(0, 0, 1, 1));
	if (qFuzzyIsNull(qMax(murect.width(), murect.height())))
		return;

	const QRectF mbrect = painter->transform().mapRect(item->boundingRect());
	if (qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
		return;

	qreal itemPenWidth=1.0;

	const qreal pad = itemPenWidth / 2;

	//const qreal penWidth = 0; // cosmetic pen

	const QColor fgcolor = option->palette.windowText().color();
	const QColor bgcolor( // ensure good contrast against fgcolor
		fgcolor.red()   > 127 ? 0 : 255,
		fgcolor.green() > 127 ? 0 : 255,
		fgcolor.blue()  > 127 ? 0 : 255);

// 	painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
// 	painter->setBrush(Qt::NoBrush);
// 	painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));

	painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
	painter->setBrush(Qt::NoBrush);
	painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));
}
///////////////////////////////////////////////////////////////////////////////////////
settingGroup_dlg::settingGroup_dlg(int dsNo,QWidget *parent)
	: QDialog(parent)
{
	//分组设置对话框
	setWindowTitle(tr("组设置"));

	colLabel=new QLabel(this);
	curColLabel=new QLabel(this);
	colLabel->setText("列设置");
	colList=new QTableWidget(this);

	tabWidget=new QTabWidget(this);
	tab1=new QWidget(this);
	tab2=new txtsetting_widget(this);
	tab2->setTwoTxt(true);

	delGrp=new QPushButton(this);
	delGrp->setText(tr("删除分组(&D)"));
	QIcon icon2;
	icon2.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165933865.gif"), QSize(), QIcon::Normal, QIcon::Off);
	delGrp->setIcon(icon2);

	delGrp->setMaximumHeight(24);
	Ok=new QPushButton(this);
	Ok->setText(tr("确定(&O)"));
	Ok->setMaximumHeight(24);
	Ok->setDefault(true);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/image/ok"), QSize(), QIcon::Normal, QIcon::Off);
	Ok->setIcon(icon);
	cancel=new QPushButton(this);
	cancel->setText(tr("取消(&C)"));
	cancel->setMaximumHeight(24);
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/image/cancel"), QSize(), QIcon::Normal, QIcon::Off);
	cancel->setIcon(icon1);

	QHBoxLayout *labelcollayout=new QHBoxLayout();
	QVBoxLayout *collayout=new QVBoxLayout();
	QHBoxLayout *bottomlayout=new QHBoxLayout();
	QVBoxLayout *tablayout=new QVBoxLayout();
	QHBoxLayout *btnlayout=new QHBoxLayout();
	//QHBoxLayout *toplayout=new QHBoxLayout();

	labelcollayout->addWidget(colLabel);
	labelcollayout->addWidget(curColLabel);
	labelcollayout->addStretch();
	labelcollayout->setMargin(2);

	collayout->addLayout(labelcollayout);
	collayout->addWidget(colList);
	collayout->setMargin(2);

	bottomlayout->addLayout(collayout);
	bottomlayout->setMargin(2);

	btnlayout->addStretch();
	btnlayout->addWidget(delGrp);
	btnlayout->addWidget(Ok);
	btnlayout->addWidget(cancel);
	btnlayout->setMargin(2);

// 	tablayout->addStretch();
// 	tablayout->addLayout(btnlayout);
// 	tablayout->setMargin(2);

	tablayout->addLayout(bottomlayout);
	tablayout->setMargin(2);
	tab1->setLayout(tablayout);

	tabWidget->addTab(tab1,"列设置");
	tabWidget->addTab(tab2,"文本设置");

	m_fieldlist = new SPtrList<CSsp_DwColumn>;
	m_dataWindow = GET_DW_CFG;
	m_pOper=DB;
	this->dsNo=dsNo;
	this->col=col;

	QVBoxLayout *layout=new QVBoxLayout();
	layout->addWidget(tabWidget);
	layout->addLayout(btnlayout);
	layout->setMargin(2);
	setLayout(layout);

	//initDlg();
}
settingGroup_dlg::~settingGroup_dlg()
{

}
void settingGroup_dlg::initDlg()
{
	SString dsName=m_pOper->SelectInto(SString::toFormat("select name from t_ssp_data_window where dw_sn=%d",dsNo));
	CSsp_Dataset* dbSet=NULL;
	colList->setColumnCount(2);
	colList->setRowCount(0);
	colList->horizontalHeader()->setHidden(true);
	colList->verticalHeader()->setHidden(true);
	colList->setColumnHidden(1,true);
	colList->verticalHeader()->setDefaultSectionSize(17);
	colList->horizontalHeader()->setStretchLastSection(true);
	colList->setSelectionMode(QAbstractItemView::SingleSelection);
	dbSet=m_dataWindow->SearchDataset(dsName);
	for(int i=0;i<dbSet->GetColumnCount();i++)
	{
		colList->insertRow(i);
		QTableWidgetItem *item=new QTableWidgetItem();
		item->setText(dbSet->GetColumn(i)->m_sName.data());
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		colList->setItem(i,0,item);

		item=new QTableWidgetItem();
		item->setText(dbSet->GetRecordset()->GetColumnName(i).data());
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		colList->setItem(i,1,item);
		//colList->addItem(dbSet->GetColumn(i)->m_sName.data());
	}

	connect(delGrp,SIGNAL(clicked()),this,SLOT(onDelGrp()));
	connect(Ok,SIGNAL(clicked()),this,SLOT(onOk()));
	connect(cancel,SIGNAL(clicked()),this,SLOT(onCancel()));
}
void settingGroup_dlg::onDelGrp()
{
	emit deleteGrp();
	col="";
	hide();
}
void settingGroup_dlg::onOk()
{
	if(colList->currentRow()>=0)
		col=QString::number(colList->currentRow());

	txt=tab2->getTxt();
	font=tab2->getFont();
	color=tab2->getColor();
	txtop=tab2->getTxtOp();
	isRect=tab2->getIsRect();
	rectPx=tab2->getRectpx();
	rectColor=tab2->getRectColor();

	hide();
}
void settingGroup_dlg::onCancel()
{
	col="";
	hide();
}
void settingGroup_dlg::setDs(int dsNo)
{
	this->dsNo=dsNo;
}

void settingGroup_dlg::setGrp(QString grp)
{
	this->col=grp;
	if(grp!="")
	{
		curColLabel->setText("当前行"+grp);
		colList->setCurrentCell(grp.toInt(),0);
	}
	else
		curColLabel->setText("");

}
QString settingGroup_dlg::getGrp()
{
	return col;
}

void settingGroup_dlg::setTxt(QString txt)
{
	this->txt=txt;
	tab2->setTxt(this->txt);
}
QString settingGroup_dlg::getTxt()
{
	return txt;
}
void settingGroup_dlg::setFontSize(QString fontSize)
{
	font.setPointSize(fontSize.toInt());
	tab2->setFont(font);
}
QString settingGroup_dlg::getFontSize()
{
	int tmp=font.pointSize();
	return QString::number(tmp);
}
void settingGroup_dlg::setFontFamily(QString family)
{
	font.setFamily(family);
	tab2->setFont(font);
}
QString settingGroup_dlg::getFontFamily()
{
	return font.family();
}
void settingGroup_dlg::setColor(QColor color)
{
	this->color=color;
	tab2->setColor(color);
}
QColor settingGroup_dlg::getColor()
{
	return color;
}
void settingGroup_dlg::setFontType(int type)//0 normal 1 bold 2 italic 3 bolditalic
{
	if(type==0)
	{
		font.setBold(false);
		font.setItalic(false);
	}
	else if(type==1)
	{
		font.setBold(true);
		font.setItalic(false);
	}
	else if(type==2)
	{
		font.setBold(false);
		font.setItalic(true);
	}
	else if(type==3)
	{
		font.setBold(true);
		font.setItalic(true);
	}
}
int settingGroup_dlg::getFontType()
{
	if(!font.bold()&&!font.italic())
		return 0;
	else if(font.bold()&&!font.italic())
		return 1;
	else if(!font.bold()&&font.italic())
		return 2;
	else if(font.bold()&&font.italic())
		return 3;
	return 0;
}
void settingGroup_dlg::setTxtOp(int op)//0 center 1 left 2 right
{
	txtop=op;
	tab2->setTxtOp(op);
}
int settingGroup_dlg::getTxtOp()
{
	return txtop;
}
void settingGroup_dlg::setFontDelLine(bool delLine)
{
	if(delLine)
		font.setStrikeOut(true);
	else
		font.setStrikeOut(false);
	tab2->setFont(font);
}
bool settingGroup_dlg::getFontDelLine()
{
	return font.strikeOut();
}
void settingGroup_dlg::setFontUndLine(bool undLine)
{
	if(undLine)
		font.setUnderline(true);
	else
		font.setUnderline(false);
	tab2->setFont(font);
}
bool settingGroup_dlg::getFontUndLine()
{
	return font.underline();
}
int settingGroup_dlg::getIsRect()
{
	return isRect;
}
void settingGroup_dlg::setIsRect(int isRect)
{
	this->isRect=isRect;
	tab2->setIsRect(isRect);
}
int settingGroup_dlg::getRectpx()
{
	return rectPx;
}
void settingGroup_dlg::setRectpx(int px)
{
	rectPx=px;
	tab2->setRectpx(rectPx);
}
QColor settingGroup_dlg::getRectColor()
{
	return rectColor;
}
void settingGroup_dlg::setRectColor(QColor color)
{
	rectColor=color;
	tab2->setRectColor(color);
}
//////////////////////////////////////////////////////////////////////
pagesetting_dlg::pagesetting_dlg(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("页面设置"));

	widthLabel=new QLabel(this);
	widthLabel->setText("宽度设置");
	widthLine=new QLineEdit(this);

	heightLabel=new QLabel(this);
	heightLabel->setText("高度设置");
	heightLine=new QLineEdit(this);

	Ok=new QPushButton(this);
	Ok->setText(tr("确定(&O)"));
	Ok->setMaximumHeight(24);
	Ok->setDefault(true);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/image/ok"), QSize(), QIcon::Normal, QIcon::Off);
	Ok->setIcon(icon);
	cancel=new QPushButton(this);
	cancel->setText(tr("取消(&C)"));
	cancel->setMaximumHeight(24);
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/image/cancel"), QSize(), QIcon::Normal, QIcon::Off);
	cancel->setIcon(icon1);

	QHBoxLayout *widthlayout=new QHBoxLayout();
	QHBoxLayout *heightlayout=new QHBoxLayout();
	QVBoxLayout *bottomlayout=new QVBoxLayout();
	QHBoxLayout *btnlayout=new QHBoxLayout();

	widthlayout->addWidget(widthLabel);
	widthlayout->addWidget(widthLine);
	widthlayout->addStretch();
	widthlayout->setMargin(2);

	//widthLabel->hide();
	//widthLine->hide();
	heightlayout->addWidget(heightLabel);
	heightlayout->addWidget(heightLine);
	heightlayout->addStretch();
	heightlayout->setMargin(2);

	bottomlayout->addLayout(widthlayout);
	bottomlayout->addLayout(heightlayout);
	bottomlayout->addStretch();
	bottomlayout->setMargin(2);

	btnlayout->addStretch();
	btnlayout->addWidget(Ok);
	btnlayout->addWidget(cancel);
	btnlayout->setMargin(2);

	bottomlayout->addLayout(btnlayout);
	setLayout(bottomlayout);
	initDlg();
	connect(Ok,SIGNAL(clicked()),this,SLOT(onOk()));
	connect(cancel,SIGNAL(clicked()),this,SLOT(onCancel()));
}
pagesetting_dlg::~pagesetting_dlg()
{

}
void pagesetting_dlg::initDlg()
{
	widthLine->setText(QString::number(width));
	heightLine->setText(QString::number(height));

}
void pagesetting_dlg::onOk()
{
	height=heightLine->text().toInt();
	width=widthLine->text().toInt();
	hide();
}
void pagesetting_dlg::onCancel()
{
	hide();
}
void pagesetting_dlg::getWH(int &width,int &height)
{
	width=this->width;
	height=this->height;
}
void pagesetting_dlg::setWH(int width,int height)
{
	this->width=width;
	this->height=height;
	initDlg();
}
//////////////////////////////////////////////////////////////////////
txtsetting_widget::txtsetting_widget(QWidget *parent)
	: QWidget(parent)
{
	setWindowTitle(tr("文本设置"));
	//文本设置页面
	txtLabel=new QLabel(this);
	txtLabel->setText("文本");
	txtLine=new QLineEdit(this);

	txtLabel2=new QLabel(this);
	txtLabel2->setText("后缀");
	txtLine2=new QLineEdit(this);

	fontExampleLabel=new QLabel(this);
	fontExampleLabel->setText("示例");

	fontButton=new QPushButton(this);
	fontButton->setText("字体设置");
	colorButton=new QPushButton(this);
	colorButton->setText("颜色设置");

	txtOpCbo=new QComboBox(this);
	txtOpCbo->addItem("居中对齐");
	txtOpCbo->addItem("左对齐");
	txtOpCbo->addItem("右对齐");
	txtOpLabel=new QLabel(this);
	txtOpLabel->setText("文本位置");

	isRect=new QCheckBox("显示矩形",this);
	pxLabel=new QLabel("矩形线宽",this);
	pxLine=new QLineEdit(this);
	pxcolorButton=new QPushButton("矩形线色",this);

	QHBoxLayout *txtlayout=new QHBoxLayout();
	QHBoxLayout *fontlayout=new QHBoxLayout();
	QHBoxLayout *txtOplayout=new QHBoxLayout();
	QHBoxLayout *pxlayout=new QHBoxLayout();
	QHBoxLayout *rectlayout=new QHBoxLayout();
	QVBoxLayout *bottomlayout=new QVBoxLayout();
	QHBoxLayout *btnlayout=new QHBoxLayout();

	txtlayout->addWidget(txtLabel);
	txtlayout->addWidget(txtLine);
	txtlayout->addWidget(txtLabel2);
	txtlayout->addWidget(txtLine2);
	txtlayout->addStretch();
	txtlayout->setMargin(2);

	fontlayout->addWidget(fontExampleLabel);
	fontlayout->addWidget(fontButton);
	fontlayout->addWidget(colorButton);
	fontlayout->addStretch();
	fontlayout->setMargin(2);

	txtOplayout->addWidget(txtOpLabel);
	txtOplayout->addWidget(txtOpCbo);
	txtOplayout->addStretch();
	txtOplayout->setMargin(2);

	pxlayout->addWidget(pxLabel);
	pxlayout->addWidget(pxLine);
	pxlayout->setMargin(2);

	rectlayout->addWidget(isRect);
	rectlayout->addLayout(pxlayout);
	rectlayout->addWidget(pxcolorButton);
	rectlayout->addStretch();
	rectlayout->setMargin(2);

	bottomlayout->addLayout(txtlayout);
	bottomlayout->addLayout(fontlayout);
	bottomlayout->addLayout(txtOplayout);
	bottomlayout->addLayout(rectlayout);
	bottomlayout->addStretch();
	bottomlayout->addLayout(btnlayout);
	bottomlayout->setMargin(2);
	setLayout(bottomlayout);
	txtLabel2->hide();
	txtLine2->hide();
	twoTxt=false;
	initDlg();	
	connect(fontButton,SIGNAL(clicked()),this,SLOT(onfontButton()));
	connect(colorButton,SIGNAL(clicked()),this,SLOT(oncolorButton()));

	connect(isRect,SIGNAL(stateChanged(int)),this,SLOT(onisRectCheck()));
	connect(pxLine,SIGNAL(textChanged (QString)),this,SLOT(onpxLine(QString)));
	connect(pxcolorButton,SIGNAL(clicked()),this,SLOT(onpxcolorButton()));
}
txtsetting_widget::~txtsetting_widget()
{

}
void txtsetting_widget::initDlg()
{
	txtLine->setText(txt);
	fontExampleLabel->setFont(font);
	QPalette palette;
	palette.setColor(QPalette::WindowText, color);  
	fontExampleLabel->setPalette(palette);
	if(isRect->checkState()==Qt::Checked)
	{
		QString srgb=QString::number(rectColor.red())+","+QString::number(rectColor.green())+","+QString::number(rectColor.blue());
		QString srgb2=QString::number(this->color.red())+","+QString::number(this->color.green())+","+QString::number(this->color.blue());
		QString ss;
		ss.sprintf("border:%dpx solid rgb(%s);color:rgb(%s);",px,srgb.toStdString().data(),srgb2.toStdString().data());
		fontExampleLabel->setStyleSheet(ss);
	}
	else
	{
		QString srgb2=QString::number(this->color.red())+","+QString::number(this->color.green())+","+QString::number(this->color.blue());
		QString ss;
		ss.sprintf("color:rgb(%s);",srgb2.toStdString().data());
		fontExampleLabel->setStyleSheet(ss);
	}
}
void txtsetting_widget::setTwoTxt(bool isTwo)
{
	twoTxt=isTwo;
	if(isTwo==true)
	{
		txtLabel->setText("前缀");
		txtLabel2->setHidden(false);
		txtLine2->setHidden(false);
	}
	else
	{
		txtLabel->setText("文本");
		txtLabel2->hide();
		txtLine2->hide();
	}
}
void txtsetting_widget::onfontButton()
{
	bool ok;  
	font=QFontDialog::getFont(&ok,font,this,tr("字体对话框")); 
	fontExampleLabel->setFont(font);
}
void txtsetting_widget::oncolorButton()
{
	color = QColorDialog::getColor(color, this);  
	if(isRect->checkState()==Qt::Checked)
	{
		QString srgb=QString::number(rectColor.red())+","+QString::number(rectColor.green())+","+QString::number(rectColor.blue());
		QString srgb2=QString::number(this->color.red())+","+QString::number(this->color.green())+","+QString::number(this->color.blue());
		QString ss;
		ss.sprintf("border:%dpx solid rgb(%s);color:rgb(%s);",px,srgb.toStdString().data(),srgb2.toStdString().data());
		fontExampleLabel->setStyleSheet(ss);
	}
	else
	{
		QString srgb2=QString::number(this->color.red())+","+QString::number(this->color.green())+","+QString::number(this->color.blue());
		QString ss;
		ss.sprintf("color:rgb(%s);",srgb2.toStdString().data());
		fontExampleLabel->setStyleSheet(ss);
	}
}
void txtsetting_widget::onisRectCheck()
{
	if(((QCheckBox*)sender())->checkState()==Qt::Checked)
	{
		QString srgb=QString::number(rectColor.red())+","+QString::number(rectColor.green())+","+QString::number(rectColor.blue());
		QString srgb2=QString::number(this->color.red())+","+QString::number(this->color.green())+","+QString::number(this->color.blue());
		QString ss;
		ss.sprintf("border:%dpx solid rgb(%s);color:rgb(%s);",px,srgb.toStdString().data(),srgb2.toStdString().data());
		fontExampleLabel->setStyleSheet(ss);
	}
	else
	{
		QString srgb2=QString::number(this->color.red())+","+QString::number(this->color.green())+","+QString::number(this->color.blue());
		QString ss;
		ss.sprintf("color:rgb(%s);",srgb2.toStdString().data());
		fontExampleLabel->setStyleSheet(ss);
	}
}
void txtsetting_widget::onpxLine(QString txt)
{
	px=txt.toInt();
	if(isRect->checkState()==Qt::Checked)
	{
		QString srgb=QString::number(rectColor.red())+","+QString::number(rectColor.green())+","+QString::number(rectColor.blue());
		QString srgb2=QString::number(this->color.red())+","+QString::number(this->color.green())+","+QString::number(this->color.blue());
		QString ss;
		ss.sprintf("border:%dpx solid rgb(%s);color:rgb(%s);",px,srgb.toStdString().data(),srgb2.toStdString().data());
		fontExampleLabel->setStyleSheet(ss);
	}
	else
	{
		QString srgb2=QString::number(this->color.red())+","+QString::number(this->color.green())+","+QString::number(this->color.blue());
		QString ss;
		ss.sprintf("color:rgb(%s);",srgb2.toStdString().data());
		fontExampleLabel->setStyleSheet(ss);
	}
}
void txtsetting_widget::onpxcolorButton()
{
	rectColor = QColorDialog::getColor(rectColor, this);  
	if(rectColor.isValid()) 
	{  
		QString srgb=QString::number(rectColor.red())+","+QString::number(rectColor.green())+","+QString::number(rectColor.blue());
		if(isRect->checkState()==Qt::Checked)
		{
			QString srgb=QString::number(rectColor.red())+","+QString::number(rectColor.green())+","+QString::number(rectColor.blue());
			QString srgb2=QString::number(this->color.red())+","+QString::number(this->color.green())+","+QString::number(this->color.blue());
			QString ss;
			ss.sprintf("border:%dpx solid rgb(%s);color:rgb(%s);",px,srgb.toStdString().data(),srgb2.toStdString().data());
			fontExampleLabel->setStyleSheet(ss);
		}
		else
		{
			QString srgb2=QString::number(this->color.red())+","+QString::number(this->color.green())+","+QString::number(this->color.blue());
			QString ss;
			ss.sprintf("color:rgb(%s);",srgb2.toStdString().data());
			fontExampleLabel->setStyleSheet(ss);
		}
	}  
}
QString txtsetting_widget::getTxt()
{
	if(twoTxt==false)
		txt=txtLine->text();
	else
	{
		txt=txtLine->text()+"@"+txtLine2->text();
	}
	return txt;
}
QFont txtsetting_widget::getFont()
{
	return font;
}
QColor txtsetting_widget::getColor()
{
	return color;
}
int txtsetting_widget::getTxtOp()
{
	int txtOp=txtOpCbo->currentIndex();
	return txtOp;
}
void txtsetting_widget::setTxt(QString txt)
{
	this->txt=txt;
	if(twoTxt==false)
		txtLine->setText(txt);
	else
	{
		QStringList tmpTxt=txt.split("@");
		txtLine->setText(tmpTxt.at(0));
		if(tmpTxt.size()>1)
			txtLine2->setText(tmpTxt.at(1));
		else
			txtLine2->setText("");
	}
	
}
void txtsetting_widget::setColor(QColor color)
{
	this->color=color;
}
void txtsetting_widget::setFont(QFont font)
{
	this->font=font;
	fontExampleLabel->setFont(font);
}
void txtsetting_widget::setTxtOp(int txtOp)
{
	txtOpCbo->setCurrentIndex(txtOp);
}
int txtsetting_widget::getIsRect()
{
	if(isRect->checkState()==Qt::Checked)
		return 1;
	else
		return 0;
}
int txtsetting_widget::getRectpx()
{
	return px;
}
QColor txtsetting_widget::getRectColor()
{
	return rectColor;
}
void txtsetting_widget::setIsRect(int isRect)
{
	if(isRect==1)
		this->isRect->setCheckState(Qt::Checked);
	else
		this->isRect->setCheckState(Qt::Unchecked);
}
void txtsetting_widget::setRectpx(int px)
{
	this->px=px;
	pxLine->setText(QString::number(px));
}
void txtsetting_widget::setRectColor(QColor color)
{
	rectColor=color;
	pxLine->setText(QString::number(px));

	//QString srgb=QString::number(rectColor.red())+","+QString::number(rectColor.green())+","+QString::number(rectColor.blue());
	if(isRect->checkState()==Qt::Checked)
	{
		QString srgb=QString::number(rectColor.red())+","+QString::number(rectColor.green())+","+QString::number(rectColor.blue());
		QString srgb2=QString::number(this->color.red())+","+QString::number(this->color.green())+","+QString::number(this->color.blue());
		QString ss;
		ss.sprintf("border:%dpx solid rgb(%s);color:rgb(%s);",px,srgb.toStdString().data(),srgb2.toStdString().data());
		fontExampleLabel->setStyleSheet(ss);
	}
	else
	{
		QString srgb2=QString::number(this->color.red())+","+QString::number(this->color.green())+","+QString::number(this->color.blue());
		QString ss;
		ss.sprintf("color:rgb(%s);",srgb2.toStdString().data());
		fontExampleLabel->setStyleSheet(ss);
	}
}
////////////////////////////////////////////////////////////////////////////////////
txtsetting_dialog::txtsetting_dialog(QWidget *parent)
	: QDialog(parent)
{
	//文本设置对话框
	setWindowTitle(tr("文本设置"));
	wig=new txtsetting_widget(this);

	Ok=new QPushButton(this);
	Ok->setText(tr("确定(&O)"));
	Ok->setMaximumHeight(24);
	Ok->setDefault(true);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/image/ok"), QSize(), QIcon::Normal, QIcon::Off);
	Ok->setIcon(icon);
	cancel=new QPushButton(this);
	cancel->setText(tr("取消(&C)"));
	cancel->setMaximumHeight(24);
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/image/cancel"), QSize(), QIcon::Normal, QIcon::Off);
	cancel->setIcon(icon1);

	QVBoxLayout *bottomlayout=new QVBoxLayout();
	QHBoxLayout *btnlayout=new QHBoxLayout();

	btnlayout->addStretch();
	btnlayout->addWidget(Ok);
	btnlayout->addWidget(cancel);
	
	btnlayout->setMargin(2);

	bottomlayout->addWidget(wig);
	bottomlayout->addStretch();
	bottomlayout->addLayout(btnlayout);
	bottomlayout->setMargin(2);

	setLayout(bottomlayout);
	initDlg();	
	connect(Ok,SIGNAL(clicked()),this,SLOT(onOk()));
	connect(cancel,SIGNAL(clicked()),this,SLOT(onCancel()));

}
txtsetting_dialog::~txtsetting_dialog()
{

}
void txtsetting_dialog::initDlg()
{
	
}
void txtsetting_dialog::onOk()
{
	txt=wig->getTxt();
	font=wig->getFont();
	color=wig->getColor();
	txtop=wig->getTxtOp();
	isRect=wig->getIsRect();
	rectPx=wig->getRectpx();
	rectColor=wig->getRectColor();
	hide();
}
void txtsetting_dialog::onCancel()
{
	hide();
}
void txtsetting_dialog::setTxt(QString txt)
{
	this->txt=txt;
	wig->setTxt(this->txt);
}
QString txtsetting_dialog::getTxt()
{
	return txt;
}
void txtsetting_dialog::setFontSize(QString fontSize)
{
	font.setPointSize(fontSize.toInt());
	wig->setFont(font);
}
QString txtsetting_dialog::getFontSize()
{
	int tmp=font.pointSize();
	return QString::number(tmp);
}
void txtsetting_dialog::setFontFamily(QString family)
{
	font.setFamily(family);
	wig->setFont(font);
}
QString txtsetting_dialog::getFontFamily()
{
	return font.family();
}
void txtsetting_dialog::setColor(QColor color)
{
	this->color=color;
	wig->setColor(color);
}
QColor txtsetting_dialog::getColor()
{
	return color;
}
void txtsetting_dialog::setFontType(int type)//0 normal 1 bold 2 italic 3 bolditalic
{
	if(type==0)
	{
		font.setBold(false);
		font.setItalic(false);
	}
	else if(type==1)
	{
		font.setBold(true);
		font.setItalic(false);
	}
	else if(type==2)
	{
		font.setBold(false);
		font.setItalic(true);
	}
	else if(type==3)
	{
		font.setBold(true);
		font.setItalic(true);
	}
	wig->setFont(font);
}
int txtsetting_dialog::getFontType()
{
	if(!font.bold()&&!font.italic())
		return 0;
	else if(font.bold()&&!font.italic())
		return 1;
	else if(!font.bold()&&font.italic())
		return 2;
	else if(font.bold()&&font.italic())
		return 3;
	return 0;
}
void txtsetting_dialog::setTxtOp(int op)//0 center 1 left 2 right
{
	txtop=op;
	wig->setTxtOp(op);
}
int txtsetting_dialog::getTxtOp()
{
	return txtop;
}
void txtsetting_dialog::setFontDelLine(bool delLine)
{
	if(delLine)
		font.setStrikeOut(true);
	else
		font.setStrikeOut(false);
	wig->setFont(font);
}
bool txtsetting_dialog::getFontDelLine()
{
	return font.strikeOut();
}
void txtsetting_dialog::setFontUndLine(bool undLine)
{
	if(undLine)
		font.setUnderline(true);
	else
		font.setUnderline(false);
	wig->setFont(font);
}
bool txtsetting_dialog::getFontUndLine()
{
	return font.underline();
}
int txtsetting_dialog::getIsRect()
{
	return isRect;
}
void txtsetting_dialog::setIsRect(int isRect)
{
	this->isRect=isRect;
	wig->setIsRect(isRect);
}
int txtsetting_dialog::getRectpx()
{
	return rectPx;
}
void txtsetting_dialog::setRectpx(int px)
{
	rectPx=px;
	wig->setRectpx(rectPx);
}
QColor txtsetting_dialog::getRectColor()
{
	return rectColor;
}
void txtsetting_dialog::setRectColor(QColor color)
{
	rectColor=color;
	wig->setRectColor(color);
}
////////////////////////////////////////////////////////////////////////////////////
linesetting_dialog::linesetting_dialog(QWidget *parent)
	: QDialog(parent)
{
	//线段设置对话框
	setWindowTitle(tr("文本设置"));
	
	colorButton=new QPushButton(this);
	colorButton->setText("颜色设置");
	pxLabel=new QLabel(this);
	pxLabel->setText("线宽");
	pxLine=new QLineEdit(this);
	typeLabel=new QLabel(this);
	typeLabel->setText("类型");
	typeCbo=new QComboBox(this);

	Ok=new QPushButton(this);
	Ok->setText(tr("确定(&O)"));
	Ok->setMaximumHeight(24);
	Ok->setDefault(true);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/image/ok"), QSize(), QIcon::Normal, QIcon::Off);
	Ok->setIcon(icon);
	cancel=new QPushButton(this);
	cancel->setText(tr("取消(&C)"));
	cancel->setMaximumHeight(24);
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/image/cancel"), QSize(), QIcon::Normal, QIcon::Off);
	cancel->setIcon(icon1);

	QHBoxLayout *colorlayout=new QHBoxLayout();
	QHBoxLayout *pxlayout=new QHBoxLayout();
	QHBoxLayout *typelayout=new QHBoxLayout();
	QVBoxLayout *bottomlayout=new QVBoxLayout();
	QHBoxLayout *btnlayout=new QHBoxLayout();

	colorlayout->addWidget(colorButton);
	colorlayout->addStretch();
	colorlayout->setMargin(2);

	pxlayout->addWidget(pxLabel);
	pxlayout->addWidget(pxLine);
	pxlayout->addStretch();
	pxlayout->setMargin(2);

	typelayout->addWidget(typeLabel);
	typelayout->addWidget(typeCbo);
	typelayout->setMargin(2);

	typeCbo->addItem("实线");typeCbo->addItem("虚线");
	typeCbo->addItem("点线");typeCbo->addItem("点虚线");
	typelayout->addStretch();

	btnlayout->addWidget(Ok);
	btnlayout->addWidget(cancel);
	btnlayout->addStretch();
	btnlayout->setMargin(2);

	bottomlayout->addLayout(colorlayout);
	bottomlayout->addLayout(pxlayout);
	bottomlayout->addLayout(typelayout);
	bottomlayout->addStretch();
	bottomlayout->addLayout(btnlayout);
	bottomlayout->setMargin(2);
	setLayout(bottomlayout);
	initDlg();	
	connect(Ok,SIGNAL(clicked()),this,SLOT(onOk()));
	connect(cancel,SIGNAL(clicked()),this,SLOT(onCancel()));
	connect(colorButton,SIGNAL(clicked()),this,SLOT(onColorButton()));
}
linesetting_dialog::~linesetting_dialog()
{

}
void linesetting_dialog::initDlg()
{

}
void linesetting_dialog::onOk()
{
	color=tmpColor;
	px=pxLine->text().toInt();
// 	if(px<2)
// 		px=2;
	type=typeCbo->currentIndex();
	hide();
}
void linesetting_dialog::onCancel()
{
	hide();
}
void linesetting_dialog::onColorButton()
{
	tmpColor=QColorDialog::getColor(tmpColor,this);  
}
void linesetting_dialog::setType(int type)
{
	this->type=type;
	typeCbo->setCurrentIndex(type);
}
int linesetting_dialog::getType()
{
	return type;
}
void linesetting_dialog::setColor(QColor color)
{
	this->color=color;
	tmpColor=color;
}
QColor linesetting_dialog::getColor()
{
	return color;
}
int linesetting_dialog::getpx()
{
	return px;
}
void linesetting_dialog::setpx(int px)
{
	this->px=px;
	pxLine->setText(QString::number(px));
}
/////////////////////////////////////////////////////////////////////////////////////////
chartsetting_dialog::chartsetting_dialog(QWidget *parent)
	: QDialog(parent)
{
	//图标设置对话框
	setWindowTitle(tr("数据窗口选择"));
	dsLabel=new QLabel(this);
	dsLabel->setText(tr("选择数据窗口"));
	curDsLabel=new QLabel(this);
	tableWidget=new QTableWidget(this);
	m_pOper=DB;
	chartType=0;

	Ok=new QPushButton(this);
	Ok->setText(tr("确定(&O)"));
	Ok->setMaximumHeight(24);
	Ok->setDefault(true);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/image/ok"), QSize(), QIcon::Normal, QIcon::Off);
	Ok->setIcon(icon);
	cancel=new QPushButton(this);
	cancel->setText(tr("取消(&C)"));
	cancel->setMaximumHeight(24);
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/image/cancel"), QSize(), QIcon::Normal, QIcon::Off);
	cancel->setIcon(icon1);

	txtLabel=new QLabel(this);
	txtLabel->setText(tr("文本"));
	txtLine=new QLineEdit(this);

	typeLabel=new QLabel(this);
	typeLabel->setText(tr("图表类型"));
	typeCbo=new QComboBox(this);
	typeCbo->addItem("饼图");
	typeCbo->addItem("柱图");
	typeCbo->addItem("折线图");
	typeCbo->addItem("仪表盘");//pointRow.pointCol标识指针刻度

	extatuLabel=new QLabel(this);
	extatuLabel->setText("扩展属性");
	extatuEdit=new QTextEdit(this);

	QVBoxLayout *labelsqllayout=new QVBoxLayout();
	QVBoxLayout *sqllayout=new QVBoxLayout();
	QHBoxLayout *txtlayout=new QHBoxLayout();
	QHBoxLayout *typelayout=new QHBoxLayout();
	QHBoxLayout *atulayout=new QHBoxLayout();
	QVBoxLayout *rightlayout=new QVBoxLayout();
	QHBoxLayout *toplayout=new QHBoxLayout();
	QHBoxLayout *btnlayout=new QHBoxLayout();

	labelsqllayout->addWidget(dsLabel);
	labelsqllayout->addStretch();
	labelsqllayout->addWidget(curDsLabel);
	labelsqllayout->setMargin(2);

	sqllayout->addLayout(labelsqllayout);
	sqllayout->addWidget(tableWidget);
	sqllayout->setMargin(2);

	txtlayout->addWidget(txtLabel);
	txtlayout->addWidget(txtLine);
	txtlayout->addStretch();
	txtlayout->setMargin(2);

	typelayout->addWidget(typeLabel);
	typelayout->addWidget(typeCbo);
	typelayout->addStretch();
	typelayout->setMargin(2);

	atulayout->addWidget(extatuLabel);
	atulayout->addWidget(extatuEdit);

	rightlayout->addLayout(txtlayout);
	rightlayout->addLayout(typelayout);
	rightlayout->addLayout(atulayout);
	rightlayout->addStretch();
	rightlayout->setMargin(2);

	toplayout->addLayout(sqllayout);
	toplayout->addLayout(rightlayout);
	toplayout->setMargin(2);

	btnlayout->addStretch();
	btnlayout->addWidget(Ok);
	btnlayout->addWidget(cancel);
	btnlayout->setMargin(2);
	QVBoxLayout *layout=new QVBoxLayout();
	layout->addLayout(toplayout);
	layout->addLayout(btnlayout);
	layout->setMargin(2);
	setLayout(layout);
	initDlg();
}
chartsetting_dialog::~chartsetting_dialog()
{

}
void chartsetting_dialog::initDlg()//设置表格内容
{
	tableWidget->clear();
	QStringList header;
	header<<tr("序号")<<tr("名称")<<tr("描述");
	tableWidget->setColumnCount(3);
	tableWidget->setRowCount(0);
	tableWidget->setHorizontalHeaderLabels(header);
	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	tableWidget->setColumnWidth(0,50);
	tableWidget->setColumnWidth(1,100);
	tableWidget->setColumnWidth(2,200);
	tableWidget->setMinimumWidth(350);
	tableWidget->verticalHeader()->setDefaultSectionSize(17);
	tableWidget->horizontalHeader()->setStretchLastSection(true);
	tableWidget->horizontalHeader()->setAutoScroll(true);
	tableWidget->verticalHeader()->setAutoScroll(true);
	SRecordset dsSet;
	m_pOper->RetrieveRecordset("select dw_sn,dstype,name,dw_desc from t_ssp_data_window order by cls_name desc,dw_desc",dsSet);

	for(int i=0;i<dsSet.GetRows();i++)
	{
		if(dsSet.GetValue(i,1).toInt()!=1)
		{
			int rowCount=tableWidget->rowCount();
			tableWidget->insertRow(rowCount);

			SString tmp=dsSet.GetValue(i,0).data();
			QTableWidgetItem *item=new QTableWidgetItem();
			item->setText(tmp.data());
			item->setFlags(item->flags()&~Qt::ItemIsEditable);
			tableWidget->setItem(rowCount,0,item);

			tmp=dsSet.GetValue(i,2).data();
			item=new QTableWidgetItem();
			item->setText(tmp.data());
			item->setFlags(item->flags()&~Qt::ItemIsEditable);
			tableWidget->setItem(rowCount,1,item);

			tmp=dsSet.GetValue(i,3).data();
			item=new QTableWidgetItem();
			item->setText(tmp.data());
			item->setFlags(item->flags()&~Qt::ItemIsEditable);
			tableWidget->setItem(rowCount,2,item);
		}
	}
	txtLine->setText(txt);
	typeCbo->setCurrentIndex(chartType);
	
	connect(Ok,SIGNAL(clicked()),this,SLOT(onOk()));
	connect(cancel,SIGNAL(clicked()),this,SLOT(onCancel()));
}
void chartsetting_dialog::onOk()
{
	if(tableWidget->selectedItems().size()>0)
	{
		int row=tableWidget->selectedItems().at(0)->row();
		chartDs=tableWidget->item(row,0)->text().toInt();
	}
	txt=txtLine->text();
	extAtu=extatuEdit->toPlainText();
	extAtu.replace("=",":");//替换符号，避免冲突
	extAtu.replace(";",".");
	chartType=typeCbo->currentIndex();
	hide();
}
void chartsetting_dialog::onCancel()
{
	extAtu.replace("=",":");//替换符号，避免冲突
	extAtu.replace(";",".");
	hide();
}
void chartsetting_dialog::setExtAtu(QString extAtu)
{
	extAtu.replace(":","=");//替换符号，避免冲突
	extAtu.replace(".",";");
	this->extAtu=extAtu;
	extatuEdit->setText(extAtu);
}
QString chartsetting_dialog::getExtAtu()
{
	return extAtu;
}
void chartsetting_dialog::setTxt(QString txt)
{
	this->txt=txt;
	txtLine->setText(txt);
}
QString chartsetting_dialog::getTxt()
{
	return txt;
}

void chartsetting_dialog::setType(int type)
{
	chartType=type;
	typeCbo->setCurrentIndex(chartType);
}
int chartsetting_dialog::getType()
{
	return chartType;
}

int chartsetting_dialog::getDs()
{
	return chartDs;
}
void chartsetting_dialog::setDs(int ds)
{
	chartDs=ds;
	curDsLabel->setText(QString("当前序号:%1").arg(chartDs));
}