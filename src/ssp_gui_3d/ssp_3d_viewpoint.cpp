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

#include "ssp_3d_viewpoint.h"


CSsp3DSceneTypeViewPoint::CSsp3DSceneTypeViewPoint()
{
	m_ViewPoints.setAutoDelete(true);
}

CSsp3DSceneTypeViewPoint::~CSsp3DSceneTypeViewPoint()
{

}

CSsp3DSceneViewPoint::CSsp3DSceneViewPoint()
{
	m_ViewPoints.setAutoDelete(true);
}

CSsp3DSceneViewPoint::~CSsp3DSceneViewPoint()
{

}
