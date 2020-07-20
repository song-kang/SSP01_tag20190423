#ifndef VIEW_VISUAL_AP_H
#define VIEW_VISUAL_AP_H

#include <QWidget>
#include "ui_view_visual_ap.h"
#include "ssp_baseview.h"
#include "sp_vt_visual.h"


class view_visual_ap : public CBaseView
{
	Q_OBJECT

public:
	view_visual_ap(QWidget *parent = 0);
	~view_visual_ap();
	virtual void OnPreShow();
	
private:
	Ui::view_visual_ap ui;
	CSpVtSvgGenerator m_SvgGenerator;
	CSpVtSvgAccessPoint *m_pSvgWnd;
	SXmlConfig m_SvgRoot;
	SString m_sSubNetName;
};

#endif // VIEW_VISUAL_AP_H
