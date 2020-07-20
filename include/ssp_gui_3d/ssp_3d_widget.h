/**
 *
 * 文 件 名 : ssp_3d_widget.h
 * 创建日期 : 2017-3-6 19:30
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : OPENGL窗口类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2017-3-6	邵凯田　创建文件
 *
 **/
#ifndef SSP_3D_WIDGET_H
#define SSP_3D_WIDGET_H

#include <math.h>
#include "glew.h"
#include "ssp_3d_model.h"
#include "GLU.h"
#include "glext.h"
#include "glut.h"
#include "ui_ssp_3d_widget.h"
#include "loaddbthread.h"
#include "ssp_3d_vrml.h"
#include "ssp_gui_3d_inc.h"

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"glew32s.lib")

#define PI				3.1415926
#define	BUFFER_LENGTH	1024
#define Point2fT		Tuple2fT

typedef union Tuple2f_t
{
	struct
	{
		GLfloat X, Y;
	} s;
	GLfloat T[2];
} Tuple2fT;

enum eSSPRotate
{
	SSP_ROTATE_MOD_NULL=0,			//0：不旋转
	SSP_ROTATE_MOD_FORWARD=1,		//1：正向旋转(顺时针)
	SSP_ROTATE_MOD_NEGATIVE=2,		//2：反向旋转(逆时针)
};

enum eSSP_LED_STATE
{
	SSP_LED_DEF = 0, //默认状态
	SSP_LED_OFF,	 //分状态
	SSP_LED_ON		 //合状态
};

//--------------------class CSsp3DWidget----------------------
class CSsp3DPannel;
class SSP_GUI_3D_EXPORT CSsp3DWidget : public QWidget
{
	Q_OBJECT

public:
	CSsp3DWidget(QWidget *parent = 0);
	~CSsp3DWidget();

	CSsp3DPannel *getSsp3DPannel() { return m_p3DPannel; }

public:
	virtual eSSP_LED_STATE OnStateById(int id1,int id2,int id3,int id4) { return SSP_LED_DEF; }
	virtual QString OnCabinetAlarm(int id1,int id2,int id3,int id4) { return QString::null; }
	virtual void OnPaint(CSsp3DSceneObj *sceneObj) {};
	
	virtual void OnCabinetDraw(CSsp3DSceneObj *sceneObj) {};
	virtual void OnCabinetBug(CSsp3DSceneObj *sceneObj,QPoint p) {};
	virtual void OnDeviceBook(CSsp3DSceneObj *sceneObj) {};
	virtual void OnDeviceDraw(CSsp3DSceneObj *sceneObj) {};
	virtual void OnDeviceLink(CSsp3DSceneObj *sceneObj) {};
	virtual void OnDeviceInfo(CSsp3DSceneObj *sceneObj) {};
	virtual void OnDeviceBug(CSsp3DSceneObj *sceneObj) {};
	virtual void OnDeviceRealState(CSsp3DSceneObj *sceneObj){};
	virtual void OnDeviceScoreState(CSsp3DSceneObj *sceneObj){};
private:
	Ui::CSsp3DWidget ui;
	QGridLayout *m_pGridLayout;
	CSsp3DPannel *m_p3DPannel;

private:
	int initSceneId();
};

//--------------------class CSsp3DPannel----------------------
class SSP_GUI_3D_EXPORT CSsp3DPannel : public QGLWidget
{
	Q_OBJECT

	friend class LoadDbThread;

public:
	struct stuSceneParam
	{
		stuSceneParam()
		{
			transfX = 0.0;
			transfY = 0.0;
			transfZ = 0.0;
			rotatfX = 0.0;
			rotatfY = 0.0;
			rotatfZ = 0.0;
		}
		GLfloat transfX;
		GLfloat transfY;
		GLfloat transfZ;
		GLfloat rotatfX;
		GLfloat rotatfY;
		GLfloat rotatfZ;
	};

	struct stuEyeParam
	{
		stuEyeParam()
		{
			eyeX = 0.0;
			eyeY = 0.0;
			eyeZ = 0.0;
			centerX = 0.0;
			centerY = 0.0;
			centerZ = 0.0;
			upX = 0.0;
			upY = 0.0;
			upZ = 0.0;
			eye2center = 0.0;
			xAngle = 0.0;
			zAngle = 0.0;
		}
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
	};

public:
	CSsp3DPannel(QGLFormat format,QWidget *parent = 0,int id = 0);
	~CSsp3DPannel();

	void setSceneId(int id) { m_sceneId = id; }
	GLUquadricObj *getQuadricObj() { return m_quadric; }
	CSsp3DModel *get3DModel() { return m_3DModel; }
	void stopTimer(){if(hintTimer != NULL) hintTimer->stop();}

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent *event);
	void keyPressEvent( QKeyEvent *event);
	void timerEvent(QTimerEvent *event);
	void popupMenu(QMouseEvent *event);
	void popupCabinetMenu(QMouseEvent *event,CSsp3DSceneObj *m_sceneObj);
	void popupDeviceMenu(QMouseEvent *event,CSsp3DSceneObj *m_sceneObj);
	
private:
	bool bInit;
	GLfloat m_lightAmbient[4];
	GLfloat m_lightDiffuse[4];
	GLfloat m_lightSpecular[4];
	GLfloat m_lightPositon[4];
	GLuint SelBuff[BUFFER_LENGTH];

	CSsp3DModel *m_3DModel;
	int	m_sceneId;

	stuEyeParam pEye;
	stuSceneParam pScene;
	Point2fT m_mousePt;
	bool bLeftButtonClicked;
	bool bLeftButtonMoved;
	bool bRightButtonClicked;
	bool bRightButtonMoved;

	QMenu *menuScene;
	QAction *refeshAction;
	QAction *parentAction;
	QAction *equalAction;
	QAction *childAction;
	QAction *fpsAction;
	QAction *verAction;
	QAction *eyeVPAction;
	QAction *selVPAction;
	QAction *savVPAction;
	QAction *delVPAction;
	QAction *firstVPAction;
	QAction *gridAction;
	QAction *increaLightAction;
	QAction *reduceLightAction;

	QMenu *menuCabinet;
	QMenu *menuDevice;
	QAction *cabTookAction;
	QAction *cabDrawAction;
	QAction *devTookAction;
	QAction *devBookAction;
	QAction *devLinkAction;
	QAction *devDrawAction;
	QAction *devInfoAction;
	QAction *devBugAction;
	QAction *devRealState;
	QAction *devScoreState;

	QMutex paintMutex;
	LoadDbThread *m_loadDbThread;

	int	progressMaxValue;
	QProgressDialog	*progressDialog;

	CSsp3DSceneObj *m_timeSceneObj;
	unsigned int times;
	eSSPRotate m_rotate;

	GLUquadricObj *m_quadric;
	bool bPaintTransparence;
	CSsp3DWidget *ssp3Dwidget;

	int tookSceneId;
	CSsp3DSceneObj *m_cabSceneObj;
	CSsp3DSceneObj *m_oeSceneObj;
	QTimer *hintTimer;
	bool bFirstPersn;
	bool bDrawGrid;

	CSsp3DVrml *vrml;
	GLuint m_skyboxTexture;

	bool vboSupported;

	int m_iTimeCount;

private:
	void init();
	void initSlots();
	bool loadSkyboxTexture();

	void paintLookAt(int sceneId);
	void paintScene(int sceneId);
	void paintChildScene(CSsp3DScene *childScene);
	void paintSceneObj(CSsp3DSceneObj *sceneObj);
	void paintPreDefine(int predefineId,QString color=QString::null);
	void paintPreDefineObj(CSsp3DPreDefineObj *predefineObj,QString color=QString::null);
	void paintVrml(int vrmlId);

	void paintLine(t_ssp_3d_scene_object *obj);
	void paintRectangle(t_ssp_3d_scene_object *obj);
	void paintTriangle(t_ssp_3d_scene_object *obj);
	void paintRound(t_ssp_3d_scene_object *obj);
	void paintCharacter(t_ssp_3d_scene_object *obj);
	void paintCylinder(t_ssp_3d_scene_object *obj);
	void paintConoid(t_ssp_3d_scene_object *obj);
	void paintSphere(t_ssp_3d_scene_object *obj);
	void paintLEDRectangle(t_ssp_3d_scene_object *obj,eSSP_LED_STATE ledState);
	void paintLEDRound(t_ssp_3d_scene_object *obj,eSSP_LED_STATE ledState);

	void paintLine(t_ssp_3d_predefine_obj *obj);
	void paintRectangle(t_ssp_3d_predefine_obj *obj);
	void paintTriangle(t_ssp_3d_predefine_obj *obj);
	void paintRound(t_ssp_3d_predefine_obj *obj);
	void paintCharacter(t_ssp_3d_predefine_obj *obj);
	void paintCylinder(t_ssp_3d_predefine_obj *obj);
	void paintConoid(t_ssp_3d_predefine_obj *obj);
	void paintSphere(t_ssp_3d_predefine_obj *obj,QString color);

	void updateEyeXYZByRotatf();
	void updateEyeCentXYZByTransR(float transfDistance);
	void updateEyeCentXYZByTransL(float transfDistance);
	void updateEyeCentXYZByTransU(float transfDistance);
	void updateEyeCentXYZByTransD(float transfDistance);
	void updateEye2Center();
	void updateXAngle();
	void updateZAngle();
	void updateCentXYZ();

	GLuint onSelection(int xPos, int yPos);
	void drawGrid();
	void drawCoordinateZero();
	void drawSkybox();
	void spotLight(GLfloat ambient[4],
				   GLfloat diffuse[4],
				   GLfloat specular[4],
				   GLfloat postion[4],
				   GLfloat direct[3],
				   GLfloat spotCutoff = 45.0,
				   GLfloat spotExponent = 0.0);

	bool isExtensionSupported(char* szTargetExtension);

private:
	inline GLuint getSelectName(CSsp3DSceneObj *obj) {  return obj->m_record.scene_id * 0x10000 + obj->m_record.obj_id; }
	inline void getIdBySelectName(GLuint sName,int &scene_id,int &obj_id) { scene_id = (sName / 0x10000) & 0xFFFF; obj_id = sName & 0xFFFF;}

signals:
	void sigProgressValue(int);

public slots:
	void slotReloadScene();
	void slotParentScene();
	void slotEqualScene();
	void slotChildScene();
	void slotTookScene();
	void slotCabDraw();
	void slotFps();
	void slotVersion();
	void slotEyeViewPoint();
	void slotSelectViewPoint();
	void slotSaveViewPoint();
	void slotDelViewPoint();
	void slotLoadDbThreadFinished();
	void onProgressValue(int value);
	void slotDeviceTook();
	void slotDeviceBook();
	void slotDeviceInfo();
	void slotDeviceLink();
	void slotDeviceDraw();
	void slotDeviceBug();
	void slotDeviceRealState();
	void slotDeviceScoreState();
	void slotHintTimerOut();
	void slotIsFirstPerson();
	void slotIsDrawGrid();
	void slotIncreaseLight();
	void slotReduceLight();
};


#endif // SSP_3D_WIDGET_H
