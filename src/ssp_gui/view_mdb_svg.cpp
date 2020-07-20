/**
 *
 * 文 件 名 : view_mdb_svg.cpp
 * 创建日期 : 2015-8-4 13:08
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

#include "view_mdb_svg.h"
#include "sp_qt_chart.h"
#include "ssp_gui.h"
#include "view_common.h"
#include "view_query.h"

CViewMdbSvg *g_pLastClickMdbSvg=NULL;
CBaseView *g_pFloatViewWnd = NULL;
CMdbSvgWnd::CMdbSvgWnd(QWidget *parent) : SSvgWnd(parent)
{

}

CMdbSvgWnd::~CMdbSvgWnd()
{
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  对象点击响应
// 作    者:  邵凯田
// 创建时间:  2015-8-4 14:07
// 参数说明:  @pSvgObj
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMdbSvgWnd::OnClickObject(SSvgObject *pSvgObj)
{
	if(pSvgObj != NULL)
		m_pViewMdbSvg->OnClickObject(pSvgObj);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取图元的提示文本，优先通过应用层回调函数提取，取不到返回图元的title文本
// 作    者:  邵凯田
// 创建时间:  2015-12-8 14:45
// 参数说明:  @pSvgObj
// 返 回 值:  SString
//////////////////////////////////////////////////////////////////////////
SString CMdbSvgWnd::GetSvgObjectTipText(SSvgObject *pSvgObj)
{
	if(pSvgObj != NULL)
		return m_pViewMdbSvg->GetSvgObjectTipText(pSvgObj);
	return "";
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  用户自绘虚函数，自绘操作仅针对矩形图元，并在扩展属性中含userdraw=true;
// 作    者:  邵凯田
// 创建时间:  2015-8-10 14:07
// 参数说明:  @pDC为窗口DC，针对QT时通过(QPainter*)pDC->GetHandle()使用
//         :  @rect为指针的绘制区域，绘制不可超出指针的区域范围
//         :  @pObj表示绘制的对象指针，由用户层根据扩展属性确定绘制模块
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMdbSvgWnd::DrawUser(SBaseDC *pDC,SRect &rect, SSvgObject *pObj)
{
	stuSvgMapToRecord *pMap = (stuSvgMapToRecord *)pObj->GetUserPtr();
	if(pMap == NULL)
		return;
	SString sExtAttr = pObj->GetAttribute("ext-attribute");
	SString type = SString::GetAttributeValue(sExtAttr,"type");
	if(type == "chart_pie")
	{
		//饼图
		CSsp_Dataset *pDataset = pMap->pDataset;
// 		SString ds = SString::GetAttributeValue(sExtAttr,"ds");
// 		CSsp_Dataset *pDataset = GET_DW_CFG->SearchDataset(ds);
		if(pDataset != NULL)
		{
			if(pDataset->GetAutoColumnTransform() == false)
			{
				pDataset->SetAutoColumnTransform(true);
				pDataset->SetInnerRsIsDirty();
			}
			SSP_GUI->SetSessionAttributeValues(sExtAttr);
			if(pDataset->GetSql().find("{@SESSION:")>=0)
				pDataset->SetInnerRsIsDirty();
			SRecordset *pRs = pDataset->GetInnerRecordset(0,SString::GetAttributeValue(sExtAttr,"cond"));
			SString tcor = SString::GetAttributeValue(sExtAttr,"tcor");
			SString lcor = SString::GetAttributeValue(sExtAttr,"lcor");
			SString sTitle = SString::GetAttributeValue(sExtAttr,"t");
			if(sTitle.length() == 0 && pDataset->GetColumn(1) != NULL)
				sTitle = pDataset->GetColumn(1)->m_sName;
			pDataset->LockInnerRs();
			CSpQtChartPainter::DrawChartPie((QPainter*)pDC->GetHandle(),rect,pRs,sTitle,tcor==""?QColor(0,0,0):QColor(SString::GetIdAttributeI(1,tcor,","),SString::GetIdAttributeI(2,tcor,","),SString::GetIdAttributeI(3,tcor,",")),
				lcor==""?QColor(255,255,255):QColor(SString::GetIdAttributeI(1,lcor,","),SString::GetIdAttributeI(2,lcor,","),SString::GetIdAttributeI(3,lcor,",")),sExtAttr);
			pDataset->UnlockInnerRs();
		}
		else
		{
			LOGWARN("dataset未定义!无法完成自绘!");
		}
	}
	else if(type == "chart_col")
	{
		//柱图
		CSsp_Dataset *pDataset = pMap->pDataset;
		// 		SString ds = SString::GetAttributeValue(sExtAttr,"ds");
		// 		CSsp_Dataset *pDataset = GET_DW_CFG->SearchDataset(ds);
		if(pDataset != NULL)
		{
			if(pDataset->GetAutoColumnTransform() == false)
			{
				pDataset->SetAutoColumnTransform(true);
				pDataset->SetInnerRsIsDirty();
				pDataset->GetInnerRecordset(0,SString::GetAttributeValue(sExtAttr,"cond"));
			}
			
			SSP_GUI->SetSessionAttributeValues(sExtAttr);
			if(pDataset->GetSql().find("{@SESSION:")>=0)
				pDataset->SetInnerRsIsDirty();
			SRecordset *pRs = pDataset->GetInnerRecordset(0,SString::GetAttributeValue(sExtAttr,"cond"));
			SString tcor = SString::GetAttributeValue(sExtAttr,"tcor");
			SString lcor = SString::GetAttributeValue(sExtAttr,"lcor");
			SString sTitle = SString::GetAttributeValue(sExtAttr,"t");
			if(sTitle.length() == 0 && pDataset->GetColumn(1) != NULL)
				sTitle = pDataset->GetColumn(1)->m_sName;
			pDataset->LockInnerRs();
			CSpQtChartPainter::DrawChartColumn((QPainter*)pDC->GetHandle(),rect,pRs,sTitle,tcor==""?QColor(0,0,0):QColor(SString::GetIdAttributeI(1,tcor,","),SString::GetIdAttributeI(2,tcor,","),SString::GetIdAttributeI(3,tcor,",")),
				lcor==""?QColor(255,255,255):QColor(SString::GetIdAttributeI(1,lcor,","),SString::GetIdAttributeI(2,lcor,","),SString::GetIdAttributeI(3,lcor,",")),sExtAttr);
			pDataset->UnlockInnerRs();
		}
		else
		{
			LOGWARN("dataset未定义!无法完成自绘!");
		}
	}
	else if(type == "chart_line")
	{
		//线图
		CSsp_Dataset *pDataset = pMap->pDataset;
		// 		SString ds = SString::GetAttributeValue(sExtAttr,"ds");
		// 		CSsp_Dataset *pDataset = GET_DW_CFG->SearchDataset(ds);
		if(pDataset != NULL)
		{
			if(pDataset->GetAutoColumnTransform() == false)
			{
				pDataset->SetAutoColumnTransform(true);
				pDataset->SetInnerRsIsDirty();
			}
			SSP_GUI->SetSessionAttributeValues(sExtAttr);
			if(pDataset->GetSql().find("{@SESSION:")>=0)
				pDataset->SetInnerRsIsDirty();
			SRecordset *pRs = pDataset->GetInnerRecordset(0,SString::GetAttributeValue(sExtAttr,"cond"));
			SString tcor = SString::GetAttributeValue(sExtAttr,"tcor");
			SString lcor = SString::GetAttributeValue(sExtAttr,"lcor");
			SString sTitle = SString::GetAttributeValue(sExtAttr,"t");
			for(int i=1;i<pDataset->GetColumnCount();i++)
				sTitle += ","+pDataset->GetColumn(i)->m_sName;
			pDataset->LockInnerRs();
			CSpQtChartPainter::DrawChartLine((QPainter*)pDC->GetHandle(),rect,pRs,sTitle,tcor==""?QColor(0,0,0):QColor(SString::GetIdAttributeI(1,tcor,","),SString::GetIdAttributeI(2,tcor,","),SString::GetIdAttributeI(3,tcor,",")),
				lcor==""?QColor(255,255,255):QColor(SString::GetIdAttributeI(1,lcor,","),SString::GetIdAttributeI(2,lcor,","),SString::GetIdAttributeI(3,lcor,",")),sExtAttr);
			pDataset->UnlockInnerRs();
		}
		else
		{
			LOGWARN("dataset未定义!无法完成自绘!");
		}
	}
	else if(type == "chart_panel")//type=chart_panel;ds=mds_port_realflow_mms;row=1;col=2;p_cor=rgb(0,0,0);p_cor_0=rgb(255,0,0);p_cor_1=rgb(255,255,127);p_cor_2=rgb(85,255,0);prot_type=4;flash=0;
	{
		//线图
		CSsp_Dataset *pDataset = pMap->pDataset;
		// 		SString ds = SString::GetAttributeValue(sExtAttr,"ds");
		// 		CSsp_Dataset *pDataset = GET_DW_CFG->SearchDataset(ds);
		if(pDataset != NULL)
		{
			if(pDataset->GetAutoColumnTransform() == false)
			{
				pDataset->SetAutoColumnTransform(true);
				pDataset->SetInnerRsIsDirty();
			}
			SSP_GUI->SetSessionAttributeValues(sExtAttr);
			if(pDataset->GetSql().find("{@SESSION:")>=0)
				pDataset->SetInnerRsIsDirty();
			SRecordset *pRs = pDataset->GetInnerRecordset(0,SString::GetAttributeValue(sExtAttr,"cond"));
			SString tcor = SString::GetAttributeValue(sExtAttr,"tcor");
			SString lcor = SString::GetAttributeValue(sExtAttr,"lcor");
			SString sTitle = SString::GetAttributeValue(sExtAttr,"t");
			for(int i=1;i<pDataset->GetColumnCount();i++)
				sTitle += ","+pDataset->GetColumn(i)->m_sName;
			pDataset->LockInnerRs();
			float fv = pRs->GetValue(pMap->iRowIndex,pMap->iColIndex).toFloat();
			pDataset->UnlockInnerRs();
			CSpQtChartPainter::DrawPanel((QPainter*)pDC->GetHandle(),rect,fv,sExtAttr);
			
		}
		else
		{
			LOGWARN("dataset未定义!无法完成自绘!");
		}
	}
}

SPtrList<CViewMdbSvg> g_mdb_svg_wnds;
//////////////////////////////////////////////////////////////////////////
// 名    称:  CViewMdbSvg
// 作    者:  邵凯田
// 创建时间:  2015-8-10 14:08
// 描    述:  映射MDB库的SVG视图
//////////////////////////////////////////////////////////////////////////
CViewMdbSvg::CViewMdbSvg(int wnd_sn,QWidget *parent,bool bFloat,SString *pExtAttr) : CBaseView(parent)
{
	S_UNUSED(pExtAttr);
	g_mdb_svg_wnds.append(this);
// 	if(bFloat)
// 	{
// 		int w = 300,h = 200;
// 		if(pExtAttr != NULL)
// 		{
// 			w = SString::GetAttributeValueI(*pExtAttr,"w");
// 			h = SString::GetAttributeValueI(*pExtAttr,"h");
// 			if(w <= 0)
// 				w = 300;
// 			if(h <= 0)
// 				h = 200;
// 		}
// 		resize(w,h);	
// #ifdef WIN32
// 		setWindowFlags( Qt::Dialog | Qt::Tool | Qt::WindowStaysOnTopHint);
// #else
// 		setWindowFlags( Qt::Dialog );
// #endif
// 	}
	m_bLoadingSvg = false;
	m_pSvgText = NULL;
	m_iSvgTextLen = 0;
	m_iDynRsRectCnt = 0;
	m_verticalLayout_2 = new QVBoxLayout(this);
	m_verticalLayout_2->setSpacing(0);
	m_verticalLayout_2->setContentsMargins(0, 0, 0, 0);
	m_verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
	m_verticalLayout_2->setContentsMargins(0, 0, 0, 0);
	m_SvgItemMaps.setAutoDelete(true);
	m_SvgItemMaps.setShared(true);
	m_SvgItemAutoFlashMaps.setAutoDelete(false);
	m_SvgMapDataset.setAutoDelete(false);
	m_DynSvgMapDataset.setAutoDelete(true);
	m_iSvgUpdateSn = m_iSvgShowSn = 0;

	m_pTimer = new QTimer(this);
	connect(m_pTimer,SIGNAL(timeout()),this,SLOT(OnTimerReloadSvg())/*,Qt::DirectConnection*/);
	m_pTimer2 = new QTimer(this);
	connect(m_pTimer2,SIGNAL(timeout()),this,SLOT(OnTimerRepaint()));
	m_pTimer2->start(200);
	connect(this,SIGNAL(DoRefSvg()),this,SLOT(OnRefreshSvg()));

	m_pSvg = new CMdbSvgWnd(this);
	m_pTempSvg = new SSvgWnd();
	m_pTempSvg->hide();
	m_pSvg->m_pViewMdbSvg = this;
	m_verticalLayout_2->addWidget(m_pSvg);
	m_pSvg->SetMovable(true);
	m_pSvg->SetDataModel(this);
	m_iWndSn = wnd_sn;
	m_iLastWndSn = 0;
	m_bFloatWnd = bFloat;
	//m_pParentView = NULL;
	if(bFloat)
	{
		OnAftShow();
	}
}

CViewMdbSvg::~CViewMdbSvg()
{
	if(m_pSvgText != NULL)
	{
		DB->FreeLobMem((unsigned char*)m_pSvgText);
		m_pSvgText = NULL;
	}
	if(g_pFloatViewWnd != NULL && g_pFloatViewWnd->parentWidget() == this)
		g_pFloatViewWnd = NULL;
	m_pTimer->stop();
	m_pTimer2->stop();
	disconnect(m_pTimer,SIGNAL(timeout()),this,SLOT(OnTimerReloadSvg()));
	disconnect(m_pTimer2,SIGNAL(timeout()),this,SLOT(OnTimerRepaint()));
	disconnect(this,SIGNAL(DoRefSvg()),this,SLOT(OnRefreshSvg()));

	if(g_pLastClickMdbSvg == this)
		g_pLastClickMdbSvg = NULL;
	delete m_pTempSvg;

	//释放回调
	RealeaseAllTrgCallback();

	//删除映射数据
	m_SvgItemMaps.clear();
	m_SvgItemAutoFlashMaps.clear();
	m_SvgMapDataset.clear();
	m_DynSvgMapDataset.clear();
	g_mdb_svg_wnds.remove(this);
}

//#define SVG_MDB_USE_THREAD
void CViewMdbSvg::OnTimerReloadSvg()
{
	if(!isVisible())
		return;
//	SSP_GUI->ShowWaiting();
//	SQt::ProcessUiEvents();
	if(!g_mdb_svg_wnds.exist(this))
		return;
#ifdef SVG_MDB_USE_THREAD
	while(m_bLoadingSvg)
	{
		SQt::ProcessUiEvents();
		if(!g_mdb_svg_wnds.exist(this))
			return;
		SApi::UsSleep(10000);
	}

	//释放回调
	RealeaseAllTrgCallback();
	m_pTimer->stop();
	SString sql;
	sql.sprintf("select wnd_name,dataset,ds_cond,refresh_sec from t_ssp_uicfg_wnd where wnd_sn=%d and wnd_type=1",m_iWndSn);
	SRecordset rs;
	int ret = DB->Retrieve(sql,rs);
	if(ret <= 0)
	{
		SSP_GUI->HideWaiting();
		return;
	}
	SString wnd_name = rs.GetValue(0,0);
	SString dataset  = rs.GetValue(0,1);
	SString ds_cond  = rs.GetValue(0,2);
	m_iRefreshSec = rs.GetValue(0,3).toInt();

	//先记录各待删除数据的记录数，映射完成后删除
	int list_rows[4];
	list_rows[0] = m_SvgItemMaps.count();
	list_rows[1] = m_SvgItemAutoFlashMaps.count();
	list_rows[2] = m_SvgMapDataset.count();
	list_rows[3] = m_DynSvgMapDataset.count();

	
	m_bLoadingSvg = true;
	//m_sTempTip = "加载组态界面...";
	//SSP_GUI->SetStatusTip("加载组态界面...");
	SKT_CREATE_THREAD(ThreadReloadSvg,this);
	//等待线程退出
	while(m_bLoadingSvg)
	{
		SQt::ProcessUiEvents();
		if(!g_mdb_svg_wnds.exist(this))
			return;
		SApi::UsSleep(10000);
	}
	//SSP_GUI->SetStatusTip("组态界面完毕!");
// 	int soc,usec;
// 	SDateTime::getSystemTime(soc,usec);

	//交换临时对象与当前SVG对象的文档资源
	SSvgObject *pRootObj = m_pTempSvg->m_Document.GetRootObject();
	m_pTempSvg->m_Document.SetRootObject(m_pSvg->m_Document.GetRootObject());
	m_pSvg->m_Document.SetRootObject(pRootObj);
	//交换图元列表
	SSvgDocument::SwapObjectList(&m_pTempSvg->m_Document,&m_pSvg->m_Document);

	//交换图片资源
	SPtrList<SSvgDocument::stuImageDefine> *pImg1 = m_pTempSvg->m_Document.GetImageDefs();
	SPtrList<SSvgDocument::stuImageDefine> *pImg2 = m_pSvg->m_Document.GetImageDefs();
	SPtrList<SSvgDocument::stuImageDefine> swap;
	pImg1->copyto(swap);
	pImg1->setAutoDelete(false);
	pImg2->setAutoDelete(false);
	pImg1->clear();
	pImg2->copyto(*pImg1);
	pImg2->clear();
	swap.copyto(*pImg2);
	swap.clear();
	pImg1->setAutoDelete(true);
	pImg2->setAutoDelete(true);

	//删除原有的映射信息
	//pThis->m_SvgItemMaps.lock();
	int i=0;
	stuSvgMapToRecord *pMap;
	for(i=0;i<list_rows[0];i++)
	{
		pMap = m_SvgItemMaps[0];
		if(pMap != NULL && pMap->pSvgObj != NULL)
			pMap->pSvgObj->SetUserPtr(NULL);
		m_SvgItemMaps.remove(pMap);
	}
	for(i=0;i<list_rows[1];i++)
	{
		m_SvgItemAutoFlashMaps.remove(0);
	}
	for(i=0;i<list_rows[2];i++)
	{
		m_SvgMapDataset.remove(0);
	}
	for(i=0;i<list_rows[3];i++)
	{
		m_DynSvgMapDataset.remove(0);
	}
	//pThis->m_SvgItemMaps.unlock();
// 	int soc2,usec2;
// 	SDateTime::getSystemTime(soc2,usec2);
// 	m_sTempTip += (SString::toFormat("界面线程:%fS",(float)(soc2-soc)+(usec2-usec)/1000000.0f));
// 	SSP_GUI->SetStatusTip(m_sTempTip);
#else
// 
// 	m_sTempTip = "加载组态界面...";
// 	SSP_GUI->SetStatusTip(m_sTempTip);
//SQt::ProcessUiEvents();
if(!g_mdb_svg_wnds.exist(this))
	return;

	//释放回调
	RealeaseAllTrgCallback();

	//删除映射数据
	unsigned long pos;
	m_SvgItemMaps.lock();
	stuSvgMapToRecord *p = m_SvgItemMaps.FetchFirst(pos);
	while(p)
	{
		if(p->pSvgObj != NULL)
			p->pSvgObj->SetUserPtr(NULL);
		p = m_SvgItemMaps.FetchNext(pos);
	}
	m_SvgItemMaps.unlock();
	m_SvgItemMaps.clear();
	m_SvgItemAutoFlashMaps.clear();
	m_SvgMapDataset.clear();
	m_DynSvgMapDataset.clear();

	m_pTimer->stop();
	SString sql;
	sql.sprintf("select wnd_name,dataset,ds_cond,refresh_sec from t_ssp_uicfg_wnd where wnd_sn=%d and wnd_type=1",m_iWndSn);
	SRecordset rs;
	int ret = DB->Retrieve(sql,rs);
	if(ret <= 0)
	{
		SSP_GUI->HideWaiting();
		return;
	}
	SString wnd_name = rs.GetValue(0,0);
	SString dataset  = rs.GetValue(0,1);
	SString ds_cond  = rs.GetValue(0,2);
	m_iRefreshSec = rs.GetValue(0,3).toInt();
	
	
	CViewMdbSvg *pThis = this;

	unsigned char *pSvgText=NULL;
	int iSvgLen=0;
// 	int soc,usec,soc2,usec2;
// 	SDateTime::getSystemTime(soc,usec);
	if(!DB->ReadLobToMem("t_ssp_uicfg_wnd","svg_file",SString::toFormat("wnd_sn=%d",pThis->m_iWndSn),pSvgText,iSvgLen) || pSvgText == NULL)
	{
		if(pSvgText != NULL)
			DB->FreeLobMem(pSvgText);
		//SQt::ShowErrorBox(tr("提示"),tr("读取组态窗口失败!"));
		pThis->m_bLoadingSvg = false;
		SSP_GUI->HideWaiting();
		return ;
	}

// 	SDateTime::getSystemTime(soc2,usec2);
// 	pThis->m_sTempTip += SString::toFormat("ReadLob:%fS,",(float)(soc2-soc)+(usec2-usec)/1000000.0f);
// 	SSP_GUI->SetStatusTip(pThis->m_sTempTip);

	SSvgWnd *pTmpSvgWnd = pThis->m_pSvg;// m_pTempSvg;
	//pTmpSvgWnd->hide();
	//SDateTime::getSystemTime(soc,usec);
	if(m_pSvgText != NULL && m_iSvgTextLen == iSvgLen && m_iDynRsRectCnt == 0 && strstr((char*)pSvgText,"type=dyn_rs_rect;") == NULL && strcmp(m_pSvgText,(char*)pSvgText) == 0)
	{
		//SVG文本完全相同，而且不含动态区域，没有必要重新加载SVG对象
	}
	else
	{
		//如果SVG中存在会话变量，则先进行替换
		if(strstr((char*)pSvgText,"{@SESSION:") == NULL)
			ret = pTmpSvgWnd->LoadText((char*)pSvgText,false);
		else
		{
			SString sFldKey,sSvg = (char*)pSvgText;
			int p1,p2;
			p1 = sSvg.find("{@SESSION:");
			while(p1 >= 0)
			{
				p2 = sSvg.find("@}",p1);
				if(p2 < 0)
					break;
				sFldKey = sSvg.mid(p1+10,p2-p1-10);
				sSvg = sSvg.left(p1)+SSP_BASE->GetSessionAttributeValue(sFldKey.data())+sSvg.mid(p2+2);
				p1 = sSvg.find("{@SESSION:");
			}
			ret = pTmpSvgWnd->LoadText(sSvg.data(),false);
		}
	}
	if(m_pSvgText != NULL)
		delete[] m_pSvgText;
	m_pSvgText = (char*)pSvgText;
	m_iSvgTextLen = iSvgLen;
// 	SDateTime::getSystemTime(soc2,usec2);
// 	pThis->m_sTempTip += SString::toFormat("LoadSvg:%fS,",(float)(soc2-soc)+(usec2-usec)/1000000.0f);
// 	SSP_GUI->SetStatusTip(pThis->m_sTempTip);

	//delete[] pSvgText;

	//SDateTime::getSystemTime(soc,usec);	
	//遍历所有一级图元,处理所有的数据集动态图元，按记录创建图元
	SSvgObject *pRootObj = pTmpSvgWnd->m_Document.GetRootObject();
	if(pRootObj == NULL)
	{
		pThis->m_bLoadingSvg = false;
		SSP_GUI->HideWaiting();
		return ;
	}
	m_iDynRsRectCnt = 0;
	SSvgObject /**pObj0,*/*pObj = pRootObj->GetChild();
	SString sExtAttr;
	while(pObj)
	{		
		sExtAttr = pObj->GetAttribute("ext-attribute");
		if(sExtAttr.length() > 0 && SString::GetAttributeValue(sExtAttr,"type") == "dyn_rs_rect")
		{
			if(!pThis->ProcessDynRsRect(pRootObj,pObj))
			{
				m_iDynRsRectCnt++;
				pObj = pRootObj->GetChild();
				continue;
			}
			m_iDynRsRectCnt++;
		}
		pObj = pObj->GetNext();
	}
// 	SDateTime::getSystemTime(soc2,usec2);
// 	pThis->m_sTempTip += SString::toFormat("DynProcess:%fS,",(float)(soc2-soc)+(usec2-usec)/1000000.0f);
// 	SSP_GUI->SetStatusTip(pThis->m_sTempTip);

	//遍历所有一级图元,重新加载数据映射关系
//	SDateTime::getSystemTime(soc,usec);
	pObj = pRootObj->GetChild();
	while(pObj)
	{
		sExtAttr = pObj->GetAttribute("ext-attribute");
		if(sExtAttr.length() > 0)
		{
			pThis->AddObjectMap(pObj,sExtAttr);
		}
		pObj = pObj->GetNext();
	}
// 	SDateTime::getSystemTime(soc2,usec2);
// 	pThis->m_sTempTip += SString::toFormat("AddMap:%fS,",(float)(soc2-soc)+(usec2-usec)/1000000.0f);
// 	SSP_GUI->SetStatusTip(pThis->m_sTempTip);
	pThis->m_bLoadingSvg = false;

#endif




#if 0
	char *pSvgText=NULL;
	int iSvgLen=0;
	if(!DB->ReadLobToMem("t_ssp_uicfg_wnd","svg_file",SString::toFormat("wnd_sn=%d",m_iWndSn),(BYTE* &)pSvgText,iSvgLen) || pSvgText == NULL)
	{
		if(pSvgText != NULL)
			delete[] pSvgText;
		SSP_GUI->HideWaiting();
		SQt::ShowErrorBox(tr("提示"),tr("读取组态窗口失败!"));
		return;
	}
	ret = m_pSvg->LoadText(pSvgText);
	delete[] pSvgText;

	//遍历所有一级图元,处理所有的数据集动态图元，按记录创建图元
	SSvgObject *pRootObj = m_pSvg->m_Document.GetRootObject();
	if(pRootObj == NULL)
	{
		//启动自动刷新
		if(m_iRefreshSec > 0)
			m_pTimer->start(m_iRefreshSec*1000);
		SSP_GUI->HideWaiting();
		return;
	}
	SSvgObject *pObj0,*pObj = pRootObj->GetChild();
	SString sExtAttr;
	while(pObj)
	{		
		sExtAttr = pObj->GetAttribute("ext-attribute");
		if(sExtAttr.length() > 0 && SString::GetAttributeValue(sExtAttr,"type") == "dyn_rs_rect")
		{
			if(!ProcessDynRsRect(pRootObj,pObj))
			{
				pObj = pRootObj->GetChild();
				continue;
			}
		}
		pObj = pObj->GetNext();
	}

	//遍历所有一级图元,重新加载数据映射关系
	pRootObj = m_pSvg->m_Document.GetRootObject();
	if(pRootObj == NULL)
	{
		SSP_GUI->HideWaiting();
		return;
	}
	pObj = pRootObj->GetChild();
	while(pObj)
	{
		sExtAttr = pObj->GetAttribute("ext-attribute");
		if(sExtAttr.length() > 0)
		{
			AddObjectMap(pObj,sExtAttr);
		}
		pObj = pObj->GetNext();
	}

#endif
	bool bRedraw=false;
	//repaint();
	//按窗口宽度自适应
	if(m_iLastWndSn != m_iWndSn)
	{
		m_iLastWndSn = m_iWndSn;
		int keep_scr = SSP_BASE->GetSessionAttributeValue((char*)"keep_scr").toInt();
		if(keep_scr == 1)
			SSP_BASE->SetSessionAttributeValue("keep_scr","");
		if(keep_scr == 0)
		{
			SSvgObject *pFirst = m_pSvg->m_Document.GetFirstObject();
			if(pFirst != NULL && pFirst->GetType() == SSvgObject::TYPE_RECT)
			{
#ifndef WIN32
//				SQt::ProcessUiEvents();
				if(!g_mdb_svg_wnds.exist(this))
					return;
#endif
				//float w = pFirst->GetRect()->width();
				//float wnd_w = (float)rect().width();
				//if(w > wnd_w)
					m_pSvg->ToWidthFull();
				//else
				//	m_pSvg->ToRaw();
				m_pSvg->RedrawWindow();
//				SQt::ProcessUiEvents();
				if(!g_mdb_svg_wnds.exist(this))
					return;
				bRedraw = true;
			}
		}
	}

	CSsp_Dataset *pDs = m_SvgMapDataset.FetchFirst(pos);
	while(pDs)
	{
		pDs->RegisterTriggerCallback(OnMdbDsTrgCallback,this);
		pDs = m_SvgMapDataset.FetchNext(pos);
	}
	pDs = m_DynSvgMapDataset.FetchFirst(pos);
	while(pDs)
	{
		pDs->RegisterTriggerCallback(OnMdbDsTrgCallback,this);
		pDs = m_DynSvgMapDataset.FetchNext(pos);
	}
	if(!bRedraw)
	{
 		m_pSvg->Render();
 		m_pSvg->RedrawWindow();
	}
	//启动自动刷新
	if(m_iRefreshSec > 0)
		m_pTimer->start(m_iRefreshSec*1000);
//	SSP_GUI->HideWaiting();
	//SSP_GUI->SetStatusTip("");
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  加载SVG线程，用于后台加载，不影响GUI主线程
// 作    者:  邵凯田
// 创建时间:  2015-12-8 17:43
// 参数说明:  @lp：this
// 返 回 值:  NULL
//////////////////////////////////////////////////////////////////////////
void* CViewMdbSvg::ThreadReloadSvg(void *lp)
{
	CViewMdbSvg *pThis = (CViewMdbSvg*)lp;

	unsigned char *pSvgText=NULL;
	int iSvgLen=0;
	int soc,usec,soc2,usec2;
	SDateTime::getSystemTime(soc,usec);
	if(!DB->ReadLobToMem("t_ssp_uicfg_wnd","svg_file",SString::toFormat("wnd_sn=%d",pThis->m_iWndSn),pSvgText,iSvgLen) || pSvgText == NULL)
	{
		if(pSvgText != NULL)
			delete[] pSvgText;
		//SQt::ShowErrorBox(tr("提示"),tr("读取组态窗口失败!"));
		pThis->m_bLoadingSvg = false;
		return NULL;
	}
	SDateTime::getSystemTime(soc2,usec2);
	pThis->m_sTempTip += SString::toFormat("ReadLob:%fS,",(float)(soc2-soc)+(usec2-usec)/1000000.0f);
	SSP_GUI->SetStatusTip(pThis->m_sTempTip);

	SSvgWnd *pTmpSvgWnd = pThis->m_pSvg;// m_pTempSvg;
	pTmpSvgWnd->hide();
	SDateTime::getSystemTime(soc,usec);	
	/*int ret = */pTmpSvgWnd->LoadText((char*)pSvgText);
	SDateTime::getSystemTime(soc2,usec2);
	pThis->m_sTempTip += SString::toFormat("LoadSvg:%fS,",(float)(soc2-soc)+(usec2-usec)/1000000.0f);
	SSP_GUI->SetStatusTip(pThis->m_sTempTip);
	DB->FreeLobMem(pSvgText);

	SDateTime::getSystemTime(soc,usec);	
	//遍历所有一级图元,处理所有的数据集动态图元，按记录创建图元
	SSvgObject *pRootObj = pTmpSvgWnd->m_Document.GetRootObject();
	if(pRootObj == NULL)
	{
		pThis->m_bLoadingSvg = false;
		return NULL;
	}
	SSvgObject /**pObj0,*/*pObj = pRootObj->GetChild();
	SString sExtAttr;
	while(pObj)
	{		
		sExtAttr = pObj->GetAttribute("ext-attribute");
		if(sExtAttr.length() > 0 && SString::GetAttributeValue(sExtAttr,"type") == "dyn_rs_rect")
		{
			if(!pThis->ProcessDynRsRect(pRootObj,pObj))
			{
				pObj = pRootObj->GetChild();
				continue;
			}
		}
		pObj = pObj->GetNext();
	}
	SDateTime::getSystemTime(soc2,usec2);
	pThis->m_sTempTip += SString::toFormat("DynProcess:%fS,",(float)(soc2-soc)+(usec2-usec)/1000000.0f);
	SSP_GUI->SetStatusTip(pThis->m_sTempTip);
#if 0
	unsigned long pos;
	pThis->m_SvgItemMaps.lock();
	stuSvgMapToRecord *p = pThis->m_SvgItemMaps.FetchFirst(pos);
	while(p)
	{
		if(p->pSvgObj != NULL)
			p->pSvgObj->SetUserPtr(NULL);
		p = pThis->m_SvgItemMaps.FetchNext(pos);
	}
	pThis->m_SvgItemMaps.unlock();
	pThis->m_SvgItemMaps.clear();
	pThis->m_SvgItemAutoFlashMaps.clear();
	pThis->m_SvgMapDataset.clear();
	pThis->m_DynSvgMapDataset.clear();
#endif
	//遍历所有一级图元,重新加载数据映射关系
	SDateTime::getSystemTime(soc,usec);
	pObj = pRootObj->GetChild();
	while(pObj)
	{
		sExtAttr = pObj->GetAttribute("ext-attribute");
		if(sExtAttr.length() > 0)
		{
			pThis->AddObjectMap(pObj,sExtAttr);
		}
		pObj = pObj->GetNext();
	}
	SDateTime::getSystemTime(soc2,usec2);
	pThis->m_sTempTip += SString::toFormat("AddMap:%fS,",(float)(soc2-soc)+(usec2-usec)/1000000.0f);
	SSP_GUI->SetStatusTip(pThis->m_sTempTip);
	pThis->m_bLoadingSvg = false;
	

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  处理动态数据集区域，按数据集内容生成各行的元素并自动映射图元
// 作    者:  邵凯田
// 创建时间:  2015-11-16 8:36
// 参数说明:  @pRootObj为根节点
//         :  @pRectObj为区域对象
// 返 回 值:  true表示有数据处理，false表示数据为空（部分节点被删除）需要重新开始遍历节点
//////////////////////////////////////////////////////////////////////////
bool CViewMdbSvg::ProcessDynRsRect(SSvgObject *pRootObj,SSvgObject *pRectObj)
{
	SSvgObject *pNewObj,*pObj = pRootObj->GetChild();
	if(pObj != NULL)
		pObj = pObj->GetNext();
	SSvgObject *pFirstElem = pObj;
	if(pFirstElem == NULL)
	{
		LOGWARN("缺少有效图元元素!");
		return true;
	}
	SString sExtAttr = pRectObj->GetAttribute("ext-attribute");
	int show_cols = SString::GetAttributeValueI(sExtAttr,"cols");//共显示的栏数
	if(show_cols <= 0)
		show_cols = 1;
	SString ds_name = SString::GetAttributeValue(sExtAttr,"ds");
	CSsp_Dataset *pDs = GET_DW_CFG->SearchDataset(ds_name);
	if(pDs == NULL)
	{
		LOGWARN("处理动态数据集区域中无效的数据集名称:%s",ds_name.data());
		return true;
	}
	pDs->SetInnerRsIsDirty();
	//保存会话参数
	SSP_GUI->SetSessionAttributeValues(sExtAttr);
	SRecordset *pRs = pDs->GetInnerRecordset();
	int i,rows/*,j,cols*/;
	rows = pRs->GetRows();
	//cols = pRs->GetColumns();
	int show_col = 0;//当前显示的栏号
	int sep = 10;
	SSvgObject::SVG_RECT *pObjRect,*pAllRect = pRectObj->GetRect();
	int w = abs((int)(pAllRect->right-pAllRect->left));
	int h = abs((int)(pAllRect->bottom-pAllRect->top));
	SString sObjExtAttr;
	float x=0,y=0;
	SPtrList<SSvgObject> objs;
	objs.setAutoDelete(false);
	pObj = pFirstElem;
	while(pObj)
	{
		pObjRect = pObj->GetRect();
		if(pObjRect->left >= pAllRect->left-sep &&
			pObjRect->top >= pAllRect->top-sep &&
			pObjRect->right <= pAllRect->right+sep &&
			pObjRect->bottom <= pAllRect->bottom+sep)
		{
			objs.append(pObj);
		}
		pObj = pObj->GetNext();
	}
	if(rows == 0)
	{
		//无记录，清除所有框内元素
		if(!objs.exist(pRectObj))
			objs.append(pRectObj);
		unsigned long pos=0;
		pObj = objs.FetchFirst(pos);
		while(pObj != NULL)
		{
			pRootObj->DeleteChild(pObj,true);
			pObj = objs.FetchNext(pos);
		}
		return false ;
	}

	unsigned long pos;
	int p1,p2,rcol;
	SString sFldKey;
	for(i=0;i<rows;i++)
	{
		pObj = objs.FetchFirst(pos);
		while(pObj)
		{
			//在区域范围内，生成动态元素，第一行不生成，直接使用模板元素
			if(i==0)
				pNewObj = pObj;
			else
			{
				pNewObj = pObj->Clone(pObj->GetParent());
				if(pObj == pRectObj)
				{
					sObjExtAttr = pNewObj->GetAttribute("ext-attribute");
					SString::SetAttributeValue(sObjExtAttr,"type","");
					pNewObj->SetAttributeValue("ext-attribute",sObjExtAttr);
				}
				pNewObj->Offset(x,y);

				sObjExtAttr = pNewObj->GetAttribute("ext-attribute");
				SString::SetAttributeValue(sObjExtAttr,"ds",ds_name);
				SString::SetAttributeValueI(sObjExtAttr,"row",i+1);

				//处理扩展属性中的动态数据集映射项
				p1 = sObjExtAttr.find("{@COL:");
				while(p1 >= 0)
				{
					p2 = sObjExtAttr.find("@}",p1);
					if(p2 < 0)
						break;
					sFldKey = sObjExtAttr.mid(p1+6,p2-p1-6);
					rcol = sFldKey.toInt()-1;
					if(rcol < 0)
						rcol = 0;
					sObjExtAttr = sObjExtAttr.left(p1)+pRs->GetValue(i,rcol)+sObjExtAttr.mid(p2+2);
					p1 = sObjExtAttr.find("{@COL:");
				}
				pNewObj->SetAttributeValue("ext-attribute",sObjExtAttr);
			}
			pObj = objs.FetchNext(pos);
		}

		//修改坐标
		show_col++;
		if(show_col >= show_cols)
		{
			show_col = 0;
			x = 0;
			y += h;
		}
		else
		{
			x += w;
		}
	}

	//处理第0个行
	for(i=0;i<1;i++)
	{
		pObj = objs.FetchFirst(pos);
		while(pObj)
		{
			//在区域范围内，生成动态元素，第一行不生成，直接使用模板元素
			{
				pNewObj = pObj;

				sObjExtAttr = pNewObj->GetAttribute("ext-attribute");
				SString::SetAttributeValue(sObjExtAttr,"ds",ds_name);
				SString::SetAttributeValueI(sObjExtAttr,"row",i+1);

				//处理扩展属性中的动态数据集映射项
				p1 = sObjExtAttr.find("{@COL:");
				while(p1 >= 0)
				{
					p2 = sObjExtAttr.find("@}",p1);
					if(p2 < 0)
						break;
					sFldKey = sObjExtAttr.mid(p1+6,p2-p1-6);
					rcol = sFldKey.toInt()-1;
					if(rcol < 0)
						rcol = 0;
					sObjExtAttr = sObjExtAttr.left(p1)+pRs->GetValue(i,rcol)+sObjExtAttr.mid(p2+2);
					p1 = sObjExtAttr.find("{@COL:");
				}
				pNewObj->SetAttributeValue("ext-attribute",sObjExtAttr);
			}
			pObj = objs.FetchNext(pos);
		}
	}
	if(show_cols > rows)
		show_cols = rows;
	x = pAllRect->left/*+20*/+w*show_cols;
	y += pAllRect->top+20+h;
	pObj = pRootObj;
	if(y > pObj->GetRect()->bottom)
	{
		pObj->GetRect()->bottom = y;
		pObj = pObj->GetChild();
		pObj->SetAttributeValue("height",SString::toString(y));
		pObj->CalcObjectRect();
	}
	pObj = pRootObj;
	if(x > pObj->GetRect()->right)
	{
		pObj->GetRect()->right = x;
		pObj = pObj->GetChild();
		pObj->SetAttributeValue("width",SString::toString(x));
		pObj->CalcObjectRect();
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  添加图元与数据的映射关系
// 作    者:  邵凯田
// 创建时间:  2015-8-6 15:56
// 参数说明:  @pSvgObj为图元指针
//         :  @sExtAttr为其扩展属性
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CViewMdbSvg::AddObjectMap(SSvgObject *pSvgObj,SString &sExtAttr)
{
	SString type = SString::GetAttributeValue(sExtAttr,"type");
	int flash = SString::GetAttributeValueI(sExtAttr,"flash");
	if(type.length() == 0 && flash == 0)
		return ;
	eSvgItemMapType eType;
	if(type == "text")
		eType = SVGMAP_TEXT;
	else if(type == "state")
		eType = SVGMAP_STATE;
	else if(type == "chart_pie")
		eType = SVGMAP_CHART_PIE;
	else if(type == "chart_col")
		eType = SVGMAP_CHART_COL;
	else if(type == "chart_line")
		eType = SVGMAP_CHART_LINE;
	else if(type == "chart_panel")
		eType = SVGMAP_CHART_PANEL;
	else if(flash == 1)
		eType = SVGMAP_AUTO_FLASH;
// 	else if(type == "dyn_rs_rect")
//		return;//eType = SVGMAP_DYN_RS_RECT;
	else
		return;
	if(eType != SVGMAP_AUTO_FLASH)
		pSvgObj->SetFlash(true);

	stuSvgMapToRecord *pMap = new stuSvgMapToRecord();
	pMap->pSvgObj = pSvgObj;
	pMap->m_iSvgMapType = eType;

	SString ds_name = SString::GetAttributeValue(sExtAttr,"ds");
	if(ds_name.length() == 0)
		ds_name = m_sDatasetName;

	pMap->pDataset = GET_DW_CFG->SearchDataset(ds_name);
	if(pMap->pDataset != NULL)
	{
		//如果指定数据集尚未加载，则立即加载其数据
		bool bNewDataset=false;
		SRecordset *pRs = NULL;
		//如果数据集为动态数据集，且不在已克隆的数据集中时(图表图元时例外)，则生成动态数据集实例
		if(eType == SVGMAP_CHART_PIE || eType == SVGMAP_CHART_COL || eType == SVGMAP_CHART_LINE)
		{
			//克隆动态数据集
			pMap->pDataset = pMap->pDataset->Clone();
			//赋值扩展属性中的会话属性值到当前会话
			SSP_GUI->SetSessionAttributeValues(sExtAttr);
			//根据会话属性固化动态条件
			pMap->pDataset->PrepareDynamicCondition();
			m_DynSvgMapDataset.append(pMap->pDataset);
		}
		else if(pMap->pDataset->IsDynamicDataset())
		{
			CSsp_Dataset *pDs = SearchDynSvgMapDataset(pMap->pDataset);
			if(pDs != NULL)
				pMap->pDataset = pDs;
			else
			{
				//克隆动态数据集
				pMap->pDataset = pMap->pDataset->Clone();
				//赋值扩展属性中的会话属性值到当前会话
				SSP_GUI->SetSessionAttributeValues(sExtAttr);
				//根据会话属性固化动态条件
				pMap->pDataset->PrepareDynamicCondition();
				m_DynSvgMapDataset.append(pMap->pDataset);
			}
		}
		else if(!m_SvgMapDataset.exist(pMap->pDataset))
		{
			if(pMap->pDataset->IsInnerRsIsDirty())
				pMap->pDataset->GetRecordset(NULL,m_sCondition);
			bNewDataset = true;
		}
		if(eType < SVGMAP_CHART_PIE || eType == SVGMAP_CHART_PANEL)
		{
			//非图表时映射数据点，图表时映射数据集
			//pMap->sRowPrimKey = SString::GetAttributeValue(sExtAttr,"row");
			pMap->iRowIndex = SString::GetAttributeValueI(sExtAttr,"row")-1;
			pRs = pMap->pDataset->GetInnerRecordset();
			if(pMap->iRowIndex < 0 || pMap->iRowIndex >= pRs->GetRows())
			{
				LOGERROR("SVG图元映射行[%d]超出了数据集[%s]行范围!扩展属性为:%s",pMap->iRowIndex,ds_name.data(),sExtAttr.data());
				delete pMap;
				return;
			}
			pMap->iColIndex = SString::GetAttributeValueI(sExtAttr,"col")-1;
			if(pMap->iColIndex < 0 || pMap->iColIndex >= pMap->pDataset->GetColumnCount())
			{
				LOGERROR("SVG图元映射列[%d]超出了数据集[%s]列范围!扩展属性为:%s",pMap->iColIndex,ds_name.data(),sExtAttr.data());
				delete pMap;
				return;
			}
			//pMap->pDsColumn = pMap->pDataset->GetColumn(iCol-1);
		}
		else
		{
			pMap->iRowIndex = -1;
			pMap->iColIndex = -1;
			if(pMap->pDataset->GetType() == 1)//mdb
				pRs = pMap->pDataset->GetInnerRecordsetDirect();
			else
				pRs = pMap->pDataset->GetInnerRecordset();
			//pMap->pDsColumn = NULL;
		}
		if(bNewDataset)
		{
			m_SvgMapDataset.append(pMap->pDataset);
			//注册触发回调函数
			//pMap->pDataset->RegisterTriggerCallback(OnMdbDsTrgCallback,this);
		}
//by skt  为什么去掉？
		SRecord *pRec;
 		if(pRs != NULL && (pRec = pRs->GetRecord(pMap->iRowIndex)) != NULL)
 			UpdateSvgState(pMap,pRec);
	}

	if(flash == 1)
	{
		SString f_ds_name = SString::GetAttributeValue(sExtAttr,"f_ds");
		SRecordset *pRs = NULL;
		bool bNewDataset=false;
		if(f_ds_name == ds_name && pMap->pDataset != NULL)
		{
			pMap->pFlashDataset = pMap->pDataset;
			pRs = pMap->pFlashDataset->GetInnerRecordset();
		}
		else
		{
			//自行关联数据集
			if(f_ds_name.length() == 0)
				f_ds_name = m_sDatasetName;
			pMap->pFlashDataset = GET_DW_CFG->SearchDataset(f_ds_name);

			if(pMap->pFlashDataset == NULL)
			{
				LOGERROR("SVG图元闪烁映射数据集[%s]无效",f_ds_name.data());
				goto end;
			}
			//如果指定数据集尚未加载，则立即加载其数据
			//如果数据集为动态数据集，则生成动态数据集实例
			else if(pMap->pFlashDataset->IsDynamicDataset())
			{
				//克隆动态数据集
				pMap->pFlashDataset = pMap->pFlashDataset->Clone();
				//赋值扩展属性中的会话属性值到当前会话
				SSP_GUI->SetSessionAttributeValues(sExtAttr);
				//根据会话属性固化动态条件
				pMap->pFlashDataset->PrepareDynamicCondition();
				m_DynSvgMapDataset.append(pMap->pFlashDataset);
			}
			else if(!m_SvgMapDataset.exist(pMap->pFlashDataset))
			{
				if(pMap->pFlashDataset->IsInnerRsIsDirty())
					pMap->pFlashDataset->GetRecordset(NULL,m_sCondition);
				bNewDataset = true;
			}
		}
		pMap->iFlashRowIndex = SString::GetAttributeValueI(sExtAttr,"f_row")-1;
		pRs = pMap->pFlashDataset->GetInnerRecordset();
		if(pMap->iFlashRowIndex < 0 || pMap->iFlashRowIndex >= pRs->GetRows())
		{
			LOGERROR("SVG图元闪烁映射行[%d]超出了数据集[%s]行范围!扩展属性为:%s",pMap->iFlashRowIndex,f_ds_name.data(),sExtAttr.data());
			goto end;
		}
		pMap->iFlashColIndex = SString::GetAttributeValueI(sExtAttr,"f_col")-1;
		if(pMap->iFlashColIndex < 0 || pMap->iFlashColIndex >= pMap->pFlashDataset->GetColumnCount())
		{
			LOGERROR("SVG图元闪烁映射列[%d]超出了数据集[%s]列范围!扩展属性为:%s",pMap->iFlashColIndex,f_ds_name.data(),sExtAttr.data());
			goto end;
		}
		//pMap->pDsColumn = pMap->pDataset->GetColumn(iCol-1);
			
		if(bNewDataset)
		{
			m_SvgMapDataset.append(pMap->pFlashDataset);
			//注册触发回调函数
			//pMap->pDataset->RegisterTriggerCallback(OnMdbDsTrgCallback,this);
		}
		if(pMap->pFlashDataset != NULL && pRs != NULL)
		{
			SRecord *pR = pRs->GetRecord(pMap->iFlashRowIndex);
			if(pR != NULL)
			{
				pSvgObj->SetAutoFlash(pR->GetValue(pMap->iFlashColIndex) == "0");//0表示自动闪烁
				m_SvgItemAutoFlashMaps.append(pMap);
			}
		}
	}//自动闪烁

end:
	if(pMap->pDataset == NULL && pMap->pFlashDataset == NULL)
	{
		LOGERROR("SVG图元(或闪烁)映射了无效的数据集!");
		delete pMap;
		return;
	}

	pMap->sExtAttr = sExtAttr;
	pSvgObj->SetUserPtr(pMap);
	m_SvgItemMaps.append(pMap);
	// 	if(eType == SVGMAP_TEXT || eType == SVGMAP_STATE)
	// 		pSvgObj->SetUserPtr(pMap);
	//更新图元属性
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  更新指针映射图元的属性
// 作    者:  邵凯田
// 创建时间:  2015-8-6 16:18
// 参数说明:  @pMap为映射关系
//         :  @pR为对应的行，NULL表示未知对应具体行
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CViewMdbSvg::UpdateSvgState(stuSvgMapToRecord *pMap,SRecord *pR)
{
	switch(pMap->m_iSvgMapType)
	{
	case SVGMAP_TEXT:
		if(pMap->pDataset != NULL /*&& pMap->pDsColumn != NULL*/)
		{
			SString sDime = SString::GetAttributeValue(pMap->sExtAttr,"dime");
			//if(pR == NULL)
			//	pR = pMap->pDataset->SearchRecordByPrimKey(NULL,pMap->sRowPrimKey);
			if(pR != NULL)
				pMap->pSvgObj->SetAttributeValue("text",pR->GetValue(pMap->iColIndex)+sDime);
		}
		break;
	case SVGMAP_STATE:
		if(pMap->pDataset != NULL /*&& pMap->pDsColumn != NULL*/)
		{
			//if(pR == NULL)
			//	pR = pMap->pDataset->SearchRecordByPrimKey(NULL,pMap->sRowPrimKey);
			if(pR != NULL)
			{
				SString sVal = pR->GetValue(pMap->iColIndex);

				bool bMultiStObj = false;
				if(pMap->pSvgObj->GetType() == SSvgObject::TYPE_GROUP)
				{
					//判断子元素是否有配置了st_val属性的
					SSvgObject *pSub = pMap->pSvgObj->GetChild();
					while(pSub)
					{
						if(pSub->GetAttribute("st_val").length() > 0)
						{
							bMultiStObj = true;
							break;
						}
						pSub = pSub->GetNext();
					}
				}
				if(bMultiStObj)
				{
					//多状态分组图元，将当前状态值写入st_val属性即可
					pMap->pSvgObj->SetAttributeValue("show_st",sVal);
					break;
				}
				if(pMap->pSvgObj->GetType() == SSvgObject::TYPE_LINE || pMap->pSvgObj->GetType() == SSvgObject::TYPE_POLYLINE)
				{
					SString str = pMap->pSvgObj->GetAttribute("st_flash");
					bool bOK = false;
					if(str.length() > 0)
					{
						//状态驱动图元是否闪烁
						pMap->pSvgObj->SetAutoFlash(sVal == str);
						bOK = true;
					}
					str = pMap->pSvgObj->GetAttribute("st_dash");
					if(str.length() > 0)
					{
						//0-0,1-2  表示状态0对应样式0，状态1对应样式2
						int i,cnt = SString::GetAttributeCount(str,",");
						for(i=1;i<=cnt;i++)
						{
							SString sLine = SString::GetIdAttribute(i,str,",");
							if(SString::GetIdAttribute(1,sLine,"-") == sVal)
							{
								pMap->pSvgObj->SetAttributeValue("stroke-dasharray",SString::GetIdAttribute(2,sLine,"-"));
								break;
							}
						}		
						bOK = true;
					}
					if(bOK)
						break;
				}

				if(sVal.left(1) == "#" || sVal.left(4) == "rgb(")
				{
					//状态值为颜色类型，直接更新到对象背景色中即可
					if(pMap->pSvgObj->GetType() == SSvgObject::TYPE_LINE || pMap->pSvgObj->GetType() == SSvgObject::TYPE_POLYLINE)
						pMap->pSvgObj->SetAttributeValue("stroke",sVal);
					else
						pMap->pSvgObj->SetAttributeValue("fill",sVal);
					break;
				}
				SString fc = SString::GetAttributeValue(pMap->sExtAttr,"fc");//缺省背景颜色
				SString color;
				int iVal = sVal.toInt();
				if(fc.length() > 0)
				{
					color = SString::GetAttributeValue(pMap->sExtAttr,SString::toFormat("fc_%d",iVal));
					if(color.length() == 0)
						color = fc;
					pMap->pSvgObj->SetAttributeValue("fill",color);
				}
				SString lc = SString::GetAttributeValue(pMap->sExtAttr,"lc");//缺省线颜色
				if(lc.length() > 0)
				{
					color = SString::GetAttributeValue(pMap->sExtAttr,SString::toFormat("lc_%d",iVal));
					if(color.length() == 0)
						color = lc;
					pMap->pSvgObj->SetAttributeValue("stroke",color);
				}
			}
		}
		break;
	case SVGMAP_CHART_PIE:
		break;
	case SVGMAP_CHART_COL:
		break;
	case SVGMAP_CHART_LINE:
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  实时刷新SVG显示，不直接刷新，发送消息
// 作    者:  邵凯田
// 创建时间:  2015-8-7 15:30
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CViewMdbSvg::RefreshSvg()
{
	m_iSvgUpdateSn++;
	if(m_iSvgUpdateSn > 2147483600)
		m_iSvgUpdateSn = 0;
	//emit DoRefSvg();
	
}

void CViewMdbSvg::OnRefreshSvg()
{
	if(m_LastRefresgSvgTime != SDateTime::getNowSoc())
	{
		OnTimerRepaint();
	}
}

void CViewMdbSvg::OnTimerRepaint()
{
	if(m_iSvgShowSn == m_iSvgUpdateSn || !isVisible())
		return;
	int iUpdSn = m_iSvgUpdateSn;
	//处理自动闪烁状态更新
	unsigned long pos=0;
	SRecordset *pRs;
	SRecord *pR;
	bool bAutoFlash;
	stuSvgMapToRecord *pMap = m_SvgItemAutoFlashMaps.FetchFirst(pos);
	while(pMap)
	{
		pRs = pMap->pFlashDataset->GetInnerRecordset();
		pR = pRs->GetRecord(pMap->iFlashRowIndex);
		if(pR != NULL)
		{
			bAutoFlash = pR->GetValue(pMap->iFlashColIndex) == "0";//0表示自动闪烁
			if(bAutoFlash != pMap->pSvgObj->IsAutoFlash())
			{
				m_pSvg->Render();
				pMap->pSvgObj->SetAutoFlash(bAutoFlash);
			}
		}
		pMap = m_SvgItemAutoFlashMaps.FetchNext(pos);
	}
	m_pSvg->repaint();
	m_LastRefresgSvgTime = SDateTime::getNowSoc();
	m_iSvgShowSn = iUpdSn;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  当窗口显示后调用虚函数
// 作    者:  邵凯田
// 创建时间:  2015-8-4 14:27
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CViewMdbSvg::OnAftShow()
{
	m_pTimer->stop();
	m_pTimer->start(10);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取图元的提示文本，优先通过应用层回调函数提取，取不到返回图元的title文本
// 作    者:  邵凯田
// 创建时间:  2015-12-8 14:45
// 参数说明:  @pSvgObj
// 返 回 值:  SString
//////////////////////////////////////////////////////////////////////////
SString CViewMdbSvg::GetSvgObjectTipText(SSvgObject *pSvgObj)
{
	SString sText;
	SString sExtAttr = pSvgObj->GetAttribute("ext-attribute");
	if(sExtAttr.length() > 0 && SSP_GUI->GetSvgObjectTipText(pSvgObj,sText))
		return sText;
	return m_pSvg->SSvgWnd::GetSvgObjectTipText(pSvgObj);
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  将最后执行点击的SVG组态窗口重定向到另一个组态页面
// 作    者:  邵凯田
// 创建时间:  2016-3-1 18:14
// 参数说明:  @wnd_sn为组态窗口序号
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CViewMdbSvg::JumpWnd(int wnd_sn)
{
	if(g_pLastClickMdbSvg == NULL)
	{
		SQt::ShowErrorBox(tr("错误"),tr("尚未指定SVG组态窗口,界面跳转请求无法执行!"));
		return;
	}
	if(wnd_sn > 0 /*&& wnd_sn != g_pLastClickMdbSvg->m_iWndSn*/)
	{
		SString sql;
		sql.sprintf("select count(*) from t_ssp_uicfg_wnd where wnd_sn=%d and wnd_type=1",wnd_sn);
		if(DB->SelectIntoI(sql) > 0)
		{
			//指定的组态窗口存在
			g_pLastClickMdbSvg->m_iWndSn = wnd_sn;
			g_pLastClickMdbSvg->m_sDatasetName = DB->SelectInto(SString::toFormat("select dataset from t_ssp_uicfg_wnd where wnd_sn=%d and wnd_type=1",wnd_sn));
			g_pLastClickMdbSvg->OnTimerReloadSvg();
		}
	}
}

void CViewMdbSvg::OnClickObject(SSvgObject *pSvgObj)
{
	SString sExtAttr = pSvgObj->GetAttribute("ext-attribute");
	if(sExtAttr.length() == 0)
		return;
	g_pLastClickMdbSvg = this;
	//将扩展属性设置到会话属性中，以更新动态条件，中间可能有一些不是动态属性的也会被一并写入了，SSP_BASE会过滤已知无效属性
	SSP_BASE->SetSessionAttributeValues(sExtAttr);
	SString type = SString::GetAttributeValue(sExtAttr,"type");
	SString act = SString::GetAttributeValue(sExtAttr,"act");
	if(type == "text")
	{
		//9.1.2.1	文本图元映射
	}
	else if(type == "state")
	{
		//9.1.2.2	简单状态图元映射
	}
	if(act == "jump_fun" || type == "jump_fun")
	{
		//9.1.2.1	功能跳转点映射
		SString sFunKey = SString::GetAttributeValue(sExtAttr,"to");
		if(!GotoFunPoint(sFunKey))
		{
			SQt::ShowErrorBox(tr("提示"),SQt::qsprintf(tr("跳转到功能点[%s]时失败!"),sFunKey.data()),this);
		}
	}
	else if(act == "jump_wnd" || type == "jump_wnd")
	{
		//9.1.2.1	组态窗口跳转点映射
		int wnd_sn = SString::GetAttributeValueI(sExtAttr,"to");
		if(wnd_sn > 0 && wnd_sn != m_iWndSn)
		{
			SString sql;
			sql.sprintf("select count(*) from t_ssp_uicfg_wnd where wnd_sn=%d and wnd_type=1",wnd_sn);
			if(DB->SelectIntoI(sql) > 0)
			{
				//指定的组态窗口存在
				m_iWndSn = wnd_sn;
				m_sDatasetName = DB->SelectInto(SString::toFormat("select dataset from t_ssp_uicfg_wnd where wnd_sn=%d and wnd_type=1",wnd_sn));
				OnTimerReloadSvg();
			}
		}
	}
	else if(act == "general_act" || type == "general_act")
	{
		int act_sn = SString::GetAttributeValueI(sExtAttr,"to");
		SSP_GUI->RunGeneralAction(act_sn);		
	}
	else if(act == "float_wnd" || type == "float_wnd")
	{
		//弹出浮动组态窗口
		int wnd_sn = SString::GetAttributeValueI(sExtAttr,"to");
		if(wnd_sn > 0)
		{
			SString sw = SString::GetAttributeValue(sExtAttr,"w");
			SString sh = SString::GetAttributeValue(sExtAttr,"h");
			int virtualWidth = 800;
			int virtualHeight = 600;
			QDesktopWidget *deskWgt = QApplication::desktop();
			if(deskWgt != NULL)
			{
				virtualWidth = deskWgt->width();  
				virtualHeight = deskWgt->height();  
			}
			if(sw.right(1) == "%")
			{
				sw = SString::toString(virtualWidth * sw.toInt()/100);
			}
			if(sh.right(1) == "%")
			{
				sh = SString::toString(virtualHeight * sh.toInt()/100);
			}
			SString title = SString::GetAttributeValue(sExtAttr,"title");
			if(title.length() > 0)
			{
				SString sFldKey;
				int p1,p2;
				p1 = title.find("{@SESSION:");
				while(p1 >= 0)
				{
					p2 = title.find("@}",p1);
					if(p2 < 0)
						break;
					sFldKey = title.mid(p1+10,p2-p1-10);
					title = title.left(p1)+SSP_BASE->GetSessionAttributeValue(sFldKey.data())+title.mid(p2+2);
					p1 = title.find("{@SESSION:");
				}
			}
			SSP_GUI->OpenFloatWnd(wnd_sn,sw.toInt(),sh.toInt(),SString::GetAttributeValueI(sExtAttr,"alpha"),title);
			//SSP_GUI->OpenDialogWnd(wnd_sn,sw.toInt(),sh.toInt(),SString::GetAttributeValueI(sExtAttr,"alpha"),title);
			/*
			SString sql;
			//sql.sprintf("select count(*) from t_ssp_uicfg_wnd where wnd_sn=%d and wnd_type=1",wnd_sn);
			int wnd_type=DB->SelectIntoI(SString::toFormat("select wnd_type from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn));
			unsigned long pos=0;
			CBaseView *pView = m_ViewFloat.FetchFirst(pos);
			while(pView)
			{
				if(pView->m_iWndSn == wnd_sn)
				{
					//pView->setWindowTitle(SString::GetAttributeValue(sExtAttr,"t").data());
					float alpha = SString::GetAttributeValueF(sExtAttr,"alpha")/100;
					if(alpha <= 0.0001)
						alpha = 1;
					else if(alpha < 0.1)
						alpha = 0.1;
					else if(alpha > 1)
						alpha = 1;
					SString title = SString::GetAttributeValue(sExtAttr,"title");
					if(title.length() == 0)
						pView->setWindowTitle(title.data());
					pView->setWindowOpacity(alpha);
					pView->OnPreShow();
					pView->show();
					pView->OnAftShow();
					return;
				}
				pView = m_ViewFloat.FetchNext(pos);
			}

			if(wnd_type == 1)//1-	映射到内存数据表的SVG图形监视窗口
			{
				//指定的组态窗口存在
				//先从已创建的窗口中查找

				//找不到，则创建新的窗口
				sql.sprintf("select dataset,ds_cond,wnd_name from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn);
				CViewMdbSvg *pNew = new CViewMdbSvg(wnd_sn,NULL,true,&sExtAttr);
				pView = pNew;
				SString title = SString::GetAttributeValue(sExtAttr,"title");
				if(title.length() == 0)
					title = DB->SelectInto(sql,2);
				pNew->setWindowTitle(title.data());
				pNew->m_pParentView = this;
				pNew->SetDatasetName(DB->SelectInto(sql));
				pNew->SetCondition(DB->SelectInto(sql,1));
			}
			else if(wnd_type == 2 || wnd_type == 4)//2-	映射到内存数据表的动态表格  4-	映射到历史数据表的数据窗口，查询窗口
			{
				bool bMdb = DB->SelectIntoI("select count(*) from t_ssp_data_window where name='%s' and dbtype=3")>0;
				sql.sprintf("select dataset,ds_cond from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn);
				SString ds = DB->SelectInto(sql);
				view_query *pNew = new view_query(NULL,ds,bMdb);
				pView = pNew;
				pNew->m_iWndSn = wnd_sn;
				SString title = SString::GetAttributeValue(sExtAttr,"title");
				if(title.length() == 0)
					title = DB->SelectInto(SString::toFormat("select dw_desc from t_ssp_data_window where name='%s' ",ds.data()));
				pNew->setWindowTitle(title.data());
			}
			else if(wnd_type == 3)//3-	映射到历史数据表的数据窗口，对应数据窗口配置中的某一数据集，可以配置若干过滤条件，可以分别指定查询/修改/添加/删除功能
			{
				sql.sprintf("select dataset,ds_cond from t_ssp_uicfg_wnd where wnd_sn=%d",wnd_sn);
				SString ds = DB->SelectInto(sql);
				view_common *pNew = new view_common(NULL,ds);
				pView = pNew;
				pNew->m_iWndSn = wnd_sn;
				SString title = SString::GetAttributeValue(sExtAttr,"title");
				if(title.length() == 0)
					title = DB->SelectInto(SString::toFormat("select dw_desc from t_ssp_data_window where name='%s' ",ds.data()));
				pNew->setWindowTitle(title.data());
			}
			else
			{
				return;
			}

			//显示新窗口
			float alpha = SString::GetAttributeValueF(sExtAttr,"alpha")/100;
			if(alpha <= 0.0001)
				alpha = 1;
			else if(alpha < 0.1)
				alpha = 0.1;
			else if(alpha > 1)
				alpha = 1;
			pView->setWindowOpacity(alpha);
			pView->OnPreShow();
			int w = SString::GetAttributeValueI(sExtAttr,"w");
			int h = SString::GetAttributeValueI(sExtAttr,"h");
			if(w == -1 && h == -1)
			{
				pView->showMaximized();
			}
			else
			{
#ifdef WIN32
				pView->setWindowFlags( Qt::Dialog | Qt::Tool | Qt::WindowStaysOnTopHint);
#else
				pView->setWindowFlags( Qt::Dialog );
#endif
				if(w <= 0)
					w = 300;
				if(h <= 0)
					h = 200;
				pView->resize(w,h);
				pView->show();
			}				
			pView->OnAftShow();
			m_ViewFloat.append(pView);

			*/
		}

	}
	else if(act == "float_view" || type == "float_view")
	{
		//弹出浮动组态窗口
		SString sView = SString::GetAttributeValue(sExtAttr,"view");
		if(sView.length() > 0)
		{
			SString sw = SString::GetAttributeValue(sExtAttr,"w");
			SString sh = SString::GetAttributeValue(sExtAttr,"h");
			int virtualWidth = 800;
			int virtualHeight = 600;
			int curMonitor = QApplication::desktop()->screenNumber(this);
			QRect rect = QApplication::desktop()->availableGeometry(curMonitor);
			virtualWidth = rect.width();
			virtualHeight = rect.height();			
			if(sw.right(1) == "%")
			{
				sw = SString::toString(virtualWidth * sw.toInt()/100);
			}
			if(sh.right(1) == "%")
			{
				sh = SString::toString(virtualHeight * sh.toInt()/100);
			}

			//static CBaseView *pView = NULL;
			if(g_pFloatViewWnd != NULL)
			{
				delete g_pFloatViewWnd;
				g_pFloatViewWnd = NULL;
			}
			CBaseView *pView = g_pFloatViewWnd = SSP_GUI->NewView(sView,this);
			if(pView == NULL)
			{
				SQt::ShowErrorBox("错误",SString::toFormat("视图[%s]不存在,请检查插件是否加载完整!").data(),this);
				return;
			}
			SString title = SString::GetAttributeValue(sExtAttr,"title");
			if(title.length() > 0)
			{
				SString sFldKey;
				int p1,p2;
				p1 = title.find("{@SESSION:");
				while(p1 >= 0)
				{
					p2 = title.find("@}",p1);
					if(p2 < 0)
						break;
					sFldKey = title.mid(p1+10,p2-p1-10);
					title = title.left(p1)+SSP_BASE->GetSessionAttributeValue(sFldKey.data())+title.mid(p2+2);
					p1 = title.find("{@SESSION:");
				}
				pView->setWindowTitle(title.data());
			}
			pView->setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
			
			if(sw == "100%" && sh == "100%")
				pView->setMinimumSize(640,480);
			else
				pView->setMinimumSize(sw.toInt(),sh.toInt());
			int cx = (virtualWidth - sw.toInt())/2;
			int cy = (virtualHeight - sh.toInt())/2;
			if(cx < 0)
				cx = 0;
			if(cy < 0)
				cy = 0;
			pView->move(cx,cy);
			pView->setWindowModality(Qt::WindowModal);
			pView->setAttribute(Qt::WA_ShowModal,true);
			//显示新窗口
			float alpha = SString::GetAttributeValueF(sExtAttr,"alpha")/100;
			if(alpha <= 0.0001)
				alpha = 1;
			else if(alpha < 0.1)
				alpha = 0.1;
			else if(alpha > 1)
				alpha = 1;
			pView->setWindowOpacity(alpha);
			pView->OnPreShow();
			if(sw == "100%" && sh == "100%")
				pView->showMaximized();
			else
				pView->show();
			pView->OnAftShow();
		}
	}
}

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
BYTE* CViewMdbSvg::OnMdbDsTrgCallback(void* cbParam, CMdbClient *pMdbClient,CSsp_Dataset *pDataset, eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData)
{
	S_UNUSED(pMdbClient);
	S_UNUSED(iTrgRows);
	S_UNUSED(iRowSize);
	S_UNUSED(pTrgData);
	CViewMdbSvg *pThis = (CViewMdbSvg*)cbParam;
	int iUpdates = 0;
	if(eType == MDB_TRG_UPDATE)
	{
#if 0
		iUpdates++;
#else
		int i;
		SRecord *pR;
		unsigned long  pos;
		stuSvgMapToRecord *pMap;
		SString sPrimKey;
		unsigned int oid;
		int rowidx;
		pDataset->LockInnerRs();
		for(i=0;i<iTrgRows;i++)
		{
			memcpy(&oid,pTrgData,sizeof(oid));
			pR = pDataset->SearchRecordByOid(NULL,oid,rowidx);
			//pR = pDataset->SearchRecordByPrimKey(NULL,pTrgData);
			//sPrimKey = pDataset->GetPrimKeyByRawRecord(NULL,pTrgData);
			pTrgData += iRowSize;
			if(pR == NULL /*|| sPrimKey.length() == 0*/)
				continue;

			//遍历所有MAP，查找对应的数据引用并更新
			pThis->m_SvgItemMaps.lock();
			pMap = pThis->m_SvgItemMaps.FetchFirst(pos);
			while(pMap)
			{
// 				if(pMap->sRowPrimKey.length() == 0)
// 				{
// 					//不指定行，表示全数据集引用(针对图表)
// 					iUpdates++;
// 					//pMap->pDataset->SetInnerRsIsDirty();
// 				}
// 				else 
				if(pMap->iRowIndex == -1)
				{
					iUpdates++;
				}
				else if(pMap->iRowIndex == rowidx)
				{
					//指定了行，确定是否需要更新
					iUpdates++;
					pThis->UpdateSvgState(pMap,pR);
				}
// 				else if(pMap->sRowPrimKey == sPrimKey)
// 				{
// 					//尚未映射行，立即映射
// 					sPrimKey = pDataset->GetPrimKeyByRawRecord(NULL,pTrgData);
// 					iUpdates++;
// 					pThis->UpdateSvgState(pMap,pR);
// 				}
				pMap = pThis->m_SvgItemMaps.FetchNext(pos);
			}
			pThis->m_SvgItemMaps.unlock();
		}
		pDataset->UnlockInnerRs();
#endif
	}
	else
	{
		iUpdates++;
		pDataset->SetInnerRsIsDirty();
	}
	if(iUpdates > 0)
		pThis->RefreshSvg();
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  删除所有触发回调函数的注册
// 作    者:  邵凯田
// 创建时间:  2015-8-7 15:02
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CViewMdbSvg::RealeaseAllTrgCallback()
{
	unsigned long pos;
	CSsp_Dataset *pDs = m_SvgMapDataset.FetchFirst(pos);
	while(pDs)
	{
		pDs->RemoveTriggerCallback(OnMdbDsTrgCallback,this);
		pDs = m_SvgMapDataset.FetchNext(pos);
	}
	
	pDs = m_DynSvgMapDataset.FetchFirst(pos);
	while(pDs)
	{
		pDs->RemoveTriggerCallback(OnMdbDsTrgCallback,this);
		pDs = m_DynSvgMapDataset.FetchNext(pos);
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取对象关联的显示文本
// 作    者:  邵凯田
// 创建时间:  2015-8-18 17:23
// 参数说明:  @pObj为图元指针
//         :  @sText为引用返回的文本内容
// 返 回 值:  true表示取到关联数据，false表示未取到关联数据
//////////////////////////////////////////////////////////////////////////
bool CViewMdbSvg::GetViewText(SSvgObject *pObj,SString &sText)
{
	stuSvgMapToRecord *pMap = (stuSvgMapToRecord *)pObj->GetUserPtr();
	if(pMap == NULL ||pMap->pSvgObj != pObj || pMap->m_iSvgMapType != SVGMAP_TEXT || pMap->pDataset == NULL)
		return false;
	CSsp_DwColumn *pCol = pMap->pDataset->GetColumn(pMap->iColIndex);
	pMap->pDataset->LockInnerRs();
	sText = pMap->pDataset->GetInnerRecordsetDirect()->GetValue(pMap->iRowIndex,pMap->iColIndex);
	//sText = pMap->pDataset->GetInnerRecordset()->GetValue(pMap->iRowIndex,pMap->iColIndex);
	if(pCol->m_pRef != NULL)
	{
		sText = pCol->m_pRef->GetRefValue(sText);
	}
	else if(pCol->m_sConv.length() > 0)
		sText = pCol->GetConvDb2Sys(sText);
	sText += SString::GetAttributeValue(pMap->sExtAttr,"dime");
	pMap->pDataset->UnlockInnerRs();
	return true;
}

bool CViewMdbSvg::GetViewTextColor(SSvgObject *pObj,SCOLOR &sColor)
{
	stuSvgMapToRecord *pMap = (stuSvgMapToRecord *)pObj->GetUserPtr();
	if(pMap == NULL ||pMap->pSvgObj != pObj || pMap->m_iSvgMapType != SVGMAP_TEXT || 
		pMap->pDataset == NULL || pMap->pDataset->GetTableName().compareNoCase("t_ssp_tagged_text") != 0)
		return false;
	//CSsp_DwColumn *pCol = pMap->pDataset->GetColumn(pMap->iColIndex);
	pMap->pDataset->LockInnerRs();
	int color = pMap->pDataset->GetInnerRecordsetDirect()->GetValue(pMap->iRowIndex,3).toInt();
	memcpy(&sColor,&color,4);
	pMap->pDataset->UnlockInnerRs();
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取对象关联的线条颜色
// 作    者:  邵凯田
// 创建时间:  2015-8-18 17:24
// 参数说明:  @pObj为图元指针
//         :  @sColor为引用返回的颜色
// 返 回 值:  true表示取到关联数据，false表示未取到关联数据
//////////////////////////////////////////////////////////////////////////
bool CViewMdbSvg::GetViewLineColor(SSvgObject *pObj,SCOLOR &sColor)
{
	stuSvgMapToRecord *pMap = (stuSvgMapToRecord *)pObj->GetUserPtr();
	if(pMap == NULL ||pMap->pSvgObj != pObj || pMap->m_iSvgMapType != SVGMAP_STATE || pMap->pDataset == NULL)
		return false;
	pMap->pDataset->LockInnerRs();
	SString sVal = pMap->pDataset->GetInnerRecordsetDirect()->GetValue(pMap->iRowIndex,pMap->iColIndex);
	pMap->pDataset->UnlockInnerRs();
	SString color;
	if(sVal.left(1) == "#" || sVal.left(4) == "rgb(")
	{
		color = SString::GetAttributeValue(pMap->sExtAttr,"lc");//缺省线颜色
		sColor = SSvgPainter::GetColorByString(color);
		return true;
	}
	int iVal = sVal.toInt();
	color = SString::GetAttributeValue(pMap->sExtAttr,SString::toFormat("lc_%d",iVal));
	if(color.length() == 0)
		color = SString::GetAttributeValue(pMap->sExtAttr,"lc");//缺省线颜色
	sColor = SSvgPainter::GetColorByString(color);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取对象关联的填充颜色
// 作    者:  邵凯田
// 创建时间:  2015-8-18 17:25
// 参数说明:  @pObj为图元指针
//         :  @bFill为引用返回的是否填充
//         :  @sColor为引用返回的颜色
// 返 回 值:  true表示取到关联数据，false表示未取到关联数据
//////////////////////////////////////////////////////////////////////////
bool CViewMdbSvg::GetViewBackColor(SSvgObject *pObj,bool &bFill,SCOLOR &sColor)
{
	stuSvgMapToRecord *pMap = (stuSvgMapToRecord *)pObj->GetUserPtr();
	if(pMap == NULL ||pMap->pSvgObj != pObj || pMap->m_iSvgMapType != SVGMAP_STATE || pMap->pDataset == NULL)
		return false;
	pMap->pDataset->LockInnerRs();
	SString sVal = pMap->pDataset->GetInnerRecordsetDirect()->GetValue(pMap->iRowIndex,pMap->iColIndex);
	pMap->pDataset->UnlockInnerRs();
	if(sVal.left(1) == "#" || sVal.left(4) == "rgb(")
	{
		bFill = true;
		sColor = SSvgPainter::GetColorByString(sVal);
		return true;
	}
	int iVal = sVal.toInt();
	SString color;
	color = SString::GetAttributeValue(pMap->sExtAttr,SString::toFormat("fc_%d",iVal));
	if(color.length() == 0)
		color = SString::GetAttributeValue(pMap->sExtAttr,"fc");//缺省背景颜色
	if(color.length() == 0)
	{
		bFill = false;
	}
	else
	{
		bFill = true;
		sColor = SSvgPainter::GetColorByString(color);
	}
	return true;
}

void CViewMdbSvg::hideEvent(QHideEvent *event)
{
	S_UNUSED(event);
// 	if(m_pParentView != NULL)
// 	{
// 		//m_pParentView->m_ViewFloat.remove(this);
// 	}
}