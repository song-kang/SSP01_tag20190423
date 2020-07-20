#ifndef DLG_LOGIN_H
#define DLG_LOGIN_H

#include <QDialog>
#include "ui_dlg_svg_dw_setup.h"
#include "ssp_gui.h"
#include "ssp_datawindow.h"

class CDlgSvgDwSetup : public QDialog
{
	Q_OBJECT

public:
	CDlgSvgDwSetup(QWidget *parent = 0);
	~CDlgSvgDwSetup();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  显示指定的数据集
	// 作    者:  邵凯田
	// 创建时间:  2017-12-14 10:24
	// 参数说明:  @pDs为数据集指针
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void ShowDataset(CSsp_Dataset *pDs);
public slots:
	void cellDoubleClickedDW(int,int);
	void cellDoubleClickedSession(int,int);
	void itemChangedDW(QTableWidgetItem*);
	void itemChangedSession(QTableWidgetItem*);
public:
private:
	Ui::dlg_svg_dw_setup ui;
};

#endif // DLG_LOGIN_H
