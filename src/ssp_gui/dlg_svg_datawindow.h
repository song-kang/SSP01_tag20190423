#ifndef __DLG_SVG_DATAWINDOW_H__
#define __DLG_SVG_DATAWINDOW_H__

#include <QWidget>
#include "ui_dlg_svg_datawindow.h"
#include "ssp_gui.h"
#include "ssp_datawindow.h"

class CDlgSvgDataWindow : public QWidget
{
	Q_OBJECT

public:
	CDlgSvgDataWindow(QWidget *parent = 0);
	~CDlgSvgDataWindow();
	void ShowDlg();
signals:
	void sigDatasetCell(const QString &dsname, int row,int col);
public slots:
	void currentIndexChangedClsName(int index);
	void currentIndexChangedName(int index);
	void textChangedFilter(const QString &text);
	void cellDoubleClickedDs(int row, int column);
	void onBtnCfg();
public:
private:
	void ShowDataset(CSsp_Dataset *pDS);
	Ui::dlg_svg_datawindow ui;
	SString m_sLastDsName;
};

#endif // __DLG_SVG_DATAWINDOW_H__
