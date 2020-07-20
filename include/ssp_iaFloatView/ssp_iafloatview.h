#ifndef SSP_IAFLOATVIEW_H
#define SSP_IAFLOATVIEW_H

#include <QtCore>
#include <QtGui>
#include "ssp_database.h"

#ifdef WIN32
  #ifdef _DEBUG
    #pragma comment(lib,"ssp_based.lib")
    #pragma comment(lib,"ssp_guid.lib")
  #else
    #pragma comment(lib,"ssp_base.lib")
    #pragma comment(lib,"ssp_gui.lib")
  #endif

  #ifdef SSP_FLOAT_WND_LIB_STATIC
  #define SSP_FLOAT_WND_EXPORT
  #else
    #ifdef SSP_FLOAT_WND_EXPORT_DLL
    #define SSP_FLOAT_WND_EXPORT __declspec(dllexport)
    #else
    #define SSP_FLOAT_WND_EXPORT __declspec(dllimport)
    #ifdef NDEBUG
    #pragma comment(lib,"ssp_iaFloatView.lib")
    #else
    #pragma comment(lib,"ssp_iaFloatViewd.lib")
    #endif
    #endif
  #endif
#else//WIN32
  #define SSP_FLOAT_WND_EXPORT
#endif//WIN32


class SSP_FLOAT_WND_EXPORT ssp_iaFloatView : public QWidget
{
	Q_OBJECT

public:
	ssp_iaFloatView(QWidget *parent = 0);
	~ssp_iaFloatView();

	QString ReadSettings(QString path);
	void WriteSettings(QString path,QString value);
	static BYTE* OnRealEventCallback(void* cbParam, SString &sTable, eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData);
	void SetLastHisSequence(unsigned int seq) { m_iLastHisSequence = seq; }
	bool OnConfirmHistory(int sn);
	void RefreshFloatWnd();

public:
	virtual void IAalarmDetail(int ia_his) {};
	virtual void IAalarmQuery();

public:
	void *m_iMenu;
	void *m_iAlarmWnd;
	QMutex m_iLock;

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent *event);

public slots:
	void setIa2AlarmPopWndHidden(bool bhidden = true);
	void setIa2AlarmPopWndOnTop(bool btop);

private:
	QFormLayout *m_iFormLayout;
	QLabel *m_iLabelCritical;
	QLabel *m_iLabelImportant;
	QLabel *m_iLabelSecondary;
	QLabel *m_iLabelGeneral;
	QLabel *m_iLabelCriticalNumber;
	QLabel *m_iLabelImportantNumber;
	QLabel *m_iLabelSecondaryNumber;
	QLabel *m_iLabelGeneralNumber;

	QPoint m_dragPosition;
	QPixmap m_pixFloatView;
	QSettings *m_iSettings;

	QTimer *m_iScanTimer;
	unsigned int m_iLastHisSequence;

private:
	void Init();
	void InitIA2UI();
	void InitIA2Menu();
	void InitIA2AlarmWnd();
	void GetSeverityNum(int&,int&,int&,int&);
	void MovePostion();

private slots:
	void SlotScanTimeout();

};

#endif // SSP_IAFLOATVIEW_H
