/**
 *
 * 文 件 名 : ssp_3d_object.cpp
 * 创建日期 : 2017-3-7 14:05
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 对象ID标识类定义
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2017-3-7	邵凯田　创建文件
 *
 **/

#include "ssp_3d_object.h"

CSsp3DObject::CSsp3DObject(int sn,int id1,int id2,int id3,int id4)
{
	m_iObjSn = sn;
	m_iId1 = id1;
	m_iId2 = id2;
	m_iId3 = id3;
	m_iId4 = id4;
}

CSsp3DObject::~CSsp3DObject()
{

}


CSsp3DObjectMgr::CSsp3DObjectMgr()
{
	m_lst3DObj.setAutoDelete(true);
	m_iLastObjSn = 1;
}

CSsp3DObjectMgr::~CSsp3DObjectMgr()
{

}


//////////////////////////////////////////////////////////////////////////
// 描    述:  通过唯一序号快速查找对象
// 作    者:  邵凯田
// 创建时间:  2017-3-7 14:22
// 参数说明:  @obj_sn
// 返 回 值:  CSsp3DObject*
//////////////////////////////////////////////////////////////////////////
CSsp3DObject* CSsp3DObjectMgr::SearchObjectBySn(int obj_sn)
{
	SSP3D_MAP_OBJECT_SN::iterator it = m_map3DObj.find(obj_sn);
	if(it == m_map3DObj.end())
	{
		return NULL;
	}
	return it->second;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  通过模型映射用的4个ID值查找对象
// 作    者:  邵凯田
// 创建时间:  2017-3-7 14:23
// 参数说明:  @id1/id2/id3/id4
// 返 回 值:  CSsp3DObject*
//////////////////////////////////////////////////////////////////////////
CSsp3DObject* CSsp3DObjectMgr::SearchObjectById(int id1,int id2,int id3,int id4)
{
	unsigned long pos;
	CSsp3DObject *p = m_lst3DObj.FetchFirst(pos);
	while(p)
	{
		if(p->m_iId1 == id1 || p->m_iId2 == id2 || p->m_iId3 == id3 || p->m_iId4 == id4)
			return p;
		p = m_lst3DObj.FetchNext(pos);
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  创建一个新的对象，并返回对象指针
// 作    者:  邵凯田
// 创建时间:  2017-3-7 14:24
// 参数说明:  @id1/id2/id3/id4
// 返 回 值:  CSsp3DObject*
//////////////////////////////////////////////////////////////////////////
CSsp3DObject* CSsp3DObjectMgr::NewObject(int id1,int id2,int id3,int id4)
{
	CSsp3DObject *pObj = SearchObjectById(id1,id2,id3,id4);
	if(pObj != NULL)
		return pObj;
	pObj = new CSsp3DObject(m_iLastObjSn,id1,id2,id3,id4);
	m_lst3DObj.append(pObj);
	m_map3DObj[m_iLastObjSn] = pObj;
	m_iLastObjSn ++;
	return pObj;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  删除一个指定标识的对象
// 作    者:  邵凯田
// 创建时间:  2017-3-7 14:39
// 参数说明:  @id1/id2/id3/id4
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CSsp3DObjectMgr::RemoveObject(int id1,int id2,int id3,int id4)
{
	CSsp3DObject *pObj = SearchObjectById(id1,id2,id3,id4);
	if(pObj == NULL)
		return false;
	m_map3DObj.erase(pObj->m_iObjSn);
	m_lst3DObj.remove(pObj);
	return true;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  清除所有对象内容,并重置SN号
// 作    者:  邵凯田
// 创建时间:  2017-3-7 14:37
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp3DObjectMgr::Clear()
{
	m_map3DObj.clear();
	m_lst3DObj.clear();
	m_iLastObjSn = 1;
}
