#ifndef PLUGIN_TEST1_H
#define PLUGIN_TEST1_H

#include "ssp_gui.h"
#include "ssp_plugin.h"


extern "C" {

//////////////////////////////////////////////////////////////////////////
// 描    述:  初始化插件
// 作    者:  邵凯田
// 创建时间:  2017-8-7 18:58
// 参数说明:  @sExtAttr表示扩展属性字符串，作保留参数，暂时为空串
// 返 回 值:  int, 0表示成功，其他表示失败
//////////////////////////////////////////////////////////////////////////
__declspec(dllexport) int SSP_Init(const char* sExtAttr);

//////////////////////////////////////////////////////////////////////////
// 描    述:  取当前插件的名称
// 作    者:  邵凯田
// 创建时间:  2017-8-4 10:26
// 参数说明:  void
// 返 回 值:  char*, 插件名称
//////////////////////////////////////////////////////////////////////////
__declspec(dllexport) const char* SSP_GetPluginName();

//////////////////////////////////////////////////////////////////////////
// 描    述:  取当前插件的版本号
// 作    者:  邵凯田
// 创建时间:  2017-8-4 10:27
// 参数说明:  void
// 返 回 值:  char*, 插件版本号
//////////////////////////////////////////////////////////////////////////
__declspec(dllexport) const char* SSP_GetPluginVer();

//////////////////////////////////////////////////////////////////////////
// 描    述:  返回当前插件支持的所有功能点名称与描述
// 作    者:  邵凯田
// 创建时间:  2017-8-4 10:22
// 参数说明:  void
// 返 回 值:  char*, 如： ia_homepage=智能告警主界面;ia_report=告警告警简报;
//////////////////////////////////////////////////////////////////////////
__declspec(dllexport) const char* SSP_FunPointSupported();

//////////////////////////////////////////////////////////////////////////
// 描    述:  创建新功能点窗口
// 作    者:  邵凯田
// 创建时间:  2017-8-4 10:20
// 参数说明:  @sFunName为待打开功能点名称
//         :  @parentWidget为父窗口指针，真实类型为QWidget*
// 返 回 值:  返回新打开的窗口指针，真实类型为CBaseView*，NULL表示非本插件对应的功能点
//////////////////////////////////////////////////////////////////////////
__declspec(dllexport) void* SSP_NewView(const char* sFunName,const void* parentWidget);

};


#endif // PLUGIN_TEST1_H
