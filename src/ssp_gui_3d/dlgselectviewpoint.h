#ifndef DLGSELECTVIEWPORT_H
#define DLGSELECTVIEWPORT_H

#include "ssp_3d_model.h"
#include <QDialog>
#include "ui_dlgselectviewpoint.h"

class DlgSelectViewPoint : public QDialog
{
	Q_OBJECT

public:
	DlgSelectViewPoint(QWidget *parent = 0);
	~DlgSelectViewPoint();

	void set3DModel(CSsp3DModel *mode) { m_3DModel = mode;}
	void setSceneId(int id) { m_sceneId = id; }
	int getSceneId() { return m_sceneId; }
	int getViewPointId() { return m_viewPointId; }

	void start();

private:
	Ui::DlgSelectViewPoint ui;

	CSsp3DModel *m_3DModel;
	int	m_sceneId;
	int m_viewPointId;
	int row;

private:
	void init();
	void initTableWidget();

public slots:
	void slotOk();
	void slotItemClicked(QTableWidgetItem *item);

};

#endif // DLGSELECTVIEWPORT_H
