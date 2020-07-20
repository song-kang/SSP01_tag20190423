/**
 *
 * 文 件 名 : ssp_3d_model.cpp
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

#include "ssp_3d_model.h"

CSsp3DModel::CSsp3DModel()
{
	m_Scenes.setAutoDelete(true);
	m_Texures.setAutoDelete(true);
	m_PreDefines.setAutoDelete(true);
	m_SceneTypeViewPoints.setAutoDelete(true);
	m_SceneViewPoints.setAutoDelete(true);
	m_Vrmls.setAutoDelete(true);
	m_pMdbClient = NULL;
	m_iThreads = 0;
	m_pGlWidget = NULL;
}

CSsp3DModel::~CSsp3DModel()
{
	if(m_pMdbClient != NULL)
	{
		//解除内存触发回调
		m_pMdbClient->RemoveTriggerCallback(OnMdbTrgCallback,this,"t_ssp_3d_predefine",				MDB_TRGFLG_ALL);
		m_pMdbClient->RemoveTriggerCallback(OnMdbTrgCallback,this,"t_ssp_3d_predefine_obj",			MDB_TRGFLG_ALL);
		m_pMdbClient->RemoveTriggerCallback(OnMdbTrgCallback,this,"t_ssp_3d_scene",					MDB_TRGFLG_ALL);
		m_pMdbClient->RemoveTriggerCallback(OnMdbTrgCallback,this,"t_ssp_3d_scene_object",			MDB_TRGFLG_ALL);
		m_pMdbClient->RemoveTriggerCallback(OnMdbTrgCallback,this,"t_ssp_3d_scene_type_viewpoint",	MDB_TRGFLG_ALL);
		m_pMdbClient->RemoveTriggerCallback(OnMdbTrgCallback,this,"t_ssp_3d_scene_viewpoint",		MDB_TRGFLG_ALL);
		m_pMdbClient->RemoveTriggerCallback(OnMdbTrgCallback,this,"t_ssp_3d_texture",				MDB_TRGFLG_ALL);
		SApi::UsSleep(100000);
	}
	//wait all thread exit
	while(m_iThreads > 0)
		SApi::UsSleep(100000);
}

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
BYTE* CSsp3DModel::OnMdbTrgCallback(void* cbParam, SString &sTable, eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData)
{
	CSsp3DModel *pThis = (CSsp3DModel*)cbParam;
	pThis->m_iThreads ++;
	pThis->m_Lock.lock();
	sTable = sTable.toLower();
	int i;
	if(sTable == "t_ssp_3d_predefine")
	{
		t_ssp_3d_predefine *pRow = (t_ssp_3d_predefine*)pTrgData;
		if(iRowSize != sizeof(*pRow))
			return NULL;
		switch(eType)
		{
		case MDB_TRG_INSERT:				//插入触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnInsertPreDefine(pRow++);
			break;
		case MDB_TRG_UPDATE:				//更新触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnUpdatePreDefine(pRow++);
			break;
		case MDB_TRG_DELETE:				//删除触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnDeletePreDefine(pRow++);
			break;
		case MDB_TRG_TRUNCATE:				//清空触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnTruncatePreDefine();
			break;
		}
	}
	else if(sTable == "t_ssp_3d_predefine_obj")
	{
		t_ssp_3d_predefine_obj *pRow = (t_ssp_3d_predefine_obj*)pTrgData;
		if(iRowSize != sizeof(*pRow))
			return NULL;
		switch(eType)
		{
		case MDB_TRG_INSERT:				//插入触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnInsertPreDefineObj(pRow++);
			break;
		case MDB_TRG_UPDATE:				//更新触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnUpdatePreDefineObj(pRow++);
			break;
		case MDB_TRG_DELETE:				//删除触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnDeletePreDefineObj(pRow++);
			break;
		case MDB_TRG_TRUNCATE:				//清空触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnTruncatePreDefineObj();
			break;
		}
	}
	else if(sTable == "t_ssp_3d_scene")
	{
		t_ssp_3d_scene *pRow = (t_ssp_3d_scene*)pTrgData;
		if(iRowSize != sizeof(*pRow))
			return NULL;
		switch(eType)
		{
		case MDB_TRG_INSERT:				//插入触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnInsertScene(pRow++);
			break;
		case MDB_TRG_UPDATE:				//更新触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnUpdateScene(pRow++);
			break;
		case MDB_TRG_DELETE:				//删除触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnDeleteScene(pRow++);
			break;
		case MDB_TRG_TRUNCATE:				//清空触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnTruncateScene();
			break;
		}
	}
	else if(sTable == "t_ssp_3d_scene_object")
	{
		t_ssp_3d_scene_object *pRow = (t_ssp_3d_scene_object*)pTrgData;
		if(iRowSize != sizeof(*pRow))
			return NULL;
		switch(eType)
		{
		case MDB_TRG_INSERT:				//插入触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnInsertSceneObj(pRow++);
			break;
		case MDB_TRG_UPDATE:				//更新触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnUpdateSceneObj(pRow++);
			break;
		case MDB_TRG_DELETE:				//删除触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnDeleteSceneObj(pRow++);
			break;
		case MDB_TRG_TRUNCATE:				//清空触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnTruncateSceneObj();
			break;
		}
	}
	else if(sTable == "t_ssp_3d_scene_type_viewpoint")
	{
		t_ssp_3d_scene_type_viewpoint *pRow = (t_ssp_3d_scene_type_viewpoint*)pTrgData;
		if(iRowSize != sizeof(*pRow))
			return NULL;
		switch(eType)
		{
		case MDB_TRG_INSERT:				//插入触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnInsertSceneTypeVP(pRow++);
			break;
		case MDB_TRG_UPDATE:				//更新触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnUpdateSceneTypeVP(pRow++);
			break;
		case MDB_TRG_DELETE:				//删除触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnDeleteSceneTypeVP(pRow++);
			break;
		case MDB_TRG_TRUNCATE:				//清空触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnTruncateSceneTypeVP();
			break;
		}
	}
	else if(sTable == "t_ssp_3d_scene_viewpoint")
	{
		t_ssp_3d_scene_viewpoint *pRow = (t_ssp_3d_scene_viewpoint*)pTrgData;
		if(iRowSize != sizeof(*pRow))
			return NULL;
		switch(eType)
		{
		case MDB_TRG_INSERT:				//插入触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnInsertSceneVP(pRow++);
			break;
		case MDB_TRG_UPDATE:				//更新触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnUpdateSceneVP(pRow++);
			break;
		case MDB_TRG_DELETE:				//删除触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnDeleteSceneVP(pRow++);
			break;
		case MDB_TRG_TRUNCATE:				//清空触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnTruncateSceneVP();
			break;
		}
	}
	else if(sTable == "t_ssp_3d_texture")
	{
		t_ssp_3d_texture *pRow = (t_ssp_3d_texture*)pTrgData;
		if(iRowSize != sizeof(*pRow))
			return NULL;
		switch(eType)
		{
		case MDB_TRG_INSERT:				//插入触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnInsertTexture(pRow++);
			break;
		case MDB_TRG_UPDATE:				//更新触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnUpdateTexture(pRow++);
			break;
		case MDB_TRG_DELETE:				//删除触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnDeleteTexture(pRow++);
			break;
		case MDB_TRG_TRUNCATE:				//清空触发
			for(i=0;i<iTrgRows;i++)
				pThis->OnTruncateTexture();
			break;
		}
	}
	pThis->m_Lock.unlock();
	pThis->m_iThreads --;
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// 名    称:  通过数据库加载初始模型
// 作    者:  邵凯田
// 创建时间:  2017-3-3 15:12
// 描    述:  @pDb为数据库操作类指针
//////////////////////////////////////////////////////////////////////////
bool CSsp3DModel::LoadByDb(SDatabaseOper *pDb,bool bLoadTexture,bool vboUse)
{
	//清除原有所有对象
	m_Scenes.clear();
	m_Texures.clear();
	m_PreDefines.clear();
	m_SceneTypeViewPoints.clear();
	m_Vrmls.clear();
	
	SRecordset rs_t_ssp_3d_predefine;
	SRecordset rs_t_ssp_3d_predefine_obj;
	SRecordset rs_t_ssp_3d_scene;
	SRecordset rs_t_ssp_3d_scene_object;
	SRecordset rs_t_ssp_3d_scene_type_viewpoint;
	SRecordset rs_t_ssp_3d_scene_viewpoint;
	SRecordset rs_t_ssp_3d_texture;
	SRecordset rs_t_ssp_3d_vrml;
	pDb->Retrieve("select * from t_ssp_3d_texture",rs_t_ssp_3d_texture);
	pDb->Retrieve("select * from t_ssp_3d_scene_type_viewpoint",rs_t_ssp_3d_scene_type_viewpoint);
	pDb->Retrieve("select * from t_ssp_3d_predefine",rs_t_ssp_3d_predefine);
	pDb->Retrieve("select * from t_ssp_3d_predefine_obj order by pre_id asc,obj_id asc",rs_t_ssp_3d_predefine_obj);
	pDb->Retrieve("select * from t_ssp_3d_scene",rs_t_ssp_3d_scene);
	pDb->Retrieve("select * from t_ssp_3d_scene_object order by scene_id asc,obj_id asc",rs_t_ssp_3d_scene_object);
	pDb->Retrieve("select * from t_ssp_3d_scene_viewpoint",rs_t_ssp_3d_scene_viewpoint);
	pDb->Retrieve("select * from t_ssp_3d_vrml",rs_t_ssp_3d_vrml);
	int i,cnt;
	SRecord *pRec;
	m_Lock.lock();
	//rs_t_ssp_3d_texture
	{
		cnt = rs_t_ssp_3d_texture.GetRows();
		t_ssp_3d_texture row;
		for(i=0;i<cnt;i++)
		{
			pRec = rs_t_ssp_3d_texture.GetRecord(i);
			row.setWithRs(pRec);
			OnInsertTexture(&row,bLoadTexture);
		}
	}
	//rs_t_ssp_3d_scene_type_viewpoint
	{
		cnt = rs_t_ssp_3d_scene_type_viewpoint.GetRows();
		t_ssp_3d_scene_type_viewpoint row;
		for(i=0;i<cnt;i++)
		{
			pRec = rs_t_ssp_3d_scene_type_viewpoint.GetRecord(i);
			row.setWithRs(pRec);
			OnInsertSceneTypeVP(&row);
		}
	}
	//rs_t_ssp_3d_predefine
	{
		cnt = rs_t_ssp_3d_predefine.GetRows();
		t_ssp_3d_predefine row;
		for(i=0;i<cnt;i++)
		{
			pRec = rs_t_ssp_3d_predefine.GetRecord(i);
			row.setWithRs(pRec);
			OnInsertPreDefine(&row);
		}
	}
	//rs_t_ssp_3d_predefine_obj
	{
		cnt = rs_t_ssp_3d_predefine_obj.GetRows();
		t_ssp_3d_predefine_obj row;
		for(i=0;i<cnt;i++)
		{
			pRec = rs_t_ssp_3d_predefine_obj.GetRecord(i);
			row.setWithRs(pRec);
			OnInsertPreDefineObj(&row);
		}
	}
	//rs_t_ssp_3d_scene
	{
		cnt = rs_t_ssp_3d_scene.GetRows();
		t_ssp_3d_scene row;
		for(i=0;i<cnt;i++)
		{
			pRec = rs_t_ssp_3d_scene.GetRecord(i);
			row.setWithRs(pRec);
			OnInsertScene(&row);
		}
	}
	//rs_t_ssp_3d_scene_object
	{
		cnt = rs_t_ssp_3d_scene_object.GetRows();
		t_ssp_3d_scene_object row;
		for(i=0;i<cnt;i++)
		{
			pRec = rs_t_ssp_3d_scene_object.GetRecord(i);
			row.setWithRs(pRec);
			OnInsertSceneObj(&row);
		}
	}
	//rs_t_ssp_3d_scene_viewpoint
	{
		cnt = rs_t_ssp_3d_scene_viewpoint.GetRows();
		t_ssp_3d_scene_viewpoint row;
		for(i=0;i<cnt;i++)
		{
			pRec = rs_t_ssp_3d_scene_viewpoint.GetRecord(i);
			row.setWithRs(pRec);
			OnInsertSceneVP(&row);
		}
	}
	//rs_t_ssp_3d_vrml
	{
		cnt = rs_t_ssp_3d_vrml.GetRows();
		t_ssp_3d_vrml row;
		for(i=0;i<cnt;i++)
		{
			pRec = rs_t_ssp_3d_vrml.GetRecord(i);
			row.setWithRs(pRec);
			OnInsertVrml(&row,bLoadTexture,vboUse);
		}
	}
	m_Lock.unlock();

	//注册内存库触发
	SDatabasePool<SDatabase>* pPool;
	SMdb *pMdb;
	if(MDB != NULL && (pPool = MDB->GetDatabasePool()) != NULL 
		&& pPool->GetPoolSize() > 0 && (pMdb = (SMdb*)pPool->GetDatabase()) != NULL)
	{
		m_pMdbClient = pMdb->GetMdbClient();
		m_pMdbClient->RegisterTriggerCallback(OnMdbTrgCallback,this,"t_ssp_3d_predefine",				MDB_TRGFLG_ALL);
		m_pMdbClient->RegisterTriggerCallback(OnMdbTrgCallback,this,"t_ssp_3d_predefine_obj",			MDB_TRGFLG_ALL);
		m_pMdbClient->RegisterTriggerCallback(OnMdbTrgCallback,this,"t_ssp_3d_scene",					MDB_TRGFLG_ALL);
		m_pMdbClient->RegisterTriggerCallback(OnMdbTrgCallback,this,"t_ssp_3d_scene_object",			MDB_TRGFLG_ALL);
		m_pMdbClient->RegisterTriggerCallback(OnMdbTrgCallback,this,"t_ssp_3d_scene_type_viewpoint",	MDB_TRGFLG_ALL);
		m_pMdbClient->RegisterTriggerCallback(OnMdbTrgCallback,this,"t_ssp_3d_scene_viewpoint",			MDB_TRGFLG_ALL);
		m_pMdbClient->RegisterTriggerCallback(OnMdbTrgCallback,this,"t_ssp_3d_texture",					MDB_TRGFLG_ALL);
		pPool->Release(pMdb);
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  纹理数据库变化处理函数
// 作    者:  邵凯田
// 创建时间:  2017-3-3 17:09
// 参数说明:  @pRow为行内容
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp3DModel::OnInsertTexture(t_ssp_3d_texture *pRow,bool bLoadTexture)
{
	CSsp3DTexture *p;
	if((p=SearchTexture(pRow->texture_id)) != NULL)
	{
		//当前对象已经存在，更新
		OnUpdateTexture(pRow);
		return;
	}
	p = new CSsp3DTexture(pRow);
	p->SetGlWidget(m_pGlWidget);
	p->LoadTexture(bLoadTexture);
	m_Texures.append(p);
}
void CSsp3DModel::OnUpdateTexture(t_ssp_3d_texture *pRow)
{
	CSsp3DTexture *p;
	if((p=SearchTexture(pRow->texture_id)) == NULL)
	{
		LOGWARN("当前纹理%d已经不存在!",pRow->texture_id);
		return;
	}
	memcpy(&p->m_record,pRow,sizeof(*pRow));
	p->LoadTexture();
}
void CSsp3DModel::OnDeleteTexture(t_ssp_3d_texture *pRow)
{
	CSsp3DTexture *p;
	if((p=SearchTexture(pRow->texture_id)) == NULL)
	{
		LOGWARN("当前纹理%d已经不存在!",pRow->texture_id);
		return;
	}
	m_Texures.remove(p);
}
void CSsp3DModel::OnTruncateTexture()
{
	unsigned long pos;
	CSsp3DTexture *p = m_Texures.FetchFirst(pos);
	while(p)
	{
		OnDeleteTexture(&p->m_record);
		p = m_Texures.FetchNext(pos);
	}
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  预定义物体数据库变化处理函数
// 作    者:  邵凯田
// 创建时间:  2017-3-3 17:09
// 参数说明:  @pRow为行内容
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp3DModel::OnInsertPreDefine(t_ssp_3d_predefine *pRow)
{
	CSsp3DPreDefine *p = SearchPreDefine(pRow->pre_id);
	if(p != NULL)
	{
		//当前对象已经存在，更新
		OnUpdatePreDefine(pRow);
		return;
	}
	p = new CSsp3DPreDefine(pRow);
	m_PreDefines.append(p);
}
void CSsp3DModel::OnUpdatePreDefine(t_ssp_3d_predefine *pRow)
{
	CSsp3DPreDefine *p = SearchPreDefine(pRow->pre_id);
	if(p == NULL)
	{
		//当前对象不存在
		return;
	}
	memcpy(&p->m_record,pRow,sizeof(*pRow));
}
void CSsp3DModel::OnDeletePreDefine(t_ssp_3d_predefine *pRow)
{
	CSsp3DPreDefine *p = SearchPreDefine(pRow->pre_id);
	if(p == NULL)
	{
		//当前对象不存在
		return;
	}
	m_PreDefines.remove(p);
}
void CSsp3DModel::OnTruncatePreDefine()
{
	unsigned long pos;
	CSsp3DPreDefine *p = m_PreDefines.FetchFirst(pos);
	while(p)
	{
		OnDeletePreDefine(&p->m_record);
		p = m_PreDefines.FetchNext(pos);
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  预定义对象数据库变化处理函数
// 作    者:  邵凯田
// 创建时间:  2017-3-3 17:09
// 参数说明:  @pRow为行内容
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp3DModel::OnInsertPreDefineObj(t_ssp_3d_predefine_obj *pRow)
{
	CSsp3DPreDefine *p0 = SearchPreDefine(pRow->pre_id);
	CSsp3DPreDefineObj *p = (p0==NULL)?NULL:p0->SearchObj(pRow->obj_id);
	if(p0 == NULL)
		return;//无效的预定义对象父记录
	if(p != NULL)
	{
		//当前对象已经存在，更新
		OnUpdatePreDefineObj(pRow);
		return;
	}
	p = new CSsp3DPreDefineObj(pRow);
	if (pRow->draw_type == 5) //文字时加图像
		p->LoadTexture();
	else
		p->m_pTexture = SearchTexture(pRow->texture_id);
	p0->m_PreObjs.append(p);
}
void CSsp3DModel::OnUpdatePreDefineObj(t_ssp_3d_predefine_obj *pRow)
{
	CSsp3DPreDefine *p0 = SearchPreDefine(pRow->pre_id);
	CSsp3DPreDefineObj *p = (p0==NULL)?NULL:p0->SearchObj(pRow->obj_id);
	if(p0 == NULL)
		return;//无效的预定义对象父记录
	if(p == NULL)
	{
		//当前记录已经不存在
		return;
	}
	if(p->m_record.texture_id != pRow->texture_id)
	{
		//纹理发生变化
		p->m_pTexture = SearchTexture(pRow->texture_id);
	}
	memcpy(&p->m_record,pRow,sizeof(*pRow));
}
void CSsp3DModel::OnDeletePreDefineObj(t_ssp_3d_predefine_obj *pRow)
{
	CSsp3DPreDefine *p0 = SearchPreDefine(pRow->pre_id);
	CSsp3DPreDefineObj *p = (p0==NULL)?NULL:p0->SearchObj(pRow->obj_id);
	if(p0 == NULL)
		return;//无效的预定义对象父记录
	if(p == NULL)
	{
		//当前记录已经不存在
		return;
	}

	if(p->m_record.draw_type == 5)
	{
		GLuint m_Texures = p->m_record.texture_id;
		glDeleteTextures(1,&m_Texures);
	}

	p0->m_PreObjs.remove(p);
}
void CSsp3DModel::OnTruncatePreDefineObj()
{
	unsigned long pos;
	CSsp3DPreDefine *p = m_PreDefines.FetchFirst(pos);
	while(p)
	{
		p->m_PreObjs.clear();
		p = m_PreDefines.FetchNext(pos);
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  场景数据库变化处理函数
// 作    者:  邵凯田
// 创建时间:  2017-3-3 17:09
// 参数说明:  @pRow为行内容
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp3DModel::OnInsertScene(t_ssp_3d_scene *pRow)
{
	CSsp3DScene *p = SearchScene(pRow->scene_id);
	if(p != NULL)
	{
		//当前记录已经存在，更新
		OnUpdateScene(pRow);
		return;
	}
	p = new CSsp3DScene(pRow);
	if(p->m_record.p_scene_id == 0)
		p->m_pParentScene = NULL;
	else
		p->m_pParentScene = SearchScene(p->m_record.p_scene_id);
	p->m_p3DObject = m_ObjectMgr.NewObject(p->m_record.id1,p->m_record.id2,p->m_record.id3,p->m_record.id4);
	m_Scenes.append(p);
}
void CSsp3DModel::OnUpdateScene(t_ssp_3d_scene *pRow)
{
	CSsp3DScene *p = SearchScene(pRow->scene_id);
	if(p == NULL)
	{
		//当前记录已经不存在
		return;
	}
	if(p->m_record.p_scene_id != pRow->p_scene_id)
		p->m_pParentScene = (pRow->p_scene_id==0)?NULL:SearchScene(pRow->p_scene_id);
	if(p->m_record.id1 != pRow->id1 || p->m_record.id2 != pRow->id2 || p->m_record.id3 != pRow->id3 || p->m_record.id4 != pRow->id4)
		p->m_p3DObject = m_ObjectMgr.NewObject(p->m_record.id1,p->m_record.id2,p->m_record.id3,p->m_record.id4);
	memcpy(&p->m_record,pRow,sizeof(*pRow));
}
void CSsp3DModel::OnDeleteScene(t_ssp_3d_scene *pRow)
{
	CSsp3DScene *p = SearchScene(pRow->scene_id);
	if(p == NULL)
	{
		//当前记录已经不存在
		return;
	}
	//MEMO:如果ID在3D场景中不唯一，则可能出现误删
	m_ObjectMgr.RemoveObject(p->m_record.id1,p->m_record.id2,p->m_record.id3,p->m_record.id4);
	m_Scenes.remove(p);
}
void CSsp3DModel::OnTruncateScene()
{
	unsigned long pos;
	CSsp3DScene *p = m_Scenes.FetchFirst(pos);
	while(p)
	{
		OnDeleteScene(&p->m_record);
		p = m_Scenes.FetchNext(pos);
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  场景对象数据库变化处理函数
// 作    者:  邵凯田
// 创建时间:  2017-3-3 17:09
// 参数说明:  @pRow为行内容
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp3DModel::OnInsertSceneObj(t_ssp_3d_scene_object *pRow)
{
	CSsp3DScene *p0 = SearchScene(pRow->scene_id);
	CSsp3DSceneObj *p = (p0==NULL)?NULL:p0->SearchObj(pRow->obj_id);
	if(p0 == NULL)
		return;//无效的场景ID
	if(p != NULL)
	{
		OnUpdateSceneObj(pRow);
		return;
	}
	p = new CSsp3DSceneObj(pRow);

	if (pRow->draw_type == 5) //文字时加图像
		p->LoadTexture();
	else
		p->m_pTexture = pRow->texture_id==0?NULL:SearchTexture(pRow->texture_id);
	p->m_p3DObject = m_ObjectMgr.NewObject(p->m_record.id1,p->m_record.id2,p->m_record.id3,p->m_record.id4);
	p0->m_Objs.append(p);
}
void CSsp3DModel::OnUpdateSceneObj(t_ssp_3d_scene_object *pRow)
{
	CSsp3DScene *p0 = SearchScene(pRow->scene_id);
	CSsp3DSceneObj *p = (p0==NULL)?NULL:p0->SearchObj(pRow->obj_id);
	if(p0 == NULL)
		return;//无效的场景ID
	if(p == NULL)
	{
		return;
	}
	if(p->m_record.texture_id != pRow->texture_id)
		p->m_pTexture = pRow->texture_id==0?NULL:SearchTexture(pRow->texture_id);
	if(p->m_record.id1 != pRow->id1 || p->m_record.id2 != pRow->id2 || p->m_record.id3 != pRow->id3 || p->m_record.id4 != pRow->id4)
		p->m_p3DObject = m_ObjectMgr.NewObject(p->m_record.id1,p->m_record.id2,p->m_record.id3,p->m_record.id4);
	memcpy(&p->m_record,pRow,sizeof(*pRow));
}
void CSsp3DModel::OnDeleteSceneObj(t_ssp_3d_scene_object *pRow)
{
	CSsp3DScene *p0 = SearchScene(pRow->scene_id);
	CSsp3DSceneObj *p = (p0==NULL)?NULL:p0->SearchObj(pRow->obj_id);
	if(p0 == NULL)
		return;//无效的场景ID
	if(p == NULL)
	{
		return;
	}

	if(p->m_record.draw_type == 5)
	{
		GLuint m_Texures = p->m_record.texture_id;
		glDeleteTextures(1,&m_Texures);
	}

	//MEMO:如果ID在3D场景中不唯一，则可能出现误删
	m_ObjectMgr.RemoveObject(p->m_record.id1,p->m_record.id2,p->m_record.id3,p->m_record.id4);
	p0->m_Objs.remove(p);
}
void CSsp3DModel::OnTruncateSceneObj()
{
	unsigned long pos;
	CSsp3DScene *p = m_Scenes.FetchFirst(pos);
	while(p)
	{
		p->m_Objs.clear();
		p = m_Scenes.FetchNext(pos);
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  场景类型视点数据库变化处理函数
// 作    者:  邵凯田
// 创建时间:  2017-3-3 17:09
// 参数说明:  @pRow为行内容
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp3DModel::OnInsertSceneTypeVP(t_ssp_3d_scene_type_viewpoint *pRow)
{
	CSsp3DSceneTypeViewPoint *p = SearchSceneTypeVP(pRow->scene_type);
	if(p == NULL)
	{
		p = new CSsp3DSceneTypeViewPoint();
		p->m_iSceneType = pRow->scene_type;
		m_SceneTypeViewPoints.append(p);
		return;
	}
	t_ssp_3d_scene_type_viewpoint *pPnt = p->SearchViewPoint(pRow->vpoint_id);
	if(pPnt != NULL)
	{
		OnUpdateSceneTypeVP(pRow);
		return;
	}
	pPnt = new t_ssp_3d_scene_type_viewpoint();
	memcpy(pPnt,pRow,sizeof(*pRow));
	p->m_ViewPoints.append(pPnt);
}
void CSsp3DModel::OnUpdateSceneTypeVP(t_ssp_3d_scene_type_viewpoint *pRow)
{
	CSsp3DSceneTypeViewPoint *p = SearchSceneTypeVP(pRow->scene_type);
	if(p == NULL)
	{
		p = new CSsp3DSceneTypeViewPoint();
		p->m_iSceneType = pRow->scene_type;
		m_SceneTypeViewPoints.append(p);
		return;
	}
	t_ssp_3d_scene_type_viewpoint *pPnt = p->SearchViewPoint(pRow->vpoint_id);
	if(pPnt == NULL)
	{
		return;
	}
	memcpy(pPnt,pRow,sizeof(*pRow));
}
void CSsp3DModel::OnDeleteSceneTypeVP(t_ssp_3d_scene_type_viewpoint *pRow)
{
	CSsp3DSceneTypeViewPoint *p = SearchSceneTypeVP(pRow->scene_type);
	if(p == NULL)
	{
		p = new CSsp3DSceneTypeViewPoint();
		p->m_iSceneType = pRow->scene_type;
		m_SceneTypeViewPoints.append(p);
		return;
	}
	t_ssp_3d_scene_type_viewpoint *pPnt = p->SearchViewPoint(pRow->vpoint_id);
	if(pPnt == NULL)
	{
		return;
	}
	p->m_ViewPoints.remove(pPnt);
}
void CSsp3DModel::OnTruncateSceneTypeVP()
{
	m_SceneTypeViewPoints.clear();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  场景视点数据库变化处理函数
// 作    者:  邵凯田
// 创建时间:  2017-3-3 17:09
// 参数说明:  @pRow为行内容
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp3DModel::OnInsertSceneVP(t_ssp_3d_scene_viewpoint *pRow)
{
	CSsp3DSceneViewPoint *p = SearchSceneVP(pRow->scene_id);
	if(p != NULL)
	{
		OnUpdateSceneVP(pRow);
		return;
	}

	p = new CSsp3DSceneViewPoint();
	p->m_iSceneId = pRow->scene_id;
	m_SceneViewPoints.append(p);

	t_ssp_3d_scene_viewpoint *pPnt = new t_ssp_3d_scene_viewpoint();
	memcpy(pPnt,pRow,sizeof(*pRow));
	p->m_ViewPoints.append(pPnt);
}
void CSsp3DModel::OnUpdateSceneVP(t_ssp_3d_scene_viewpoint *pRow)
{
	CSsp3DSceneViewPoint *p = SearchSceneVP(pRow->scene_id);
	if(p == NULL)
	{
		LOGWARN("更新场景[%d]视点[%d]不存在，无法更新!",pRow->scene_id,pRow->vpoint_id);
		return;
	}
	t_ssp_3d_scene_viewpoint *pPnt = p->SearchViewPoint(pRow->vpoint_id);
	if(pPnt == NULL)
	{
		t_ssp_3d_scene_viewpoint *pPnt = new t_ssp_3d_scene_viewpoint();
		memcpy(pPnt,pRow,sizeof(*pRow));
		p->m_ViewPoints.append(pPnt);
		return;
	}
	memcpy(pPnt,pRow,sizeof(*pRow));
}
void CSsp3DModel::OnDeleteSceneVP(t_ssp_3d_scene_viewpoint *pRow)
{
	CSsp3DSceneViewPoint *p = SearchSceneVP(pRow->scene_id);
	if(p == NULL)
	{
		LOGWARN("删除场景[%d]视点[%d]不存在，无法删除!",pRow->scene_id,pRow->vpoint_id);
		return;
	}
	t_ssp_3d_scene_viewpoint *pPnt = p->SearchViewPoint(pRow->vpoint_id);
	if(pPnt == NULL)
	{
		return;
	}
	p->m_ViewPoints.remove(pPnt);
}
void CSsp3DModel::OnTruncateSceneVP()
{
	m_SceneViewPoints.clear();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  VRML数据库变化处理函数
// 作    者:  邵凯田
// 创建时间:  2017-3-3 17:09
// 参数说明:  @pRow为行内容
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp3DModel::OnInsertVrml(t_ssp_3d_vrml *pRow,bool bLoadVrml,bool vboUse)
{
	CSsp3DVrml *p;
	if((p=SearchVrml(pRow->vrml_id)) != NULL)
	{
		OnUpdateVrml(pRow,bLoadVrml,vboUse);
		return;
	}
	p = new CSsp3DVrml();
	p->SetRecord(pRow);
	if (p->Load(bLoadVrml))
	{
		p->vboSupported = vboUse;
		m_Vrmls.append(p);
	}
}
void CSsp3DModel::OnUpdateVrml(t_ssp_3d_vrml *pRow,bool bLoadVrml,bool vboUse)
{
	CSsp3DVrml *p;
	if((p=SearchVrml(pRow->vrml_id)) == NULL)
	{
		LOGWARN("当前VRML%d已经不存在!",pRow->vrml_id);
		return;
	}
	if (!m_Vrmls.remove(p))
	{
		LOGWARN("更新VRML操作中，删除VRML%d失败!",pRow->vrml_id);
		return;
	}
	p = new CSsp3DVrml();
	p->SetRecord(pRow);
	if (p->Load(bLoadVrml))
	{
		p->vboSupported = vboUse;
		m_Vrmls.append(p);
	}
}
void CSsp3DModel::OnDeleteVrml(t_ssp_3d_vrml *pRow)
{
	CSsp3DVrml *p;
	if((p=SearchVrml(pRow->vrml_id)) == NULL)
	{
		LOGWARN("当前VRML%d已经不存在!",pRow->vrml_id);
		return;
	}
	m_Vrmls.remove(p);
}
void CSsp3DModel::OnTruncateVrml()
{
	unsigned long pos;
	CSsp3DVrml *p = m_Vrmls.FetchFirst(pos);
	while(p)
	{
		OnDeleteVrml(&p->m_record);
		p = m_Vrmls.FetchNext(pos);
	}
}
