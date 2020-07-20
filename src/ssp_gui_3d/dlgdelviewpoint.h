#ifndef DLGDELVIEWPOINT_H
#define DLGDELVIEWPOINT_H

#include "ssp_3d_model.h"
#include <QDialog>
#include "ui_dlgdelviewpoint.h"

class DlgDelViewPoint : public QDialog
{
	Q_OBJECT

public:
	DlgDelViewPoint(QWidget *parent = 0);
	~DlgDelViewPoint();

	void set3DModel(CSsp3DModel *mode) { m_3DModel = mode;}
	void setSceneId(int id) { m_sceneId = id; }

	void start();

private:
	Ui::DlgDelViewPoint ui;

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

#endif // DLGDELVIEWPOINT_H
