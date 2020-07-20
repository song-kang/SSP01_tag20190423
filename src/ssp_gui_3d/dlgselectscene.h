#ifndef DLGSELECTSCENE_H
#define DLGSELECTSCENE_H

#include "ssp_3d_model.h"
#include <QDialog>
#include "ui_dlgselectscene.h"

#define SCENE_TYPE_EQUAL	1	
#define SCENE_TYPE_CHILD	2

class DlgSelectScene : public QDialog
{
	Q_OBJECT

public:
	DlgSelectScene(QWidget *parent = 0);
	~DlgSelectScene();

	void set3DModel(CSsp3DModel *mode) { m_3DModel = mode;}
	void setSceneId(int id) { m_sceneId = id; }
	void setSceneType(int type) {m_sceneType = type; }
	int getSceneId() { return m_sceneId; }

	void start();

private:
	Ui::DlgSelectScene ui;
	
	CSsp3DModel *m_3DModel;
	int	m_sceneId;
	int m_sceneType;
	int row;

private:
	void init();
	void initTableWidget();

public slots:
	void slotOk();
	void slotItemClicked(QTableWidgetItem *item);

};

#endif // DLGSELECTSCENE_H
