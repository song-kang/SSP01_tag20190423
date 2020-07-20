/**
 *
 *添加自定义数据结构
 *
 **/

#ifndef _STRUCT_DEFINE_H_
#define _STRUCT_DEFINE_H_

struct tableWidgetInfo
{
	int m_iFormerRow;//原先行数
	int m_iInsertRow;//新增行数
	int m_iColumn;//列数
	int m_iKey;//主键索引
	bool m_bInsert;//是否新增
};
struct fieldInfo//字段信息
{
	SString m_sName;//字段名
	SString m_sType;//字段类型
	int m_iLen;//字段长度
	int m_iPoint;//浮点长度 非浮点类型为0
	SString m_sReg;//匹配字段值的正则表达式
};

#endif