#ifndef _RCLICKMENU_H 
#define _RCLICKMENU_H
#include <QtGui/QMenu>
//#define FUN()
class Mmenu:public QMenu
{
public:
	enum menuType
	{
		NOITEM,//鼠标点在空白处
		FATHERITEM,//鼠标点在父节点
		CHILDITEM,//鼠标点在子节点
	};
	Mmenu(QWidget * parent = 0){}
	~Mmenu(void){}
	virtual void exec(QPoint pos){}
	virtual void regist(QObject * tar){}
	virtual	void setType(menuType mt){}
	QMenu * set(QMenu*qm){pMenu = qm;return pMenu;}
	QMenu * get(){return pMenu;};
public:
	QAction *aConnect;//连接
	QAction *aDelDb;//删除连接
	QAction *aDelTable;//删除表格
	QAction *aRegist;//注册事件
	QAction *aDisRegist;//删除注册
	QAction *aRegistAll;//全部注册
	QAction *aDisRegistAll;//删除全部注册
private:
	QMenu * pMenu;
};
class connectMenu:public Mmenu
{
	public:
	connectMenu(QWidget * parent,menuType mt);
	~connectMenu();
	void exec(QPoint pos);
	void regist(QObject * tar);
	void setType(menuType mt);
private:
};
class topMenu:public Mmenu
{
public:
	topMenu(QWidget * parent = 0);
	~topMenu();
	void exec(QPoint pos);
	void regist(QObject * tar);
private:

	QAction *aConnect;
	QAction *aDel;
};
#endif
