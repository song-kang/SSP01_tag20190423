/**
 *
 * 文 件 名 : include_macdef.h
 * 创建日期 : 2015-8-3 9:20
 * 作    者 : 褚冰冰
 * 修改日期 : 2015-8-3 9:20
 * 当前版本 : 1.0
 * 功能描述 : 包含头文件、添加自定义宏
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-8-3 	褚冰冰　添加自定义宏
 *
 **/
#ifndef _HEADER_H_
#define _HEADER_H_

#include "ssp_database.h"
#include <QMessageBox>

#define BOOLCHECK(fun,info)\
	if(!fun)\
	{\
	QMessageBox::information(NULL,"tip",info);\
	}

#define BOOLCHECKRETURN(fun,info,v_return)\
	if(!fun)\
	{\
		QMessageBox::information(NULL,"tip",info);\
		return v_return;\
	}

class _storeBoolInfo
{
public:
	_storeBoolInfo():m_err(false){}
	~_storeBoolInfo(){}
	void check(SString sInfo)
	{
		BOOLCHECK((!m_err),QObject::tr((m_sInfo+sInfo).data()))
	}
	void addInfo(SString sInfo)
	{
		m_err=true;
		m_sInfo+=sInfo+",";
	}
private:
	bool m_err;
	SString m_sInfo;
};
#define FALSECHECKINFOSTART\
	_storeBoolInfo check_storeBoolInfo;

#define FALSECHECKINFORECORD(fun,info)\
	if (!fun)\
	{\
		check_storeBoolInfo.addInfo(info);\
	}

#define FALSECHECKINFOEND(info)\
	check_storeBoolInfo.check(info);

#define USE_MYSQL
#include <vector>
using namespace std;
#endif