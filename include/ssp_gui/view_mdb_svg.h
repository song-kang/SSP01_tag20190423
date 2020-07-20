/**
 *
 * 文 件 名 : view_mdb_svg.h
 * 创建日期 : 2015-8-4 13:09
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 映射到内存数据库的SVG图形监视窗口
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-8-4	邵凯田　创建文件
 *
 **/
#ifndef __SKT_VIEW_MDB_SVG_H__
#define __SKT_VIEW_MDB_SVG_H__

#include <QtGui>
#include "ssp_baseview.h"
#include "ssp_database.h"
#include "ssp_datawindow.h"
#include "SSvgWnd.h"

class SSP_GUI_EXPORT CViewMdbSvg;

enum eSvgItemMapType
{
	SVGMAP_TEXT=1,		//文本图元映射
	SVGMAP_STATE,		//简单状态图元映射
	SVGMAP_CHART_PIE,	//饼图图元映射
	SVGMAP_CHART_COL,	//柱图图元映射
	SVGMAP_CHART_LINE,	//折线图元映射
	SVGMAP_CHART_PANEL,	//仪表盘图元映射
	SVGMAP_DYN_RS_RECT,	//动态记录集映射区域
	SVGMAP_AUTO_FLASH,	//自动闪烁图元
};
struct stuSvgMapToRecord
{
	SSvgObject *pSvgObj;		//SVG对象
	CSsp_Dataset *pDataset;		//对应数据集
	int iRowIndex;				//对应的行号，从0开始编号
	int iColIndex;				//对应的列号，从0开始编号
	CSsp_Dataset *pFlashDataset;//自动闪烁对应数据集
	int iFlashRowIndex;			//自动闪烁对应的行号，从0开始编号
	int iFlashColIndex;			//自动闪烁对应的列号，从0开始编号
	//SString sRowPrimKey;		//对应行的主键值，多个主键通过","分隔，结尾不加","
	//SRecord *pRecord;			//映射图元对应的行
	SString sExtAttr;			//扩展属性
	eSvgItemMapType m_iSvgMapType;//图元映射类型
	//CSsp_DwColumn *pDsColumn;	//对应数据集中的列
};

class SSP_GUI_EXPORT CMdbSvgWnd : public SSvgWnd
{
public:
	friend class CViewMdbSvg;
	CMdbSvgWnd(QWidget *parent);
	virtual ~CMdbSvgWnd();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  对象点击响应
	// 作    者:  邵凯田
	// 创建时间:  2015-8-4 14:07
	// 参数说明:  @pSvgObj
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnClickObject(SSvgObject *pSvgObj);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取图元的提示文本，优先通过应用层回调函数提取，取不到返回图元的title文本
	// 作    者:  邵凯田
	// 创建时间:  2015-12-8 14:45
	// 参数说明:  @pSvgObj
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	virtual SString GetSvgObjectTipText(SSvgObject *pSvgObj);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  用户自绘虚函数，自绘操作仅针对矩形图元，并在扩展属性中含userdraw=true;
	// 作    者:  邵凯田
	// 创建时间:  2015-8-10 14:07
	// 参数说明:  @pDC为窗口DC，针对QT时通过(QPainter*)pDC->GetHandle()使用
	//         :  @rect为指针的绘制区域，绘制不可超出指针的区域范围
	//         :  @pObj表示绘制的对象指针，由用户层根据扩展属性确定绘制模块
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void DrawUser(SBaseDC *pDC,SRect &rect, SSvgObject *pObj);


	CViewMdbSvg *m_pViewMdbSvg;
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CViewMdbSvg
// 作    者:  邵凯田
// 创建时间:  2015-8-10 14:08
// 描    述:  映射MDB库的SVG视图
//////////////////////////////////////////////////////////////////////////
class SSP_GUI_EXPORT CViewMdbSvg : public CBaseView,public SSvgDataModel
{
	Q_OBJECT
public:
	friend class CMdbSvgWnd;

	CViewMdbSvg(int wnd_sn,QWidget *parent,bool bFloat=false,SString *pExtAttr=NULL);
	virtual ~CViewMdbSvg();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  当窗口显示后调用虚函数
	// 作    者:  邵凯田
	// 创建时间:  2015-8-4 14:27
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnAftShow();

	virtual void OnClickObject(SSvgObject *pSvgObj);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取图元的提示文本，优先通过应用层回调函数提取，取不到返回图元的title文本
	// 作    者:  邵凯田
	// 创建时间:  2015-12-8 14:45
	// 参数说明:  @pSvgObj
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	SString GetSvgObjectTipText(SSvgObject *pSvgObj);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置缺省数据集名称
	// 作    者:  邵凯田
	// 创建时间:  2015-8-6 15:58
	// 参数说明:  @str为数据集名称
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetDatasetName(SString str){m_sDatasetName = str;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置动态条件
	// 作    者:  邵凯田
	// 创建时间:  2015-8-6 16:16
	// 参数说明:  @str为条件字符串
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetCondition(SString str){m_sCondition = str;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取对象关联的显示文本
	// 作    者:  邵凯田
	// 创建时间:  2015-8-18 17:23
	// 参数说明:  @pObj为图元指针
	//         :  @sText为引用返回的文本内容
	// 返 回 值:  true表示取到关联数据，false表示未取到关联数据
	//////////////////////////////////////////////////////////////////////////
	virtual bool GetViewText(SSvgObject *pObj,SString &sText);

	virtual bool GetViewTextColor(SSvgObject *pObj,SCOLOR &sColor);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取对象关联的线条颜色
	// 作    者:  邵凯田
	// 创建时间:  2015-8-18 17:24
	// 参数说明:  @pObj为图元指针
	//         :  @sColor为引用返回的颜色
	// 返 回 值:  true表示取到关联数据，false表示未取到关联数据
	//////////////////////////////////////////////////////////////////////////
	virtual bool GetViewLineColor(SSvgObject *pObj,SCOLOR &sColor);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取对象关联的填充颜色
	// 作    者:  邵凯田
	// 创建时间:  2015-8-18 17:25
	// 参数说明:  @pObj为图元指针
	//         :  @bFill为引用返回的是否填充
	//         :  @sColor为引用返回的颜色
	// 返 回 值:  true表示取到关联数据，false表示未取到关联数据
	//////////////////////////////////////////////////////////////////////////
	virtual bool GetViewBackColor(SSvgObject *pObj,bool &bFill,SCOLOR &sColor);

	virtual void hideEvent(QHideEvent *event);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  将最后执行点击的SVG组态窗口重定向到另一个组态页面
	// 作    者:  邵凯田
	// 创建时间:  2016-3-1 18:14
	// 参数说明:  @wnd_sn为组态窗口序号
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	static void JumpWnd(int wnd_sn);

public slots:
	void OnTimerReloadSvg();
	void OnRefreshSvg();
	void OnTimerRepaint();


signals:
	void DoRefSvg();
private:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加图元与数据的映射关系
	// 作    者:  邵凯田
	// 创建时间:  2015-8-6 15:56
	// 参数说明:  @pSvgObj为图元指针
	//         :  @sExtAttr为其扩展属性
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void AddObjectMap(SSvgObject *pSvgObj,SString &sExtAttr);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  处理动态数据集区域，按数据集内容生成各行的元素并自动映射图元
	// 作    者:  邵凯田
	// 创建时间:  2015-11-16 8:36
	// 参数说明:  @pRootObj为根节点
	//         :  @pRectObj为区域对象
	// 返 回 值:  true表示有数据处理，false表示数据为空（部分节点被删除）需要重新开始遍历节点
	//////////////////////////////////////////////////////////////////////////
	bool ProcessDynRsRect(SSvgObject *pRootObj,SSvgObject *pRectObj);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  更新指针映射图元的属性
	// 作    者:  邵凯田
	// 创建时间:  2015-8-6 16:18
	// 参数说明:  @pMap为映射关系
	//         :  @pR为对应的行，NULL表示未知对应具体行
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void UpdateSvgState(stuSvgMapToRecord *pMap,SRecord *pR=NULL);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  数据集更新的回调函数
	// 作    者:  邵凯田
	// 创建时间:  2015-8-7 15:00
	// 参数说明:  @cbParam为this指针
	//         :  @pMdbClient为MDB客户端接口
	//         :  @pDataset为当前数据集
	//         :  @...
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static BYTE* OnMdbDsTrgCallback(void* cbParam, CMdbClient *pMdbClient,CSsp_Dataset *pDataset, eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  删除所有触发回调函数的注册
	// 作    者:  邵凯田
	// 创建时间:  2015-8-7 15:02
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void RealeaseAllTrgCallback();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  实时刷新SVG显示，不直接刷新，发送消息
	// 作    者:  邵凯田
	// 创建时间:  2015-8-7 15:30
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void RefreshSvg();

	inline CSsp_Dataset* SearchDynSvgMapDataset(CSsp_Dataset* pDs)
	{
		unsigned long pos=0;
		CSsp_Dataset *p = m_DynSvgMapDataset.FetchFirst(pos);
		while(p)
		{
			if(p->GetName() == pDs->GetName())
				return p;
			p = m_DynSvgMapDataset.FetchNext(pos);
		}
		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  加载SVG线程，用于后台加载，不影响GUI主线程
	// 作    者:  邵凯田
	// 创建时间:  2015-12-8 17:43
	// 参数说明:  @lp：this
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadReloadSvg(void *lp);

private:
	QVBoxLayout *m_verticalLayout_2;
	CMdbSvgWnd *m_pSvg;
	SSvgWnd *m_pTempSvg;//临时交换用的SVG实例
	QTimer *m_pTimer,*m_pTimer2;
	SString m_sDatasetName;//关联的缺省数据集名称
	SString m_sCondition;//动态条件
	SPtrList<CSsp_Dataset> m_SvgMapDataset;//SVG图元引用的数据集集合
	SPtrList<CSsp_Dataset> m_DynSvgMapDataset;//动态SVG图元引用的数据集集合，需要释放
	SPtrList<stuSvgMapToRecord> m_SvgItemMaps;//SVG图元与记录集中元素的映射关系集
	SPtrList<stuSvgMapToRecord> m_SvgItemAutoFlashMaps;//自动闪烁SVG图元与记录集中元素的映射关系集
	int m_iSvgUpdateSn;//SVG更新序号
	int m_iSvgShowSn;//SVG显示序号
	time_t m_LastRefresgSvgTime;
	bool m_bFloatWnd;//是否为浮动窗口
	//CViewMdbSvg *m_pParentView;//父窗口，浮动窗口中有效
	bool m_bLoadingSvg;//是否正在加载SVG
	SString m_sTempTip;
	char *m_pSvgText;
	int m_iSvgTextLen;
	int m_iDynRsRectCnt;
};

#endif//__SKT_VIEW_MDB_SVG_H__
