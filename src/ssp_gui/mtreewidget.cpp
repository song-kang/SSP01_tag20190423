#include "mtreewidget.h"
#include "SDateTime.h"
#include "ssp_gui.h"
#include "mpushbutton.h"
#define  MTREEWIDGET_COL_IMG_NORMAL        8
#define  MTREEWIDGET_COL_IMG_DOWN          9
#define  MTREEWIDGET_COL_IMG_HOVER         10
#define  MTREEWIDGET_COL_IMG_DISABLE       11
mTreeWidget::mTreeWidget(QWidget *parent)
	: QTreeWidget(parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	m_citem = NULL;
	m_pitem = NULL;
	m_bRmenu = false;
	m_bCheck = false;
	m_bManual = false;
	m_bresized = false;
	m_choosed = NULL;
	m_rcMenu=NULL;
	mpcomb=NULL;
	m_dbOper = DB;
	widget = NULL;
	QObject::connect(this,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(onItemChanged(QTreeWidgetItem*,int)));
	QObject::connect(this,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(onCustomContextMenuRequested(const QPoint&)));
	QObject::connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(OnItemDoubleClicked(QTreeWidgetItem *,int)));
	QObject::connect(this,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),this,SLOT(OnCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
	
}

mTreeWidget::~mTreeWidget()
{

}
void mTreeWidget::OnCurrentItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous )
{
	if (!m_bEdit)
	{
		return;
	}
	if (current == NULL)
	{
		return;
	}
	QString strPreType;
	QString strRefValue;
	strPreType = current->text(4);
	strRefValue = current->text(5);

	if (previous!=NULL)
	{
		this->removeItemWidget(previous,4);
		this->removeItemWidget(previous,5);
		this->removeItemWidget(previous,MTREEWIDGET_COL_IMG_NORMAL);
		previous->setText(MTREEWIDGET_COL_IMG_NORMAL,"<BLOB>");

		this->removeItemWidget(previous,MTREEWIDGET_COL_IMG_DOWN);
		previous->setText(MTREEWIDGET_COL_IMG_DOWN,"<BLOB>");

		this->removeItemWidget(previous,MTREEWIDGET_COL_IMG_HOVER);
		previous->setText(MTREEWIDGET_COL_IMG_HOVER,"<BLOB>");

		this->removeItemWidget(previous,MTREEWIDGET_COL_IMG_DISABLE);
		previous->setText(MTREEWIDGET_COL_IMG_DISABLE,"<BLOB>");
	}

	mpcomb=new mCombobox(0,1);
	mpcomb->installEventFilter(this);
	mpcomb->setMinimumHeight(23);
	mpcomb->addItem(tr("静态功能点"));
	mpcomb->addItem(tr("动态菜单项"));
	mpcomb->addItem(tr("组态界面功能点"));
	mpcomb->addItem(tr("动态报表功能点"));
	mpcomb->addItem(tr("隐式功能点"));
	mpcomb->addItem(tr("WEB页面"));
	mpcomb->addItem(tr("JAVA三维场景界面类"));
	int i_index=-1;
	if (current->text(4)=="静态功能点")
		i_index=0;
	else if (current->text(4)=="动态菜单项")
		i_index=1;
	else if (current->text(4)=="组态界面功能点")
		i_index=2;
	else if (current->text(4)=="动态报表功能点")
		i_index=3;
	else if (current->text(4)=="隐式功能点")
		i_index=4;
	else if (current->text(4)=="WEB页面")
		i_index=5;
	else if (current->text(4)=="JAVA三维场景界面类")
		i_index=6;
	mpcomb->setTreeItemAndColumn(current,4);
	mpcomb->setCurrentIndex(i_index);
	QObject::connect(mpcomb,SIGNAL(whitch(QTreeWidgetItem*,int)),this,SLOT(oneCellChanged(QTreeWidgetItem*,int)));
	this->setItemWidget(current,4,mpcomb);
	mpcomb->setFocusProxy(this->parentWidget());

	if (i_index==2)
	{
		mCombobox * refNumComb=getRefNum();
		if(refNumComb == NULL)
			return;
		refNumComb->installEventFilter(this);
		if (!strRefValue.isEmpty())
		{
			int iIndex = refNumComb->findText(strRefValue);
			refNumComb->setCurrentIndex(iIndex);
		}
		
		refNumComb->setTreeItemAndColumn(current,5);
		//setRefIndex(refNumComb,current->text(5));
		QObject::connect(refNumComb,SIGNAL(whitch(QTreeWidgetItem*,int)),this,SLOT(oneCellChanged(QTreeWidgetItem*,int)));
		this->setItemWidget(current,5,refNumComb);
		refNumComb->setFocusProxy(this->parentWidget());
		resizeColumnToContents(5);
	}
	if (i_index == 3)
	{
		mCombobox * reportNumComb=getReportNum();
		if(reportNumComb == NULL)
			return;
		reportNumComb->installEventFilter(this);
		
		reportNumComb->setTreeItemAndColumn(current,5);
		if (!strRefValue.isEmpty())
		{
			int iIndex = reportNumComb->findText(strRefValue);
			reportNumComb->setCurrentIndex(iIndex);
		}
		//setRefIndex(reportNumComb,current->text(5));
		QObject::connect(reportNumComb,SIGNAL(whitch(QTreeWidgetItem*,int)),this,SLOT(oneCellChanged(QTreeWidgetItem*,int)));
		this->setItemWidget(current,5,reportNumComb);
		reportNumComb->setFocusProxy(this->parentWidget());
		resizeColumnToContents(5);
	}
	QString a = current->text(0);
	widget=new QWidget(this);
	QGridLayout *gridLayout;
	mPushButton *uploadRowBtn;
	mPushButton *downloadRowBtn;

	gridLayout = new QGridLayout(widget);
	gridLayout->setSpacing(0);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	uploadRowBtn = new mPushButton(widget);
	uploadRowBtn->setName(a);
	uploadRowBtn->setColID(MTREEWIDGET_COL_IMG_NORMAL);
	uploadRowBtn->setObjectName(QString::fromUtf8("pushButton"));
	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(uploadRowBtn->sizePolicy().hasHeightForWidth());
	uploadRowBtn->setSizePolicy(sizePolicy);
	gridLayout->addWidget(uploadRowBtn, 0, 0, 1, 1);

	downloadRowBtn = new mPushButton(widget);
	downloadRowBtn->setName(a);
	downloadRowBtn->setColID(MTREEWIDGET_COL_IMG_NORMAL);
	downloadRowBtn->setObjectName(QString::fromUtf8("pushButton_2"));
	sizePolicy.setHeightForWidth(downloadRowBtn->sizePolicy().hasHeightForWidth());
	downloadRowBtn->setSizePolicy(sizePolicy);
	gridLayout->addWidget(downloadRowBtn, 0, 1, 1, 1);

	uploadRowBtn->setText(tr("上传..."));
	downloadRowBtn->setText(tr("下载..."));
	widget->setLayout(gridLayout); 
	connect(uploadRowBtn,SIGNAL(clicked()),this,SLOT(slotupLoadRowClicked()));
	connect(downloadRowBtn,SIGNAL(clicked()),this,SLOT(slotDownLoadRowClicked()));
	this->setItemWidget(current,MTREEWIDGET_COL_IMG_NORMAL,widget);
	current->setText(MTREEWIDGET_COL_IMG_NORMAL,"");
	resizeColumnToContents(MTREEWIDGET_COL_IMG_NORMAL);


	widget=new QWidget(this);
	gridLayout = new QGridLayout(widget);
	gridLayout->setSpacing(0);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	uploadRowBtn = new mPushButton(widget);
	uploadRowBtn->setName(a);
	uploadRowBtn->setColID(MTREEWIDGET_COL_IMG_DOWN);
	uploadRowBtn->setObjectName(QString::fromUtf8("pushButton"));
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(uploadRowBtn->sizePolicy().hasHeightForWidth());
	uploadRowBtn->setSizePolicy(sizePolicy);
	gridLayout->addWidget(uploadRowBtn, 0, 0, 1, 1);
	downloadRowBtn = new mPushButton(widget);
	downloadRowBtn->setName(a);
	downloadRowBtn->setColID(MTREEWIDGET_COL_IMG_DOWN);
	downloadRowBtn->setObjectName(QString::fromUtf8("pushButton_2"));
	sizePolicy.setHeightForWidth(downloadRowBtn->sizePolicy().hasHeightForWidth());
	downloadRowBtn->setSizePolicy(sizePolicy);
	gridLayout->addWidget(downloadRowBtn, 0, 1, 1, 1);
	uploadRowBtn->setText(tr("上传..."));
	downloadRowBtn->setText(tr("下载..."));
	widget->setLayout(gridLayout); 
	connect(uploadRowBtn,SIGNAL(clicked()),this,SLOT(slotupLoadRowClicked()));
	connect(downloadRowBtn,SIGNAL(clicked()),this,SLOT(slotDownLoadRowClicked()));
	this->setItemWidget(current,MTREEWIDGET_COL_IMG_DOWN,widget);
	current->setText(MTREEWIDGET_COL_IMG_DOWN,"");
	resizeColumnToContents(MTREEWIDGET_COL_IMG_DOWN);

	widget=new QWidget(this);
	gridLayout = new QGridLayout(widget);
	gridLayout->setSpacing(0);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	uploadRowBtn = new mPushButton(widget);
	uploadRowBtn->setName(a);
	uploadRowBtn->setColID(MTREEWIDGET_COL_IMG_HOVER);
	uploadRowBtn->setObjectName(QString::fromUtf8("pushButton"));
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(uploadRowBtn->sizePolicy().hasHeightForWidth());
	uploadRowBtn->setSizePolicy(sizePolicy);
	gridLayout->addWidget(uploadRowBtn, 0, 0, 1, 1);
	downloadRowBtn = new mPushButton(widget);
	downloadRowBtn->setName(a);
	downloadRowBtn->setColID(MTREEWIDGET_COL_IMG_HOVER);
	downloadRowBtn->setObjectName(QString::fromUtf8("pushButton_2"));
	sizePolicy.setHeightForWidth(downloadRowBtn->sizePolicy().hasHeightForWidth());
	downloadRowBtn->setSizePolicy(sizePolicy);
	gridLayout->addWidget(downloadRowBtn, 0, 1, 1, 1);
	uploadRowBtn->setText(tr("上传..."));
	downloadRowBtn->setText(tr("下载..."));
	widget->setLayout(gridLayout); 
	connect(uploadRowBtn,SIGNAL(clicked()),this,SLOT(slotupLoadRowClicked()));
	connect(downloadRowBtn,SIGNAL(clicked()),this,SLOT(slotDownLoadRowClicked()));
	this->setItemWidget(current,MTREEWIDGET_COL_IMG_HOVER,widget);
	current->setText(MTREEWIDGET_COL_IMG_HOVER,"");
	resizeColumnToContents(MTREEWIDGET_COL_IMG_HOVER);

	widget=new QWidget(this);
	gridLayout = new QGridLayout(widget);
	gridLayout->setSpacing(0);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	uploadRowBtn = new mPushButton(widget);
	uploadRowBtn->setName(a);
	uploadRowBtn->setColID(MTREEWIDGET_COL_IMG_DISABLE);
	uploadRowBtn->setObjectName(QString::fromUtf8("pushButton"));
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(uploadRowBtn->sizePolicy().hasHeightForWidth());
	uploadRowBtn->setSizePolicy(sizePolicy);
	gridLayout->addWidget(uploadRowBtn, 0, 0, 1, 1);
	downloadRowBtn = new mPushButton(widget);
	downloadRowBtn->setName(a);
	downloadRowBtn->setColID(MTREEWIDGET_COL_IMG_DISABLE);
	downloadRowBtn->setObjectName(QString::fromUtf8("pushButton_2"));
	sizePolicy.setHeightForWidth(downloadRowBtn->sizePolicy().hasHeightForWidth());
	downloadRowBtn->setSizePolicy(sizePolicy);
	gridLayout->addWidget(downloadRowBtn, 0, 1, 1, 1);
	uploadRowBtn->setText(tr("上传..."));
	downloadRowBtn->setText(tr("下载..."));
	widget->setLayout(gridLayout); 
	connect(uploadRowBtn,SIGNAL(clicked()),this,SLOT(slotupLoadRowClicked()));
	connect(downloadRowBtn,SIGNAL(clicked()),this,SLOT(slotDownLoadRowClicked()));
	this->setItemWidget(current,MTREEWIDGET_COL_IMG_DISABLE,widget);
	current->setText(MTREEWIDGET_COL_IMG_DISABLE,"");
	resizeColumnToContents(MTREEWIDGET_COL_IMG_DISABLE);
}
void mTreeWidget::onItemChanged(QTreeWidgetItem * item, int column)
{
	if (!m_bCheck)
	{
		if (column==3||column==4||column==5)
		{
			if (m_bManual)
			{
				emit updateNode(item,"",column);
			}
		}
		return;
	}
		
	if (m_bManual)
	{
		emit updateCheck(item,column);
		if (item->parent()!=NULL)
		{
			if (item->checkState(column)==Qt::Checked)
			{
				item->parent()->setCheckState(column,Qt::Checked);
			}
		}
	}
}
QTreeWidgetItem * mTreeWidget::chooseItem(int topIndex,QString sText)
{
	m_choosed=NULL;
	if (topIndex>=topLevelItemCount())
	{
		return NULL;
	}
	QTreeWidgetItem * topItem = topLevelItem(topIndex);
	m_choosed=NULL;
	QTreeWidgetItem * rItem = chooseItem(topItem,sText);
	return rItem;

}
QTreeWidgetItem * mTreeWidget::chooseItem(QTreeWidgetItem *topItem,QString sText)
{
	int iCount = topItem->childCount();
	if ((m_rcMenu?topItem->text(0):topItem->data(0,5).toString())==sText)
	{
		m_choosed=topItem;
	}
	for (int i = 0;i<iCount;i++)
	{
		if ((m_bRmenu?topItem->child(i)->text(0):topItem->child(i)->data(0,5).toString())!=sText)
		{
			if (topItem->child(i)->childCount()>0)
			{
				chooseItem(topItem->child(i),sText);
			}
		}else
		{
			m_choosed = topItem->child(i);
		}
	}
	return m_choosed;
}
void mTreeWidget::addData(SRecordset &rs,SString sData,QTreeWidgetItem *item)
{
	int iCount =rs.GetColumns()-1;
	for (int i = 0;i<iCount;i++)
	{
		if (m_bRmenu)
		{
			if (i==0)
			{
				item->setText(i,sData.data());
			}else
			{
				//item->setText(i,rs.GetValue(0,i+1).data());
				if (i<(rs.GetColumns()-5))
				{
					item->setText(i,rs.GetValue(0,i+1).data());
				}
				else
				{
					item->setText(i,"<BLOB>");
				}
			}
		}
		if (m_bCheck&&!m_bEdit)
		{
			item->setData(i,5,QVariant(sData.data()));
			item->setText(i,rs.GetValue(0,2).data());
			break;		
		}
	}
	item->setExpanded(true);
	if (m_bCheck&&!m_bEdit)
	{
		item->setCheckState(1,Qt::Unchecked);
		return;
	}
	
	setColumnToCheck(item,3);
	setColumnToRef(item,4);
	setColumnToUicfgOrReportRef(item,5);
}
void mTreeWidget::onCustomContextMenuRequested(const QPoint &pos)
{
	m_citem = NULL;
	m_pitem = NULL;
	if (!m_bRmenu)
		return;
	m_citem =  itemAt(pos);
	if(m_citem!=NULL)
		m_pitem = m_citem->parent();
	else
	{
		return;
	}
	if (m_rcMenu==NULL)
	{
		m_rcMenu = new QMenu(this);
		m_addNode = new QAction(tr("添加"),0);
		m_rcMenu->addAction(m_addNode);
		QObject::connect(m_addNode,SIGNAL(triggered()),this,SLOT(OnAddNode()));	
		m_delNode = new QAction(tr("删除"),0);
		m_rcMenu->addAction(m_delNode);
		QObject::connect(m_delNode,SIGNAL(triggered()),this,SLOT(OnDelNode()));	
		m_editNode = new QAction(tr("编辑"),0);
		m_rcMenu->addAction(m_editNode);
		QObject::connect(m_editNode,SIGNAL(triggered()),this,SLOT(OnEditNode()));	
	}
	m_rcMenu->clear();
	if (getLevel(m_citem)<3)
	{
		m_rcMenu->addAction(m_addNode);
	}
	m_rcMenu->addAction(m_delNode);
	m_rcMenu->addAction(m_editNode);
	m_rcMenu->exec(QCursor::pos());
}
void mTreeWidget::OnItemDoubleClicked(QTreeWidgetItem * item, int column)
{
	if (!m_bEdit||column==3||column==4||column==MTREEWIDGET_COL_IMG_NORMAL||column==MTREEWIDGET_COL_IMG_DOWN||column==MTREEWIDGET_COL_IMG_HOVER||column==MTREEWIDGET_COL_IMG_DISABLE)
	{
		return;
	}
	if (column == 5)
	{
		QString strType = item->text(4);
		if (strType == "组态界面功能点" ||
			strType == "动态报表功能点")
		{
			return;
		}
	}
	m_clickCloumn = column;
	//m_citem = item;
	//m_pitem = item->parent();
	QLineEdit *lineEdit=new QLineEdit(this);  
	connect(lineEdit,SIGNAL(editingFinished()),this,SLOT(finishEdit()));  

	// 设置widget  
	this->setItemWidget(item,column,lineEdit);  
	this->expandItem(item);


	// 选中项  
	this->setCurrentItem(item,column);  
	// 设置默认值  
	lineEdit->setText(item->text(column)); 
	lineEdit->selectAll();
	lineEdit->setFocus();
	m_addOrUpdate = false;
}
void mTreeWidget::emptyCheck(int topIndex)
{
	if (topIndex>=topLevelItemCount())
	{
		return ;
	}
	QTreeWidgetItem * topItem = topLevelItem(topIndex);
	topItem->setDisabled(false);
	emptyCheck(topItem);
}
void mTreeWidget::emptyCheck(QTreeWidgetItem * item)
{
	item->setCheckState(1,Qt::Unchecked);
	item->setFlags(item->flags() |Qt::ItemIsUserCheckable);
	item->setTextColor(1,Qt::black);
	item->setTextColor(0,Qt::black);
	for(int i =0;i<item->childCount();i++)
	{
		if (item->child(i)->childCount()==0)
		{
			item->child(i)->setCheckState(1,Qt::Unchecked);
			item->child(i)->setFlags(item->child(i)->flags() |Qt::ItemIsUserCheckable);
			item->child(i)->setTextColor(0,Qt::black);
			item->child(i)->setTextColor(1,Qt::black);
		}else
		{
			emptyCheck(item->child(i));
		}	
	}
}
void mTreeWidget::OnAddNode()
{
	setManual(false);
	if (m_citem==NULL)
	{
		return;
	}
	QTreeWidgetItem * aitem = new QTreeWidgetItem(m_citem);
	setColumnToCheck(aitem,3);
	setColumnToRef(aitem,4);
	aitem->setExpanded(true);
	//aitem->setFlags(aitem->flags()|Qt::ItemIsEditable); 
	this->setCurrentItem(aitem);
	//this->editItem(aitem, 0);  
  
	this->setColumnWidth(0,this->columnWidth(0)+25);
	// 使用QTextEdit的信号  
	QLineEdit *lineEdit=new QLineEdit(this);  
	connect(lineEdit,SIGNAL(editingFinished()),this,SLOT(finishEdit()));  

	// 设置widget  
	this->setItemWidget(aitem,0,lineEdit);  
	this->expandItem(aitem);


	// 选中项  
	this->setCurrentItem(aitem,0);  
	// 设置默认值  
	lineEdit->setText("default");  
	lineEdit->setSelection(0,7);
	lineEdit->setFocus();
	m_addOrUpdate=true;
	setManual(true);
}
void mTreeWidget::OnDelNode()
{
	if (m_pitem==NULL)
	{
		return;
	}
	emit delNode(m_citem,m_pitem);
	delete m_citem;
}

void mTreeWidget::OnEditNode()
{
	if (m_pitem==NULL)
	{
		return;
	}
	emit editNode(m_citem,m_pitem);;
}

void mTreeWidget::finishEdit()
{
	QTreeWidgetItem *curItem=this->currentItem(); 
	QString text;
	if (m_addOrUpdate)
	{
		QLineEdit *edit=qobject_cast<QLineEdit*>(this->itemWidget(curItem,0));  
		if(!edit)  
			return;  
		text=edit->text();

		this->removeItemWidget(curItem,0);
		if (text=="")
		{
			QMessageBox::information(NULL,tr("提示"),tr("功能点不能为空！"));
			delete curItem;
			return;
		}
		if (chooseItem(0,text)!=NULL)
		{
			QMessageBox::information(NULL,tr("提示"),tr("该功能点已存在，重新添加！"));
			delete curItem;
			return;
		}

		curItem->setText(0,text);
		// 移除widget  
		emit addNode(curItem,curItem->parent());
			this->resizeColumnToContents(0);
	}else
	{
		QLineEdit *edit=qobject_cast<QLineEdit*>(this->itemWidget(curItem,m_clickCloumn));  
		if(!edit)  
			return;  
		text=edit->text();
		// 移除widget  
		this->removeItemWidget(curItem,m_clickCloumn);
		//this->setFocus();

		if (text==""&&m_clickCloumn==0)
		{
			this->setCurrentItem(curItem);
			return;
		}
		if (curItem->text(m_clickCloumn)!=text)
		{
			SString sPre = curItem->text(0).toStdString();
			if (m_clickCloumn==0)
			{
				if (chooseItem(0,text)==NULL)
				{
					curItem->setText(m_clickCloumn,text);
					emit updateNode(curItem,sPre,m_clickCloumn);

				}					
			}else
			{
				curItem->setText(m_clickCloumn,text);
				emit updateNode(curItem,sPre,m_clickCloumn);
			}

			this->resizeColumnToContents(0);
		}
		//this->resizeColumnToContents(m_clickCloumn);
	}

	//this->setStyleSheet("QTreeWidget::item{height:20px}");
}
void mTreeWidget::setColumnToCheck(QTreeWidgetItem * item,int column)
{
	QString sVlue=item->text(column);
	if ((sVlue=="")||(sVlue=="0"))
	{
		item->setCheckState(column,Qt::Unchecked);
	}else if (sVlue=="1")
	{
		item->setCheckState(column,Qt::Checked);
	}
	item->setText(column,tr("需认证"));
}
void mTreeWidget::setColumnToRef(QTreeWidgetItem * item,int column)
{
	int i_index=-1;
	if (item->text(column)!="")
	{
		i_index = item->text(column).toInt();
	}
	QString sValue;
	switch(i_index)
	{
	case 0:
		sValue=tr("静态功能点");
		break;
	case 1:
		sValue=tr("动态菜单项");
		break;
	case 2:
		sValue=tr("组态界面功能点");
		break;
	case 3:
		sValue=tr("动态报表功能点");
		break;
	case 4:
		sValue=tr("隐式功能点");
		break;
	case 5:
		sValue=tr("WEB页面");
		break;
	case 6:
		sValue=tr("JAVA三维场景界面类");
		break;
	default:
		sValue = tr("静态功能点");
		break;
	}
	item->setText(4,sValue);
}
void mTreeWidget::setColumnToUicfgOrReportRef(QTreeWidgetItem * item,int column)
{
	QString strRef = item->text(column);
	if (!strRef.isEmpty())
	{
		QString strType = item->text(column -1);
		if (strType == tr("组态界面功能点"))
		{
			//QString strShow = strRef + "-" + m_uicfgWndMap[strRef];
			QString strShow = m_uicfgWndMap[strRef];
			item->setText(column,strShow);
		} else if (strType == tr("动态报表功能点"))
		{
			//QString strShow = strRef + "-" + m_reportInfoMap[strRef];
			QString strShow = m_reportInfoMap[strRef];
			item->setText(column,strShow);
		}
	}
}
void mTreeWidget::oneCellChanged(QTreeWidgetItem* item,int column)
{
	mCombobox * pComb = (mCombobox *)sender();
	SString sValue=SString::toFormat("%d",pComb->currentIndex());

	if (column==4)
	{
		if (pComb->currentIndex()==2)
		{
			mCombobox * refNumComb=getRefNum();
			refNumComb->installEventFilter(this);
			refNumComb->setTreeItemAndColumn(item,5);
			setRefIndex(refNumComb,item->text(5));
			QObject::connect(refNumComb,SIGNAL(whitch(QTreeWidgetItem*,int)),this,SLOT(oneCellChanged(QTreeWidgetItem*,int)));
			this->setItemWidget(item,5,refNumComb);
			refNumComb->setFocusProxy(this->parentWidget());
			resizeColumnToContents(5);
		} else if (pComb->currentIndex()==3)
		{
			mCombobox * refNumComb=getReportNum();
			refNumComb->installEventFilter(this);
			refNumComb->setTreeItemAndColumn(item,5);
			setRefIndex(refNumComb,item->text(5));
			QObject::connect(refNumComb,SIGNAL(whitch(QTreeWidgetItem*,int)),this,SLOT(oneCellChanged(QTreeWidgetItem*,int)));
			this->setItemWidget(item,5,refNumComb);
			refNumComb->setFocusProxy(this->parentWidget());
			resizeColumnToContents(5);
		}else
		{
			this->removeItemWidget(item,5);
			item->setText(5,"");
		}
	}
	if (column==5)
	{
		QString sCombValue;
		QString sleft;

		sCombValue=pComb->currentText();
		item->setText(column,sCombValue);
		//sleft=sCombValue.left(sCombValue.indexOf('-'));
		//item->setText(column,sleft);

	}else
	{
		item->setText(column,pComb->currentText());
	}
}

bool mTreeWidget::eventFilter(QObject *obj, QEvent *event)
{
	//QString objname=obj->objectName();
	if(event->type() == QEvent::Wheel)
	{
		return true;
	}
	else
	{
		return QWidget::eventFilter(obj, event);
	}
}

int mTreeWidget::getLevel(QTreeWidgetItem *item)
{
	int iret=0;
	QTreeWidgetItem * pitem;
	pitem=item->parent();
	while(pitem!=NULL)
	{
		iret++;
		pitem=pitem->parent();
	}
	return iret;
}

void mTreeWidget::slotupLoadRowClicked()
{
	QObject * object = QObject::sender();
	if (!object)
		return;
	QStringList filters;
	filters << "Image files (*.png)";
	QFileDialog dialog;
	dialog.setNameFilters(filters);
	QString fName;
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	if (dialog.exec())
	{

		fName = dialog.selectedFiles().first();
		mPushButton * sender = qobject_cast<mPushButton *>(object);
		QString a = sender->name();
		SString sWhere = SString::toFormat("fun_key='%s'",a.toStdString().c_str());
		SString sLob = "";
		switch(sender->colID())
		{
		case MTREEWIDGET_COL_IMG_NORMAL:
			sLob = "img_normal";
			break;
		case MTREEWIDGET_COL_IMG_DOWN:
			sLob = "img_down";
			break;
		case MTREEWIDGET_COL_IMG_HOVER:
			sLob = "img_hover";
			break;
		case MTREEWIDGET_COL_IMG_DISABLE:
			sLob = "img_disable";
			break;
		default:
			sLob = "";
			break;
		}
		if(!m_dbOper->UpdateLobFromFile("t_ssp_fun_point",sLob,sWhere,fName.toStdString().data()))
		{
			QMessageBox::warning(this,tr("警告"),tr("从[%1]加载文件失败").arg(fName),QMessageBox::Ok);
			return;		
		}
	}
}

void mTreeWidget::slotDownLoadRowClicked()
{
	QObject * object = QObject::sender();
	QStringList filters;
	filters << "Image files (*.png)";
	QFileDialog dialog;
	dialog.setNameFilters(filters);
	QString fName;
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	if (dialog.exec())
	{
		fName = dialog.selectedFiles().first();
		if (fName.indexOf(".png") < 0)
		{
			fName += ".png";
		}
		mPushButton * sender = qobject_cast<mPushButton *>(object);
		QString a = sender->name();
		SString sWhere = SString::toFormat("fun_key='%s'",a.toStdString().c_str());
		SString sLob = "";
		switch(sender->colID())
		{
		case MTREEWIDGET_COL_IMG_NORMAL:
			sLob = "img_normal";
			break;
		case MTREEWIDGET_COL_IMG_DOWN:
			sLob = "img_down";
			break;
		case MTREEWIDGET_COL_IMG_HOVER:
			sLob = "img_hover";
			break;
		case MTREEWIDGET_COL_IMG_DISABLE:
			sLob = "img_disable";
			break;
		default:
			sLob = "";
			break;
		}
		if(!m_dbOper->ReadLobToFile("t_ssp_fun_point",sLob,sWhere,fName.toStdString().data()))
		{
			QMessageBox::warning(this,tr("警告"),tr("加载数据到文件[%1]失败").arg(fName),QMessageBox::Ok);
			return;		
		}
	}
}

