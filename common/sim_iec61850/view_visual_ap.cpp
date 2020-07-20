#include "view_visual_ap.h"
#include "GuiApplication.h"

view_visual_ap::view_visual_ap(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	QGridLayout *gridLayout_2;
	gridLayout_2 = new QGridLayout(this);
	gridLayout_2->setSpacing(0);
	gridLayout_2->setContentsMargins(0, 0, 0, 0);
	gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));

	CSimConfig *pSimConfig = &CGuiApplication::GetApp()->m_SimConfig;
	m_pSvgWnd = NULL;
#if 1
	m_pSvgWnd = new CSpVtSvgAccessPoint(this);
	m_SvgGenerator.SetSclVt(&pSimConfig->m_VirtualTerminal);
	m_pSvgWnd->SetSvgGenerator(&m_SvgGenerator);
	gridLayout_2->addWidget(m_pSvgWnd);
	m_SvgGenerator.GeneratorSubNetSvg(m_sSubNetName,m_SvgRoot);
	m_pSvgWnd->Load(&m_SvgRoot);
	m_pSvgWnd->SetOffset(0,0);
	m_pSvgWnd->SetScale(1);
	m_pSvgWnd->Render();
	m_pSvgWnd->RedrawWindow();
	m_pSvgWnd->SetMovable(true);
#endif
}

view_visual_ap::~view_visual_ap()
{

}

void view_visual_ap::OnPreShow()
{
	if(m_pSvgWnd == NULL)
		return;
	m_SvgGenerator.GeneratorSubNetSvg(m_sSubNetName,m_SvgRoot);
	m_pSvgWnd->Load(&m_SvgRoot);
}
