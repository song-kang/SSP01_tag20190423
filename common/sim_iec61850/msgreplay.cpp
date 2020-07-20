#include "msgreplay.h"

MsgReplay::MsgReplay(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	m_dlg=new MsgReplay_taskdlg(this);
	connect(ui.pushButton_add,SIGNAL(clicked()),this,SLOT(onpushButton_add()));
	connect(ui.pushButton_del,SIGNAL(clicked()),this,SLOT(onpushButton_del()));
	ui.tableWidget->setColumnCount(2);
	ui.tableWidget->setRowCount(0);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget->horizontalHeader()->hide();
	ui.tableWidget->verticalHeader()->hide();
	ui.tableWidget->hideColumn(1);
	//ui.tableWidget->setColumnWidth(0,1000);
}

MsgReplay::~MsgReplay()
{

}
void MsgReplay::onpushButton_add()
{
	m_dlg->exec();
	MsgReplayStruct *msg=m_dlg->getMsg();
	if(msg!=NULL)
	{
		int rowCount=ui.tableWidget->rowCount();
		ui.tableWidget->insertRow(rowCount);
		QWidget *w=new QWidget(this);
		QLabel *no=new QLabel(this);
		no->setText(QString::number(ui.tableWidget->rowCount()));
		QLabel *name =new QLabel(this);
		name->setText(msg->filePath);
		QLabel *msgNum=new QLabel(this);
		msgNum->setText(tr("报文数量:0"));
		QLabel *byteNum=new QLabel(this);
		byteNum->setText(tr("总字节数:0"));
		QLabel *realFlow=new QLabel(this);
		realFlow->setText(tr("实时流量:0"));
		QPushButton *btn_continue=new QPushButton(this);
		btn_continue->setVisible(false);
		btn_continue->setText(tr("继续"));
		QPushButton *btn_wait=new QPushButton(this);
		btn_wait->setVisible(false);
		btn_wait->setText(tr("暂停"));
		QPushButton *btn_del=new QPushButton(this);
		btn_del->setVisible(false);
		btn_del->setText(tr("终止"));
		QHBoxLayout *hb=new QHBoxLayout();
		hb->addWidget(no);
		hb->addWidget(name);
		hb->addWidget(msgNum);
		hb->addWidget(byteNum);
		hb->addWidget(realFlow);
		hb->addWidget(btn_continue);
		hb->addWidget(btn_wait);
		hb->addWidget(btn_del);
		btn_continue->hide();
		hb->addStretch();
		hb->setMargin(4);
		w->setLayout(hb);
		ui.tableWidget->setCellWidget(rowCount,0,w);
		connect(btn_continue,SIGNAL(clicked()),this,SLOT(onBtn_continue()));
		connect(btn_wait,SIGNAL(clicked()),this,SLOT(onBtn_wait()));
		connect(btn_del,SIGNAL(clicked()),this,SLOT(onBtn_del()));

		int noMax=getNoMax()+1;
		QTableWidgetItem *item=new QTableWidgetItem(QString::number(noMax));
		ui.tableWidget->setItem(rowCount,1,item);
		MsgThreadStruct threadStruct;
		threadStruct.no=noMax;
		threadStruct.con=new QWaitCondition();
		threadStruct.replayThread=new MsgReplayThread(noMax,threadStruct.con,msg,this);
		threadStruct.thread=new QThread();
		
		connect(threadStruct.thread, SIGNAL(finished()), threadStruct.replayThread, SLOT(deleteLater()));
		connect(this, SIGNAL(operate(int)), threadStruct.replayThread,SLOT(run(int)));
		connect(this, SIGNAL(finished()), threadStruct.replayThread,SLOT(finished()));
		connect(this,SIGNAL(setSleep(int)),threadStruct.replayThread,SLOT(setsleep(int)));
		connect(this,SIGNAL(stopThread(int)),threadStruct.replayThread,SLOT(stopThread(int)),Qt::DirectConnection);
		connect(this,SIGNAL(setWake(int)),threadStruct.replayThread,SLOT(setWake(int)));
		threadStruct.replayThread->moveToThread(threadStruct.thread);
		threadStruct.thread->start();
		emit operate(noMax);
		m_listmsg.append(threadStruct);
	}	
}
int MsgReplay::getNoMax()
{
	int noMax=0;
	for(int i=0;i<m_listmsg.size();i++)
	{
		if(noMax<m_listmsg.at(i).no)
			noMax=m_listmsg.at(i).no;
	}
	return noMax;
}
void MsgReplay::onpushButton_del()
{
	int curRow=ui.tableWidget->currentRow();
	if(curRow<0)
		return;
	int i_ret = QMessageBox::warning(this,tr("警告"),tr("是否删除第")+QString::number(curRow+1)+tr("行？"),QMessageBox::Yes,QMessageBox::No);
	if (i_ret==QMessageBox::No)
	{
		return;
	}
	for(int i=curRow;i<ui.tableWidget->rowCount();i++)
	{
		QLabel *label=((QLabel*)ui.tableWidget->cellWidget(i,0)->layout()->itemAt(0)->widget());
		int no=label->text().toInt();
		label->setText(QString::number(no-1));
	}

	MsgThreadStruct threadStruct=m_listmsg.takeAt(curRow);
	emit stopThread(threadStruct.no);
	threadStruct.thread->terminate();
	threadStruct.thread->deleteLater();
	//delete threadStruct.thread;
	ui.tableWidget->removeRow(curRow);
	
	delete threadStruct.con;
	threadStruct.replayThread->deleteLater();
	//delete threadStruct.replayThread;
}
void MsgReplay::onBtn_continue()
{
// 	int num=((QLabel*)((QWidget*)sender()->parent())->layout()->itemAt(0)->widget())->text().toInt()-1;
// 	int no=ui.tableWidget->item(num,1)->text().toInt();
//  	if(no>0)
//  	{
// 		for(int i=0;i<m_listmsg.size();i++)
// 		{
// 			if(m_listmsg.at(i).no==no)
// 			{
//  				m_listmsg.at(i).con->wakeOne();
// 				break;
// 			}
// 		}
//  	}
}
void MsgReplay::onBtn_wait()
{
	int num=((QLabel*)((QWidget*)sender()->parent())->layout()->itemAt(0)->widget())->text().toInt()-1;
	int no=ui.tableWidget->item(num,1)->text().toInt();
	QPushButton *btn=((QPushButton*)((QWidget*)sender()->parent())->layout()->itemAt(6)->widget());
	if(no>=0)
	{
		if(btn->text()==tr("暂停"))
		{
			emit (setSleep(no));
			btn->setText(tr("继续"));
		}
		else if(btn->text()==tr("继续"))
		{
			for(int i=0;i<m_listmsg.size();i++)
			{
				if(m_listmsg.at(i).no==no)
				{
					m_listmsg.at(i).con->wakeOne();
					break;
				}
			}
			btn->setText(tr("暂停"));
		}
		//m_listmsg.at(no)->thread->setSleep();
	}
}
void MsgReplay::onBtn_del()
{
	int num=((QLabel*)((QWidget*)sender()->parent())->layout()->itemAt(0)->widget())->text().toInt()-1;
	int no=ui.tableWidget->item(num,1)->text().toInt();
	if(no>=0)
	{
		emit stopThread(no);
		for(int i=0;i<m_listmsg.size();i++)
		{
			if(m_listmsg.at(i).no==no)
			{
				m_listmsg.at(i).thread->terminate();
				break;
			}
		}
	}
}
void MsgReplay::refreshMsgNum(int no,int count,int byteCount,int ts)
{
	int num=0;
	if(ui.tableWidget->cellWidget(num,0)==NULL)
		return;
	for(int i=0;i<ui.tableWidget->rowCount();i++)
	{
		if(ui.tableWidget->item(i,1)->text().toInt()==no)
		{
			num=i;
			break;
		}
	}
	if(count == -100)
	{
		QLabel *lab = (QLabel*)ui.tableWidget->cellWidget(num,0)->layout()->itemAt(4)->widget();
		lab->setText(lab->text()+" 发送完毕!");
		return;
	}
	((QLabel*)ui.tableWidget->cellWidget(num,0)->layout()->itemAt(2)->widget())->setText(tr("报文数量:")+QString::number(count));
	((QLabel*)ui.tableWidget->cellWidget(num,0)->layout()->itemAt(3)->widget())->setText(tr("总字节数:")+QString::number(byteCount));
	if(ts!=0)
		((QLabel*)ui.tableWidget->cellWidget(num,0)->layout()->itemAt(4)->widget())->setText(tr("实时流量:")+QString::number(byteCount/(ts))+tr("B/s"));
}
//////////////////////////////////////////////////////////////////
MsgReplayThread::MsgReplayThread(int no,QWaitCondition *con,MsgReplayStruct *msg,QObject *parent)
{
	this->msg=msg;
	this->con=con;
	this->no=no;
	isStop=false;
	this->parent=parent;
	connect(this,SIGNAL(msgNum(int,int,int,int)),parent,SLOT(refreshMsgNum(int,int,int,int)));
}
MsgReplayThread::~MsgReplayThread()
{
	//disconnect(this,SIGNAL(msgNum(int,int,int,int)),parent,SLOT(refreshMsgNum(int,int,int,int)));
}
void MsgReplayThread::stopThread(int no)
{
	if(this->no==no)
	{
		isStop=true;
		disconnect(this,SIGNAL(msgNum(int,int,int,int)),parent,SLOT(refreshMsgNum(int,int,int,int)));
	}
}
void MsgReplayThread::setsleep(int no)
{
	if(this->no==no)
	{
		mutex.lock();
		con->wait(&mutex);
		mutex.unlock();
	}

}
void MsgReplayThread::setWake(int no)
{
	if(this->no==no)
		con->wakeOne();
}

void MsgReplayThread::finished()
{
	
}

void MsgReplayThread::run(int no)
{
	if(this->no!=no)
		return;
	SRawSocketSend sock;
	CPcapFile pcap;
	QDateTime startTime=QDateTime::currentDateTime();
	sock.Create(msg->port.toStdString().data());
	BYTE *pBuffer=NULL;
	int iLen,iBufLen=0;
	s_pcap_pkthdr last_t;
	uint ts;
	int times=msg->sendType;
	const char*  ch;
// 	QByteArray ba=msg->filePath.toLatin1();    
// 	ch=ba.data();
	std::string str = msg->filePath.toStdString();
	ch = str.c_str();
	char *buf = new char[strlen(ch)+1];
	strcpy(buf,ch);
	if(!pcap.OpenEx(buf))
	{
		return;
	}
	delete [] buf;
	int count=0;
	int byteCount=0;
	int tsCount=0;
	int first_soc=0,first_usec=0;
	int first_ts_soc=0,first_ts_usec;
	int last_soc,last_usec;
	int soc,usec;
	SDateTime::getSystemTime(last_soc,last_usec);
	SDateTime::getSystemTime(soc,usec);
	int local_ts;
	for(int i=0;i<times||times==-2;i++)
	{
		first_soc = 0;
		memset(&last_t,0,sizeof(last_t));
		pcap.GotoFirstEx();
		if(isStop==true)
		{
			LOGWARN("is stoped!");
			break;
		}
		while(1)
		{
			if(!pcap.ReadFrameEx(pBuffer,iLen,iBufLen))
			{
				LOGWARN("ReadFrameEx error!");
				break;
			}
			if(isStop==true)
			{
				LOGWARN("is stoped!");
				break;
			}
			QCoreApplication::processEvents();
			if(pcap.m_iTimeType == 1)
				ts = (pcap.m_pp.ts.tv_sec-first_ts_soc)*1000000+(pcap.m_pp.ts.tv_usec-first_ts_usec);
			else
				ts = (pcap.m_pp.ts.tv_sec-first_ts_soc)*1000000+(pcap.m_pp.ts.tv_usec-first_ts_usec)/1000;
			SDateTime::getSystemTime(soc,usec);
			local_ts = (soc-first_soc)*1000000+(usec-first_usec);
			if(first_soc == 0)
			{
				first_soc = soc;
				first_usec = usec;

				first_ts_soc = pcap.m_pp.ts.tv_sec;
				if(pcap.m_iTimeType == 1)
					first_ts_usec = pcap.m_pp.ts.tv_usec;
				else
					first_ts_usec = pcap.m_pp.ts.tv_usec/1000;
			}
			else if(ts>local_ts)
			{
				SApi::UsSleep(ts - local_ts);
			}
			int num=sock.SendRaw(pBuffer,iLen);
			if(num != iLen)
			{
				LOGERROR("发送报文(len:%d)失败!ret=%d",iLen,num);
			}
			int t=QDateTime::currentDateTime().secsTo(startTime);
			if(num>0)
			{
				count++;byteCount+=num;tsCount+=t;
				emit msgNum(no,count,byteCount,-t);
			}
		}
		//SApi::UsSleep(3000000);
	}
	emit msgNum(no,-100,0,0);
}
/////////////////////////////////////////////////////////////////
MsgReplay_taskdlg::MsgReplay_taskdlg(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("新建"));
	sendLabel=new QLabel(this);
	sendLabel->setText(tr("发送方式"));
	sendCbo=new QComboBox(this);
	inputNumName=new QLabel(this);
	inputNumName->setText(tr("发送次数"));
	inputNum=new QLineEdit(this);
	inputNum->setMaximumWidth(40);
	fileLabel=new QLabel(this);
	fileLabel->setText(tr("选择文件"));
	fileLine=new QLineEdit(this);
	filePath=new QPushButton(this);
	filePath->setText("...");
	portLabel=new QLabel(this);
	portLabel->setText(tr("端口"));
	portCbo=new QComboBox(this);
	Ok=new QPushButton(this);
	Ok->setText(tr("确定"));
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/image/ok"), QSize(), QIcon::Normal, QIcon::Off);
	Ok->setIcon(icon);

	cancel=new QPushButton(this);
	cancel->setText(tr("取消"));
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/image/cancel"), QSize(), QIcon::Normal, QIcon::Off);
	cancel->setIcon(icon1);
	msg=NULL;
	QHBoxLayout *sendlayout=new QHBoxLayout();
	QHBoxLayout *filelayout=new QHBoxLayout();
	QHBoxLayout *portlayout=new QHBoxLayout();
	QHBoxLayout *btnlayout=new QHBoxLayout();
	sendlayout->addWidget(sendLabel);
	sendlayout->addWidget(sendCbo);
	sendlayout->addWidget(inputNumName);
	sendlayout->addWidget(inputNum);
	inputNum->setEnabled(false);
	sendlayout->addStretch();
	filelayout->addWidget(fileLabel);
	filelayout->addWidget(fileLine);
	filelayout->addWidget(filePath);
	portlayout->addWidget(portLabel);
	portlayout->addWidget(portCbo);
	btnlayout->addStretch();
	btnlayout->addWidget(Ok);
	btnlayout->addWidget(cancel);
	QVBoxLayout *layout=new QVBoxLayout();
	layout->addLayout(sendlayout);
	layout->addLayout(filelayout);
	layout->addLayout(portlayout);
	layout->addLayout(btnlayout);
	setLayout(layout);
	initDlg();
}
MsgReplay_taskdlg::~MsgReplay_taskdlg()
{

}
void MsgReplay_taskdlg::initDlg()
{
	sendCbo->addItem(tr("单次发送"));
	sendCbo->addItem(tr("多次发送"));
	sendCbo->addItem(tr("循环发送"));
	sendCbo->setCurrentIndex(0);
	connect(sendCbo,SIGNAL(currentIndexChanged(QString)),this,SLOT(onsendCboChanged(QString)));
//#ifdef WIN32
	SPtrList<SRawSocketRecv::stuPcapDevice> list;
	SRawSocketRecv::FindAllPcapDevices(list);
#ifndef WIN32
	if(list.count() == 0)
	{
		SRawSocketRecv::SetEthNamePrex("em");
		SRawSocketRecv::FindAllPcapDevices(list);
	}
#endif
	for(int i=0;i<list.count();i++)
	{
		portCbo->addItem(QString("%1 : %2").arg((list.at(i))->ip.data()).arg((list.at(i))->desc.data()));
		portCbo->setItemData(i,QString((list.at(i))->name.data()));
	}
// #else
// 	portCbo->addItem("eth0");
// 	portCbo->addItem("eth1");
// 	portCbo->addItem("eth2");
// 	portCbo->addItem("eth3");
// #endif
	portCbo->setCurrentIndex(0);
	connect(portCbo,SIGNAL(currentIndexChanged(QString)),this,SLOT(onportCboChanged(QString)));
	connect(filePath,SIGNAL(clicked()),this,SLOT(onfilePath()));

	connect(Ok,SIGNAL(clicked()),this,SLOT(onOk()));

	connect(cancel,SIGNAL(clicked()),this,SLOT(onCancel()));
}
void MsgReplay_taskdlg::onsendCboChanged(QString text)
{
	if(text==tr("单次发送")||text==tr("循环发送"))
	{
		inputNum->setText("");
		inputNum->setEnabled(false);
	}
	else if(text==tr("多次发送"))
	{
		inputNum->setEnabled(true);
	}
}
void MsgReplay_taskdlg::onportCboChanged(QString text)
{
	S_UNUSED(text);
}
void MsgReplay_taskdlg::onfilePath()
{
	QString path=QFileDialog::getOpenFileName(this,"打开文件","","*.pcap",0,0);
	fileLine->setText(path);
}
void MsgReplay_taskdlg::onOk()
{
	if(portCbo->currentIndex() < 0)
		return;
	msg=new MsgReplayStruct();
	msg->filePath=fileLine->text();
	if(sendCbo->currentIndex()==0)
	{
		msg->sendType=1;
	}
	else if(sendCbo->currentIndex()==1)
	{
		msg->sendType=inputNum->text().toInt();
	}
	else if(sendCbo->currentIndex()==2)
	{
		msg->sendType=-2;
	}
	msg->port= portCbo->itemData(portCbo->currentIndex()).toString();
	hide();
}
void MsgReplay_taskdlg::onCancel()
{
	msg=NULL;
	hide();
}
MsgReplayStruct* MsgReplay_taskdlg::getMsg()
{
	return msg;
}