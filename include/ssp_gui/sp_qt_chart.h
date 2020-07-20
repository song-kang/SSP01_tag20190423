/**
 *
 * 文 件 名 : sp_qt_chart.h
 * 创建日期 : 2015-8-10 14:14
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 基于QT的图表绘制函数接口
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-8-10	邵凯田　创建文件
 *
 **/

#ifndef __SP_QT_CHART_H__
#define __SP_QT_CHART_H__

#include "SDatabase.h"
#include "qt/SQt.h"
#include "gui/SBaseDC.h"
#include "ssp_gui_inc.h"

class SSP_GUI_EXPORT CSpQtChartPainter
{
public:
	CSpQtChartPainter();
	~CSpQtChartPainter();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  绘制饼图
	// 作    者:  
	// 创建时间:  2015-8-10 14:18
	// 参数说明:  @pPainter为QT画板对象
	//         :  @rect为绘制区域，绘制行为不可超出该区域
	//         :  @pRs为需要显示的数据集
	//         :  @sTitle为图表的显示标题，当sTitle为空时取第二列的列标题
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	static void DrawChartPie(QPainter *pPainter,SRect &rect,SRecordset *pRs,SString sTitle="",QColor corText=Qt::black,QColor corLine=Qt::gray,SString sExtAttr="");

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  绘制柱图
	// 作    者:  
	// 创建时间:  2015-8-10 14:18
	// 参数说明:  @pPainter为QT画板对象
	//         :  @rect为绘制区域，绘制行为不可超出该区域
	//         :  @pRs为需要显示的数据集
	//         :  @sTitle为图表的显示标题，当sTitle为空时取第二列的列标题
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	static void DrawChartColumn(QPainter *pPainter,SRect &rect,SRecordset *pRs,SString sTitle="",QColor corText=Qt::black,QColor corLine=Qt::gray,SString sExtAttr="");

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  绘制折线图
	// 作    者:  
	// 创建时间:  2015-8-10 14:18
	// 参数说明:  @pPainter为QT画板对象
	//         :  @rect为绘制区域，绘制行为不可超出该区域
	//         :  @pRs为需要显示的数据集
	//         :  @sTitle为图表的显示标题，当sTitle为空时取第二列的列标题
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	static void DrawChartLine(QPainter *pPainter,SRect &rect,SRecordset *pRs,SString sTitle="",QColor corText=Qt::black,QColor corLine=Qt::gray,SString sExtAttr="");


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  绘制仪表图
	// 作    者:  
	// 创建时间:  2015-12-11 14:18
	// 参数说明:  @pPainter为QT画板对象
	//         :  @rect为绘制区域，绘制行为不可超出该区域
	//         :  @pRs为需要显示的数据集
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	static void DrawPanel(QPainter *pPainter,SRect &rect,float currentValue,SString sExtAttr);
private:
	static QColor ScorToQcor(SString srgb);
	static QColor StrToQcor(SString srgb);
};

#endif//__SP_QT_CHART_H__