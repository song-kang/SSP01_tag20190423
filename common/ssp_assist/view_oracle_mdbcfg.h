#ifndef VIEW_ORACLE_MDBCFG_H
#define VIEW_ORACLE_MDBCFG_H

#include "ssp_baseview.h"
#include <QWidget>
#include "ui_view_oracle_mdbcfg.h"
#include "SOracle.h"
#include "SApi.h"
#include "SspDbRestore.h"

class view_oracle_mdbcfg : public CBaseView
{
	Q_OBJECT
public:
	view_oracle_mdbcfg(QWidget *parent = 0);
	~view_oracle_mdbcfg();
	static SString OracleType2Mdb(SString sType);
	static SString OracleKey2Mdb(SString sKey);
	static SString OracleType2Mysql(SString sType);

public slots:
	void OnConnDB();
	void OnNewCfg();
	void OnLoadCfg();
	void OnSaveCfg();
	void OnExportStruct();
	void OnExportSQL();
	void OnImportData_SDB();
	void OnExportData_SDB();
	void OnExportSQL_MYSQL();
	void OnExportSQL_ORACLE();
	void slotClickTreeChkbox(QTreeWidgetItem*,int);

	void OnLoadTables();
private:
	Ui::view_oracle_mdbcfg ui;
	SOracle m_oracle;
	SXmlConfig m_XmlMdbCfg;
	CSspDbRestoreOracle m_DbRestore;
};

#endif // VIEW_MYSQL_MDBCFG_H
