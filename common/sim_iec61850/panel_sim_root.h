#ifndef PANEL_SIM_ROOT_H
#define PANEL_SIM_ROOT_H

#include <QWidget>
#include "ui_panel_sim_root.h"
#include "sim_manager.h"
#include "SQt.h"
#include "highlighter.h"
#include "SimScript.h"
class panel_sim_root : public QWidget
{
	Q_OBJECT

public:
	panel_sim_root(CSimManager *pSim,QWidget *parent = 0);
	~panel_sim_root();
	CSimManager *m_pSimManager;
	void LoadScriptTree();
	virtual void closeEvent(QCloseEvent * event);
	bool m_bStarted;

public slots:
	void Start();
	void Stop();
	void OnTreeCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void treeItemPressed(QTreeWidgetItem *item, int column);
	void OnNewScript();
	void OnRenameScript();
	void OnRemoveScript();
	void OnFcdaSearch();
	void OnBcastScript();
	void OnScriptStart();
	void OnScriptEnd();
	void OnScriptCheck();
	void OnSaveScript();
	void OnScriptStatus(stuSimRunScript* pScript);

	//专门针对智能告警测控工具响应命令操作使用 LDZ start
	bool CommandToStart();//启动服务
	bool CommandToStop();//停止服务
	////////////LED end
signals:
	void sigScriptStatus(stuSimRunScript*);
private:
	static void* ThreadRunBcastScript(void *lp);
	static void* ThreadRunScript(void *lp);
	Ui::panel_sim_root ui;
	QIcon m_iconRoot,m_iconRun,m_iconStop;
	QTreeWidgetItem *m_pRoot;
	Highlighter *m_highlighter;
	int m_iThreads;
	bool m_bStop;
};

#endif // PANEL_SIM_ROOT_H
