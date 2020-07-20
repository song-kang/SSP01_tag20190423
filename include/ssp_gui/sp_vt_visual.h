/**
 *
 * 文 件 名 : sp_vt_visual.h
 * 创建日期 : 2016-1-8 10:13
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 虚端子相关可视化实现
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-1-8	邵凯田　创建文件
 *
 **/

#ifndef __SP_VIRTUAL_TERMINAL_VISUAL_H__
#define __SP_VIRTUAL_TERMINAL_VISUAL_H__

#include "sp_virtual_terminal.h"
#include "SSvgWnd.h"
#include "ssp_gui_inc.h"

/*
URI定义：
home							表示回首页按钮
subnet.<subnet_name>			表示一个通讯子网
ied.<ied_name>					表示IED对象
svcb.<ied_name>$0x<appid>		表示指定IED的指定APPID的SV输出数据集
gocb.<ied_name>$0x<appid>		表示指定IED的指定APPID的GOOSE输出数据集
svin.<out_ied_name>$<out_smv_no>$<in_ied_name>  表示SV数据集发布与订阅
goin.<out_ied_name>$<out_gse_no>$<in_ied_name>  表示GOOSE数据集发布与订阅
svchin.<out_ied_name>$<out_smv_no>$<in_ied_name>$<in_smv_no>  表示SV数据集发布与订阅
gochin.<out_ied_name>$<out_gse_no>$<in_ied_name>$<in_smv_no>  表示GOOSE数据集发布与订阅
v_line							表示切换为虚链路图
v_terminal						表示切换为虚端子图
*/

//////////////////////////////////////////////////////////////////////////
// 名    称:  CSpVtSvgGenerator
// 作    者:  邵凯田
// 创建时间:  2016-1-8 10:18
// 描    述:  SVG生成类
//////////////////////////////////////////////////////////////////////////
class SSP_GUI_EXPORT CSpVtSvgGenerator
{
public:
	struct SSP_GUI_EXPORT stuHistoryLine
	{
		float x1,y1;
		float x2,y2;
	};
	struct stuSvgVtCbIedItem;
	struct SSP_GUI_EXPORT stuSvgVtCbIed
	{
		stuSvgVtCbIed()
		{
			items.setAutoDelete(true);
		}
		~stuSvgVtCbIed()
		{
			items.clear();
		}
		stuSclVtIed *pRelaIed;//关联IED
		SPtrList<stuSvgVtCbIedItem> items;
		int ied_height;//ied总高度
	};
	struct SSP_GUI_EXPORT stuSvgVtCbIedItem
	{
		stuSvgVtCbIedItem()
		{
			memset(this,0,sizeof(stuSvgVtCbIedItem));
		}
		int type;//1:smv in 2:gse in 3:smv out 4:gse out
		stuSclVtIed *pRelaIed;//关联IED
		stuSclVtIedSmvOut *pSmvOut;
		stuSclVtIedGooseOut *pGooseOut;
		stuSclVtIedSmvIn *pSmvIn;
		stuSclVtIedGooseIn *pGooseIn;
		int center_y;//输出控制块中心点Y轴坐标

	};
	stuSvgVtCbIed* SearchRelaIed(stuSclVtIed *pRelaIed);
	void SortRelaIed();

	//struct stuIed
	//间隔图中有控制关系的IED分组，一个间隔可能包含多个分组
	struct SSP_GUI_EXPORT stuBayIedGroup
	{
		SPtrList<stuSclVtIed> m_GroupIeds;
		SSvgObject::SVG_RECT m_GruopRect;//当前组的坐标范围
	};
	CSpVtSvgGenerator();
	virtual ~CSpVtSvgGenerator();

	void SetSclVt(CSclVt *p){m_pSclVt = p;};

	SString GetCBColor(stuSclVtIedSmvOut *pOut);

	SString GetCBColor(stuSclVtIedGooseOut *pOut);

	SString GetCBLineColor(stuSclVtIedSmvOut *pOut,stuSclVtIedSmvInChannel *pChn=NULL);

	SString GetCBLineColor(stuSclVtIedGooseOut *pOut,stuSclVtIedGooseInChannel *pChn=NULL);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  生成指定子网的通讯访问点可视化SVG图
	// 作    者:  邵凯田
	// 创建时间:  2016-1-11 8:59
	// 参数说明:  @sSubNetName表示子网名称，""表示第一个子网
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool GeneratorSubNetSvg(SString sSubNetName, SXmlConfig &SvgXml);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  生成指定IED的控制块发布、订阅关系图，以指定IED为中心
	// 作    者:  邵凯田
	// 创建时间:  2016-1-11 9:00
	// 参数说明:  @sIedName表示IED名称，""表示第一个IED
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool GeneratorIedCBInOut(SString sIedName,SXmlConfig &SvgXml);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  生成指定IED的出、入控制块所有虚端子连接图，以指定IED为中心
	// 作    者:  邵凯田
	// 创建时间:  2016-1-13 17:00
	// 参数说明:  @sIedName表示IED名称，""表示第一个IED
	// 返 回 值:  false/true
	//////////////////////////////////////////////////////////////////////////
	bool GeneratorIedVirtualTerminal(SString sIedName,SXmlConfig &SvgXml);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加线段
	// 作    者:  邵凯田
	// 创建时间:  2016-1-12 9:02
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void AddLine(SBaseConfig *pRootNode, int x1,int y1,int x2,int y2,bool bArrow,char* sAttrs);
	void AddBeeLine(SBaseConfig *pRootNode, int x1,int y1,int x2,int y2,bool bArrow,char* sAttrs);

	int NewHLineY(int y0)
	{
		unsigned long pos;
		int* p;
		int y=y0;
		int idx=1;
		again:
		p = m_iHLineY.FetchFirst(pos);
		while(p)
		{
			if(*p == y)
			{
//				if(idx&1==0)
					y = y0+idx*5;
// 				else
// 					y = y0-idx*10;
 				idx++;
				goto again;
			}
			p = m_iHLineY.FetchNext(pos);
		}
		m_iHLineY.append(y);
		return y;
	}

	int NewVLineX(int x0)
	{
		unsigned long pos;
		int* p;
		int x = x0;
		int idx=1;
		again:
		p = m_iVLineX.FetchFirst(pos);
		while(p)
		{
			if(*p == x)
			{
				if((idx&1)==0)
					x = x0+idx*10;
				else
					x = x0-idx*10;
				idx++;
				goto again;
			}
			p = m_iVLineX.FetchNext(pos);
		}
		m_iVLineX.append(x);
		return x;
	}

	void SortAndMoveIed(SPtrList<stuSclVtIed> &src_ied,SPtrList<stuSclVtIed> &dst_ied)
	{
		stuSclVtIed *pIedMin,*pIed2;
		unsigned long pos1;
		while(src_ied.count()>0)
		{
			pIedMin = pIed2 = src_ied.FetchFirst(pos1);
			while(pIed2)
			{
				if(strcmp(pIed2->name,pIedMin->name) < 0)
					pIedMin = pIed2;
				pIed2 = src_ied.FetchNext(pos1);
			}
			dst_ied.append(pIedMin);
			src_ied.remove(pIedMin);
		}

	}

	CSclVt *m_pSclVt;
	bool m_bShowVT;//是否显示虚端子图
	SString m_sStationName;//厂站名称	
	SPtrList<stuHistoryLine> m_HisLines;
	SValueList<int> m_iHLineY;//水平线Y坐标点
	SValueList<int> m_iVLineX;//垂直线X坐标点
	SPtrList<stuSvgVtCbIed> m_RelaIeds;
};

class SSP_GUI_EXPORT CSpVtSvgAccessPoint : public SSvgWnd
{
public:
	CSpVtSvgAccessPoint(QWidget *parent);
	virtual ~CSpVtSvgAccessPoint();

	void GotoSubNet(SString sSubNetName);
	void GotoIed(SString sIedName);


	void SetSvgGenerator(CSpVtSvgGenerator *p){m_pGenerator = p;};
	virtual void OnClickObject(SSvgObject *pSvgObj);
	//取弹出提示文本回调
	virtual SString GetSvgObjectTipText(SSvgObject *pSvgObj);

	CSpVtSvgGenerator *m_pGenerator;
	SString m_sLastShowType;
	SString m_sLastSubNetName;
	SString m_sLastIedName;
};

#endif//__SP_VIRTUAL_TERMINAL_VISUAL_H__
