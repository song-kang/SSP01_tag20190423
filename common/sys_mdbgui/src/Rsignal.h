#pragma once
#include <QObject>
#include "SApi.h"
enum  UPDATETYPE
{
	CREATE,
	DROP,
	SELECTALL
};
class Rsignal:public QObject
{
	Q_OBJECT
public:
	Rsignal(QObject *parent = 0);
	~Rsignal(void);
	virtual void sendSiganl(void *mData,bool b_select){}
	virtual void sendSiganl(void *mData,UPDATETYPE type){}
signals:
	void runsql(SString sSql,bool b_select);//发送执行的语句
	void updateTree(SString sTable,UPDATETYPE type);//更新treewidget
};

class mRsignal:public Rsignal
{
	Q_OBJECT
public:
	mRsignal() {}
	~mRsignal(){}
	void sendSiganl(void *mData,bool b_select)
	{
		SString sSql = *((SString *)mData);
		emit runsql(sSql,b_select);
	}
	void sendSiganl(void *mData,UPDATETYPE type)
	{
		SString sTable = *((SString *)mData);
		emit updateTree(sTable,type);
	}
};