#ifndef VIEW_MYSQL_MDBCFG_H
#define VIEW_MYSQL_MDBCFG_H

#include "ssp_baseview.h"
#include <QWidget>
#include "ui_view_mysql_mdbcfg.h"
#include "SMySQL.h"
#include "SApi.h"
#include "SspDbRestore.h"

class view_mysql_mdbcfg : public CBaseView
{
	Q_OBJECT
public:
	view_mysql_mdbcfg(QWidget *parent = 0);
	~view_mysql_mdbcfg();
	static SString MySqlType2Mdb(SString sType);
	static SString MysqlKey2Mdb(SString sKey);
	static SString MySqlType2Oracle(SString sType);

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
	Ui::view_mysql_mdbcfg ui;
	SMySQL m_mysql;
	SXmlConfig m_XmlMdbCfg;
	CSspDbRestoreMysql m_DbRestore;
};

#endif // VIEW_MYSQL_MDBCFG_H
