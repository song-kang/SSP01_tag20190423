/**
 *
 * 文 件 名 : ssp_3d_scene.h
 * 创建日期 : 2017-3-2 9:48
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 3D场景类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2017-3-2	邵凯田　创建文件
 *
 **/

#ifndef __SSP_3D_SCENE_H__
#define __SSP_3D_SCENE_H__

#include "SString.h"
#include "SList.h"
#include "ssp_3d_inc.h"
#include "ssp_3d_predefine.h"
#include "ssp_3d_texture.h"



class SSP_GUI_3D_EXPORT CSsp3DSceneObj
{
public:
	CSsp3DSceneObj(t_ssp_3d_scene_object *p=NULL);
	~CSsp3DSceneObj();

	t_ssp_3d_scene_object m_record;
	CSsp3DTexture *m_pTexture;
	CSsp3DObject *m_p3DObject;//3D有名对象的指针，NULL表示非有名对象

	SString color;

public:
	bool LoadTexture();

public:
	bool LoadRGBATexture(SString path);
	unsigned char* LoadBMPFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader,unsigned char *backgroundColor,bool bAlpha = false);
};

class SSP_GUI_3D_EXPORT CSsp3DScene
{
public:
	CSsp3DScene(t_ssp_3d_scene *p=NULL);
	~CSsp3DScene();
	inline CSsp3DSceneObj* SearchObj(int obj_id)
	{
		unsigned long pos;
		CSsp3DSceneObj *p = m_Objs.FetchFirst(pos);
		while(p)
		{
			if(p->m_record.obj_id == obj_id)
				return p;
			p = m_Objs.FetchNext(pos);
		}
		return NULL;
	}

	t_ssp_3d_scene m_record;
	/*
	int m_iSceneId;
	int m_iPSceneId;
	SString m_sSceneName;
	int m_iSceneType;
	SString m_sSceneDesc;
	int m_iId1;
	int m_iId2;
	int m_iId3;
	int m_iId4;
	float m_fCenter[3];
	float m_fRotate[3];
	float m_fScale;
	float m_fEyeRange[3][2];//x/y/z对应的最小、最大值
	*/

	//
	CSsp3DScene *m_pParentScene;
	CSsp3DObject *m_p3DObject;//3D有名对象的指针，NULL表示非有名对象
	SPtrList<CSsp3DSceneObj> m_Objs;
};


#endif//__SSP_3D_SCENE_H__
