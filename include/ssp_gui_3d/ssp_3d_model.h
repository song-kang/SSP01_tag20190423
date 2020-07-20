/**
 *
 * 文 件 名 : ssp_3d_model.h
 * 创建日期 : 2017-3-3 15:08
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SSP 3D模型相关类封装
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2017-3-3	邵凯田　创建文件
 *
 **/

#ifndef __SSP_3D_MODEL_H__
#define __SSP_3D_MODEL_H__

#include "ssp_base.h"
#include "ssp_3d_mdb.h"
#include "ssp_3d_scene.h"
#include "ssp_3d_predefine.h"
#include "ssp_3d_texture.h"
#include "ssp_3d_viewpoint.h"
#include "ssp_3d_vrml.h"
#include <QGLWidget>

class SSP_GUI_3D_EXPORT CSsp3DModel
{
public:
	CSsp3DModel();
	~CSsp3DModel();

	//////////////////////////////////////////////////////////////////////////
	// 名    称:  通过数据库加载初始模型
	// 作    者:  邵凯田
	// 创建时间:  2017-3-3 15:12
	// 描    述:  @pDb为数据库操作类指针
	//////////////////////////////////////////////////////////////////////////
	bool LoadByDb(SDatabaseOper *pDb,bool bLoadTexture = true,bool vboUse = false);

	void SetGlWidget(QGLWidget *pWnd){m_pGlWidget = pWnd;};

	//////////////////////////////////////////////////////////////////////////
	// 名    称:  根据纹理编号查找指定的纹理
	// 作    者:  邵凯田
	// 创建时间:  2017-3-1 19:12
	// 描    述:  CSsp3DTexture* ，NULL表示找不到
	//////////////////////////////////////////////////////////////////////////
	inline CSsp3DTexture* SearchTexture(int tex_id)
	{
		unsigned long pos;
		CSsp3DTexture *p = m_Texures.FetchFirst(pos);
		while(p)
		{
			if(p->m_record.texture_id == tex_id)
				return p;
			p = m_Texures.FetchNext(pos);
		}
		return NULL;
	}
	inline CSsp3DScene* SearchScene(int scene_id)
	{
		unsigned long pos;
		CSsp3DScene *p = m_Scenes.FetchFirst(pos);
		while(p)
		{
			if(p->m_record.scene_id == scene_id)
				return p;
			p = m_Scenes.FetchNext(pos);
		}
		return NULL;
	}
	inline CSsp3DPreDefine* SearchPreDefine(int pre_id)
	{
		unsigned long pos;
		CSsp3DPreDefine *p = m_PreDefines.FetchFirst(pos);
		while(p)
		{
			if(p->m_record.pre_id == pre_id)
				return p;
			p = m_PreDefines.FetchNext(pos);
		}
		return NULL;
	}
	inline CSsp3DSceneTypeViewPoint* SearchSceneTypeVP(int scene_type)
	{
		unsigned long pos;
		CSsp3DSceneTypeViewPoint *p = m_SceneTypeViewPoints.FetchFirst(pos);
		while(p)
		{
			if(p->m_iSceneType == scene_type)
				return p;
			p = m_SceneTypeViewPoints.FetchNext(pos);
		}
		return NULL;
	}
	inline CSsp3DSceneViewPoint* SearchSceneVP(int scene_id)
	{
		unsigned long pos;
		CSsp3DSceneViewPoint *p = m_SceneViewPoints.FetchFirst(pos);
		while(p)
		{
			if(p->m_iSceneId == scene_id)
				return p;
			p = m_SceneViewPoints.FetchNext(pos);
		}
		return NULL;
	}
	inline CSsp3DVrml* SearchVrml(int rvml_id)
	{
		unsigned long pos;
		CSsp3DVrml *p = m_Vrmls.FetchFirst(pos);
		while(p)
		{
			if(p->m_record.vrml_id == rvml_id)
				return p;
			p = m_Vrmls.FetchNext(pos);
		}
		return NULL;
	}
protected:

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  针对内存数据库时的记录更新回调处理函数，包括记录更新、插入、删除和表截断
	// 作    者:  邵凯田
	// 创建时间:  2017-3-3 15:25
	// 参数说明:  @cbParam为当前记录集的对象指针
	//         :  @sTable为表名
	//         :  @eType为触发类型
	//         :  @iTrgRows为触发行数
	//         :  @iRowSize为每行的记录长度
	//         :  @pTrgData为触发数据内容，总有效长度为iTrgRows*iRowSize
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static BYTE* OnMdbTrgCallback(void* cbParam, SString &sTable, eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  纹理数据库变化处理函数
	// 作    者:  邵凯田
	// 创建时间:  2017-3-3 17:09
	// 参数说明:  @pRow为行内容
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void OnInsertTexture(t_ssp_3d_texture *pRow,bool bLoadTexture = true);
	void OnUpdateTexture(t_ssp_3d_texture *pRow);
	void OnDeleteTexture(t_ssp_3d_texture *pRow);
	void OnTruncateTexture();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  预定义物体数据库变化处理函数
	// 作    者:  邵凯田
	// 创建时间:  2017-3-3 17:09
	// 参数说明:  @pRow为行内容
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void OnInsertPreDefine(t_ssp_3d_predefine *pRow);
	void OnUpdatePreDefine(t_ssp_3d_predefine *pRow);
	void OnDeletePreDefine(t_ssp_3d_predefine *pRow);
	void OnTruncatePreDefine();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  预定义对象数据库变化处理函数
	// 作    者:  邵凯田
	// 创建时间:  2017-3-3 17:09
	// 参数说明:  @pRow为行内容
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void OnInsertPreDefineObj(t_ssp_3d_predefine_obj *pRow);
	void OnUpdatePreDefineObj(t_ssp_3d_predefine_obj *pRow);
	void OnDeletePreDefineObj(t_ssp_3d_predefine_obj *pRow);
	void OnTruncatePreDefineObj();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  场景数据库变化处理函数
	// 作    者:  邵凯田
	// 创建时间:  2017-3-3 17:09
	// 参数说明:  @pRow为行内容
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void OnInsertScene(t_ssp_3d_scene *pRow);
	void OnUpdateScene(t_ssp_3d_scene *pRow);
	void OnDeleteScene(t_ssp_3d_scene *pRow);
	void OnTruncateScene();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  场景对象数据库变化处理函数
	// 作    者:  邵凯田
	// 创建时间:  2017-3-3 17:09
	// 参数说明:  @pRow为行内容
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void OnInsertSceneObj(t_ssp_3d_scene_object *pRow);
	void OnUpdateSceneObj(t_ssp_3d_scene_object *pRow);
	void OnDeleteSceneObj(t_ssp_3d_scene_object *pRow);
	void OnTruncateSceneObj();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  场景类型视点数据库变化处理函数
	// 作    者:  邵凯田
	// 创建时间:  2017-3-3 17:09
	// 参数说明:  @pRow为行内容
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void OnInsertSceneTypeVP(t_ssp_3d_scene_type_viewpoint *pRow);
	void OnUpdateSceneTypeVP(t_ssp_3d_scene_type_viewpoint *pRow);
	void OnDeleteSceneTypeVP(t_ssp_3d_scene_type_viewpoint *pRow);
	void OnTruncateSceneTypeVP();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  场景视点数据库变化处理函数
	// 作    者:  邵凯田
	// 创建时间:  2017-3-3 17:09
	// 参数说明:  @pRow为行内容
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void OnInsertSceneVP(t_ssp_3d_scene_viewpoint *pRow);
	void OnUpdateSceneVP(t_ssp_3d_scene_viewpoint *pRow);
	void OnDeleteSceneVP(t_ssp_3d_scene_viewpoint *pRow);
	void OnTruncateSceneVP();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  VRML数据库变化处理函数
	// 作    者:  邵凯田
	// 创建时间:  2017-3-3 17:09
	// 参数说明:  @pRow为行内容
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void OnInsertVrml(t_ssp_3d_vrml *pRow,bool bLoadVrml,bool vboUse);
	void OnUpdateVrml(t_ssp_3d_vrml *pRow,bool bLoadVrml,bool vboUse);
	void OnDeleteVrml(t_ssp_3d_vrml *pRow);
	void OnTruncateVrml();

public:
	//顶级场景集合
	SPtrList<CSsp3DScene> m_Scenes;
	//3D纹理集合
	SPtrList<CSsp3DTexture> m_Texures;
	//预定义物体集合
	SPtrList<CSsp3DPreDefine> m_PreDefines;
	//场景类型视点集合
	SPtrList<CSsp3DSceneTypeViewPoint> m_SceneTypeViewPoints;
	//场景视点集合
	SPtrList<CSsp3DSceneViewPoint> m_SceneViewPoints;
	//VRML集合
	SPtrList<CSsp3DVrml> m_Vrmls;

private:
	//内存库回调用的客户端实例
	CMdbClient *m_pMdbClient;
	//当前类的正在工作的线程数量
	int m_iThreads;
	//模型访问锁，用于读写模型互折
	SLock m_Lock;

	QGLWidget *m_pGlWidget;
	CSsp3DObjectMgr m_ObjectMgr;//有名对象管理器
};

#endif//__SSP_3D_MODEL_H__
