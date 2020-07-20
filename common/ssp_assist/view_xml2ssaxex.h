#ifndef __VIEW_XML2SSAXEX_H__
#define __VIEW_XML2SSAXEX_H__

#include "ssp_baseview.h"
#include <QWidget>
#include "ui_view_xml2ssaxex.h"
#include <QDialog>
#include <QtGui>
#include <QtCore>

class CViewXml2SSaxEx : public CBaseView
{
	Q_OBJECT

public:
	CViewXml2SSaxEx(QWidget *parent = 0);
	~CViewXml2SSaxEx();

private:
	void PrepareNodeName(SBaseConfig *p);
	void RemoveMoreNode(SBaseConfig *p);
	void GetNodeStructPre(SString &str,SString prix,SString &sListStr,SBaseConfig *p);
	void GetNodeStruct(SString &str,SString prix,SString &sListStr,SBaseConfig *p);
	void GetNodeCallbackDef(SString &str,SString prix,SBaseConfig *p);
	void GetNodeCallback(SString &str,SString cls_name,SString prix,SBaseConfig *p);
	void GetNodeTempPtr(SString &str,SString prix,SBaseConfig *p);

	Ui::view_xml2ssaxex ui;


private slots:
	void OnBrowser();
	void OnConvert();
	void OnCopyH();
	void OnCopyCpp();
};

#endif // VIEWGSEDATASET_H
