/**
 *
 * 文 件 名 : sp_svglib_config.cpp
 * 创建日期 : 2015-10-26 10:40
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SVG图元库配置文件类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-10-26	邵凯田　创建文件
 *
 **/

#include "sp_svglib_config.h"
#include "ssp_database.h"
#include "SXmlConfig.h"
#include "gui/qt/SSvgLibraryWnd.h"

CSp_SvgLibConfig::CSp_SvgLibConfig()
{
	SetConfigType(SPCFG_SVGLIB);
	SetAutoReload(true);//允许重新加载
}
CSp_SvgLibConfig::~CSp_SvgLibConfig()
{
	
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  加载配置文件
// 作    者:  邵凯田
// 创建时间:  2015-8-1 13:04
// 参数说明:  @sPathFile配置文件名
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CSp_SvgLibConfig::Load(SString sPathFile)
{
	if(!SFile::exists(sPathFile))
		return false;
	SXmlConfig *pXml = SSvgLibraryWnd::GetGlobalXmlConfig();
	pXml->lock();
	pXml->clear();
	if(!pXml->ReadConfig(sPathFile))
	{
		pXml->unlock();
		LOGWARN("打开SVG图元库文件失败!file=%s",sPathFile.data());
		return false;
	}
	pXml->unlock();
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  从数据库加载图元库配置
// 作    者:  邵凯田
// 创建时间:  2015-11-9 13:28
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CSp_SvgLibConfig::LoadByDb()
{
	if(DB->SelectIntoI("select count(*) from t_ssp_svglib_item") <= 0)
		return false;
//	view_svglib_edit::svgFiletoXmlTxt();
	SXmlConfig &pCfg = *SSvgLibraryWnd::GetGlobalXmlConfig();
	SRecordset m_svgType;//主表信息
	SRecordset m_svg;//svg数
	QMap<int,SBaseConfig *>rootMap;
	SDatabaseOper *pDb=DB;
	pCfg.clear();
	pDb->RetrieveRecordset("select svgtype_sn,svgtype_name from t_ssp_svglib_type order by svgtype_sn",m_svgType);
	pDb->RetrieveRecordset("select svg_sn,svgtype_sn,svg_name from t_ssp_svglib_item order by svg_sn",m_svg);//,svg_file
	pCfg.SetNodeName("svg-library");
	QString headSvg="<\?xml version=\"1.0\" encoding=\"GBK\"\?>\n<!--\nGenerated by : Skt-SVG Editor\nDate         : 2015-09-26 12:39:59\nVersion      : 1.0.4.18\n-->\n<svg width=\"\" height=\"\" >";
	QString rearSvg="</svg>";
	for(int i=0;i<m_svgType.GetRows();i++)
	{
		SBaseConfig *pCfgChild=pCfg.AddChildNode("svg-type");
		pCfgChild->SetAttribute("name",m_svgType.GetValue(i,1));
		rootMap.insert(m_svgType.GetValue(i,0).toInt(),pCfgChild);
	}
	for(int i=0;i<m_svg.GetRows();i++)
	{
		int curNo=m_svg.GetValue(i,0).toInt();
		SBaseConfig *pCfgChild=rootMap.value(m_svg.GetValue(i,1).toInt());
		if(pCfgChild!=NULL)
		{
			SBaseConfig *svgNode=pCfgChild->AddChildNode("svg");
			unsigned char* buff=NULL;
			int buflen;
			bool flag=pDb->ReadLobToMem("t_ssp_svglib_item","svg_file",SString::toFormat("svg_sn=%d",curNo),buff,buflen);
			if(flag==false || buff == NULL)
			{
				LOGWARN("数据库读取图元数据失败!");
				continue;
			}
			//QString svgFile=buff;
			//svgFile.remove(QRegExp("<\?xml[^>]*>")).remove(QRegExp("<![^>]*>")).remove("<rect uri=\"\" caption=\"\" x=\"0\" y=\"0\" z=\"\" width=\"1280\" height=\"1024\" stroke=\"\" stroke-width=\"\" stroke-dasharray=\"\" fill=\"#ffffff\" />").remove("<rect uri=\"\" caption=\"\" x=\"0\" y=\"0\" z=\"\" width=\"1280\" height=\"1024\" stroke=\"#000000\" stroke-width=\"1\" stroke-dasharray=\"0\" fill=\"#000000\" />").remove(QRegExp("<svg[^>]*>")).remove(rearSvg).remove("<?");
			char *pSvg = strstr((char*)buff,"<defs>");
			if(pSvg == NULL)
			{
				pSvg = strstr((char*)buff,"<rect");
				if(pSvg != NULL)
					pSvg = strstr(pSvg,"/>");
				if(pSvg != NULL)
					pSvg += 2;
			}
			else
			{
				int p1 = -1,p2 = -1;
				char *pStr1;
				pStr1 = strstr((char*)buff,"<rect");
				if(pStr1 != NULL)
				{
					p1 = pStr1-(char*)buff;
					pStr1 = strstr(pStr1,"/>");
					if(pStr1 != NULL)
						p2 = pStr1+2-(char*)buff;
				}
				if(p1 > 0 && p2 > 0)
				{
					for(int i=p1;i<p2;i++)
						buff[i] = ' ';
				}
			}
			if(pSvg != NULL)
			{
				char *pTail = strstr(pSvg,"</svg>");
				if(pTail != NULL)
					pTail[0] = '\0';
				svgNode->SetAttribute("name",m_svg.GetValue(i,2));
				svgNode->SetNodeValue(pSvg/*svgFile.toStdString().data()*/);
				//svgNode->SetNodeValue("<![CDATA["+svgNode->GetNodeValue().trim()+"]]>");
			}
			pDb->FreeLobMem(buff);
		}

	}
	return true;
}