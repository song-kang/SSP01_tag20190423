/**
 *
 * 文 件 名 : ssp_gui_inc.h
 * 创建日期 : 2015-7-8 11:02
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SSP图形人机对话程序（工作站）软件平台接口类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-8	邵凯田　创建文件
 *
 **/
#ifndef SSP_GUI_INC_H
#define SSP_GUI_INC_H

// #ifdef SSP_GUI_LIB
// #ifdef WIN32
// #ifdef NDEBUG
// #pragma comment(lib,"sbase_qtgui.lib")
// //#pragma comment(lib,"ssp_gui.lib")
// #else
// #pragma comment(lib,"sbase_qtguid.lib")
// //#pragma comment(lib,"ssp_guid.lib")
// #endif
// #endif
// #define SSP_GUI_EXPORT __declspec(dllexport)
// 
// #else//SSP_GUI_LIB
// 
// #define SSP_GUI_EXPORT __declspec(dllimport)
// 
// #endif//SSP_GUI_LIB


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
  #endif//_DEBUG
  
  #ifdef SSP_GUI_LIB_STATIC  //静态库，不需要导出、导出
    #define SSP_GUI_EXPORT
  #else //动态库，导出动态库中为导出申明，其他为导入申明
    #ifdef  SSP_GUI_EXPORT_DLL
      #define SSP_GUI_EXPORT __declspec(dllexport)
    #else //SSP_GUI_EXPORT_DLL
      #define SSP_GUI_EXPORT __declspec(dllimport)
      #ifdef _DEBUG
        #pragma comment(lib,"ssp_guid.lib")
      #else//_DEBUG
        #pragma comment(lib,"ssp_gui.lib")
      #endif//_DEBUG
    #endif//SBASE_EXPORT_DLL
  #endif//SBASE_LIB_STATIC
#else//WIN32
#define SSP_GUI_EXPORT
#endif//WIN32

#endif // SSP_GUI_INC_H
