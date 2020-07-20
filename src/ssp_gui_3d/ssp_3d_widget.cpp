/**
*
* 文 件 名 : ssp_3d_widget.cpp
* 创建日期 : 2017-3-6 19:31
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

#include "ssp_3d_widget.h"
#include "dlgselectscene.h"
#include "dlgselectviewpoint.h"
#include "dlgsaveviewpoint.h"
#include "dlgdelviewpoint.h"

#define SSP_ZFAR	1000000.0

//--------------------class CSsp3DWidget----------------------
CSsp3DWidget::CSsp3DWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	QGLFormat format = QGLFormat::defaultFormat();
	format.setSampleBuffers(true);
	m_p3DPannel = new CSsp3DPannel(format,this,initSceneId());
	m_p3DPannel->setFocus();

	m_pGridLayout = new QGridLayout(ui.wndPannel);
	m_pGridLayout->setSpacing(0);
	m_pGridLayout->setContentsMargins(0, 0, 0, 0);
	m_pGridLayout->setObjectName(QString::fromUtf8("gridLayout"));
	m_pGridLayout->addWidget(m_p3DPannel);
}

CSsp3DWidget::~CSsp3DWidget()
{
	m_p3DPannel->stopTimer();
}

int CSsp3DWidget::initSceneId()
{
	SString sql;
	SRecordset rs;

	sql.sprintf("select scene_id from t_ssp_3d_scene where p_scene_id=0 order by scene_id asc");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt)
		return rs.GetValue(0,0).toInt();

	return 0;
}

//--------------------class CSsp3DPannel----------------------
CSsp3DPannel::CSsp3DPannel(QGLFormat format,QWidget *parent,int id)
	: QGLWidget(format, parent),m_sceneId(id)
{
	ssp3Dwidget = (CSsp3DWidget *)parent;
	hintTimer = NULL;
	init();
	initSlots();
}

CSsp3DPannel::~CSsp3DPannel()
{
	hintTimer->stop();
	delete hintTimer;
	hintTimer = NULL;
	delete progressDialog;
	delete m_3DModel;
	delete menuScene;
	delete menuCabinet;
	delete menuDevice;
	delete m_loadDbThread;
	//delete vrml;
	glDeleteTextures(1,&m_skyboxTexture);
}

void CSsp3DPannel::init()
{
	bInit = false;
	bLeftButtonClicked = false;
	bLeftButtonMoved = false;
	bRightButtonClicked = false;
	bRightButtonMoved = false;

	m_3DModel = new CSsp3DModel;
	menuScene = new QMenu;
	menuCabinet = new QMenu;
	menuDevice = new QMenu;

	progressMaxValue = 0;
	progressDialog = new QProgressDialog();
	progressDialog->setModal(true);
	progressDialog->setCancelButton(NULL);

	m_loadDbThread = new LoadDbThread(NULL);
	m_loadDbThread->set3DPannel(this);
	setMouseTracking(true);

	m_timeSceneObj = NULL;
	times = 0;
	m_rotate = SSP_ROTATE_MOD_NULL;
	startTimer(100);

	bPaintTransparence = false;
	tookSceneId = 0;
	hintTimer = new QTimer();
	hintTimer->setInterval(1000);
	hintTimer->start();

	bFirstPersn = false;
	bDrawGrid = false;

	SString sPath = SBASE_SAPP->GetHomePath()+"textures";
	QDir dir = QDir(sPath.data());
	if (!dir.exists())
		bool b = dir.mkdir(sPath.data());

	sPath = SBASE_SAPP->GetHomePath()+"vrmls";
	dir = QDir(sPath.data());
	if (!dir.exists())
		bool b = dir.mkdir(sPath.data());

	m_skyboxTexture = 0;
	vboSupported = false;
	m_iTimeCount = 0;
}

void CSsp3DPannel::initSlots()
{
	connect(m_loadDbThread,SIGNAL(finished()),this,SLOT(slotLoadDbThreadFinished()));
	connect(this,SIGNAL(sigProgressValue(int)),this,SLOT(onProgressValue(int)));
	connect(hintTimer,SIGNAL(timeout()),this,SLOT(slotHintTimerOut()));
}

void CSsp3DPannel::slotLoadDbThreadFinished()
{
	paintMutex.lock();
	m_3DModel->LoadByDb(DB,false); 
	bInit = false;
	paintMutex.unlock();
	updateGL();
}

void CSsp3DPannel::onProgressValue(int value)
{
	progressDialog->setValue(value);
}

void CSsp3DPannel::resizeGL(int width, int height)
{
	if (height == 0)
		height = 1;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,1.0f,SSP_ZFAR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CSsp3DPannel::initializeGL()
{
	GLenum err = glewInit();  
	if (GLEW_OK != err)  
		QMessageBox::warning(NULL,tr("告警"),tr("glewInit初始化失败:%1。").arg(err));

	if (isExtensionSupported("GL_ARB_vertex_buffer_object"))
	{
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
		glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
		glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
		glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
		glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");

		if(glGenBuffersARB && glBindBufferARB && glBufferDataARB && glBufferSubDataARB &&
			glMapBufferARB && glUnmapBufferARB && glDeleteBuffersARB && glGetBufferParameterivARB)
		{
			vboSupported = true;
			LOGDEBUG("Video card supports GL_ARB_vertex_buffer_object.");
		}
		else
		{
			vboSupported = false;
			LOGWARN("Video card does NOT support GL_ARB_vertex_buffer_object.");
		}
	}

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);  
	//glAlphaFunc(GL_GREATER, 0.0);
	
	glEnable(GL_LIGHTING);
	m_lightAmbient[0] = 0.8; m_lightAmbient[1] = 0.8; m_lightAmbient[2] = 0.8; m_lightAmbient[3] = 1.0;
	m_lightDiffuse[0] = 1.0; m_lightDiffuse[1] = 1.0; m_lightDiffuse[2] = 1.0; m_lightDiffuse[3] = 1.0;
	m_lightSpecular[0] = 1.0; m_lightSpecular[1] = 1.0; m_lightSpecular[2] = 1.0; m_lightSpecular[3] = 1.0;
	m_lightPositon[0] = 0.0; m_lightPositon[1] = 0.0; m_lightPositon[2] = 0.0; m_lightPositon[3] = 1.0; 
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,m_lightAmbient);
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,m_lightDiffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,m_lightSpecular);
	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,m_lightAmbient);
	glMaterialfv(GL_FRONT,GL_SPECULAR,m_lightSpecular);
	glMateriali(GL_FRONT,GL_SHININESS,128);
	
	m_3DModel->SetGlWidget(this);
	m_3DModel->LoadByDb(DB,false,vboSupported);

	m_quadric = gluNewQuadric();
	gluQuadricNormals(m_quadric, GL_SMOOTH);
	gluQuadricTexture(m_quadric, GL_TRUE);

	if (!loadSkyboxTexture())
		m_skyboxTexture = 0;

	//vrml = new CSsp3DVrml();
	//vrml->vboSupported = vboSupported;
	//vrml->LoadFile("D:\\Temps\\vrml\\鼓楼公园.WRL");
	//vrml->LoadFile("D:\\Temps\\vrml\\线架.WRL");
	//vrml->LoadFile("D:\\Temps\\vrml\\变压器.WRL");
	//vrml->LoadFile("D:\\Temps\\vrml\\变压器样式3\\变压器样式3.WRL");
	//vrml->LoadFile("D:\\Temps\\vrml\\变压器样式1\\变压器样式1.WRL");
	//vrml->LoadFile("D:\\Temps\\vrml\\变压器样式2\\变压器样式2.WRL");
	//vrml->LoadFile("D:\\Temps\\vrml\\联想台式机\\联想台式机.WRL");
	//vrml->LoadFile("D:\\SoftwarePlatform\\trunk\\SSP01\\out\\vrmls\\端庄变\\端庄变.WRL");
	//vrml->LoadFile("D:\\Temps\\vrml\\500kV聊城\\500kV聊城.WRL");
	//vrml->LoadFile("D:\\Temps\\vrml\\城市\\城市.WRL");
	//vrml->LoadFile("D:\\Temps\\vrml\\汽车1\\汽车1.WRL");
	//vrml->LoadFile("D:\\Temps\\vrml\\变电站部件\\变电站部件.WRL");
	//vrml->LoadFile("D:\\Temps\\vrml\\笔记本电脑\\笔记本电脑.WRL");
	//vrml->LoadFile("D:\\新建文件夹 (2)\\1.WRL");
}

bool CSsp3DPannel::loadSkyboxTexture()
{
	QImage image(":/ssp_gui_3d/skybox");
	if (!image.width())
		return false;

	image = image.convertToFormat(QImage::Format_RGB888);
	image = image.mirrored();

	glGenTextures(1, &m_skyboxTexture);
	glBindTexture(GL_TEXTURE_2D, m_skyboxTexture);      
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.bits());

	return true;
}

void CSsp3DPannel::updateEyeXYZByRotatf()
{
	pEye.eyeY = sin(PI / 180 * pEye.xAngle) * pEye.eye2center;
	GLdouble xz = cos(PI / 180 * pEye.xAngle) * pEye.eye2center;
	pEye.eyeX = sin(PI / 180 * pEye.zAngle) * xz + pEye.centerX;
	pEye.eyeZ = cos(PI / 180 * pEye.zAngle) * xz + pEye.centerZ;
}

void CSsp3DPannel::updateEyeCentXYZByTransR(float transfDistance)
{
	float angle = 0.0;
	if (pEye.zAngle >= 0.0 && pEye.zAngle < 90.0)
	{
		angle = pEye.zAngle;
		GLdouble xTransf = cos(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = sin(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
		else
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
	}
	else if (pEye.zAngle >= 90.0 && pEye.zAngle <= 180.0)
	{
		angle = pEye.zAngle - 90.0;
		GLdouble xTransf = sin(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = cos(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
		else
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
	}
	else if (pEye.zAngle >= -180.0 && pEye.zAngle <= -90.0)
	{
		angle = pEye.zAngle + 180.0;
		GLdouble xTransf = cos(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = sin(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
		else
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
	}
	else if (pEye.zAngle > -90.0 && pEye.zAngle < 0.0)
	{
		angle = pEye.zAngle + 90.0;
		GLdouble xTransf = sin(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = cos(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
		else
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
	}
}

void CSsp3DPannel::updateEyeCentXYZByTransL(float transfDistance)
{
	float angle = 0.0;
	if (pEye.zAngle >= 0.0 && pEye.zAngle < 90.0)
	{
		angle = pEye.zAngle;
		GLdouble xTransf = cos(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = sin(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
		else
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
	}
	else if (pEye.zAngle >= 90.0 && pEye.zAngle <= 180.0)
	{
		angle = pEye.zAngle - 90.0;
		GLdouble xTransf = sin(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = cos(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
		else
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
	}
	else if (pEye.zAngle >= -180.0 && pEye.zAngle <= -90.0)
	{
		angle = pEye.zAngle + 180.0;
		GLdouble xTransf = cos(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = sin(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
		else
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
	}
	else if (pEye.zAngle > -90.0 && pEye.zAngle < 0.0)
	{
		angle = pEye.zAngle + 90.0;
		GLdouble xTransf = sin(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = cos(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
		else
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
	}
}

void  CSsp3DPannel::updateEyeCentXYZByTransU(float transfDistance)
{
	float angle = 0.0;
	if (pEye.zAngle >= 0.0 && pEye.zAngle < 90.0)
	{
		angle = pEye.zAngle;
		GLdouble xTransf = sin(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = cos(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
		else
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
	}
	else if (pEye.zAngle >= 90.0 && pEye.zAngle <= 180.0)
	{
		angle = pEye.zAngle - 90.0;
		GLdouble xTransf = cos(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = sin(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
		else
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
	}
	else if (pEye.zAngle >= -180.0 && pEye.zAngle <= -90.0)
	{
		angle = pEye.zAngle + 180.0;
		GLdouble xTransf = sin(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = cos(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
		else
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
	}
	else if (pEye.zAngle > -90.0 && pEye.zAngle < 0.0)
	{
		angle = pEye.zAngle + 90.0;
		GLdouble xTransf = cos(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = sin(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
		else
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
	}
}

void  CSsp3DPannel::updateEyeCentXYZByTransD(float transfDistance)
{
	float angle = 0.0;
	if (pEye.zAngle >= 0.0 && pEye.zAngle < 90.0)
	{
		angle = pEye.zAngle;
		GLdouble xTransf = sin(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = cos(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
		else
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
	}
	else if (pEye.zAngle >= 90.0 && pEye.zAngle <= 180.0)
	{
		angle = pEye.zAngle - 90.0;
		GLdouble xTransf = cos(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = sin(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
		else
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
	}
	else if (pEye.zAngle >= -180.0 && pEye.zAngle <= -90.0)
	{
		angle = pEye.zAngle + 180.0;
		GLdouble xTransf = sin(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = cos(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
		else
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ -= zTransf;
			pEye.centerZ -= zTransf;
		}
	}
	else if (pEye.zAngle > -90.0 && pEye.zAngle < 0.0)
	{
		angle = pEye.zAngle + 90.0;
		GLdouble xTransf = cos(PI / 180 * angle) * transfDistance;
		GLdouble zTransf = sin(PI / 180 * angle) * transfDistance;
		if (!bFirstPersn)
		{
			pEye.eyeX -= xTransf;
			pEye.centerX -= xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
		else
		{
			pEye.eyeX += xTransf;
			pEye.centerX += xTransf;
			pEye.eyeZ += zTransf;
			pEye.centerZ += zTransf;
		}
	}
}

void CSsp3DPannel::updateEye2Center()
{
	GLdouble xz = sqrt(pow(pEye.eyeX-pEye.centerX,2)+pow(pEye.eyeZ-pEye.centerZ,2));
	pEye.eye2center = sqrt(pow(xz,2)+pow(pEye.eyeY,2));
}

void CSsp3DPannel::updateXAngle()
{
	pEye.xAngle = asin((pEye.eyeY-pEye.centerZ) / pEye.eye2center) * 180 / PI;
}

void CSsp3DPannel::updateZAngle()
{
	//pEye.zAngle = atan((pEye.eyeX-pEye.centerX) / pEye.eyeZ) * 180 / PI;
	if (pEye.eyeZ == pEye.centerZ)
	{
		if (pEye.eyeX > pEye.centerX)
			pEye.zAngle = -90;
		else if (pEye.eyeX < pEye.centerX)
			pEye.zAngle = 90;
		else if (pEye.eyeX == pEye.centerX)
			pEye.zAngle = 0;
	}
	else if (pEye.eyeZ > pEye.centerZ)
	{
		if (pEye.eyeX > pEye.centerX)
			pEye.zAngle = 0 - atan(fabs(pEye.eyeX -pEye.centerX) / fabs(pEye.eyeZ-pEye.centerZ)) * 180 / PI;
		else if (pEye.eyeX < pEye.centerX)
			pEye.zAngle = atan(fabs(pEye.eyeX -pEye.centerX) / fabs(pEye.eyeZ-pEye.centerZ)) * 180 / PI;
		else if (pEye.eyeX == pEye.centerX)
			pEye.zAngle = 0;
	}
	else if (pEye.eyeZ < pEye.centerZ)
	{
		if (pEye.eyeX > pEye.centerX)
			pEye.zAngle = -90 - atan(fabs(pEye.eyeZ-pEye.centerZ) / fabs(pEye.eyeX -pEye.centerX)) * 180 / PI;
		else if (pEye.eyeX < pEye.centerX)
			pEye.zAngle = 90 + atan(fabs(pEye.eyeZ-pEye.centerZ) / fabs(pEye.eyeX -pEye.centerX)) * 180 / PI;
		else if (pEye.eyeX == pEye.centerX)
			pEye.zAngle = -180;
	}
}

void CSsp3DPannel::updateCentXYZ()
{
	//抬低头,上下各60度
	GLdouble y = sin(PI / 180 * pEye.xAngle) * pEye.eye2center;
	pEye.centerY = pEye.eyeY + y;
	GLdouble z = cos(PI / 180 * pEye.xAngle) * pEye.eye2center;
	z = fabs(z);
	if (pEye.centerZ < pEye.eyeZ)
		pEye.centerZ = pEye.eyeZ - z;
	else
		pEye.centerZ = pEye.eyeZ + z;

	//左右转头，360度
	float angle = 0.0;
	if (pEye.zAngle >= 0.0 && pEye.zAngle <= 90.0)
	{
		angle = pEye.zAngle;
		GLdouble xTransf = sin(PI / 180 * angle) * z;
		GLdouble zTransf = cos(PI / 180 * angle) * z;
		pEye.centerX = pEye.eyeX + xTransf;
		pEye.centerZ = pEye.eyeZ - zTransf;
	}
	else if (pEye.zAngle > 90.0 && pEye.zAngle <= 180.0)
	{
		angle = pEye.zAngle - 90.0;
		GLdouble xTransf = cos(PI / 180 * angle) * z;
		GLdouble zTransf = sin(PI / 180 * angle) * z;
		pEye.centerX = pEye.eyeX + xTransf;
		pEye.centerZ = pEye.eyeZ + zTransf;
	}
	else if (pEye.zAngle > -180.0 && pEye.zAngle <= -90.0)
	{
		angle = pEye.zAngle + 180.0;
		GLdouble xTransf = sin(PI / 180 * angle) * z;
		GLdouble zTransf = cos(PI / 180 * angle) * z;
		pEye.centerX = pEye.eyeX - xTransf;
		pEye.centerZ = pEye.eyeZ + zTransf;
	}
	else if (pEye.zAngle > -90.0 && pEye.zAngle < 0.0)
	{
		angle = pEye.zAngle + 90.0;
		GLdouble xTransf = cos(PI / 180 * angle) * z;
		GLdouble zTransf = sin(PI / 180 * angle) * z;
		pEye.centerX = pEye.eyeX - xTransf;
		pEye.centerZ = pEye.eyeZ - zTransf;
	}
}
  
void CSsp3DPannel::paintGL()
{
	paintMutex.lock();
	glEnable(GL_MULTISAMPLE_ARB);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (m_sceneId)
	{
		paintLookAt(m_sceneId);
		drawSkybox();

		bPaintTransparence = false;
		paintScene(m_sceneId);
		bPaintTransparence = true;
		paintScene(m_sceneId);
	}

	//vrml->Draw();

#if 0
	GLfloat lightAmbient[4] = {1,1,1,1};
	GLfloat lightDiffuse[4] = {1,0,0,1};
	GLfloat lightSpecular[4] = {1,0,0,1};
	GLfloat lightPostion[4] = {0,500,0,1};
	GLfloat lightDirect[3] = {0,-1,0};
	spotLight(lightAmbient,lightDiffuse,lightSpecular,lightPostion,lightDirect);
#endif

	if (bDrawGrid)
	{
		//drawGrid();
		drawCoordinateZero();
	}
	glDisable(GL_MULTISAMPLE_ARB);
	paintMutex.unlock();
}

void CSsp3DPannel::paintLookAt(int sceneId)
{
	CSsp3DScene *m_scene = m_3DModel->SearchScene(sceneId);
	if (!m_scene)
		return;

	t_ssp_3d_scene_viewpoint *m_sceneDefVP = NULL;
	CSsp3DSceneViewPoint *m_sceneVP = m_3DModel->SearchSceneVP(m_scene->m_record.scene_id);
	if (m_sceneVP)
	{
		m_sceneDefVP = m_sceneVP->SearchDefViewPoint();
		if (!m_sceneDefVP)
			return;
	}
	else
		return;

	if (!bInit)
	{
		pScene.transfX = m_scene->m_record.center_x;
		pScene.transfY = m_scene->m_record.center_y;
		pScene.transfZ = m_scene->m_record.center_z;
		pScene.rotatfX = m_scene->m_record.rotate_x;
		pScene.rotatfY = m_scene->m_record.rotate_y;
		pScene.rotatfZ = m_scene->m_record.rotate_z;

		if (bFirstPersn)
		{
			pEye.eyeX = m_sceneDefVP->eye_x;
			pEye.eyeY = 170.0;
			pEye.eyeZ = m_sceneDefVP->eye_z;
			pEye.centerX = m_sceneDefVP->eye_forw_x;
			pEye.centerY = 170.0;
			pEye.centerZ = m_sceneDefVP->eye_forw_z;
			pEye.upY = 1.0;
			pEye.eye2center = fabs(pEye.eyeZ - pEye.centerZ);
			pEye.xAngle = 0.0;
			//pEye.zAngle = 0.0;
			 updateZAngle();
		}
		else
		{
			pEye.eyeX = m_sceneDefVP->eye_x;
			pEye.eyeY = m_sceneDefVP->eye_y;
			pEye.eyeZ = m_sceneDefVP->eye_z;
			pEye.centerX = m_sceneDefVP->eye_forw_x;
			pEye.centerY = m_sceneDefVP->eye_forw_y;
			pEye.centerZ = m_sceneDefVP->eye_forw_z;
			pEye.upY = 1.0;
			//updateEye2Center();
			//updateXAngle();
			//updateZAngle();
			pEye.eye2center = m_sceneDefVP->cent2eye;
			if (pEye.eye2center == 0.0)
				updateEye2Center();
			pEye.xAngle = m_sceneDefVP->x_angle;
			pEye.zAngle = m_sceneDefVP->z_angle;
			if (pEye.eye2center == 0.0)
				updateZAngle();
		}
		
		bInit = true;
	}

	gluLookAt(pEye.eyeX,pEye.eyeY,pEye.eyeZ,pEye.centerX,pEye.centerY,pEye.centerZ,pEye.upX,pEye.upY,pEye.upZ);
}

void CSsp3DPannel::paintScene(int sceneId)
{
	CSsp3DScene *m_scene = m_3DModel->SearchScene(sceneId);
	if (!m_scene)
		return;

	unsigned long pos;
	CSsp3DSceneObj *obj = m_scene->m_Objs.FetchFirst(pos);
	while(obj)
	{
		paintSceneObj(obj);
		obj = m_scene->m_Objs.FetchNext(pos);
	}

	if (m_scene->m_record.scene_type == 4) //如果场景类型为4，则不画此场景下所有子场景
		return;

	m_scene = m_3DModel->m_Scenes.FetchFirst(pos);
	while(m_scene)
	{
		if (m_scene->m_record.p_scene_id == sceneId)
		{
			paintChildScene(m_scene);
		}
		m_scene = m_3DModel->m_Scenes.FetchNext(pos);
	}
}

void CSsp3DPannel::paintChildScene(CSsp3DScene *childScene)
{
	glPushMatrix();
	glTranslatef(childScene->m_record.center_x,childScene->m_record.center_y,childScene->m_record.center_z);
	glRotatef(childScene->m_record.rotate_x,1.0f,0.0f,0.0f);
	glRotatef(childScene->m_record.rotate_y,0.0f,1.0f,0.0f);
	glRotatef(childScene->m_record.rotate_z,0.0f,0.0f,1.0f);
	glScaled(childScene->m_record.scale,childScene->m_record.scale,childScene->m_record.scale);

	unsigned long pos;
	CSsp3DSceneObj *obj = childScene->m_Objs.FetchFirst(pos);
	while(obj)
	{
		paintSceneObj(obj);
		obj = childScene->m_Objs.FetchNext(pos);
	}

	//glTranslatef(0.0-childScene->m_record.center_x,0.0-childScene->m_record.center_y,0.0-childScene->m_record.center_z);
	//glRotatef(0.0-childScene->m_record.rotate_x,1.0f,0.0f,0.0f);
	//glRotatef(0.0-childScene->m_record.rotate_y,0.0f,1.0f,0.0f);
	//glRotatef(0.0-childScene->m_record.rotate_z,0.0f,0.0f,1.0f);
	//glPopMatrix();

	CSsp3DScene *m_scene = m_3DModel->m_Scenes.FetchFirst(pos);
	while(m_scene)
	{
		if (m_scene->m_record.p_scene_id == childScene->m_record.scene_id)
		{
			paintChildScene(m_scene);
		}
		m_scene = m_3DModel->m_Scenes.FetchNext(pos);
	}

	glTranslatef(0.0-childScene->m_record.center_x,0.0-childScene->m_record.center_y,0.0-childScene->m_record.center_z);
	glRotatef(0.0-childScene->m_record.rotate_x,1.0f,0.0f,0.0f);
	glRotatef(0.0-childScene->m_record.rotate_y,0.0f,1.0f,0.0f);
	glRotatef(0.0-childScene->m_record.rotate_z,0.0f,0.0f,1.0f);
	glPopMatrix();
}

void CSsp3DPannel::paintSceneObj(CSsp3DSceneObj *sceneObj)
{
	glPushMatrix();
	glTranslatef(sceneObj->m_record.center_x,sceneObj->m_record.center_y,sceneObj->m_record.center_z);
	glRotatef(sceneObj->m_record.rotate_x,1.0f,0.0f,0.0f);
	glRotatef(sceneObj->m_record.rotate_y,0.0f,1.0f,0.0f);
	glRotatef(sceneObj->m_record.rotate_z,0.0f,0.0f,1.0f);
	glScaled(sceneObj->m_record.scale,sceneObj->m_record.scale,sceneObj->m_record.scale);

	switch (sceneObj->m_record.draw_type)
	{
	case 1: //直线
		paintLine(&sceneObj->m_record);
		break;
	case 2: //矩形
		paintRectangle(&sceneObj->m_record);
		break;
	case 3: //三角形
		paintTriangle(&sceneObj->m_record);
		break;
	case 4: //圆形
		paintRound(&sceneObj->m_record);
		break;
	case 5: //文字
		paintCharacter(&sceneObj->m_record);
		break;
	case 6: //长方体
		break;
	case 7: //圆柱体
		paintCylinder(&sceneObj->m_record);
		break;
	case 8: //圆锥体
		paintConoid(&sceneObj->m_record);
		break;
	case 9: //球体
		paintSphere(&sceneObj->m_record);
		break;
	case 10: //圆形灯
		{
			eSSP_LED_STATE e = ssp3Dwidget->OnStateById(sceneObj->m_record.id1,sceneObj->m_record.id2,sceneObj->m_record.id3,sceneObj->m_record.id4);
			paintLEDRound(&sceneObj->m_record,e);
		}
		break;
	case 11: //矩形灯
		{
			eSSP_LED_STATE e = ssp3Dwidget->OnStateById(sceneObj->m_record.id1,sceneObj->m_record.id2,sceneObj->m_record.id3,sceneObj->m_record.id4);
			paintLEDRectangle(&sceneObj->m_record,e);
		}
		break;
	case 100: //VRML对象
		paintVrml(sceneObj->m_record.predefine_id);
		break;
	case 101: //预定义对象
		if (sceneObj->m_record.predefine_id)
		{
			GLuint name = getSelectName(sceneObj);
			glLoadName(name);
			paintPreDefine(sceneObj->m_record.predefine_id);
		}
		break;
	case 200:
		{
			GLuint name = getSelectName(sceneObj);
			glLoadName(name);
			//QString color = ssp3Dwidget->OnCabinetAlarm(sceneObj->m_record.id1,sceneObj->m_record.id2,sceneObj->m_record.id3,sceneObj->m_record.id4);
			if (!sceneObj->color.isEmpty() && sceneObj->m_record.predefine_id)
				paintPreDefine(sceneObj->m_record.predefine_id,sceneObj->color.data());
		}
		break;
	default:
		break;
	}

	glTranslatef(0.0-sceneObj->m_record.center_x,0.0-sceneObj->m_record.center_y,0.0-sceneObj->m_record.center_z);
	glRotatef(0.0-sceneObj->m_record.rotate_x,1.0f,0.0f,0.0f);
	glRotatef(0.0-sceneObj->m_record.rotate_y,0.0f,1.0f,0.0f);
	glRotatef(0.0-sceneObj->m_record.rotate_z,0.0f,0.0f,1.0f);
	glPopMatrix(); 
}

void CSsp3DPannel::paintPreDefine(int predefineId,QString color)
{
	CSsp3DPreDefine *m_preDefine = m_3DModel->SearchPreDefine(predefineId);
	if (!m_preDefine)
		return;

	unsigned long pos;
	CSsp3DPreDefineObj *obj = m_preDefine->m_PreObjs.FetchFirst(pos);
	while(obj)
	{
		paintPreDefineObj(obj,color);
		obj = m_preDefine->m_PreObjs.FetchNext(pos);
	}
}

void CSsp3DPannel::paintVrml(int vrmlId)
{
	if (bPaintTransparence)
		return;

	CSsp3DVrml *m_vrml = m_3DModel->SearchVrml(vrmlId);
	if (!m_vrml)
		return;

	m_vrml->Draw();
}

void CSsp3DPannel::paintPreDefineObj(CSsp3DPreDefineObj *predefineObj,QString color)
{
	glPushMatrix();
	glTranslatef(predefineObj->m_record.center_x,predefineObj->m_record.center_y,predefineObj->m_record.center_z);
	glRotatef(predefineObj->m_record.rotate_x,1.0f,0.0f,0.0f);
	glRotatef(predefineObj->m_record.rotate_y,0.0f,1.0f,0.0f);
	glRotatef(predefineObj->m_record.rotate_z,0.0f,0.0f,1.0f);
	glScaled(predefineObj->m_record.scale,predefineObj->m_record.scale,predefineObj->m_record.scale);

	switch (predefineObj->m_record.draw_type)
	{
	case 1: //直线
		paintLine(&predefineObj->m_record);
		break;
	case 2: //矩形
		paintRectangle(&predefineObj->m_record);
		break;
	case 3: //三角形
		paintTriangle(&predefineObj->m_record);
		break;
	case 4: //圆形
		paintRound(&predefineObj->m_record);
		break;
	case 5: //文字
		paintCharacter(&predefineObj->m_record);
		break;
	case 6: //长方体
		break;
	case 7: //圆柱体
		paintCylinder(&predefineObj->m_record);
		break;
	case 8: //圆锥体
		paintConoid(&predefineObj->m_record);
		break;
	case 9: //球体
		paintSphere(&predefineObj->m_record,color);
		break;
	case 100: //VRML对象
		paintVrml(predefineObj->m_record.predefine_id);
		break;
	case 101: //预定义对象
		if (predefineObj->m_record.predefine_id)
			paintPreDefine(predefineObj->m_record.predefine_id);
		break;
	default:
		break;
	}

	glTranslatef(0.0-predefineObj->m_record.center_x,0.0-predefineObj->m_record.center_y,0.0-predefineObj->m_record.center_z);
	glRotatef(0.0-predefineObj->m_record.rotate_x,1.0f,0.0f,0.0f);
	glRotatef(0.0-predefineObj->m_record.rotate_y,0.0f,1.0f,0.0f);
	glRotatef(0.0-predefineObj->m_record.rotate_z,0.0f,0.0f,1.0f);
	glPopMatrix(); 
}

//场景对象形状
void CSsp3DPannel::paintLine(t_ssp_3d_scene_object *obj)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);  
	glVertex3f(obj->vertex1_x,obj->vertex1_y,obj->vertex1_z);  
	glVertex3f(obj->vertex2_x,obj->vertex2_y,obj->vertex2_z);  
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void CSsp3DPannel::paintRectangle(t_ssp_3d_scene_object *obj)
{
	if (obj->texture_id)
	{
		CSsp3DTexture *tex = m_3DModel->SearchTexture(obj->texture_id);
		if (tex->m_Texture)
		{
			if (tex->m_record.tex_color[0] != 0x00)
				glEnable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, tex->m_Texture);
			glBegin(GL_QUADS);

			if (QString(tr("%1").arg(obj->obj_name)).contains(tr("前")))
				glNormal3f(0.0f,0.0f,1.0f);
			else if (QString(tr("%1").arg(obj->obj_name)).contains(tr("后")))
				glNormal3f(0.0f,0.0f,-1.0f);
			else if (QString(tr("%1").arg(obj->obj_name)).contains(tr("左")))
				glNormal3f(-1.0f,0.0f,0.0f);
			else if (QString(tr("%1").arg(obj->obj_name)).contains(tr("右")))
				glNormal3f(1.0f,0.0f,0.0f);
			else if (QString(tr("%1").arg(obj->obj_name)).contains(tr("上")))
				glNormal3f(0.0f,1.0f,0.0f);
			else if (QString(tr("%1").arg(obj->obj_name)).contains(tr("下")))
				glNormal3f(0.0f,-1.0f,0.0f);

			glTexCoord2f(obj->tex1_x,obj->tex1_y);
			glVertex3f(obj->vertex1_x,obj->vertex1_y,obj->vertex1_z);
			glTexCoord2f(obj->tex2_x,obj->tex2_y);
			glVertex3f(obj->vertex2_x,obj->vertex2_y,obj->vertex2_z);
			glTexCoord2f(obj->tex3_x,obj->tex3_y);
			glVertex3f(obj->vertex3_x,obj->vertex3_y,obj->vertex3_z);
			glTexCoord2f(obj->tex4_x,obj->tex4_y);
			glVertex3f(obj->vertex4_x,obj->vertex4_y,obj->vertex4_z);
			glEnd();

			if (tex->m_record.tex_color[0] != 0x00)
				glDisable(GL_BLEND);

			return;
		}
	}

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glVertex3f(obj->vertex1_x,obj->vertex1_y,obj->vertex1_z);
	glVertex3f(obj->vertex2_x,obj->vertex2_y,obj->vertex2_z);
	glVertex3f(obj->vertex3_x,obj->vertex3_y,obj->vertex3_z);
	glVertex3f(obj->vertex4_x,obj->vertex4_y,obj->vertex4_z);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void CSsp3DPannel::paintTriangle(t_ssp_3d_scene_object *obj)
{
	if (obj->texture_id)
	{
		CSsp3DTexture *tex = m_3DModel->SearchTexture(obj->texture_id);
		if (tex->m_Texture)
		{
			if (tex->m_record.tex_color[0] != 0x00)
				glEnable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, tex->m_Texture);
			glBegin(GL_TRIANGLES);
			glTexCoord2f(obj->tex1_x,obj->tex1_y);
			glVertex3f(obj->vertex1_x,obj->vertex1_y,obj->vertex1_z);
			glTexCoord2f(obj->tex2_x,obj->tex2_y);
			glVertex3f(obj->vertex2_x,obj->vertex2_y,obj->vertex2_z);
			glTexCoord2f(obj->tex3_x,obj->tex3_y);
			glVertex3f(obj->vertex3_x,obj->vertex3_y,obj->vertex3_z);
			glEnd();

			if (tex->m_record.tex_color[0] != 0x00)
				glDisable(GL_BLEND);

			return;
		}
	}

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLES);
	glVertex3f(obj->vertex1_x,obj->vertex1_y,obj->vertex1_z);
	glVertex3f(obj->vertex2_x,obj->vertex2_y,obj->vertex2_z);
	glVertex3f(obj->vertex3_x,obj->vertex3_y,obj->vertex3_z);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void CSsp3DPannel::paintRound(t_ssp_3d_scene_object *obj)
{
	if (obj->texture_id)
	{
		CSsp3DTexture *tex = m_3DModel->SearchTexture(obj->texture_id);
		if (tex->m_Texture)
		{
			if (tex->m_record.tex_color[0] != 0x00)
				glEnable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, tex->m_Texture);
			gluDisk(m_quadric,obj->vertex1_x,obj->vertex1_y,obj->vertex2_x,obj->vertex2_y);

			if (tex->m_record.tex_color[0] != 0x00)
				glDisable(GL_BLEND);
		}
	}
}

void CSsp3DPannel::paintCharacter(t_ssp_3d_scene_object *obj)
{
	glEnable(GL_BLEND);
	GLuint m_Texture = obj->texture_id;
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glBegin(GL_QUADS);
	glNormal3f(0.0f,0.0f,1.0f);
	glTexCoord2f(0,1);
	glVertex3f(obj->vertex1_x,obj->vertex1_y,obj->vertex1_z);
	glTexCoord2f(1,1);
	glVertex3f(obj->vertex2_x,obj->vertex2_y,obj->vertex2_z);
	glTexCoord2f(1,0);
	glVertex3f(obj->vertex3_x,obj->vertex3_y,obj->vertex3_z);
	glTexCoord2f(0,0);
	glVertex3f(obj->vertex4_x,obj->vertex4_y,obj->vertex4_z);
	glEnd();
	glDisable(GL_BLEND);
}

void CSsp3DPannel::paintCylinder(t_ssp_3d_scene_object *obj)
{
	if (obj->texture_id)
	{
		CSsp3DTexture *tex = m_3DModel->SearchTexture(obj->texture_id);
		if (tex->m_Texture)
		{
			if (tex->m_record.tex_color[0] != 0x00)
				glEnable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, tex->m_Texture);
			gluCylinder(m_quadric,obj->vertex1_x,obj->vertex1_y,obj->vertex1_z,obj->vertex2_x,obj->vertex2_y);

			if (tex->m_record.tex_color[0] != 0x00)
				glDisable(GL_BLEND);
		}
	}
}

void CSsp3DPannel::paintConoid(t_ssp_3d_scene_object *obj)
{
	if (obj->texture_id)
	{
		CSsp3DTexture *tex = m_3DModel->SearchTexture(obj->texture_id);
		if (tex->m_Texture)
		{
			if (tex->m_record.tex_color[0] != 0x00)
				glEnable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, tex->m_Texture);
			gluCylinder(m_quadric,obj->vertex1_x,0.0,obj->vertex1_y,obj->vertex2_x,obj->vertex2_y);

			if (tex->m_record.tex_color[0] != 0x00)
				glDisable(GL_BLEND);
		}
	}
}

void CSsp3DPannel::paintSphere(t_ssp_3d_scene_object *obj)
{
	if (obj->texture_id)
	{
		CSsp3DTexture *tex = m_3DModel->SearchTexture(obj->texture_id);
		if (tex->m_Texture)
		{
			if (tex->m_record.tex_color[0] != 0x00)
				glEnable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, tex->m_Texture);
			gluSphere(m_quadric,obj->vertex1_x,obj->vertex2_x,obj->vertex2_y);

			if (tex->m_record.tex_color[0] != 0x00)
				glDisable(GL_BLEND);
		}
	}
}

void CSsp3DPannel::paintLEDRectangle(t_ssp_3d_scene_object *obj,eSSP_LED_STATE ledState)
{
	int texture_id = 0;
	switch (ledState)
	{
	case SSP_LED_OFF:
		texture_id = obj->texture_off_id;
		break;
	case SSP_LED_ON:
		texture_id = obj->texture_on_id;
		break;
	case SSP_LED_DEF:
		texture_id = obj->texture_id;
		break;
	}

	if (texture_id)
	{
		CSsp3DTexture *tex = m_3DModel->SearchTexture(texture_id);
		if (tex && tex->m_Texture)
		{
			if (tex->m_record.tex_color[0] != 0x00)
				glEnable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, tex->m_Texture);
			glBegin(GL_QUADS);
			glTexCoord2f(obj->tex1_x,obj->tex1_y);
			glVertex3f(obj->vertex1_x,obj->vertex1_y,obj->vertex1_z);
			glTexCoord2f(obj->tex2_x,obj->tex2_y);
			glVertex3f(obj->vertex2_x,obj->vertex2_y,obj->vertex2_z);
			glTexCoord2f(obj->tex3_x,obj->tex3_y);
			glVertex3f(obj->vertex3_x,obj->vertex3_y,obj->vertex3_z);
			glTexCoord2f(obj->tex4_x,obj->tex4_y);
			glVertex3f(obj->vertex4_x,obj->vertex4_y,obj->vertex4_z);
			glEnd();

			if (tex->m_record.tex_color[0] != 0x00)
				glDisable(GL_BLEND);

			return;
		}
	}

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glVertex3f(obj->vertex1_x,obj->vertex1_y,obj->vertex1_z);
	glVertex3f(obj->vertex2_x,obj->vertex2_y,obj->vertex2_z);
	glVertex3f(obj->vertex3_x,obj->vertex3_y,obj->vertex3_z);
	glVertex3f(obj->vertex4_x,obj->vertex4_y,obj->vertex4_z);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void CSsp3DPannel::paintLEDRound(t_ssp_3d_scene_object *obj,eSSP_LED_STATE ledState)
{
	int texture_id = 0;
	switch (ledState)
	{
	case SSP_LED_OFF:
		texture_id = obj->texture_off_id;
		break;
	case SSP_LED_ON:
		texture_id = obj->texture_on_id;
		break;
	case SSP_LED_DEF:
		texture_id = obj->texture_id;
		break;
	}

	if (texture_id)
	{
		CSsp3DTexture *tex = m_3DModel->SearchTexture(texture_id);
		if (tex && tex->m_Texture)
		{
			if (tex->m_record.tex_color[0] != 0x00)
				glEnable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, tex->m_Texture);
			gluDisk(m_quadric,obj->vertex1_x,obj->vertex1_y,obj->vertex2_x,obj->vertex2_y);

			if (tex->m_record.tex_color[0] != 0x00)
				glDisable(GL_BLEND);

			return;
		}
	}

	glDisable(GL_TEXTURE_2D);
	gluDisk(m_quadric,obj->vertex1_x,obj->vertex1_y,obj->vertex2_x,obj->vertex2_y);
	glEnable(GL_TEXTURE_2D);
}

//预定义对象形状
void CSsp3DPannel::paintLine(t_ssp_3d_predefine_obj *obj)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);  
	glVertex3f(obj->vertex1_x,obj->vertex1_y,obj->vertex1_z);  
	glVertex3f(obj->vertex2_x,obj->vertex2_y,obj->vertex2_z);  
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void CSsp3DPannel::paintRectangle(t_ssp_3d_predefine_obj *obj)
{
	if (obj->texture_id)
	{
		CSsp3DTexture *tex = m_3DModel->SearchTexture(obj->texture_id);
		if (tex && tex->m_Texture)
		{
			if ((bPaintTransparence == false && tex->m_record.tex_color[0] == 0x00) ||
				(bPaintTransparence == true && tex->m_record.tex_color[0] != 0x00))
			{
				if (tex->m_record.tex_color[0] != 0x00)
					glEnable(GL_BLEND);

				glBindTexture(GL_TEXTURE_2D, tex->m_Texture);
				glBegin(GL_QUADS);

				if (QString(tr("%1").arg(obj->obj_name)).contains(tr("前")))
					glNormal3f(0.0f,0.0f,1.0f);
				else if (QString(tr("%1").arg(obj->obj_name)).contains(tr("后")))
					glNormal3f(0.0f,0.0f,-1.0f);
				else if (QString(tr("%1").arg(obj->obj_name)).contains(tr("左")))
					glNormal3f(-1.0f,0.0f,0.0f);
				else if (QString(tr("%1").arg(obj->obj_name)).contains(tr("右")))
					glNormal3f(1.0f,0.0f,0.0f);
				else if (QString(tr("%1").arg(obj->obj_name)).contains(tr("上")))
					glNormal3f(0.0f,1.0f,0.0f);
				else if (QString(tr("%1").arg(obj->obj_name)).contains(tr("下")))
					glNormal3f(0.0f,-1.0f,0.0f);

				glTexCoord2f(obj->tex1_x,obj->tex1_y);
				glVertex3f(obj->vertex1_x,obj->vertex1_y,obj->vertex1_z);
				glTexCoord2f(obj->tex2_x,obj->tex2_y);
				glVertex3f(obj->vertex2_x,obj->vertex2_y,obj->vertex2_z);
				glTexCoord2f(obj->tex3_x,obj->tex3_y);
				glVertex3f(obj->vertex3_x,obj->vertex3_y,obj->vertex3_z);
				glTexCoord2f(obj->tex4_x,obj->tex4_y);
				glVertex3f(obj->vertex4_x,obj->vertex4_y,obj->vertex4_z);
				glEnd();

				if (tex->m_record.tex_color[0] != 0x00)
					glDisable(GL_BLEND);
			}
		}
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glVertex3f(obj->vertex1_x,obj->vertex1_y,obj->vertex1_z);
		glVertex3f(obj->vertex2_x,obj->vertex2_y,obj->vertex2_z);
		glVertex3f(obj->vertex3_x,obj->vertex3_y,obj->vertex3_z);
		glVertex3f(obj->vertex4_x,obj->vertex4_y,obj->vertex4_z);
		glEnd();
		glEnable(GL_TEXTURE_2D);
	}
}

void CSsp3DPannel::paintTriangle(t_ssp_3d_predefine_obj *obj)
{
	if (obj->texture_id)
	{
		CSsp3DTexture *tex = m_3DModel->SearchTexture(obj->texture_id);
		if (tex->m_Texture)
		{
			if (tex->m_record.tex_color[0] != 0x00)
				glEnable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, tex->m_Texture);
			glBegin(GL_TRIANGLES);
			glTexCoord2f(obj->tex1_x,obj->tex1_y);
			glVertex3f(obj->vertex1_x,obj->vertex1_y,obj->vertex1_z);
			glTexCoord2f(obj->tex2_x,obj->tex2_y);
			glVertex3f(obj->vertex2_x,obj->vertex2_y,obj->vertex2_z);
			glTexCoord2f(obj->tex3_x,obj->tex3_y);
			glVertex3f(obj->vertex3_x,obj->vertex3_y,obj->vertex3_z);
			glEnd();

			if (tex->m_record.tex_color[0] != 0x00)
				glDisable(GL_BLEND);

			return;
		}
	}

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLES);
	glVertex3f(obj->vertex1_x,obj->vertex1_y,obj->vertex1_z);
	glVertex3f(obj->vertex2_x,obj->vertex2_y,obj->vertex2_z);
	glVertex3f(obj->vertex3_x,obj->vertex3_y,obj->vertex3_z);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void CSsp3DPannel::paintRound(t_ssp_3d_predefine_obj *obj)
{
	if (obj->texture_id)
	{
		CSsp3DTexture *tex = m_3DModel->SearchTexture(obj->texture_id);
		if (tex->m_Texture)
		{
			if (tex->m_record.tex_color[0] != 0x00)
				glEnable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, tex->m_Texture);
			gluDisk(m_quadric,obj->vertex1_x,obj->vertex1_y,obj->vertex2_x,obj->vertex2_y);

			if (tex->m_record.tex_color[0] != 0x00)
				glDisable(GL_BLEND);
		}
	}
}

void CSsp3DPannel::paintCharacter(t_ssp_3d_predefine_obj *obj)
{
	glEnable(GL_BLEND);
	GLuint m_Texture = obj->texture_id;
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glBegin(GL_QUADS);
	glNormal3f(0.0f,0.0f,1.0f);
	glTexCoord2f(0,1);
	glVertex3f(obj->vertex1_x,obj->vertex1_y,obj->vertex1_z);
	glTexCoord2f(1,1);
	glVertex3f(obj->vertex2_x,obj->vertex2_y,obj->vertex2_z);
	glTexCoord2f(1,0);
	glVertex3f(obj->vertex3_x,obj->vertex3_y,obj->vertex3_z);
	glTexCoord2f(0,0);
	glVertex3f(obj->vertex4_x,obj->vertex4_y,obj->vertex4_z);
	glEnd();
	glDisable(GL_BLEND);
}

void CSsp3DPannel::paintCylinder(t_ssp_3d_predefine_obj *obj)
{
	if (obj->texture_id)
	{
		CSsp3DTexture *tex = m_3DModel->SearchTexture(obj->texture_id);
		if (tex->m_Texture)
		{
			if (tex->m_record.tex_color[0] != 0x00)
				glEnable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, tex->m_Texture);
			gluCylinder(m_quadric,obj->vertex1_x,obj->vertex1_y,obj->vertex1_z,obj->vertex2_x,obj->vertex2_y);

			if (tex->m_record.tex_color[0] != 0x00)
				glDisable(GL_BLEND);
		}
	}
}

void CSsp3DPannel::paintConoid(t_ssp_3d_predefine_obj *obj)
{
	if (obj->texture_id)
	{
		CSsp3DTexture *tex = m_3DModel->SearchTexture(obj->texture_id);
		if (tex->m_Texture)
		{
			if (tex->m_record.tex_color[0] != 0x00)
				glEnable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, tex->m_Texture);
			gluCylinder(m_quadric,obj->vertex1_x,0.0,obj->vertex1_y,obj->vertex2_x,obj->vertex2_y);

			if (tex->m_record.tex_color[0] != 0x00)
				glDisable(GL_BLEND);
		}
	}
}

void CSsp3DPannel::paintSphere(t_ssp_3d_predefine_obj *obj,QString color)
{
	if (!color.isEmpty())
	{
		glDisable(GL_TEXTURE_2D);
		QColor c;
		c.setNamedColor(color);
		glColor3f((GLfloat)c.red()/255.0f,(GLfloat)c.green()/255.0f,(GLfloat)c.blue()/255.0f); 
		gluSphere(m_quadric,obj->vertex1_x,obj->vertex2_x,obj->vertex2_y);
		glColor3f(1.0f,1.0f,1.0f);
		glEnable(GL_TEXTURE_2D);
	}
	else if (obj->texture_id)
	{
		CSsp3DTexture *tex = m_3DModel->SearchTexture(obj->texture_id);
		if (tex->m_Texture)
		{
			if (tex->m_record.tex_color[0] != 0x00)
				glEnable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, tex->m_Texture);
			gluSphere(m_quadric,obj->vertex1_x,obj->vertex2_x,obj->vertex2_y);

			if (tex->m_record.tex_color[0] != 0x00)
				glDisable(GL_BLEND);
		}
	}
}

void CSsp3DPannel::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::RightButton)
	{
		m_mousePt.s.X = (GLfloat)(event->pos().x());
		m_mousePt.s.Y = (GLfloat)(event->pos().y());
		bRightButtonClicked = true;
	}
	else if(event->button() == Qt::LeftButton)
	{
		m_mousePt.s.X = (GLfloat)(event->pos().x());
		m_mousePt.s.Y = (GLfloat)(event->pos().y());
		bLeftButtonClicked = true;
	}

	setFocus();
	updateGL();

	QGLWidget::mousePressEvent(event);
}

void CSsp3DPannel::mouseMoveEvent(QMouseEvent *event)
{
	CSsp3DScene *m_scene = m_3DModel->SearchScene(m_sceneId);
	if (!m_scene)
		return;
	if (m_scene->m_record.scene_type == 4) //场景类型为变电站则为静态画面
		return;

	if (!bFirstPersn)
	{
		if(bLeftButtonClicked)
		{
			GLfloat posX = (GLfloat)(event->pos().x());
			GLfloat posY = (GLfloat)(event->pos().y());
			GLfloat m_x = posX-m_mousePt.s.X;
			GLfloat m_y = posY-m_mousePt.s.Y;
			m_mousePt.s.X = posX;
			m_mousePt.s.Y = posY;
			if (m_x > 0.0)
				pEye.zAngle -= 1.0;
			else if (m_x < 0.0)
				pEye.zAngle += 1.0;
			if (pEye.zAngle > 180.0)
				pEye.zAngle = -(360.0 - pEye.zAngle);
			if (pEye.zAngle < -180.0)
				pEye.zAngle = 360 + pEye.zAngle;

			if (m_y > 0.0)
			{
				if (pEye.xAngle < 89.0)
					pEye.xAngle += 1.0;
			}
			else if (m_y < 0.0)
			{
				if (pEye.xAngle > -89.0)
					pEye.xAngle -= 1.0;
			}

			bLeftButtonMoved = true;
			updateEyeXYZByRotatf();
			updateGL();	
		}
		else if (bRightButtonClicked)
		{
			GLfloat posX = (GLfloat)(event->pos().x());
			GLfloat posY = (GLfloat)(event->pos().y());
			GLfloat m_x = posX-m_mousePt.s.X;
			GLfloat m_y = posY-m_mousePt.s.Y;
			m_mousePt.s.X = posX;
			m_mousePt.s.Y = posY;

			float m_offset = 10.0;
			if (pEye.eye2center > 10000.0)
				m_offset = 100.0;
			else if (pEye.eye2center > 5000.0)
				m_offset = 50.0;
			else if (pEye.eye2center > 1000.0)
				m_offset = 20.0;
			else if (pEye.eye2center > 100.0)
				m_offset = 10.0;
			else
				m_offset = 5.0;

			if (m_x > 0.0)
				updateEyeCentXYZByTransL(m_offset);
			else if (m_x < 0.0)
				updateEyeCentXYZByTransR(m_offset);

			if (m_y > 0.0)
				updateEyeCentXYZByTransU(m_offset);
			else if (m_y < 0.0)
				updateEyeCentXYZByTransD(m_offset);

			bRightButtonMoved = true;
			updateGL();
		}
		else
		{
			QToolTip::hideText();
		}
	}
	else
	{
		if(bLeftButtonClicked)
		{
			GLfloat posX = (GLfloat)(event->pos().x());
			GLfloat posY = (GLfloat)(event->pos().y());
			GLfloat m_x = posX-m_mousePt.s.X;
			GLfloat m_y = posY-m_mousePt.s.Y;
			m_mousePt.s.X = posX;
			m_mousePt.s.Y = posY;
			if (m_x > 0.0)
				pEye.zAngle += 0.5;
			else if (m_x < 0.0)
				pEye.zAngle -= 0.5;
			if (pEye.zAngle > 180.0)
				pEye.zAngle = -(360.0 - pEye.zAngle);
			if (pEye.zAngle < -180.0)
				pEye.zAngle = 360 + pEye.zAngle;

			if (m_y > 0.0)
			{
				if (pEye.xAngle > -60.0)
					pEye.xAngle -= 0.5;
			}
			else if (m_y < 0.0)
			{
				if (pEye.xAngle < 60.0)
					pEye.xAngle += 0.5;
			}

			bLeftButtonMoved = true;
			updateCentXYZ();
			updateGL();	
		}
		else
		{
			QToolTip::hideText();
		}
	}

	QGLWidget::mouseMoveEvent(event);
}

void CSsp3DPannel::mouseReleaseEvent(QMouseEvent *event)
{
	if (bLeftButtonClicked && !bLeftButtonMoved)
	{
		int obj_id = 0;
		int scene_id = 0;
		GLuint name = onSelection(event->pos().x(),event->pos().y());
		getIdBySelectName(name,scene_id,obj_id);
		if (obj_id == 0 && scene_id == 0)
		{
			bLeftButtonClicked = false;
			bLeftButtonMoved = false;
			bRightButtonClicked = false;
			bRightButtonMoved = false;
			return;
		}

		CSsp3DScene *m_scene = m_3DModel->SearchScene(scene_id);
		if (m_scene)
		{
			CSsp3DSceneObj *m_sceneObj = m_scene->SearchObj(obj_id);
			if (m_sceneObj)
			{
				switch (m_sceneObj->m_record.obj_type)
				{
				case 1: //小室
					if (m_sceneObj->m_record.id3 > 0)
					{
						m_sceneId = m_sceneObj->m_record.id3;
						bInit = false;
					}
					break;
				case 2: //屏柜
					popupCabinetMenu(event,m_sceneObj);
					break;
				case 3: //保护、保测一体装置
				case 4: //测控装置
				case 5:	//合并单元
				case 6:	//智能终端
				case 7:	//远动机
				case 8:	//后台监控
				case 9:	//录波器
				case 10: //网络分析仪
					popupDeviceMenu(event,m_sceneObj);
					break;
				case 50: //硬开关、硬压板、硬按钮、硬刀闸

					break;
				case 100: //物体X轴顺时针旋转
					if (times == 0)
					{
						m_timeSceneObj = m_sceneObj;
						if (m_sceneObj->m_record.rotate_x == 0.0)
							m_rotate = SSP_ROTATE_MOD_FORWARD; //物体未旋转时需顺时针旋转
						else
							m_rotate = SSP_ROTATE_MOD_NEGATIVE;//物体已旋转后需逆时针旋转
					}
					break;
				case 101: //物体X轴逆时针旋转
					if (times == 0)
					{
						m_timeSceneObj = m_sceneObj;
						if (m_sceneObj->m_record.rotate_x == 0.0)
							m_rotate = SSP_ROTATE_MOD_NEGATIVE;//物体未旋转时需逆时针旋转
						else
							m_rotate = SSP_ROTATE_MOD_FORWARD; //物体已旋转后需顺时针旋转
					}
					break;
				case 102: //物体Y轴顺时针旋转
					if (times == 0)
					{
						m_timeSceneObj = m_sceneObj;
						if (m_sceneObj->m_record.rotate_y == 0.0)
							m_rotate = SSP_ROTATE_MOD_FORWARD; //物体未旋转时需顺时针旋转
						else
							m_rotate = SSP_ROTATE_MOD_NEGATIVE;//物体已旋转后需逆时针旋转
					}
					break;
				case 103: //物体Y轴逆时针旋转
					if (times == 0)
					{
						m_timeSceneObj = m_sceneObj;
						if (m_sceneObj->m_record.rotate_y == 0.0)
							m_rotate = SSP_ROTATE_MOD_NEGATIVE;//物体未旋转时需逆时针旋转
						else
							m_rotate = SSP_ROTATE_MOD_FORWARD; //物体已旋转后需顺时针旋转
					}
					break;
				case 200: //空调

					break;
				case 201: //摄像头

					break;
				case 202: //灭火器，灭火箱

					break;
				case 203: //火警探头

					break;
				case 204: //门开关

					break;
				case 205: //门铃

					break;
				case 206: //屏上告警球
					ssp3Dwidget->OnCabinetBug(m_sceneObj,event->globalPos());
					break;
				default:
					break;
				}
			}
		}
	}
	
	if (bRightButtonClicked && !bRightButtonMoved)
		popupMenu(event);

	bLeftButtonClicked = false;
	bLeftButtonMoved = false;
	bRightButtonClicked = false;
	bRightButtonMoved = false;

	updateGL();

	QGLWidget::mouseReleaseEvent(event);
}

void CSsp3DPannel::mouseDoubleClickEvent(QMouseEvent* event)
{
	QGLWidget::mouseDoubleClickEvent(event);
}

void CSsp3DPannel::wheelEvent(QWheelEvent *event)
{
	CSsp3DScene *m_scene = m_3DModel->SearchScene(m_sceneId);
	if (!m_scene)
		return;
	if (m_scene->m_record.scene_type == 4) //场景类型为变电站则为静态画面
		return;

	if (!bFirstPersn)
	{
		if(event->delta() > 0 )
		{
			if (pEye.eye2center < 50.0)
				return;

			if (pEye.eye2center > 10000.0)
				pEye.eye2center -= 500.0;
			else if (pEye.eye2center > 5000.0)
				pEye.eye2center -= 250.0;
			else if (pEye.eye2center > 1000.0)
				pEye.eye2center -= 100.0;
			else if (pEye.eye2center > 100.0)
				pEye.eye2center -= 50.0;
			else
				pEye.eye2center -= 10.0;
			updateEyeXYZByRotatf();
			updateGL();
		}
		else //if (pEye.eye2center < 4000.0)
		{
			if (pEye.eye2center > 10000.0)
				pEye.eye2center += 500.0;
			else if (pEye.eye2center > 5000.0)
				pEye.eye2center += 250.0;
			else if (pEye.eye2center > 1000.0)
				pEye.eye2center += 100.0;
			else if (pEye.eye2center > 100.0)
				pEye.eye2center += 50.0;
			else
				pEye.eye2center += 10.0;
			updateEyeXYZByRotatf();
			updateGL();
		}
	}
	else
	{
		if(event->delta() > 0 )
		{
			if (pEye.eyeY < 1000.0)
			{
				pEye.eyeY += 10.0;
				pEye.centerY += 10.0;
				updateGL();
			}
		}
		else if (pEye.eyeY > -1000.0)
		{
			pEye.eyeY -= 10.0;
			pEye.centerY -= 10.0;
			updateGL();
		}
	}

	QGLWidget::wheelEvent(event);
}

void CSsp3DPannel::keyPressEvent( QKeyEvent *event)
{
	CSsp3DScene *m_scene = m_3DModel->SearchScene(m_sceneId);
	if (!m_scene)
		return;
	if (m_scene->m_record.scene_type == 4) //场景类型为变电站则为静态画面
		return;

	float m_offset = 10.0;
	switch(event->key())
	{
	case Qt::Key_Right:
		break;
	case Qt::Key_Left:
		break;
	case Qt::Key_Up:
		if (bFirstPersn)
		{
			pEye.xAngle += 1.0;
			updateCentXYZ();
		}
		break;
	case Qt::Key_Down:
		if (bFirstPersn)
		{
			pEye.xAngle -= 1.0;
			updateCentXYZ();
		}
		break;
	case Qt::Key_PageUp:
		break;
	case Qt::Key_PageDown:
		break;
	case Qt::Key_D:
	case Qt::Key_A:
	case Qt::Key_W:
	case Qt::Key_S:
		if (pEye.eye2center > 10000.0)
			m_offset = 100.0;
		else if (pEye.eye2center > 5000.0)
			m_offset = 50.0;
		else if (pEye.eye2center > 1000.0)
			m_offset = 20.0;
		else if (pEye.eye2center > 100.0)
			m_offset = 10.0;
		else
			m_offset = 5.0;
		if (event->key() == Qt::Key_D)
			updateEyeCentXYZByTransR(m_offset);
		else if (event->key() == Qt::Key_A)
			updateEyeCentXYZByTransL(m_offset);
		else if (event->key() == Qt::Key_W)
			updateEyeCentXYZByTransU(m_offset);
		else if (event->key() == Qt::Key_S)
			updateEyeCentXYZByTransD(m_offset);
		break;
	case Qt::Key_Q:
		if (!bFirstPersn)
		{
			pEye.zAngle -= 1.0;
			if (pEye.zAngle > 180.0)
				pEye.zAngle = -(360.0 - pEye.zAngle);
			if (pEye.zAngle < -180.0)
				pEye.zAngle = 360 + pEye.zAngle;
			updateEyeXYZByRotatf();
		}
		else
		{
			pEye.zAngle -= 1.0;
			if (pEye.zAngle > 180.0)
				pEye.zAngle = -(360.0 - pEye.zAngle);
			if (pEye.zAngle < -180.0)
				pEye.zAngle = 360 + pEye.zAngle;
			updateCentXYZ();
		}
		break;
	case Qt::Key_E:
		if (!bFirstPersn)
		{
			pEye.zAngle += 1.0;
			if (pEye.zAngle > 180.0)
				pEye.zAngle = -(360.0 - pEye.zAngle);
			if (pEye.zAngle < -180.0)
				pEye.zAngle = 360 + pEye.zAngle;
			updateEyeXYZByRotatf();
		}
		else
		{
			pEye.zAngle += 1.0;
			if (pEye.zAngle > 180.0)
				pEye.zAngle = -(360.0 - pEye.zAngle);
			if (pEye.zAngle < -180.0)
				pEye.zAngle = 360 + pEye.zAngle;
			updateCentXYZ();
		}
		break;
	case Qt::Key_Z:
		if (!bFirstPersn)
		{
			if (pEye.eye2center > 100.0)
			{
				pEye.eye2center -= 50.0;
				updateEyeXYZByRotatf();
				updateGL();
			}
		}
		break;
	case Qt::Key_X:
		if (!bFirstPersn)
		{
			if (pEye.eye2center < 4000.0)
			{
				pEye.eye2center += 50.0;
				updateEyeXYZByRotatf();
				updateGL();
			}
		}
		break;
	}

	updateGL();

	QGLWidget::keyPressEvent(event);
}

static int refeshTimes = 0;
void CSsp3DPannel::timerEvent(QTimerEvent *event)
{
	if(hintTimer == NULL)
		return;
	if (!m_timeSceneObj)
	{
		if (++refeshTimes > 5)
		{
			refeshTimes = 0;
			updateGL();
		}
		return;
	}

	if (times >= 3)
	{
		times = 0;
		m_timeSceneObj = NULL;
		m_rotate = SSP_ROTATE_MOD_NULL;
		return;
	}

	switch (m_timeSceneObj->m_record.obj_type)
	{
	case 100:
	case 101:
		if (m_rotate == SSP_ROTATE_MOD_FORWARD)
			m_timeSceneObj->m_record.rotate_x -= 15.0;
		else if (m_rotate == SSP_ROTATE_MOD_NEGATIVE)
			m_timeSceneObj->m_record.rotate_x += 15.0;
		break;
	case 102:
	case 103:
		if (m_rotate == SSP_ROTATE_MOD_FORWARD)
			m_timeSceneObj->m_record.rotate_y -= 31.0;
		else if (m_rotate == SSP_ROTATE_MOD_NEGATIVE)
			m_timeSceneObj->m_record.rotate_y += 31.0;
		break;
	}

	times++;
	updateGL();
	refeshTimes = 0;
	
	QGLWidget::timerEvent(event);
}

void CSsp3DPannel::popupMenu(QMouseEvent *event)
{
	menuScene->clear();

	refeshAction = menuScene->addAction(QIcon(":/ssp_gui_3d/reload"),tr("重新加载(&R)"));
	connect(refeshAction,SIGNAL(triggered()),this,SLOT(slotReloadScene()));

	CSsp3DScene *m_scene = m_3DModel->SearchScene(m_sceneId);
	if (!m_scene)
		return;

	menuScene->addSeparator();
	parentAction = menuScene->addAction(QIcon(":/ssp_gui_3d/scene"),tr("父场景(&P)"));
	connect(parentAction,SIGNAL(triggered()),this,SLOT(slotParentScene()));
	if (!m_scene->m_record.p_scene_id)
		parentAction->setEnabled(false);

	int equalSceneNum = 0;
	int childSceneNum = 0;
	unsigned long pos = 0;
	CSsp3DScene *s = m_3DModel->m_Scenes.FetchFirst(pos);
	while(s)
	{
		if (s->m_record.p_scene_id == m_scene->m_record.p_scene_id && s->m_record.scene_id != m_sceneId)
			equalSceneNum++;
		else if (s->m_record.p_scene_id == m_scene->m_record.scene_id)
			childSceneNum++;
		s = m_3DModel->m_Scenes.FetchNext(pos);
	}
	equalAction = menuScene->addAction(QIcon(":/ssp_gui_3d/scene"),tr("同场景(&E)"));
	connect(equalAction,SIGNAL(triggered()),this,SLOT(slotEqualScene()));
	childAction = menuScene->addAction(QIcon(":/ssp_gui_3d/scene"),tr("子场景(&C)"));
	connect(childAction,SIGNAL(triggered()),this,SLOT(slotChildScene()));
	if (!equalSceneNum)
		equalAction->setEnabled(false);
	if (!childSceneNum)
		childAction->setEnabled(false);

	menuScene->addSeparator();
	eyeVPAction = menuScene->addAction(QIcon(":/ssp_gui_3d/eye"),tr("视点位置(&W)"));
	connect(eyeVPAction,SIGNAL(triggered()),this,SLOT(slotEyeViewPoint()));
	selVPAction = menuScene->addAction(QIcon(":/ssp_gui_3d/eye"),tr("选择视点(&L)"));
	connect(selVPAction,SIGNAL(triggered()),this,SLOT(slotSelectViewPoint()));
	savVPAction = menuScene->addAction(QIcon(":/ssp_gui_3d/save"),tr("保存视点(&S)"));
	connect(savVPAction,SIGNAL(triggered()),this,SLOT(slotSaveViewPoint()));
	bFirstPersn ? savVPAction->setEnabled(false) : savVPAction->setEnabled(true);
	delVPAction = menuScene->addAction(QIcon(":/ssp_gui_3d/cancel"),tr("删除视点(&D)"));
	connect(delVPAction,SIGNAL(triggered()),this,SLOT(slotDelViewPoint()));

	menuScene->addSeparator();
	increaLightAction = menuScene->addAction(QIcon(":/ssp_gui_3d/increaseLig"),tr("提高亮度(&+)"));
	connect(increaLightAction,SIGNAL(triggered()),this,SLOT(slotIncreaseLight()));
	reduceLightAction = menuScene->addAction(QIcon(":/ssp_gui_3d/reduceLig"),tr("降低亮度(&-)"));
	connect(reduceLightAction,SIGNAL(triggered()),this,SLOT(slotReduceLight()));
	if (m_lightAmbient[0] >= 1.0 || m_lightAmbient[1] >= 1.0 || m_lightAmbient[2] >= 1.0)
		increaLightAction->setEnabled(false);
	if (m_lightAmbient[0] <= -1.0 || m_lightAmbient[1] <= -1.0 || m_lightAmbient[2] <= -1.0)
		reduceLightAction->setEnabled(false);

	menuScene->addSeparator();
	fpsAction = menuScene->addAction(QIcon(":/ssp_gui_3d/setting"),tr("每秒帧数(FPS)(&F)"));
	connect(fpsAction,SIGNAL(triggered()),this,SLOT(slotFps()));
	verAction = menuScene->addAction(QIcon(":/ssp_gui_3d/setting"),tr("显卡信息(&G)"));
	connect(verAction,SIGNAL(triggered()),this,SLOT(slotVersion()));

	menuScene->addSeparator();
	firstVPAction = menuScene->addAction(tr("第一人称(&N)"));
	firstVPAction->setCheckable(true);
	if (bFirstPersn)
		firstVPAction->setChecked(true);
	else
		firstVPAction->setChecked(false);
	connect(firstVPAction,SIGNAL(triggered()),this,SLOT(slotIsFirstPerson()));
	gridAction = menuScene->addAction(tr("坐标轴(&A)"));
	gridAction->setCheckable(true);
	if (bDrawGrid)
		gridAction->setChecked(true);
	else
		gridAction->setChecked(false);
	connect(gridAction,SIGNAL(triggered()),this,SLOT(slotIsDrawGrid()));

	menuScene->popup(event->globalPos());
}

void CSsp3DPannel::popupCabinetMenu(QMouseEvent *event,CSsp3DSceneObj *m_sceneObj)
{
	menuCabinet->clear();

	m_cabSceneObj = m_sceneObj;
	tookSceneId = m_sceneObj->m_record.scene_id;

	cabTookAction = menuCabinet->addAction(QIcon(":/ssp_gui_3d/scene"),tr("查看屏柜(&V)"));
	menuCabinet->addSeparator();
	cabDrawAction = menuCabinet->addAction(QIcon(":/ssp_gui_3d/draw"),tr("屏柜图纸(&W)"));

	connect(cabTookAction,SIGNAL(triggered()),this,SLOT(slotTookScene()));
	connect(cabDrawAction,SIGNAL(triggered()),this,SLOT(slotCabDraw()));
	
	menuCabinet->popup(event->globalPos());
}

void CSsp3DPannel::popupDeviceMenu(QMouseEvent *event,CSsp3DSceneObj *m_sceneObj)
{
	menuDevice->clear();

	m_oeSceneObj = m_sceneObj;
	//devBookAction = menuDevice->addAction(QIcon(":/ssp_gui_3d/scene"),tr("装置台账(&T)"));
	//connect(devBookAction,SIGNAL(triggered()),this,SLOT(slotDeviceBook()));

	devTookAction = menuDevice->addAction(QIcon(":/ssp_gui_3d/scene"),tr("查看设备(&V)"));
	menuDevice->addSeparator();
	devInfoAction = menuDevice->addAction(QIcon(":/ssp_gui_3d/device"),tr("设备信息(&I)"));
	devLinkAction = menuDevice->addAction(QIcon(":/ssp_gui_3d/link"),tr("设备虚链路(&L)"));
	//menuDevice->addSeparator();
	//devDrawAction = menuDevice->addAction(QIcon(":/ssp_gui_3d/draw"),tr("设备图纸(&W)"));
	menuDevice->addSeparator();
	devBugAction = menuDevice->addAction(QIcon(":/ssp_gui_3d/bug"),tr("设备缺陷(&U)"));
	devRealState = menuDevice->addAction(QIcon(":/ssp_gui_3d/bug"),tr("设备实时状态(&R)"));
	devScoreState = menuDevice->addAction(QIcon(":/ssp_gui_3d/bug"),tr("健康趋势(&Q)"));

	connect(devTookAction,SIGNAL(triggered()),this,SLOT(slotDeviceTook()));
	connect(devInfoAction,SIGNAL(triggered()),this,SLOT(slotDeviceInfo()));
	//connect(devDrawAction,SIGNAL(triggered()),this,SLOT(slotDeviceDraw()));
	connect(devLinkAction,SIGNAL(triggered()),this,SLOT(slotDeviceLink()));
	connect(devBugAction,SIGNAL(triggered()),this,SLOT(slotDeviceBug()));
	connect(devRealState,SIGNAL(triggered()),this,SLOT(slotDeviceRealState()));
	connect(devScoreState,SIGNAL(triggered()),this,SLOT(slotDeviceScoreState()));

	menuDevice->popup(event->globalPos());
}

void CSsp3DPannel::slotReloadScene()
{
	if(QMessageBox::question(NULL,"询问","您确定要重新加载吗?",QDialogButtonBox::Yes,QDialogButtonBox::No) != QDialogButtonBox::Yes)
		return ;

	progressMaxValue = 0;
	SRecordset rs;
	DB->Retrieve("select count(*) from t_ssp_3d_texture",rs);
	progressMaxValue += rs.GetValue(0,0).toInt();
	DB->Retrieve("select count(*) from t_ssp_3d_scene_object where draw_type=5",rs);
	progressMaxValue += rs.GetValue(0,0).toInt() > 0 ? 1 : 0;
	DB->Retrieve("select count(*) from t_ssp_3d_predefine_obj where draw_type=5",rs);
	progressMaxValue += rs.GetValue(0,0).toInt() > 0 ? 1 : 0;
	DB->Retrieve("select count(*) from t_ssp_3d_vrml",rs);
	progressMaxValue += rs.GetValue(0,0).toInt();
	if (!progressMaxValue)
	{
		QMessageBox::information(NULL,tr("提示"),tr("无图片模型。"));
		paintMutex.lock();
		m_3DModel->LoadByDb(DB,false); 
		bInit = false;
		paintMutex.unlock();
		updateGL();
		return;
	}
	
	progressDialog->setRange(0,progressMaxValue);
	progressDialog->setWindowTitle(tr("信息"));
	progressDialog->setLabelText(tr("3D模型导入中，请等待......"));
	progressDialog->setValue(0);
	progressDialog->show();

	m_loadDbThread->start();
}

void CSsp3DPannel::slotParentScene()
{
	CSsp3DScene *m_scene = m_3DModel->SearchScene(m_sceneId);
	if (!m_scene)
		return;

	m_sceneId = m_scene->m_record.p_scene_id;
	bInit = false;
}

void CSsp3DPannel::slotEqualScene()
{
	DlgSelectScene dlg(this);
	dlg.set3DModel(m_3DModel);
	dlg.setSceneId(m_sceneId);
	dlg.setSceneType(SCENE_TYPE_EQUAL);
	dlg.start();
	if (dlg.exec() == QDialog::Accepted)
	{
		m_sceneId = dlg.getSceneId();
		bInit = false;
	}
}

void CSsp3DPannel::slotChildScene()
{
	DlgSelectScene dlg(this);
	dlg.set3DModel(m_3DModel);
	dlg.setSceneId(m_sceneId);
	dlg.setSceneType(SCENE_TYPE_CHILD);
	dlg.start();
	if (dlg.exec() == QDialog::Accepted)
	{
		m_sceneId = dlg.getSceneId();
		bInit = false;
	}
}

void CSsp3DPannel::slotTookScene()
{
	m_sceneId = tookSceneId;
	bInit = false;
}

void CSsp3DPannel::slotCabDraw()
{
	if (m_cabSceneObj)
		ssp3Dwidget->OnCabinetDraw(m_cabSceneObj);
}

void CSsp3DPannel::slotDeviceTook()
{
	m_sceneId = m_oeSceneObj->m_record.scene_id;
	bInit = false;
}

void CSsp3DPannel::slotDeviceBook()
{
	if (m_oeSceneObj)
		ssp3Dwidget->OnDeviceBook(m_oeSceneObj);
}

void CSsp3DPannel::slotDeviceInfo()
{
	if (m_oeSceneObj)
		ssp3Dwidget->OnDeviceInfo(m_oeSceneObj);
}

void CSsp3DPannel::slotDeviceLink()
{
	if (m_oeSceneObj)
		ssp3Dwidget->OnDeviceLink(m_oeSceneObj);
}

void CSsp3DPannel::slotDeviceDraw()
{
	if (m_oeSceneObj)
		ssp3Dwidget->OnDeviceDraw(m_oeSceneObj);
}

void CSsp3DPannel::slotDeviceBug()
{
	if (m_oeSceneObj)
		ssp3Dwidget->OnDeviceBug(m_oeSceneObj);
}

void CSsp3DPannel::slotDeviceRealState()
{
	if (m_oeSceneObj)
		ssp3Dwidget->OnDeviceRealState(m_oeSceneObj);
}


void CSsp3DPannel::slotDeviceScoreState()
{
	if (m_oeSceneObj)
		ssp3Dwidget->OnDeviceScoreState(m_oeSceneObj);
}


void CSsp3DPannel::slotFps()
{
	int frames = 50;
	clock_t start = clock();
	for(int i = 0; i < frames; i++ )
		updateGL();
	clock_t finish = clock();
	float fps = float(frames) / (finish-start) * 1000;

	QMessageBox::information(NULL,tr("提示"),tr("每秒帧数(FPS)：%1").arg(fps));
}

void CSsp3DPannel::slotVersion()
{
	QString s;
	s += tr("显卡型号：%1\n\n").arg((char *)glGetString(GL_RENDERER));
	s += tr("OpenGL版本：%1").arg((char *)glGetString(GL_VERSION));
	QMessageBox::information(NULL,tr("提示"),s);
}

void CSsp3DPannel::slotEyeViewPoint()
{
	QString s;
	s += tr("眼睛位置：X(%1)、Y(%2)、Z(%3)\n\n").arg(pEye.eyeX).arg(pEye.eyeY).arg(pEye.eyeZ);
	s += tr("视点位置：X(%1)、Y(%2)、Z(%3)\n\n").arg(pEye.centerX).arg(pEye.centerY).arg(pEye.centerZ);
	s += tr("视点距离：%1\n\n").arg(pEye.eye2center);
	s += tr("X轴 角度：%1\n\n").arg(pEye.xAngle);
	s += tr("Z轴 角度：%1").arg(pEye.zAngle);
	QMessageBox::information(NULL,tr("提示"),s);
}

void CSsp3DPannel::slotSelectViewPoint()
{
	DlgSelectViewPoint dlg(this);
	dlg.set3DModel(m_3DModel);
	dlg.setSceneId(m_sceneId);
	dlg.start();
	if (dlg.exec() == QDialog::Accepted)
	{
		int sceneId = dlg.getSceneId();
		int vpId = dlg.getViewPointId();

		unsigned long pos = 0;
		CSsp3DSceneViewPoint *v = m_3DModel->m_SceneViewPoints.FetchFirst(pos);
		while (v)
		{
			if (v->m_iSceneId == sceneId)
			{
				unsigned long pos1 = 0;
				t_ssp_3d_scene_viewpoint *p = v->m_ViewPoints.FetchFirst(pos1);
				while (p)
				{
					if (p->vpoint_id == vpId)
					{
						paintMutex.lock();
						if (bFirstPersn)
						{
							pEye.eyeX = p->eye_x;
							pEye.eyeY = 170.0;
							pEye.eyeZ = p->eye_z;
							pEye.centerX = p->eye_forw_x;
							pEye.centerY = 170.0;
							pEye.centerZ = p->eye_forw_z;
							pEye.eye2center = fabs(pEye.eyeZ - pEye.centerZ);
							pEye.xAngle = 0.0;
							//pEye.zAngle = 0.0;
							updateZAngle();
						}
						else
						{
							pEye.eyeX = p->eye_x;
							pEye.eyeY = p->eye_y;
							pEye.eyeZ = p->eye_z;
							pEye.centerX = p->eye_forw_x;
							pEye.centerY = p->eye_forw_y;
							pEye.centerZ = p->eye_forw_z;
							pEye.eye2center = p->cent2eye;
							pEye.xAngle = p->x_angle;
							pEye.zAngle = p->z_angle;
						}
						paintMutex.unlock();
						return;
					}
					p = v->m_ViewPoints.FetchNext(pos1);
				}
			}
			v = m_3DModel->m_SceneViewPoints.FetchNext(pos);
		}
	}
}

void CSsp3DPannel::slotSaveViewPoint()
{
	DlgSaveViewPoint dlg(this);
	dlg.set3DModel(m_3DModel);
	dlg.setSceneId(m_sceneId);
	dlg.setEyeX(pEye.eyeX);
	dlg.setEyeY(pEye.eyeY);
	dlg.setEyeZ(pEye.eyeZ);
	dlg.setCenterX(pEye.centerX);
	dlg.setCenterY(pEye.centerY);
	dlg.setCenterZ(pEye.centerZ);
	dlg.setUpX(pEye.upX);
	dlg.setUpY(pEye.upY);
	dlg.setUpZ(pEye.upZ);
	dlg.setEye2Center(pEye.eye2center);
	dlg.setXAngle(pEye.xAngle);
	dlg.setZAngle(pEye.zAngle);
	dlg.start();
	if (dlg.exec() == QDialog::Accepted)
	{
		QMessageBox::information(this,tr("提示"),tr("重启系统，修改视点才可生效。"));
	}
}

void CSsp3DPannel::slotDelViewPoint()
{
	DlgDelViewPoint dlg(this);
	dlg.set3DModel(m_3DModel);
	dlg.setSceneId(m_sceneId);
	dlg.start();
	if (dlg.exec() == QDialog::Accepted)
	{
		QMessageBox::information(this,tr("提示"),tr("重启系统，删除视点才可生效。"));
	}
}

void CSsp3DPannel::slotIsFirstPerson()
{
	bFirstPersn = !bFirstPersn;
	if (bFirstPersn)
		firstVPAction->setChecked(true);
	else
		firstVPAction->setChecked(false);

	bInit = false;
	updateGL();
}

void CSsp3DPannel::slotIsDrawGrid()
{
	bDrawGrid = !bDrawGrid;
	if (bDrawGrid)
		gridAction->setChecked(true);
	else
		gridAction->setChecked(false);

	updateGL();
}

void CSsp3DPannel::slotIncreaseLight()
{
	m_lightAmbient[0] += 0.1;
	m_lightAmbient[1] += 0.1;
	m_lightAmbient[2] += 0.1;
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, m_lightAmbient);
}

void CSsp3DPannel::slotReduceLight()
{
	m_lightAmbient[0] -= 0.1;
	m_lightAmbient[1] -= 0.1;
	m_lightAmbient[2] -= 0.1;
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, m_lightAmbient);
}

GLuint CSsp3DPannel::onSelection(int xPos, int yPos)
{
	GLint hits;
	GLint viewport[4];
	GLuint modelselect = 0;
	GLfloat aspect;

	glSelectBuffer(BUFFER_LENGTH, SelBuff);		//设置选择缓冲区
	glGetIntegerv(GL_VIEWPORT, viewport);		//获取可视区域

	glRenderMode(GL_SELECT);		//改变渲染模式为选择模式
	glInitNames();					// 设名字堆栈
	glPushName(0);					// Push 0 (最少一个) 在栈上

	glMatrixMode(GL_PROJECTION);	//切换矩阵模式为投影模式并保存当前矩阵
	glPushMatrix();
	glLoadIdentity();				//以 xPos、yPos 为中心建立新的单位立方体裁剪区，并向外扩展两个像素
	gluPickMatrix((GLdouble)xPos,(GLdouble)(viewport[3] - yPos/* + viewport[1]*/),1,1,viewport);

	aspect = GLfloat(viewport[2]) / GLfloat(viewport[3]);
	gluPerspective(45.0f, aspect, 1.0f, 200000.0f);

	glMatrixMode(GL_MODELVIEW);	// 选择模型变换矩阵
	updateGL();
	glMatrixMode(GL_PROJECTION);// 选择投影变换矩阵
	glPopMatrix();				// 取出投影矩阵
	glMatrixMode(GL_MODELVIEW);	// 选模式显示矩阵

	hits = glRenderMode(GL_RENDER);	//收集单击记录
	if(hits > 0)
	{
		int n = 0;
		double minz = 0xffffffff;
		QString t;
		for (int i = 0; i < hits; i++)
		{
			if (SelBuff[i*4+3] == 0)
				continue;

			if (SelBuff[i*4+1] < minz)
			{
				n = i;
				minz = SelBuff[1+i*4];
			}
			t+=tr("%1,").arg(SelBuff[i*4+3]);
		}
		modelselect = SelBuff[n*4+3];
	}

	return modelselect;
}

#define TIME_LOOP_CABINET		60

void CSsp3DPannel::slotHintTimerOut()
{
	if (!(m_iTimeCount % 60))
	{
		unsigned long pos,pos1;
		CSsp3DScene *scene = m_3DModel->m_Scenes.FetchFirst(pos);
		while(scene)
		{
			CSsp3DSceneObj *obj = scene->m_Objs.FetchFirst(pos1);
			while(obj)
			{
				if (obj->m_record.draw_type == 200)
					obj->color = ssp3Dwidget->OnCabinetAlarm(obj->m_record.id1,obj->m_record.id2,obj->m_record.id3,obj->m_record.id4).toStdString().data();
				obj = scene->m_Objs.FetchNext(pos1);
			}
			scene = m_3DModel->m_Scenes.FetchNext(pos);
		}
	}
	m_iTimeCount++;

	int obj_id = 0;
	int scene_id = 0;
	QPoint p = QCursor::pos();
	p = this->mapFromGlobal(p);
	GLuint name = onSelection(p.x(),p.y());
	getIdBySelectName(name,scene_id,obj_id);
	CSsp3DScene *m_scene = m_3DModel->SearchScene(scene_id);
	if (m_scene)
	{
		CSsp3DSceneObj *m_sceneObj = m_scene->SearchObj(obj_id);
		if (!m_sceneObj)
			return;

		if (m_sceneObj->m_record.obj_type == 7 ||
			m_sceneObj->m_record.obj_type == 8 || m_sceneObj->m_record.obj_type == 9 || m_sceneObj->m_record.obj_type ==10 ||
			m_sceneObj->m_record.obj_type ==50 || m_sceneObj->m_record.obj_type ==200|| m_sceneObj->m_record.obj_type ==201||
			m_sceneObj->m_record.obj_type ==202|| m_sceneObj->m_record.obj_type ==203|| m_sceneObj->m_record.obj_type ==204||
			m_sceneObj->m_record.obj_type ==205)
		{
			if (isActiveWindow())
				QToolTip::showText(p,tr("%1").arg(m_sceneObj->m_record.obj_name));
		}
		else if (m_sceneObj->m_record.obj_type == 2) //屏柜
		{
			QString tip;
			tip = tr("%1\n-------------------------------").arg(m_sceneObj->m_record.obj_name);
			SString sql;
			SRecordset rs;
			sql.sprintf("select a.name,a.curr_score,a.oh_name from v_nam_device a,t_oms_cabinets_device b "
				"where a.dev_cls=b.dev_cls and a.dev_id=b.dev_id and b.sub_no=%d and b.room_id=%d and b.cab_id=%d",
				m_sceneObj->m_record.id1,m_sceneObj->m_record.id3,m_sceneObj->m_record.id4);
			int cnt = DB->Retrieve(sql,rs);
			if (cnt > 0)
			{
				for (int i = 0; i < cnt; i++)
				{
					QString name = rs.GetValue(i,0).data();
					float curr_score = rs.GetValue(i,1).toFloat();
					QString oh_name = rs.GetValue(i,2).data();
					tip += tr("\n%1，状态:%2(%3分)").arg(name).arg(oh_name).arg(curr_score);
				}
			}
			else
				tip += tr("\n未发现设备");

			if (isActiveWindow())
				QToolTip::showText(p,tip);
		}
		else if (m_sceneObj->m_record.obj_type == 3 || m_sceneObj->m_record.obj_type == 4 ||
				 m_sceneObj->m_record.obj_type == 5 || m_sceneObj->m_record.obj_type == 6) //保护、保测一体装置、测控装置、合并单元、智能终端
		{
			QString model = "未知";
			QString manufacturer = "未知";
			QString version = "未知";
			QString crc = "未知";

			QString tip;
			SString sql;
			SRecordset rs;
			sql.sprintf("select name,curr_score,oh_name from v_nam_device where sub_no=%d and dev_cls=1 and dev_id=%d",
				m_sceneObj->m_record.id1,m_sceneObj->m_record.id3);
			int cnt = DB->Retrieve(sql,rs);
			if (cnt > 0)
			{
				QString name = rs.GetValue(0,0).data();
				float curr_score = rs.GetValue(0,1).toFloat();
				QString oh_name = rs.GetValue(0,2).data();
				tip = tr("%1，状态:%2(%3分)\n-------------------------------").arg(name).arg(oh_name).arg(curr_score);
				sql.sprintf("select model,manufacturer,version,crc from t_oe_account where sub_no=%d and dev_cls=1 and dev_id=%d",
					m_sceneObj->m_record.id1,m_sceneObj->m_record.id3);
				cnt = DB->Retrieve(sql,rs);
				if (cnt > 0)
				{
					model = rs.GetValue(0,0).data();
					manufacturer = rs.GetValue(0,1).data();
					version = rs.GetValue(0,2).data();
					crc = rs.GetValue(0,3).data();
				}
			}
			else
			{
				tip = tr("未发现设备\n----------------");
			}
			tip += tr("\n装置型号：%1").arg(model);
			tip += tr("\n制造厂商：%1").arg(manufacturer);
			tip += tr("\n装置版本：%1").arg(version);
			tip += tr("\n校 验 码：%1").arg(crc);

			if (isActiveWindow())
				QToolTip::showText(p,tip);
		}
	}
}

void CSsp3DPannel::drawGrid()
{
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.5f,0.5f,0.5f);
	for(float i = -50000; i <= 50000; i += 100)
	{
		glLineWidth(1); 
		glBegin(GL_LINES);
		glVertex3f(-50000, 0, i);					
		glVertex3f(50000, 0, i);
		glVertex3f(i, 0,-50000);							
		glVertex3f(i, 0, 50000);
		glEnd();
	}
	glColor3f(1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
}

void CSsp3DPannel::drawCoordinateZero()
{
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f,0.0f,0.0f); 
	glLineWidth(1); 
	glBegin(GL_LINES);  
	glVertex3f(0.0f,0.0f,0.0f);  
	glVertex3f(1000000.0f,0.0f,0.0f);  
	glEnd();  
	glPushMatrix();  
	glTranslatef(1000000.0f,0.0f,0.0f);  
	glRotatef(90.0f,0.0f,1.0f,0.0f);  
	glutWireCone(3,6,10,10);  
	glPopMatrix(); 
	glEnable(GL_LINE_STIPPLE); 
	glLineStipple(1,0x0F0F);    
	glBegin(GL_LINES);  
	glVertex3f(0.0f,0.0f,0.0f);  
	glVertex3f(-1000000.0f,0.0f,0.0f);  
	glEnd(); 
	glDisable(GL_LINE_STIPPLE); 

	glColor3f(0.0f,1.0f,0.0f); 
	glLineWidth(1); 
	glBegin(GL_LINES);  
	glVertex3f(0.0f,0.0f,0.0f);  
	glVertex3f(0.0f,1000000.0f,0.0f); 
	glEnd();
	glPushMatrix();  
	glTranslatef(0.0f,1000000.0f,0.0f);  
	glRotatef(-90.0f,1.0f,0.0f,0.0f);  
	glutWireCone(3,6,10,10);  
	glPopMatrix();
	glEnable(GL_LINE_STIPPLE); 
	glLineStipple(1,0x0F0F);    
	glBegin(GL_LINES);  
	glVertex3f(0.0f,0.0f,0.0f);  
	glVertex3f(0.0f,-1000000.0f,0.0f);  
	glEnd(); 
	glDisable(GL_LINE_STIPPLE); 

	glColor3f(0.0f,0.0f,1.0f); 
	glLineWidth(1); 
	glBegin(GL_LINES);  
	glVertex3f(0.0f,0.0f,0.0f);  
	glVertex3f(0.0f,0.0f,1000000.0f);  
	glEnd();
	glPushMatrix();  
	glTranslatef(0.0f,0.0f,1000000.0f);  
	glRotatef(90.0f,0.0f,0.0f,1.0f);  
	glutWireCone(3,6,10,10);  
	glPopMatrix(); 
	glEnable(GL_LINE_STIPPLE); 
	glLineStipple(1,0x0F0F);    
	glBegin(GL_LINES);  
	glVertex3f(0.0f,0.0f,0.0f);  
	glVertex3f(0.0f,0.0f,-1000000.0f);  
	glEnd(); 
	glDisable(GL_LINE_STIPPLE); 

	glColor3f(1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
}

void CSsp3DPannel::drawSkybox()
{
	if (m_skyboxTexture)
	{
		glPushMatrix();
		glDisable(GL_LIGHTING);
		glRotatef(-90,1.0f,0.0f,0.0f);
		glBindTexture(GL_TEXTURE_2D, m_skyboxTexture);
		gluSphere(m_quadric,SSP_ZFAR-500000.0,64,64);
		glRotatef(90,1.0f,0.0f,0.0f);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}
}

void CSsp3DPannel::spotLight(GLfloat ambient[4],
							 GLfloat diffuse[4],
							 GLfloat specular[4],
							 GLfloat postion[4],
							 GLfloat direct[3],
							 GLfloat spotCutoff,
							 GLfloat spotExponent)
{
	glPushMatrix();
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,spotCutoff);
	glLighti(GL_LIGHT0,GL_SPOT_EXPONENT,spotExponent);
	glLightfv(GL_LIGHT0,GL_POSITION,postion);
	glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,direct);
	glEnable(GL_LIGHT0);
	glPopMatrix();
}

bool CSsp3DPannel::isExtensionSupported(char* szTargetExtension)
{
	const unsigned char *pszExtensions = NULL;
	const unsigned char *pszStart;
	unsigned char *pszWhere, *pszTerminator;
	pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
	if( pszWhere || *szTargetExtension == '\0' )
		return false;
	
	pszExtensions = glGetString(GL_EXTENSIONS); //返回扩展字符串
	pszStart = pszExtensions;
	for(;;) //在扩展字符串中搜索
	{
		pszWhere = (unsigned char *)strstr((const char *) pszStart, szTargetExtension);
		if( !pszWhere )
			break;
		pszTerminator = pszWhere + strlen( szTargetExtension );
		if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
			if( *pszTerminator == ' ' || *pszTerminator == '\0' )
				return true; //如果存在返回True
		pszStart = pszTerminator;
	}

	return false;
}
