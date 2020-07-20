/**
 *
 * 文 件 名 : ssp_3d_object.h
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

#ifndef __SSP_3D_OBJECT_H__
#define __SSP_3D_OBJECT_H__

#include "SList.h"
#include "ssp_gui_3d_inc.h"
#include <map>

class SSP_GUI_3D_EXPORT CSsp3DObject
{
public:
	CSsp3DObject(int sn,int id1,int id2,int id3,int id4);
	~CSsp3DObject();

	int m_iObjSn;//对象唯一编号

	int m_iId1;//对象模型对应的编号
	int m_iId2;
	int m_iId3;
	int m_iId4;
};

typedef std::map<int,CSsp3DObject*> SSP3D_MAP_OBJECT_SN;

class SSP_GUI_3D_EXPORT CSsp3DObjectMgr
{
public:
	CSsp3DObjectMgr();
	~CSsp3DObjectMgr();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  通过唯一序号快速查找对象
	// 作    者:  邵凯田
	// 创建时间:  2017-3-7 14:22
	// 参数说明:  @obj_sn
	// 返 回 值:  CSsp3DObject*
	//////////////////////////////////////////////////////////////////////////
	CSsp3DObject* SearchObjectBySn(int obj_sn);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  通过模型映射用的4个ID值查找对象
	// 作    者:  邵凯田
	// 创建时间:  2017-3-7 14:23
	// 参数说明:  @id1/id2/id3/id4
	// 返 回 值:  CSsp3DObject*
	//////////////////////////////////////////////////////////////////////////
	CSsp3DObject* SearchObjectById(int id1,int id2,int id3,int id4);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  创建一个新的对象，并返回对象指针
	// 作    者:  邵凯田
	// 创建时间:  2017-3-7 14:24
	// 参数说明:  @id1/id2/id3/id4
	// 返 回 值:  CSsp3DObject*
	//////////////////////////////////////////////////////////////////////////
	CSsp3DObject* NewObject(int id1,int id2,int id3,int id4);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  删除一个指定标识的对象
	// 作    者:  邵凯田
	// 创建时间:  2017-3-7 14:39
	// 参数说明:  @id1/id2/id3/id4
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool RemoveObject(int id1,int id2,int id3,int id4);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  清除所有对象内容,并重置SN号
	// 作    者:  邵凯田
	// 创建时间:  2017-3-7 14:37
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void Clear();
private:
	SPtrList<CSsp3DObject> m_lst3DObj;
	SSP3D_MAP_OBJECT_SN m_map3DObj;
	int m_iLastObjSn;
};

#endif//__SSP_3D_OBJECT_H__
