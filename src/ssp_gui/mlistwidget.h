/**
 *
 * 文 件 名 : mlistwidget.h
 * 创建日期 : 2015-7-30 15:06
 * 作    者 : 褚冰冰
 * 修改日期 : 2015-7-30 15:06
 * 当前版本 : 1.0
 * 功能描述 : 重写QListWidget
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-30 	褚冰冰　重写QListWidget
 *
 **/

#ifndef MLISTWIDGET_H
#define MLISTWIDGET_H

#include "SObject.h"
#include <QListWidget>

class mListWidget : public QListWidget
{
	Q_OBJECT

public:
	mListWidget(QWidget *parent=0);
	virtual ~mListWidget();
signals:
	void choosed(QListWidgetItem*item);
public slots:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  点击触发槽函数发送choosed信号
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 15:08
	// 参数说明:  @
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void OnItemClicked(QListWidgetItem*item);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  选择发生变化槽函数发送choosed信号
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 15:09
	// 参数说明:  @
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void OnCurrentItemChanged(QListWidgetItem * current, QListWidgetItem * previous);
private:
	
};

#endif // MLISTWIDGET_H
