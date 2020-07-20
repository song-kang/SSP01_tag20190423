#ifndef LOADDBTHREAD_H
#define LOADDBTHREAD_H

#include <QThread>
#include <QtGui>
#include "SApplication.h"
#include "ssp_base.h"
#include "ssp_gui_3d_inc.h"
class CSsp3DPannel;

class SSP_GUI_3D_EXPORT LoadDbThread : public QThread
{
	Q_OBJECT

public:
	LoadDbThread(QObject *parent);
	~LoadDbThread();

	void set3DPannel(CSsp3DPannel *p) { m_3DPannel = p; }

protected:
	virtual void run();
	
private:
	CSsp3DPannel *m_3DPannel;

private:
	void makeCharBMP(SString filename,SString path,float width,float hight);

};

#endif // LOADDBTHREAD_H
