/**
 *
 * 文 件 名 : ssp_3d_texture.h
 * 创建日期 : 2017-3-1 18:46
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 3D纹理相关定义
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2017-3-1	邵凯田　创建文件
 *
 **/

#ifndef __SSP_3D_TEXTURE_H__
#define __SSP_3D_TEXTURE_H__

#include "SString.h"
#include "SList.h"
#include "ssp_3d_inc.h"
#include <QGLWidget>

class SSP_GUI_3D_EXPORT CSsp3DTexture
{
public:

	CSsp3DTexture(t_ssp_3d_texture *p=NULL);
	~CSsp3DTexture();

	void SetGlWidget(QGLWidget *pWnd){m_pGlWidget = pWnd;};

	bool LoadTexture(bool bLoadTexture = true);
	bool RemoveTexture();
	bool LoadRGBTexture(SString path);
	bool LoadRGBATexture(SString path);
	bool LoadColorTexture();
	unsigned char* LoadBMPFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader,unsigned char *backgroundColor,bool bAlpha = false);

	t_ssp_3d_texture m_record;
	QGLWidget *m_pGlWidget;
	GLuint m_Texture;
};



#endif//__SSP_3D_TEXTURE_H__
