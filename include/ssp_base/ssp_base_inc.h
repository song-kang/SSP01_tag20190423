/**
 *
 * 文 件 名 : ssp_base_inc.h
 * 创建日期 : 2015-11-4 16:32
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SSP软件平台基础类头文件
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-11-4	邵凯田　创建文件
 *
 **/

#ifndef __SSP_BASE_INC_H__
#define __SSP_BASE_INC_H__

#define SSP_DBUSED_MYSQL
//#define SSP_DBUSED_PGSQL
#define SSP_DBUSED_ORACLE
//#define SSP_DBUSED_ODBC

#ifdef SSP_DBUSED_MYSQL
#include "SMySQL.h"
#endif
#ifdef SSP_DBUSED_ODBC
#include "SOdbc.h"
#endif
#ifdef SSP_DBUSED_PGSQL
#include "SPoostgres.h"
#endif
#ifdef SSP_DBUSED_ORACLE
#include "SOracle.h"
#endif

// #ifdef QT_CORE_LIB
// #ifdef SSP_GUI_LIB
// #define USED_STATIC_LIB
// #endif
// #else 
// #define USED_STATIC_LIB
// #endif

#ifdef WIN32
  #ifdef _DEBUG
    #pragma comment(lib,"sbased.lib")
    #pragma comment(lib,"sbase_mdbd.lib")
    #ifdef SSP_DBUSED_MYSQL
      #pragma comment(lib,"sbase_mysqld.lib")
    #endif
	#ifdef SSP_DBUSED_ODBC
		#pragma comment(lib,"sbase_odbcd.lib")
	#endif
    #ifdef SSP_DBUSED_PGSQL
      #pragma comment(lib,"sbase_pgsqld.lib")
    #endif
    #ifdef SSP_DBUSED_ORACLE
      #pragma comment(lib,"sbase_oracled.lib")
    #endif
    #pragma comment(lib,"sbase_zipd.lib")
  #else//_DEBUG
    #pragma comment(lib,"sbase.lib")
    #pragma comment(lib,"sbase_mdb.lib")
    #ifdef SSP_DBUSED_MYSQL
      #pragma comment(lib,"sbase_mysql.lib")
    #endif
	#ifdef SSP_DBUSED_ODBC
		#pragma comment(lib,"sbase_odbc.lib")
	#endif
    #ifdef SSP_DBUSED_PGSQL
      #pragma comment(lib,"sbase_pgsql.lib")
    #endif
    #ifdef SSP_DBUSED_ORACLE
      #pragma comment(lib,"sbase_oracle.lib")
    #endif
    #pragma comment(lib,"sbase_zip.lib")
  #endif//_DEBUG
  
  #ifdef SSP_BASE_LIB_STATIC  //静态库，不需要导出、导出
    #define SSP_BASE_EXPORT
  #else //动态库，导出动态库中为导出申明，其他为导入申明
    #ifdef  SSP_BASE_EXPORT_DLL
      #define SSP_BASE_EXPORT __declspec(dllexport)
    #else //SSP_BASE_EXPORT_DLL
      #define SSP_BASE_EXPORT __declspec(dllimport)
      #ifdef _DEBUG
        #pragma comment(lib,"ssp_based.lib")
      #else//_DEBUG
        #pragma comment(lib,"ssp_base.lib")
      #endif//_DEBUG
    #endif//SBASE_EXPORT_DLL
  #endif//SBASE_LIB_STATIC
#else//WIN32
  #define SSP_BASE_EXPORT
#endif//WIN32

// 
// #ifdef USED_STATIC_LIB
// #ifdef WIN32
// #ifdef NDEBUG
// #pragma comment(lib,"sbase.lib")
// #pragma comment(lib,"sbase_mdb.lib")
// #ifdef SSP_DBUSED_MYSQL
// #pragma comment(lib,"sbase_mysql.lib")
// #endif
// #ifdef SSP_DBUSED_ORACLE
// #pragma comment(lib,"sbase_oracle.lib")
// #endif
// #pragma comment(lib,"sbase_zip.lib")
// #pragma comment(lib,"ssp_base.lib")
// #else
// #pragma comment(lib,"sbased.lib")
// #pragma comment(lib,"sbase_mdbd.lib")
// #ifdef SSP_DBUSED_MYSQL
// #pragma comment(lib,"sbase_mysqld.lib")
// #endif
// #ifdef SSP_DBUSED_ORACLE
// #pragma comment(lib,"sbase_oracled.lib")
// #endif
// #pragma comment(lib,"sbase_zipd.lib")
// #pragma comment(lib,"ssp_based.lib")
// #endif
// #endif
// 
// #endif//USED_STATIC_LIB

#endif//__SSP_BASE_INC_H__
