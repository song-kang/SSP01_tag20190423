/**
 *
 * 文 件 名 : m.h
 * 创建日期 : 2017-03-03 16:52
 * 作    者 : SSP_ASSIST(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author       Comments
 * ---  ----------  -----------  -------------------------------------------
 * 001	2017-03-03  SSP_ASSIST  　由SSP_ASSIST自动生成文件
 *
 **/
#ifndef __SSP_3D_MDB_STRUCT_DEF_MDB_H__
#define __SSP_3D_MDB_STRUCT_DEF_MDB_H__

#include "ssp_database.h"
#include "ssp_gui_3d_inc.h"
#pragma pack(push, Old, 1)

struct SSP_GUI_3D_EXPORT t_ssp_3d_predefine
{
	void setWithRs(SRecord *p)
	{
		pre_id = p->GetValueInt(0);
		SString::strncpy(pre_name,64,p->GetValueStr(1));
		pre_type = p->GetValueInt(2);
		SString::strncpy(pre_desc,512,p->GetValueStr(3));
	}

	int           pre_id;
	char          pre_name[64];
	unsigned char pre_type;
	char          pre_desc[512];
};

struct SSP_GUI_3D_EXPORT t_ssp_3d_predefine_obj
{
	void setWithRs(SRecord *p)
	{
		pre_id = p->GetValueInt(0);
		obj_id = p->GetValueInt(1);
		SString::strncpy(obj_name,64,p->GetValueStr(2));
		obj_type = p->GetValueInt(3);
		draw_type = p->GetValueInt(4);
		//id1 = p->GetValueInt(5);
		//id2 = p->GetValueInt(6);
		//id3 = p->GetValueInt(7);
		//id4 = p->GetValueInt(8);
		predefine_id = p->GetValueInt(9-4);
		SString::strncpy(obj_desc,512,p->GetValueStr(10)-4);
		center_x = p->GetValueFloat(11-4);
		center_y = p->GetValueFloat(12-4);
		center_z = p->GetValueFloat(13-4);
		rotate_x = p->GetValueFloat(14-4);
		rotate_y = p->GetValueFloat(15-4);
		rotate_z = p->GetValueFloat(16-4);
		scale = p->GetValueFloat(17-4);
		texture_id = p->GetValueInt(18-4);
		vertex1_x = p->GetValueFloat(19-4);
		vertex1_y = p->GetValueFloat(20-4);
		vertex1_z = p->GetValueFloat(21-4);
		vertex2_x = p->GetValueFloat(22-4);
		vertex2_y = p->GetValueFloat(23-4);
		vertex2_z = p->GetValueFloat(24-4);
		vertex3_x = p->GetValueFloat(25-4);
		vertex3_y = p->GetValueFloat(26-4);
		vertex3_z = p->GetValueFloat(27-4);
		vertex4_x = p->GetValueFloat(28-4);
		vertex4_y = p->GetValueFloat(29-4);
		vertex4_z = p->GetValueFloat(30-4);
		vertex5_x = p->GetValueFloat(31-4);
		vertex5_y = p->GetValueFloat(32-4);
		vertex5_z = p->GetValueFloat(33-4);
		vertex6_x = p->GetValueFloat(34-4);
		vertex6_y = p->GetValueFloat(35-4);
		vertex6_z = p->GetValueFloat(36-4);
		vertex7_x = p->GetValueFloat(37-4);
		vertex7_y = p->GetValueFloat(38-4);
		vertex7_z = p->GetValueFloat(39-4);
		vertex8_x = p->GetValueFloat(40-4);
		vertex8_y = p->GetValueFloat(41-4);
		vertex8_z = p->GetValueFloat(42-4);
		tex1_x = p->GetValueFloat(43-4);
		tex1_y = p->GetValueFloat(44-4);
		tex2_x = p->GetValueFloat(45-4);
		tex2_y = p->GetValueFloat(46-4);
		tex3_x = p->GetValueFloat(47-4);
		tex3_y = p->GetValueFloat(48-4);
		tex4_x = p->GetValueFloat(49-4);
		tex4_y = p->GetValueFloat(50-4);
		tex5_x = p->GetValueFloat(51-4);
		tex5_y = p->GetValueFloat(52-4);
		tex6_x = p->GetValueFloat(53-4);
		tex6_y = p->GetValueFloat(54-4);
		tex7_x = p->GetValueFloat(55-4);
		tex7_y = p->GetValueFloat(56-4);
		tex8_x = p->GetValueFloat(57-4);
		tex8_y = p->GetValueFloat(58-4);
	}

	int           pre_id;
	short         obj_id;
	char          obj_name[64];
	unsigned char obj_type;
	unsigned char draw_type;
	unsigned char id1;
	unsigned char id2;
	unsigned int  id3;
	unsigned int  id4;
	unsigned char predefine_id;
	char          obj_desc[512];
	float         center_x;
	float         center_y;
	float         center_z;
	float         rotate_x;
	float         rotate_y;
	float         rotate_z;
	float         scale;
	int           texture_id;
	float         vertex1_x;
	float         vertex1_y;
	float         vertex1_z;
	float         vertex2_x;
	float         vertex2_y;
	float         vertex2_z;
	float         vertex3_x;
	float         vertex3_y;
	float         vertex3_z;
	float         vertex4_x;
	float         vertex4_y;
	float         vertex4_z;
	float         vertex5_x;
	float         vertex5_y;
	float         vertex5_z;
	float         vertex6_x;
	float         vertex6_y;
	float         vertex6_z;
	float         vertex7_x;
	float         vertex7_y;
	float         vertex7_z;
	float         vertex8_x;
	float         vertex8_y;
	float         vertex8_z;
	float         tex1_x;
	float         tex1_y;
	float         tex2_x;
	float         tex2_y;
	float         tex3_x;
	float         tex3_y;
	float         tex4_x;
	float         tex4_y;
	float         tex5_x;
	float         tex5_y;
	float         tex6_x;
	float         tex6_y;
	float         tex7_x;
	float         tex7_y;
	float         tex8_x;
	float         tex8_y;
};

struct SSP_GUI_3D_EXPORT t_ssp_3d_scene
{
	void setWithRs(SRecord *p)
	{
		scene_id = p->GetValueInt(0);
		p_scene_id = p->GetValueInt(1);
		SString::strncpy(scene_name,64,p->GetValueStr(2));
		scene_type = p->GetValueInt(3);
		SString::strncpy(scene_desc,512,p->GetValueStr(4));
		id1 = p->GetValueInt(5);
		id2 = p->GetValueInt(6);
		id3 = p->GetValueInt(7);
		id4 = p->GetValueInt(8);
		center_x = p->GetValueFloat(9);
		center_y = p->GetValueFloat(10);
		center_z = p->GetValueFloat(11);
		rotate_x = p->GetValueFloat(12);
		rotate_y = p->GetValueFloat(13);
		rotate_z = p->GetValueFloat(14);
		scale = p->GetValueFloat(15);
		eye_range_minx = p->GetValueFloat(16);
		eye_range_maxx = p->GetValueFloat(17);
		eye_range_miny = p->GetValueFloat(18);
		eye_range_maxy = p->GetValueFloat(19);
		eye_range_minz = p->GetValueFloat(20);
		eye_range_maxz = p->GetValueFloat(21);
	}

	int           scene_id;
	int           p_scene_id;
	char          scene_name[64];
	unsigned char scene_type;
	char          scene_desc[512];
	unsigned char id1;
	unsigned char id2;
	unsigned int  id3;
	unsigned int  id4;
	float         center_x;
	float         center_y;
	float         center_z;
	float         rotate_x;
	float         rotate_y;
	float         rotate_z;
	float         scale;
	float         eye_range_minx;
	float         eye_range_maxx;
	float         eye_range_miny;
	float         eye_range_maxy;
	float         eye_range_minz;
	float         eye_range_maxz;
};

struct SSP_GUI_3D_EXPORT t_ssp_3d_scene_object
{
	void setWithRs(SRecord *p)
	{
		scene_id = p->GetValueInt(0);
		obj_id = p->GetValueInt(1);
		SString::strncpy(obj_name,64,p->GetValueStr(2));
		obj_type = p->GetValueInt(3);
		draw_type = p->GetValueInt(4);
		id1 = p->GetValueInt(5);
		id2 = p->GetValueInt(6);
		id3 = p->GetValueInt(7);
		id4 = p->GetValueInt(8);
		predefine_id = p->GetValueInt(9);
		SString::strncpy(obj_desc,512,p->GetValueStr(10));
		center_x = p->GetValueFloat(11);
		center_y = p->GetValueFloat(12);
		center_z = p->GetValueFloat(13);
		rotate_x = p->GetValueFloat(14);
		rotate_y = p->GetValueFloat(15);
		rotate_z = p->GetValueFloat(16);
		scale = p->GetValueFloat(17);
		texture_id = p->GetValueInt(18);
		vertex1_x = p->GetValueFloat(19);
		vertex1_y = p->GetValueFloat(20);
		vertex1_z = p->GetValueFloat(21);
		vertex2_x = p->GetValueFloat(22);
		vertex2_y = p->GetValueFloat(23);
		vertex2_z = p->GetValueFloat(24);
		vertex3_x = p->GetValueFloat(25);
		vertex3_y = p->GetValueFloat(26);
		vertex3_z = p->GetValueFloat(27);
		vertex4_x = p->GetValueFloat(28);
		vertex4_y = p->GetValueFloat(29);
		vertex4_z = p->GetValueFloat(30);
		vertex5_x = p->GetValueFloat(31);
		vertex5_y = p->GetValueFloat(32);
		vertex5_z = p->GetValueFloat(33);
		vertex6_x = p->GetValueFloat(34);
		vertex6_y = p->GetValueFloat(35);
		vertex6_z = p->GetValueFloat(36);
		vertex7_x = p->GetValueFloat(37);
		vertex7_y = p->GetValueFloat(38);
		vertex7_z = p->GetValueFloat(39);
		vertex8_x = p->GetValueFloat(40);
		vertex8_y = p->GetValueFloat(41);
		vertex8_z = p->GetValueFloat(42);
		tex1_x = p->GetValueFloat(43);
		tex1_y = p->GetValueFloat(44);
		tex2_x = p->GetValueFloat(45);
		tex2_y = p->GetValueFloat(46);
		tex3_x = p->GetValueFloat(47);
		tex3_y = p->GetValueFloat(48);
		tex4_x = p->GetValueFloat(49);
		tex4_y = p->GetValueFloat(50);
		tex5_x = p->GetValueFloat(51);
		tex5_y = p->GetValueFloat(52);
		tex6_x = p->GetValueFloat(53);
		tex6_y = p->GetValueFloat(54);
		tex7_x = p->GetValueFloat(55);
		tex7_y = p->GetValueFloat(56);
		tex8_x = p->GetValueFloat(57);
		tex8_y = p->GetValueFloat(58);
		pick_action_mode = p->GetValueInt(59);
		pick_hover_mode = p->GetValueInt(60);
		pick_box_x1 = p->GetValueFloat(61);
		pick_box_x2 = p->GetValueFloat(62);
		pick_box_y1 = p->GetValueFloat(63);
		pick_box_y2 = p->GetValueFloat(64);
		pick_box_z1 = p->GetValueFloat(65);
		pick_box_z2 = p->GetValueFloat(66);
		texture_off_id = p->GetValueInt(67);
		texture_on_id = p->GetValueInt(68);
	}

	int           scene_id;
	short         obj_id;
	char          obj_name[64];
	unsigned char obj_type;
	unsigned char draw_type;
	unsigned char id1;
	unsigned char id2;
	unsigned int  id3;
	unsigned int  id4;
	unsigned char predefine_id;
	char          obj_desc[512];
	float         center_x;
	float         center_y;
	float         center_z;
	float         rotate_x;
	float         rotate_y;
	float         rotate_z;
	float         scale;
	int           texture_id;
	float         vertex1_x;
	float         vertex1_y;
	float         vertex1_z;
	float         vertex2_x;
	float         vertex2_y;
	float         vertex2_z;
	float         vertex3_x;
	float         vertex3_y;
	float         vertex3_z;
	float         vertex4_x;
	float         vertex4_y;
	float         vertex4_z;
	float         vertex5_x;
	float         vertex5_y;
	float         vertex5_z;
	float         vertex6_x;
	float         vertex6_y;
	float         vertex6_z;
	float         vertex7_x;
	float         vertex7_y;
	float         vertex7_z;
	float         vertex8_x;
	float         vertex8_y;
	float         vertex8_z;
	float         tex1_x;
	float         tex1_y;
	float         tex2_x;
	float         tex2_y;
	float         tex3_x;
	float         tex3_y;
	float         tex4_x;
	float         tex4_y;
	float         tex5_x;
	float         tex5_y;
	float         tex6_x;
	float         tex6_y;
	float         tex7_x;
	float         tex7_y;
	float         tex8_x;
	float         tex8_y;
	unsigned char pick_action_mode;
	unsigned char pick_hover_mode;
	float         pick_box_x1;
	float         pick_box_x2;
	float         pick_box_y1;
	float         pick_box_y2;
	float         pick_box_z1;
	float         pick_box_z2;
	int           texture_off_id;
	int           texture_on_id;
};

struct SSP_GUI_3D_EXPORT t_ssp_3d_scene_type_viewpoint
{
	void setWithRs(SRecord *p)
	{
		scene_type = p->GetValueInt(0);
		vpoint_id = p->GetValueInt(1);
		SString::strncpy(vpoint_name,64,p->GetValueStr(2));
		def_vpoint = p->GetValueInt(3);
		eye_x = p->GetValueFloat(4);
		eye_y = p->GetValueFloat(5);
		eye_z = p->GetValueFloat(6);
		eye_forw_x = p->GetValueFloat(7);
		eye_forw_y = p->GetValueFloat(8);
		eye_forw_z = p->GetValueFloat(9);
		eye_up_x = p->GetValueFloat(10);
		eye_up_y = p->GetValueFloat(11);
		eye_up_z = p->GetValueFloat(12);
		eye_rig_x = p->GetValueFloat(13);
		eye_rig_y = p->GetValueFloat(14);
		eye_rig_z = p->GetValueFloat(15);
		view_angle = p->GetValueFloat(16);
		cent2eye = p->GetValueFloat(17);
	}

	unsigned char scene_type;
	unsigned char vpoint_id;
	char          vpoint_name[64];
	unsigned char def_vpoint;
	float         eye_x;
	float         eye_y;
	float         eye_z;
	float         eye_forw_x;
	float         eye_forw_y;
	float         eye_forw_z;
	float         eye_up_x;
	float         eye_up_y;
	float         eye_up_z;
	float         eye_rig_x;
	float         eye_rig_y;
	float         eye_rig_z;
	float         view_angle;
	float         cent2eye;
};

struct SSP_GUI_3D_EXPORT t_ssp_3d_scene_viewpoint
{
	void setWithRs(SRecord *p)
	{
		scene_id = p->GetValueInt(0);
		vpoint_id = p->GetValueInt(1);
		SString::strncpy(vpoint_name,64,p->GetValueStr(2));
		def_vpoint = p->GetValueInt(3);
		eye_x = p->GetValueFloat(4);
		eye_y = p->GetValueFloat(5);
		eye_z = p->GetValueFloat(6);
		eye_forw_x = p->GetValueFloat(7);
		eye_forw_y = p->GetValueFloat(8);
		eye_forw_z = p->GetValueFloat(9);
		eye_up_x = p->GetValueFloat(10);
		eye_up_y = p->GetValueFloat(11);
		eye_up_z = p->GetValueFloat(12);
		eye_rig_x = p->GetValueFloat(13);
		eye_rig_y = p->GetValueFloat(14);
		eye_rig_z = p->GetValueFloat(15);
		view_angle = p->GetValueFloat(16);
		cent2eye = p->GetValueFloat(17);
		x_angle = p->GetValueFloat(18);
		z_angle = p->GetValueFloat(19);
	}

	int           scene_id;
	unsigned char vpoint_id;
	char          vpoint_name[64];
	unsigned char def_vpoint;
	float         eye_x;
	float         eye_y;
	float         eye_z;
	float         eye_forw_x;
	float         eye_forw_y;
	float         eye_forw_z;
	float         eye_up_x;
	float         eye_up_y;
	float         eye_up_z;
	float         eye_rig_x;
	float         eye_rig_y;
	float         eye_rig_z;
	float         view_angle;
	float         cent2eye;
	float		  x_angle;
	float		  z_angle;
};

struct SSP_GUI_3D_EXPORT t_ssp_3d_texture
{
	void setWithRs(SRecord *p)
	{
		texture_id = p->GetValueInt(0);
		SString::strncpy(tex_name,64,p->GetValueStr(1));
		tex_type = p->GetValueInt(2);
		tex_width = p->GetValueInt(3);
		tex_height = p->GetValueInt(4);
		SString::strncpy(tex_color,16,p->GetValueStr(5));
		SString::strncpy(tex_filename,sizeof(tex_filename),p->GetValueStr(6));
		//MEMO: tex_image is blob type, not support it!
	}

	int           texture_id;
	char          tex_name[64];
	unsigned char tex_type;
	int           tex_width;
	int           tex_height;
	char          tex_color[16];
	char          tex_filename[256];
	unsigned int  tex_image;
};

struct SSP_GUI_3D_EXPORT t_ssp_3d_vrml
{
	void setWithRs(SRecord *p)
	{
		vrml_id = p->GetValueInt(0);
		SString::strncpy(vrml_name,sizeof(vrml_name),p->GetValueStr(1));
		SString::strncpy(vrml_filename,sizeof(vrml_filename),p->GetValueStr(2));
		//MEMO: vrml_content is blob type, not support it!
	}

	int		vrml_id;
	char    vrml_name[128];
	char    vrml_filename[128];
};

#pragma pack(pop, Old)

#endif//__SSP_3D_MDB_STRUCT_DEF_MDB_H__
