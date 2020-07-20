#include "RClickMenu.h"


connectMenu::connectMenu(QWidget * parent,menuType mt)
{


	aConnect = new QAction(QString::fromLocal8Bit("添加连接"),parent);
	aDelDb = new QAction(QString::fromLocal8Bit("删除连接"),parent);
	aDelTable = new QAction(QString::fromLocal8Bit("删除表(&D)"),parent);
	aRegist = new QAction(QString::fromLocal8Bit("注册触发(&R)"),parent);
	aDisRegist = new QAction(QString::fromLocal8Bit("删除触发(&T)"),parent);
	aRegistAll = new QAction(QString::fromLocal8Bit("全部注册触发(&Z)"),parent);
	aDisRegistAll = new QAction(QString::fromLocal8Bit("删除全部触发(&C)"),parent);
	set(new QMenu(parent));
	get()->addAction(aConnect);
	get()->addAction(aDelDb);
	get()->addAction(aDelTable);
	get()->addAction(aRegist);
	get()->addAction(aDisRegist);
	get()->addAction(aRegistAll);
	get()->addAction(aDisRegistAll);
	switch(mt)
	{
	case NOITEM:
		aDelDb->setEnabled(false);
		aDelTable->setEnabled(false);
		//get()->removeAction(aDelDb);
		//get()->removeAction(aDelTable);
		break;
	case FATHERITEM:
		aDelTable->setEnabled(false);
		//get()->removeAction(aDelTable);
		break;
	case CHILDITEM:
		aDelDb->setEnabled(false);
		//get()->removeAction(aDelDb);
		break;
	}

}

void connectMenu::exec(QPoint pos)
{
	get()->exec(pos);
}
void connectMenu::regist(QObject * tar)
{
	QObject::connect(aConnect,SIGNAL(triggered()),tar,SLOT(connect()));
	QObject::connect(aDelDb,SIGNAL(triggered()),tar,SLOT(delDb()));
	QObject::connect(aDelTable,SIGNAL(triggered()),tar,SLOT(delTable()));
	QObject::connect(aRegist,SIGNAL(triggered()),tar,SLOT(registCallback()));
	QObject::connect(aDisRegist,SIGNAL(triggered()),tar,SLOT(removeRegistCall()));	
	QObject::connect(aRegistAll,SIGNAL(triggered()),tar,SLOT(registAllCallback()));
	QObject::connect(aDisRegistAll,SIGNAL(triggered()),tar,SLOT(removeAllRegistCall()));
}
void connectMenu::setType(menuType mt)
{
	get()->removeAction(aDelDb);
	get()->removeAction(aDelTable);
	get()->removeAction(aConnect);
	get()->removeAction(aRegist);
	get()->removeAction(aDisRegist);
	get()->removeAction(aRegistAll);
	get()->removeAction(aDisRegistAll);
	aConnect->setEnabled(true);
	aDelDb->setEnabled(true);
	aDelTable->setEnabled(true);
	switch(mt)
	{
	case NOITEM:
		get()->addAction(aConnect);
		aDelDb->setEnabled(false);
		aDelTable->setEnabled(false);
		break;
	case FATHERITEM:
		get()->addAction(aDelDb);
		aDelTable->setEnabled(false);
		aConnect->setEnabled(false);
		get()->addAction(aRegistAll);
		get()->addAction(aDisRegistAll);
		break;
	case CHILDITEM:
		get()->addAction(aDelTable);
		get()->addAction(aRegist);
		get()->addAction(aDisRegist);
		aDelDb->setEnabled(false);
		break;
	}

}
connectMenu::~connectMenu(void)
{
	delete get();
	delete aConnect;
	delete aDelDb;
	delete aDelTable;
}
