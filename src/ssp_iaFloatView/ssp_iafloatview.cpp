#include "ssp_iafloatview.h"
#include "cfloatviewmenu.h"
#include "cia2alarmpopwnd.h"
#include "ssp_gui.h"
#include "dlgmessage.h"

#define SCAN_FREQ		10 //秒

//3.1.12	历史智能告警表T_IA2_HISTORY
struct t_ia2_history
{
	int           ia_his;
	short         sub_no;
	int           rule_no;
	unsigned char rule_type;
	int           bay_no;
	int           act_soc;
	int           act_usec;
	char          ia_conclusion[2048];
	char          ia_suggest[2048];
	unsigned char rule_severity;
	unsigned char is_confirm;
	char          confirm_user[16];
	int           confirm_soc;
};

ssp_iaFloatView::ssp_iaFloatView(QWidget *parent) 
	: QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool | Qt::X11BypassWindowManagerHint);
	setAttribute(Qt::WA_TranslucentBackground,true);
	setMouseTracking(true);

	Init();

#ifdef _IA2_STYLE_
	m_pixFloatView.load(":/images/ia2floatview");
	InitIA2UI();
	InitIA2Menu();
	InitIA2AlarmWnd();
#endif
	setFixedSize(m_pixFloatView.width(),m_pixFloatView.height());

	MovePostion();

	m_iScanTimer->start();
}

ssp_iaFloatView::~ssp_iaFloatView()
{
	delete (CFloatViewMenu*)m_iMenu;
	delete (CIa2AlarmPopWnd*)m_iAlarmWnd;
	delete m_iSettings;
}

void ssp_iaFloatView::Init()
{
	QString sFile = QCoreApplication::applicationDirPath() + "/" + "../conf/floatwndcfg.ini";
	if (!QFile::exists(sFile))
	{
		QString text = "[pop]\r\nnone=0\r\ncritical=1\r\nimportant=1\r\nsecondary=0\r\ngeneral=0\r\n"
			"[color]\r\ncritical=#ff0000\r\nimportant=#ff5500\r\nsecondary=#ffaa00\r\ngeneral=#55aaff\r\n"
			"[count]\r\nmaxshow=5000\r\nrestoretime=600\r\n";
		QFile file(sFile);  
		if (file.open(QIODevice::ReadWrite))
			file.write(text.toStdString().data());
		file.close(); 
	}

	m_iSettings = new QSettings(sFile,QSettings::IniFormat);

	m_iScanTimer = new QTimer(this);
	m_iScanTimer->setInterval(SCAN_FREQ * 1000);
	connect(m_iScanTimer,SIGNAL(timeout()),this,SLOT(SlotScanTimeout()));

	//CMdbClient *m_pMdbTrgClient = ((SMdb*)MDB->GetDatabasePool()->GetDatabaseByIdx(MDB->GetDatabasePool()->GetPoolSize()-1))->GetMdbClient();
	//m_pMdbTrgClient->RegisterTriggerCallback(OnRealEventCallback,this,"t_ia2_history",0x01);
}

void ssp_iaFloatView::InitIA2UI()
{
#ifdef WIN32
	QFont font(tr("Microsoft Yahei"),10,QFont::Bold,false);
#else
	QFont font(tr("宋体"),10,QFont::Bold,false);
#endif

	m_iLabelCritical = new QLabel;
	m_iLabelImportant = new QLabel;
	m_iLabelSecondary = new QLabel;
	m_iLabelGeneral = new QLabel;
	m_iLabelCriticalNumber = new QLabel;
	m_iLabelImportantNumber = new QLabel;
	m_iLabelSecondaryNumber = new QLabel;
	m_iLabelGeneralNumber = new QLabel;

	m_iLabelCritical->setText(tr("紧急："));
	m_iLabelImportant->setText(tr("重要："));
	m_iLabelSecondary->setText(tr("次要："));
	m_iLabelGeneral->setText(tr("一般："));
	m_iLabelCritical->setFont(font);
	m_iLabelImportant->setFont(font);
	m_iLabelSecondary->setFont(font);
	m_iLabelGeneral->setFont(font);
	m_iLabelCriticalNumber->setFont(font);
	m_iLabelImportantNumber->setFont(font);
	m_iLabelSecondaryNumber->setFont(font);
	m_iLabelGeneralNumber->setFont(font);
	RefreshFloatWnd();
	m_iFormLayout = new QFormLayout;

#ifdef WIN32
	m_iFormLayout->setContentsMargins(15,57,2,6);
#else
	m_iFormLayout->setContentsMargins(15,57,6,6);
	m_iFormLayout->setVerticalSpacing(8);
#endif

	m_iFormLayout->addRow(m_iLabelCritical,m_iLabelCriticalNumber);
	m_iFormLayout->addRow(m_iLabelImportant,m_iLabelImportantNumber);
	m_iFormLayout->addRow(m_iLabelSecondary,m_iLabelSecondaryNumber);
	m_iFormLayout->addRow(m_iLabelGeneral,m_iLabelGeneralNumber);

	setLayout(m_iFormLayout);
}

void ssp_iaFloatView::InitIA2Menu()
{
	m_iMenu = new CFloatViewMenu(this);
}

void ssp_iaFloatView::InitIA2AlarmWnd()
{
	m_iAlarmWnd = new CIa2AlarmPopWnd(this);
	((CIa2AlarmPopWnd*)m_iAlarmWnd)->Start();
}

void ssp_iaFloatView::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	painter.drawPixmap(this->rect(),m_pixFloatView);
}

void ssp_iaFloatView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_dragPosition = event->globalPos() - frameGeometry().topLeft();
	}
	else if (event->button() == Qt::RightButton)
	{
		((CFloatViewMenu*)m_iMenu)->clear();
		((CFloatViewMenu*)m_iMenu)->CreateMenuItem();
		((CFloatViewMenu*)m_iMenu)->popup(event->globalPos());
	}
	event->accept();

	QWidget::mousePressEvent(event);
}

void ssp_iaFloatView::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		move(event->globalPos() - m_dragPosition);
		event->accept();
	}

	QWidget::mouseMoveEvent(event);
}

void ssp_iaFloatView::mouseDoubleClickEvent(QMouseEvent *event)
{
	((CIa2AlarmPopWnd*)m_iAlarmWnd)->show();
	this->hide();
}

void ssp_iaFloatView::setIa2AlarmPopWndHidden(bool bhidden)
{
	if (!bhidden)
		((CIa2AlarmPopWnd*)m_iAlarmWnd)->show();
	else
		((CIa2AlarmPopWnd*)m_iAlarmWnd)->hide();
}

void ssp_iaFloatView::setIa2AlarmPopWndOnTop(bool btop)
{
	if (btop)
		this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
	else
		this->setWindowFlags(this->windowFlags() &~ Qt::WindowStaysOnTopHint);
}

void ssp_iaFloatView::GetSeverityNum(int& s1,int& s2,int& s3,int& s4)
{
	s1 = DB->SelectIntoI("select count(*) from t_ia2_history where is_confirm=0 and rule_severity=4");
	s2 = DB->SelectIntoI("select count(*) from t_ia2_history where is_confirm=0 and rule_severity=3");
	s3 = DB->SelectIntoI("select count(*) from t_ia2_history where is_confirm=0 and rule_severity=2");
	s4 = DB->SelectIntoI("select count(*) from t_ia2_history where is_confirm=0 and rule_severity=1");
}

QString ssp_iaFloatView::ReadSettings(QString path)
{
	return m_iSettings->value(path).toString();
}

void ssp_iaFloatView::WriteSettings(QString path,QString value)
{
	m_iSettings->setValue(path,value);  
}

void ssp_iaFloatView::MovePostion()
{
	int curMonitor = QApplication::desktop()->screenNumber(this);
	QRect rect = QApplication::desktop()->availableGeometry(curMonitor);
	move(rect.right()-m_pixFloatView.width()-100,rect.top()+100);
}

BYTE* ssp_iaFloatView::OnRealEventCallback(void* cbParam, SString &sTable, eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData)
{
	S_UNUSED(eType);
	if (sTable == "t_ia2_history" && sizeof(t_ia2_history) != iRowSize)
		return 0;

	stuHisItem *tItem;
	t_ia2_history tHis;
	ssp_iaFloatView *pThis = (ssp_iaFloatView*)cbParam;
	pThis->m_iLock.lock();
	for (int i = 0; i < iTrgRows; i++)
	{
		memcpy(&tHis,pTrgData+i*sizeof(t_ia2_history*),sizeof(t_ia2_history));
		tItem = new stuHisItem;
		tItem->sn = tHis.ia_his;
		tItem->soc = tHis.act_soc;
		tItem->usec = tHis.act_usec;
		tItem->severity = tHis.rule_severity;
		tItem->subNo = tHis.sub_no;
		tItem->bayNo = tHis.bay_no;
		tItem->conclusion = tr("%1").arg(tHis.ia_conclusion);
		tItem->isConfirm = tHis.is_confirm == 1 ? true : false;
		tItem->confirmUser = tr("%1").arg(tHis.confirm_user);
		tItem->confirmSoc = tHis.confirm_soc;
		((CIa2AlarmPopWnd*)pThis->m_iAlarmWnd)->InsertMapHisItem(tItem);
		((CIa2AlarmPopWnd*)pThis->m_iAlarmWnd)->InsertTableWidget(tItem);
		((CIa2AlarmPopWnd*)pThis->m_iAlarmWnd)->PopupWnd(tItem);
	}
	pThis->m_iLock.unlock();

	pThis->RefreshFloatWnd();
	return 0;
}

void ssp_iaFloatView::SlotScanTimeout()
{
	SString sql;
	SRecordset rs;

	sql.sprintf("select ia_his,sub_no,bay_no,act_soc,act_usec,ia_conclusion,rule_severity,is_confirm,confirm_user,confirm_soc "
		"from t_ia2_history where ia_his>%d",m_iLastHisSequence);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0)
	{
		if (((CIa2AlarmPopWnd*)m_iAlarmWnd)->GetCurrentSeverity() != SEVERITY_ALL) //有新告警切回全部告警
			((CIa2AlarmPopWnd*)m_iAlarmWnd)->CallSlotBtnAllClicked();

		m_iLock.lock();
		for (int i = 0; i < cnt; i++)
		{
			stuHisItem *item = new stuHisItem;
			item->sn =  rs.GetValue(i,0).toInt();
			item->subNo = rs.GetValue(i,1).toInt();
			item->bayNo = rs.GetValue(i,2).toInt();
			item->soc = rs.GetValue(i,3).toInt();
			item->usec = rs.GetValue(i,4).toInt();
			item->conclusion = rs.GetValue(i,5).data();
			item->severity = rs.GetValue(i,6).toInt();
			item->isConfirm = rs.GetValue(i,7).toInt() == 0 ? false : true;
			item->confirmUser = rs.GetValue(i,8).data();
			item->confirmSoc = rs.GetValue(i,9).toInt();
			if (item->sn > m_iLastHisSequence)
				m_iLastHisSequence = item->sn;

			((CIa2AlarmPopWnd*)m_iAlarmWnd)->InsertMapHisItem(item);
			((CIa2AlarmPopWnd*)m_iAlarmWnd)->InsertTableWidget(item);
			((CIa2AlarmPopWnd*)m_iAlarmWnd)->PopupWnd(item);
		}
		m_iLock.unlock();

		RefreshFloatWnd();
	}
}

void ssp_iaFloatView::RefreshFloatWnd()
{
	int criticalNum = 0,importantNum = 0,secondaryNum = 0,generalNum = 0;
	GetSeverityNum(criticalNum,importantNum,secondaryNum,generalNum);
	m_iLabelCriticalNumber->setText(tr("%1").arg(criticalNum));
	m_iLabelImportantNumber->setText(tr("%1").arg(importantNum));
	m_iLabelSecondaryNumber->setText(tr("%1").arg(secondaryNum));
	m_iLabelGeneralNumber->setText(tr("%1").arg(generalNum));

	QString level = ReadSettings("/color/critical");
	if (level.isEmpty())
		level = "#ff0000";
	QColor color(level);
	m_iLabelCritical->setStyleSheet(tr("text-align:center; color:rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
	m_iLabelCriticalNumber->setStyleSheet(tr("text-align:center; color:rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));

	level = ReadSettings("/color/important");
	if (level.isEmpty())
		level = "#ff5500";
	color.setNamedColor(level);
	m_iLabelImportant->setStyleSheet(tr("text-align:center; color:rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
	m_iLabelImportantNumber->setStyleSheet(tr("text-align:center; color:rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));

	level = ReadSettings("/color/secondary");
	if (level.isEmpty())
		level = "#ffaa00";
	color.setNamedColor(level);
	m_iLabelSecondary->setStyleSheet(tr("text-align:center; color:rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
	m_iLabelSecondaryNumber->setStyleSheet(tr("text-align:center; color:rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));

	level = ReadSettings("/color/general");
	if (level.isEmpty())
		level = "#55aaff";
	color.setNamedColor(level);
	m_iLabelGeneral->setStyleSheet(tr("text-align:center; color:rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
	m_iLabelGeneralNumber->setStyleSheet(tr("text-align:center; color:rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
}

bool ssp_iaFloatView::OnConfirmHistory(int sn)
{
	return ((CIa2AlarmPopWnd*)m_iAlarmWnd)->OnConfirmHistory(sn);
}

void ssp_iaFloatView::IAalarmQuery()
{
	((CIa2AlarmPopWnd*)m_iAlarmWnd)->hide();
	this->show();
}