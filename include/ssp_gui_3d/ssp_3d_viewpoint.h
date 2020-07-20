/**
 *
 * 文 件 名 : ssp_3d_viewpoint.h
 * 创建日期 : 2017-3-2 9:19
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 视点类封装
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2017-3-2	邵凯田　创建文件
 *
 **/

#ifndef __SSP_3D_VIEW_POINT_H__
#define __SSP_3D_VIEW_POINT_H__

#include "SString.h"
#include "SList.h"
#include "ssp_3d_inc.h"
#include "ssp_gui_3d_inc.h"

/*
class CSsp3DViewPoint
{
public:
	CSsp3DViewPoint();
	~CSsp3DViewPoint();

	int m_iVpId;
	SString m_sVpName;
	float m_fEye[3];
	float m_fEyeForward[3];
	float m_fEyeUp[3];
	float m_fEyeRight[3];
	float m_fViewAngle;
	float m_fCent2Eye;
};*/

class SSP_GUI_3D_EXPORT CSsp3DSceneTypeViewPoint
{
public:
	CSsp3DSceneTypeViewPoint();
	~CSsp3DSceneTypeViewPoint();

	inline t_ssp_3d_scene_type_viewpoint* SearchViewPoint(int vpoint_id)
	{
		unsigned long pos;
		t_ssp_3d_scene_type_viewpoint *p = m_ViewPoints.FetchFirst(pos);
		while(p)
		{
			if(p->vpoint_id == vpoint_id)
				return p;
			p = m_ViewPoints.FetchNext(pos);
		}
		return NULL;
	}

	int m_iSceneType;
	SPtrList<t_ssp_3d_scene_type_viewpoint> m_ViewPoints;
};


class SSP_GUI_3D_EXPORT CSsp3DSceneViewPoint
{
public:
	CSsp3DSceneViewPoint();
	~CSsp3DSceneViewPoint();

	inline t_ssp_3d_scene_viewpoint* SearchViewPoint(int vpoint_id)
	{
		unsigned long pos;
		t_ssp_3d_scene_viewpoint *p = m_ViewPoints.FetchFirst(pos);
		while(p)
		{
			if(p->vpoint_id == vpoint_id)
				return p;
			p = m_ViewPoints.FetchNext(pos);
		}
		return NULL;
	}

	inline t_ssp_3d_scene_viewpoint* SearchDefViewPoint()
	{
		unsigned long pos;
		t_ssp_3d_scene_viewpoint *p = m_ViewPoints.FetchFirst(pos);
		while(p)
		{
			if(p->def_vpoint == 1)
				return p;
			p = m_ViewPoints.FetchNext(pos);
		}
		return NULL;
	}

	int m_iSceneId;
	SPtrList<t_ssp_3d_scene_viewpoint> m_ViewPoints;
};


#endif//__SSP_3D_VIEW_POINT_H__

