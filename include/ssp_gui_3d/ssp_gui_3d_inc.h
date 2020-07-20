/**
 *
 * 文 件 名 : ssp_gui_3d_inc.h
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

#ifndef __SSP_GUI_3D_INCLUDE_H__
#define __SSP_GUI_3D_INCLUDE_H__


#ifdef WIN32
  #ifdef _DEBUG
    #pragma comment(lib,"sbased.lib")
    #pragma comment(lib,"sbase_mdbd.lib")
    #ifdef SSP_DBUSED_MYSQL
      #pragma comment(lib,"sbase_mysqld.lib")
    #endif
    #ifdef SSP_DBUSED_PGSQL
      #pragma comment(lib,"sbase_pgsqld.lib")
    #endif
    #ifdef SSP_DBUSED_ORACLE
      #pragma comment(lib,"sbase_oracled.lib")
    #endif
    #pragma comment(lib,"sbase_zipd.lib")
    #pragma comment(lib,"sbase_qtguid.lib")
    #pragma comment(lib,"ssp_based.lib")
    #pragma comment(lib,"ssp_guid.lib")
  #else//_DEBUG
    #pragma comment(lib,"sbase.lib")
    #pragma comment(lib,"sbase_mdb.lib")
    #ifdef SSP_DBUSED_MYSQL
      #pragma comment(lib,"sbase_mysql.lib")
    #endif
    #ifdef SSP_DBUSED_PGSQL
      #pragma comment(lib,"sbase_pgsql.lib")
    #endif
    #ifdef SSP_DBUSED_ORACLE
      #pragma comment(lib,"sbase_oracle.lib")
    #endif
    #pragma comment(lib,"sbase_zip.lib")
    #pragma comment(lib,"sbase_qtgui.lib")
    #pragma comment(lib,"ssp_base.lib")
    #pragma comment(lib,"ssp_gui.lib")
  #endif//_DEBUG

  #ifdef SSP_GUI_3D_LIB_STATIC  //静态库，不需要导出、导出
    #define SSP_GUI_3D_EXPORT
  #else //动态库，导出动态库中为导出申明，其他为导入申明
    #ifdef  SSP_GUI_3D_EXPORT_DLL
      #define SSP_GUI_3D_EXPORT __declspec(dllexport)
    #else //SSP_GUI_3D_EXPORT_DLL
      #define SSP_GUI_3D_EXPORT __declspec(dllimport)
      #ifdef _DEBUG
        #pragma comment(lib,"ssp_gui_3dd.lib")
      #else//_DEBUG
        #pragma comment(lib,"ssp_gui_3d.lib")
      #endif//_DEBUG
    #endif//SBASE_EXPORT_DLL
  #endif//SBASE_LIB_STATIC
#else//WIN32
  #define SSP_GUI_3D_EXPORT
#endif//WIN32

#endif//__SSP_GUI_3D_INCLUDE_H__
