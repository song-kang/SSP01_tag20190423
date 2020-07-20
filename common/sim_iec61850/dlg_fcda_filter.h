#ifndef DLG_FCDA_FILTER_H
#define DLG_FCDA_FILTER_H

#include <QWidget>
#include <QTextEdit>
#include "ui_dlg_fcda_filter.h"
#include "sim_config.h"
#include "SimMmsServer.h"
class dlg_fcda_filter : public QWidget
{
	Q_OBJECT

public:
	dlg_fcda_filter(QWidget *parent = 0);
	~dlg_fcda_filter();
	CSimConfig *m_pSimConfig;
	CSimMmsServer *m_pSimMmsServer;
	QTextEdit *m_pTextEdit;
public slots:
	void OnNameChanged(const QString &str);
	void OnTableDbClicked(int row,int col);
private:
	Ui::dlg_fcda_filter ui;
};

#endif // DLG_FCDA_FILTER_H
