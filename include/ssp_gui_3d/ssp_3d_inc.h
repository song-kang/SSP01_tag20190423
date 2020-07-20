/**
 *
 * 文 件 名 : ssp_3d_inc.h
 * 创建日期 : 2017-3-2 10:35
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SSP 3D 基础定义头文件
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2017-3-2	邵凯田　创建文件
 *
 **/

#ifndef __SSP_3D_INCLUDE_H__
#define __SSP_3D_INCLUDE_H__

#include "ssp_3d_mdb.h"
#include "ssp_3d_object.h"
#include "ssp_gui_3d_inc.h"

enum eSsp3DPickActionMode
{
	SSP_3D_PICK_MOD_NONE=0,		//0：不用于场景拾取
	SSP_3D_PICK_MOD_MOVE=1,		//1：切换视角到子场景的视点
	SSP_3D_PICK_MOD_INTO=2,		//2：进入子场景显示
	SSP_3D_PICK_MOD_CALLBACK=3,	//3：回调应用层处理
};

enum eSsp3DPickHoverMode
{
	SSP_3D_PICK_HOVER_NONE=0,		//0：无额外绘制
	SSP_3D_PICK_HOVER_BOX=1,		//1：绘制半透明拾取盒子
	SSP_3D_PICK_HOVER_NAME=2,		//2：鼠标位置绘制拾取对象名称
	SSP_3D_PICK_HOVER_CALLBACK=3,	//3：回调应用层处理
};


enum eSsp3DDrawType
{
	SSP_3D_DRAW_LINE=1,		//1-直线段
	SSP_3D_DRAW_RECT=2,		//2-平面矩形
	SSP_3D_DRAW_TRIANGLE=3,	//3-平面三角形
	SSP_3D_DRAW_CIRCULAR=4,	//4-平面圆形 
	SSP_3D_DRAW_TEXT=5,		//5-平面文字
	SSP_3D_DRAW_BOX=6,		//6-长方体
	SSP_3D_DRAW_CYLINDER=7,	//7-圆柱体
	SSP_3D_DRAW_CONE=8,		//8-圆锥体
	SSP_3D_DRAW_BALL=9,		//9-球体
	SSP_3D_DRAW_PREDEF=101,	//101-预定义对象
	SSP_3D_DRAW_CALLBACK=102,//102-实时回调绘制物体
};

enum eSsp3DTextureType
{
	SSP_3D_TEXTURE_BMP=1,
	SSP_3D_TEXTURE_COLOR=2,
};



#endif//__SSP_3D_INCLUDE_H__
