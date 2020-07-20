#include "panel_sim_root.h"
#include "GuiApplication.h"
#include <math.h>
panel_sim_root::panel_sim_root(CSimManager *pSim,QWidget *parent)
	: QWidget(parent)
{
	m_pSimManager = pSim;
	ui.setupUi(this);
	ui.pushButton_Start->setEnabled(true);
	ui.pushButton_Stop->setEnabled(false);
	connect(ui.pushButton_Start,SIGNAL(clicked()),this,SLOT(Start()));
	connect(ui.pushButton_Stop,SIGNAL(clicked()),this,SLOT(Stop()));

	m_iconRoot = QIcon(":/sim_iec61850/Resources/FONT02.ICO");
	m_iconRun = QIcon(":/sim_iec61850/Resources/20070402165933835.gif");
	m_iconStop = QIcon(":/sim_iec61850/Resources/20070402165933865.gif");

	connect(ui.twScripts, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(treeItemPressed(QTreeWidgetItem*, int)));
	connect(ui.twScripts,SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),this,SLOT(OnTreeCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
	connect(ui.btnCheckScript,SIGNAL(clicked()),this,SLOT(OnScriptCheck()));
	connect(ui.btnStartScript,SIGNAL(clicked()),this,SLOT(OnScriptStart()));
	connect(ui.btnStopScript,SIGNAL(clicked()),this,SLOT(OnScriptEnd()));
	connect(ui.btnSearch,SIGNAL(clicked()),this,SLOT(OnFcdaSearch()));
	connect(ui.btnSave,SIGNAL(clicked()),this,SLOT(OnSaveScript()));
	connect(this,SIGNAL(sigScriptStatus(stuSimRunScript*)),this,SLOT(OnScriptStatus(stuSimRunScript*)));

	ui.edtScript->setEnabled(false);
	ui.edtScriptTimes->setEnabled(false);
	ui.edtScriptSepMs->setEnabled(false);
	ui.edtScriptTimes->setEnabled(false);
	ui.btnCheckScript->setEnabled(false);
	ui.btnStartScript->setEnabled(false);
	ui.btnStopScript->setEnabled(false);

	LoadScriptTree();
	m_iThreads = 0;
	m_bStop = true;
	m_bStarted = false;
}

panel_sim_root::~panel_sim_root()
{
	Stop();
}

void panel_sim_root::closeEvent(QCloseEvent * event)
{
	
}

void panel_sim_root::Start()
{
	m_bStarted = true;
	ui.pushButton_Start->setEnabled(false);
	m_pSimManager->m_MmsServer.OnProgressText("正在启动SV、GOOSE模拟器...");
	SQt::ProcessUiEvents();
	if(!m_pSimManager->Start())
	{
		SQt::ShowErrorBox("错误","启动SV、GOOSE模拟器服务失败!");
		m_pSimManager->m_MmsServer.OnProgressText("启动SV、GOOSE模拟器服务失败!");
		ui.pushButton_Start->setEnabled(true);
		return;
	}
	m_pSimManager->m_MmsServer.m_pSimConfig = m_pSimManager->m_pSimConfig;
	if(m_pSimManager->m_pSimConfig->m_bOpenMmsServer && SFile::exists(SBASE_SAPP->GetConfPath()+"1.scd"))
	{
#ifndef WIN32
		if(m_pSimManager->m_pSimConfig->m_bMmsSvrSimIpAddr)
			m_pSimManager->m_MmsServer.AddApIpAddr();
#endif

		SString scd=SBASE_SAPP->GetConfPath()+"1.scd";
		int id=0;
#if 1
		unsigned long pos;
		stuSclVtIed *pIed = m_pSimManager->m_pSimConfig->m_MmsServerIed.FetchFirst(pos);
		while(pIed)
		{
			m_pSimManager->m_MmsServer.AddIed(stuMmsServerIed::New(scd,++id,pIed->name,"S1"));
			pIed = m_pSimManager->m_pSimConfig->m_MmsServerIed.FetchNext(pos);
		}
#else
 		int ret = m_pSimManager->m_MmsServer.AddIedByScd(scd);
 		if(ret <0)
 			CGuiApplication::GetApp()->NewEvent("系统",0,0,"","提示","加载SCD文件时失败!");
#endif
		CGuiApplication::GetApp()->NewEvent("系统",0,0,"","提示",SString::toFormat("成功加载%d个MMS访问点服务!",id));

		m_pSimManager->m_MmsServer.Start();		
		CGuiApplication::GetApp()->NewEvent("系统",0,0,"","提示",SString::toFormat("MMS服务启动完毕!"));
	}
	ui.pushButton_Stop->setEnabled(true);
	m_iThreads = 0;
	m_bStop = false;
	SKT_CREATE_THREAD(ThreadRunBcastScript,this);
	CGuiApplication::GetApp()->NewEvent("系统",0,0,"","提示","模拟器服务启动!");
	m_pSimManager->m_MmsServer.OnProgressText("模拟服务启动完毕!");
}

void panel_sim_root::Stop()
{
	if(m_bStarted == false)
		return;
	m_bStarted = false;
	m_bStop = true;
	if(!m_pSimManager->Stop())
	{
		SQt::ShowErrorBox("错误","停止模拟器服务失败!");
		return;
	}
#ifndef WIN32
	m_pSimManager->m_MmsServer.RemoveApIpAddr();
#endif
	if(m_pSimManager->m_pSimConfig->m_bOpenMmsServer)
		m_pSimManager->m_MmsServer.StopAndWait();
	while(m_iThreads > 0)
		SApi::UsSleep(100000);
	ui.pushButton_Start->setEnabled(true);
	ui.pushButton_Stop->setEnabled(false);
	CGuiApplication::GetApp()->NewEvent("系统",0,0,"","提示","模拟器服务停止!");
}

void panel_sim_root::LoadScriptTree()
{
	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(10);

	ui.edtScript->setFont(font);

	m_highlighter = new Highlighter(ui.edtScript->document());

	ui.twScripts->clear();
	m_pRoot = new QTreeWidgetItem();
	m_pRoot->setData(0,Qt::UserRole,QVariant(0));
	m_pRoot->setText(0,"CS运行脚本");
	m_pRoot->setIcon(0,m_iconRoot);
	ui.twScripts->addTopLevelItem(m_pRoot);
	ui.twScripts->setCurrentItem(m_pRoot);

	unsigned long pos;
	stuSimRunScript *pScript = m_pSimManager->m_pSimConfig->m_Scripts.FetchFirst(pos);
	while(pScript)
	{
		//添加
		QTreeWidgetItem *pItem = new QTreeWidgetItem();
		pItem->setData(0,Qt::UserRole,QVariant(1));
		pItem->setData(0,Qt::UserRole+1,QVariant(pScript->name.data()));
		pItem->setText(0,pScript->name.data());
		pItem->setToolTip(0,pScript->name.data());
		pItem->setIcon(0,m_iconStop);
		m_pRoot->addChild(pItem);
		pItem->setText(1,"未运行");

		pScript = m_pSimManager->m_pSimConfig->m_Scripts.FetchNext(pos);
	}
	m_pRoot->setExpanded(true);
}


void panel_sim_root::OnTreeCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	if(previous != NULL)
	{
		if(previous->data(0,Qt::UserRole).toInt() == 1)
		{
			//保存前一个
			SString name = previous->data(0,Qt::UserRole+1).toString().toStdString().data();
			stuSimRunScript *pScript = m_pSimManager->m_pSimConfig->SearchRunScriptByName(name);
			pScript->script = ui.edtScript->toPlainText().toStdString().data();
			pScript->run_times = atoi(ui.edtScriptTimes->text().toStdString().data());
			pScript->run_sepms = atoi(ui.edtScriptSepMs->text().toStdString().data());
		}
	}
	if(current != NULL)
	{
		if(current->data(0,Qt::UserRole).toInt() == 1)
		{
			//加载当前的脚本
			ui.edtScript->setEnabled(true);
			ui.edtScriptTimes->setEnabled(true);
			ui.edtScriptSepMs->setEnabled(true);
			ui.edtScriptTimes->setEnabled(true);
			ui.btnCheckScript->setEnabled(true);
			ui.btnStartScript->setEnabled(true);
			ui.btnStopScript->setEnabled(true);
			SString name = current->data(0,Qt::UserRole+1).toString().toStdString().data();
			stuSimRunScript *pScript = m_pSimManager->m_pSimConfig->SearchRunScriptByName(name);
			ui.edtScript->setPlainText(pScript->script.data());
			ui.edtScriptTimes->setText(SString::toString(pScript->run_times).data());
			ui.edtScriptSepMs->setText(SString::toString(pScript->run_sepms).data());
// 			if(pScript->rt_running)
// 				ui.labScriptStatus->setText(SString::toFormat("第%d次运行...",pScript->rt_times).data());
// 			else
// 				ui.labScriptStatus->setText("未运行");
		}
		else
		{
			ui.edtScript->setEnabled(false);
			ui.edtScriptTimes->setEnabled(false);
			ui.edtScriptSepMs->setEnabled(false);
			ui.edtScriptTimes->setEnabled(false);
			ui.btnCheckScript->setEnabled(false);
			ui.btnStartScript->setEnabled(false);
			ui.btnStopScript->setEnabled(false);
			ui.edtScript->setPlainText("");
			ui.edtScriptTimes->setText("");
			ui.edtScriptSepMs->setText("");
		}
	}
}


void panel_sim_root::treeItemPressed( QTreeWidgetItem *item, int column )
{
	if (qApp->mouseButtons() == Qt::RightButton && item != NULL) 
	{
		QMenu menu(this);
		if(item->data(0,Qt::UserRole).toInt() == 0)
		{
			menu.addAction(tr("保存所有脚本"),this,SLOT(OnSaveScript()));
			menu.addAction(tr("添加运行脚本"),this,SLOT(OnNewScript()));
		}
		else
		{			
			//menu.addAction(tr("运行"),this,SLOT(OnRenameScript()));
			//menu.addSeparator();
			menu.addAction(tr("重命名"),this,SLOT(OnRenameScript()));
			menu.addAction(tr("删除"),this,SLOT(OnRemoveScript()));
			menu.addSeparator();
			menu.addAction(tr("广播此脚本"),this,SLOT(OnBcastScript()));
		}
		menu.exec(QCursor::pos());
	}
}


void panel_sim_root::OnSaveScript()
{
	m_pSimManager->m_pSimConfig->Save();
}

struct stuThreadRunScriptParam
{
	panel_sim_root *pThis;
	stuSimRunScript *pScript;
};

#include "dlg_fcda_filter.h"

void panel_sim_root::OnFcdaSearch()
{
	static dlg_fcda_filter *dlg = NULL;

	if(dlg == NULL)
	{
		dlg = new dlg_fcda_filter();
		dlg->m_pSimConfig = m_pSimManager->m_pSimConfig;
		dlg->m_pSimMmsServer = &m_pSimManager->m_MmsServer;
		dlg->m_pTextEdit = ui.edtScript;
	}
	dlg->show();
}

void panel_sim_root::OnScriptStart()
{
	QTreeWidgetItem *pItem = ui.twScripts->currentItem();
	if(pItem == NULL || pItem->data(0,Qt::UserRole).toInt() == 0)
		return;
	SString name = pItem->data(0,Qt::UserRole+1).toString().toStdString().data();
	stuSimRunScript *pScript = m_pSimManager->m_pSimConfig->SearchRunScriptByName(name);
	if(pScript == NULL)
		return;

	//保存前一个
	pScript->script = ui.edtScript->toPlainText().toStdString().data();
	pScript->run_times = atoi(ui.edtScriptTimes->text().toStdString().data());
	pScript->run_sepms = atoi(ui.edtScriptSepMs->text().toStdString().data());


	if(pScript->rt_running)
	{
		SQt::ShowInfoBox("提示","当前脚本正在运行中...");
		return;
	}

	CSimScriptParser sim;
	if(!sim.LoadSyntaxText(pScript->script.data()))
	{
		QTextBlock block = ui.edtScript->document()->findBlockByNumber(sim.GetErrorTextRow());  
		ui.edtScript->setTextCursor(QTextCursor(block));  
		SQt::ShowErrorBox("错误",SString::toFormat("无法运行脚本!\r\n第%d行脚本语法错误!",sim.GetErrorTextRow()).data());
		return;
	}
	stuThreadRunScriptParam *pParam = new stuThreadRunScriptParam();
	pParam->pScript = pScript;
	pParam->pThis = this;
	pScript->rt_stop = false;
	pScript->rt_times = 0;
	pScript->rt_running = true;
	emit sigScriptStatus(pScript);
	SKT_CREATE_THREAD(ThreadRunScript,pParam);
}

void panel_sim_root::OnScriptEnd()
{
	QTreeWidgetItem *pItem = ui.twScripts->currentItem();
	if(pItem == NULL || pItem->data(0,Qt::UserRole).toInt() == 0)
		return;
	SString name = pItem->data(0,Qt::UserRole+1).toString().toStdString().data();
	stuSimRunScript *pScript = m_pSimManager->m_pSimConfig->SearchRunScriptByName(name);
	if(pScript == NULL)
		return;
	if(!pScript->rt_running)
	{
		SQt::ShowInfoBox("提示","当前脚本尚未运行!");
		return;
	}
	pScript->rt_stop = true;
}

void panel_sim_root::OnScriptCheck()
{
	QTreeWidgetItem *pItem = ui.twScripts->currentItem();
	if(pItem == NULL || pItem->data(0,Qt::UserRole).toInt() == 0)
		return;
	SString name = pItem->data(0,Qt::UserRole+1).toString().toStdString().data();
	stuSimRunScript *pScript = m_pSimManager->m_pSimConfig->SearchRunScriptByName(name);
	if(pScript == NULL)
		return;
	CSimScriptParser sim;
	if(!sim.LoadSyntaxText(pScript->script.data()))
	{
		QTextBlock block = ui.edtScript->document()->findBlockByNumber(sim.GetErrorTextRow());  
		ui.edtScript->setTextCursor(QTextCursor(block));  
		SQt::ShowErrorBox("错误",SString::toFormat("第%d行脚本语法错误!",sim.GetErrorTextRow()).data());
	}
	else
	{
		SQt::ShowInfoBox("提示","脚本校验通过!");
	}
}


void panel_sim_root::OnScriptStatus(stuSimRunScript* pScript)
{
	int i,cnt = m_pRoot->childCount();
	for(i=0;i<cnt;i++)
	{
		QTreeWidgetItem *pItem = m_pRoot->child(i);
		SString name = pItem->data(0,Qt::UserRole+1).toString().toStdString().data();
		if(name == pScript->name)
		{
			if(pScript->rt_running)
			{
				if(pScript->rt_times == 0)
					pItem->setText(1,"准备运行...");
				else
					pItem->setText(1,SString::toFormat("第%d次运行...",pScript->rt_times).data());
				pItem->setIcon(0,m_iconRun);
			}
			else
			{
				pItem->setText(1,"未运行");
				pItem->setIcon(0,m_iconStop);
			}
			break;
		}
	}
}


void* panel_sim_root::ThreadRunScript(void *lp)
{
	stuThreadRunScriptParam *pParam = (stuThreadRunScriptParam*)lp;
	panel_sim_root *pThis = (panel_sim_root*)pParam->pThis;
	stuSimRunScript *pScript = pParam->pScript;
	pScript->rt_running = true;
	pScript->rt_times = 0;
	CSimScriptParser sim;
	sim.SetSimManagerPtr(pThis->m_pSimManager);
	if(!sim.LoadSyntaxText(pScript->script.data()))
	{
		pScript->rt_running = false;
		SSP_GUI->SetStatusTip(SString::toFormat("脚本'%s'语法错误，无法进行加载!",pScript->name.data()));
		delete pParam;
		return NULL;
	}
	int i,cnt = pScript->run_times;
	SScriptParser::stuValue RetValue;
	SString sMain = "main";
	pThis->m_iThreads ++;
	for(i=0;pThis->m_bStop == false && pScript->rt_stop==false && i<cnt;i++)
	{
		pScript->rt_times++;
		emit pThis->sigScriptStatus(pScript);
		sim.RunFunction(RetValue,sMain);
		if(i!=cnt-1)
			SApi::UsSleep(pScript->run_sepms*1000);
	}
	pScript->rt_running = false;
	pScript->rt_stop = true;
	emit pThis->sigScriptStatus(pScript);
	delete pParam;
	pThis->m_iThreads --;
	return NULL;
}

void panel_sim_root::OnNewScript()
{
	SString name;
	while(1)
	{
		name = SQt::GetInput("输入","请输入脚本名称","新脚本");
		if(name.length() == 0)
			return;
		if(m_pSimManager->m_pSimConfig->SearchRunScriptByName(name) != NULL)
		{
			SQt::ShowErrorBox("错误",SString::toFormat("'%s'已存在，请重新输入!",name.data()).data());
			continue;
		}
		break;
	}
	stuSimRunScript *pScript = new stuSimRunScript();
	pScript->name = name;
	pScript->run_times = 1;
	pScript->run_sepms = 1000;
	pScript->rt_running = false;
	pScript->rt_times = 0;
	pScript->script =	
		"void main()\r\n"
		"{\r\n"
		"  //bool GooseChg(int appid,string vals);\r\n"
		"  //     生成一次GOOSE变位，vals为ch_name1=val1;ch_name2=val2;\r\n"
		"  //     val可以为true/false，也可以为now(表示当前时间) \r\n"
		"  //bool SvChg(int appid,int ch_idx,float rms,int q);\r\n"
		"  //     修改SV值，ch_id为通道号(从1开始),rms为有效值,q为品质 \r\n"
		"  //bool SetLeaf(string ied_ld,string leaf_path,int vt,string val);\r\n"
		"  //     修改LN值，ied_ldIED与LD名称，leaf_path为叶子节点路径,\r\n"
		"  //     vt为值类型(1-bool,2-int,3-float,4-utctime)\r\n"
		"  //e.g. :\r\n"
		"  //      GooseChg(0x1001,\"重合闸=true;跳A相=false;\");\r\n"
		"  //      SvChg(0x4001,2,1000,0);\r\n"
		"  //      $SetLeaf(\"CL2223CTRL\",\"GGIO26$ST$Alm1$stVal\",1,\"1\");\r\n"
		"  //      $SetLeaf(\"CL2223CTRL\",\"GGIO26$ST$Alm1$t\",4,\"now\");\r\n"
		"  //TODO:从这里开始编写脚本...\r\n"
		"}\r\n";
	m_pSimManager->m_pSimConfig->m_Scripts.append(pScript);

	//添加
	QTreeWidgetItem *pItem = new QTreeWidgetItem();
	pItem->setData(0,Qt::UserRole,QVariant(1));
	pItem->setData(0,Qt::UserRole+1,QVariant(pScript->name.data()));
	pItem->setText(0,pScript->name.data());
	pItem->setToolTip(0,pScript->name.data());
	pItem->setIcon(0,m_iconStop);
	m_pRoot->addChild(pItem);
	ui.twScripts->setCurrentItem(pItem);
}

struct stuBcastScriptInfo
{
	int sn;//命令随机序号
	int idx;//当前帧号，从0开始
	int frames;//总帧数
	int len;//有效长度
	int times;//运行次数
	int sep_ms;//运行间隔
	char title[32];
	char text[1028];
};

SString g_BcastScriptIpAddr="255.255.255.255";
void panel_sim_root::OnBcastScript()
{
	QTreeWidgetItem *pItem = ui.twScripts->currentItem();
	if(pItem == NULL || pItem->data(0,Qt::UserRole).toInt() == 0)
		return;
	SString old_name = pItem->data(0,Qt::UserRole+1).toString().toStdString().data();
	stuSimRunScript *pScript = m_pSimManager->m_pSimConfig->SearchRunScriptByName(old_name);
	if(pScript == NULL)
		return;
	pScript->script = ui.edtScript->toPlainText().toStdString().data();
	pScript->run_times = atoi(ui.edtScriptTimes->text().toStdString().data());
	pScript->run_sepms = atoi(ui.edtScriptSepMs->text().toStdString().data());

	SString sBip = SQt::GetInput("广播此脚本","请输入广播地址",g_BcastScriptIpAddr.data());
	if(sBip.length() == 0)
		return;

	stuBcastScriptInfo frame;
	int cnt = pScript->script.length()/1024;
	if(pScript->script.length()%1024 != 0)
		cnt ++;
	SSocket udp;
	if(!udp.CreateBroadCast())
	{
		SQt::ShowErrorBox("错误","创建广播UDP失败!");
		return ;
	}
	memset(&frame.title,0,sizeof(frame.title));
	strcpy(frame.title,pScript->name.left(30).data());
	frame.sn = rand();
	frame.frames = cnt;
	frame.times = pScript->run_times;
	frame.sep_ms = pScript->run_sepms;
	int pos = 0;
	int len = pScript->script.length();
	for(int i=0;i<cnt;i++)
	{
		frame.idx = i;
		frame.len = len;
		if(frame.len > 1024)
			frame.len = 1024;
		memset(frame.text,0,sizeof(frame.text));
		strcpy(frame.text,pScript->script.mid(pos,frame.len));
		pos += frame.len;
		len -= frame.len;
		int ret = udp.SendTo(sBip.data(),1421,&frame,sizeof(frame));
		if(ret != sizeof(frame))
		{
			SQt::ShowErrorBox("错误","发送广播UDP报文时失败!");
			break;
		}
		udp.SendTo(sBip.data(),1421,&frame,sizeof(frame));
		udp.SendTo(sBip.data(),1421,&frame,sizeof(frame));
	}
}


void* panel_sim_root::ThreadRunBcastScript(void *lp)
{
	panel_sim_root *pThis = (panel_sim_root*)lp;
	int sn=-1,idx,frames;
	SString sTitle,sScriptText;
	int iTimes;
	int iSepMs;
	stuBcastScriptInfo frame;
	SSocket udp;
	if(!udp.CreateUdp() || !udp.Bind(1421))
	{
		SSP_GUI->SetStatusTip("绑定UDP端口1421时失败!");
		return NULL;
	}
	udp.SetTimeout(2000,2000);
	pThis->m_iThreads ++;
	int i,ret;
	sockaddr_in sa;
	while(pThis->m_bStop == false)
	{
		ret = udp.RecvFrom(&frame,sizeof(frame),sa);
		if(ret != sizeof(frame))
		{
			SApi::UsSleep(10000);
			continue;
		}
		if(frame.sn == sn)
			continue;
		sn = frame.sn;
		idx = frame.idx;
		frames = frame.frames;
		sTitle = frame.title;
		iTimes = frame.times;
		iSepMs = frame.sep_ms;
		sScriptText = frame.text;
		if(idx != 0)
		{
			continue;
		}
		bool berr=false;
		for(i=idx+1;i<frames;i++)
		{
			//接收余下的帧
			ret = udp.RecvFrom(&frame,sizeof(frame),sa);
			if(ret != sizeof(frame))
			{
				berr = true;
				break;
			}
			if(frame.sn != sn || frame.idx != idx+1)
				continue;
			idx = frame.idx;
			sScriptText += frame.text;
		}
		if(berr)
		{
			continue;
		}
		SSP_GUI->SetStatusTip(SString::toFormat("收到广播脚本[%s]共%d帧!开始运行...",sTitle.data(),frames));
		CSimScriptParser sim;
		sim.SetSimManagerPtr(pThis->m_pSimManager);
		if(!sim.LoadSyntaxText(sScriptText.data()))
		{
			SSP_GUI->SetStatusTip(SString::toFormat("广播脚本'%s'语法错误，无法进行加载!",sTitle.data()));
			continue;
		}
		SScriptParser::stuValue RetValue;
		SString sMain = "main";
		for(i=0;pThis->m_bStop==false&&i<iTimes;i++)
		{
			SSP_GUI->SetStatusTip(SString::toFormat("广播脚本[%s]第%d次运行...",sTitle.data(),i+1));
			sim.RunFunction(RetValue,sMain);
			if(i!=iTimes-1)
				SApi::UsSleep(iSepMs*1000);
		}
		SSP_GUI->SetStatusTip(SString::toFormat("广播脚本[%s]运行结束!",sTitle.data()));
	}
	pThis->m_iThreads --;
	return NULL;
}

void panel_sim_root::OnRenameScript()
{
	QTreeWidgetItem *pItem = ui.twScripts->currentItem();
	if(pItem == NULL || pItem->data(0,Qt::UserRole).toInt() == 0)
		return;
	SString old_name = pItem->data(0,Qt::UserRole+1).toString().toStdString().data();
	SString name;
	while(1)
	{
		name = SQt::GetInput("输入","请输入脚本名称",old_name.data());
		if(name.length() == 0 || name == old_name)
			return;
		if(m_pSimManager->m_pSimConfig->SearchRunScriptByName(name) != NULL)
		{
			SQt::ShowErrorBox("错误",SString::toFormat("'%s'已存在，请重新输入!",name.data()).data());
			continue;
		}
		break;
	}
	stuSimRunScript *pScript = m_pSimManager->m_pSimConfig->SearchRunScriptByName(old_name);
	if(pScript == NULL)
		return;
	pScript->name = name;
	pItem->setData(0,Qt::UserRole+1,QVariant(pScript->name.data()));
	pItem->setText(0,pScript->name.data());
	pItem->setToolTip(0,pScript->name.data());
}
void panel_sim_root::OnRemoveScript()
{
	QTreeWidgetItem *pItem = ui.twScripts->currentItem();
	if(pItem == NULL || pItem->data(0,Qt::UserRole).toInt() == 0)
		return;
	SString name = pItem->data(0,Qt::UserRole+1).toString().toStdString().data();
	if(!SQt::ShowQuestionBox("提示",SString::toFormat("您确定要删除脚本'%s'吗?",name.data()).data()))
		return;
	stuSimRunScript *pScript = m_pSimManager->m_pSimConfig->SearchRunScriptByName(name);
	if(pScript == NULL)
		return;
	m_pRoot->removeChild(pItem);
	m_pSimManager->m_pSimConfig->m_Scripts.remove(pScript);
}

bool panel_sim_root::CommandToStart()
{
	ui.pushButton_Start->setEnabled(false);
	m_pSimManager->m_MmsServer.OnProgressText("正在启动SV、GOOSE模拟器...");
	SQt::ProcessUiEvents();
	if(!m_pSimManager->Start())
	{
		//SQt::ShowErrorBox("错误","启动SV、GOOSE模拟器服务失败!");
		m_pSimManager->m_MmsServer.OnProgressText("启动SV、GOOSE模拟器服务失败!");
		ui.pushButton_Start->setEnabled(true);
		return false;
	}
	m_pSimManager->m_MmsServer.m_pSimConfig = m_pSimManager->m_pSimConfig;
	if(m_pSimManager->m_pSimConfig->m_bOpenMmsServer && SFile::exists(SBASE_SAPP->GetConfPath()+"1.scd"))
	{
#ifndef WIN32
		if(m_pSimManager->m_pSimConfig->m_bMmsSvrSimIpAddr)
			m_pSimManager->m_MmsServer.AddApIpAddr();
#endif

		SString scd=SBASE_SAPP->GetConfPath()+"1.scd";
		int id=0;
#if 1
		unsigned long pos;
		stuSclVtIed *pIed = m_pSimManager->m_pSimConfig->m_MmsServerIed.FetchFirst(pos);
		while(pIed)
		{
			m_pSimManager->m_MmsServer.AddIed(stuMmsServerIed::New(scd,++id,pIed->name,"S1"));
			pIed = m_pSimManager->m_pSimConfig->m_MmsServerIed.FetchNext(pos);
		}
#else
		int ret = m_pSimManager->m_MmsServer.AddIedByScd(scd);
		if(ret <0)
			CGuiApplication::GetApp()->NewEvent("系统",0,0,"","提示","加载SCD文件时失败!");
#endif
		CGuiApplication::GetApp()->NewEvent("系统",0,0,"","提示",SString::toFormat("成功加载%d个MMS访问点服务!",id));

		m_pSimManager->m_MmsServer.Start();		
		CGuiApplication::GetApp()->NewEvent("系统",0,0,"","提示",SString::toFormat("MMS服务启动完毕!"));
	}
	ui.pushButton_Stop->setEnabled(true);
	m_iThreads = 0;
	m_bStop = false;
	SKT_CREATE_THREAD(ThreadRunBcastScript,this);
	CGuiApplication::GetApp()->NewEvent("系统",0,0,"","提示","模拟器服务启动!");
	m_pSimManager->m_MmsServer.OnProgressText("模拟服务启动完毕!");
	return false;
}

bool panel_sim_root::CommandToStop()
{
	m_bStop = true;
	if(!m_pSimManager->Stop())
	{
		//SQt::ShowErrorBox("错误","停止模拟器服务失败!");
		return false;
	}
#ifndef WIN32
	m_pSimManager->m_MmsServer.RemoveApIpAddr();
#endif
	if(m_pSimManager->m_pSimConfig->m_bOpenMmsServer)
		m_pSimManager->m_MmsServer.StopAndWait();
	while(m_iThreads > 0)
		SApi::UsSleep(100000);
	ui.pushButton_Start->setEnabled(true);
	ui.pushButton_Stop->setEnabled(false);
	CGuiApplication::GetApp()->NewEvent("系统",0,0,"","提示","模拟器服务停止!");
	return true;
}