#ifndef VIEW_QUERY_H
#define VIEW_QUERY_H
/**
 *
 * 文 件 名 : view_query.h
 * 创建日期 : 2015-8-7 11:04
 * 作    者 : 郁文枢
 * 修改日期 : 2015-8-7 11:04
 * 当前版本 : 1.0
 * 功能描述 : 查询窗口
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-8-7 	郁文枢　查询窗口
 *
 **/
#include <QWidget>
#include "ui_view_query.h"
#include "ssp_baseview.h"
#include "db/mysql/SMySQL.h"
#include "dbInteraction.h"
#include "include_macdef.h"
#include "structDefine.h"
#include "ssp_datawindow.h"
#include "mtreewidget.h"
#include "mcombobox.h"
#include "record_sql.h"
#include "clearData.h"
#include "qt/SQt.h"
#include "view_dw_condlg.h"

class view_query : public CBaseView
{
	Q_OBJECT

public:
	view_query(QWidget *parent = 0,SString sFunName="",bool isMDB=false);
	~view_query();
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  初始化数据窗口
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 11:13
	// 参数说明:  @sWname 配置文件中dataset引用名
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void InitDataWindow(SString sWname);
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据不同菜单项初始化窗口
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 11:16
	// 参数说明:  @sWname 配置文件中dataset引用名
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	bool InitWidget(SString sWname);
	bool SetTableValue(int row,int col,QString value);//通过行列设置表格值
	int getTableRowByFcol(QString colValue);//通过第一列（假定唯一）找到行号
	void OnPreShow();
	bool refreshReady;
private slots:
	void onPushButtonPressed();//查询按钮响应
	void onPushButton2Pressed();//打印按钮响应
	void onPushButton3Pressed();//导出按钮响应
	void queryTable();//得到查询返回信息并显示结果
	void timerOut();
	void on_tableWidget_itemClicked(QTableWidgetItem * item);
private:
	Ui::view_query ui;
	SDatabasePool<SMySQL> *m_pPool;
	SDatabaseOper *m_pOper;
	SString m_tableName;//表名
	SString m_refName;//索引名
	SString m_pre;//修改之前的值
	bool m_bInsert;//是否是
	CSsp_DwColumn *m_fieldInfo;//字段信息
	SPtrList<CSsp_DwColumn> *m_fieldlist;//所有字段信息
	vector<SString> m_fieldNamelist;//字段原本名称列表
	int m_nPkey;//主键数字索引
	int m_tableRow;//不算上插入行的行数
	CSsp_DatawindowMgr *m_dataWindow;//数据窗口管理对象
	QWidget * m_widget;
	SRecordset* m_Record;
	record_sql m_sqlRecord;//命令集
	view_dw_condlg *dlg;
	bool isMDB;//是否是内存数据库
	bool m_bNotPreShow;
	bool m_bCondConfirm;//是否条件已经明确，无条件时为true，条件确定过一次也为true，否则为false
	QTimer *timer;
	QFont m_font_underline;

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  将通用表初始化到通用窗口
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 14:37
	// 参数说明:  @
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	SPtrList<CSsp_DwColumn> *getFieldInfo(SString sWname,SPtrList<CSsp_DwColumn> *cFinfo=NULL);
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  将通用表初始化到通用窗口
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 14:37
	// 参数说明:  @查询信息，个数为0不查询
	// 返 回 值:  
	// 修 改 者:  郁文枢
	// 修改时间:  2015-8-8 11:15
	// 修改描述:  修改行添加方式，添加过滤查询内容等
	//////////////////////////////////////////////////////////////////////////
	void SetCommonToWidget(CSsp_Dataset* dbSet);
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从sql语句中获取表名
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 14:47
	// 参数说明:  @sql 语句（只支持单张表语句）
	// 返 回 值:  表名
	//////////////////////////////////////////////////////////////////////////
	SString GetTableFromSql(SString sSql);
	
	static BYTE* OnMdbDatasetTrgCallback(void* cbParam, CMdbClient *pMdbClient,CSsp_Dataset *pDataset, eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData);//回调函数
};

#endif // VIEW_QUERY_H
