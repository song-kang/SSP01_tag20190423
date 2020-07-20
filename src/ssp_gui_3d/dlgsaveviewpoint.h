#ifndef DLGSAVEVIEWPOINT_H
#define DLGSAVEVIEWPOINT_H

#include "ssp_3d_model.h"
#include <QDialog>
#include "ui_dlgsaveviewpoint.h"

class DlgSaveViewPoint : public QDialog
{
	Q_OBJECT

public:
	DlgSaveViewPoint(QWidget *parent = 0);
	~DlgSaveViewPoint();

	void set3DModel(CSsp3DModel *mode) { m_3DModel = mode;}
	void setSceneId(int id) { m_sceneId = id; }
	void setEyeX(float x) { eyeX = x; }
	void setEyeY(float y) { eyeY = y; }
	void setEyeZ(float z) { eyeZ = z; }
	void setCenterX(float x) { centerX = x; }
	void setCenterY(float y) { centerY = y; }
	void setCenterZ(float z) { centerZ = z; }
	void setUpX(float x) { upX = x; }
	void setUpY(float y) { upY = y; }
	void setUpZ(float z) { upZ = z; }
	void setEye2Center(float x) { eye2center = x; }
	void setXAngle(float x) { xAngle = x; }
	void setZAngle(float z) { zAngle = z; }

	void start();

private:
	Ui::DlgSaveViewPoint ui;

	CSsp3DModel *m_3DModel;
	int	m_sceneId;
	int m_viewPointId;
	int m_viewPointDefault;
	SString m_viewPointName;
	int row;

	GLdouble eyeX;
	GLdouble eyeY;
	GLdouble eyeZ;
	GLdouble centerX;
	GLdouble centerY;
	GLdouble centerZ;
	GLdouble upX;
	GLdouble upY;
	GLdouble upZ;
	GLdouble eye2center;
	float xAngle;
	float zAngle;

private:
	void init();
	void initTableWidget();
	void setDefaultEnable();
	int getMaxViewPointId();

public slots:
	void slotOk();
	void slotRadioButtonAdd();
	void slotRadioButtonReplace();
	void slotItemClicked(QTableWidgetItem *item);
};

#endif // DLGSAVEVIEWPOINT_H
