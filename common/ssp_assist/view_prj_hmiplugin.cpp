#include "view_prj_hmiplugin.h"
#include "SFile.h"

view_prj_hmiplugin::view_prj_hmiplugin(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	ui.edtPrjName->setText("plugin_");
}

view_prj_hmiplugin::~view_prj_hmiplugin()
{

}

void view_prj_hmiplugin::on_btnDir_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("打开目录"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if(dir.length() == 0)
		return;
	ui.edtPrjPath->setText(dir);
}

void view_prj_hmiplugin::on_btnBuild_clicked()
{
	bool bCreateSubDir = ui.chkCreateSubDir->isChecked();
	SString sPrjName = ui.edtPrjName->text().toStdString().data();
	SString sPrjDesc = ui.edtPrjDesc->text().toStdString().data();
	if(sPrjName.length() == 0)
	{
		SQt::ShowErrorBox("错误","请先输入项目名称!");
		return;
	}
	if(sPrjName.left(7) != "plugin_")
	{
		SQt::ShowErrorBox("错误","项目名称应以'plugin_'开头!");
		return;
	}
	SString sPath = ui.edtPrjPath->text().toStdString().data();
	if(!SDir::dirExists(sPath))
	{
		SQt::ShowErrorBox("错误","项目目录不存在!");
		return;
	}
	if(sPath.right(1) != "/" && sPath.right(1) != "\\")
		sPath += SDIR_SEPARATOR;
	if(bCreateSubDir)
	{
		sPath += sPrjName;
		sPath += SDIR_SEPARATOR;
	}
	SDir::createDir(sPath);
	SDir::createDir(sPath+"Resources");

	SString sText;
	SString sTime = SDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
	SString sDate = SDateTime::currentDateTime().toString("yyyy-MM-dd");
	SFile f;

	//main.cpp
	sText += \
	"/**\r\n"
	" *\r\n"
	" * 文 件 名 : "+sPrjName+".cpp\r\n"
	" * 创建日期 : "+sTime+"\r\n"
	" * 作    者 : SspAssist(skt001@163.com)\r\n"
	" * 修改日期 : $Date: $\r\n"
	" * 当前版本 : $Revision: $\r\n"
	" * 功能描述 : "+sPrjDesc+"\r\n"
	" * 修改记录 : \r\n"
	" *            $Log: $\r\n"
	" *\r\n"
	" * Ver  Date        Author    Comments\r\n"
	" * ---  ----------  --------  -------------------------------------------\r\n"
	" * 001	 "+sDate+"	SspAssist　创建文件\r\n"
	" *\r\n"
	" **/\r\n"
	"#include \""+sPrjName+".h\"\r\n"
	"#include \"view_"+sPrjName+".h\"\r\n"
	"\r\n"
	"\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"// 描    述:  初始化插件\r\n"
	"// 作    者:  SspAssist\r\n"
	"// 创建时间:  "+sTime+"\r\n"
	"// 参数说明:  @sExtAttr表示扩展属性字符串，作保留参数，暂时为空串\r\n"
	"// 返 回 值:  int, 0表示成功，其他表示失败\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"PLUGIN_EXPORT int SSP_Init(const char* sExtAttr)\r\n"
	"{\r\n"
	"	SSP_PLUGIN_INIT;\r\n"
	"	//TODO: 在这里添加初始化代码，如通用动作注册、SVGTip回调注册等\r\n"
	"	return 0;\r\n"
	"}\r\n"
	"\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"// 描    述:  退出插件\r\n"
	"// 作    者:  SspAssist\r\n"
	"// 创建时间:  "+sTime+"\r\n"
	"// 参数说明:  void\r\n"
	"// 返 回 值:  int, 0表示成功，其他表示失败\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"PLUGIN_EXPORT int SSP_Exit()\r\n"
	"{\r\n"
	"	//TODO: 在这里添加退出代码\r\n"
	"	return 0;\r\n"
	"}\r\n"
	"\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"// 描    述:  取当前插件的名称\r\n"
	"// 作    者:  SspAssist\r\n"
	"// 创建时间:  "+sTime+"\r\n"
	"// 参数说明:  void\r\n"
	"// 返 回 值:  char*, 插件名称\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"PLUGIN_EXPORT const char* SSP_GetPluginName()\r\n"
	"{\r\n"
	"	return \""+sPrjDesc+"\";\r\n"
	"}\r\n"
	"\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"// 描    述:  取当前插件的版本号\r\n"
	"// 作    者:  SspAssist\r\n"
	"// 创建时间:  "+sTime+"\r\n"
	"// 参数说明:  void\r\n"
	"// 返 回 值:  char*, 插件版本号\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"PLUGIN_EXPORT const char* SSP_GetPluginVer()\r\n"
	"{\r\n"
	"	return \"1.0.0\";\r\n"
	"}\r\n"
	"\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"// 描    述:  返回当前插件支持的所有功能点名称与描述\r\n"
	"// 作    者:  SspAssist\r\n"
	"// 创建时间:  "+sTime+"\r\n"
	"// 参数说明:  void\r\n"
	"// 返 回 值:  char*, 如： ia_homepage=智能告警主界面;ia_report=告警告警简报;\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"PLUGIN_EXPORT const char* SSP_FunPointSupported()\r\n"
	"{\r\n"
	"	return \""+sPrjName+"="+sPrjDesc+";\";\r\n"
	"}\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"// 描    述:  返回当前插件支持的所有通用动作名称与描述\r\n"
	"// 作    者:  SspAssist\r\n"
	"// 创建时间:  "+sTime+"\r\n"
	"// 参数说明:  void\r\n"
	"// 返 回 值:  char*, 如：gact.nam.station.start=启动厂站网分通信;gact.nam.station.stop=停止厂站网分通信;\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"PLUGIN_EXPORT const char* SSP_GActSupported()\r\n"
	"{\r\n"
	"	return \"\";\r\n"
	"}\r\n"
	"\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"// 描    述:  创建新功能点窗口\r\n"
	"// 作    者:  SspAssist\r\n"
	"// 创建时间:  "+sTime+"\r\n"
	"// 参数说明:  @sFunName为待打开功能点名称\r\n"
	"//         :  @parentWidget为父窗口指针，真实类型为QWidget*\r\n"
	"// 返 回 值:  返回新打开的窗口指针，真实类型为CBaseView*，NULL表示非本插件对应的功能点\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"PLUGIN_EXPORT void* SSP_NewView(const char* sFunName,const void* parentWidget)\r\n"
	"{\r\n"
	"	SString sName = sFunName;\r\n"
	"	if(sName == \""+sPrjName+"\")\r\n"
	"		return new view_"+sPrjName+"((QWidget*)parentWidget);\r\n"
	"\r\n"
	"	return NULL;\r\n"
	"}\r\n";
	f.setFileName(sPath+sPrjName+".cpp");
	f.open(IO_Truncate);
	f.writeString(sText);
	f.close();

	sText = \
	"/**\r\n"
	" *\r\n"
	" * 文 件 名 : "+sPrjName+".h\r\n"
	" * 创建日期 : "+sTime+"\r\n"
	" * 作    者 : SspAssist(skt001@163.com)\r\n"
	" * 修改日期 : $Date: $\r\n"
	" * 当前版本 : $Revision: $\r\n"
	" * 功能描述 : "+sPrjDesc+"\r\n"
	" * 修改记录 : \r\n"
	" *            $Log: $\r\n"
	" *\r\n"
	" * Ver  Date        Author    Comments\r\n"
	" * ---  ----------  --------  -------------------------------------------\r\n"
	" * 001	 "+sDate+"	SspAssist　创建文件\r\n"
	" *\r\n"
	" **/\r\n"
	"\r\n"
	"#ifndef __"+sPrjName.toUpper()+"_H__\r\n"
	"#define __"+sPrjName.toUpper()+"_H__\r\n"
	"\r\n"
	"#include \"ssp_gui.h\"\r\n"
	"#include \"ssp_plugin.h\"\r\n"
	"\r\n"
	"extern \"C\" {\r\n"
	"\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"// 描    述:  初始化插件\r\n"
	"// 作    者:  SspAssist\r\n"
	"// 创建时间:  "+sTime+"\r\n"
	"// 参数说明:  @sExtAttr表示扩展属性字符串，作保留参数，暂时为空串\r\n"
	"// 返 回 值:  int, 0表示成功，其他表示失败\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"PLUGIN_EXPORT int SSP_Init(const char* sExtAttr);\r\n"
	"\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"// 描    述:  退出插件\r\n"
	"// 作    者:  SspAssist\r\n"
	"// 创建时间:  "+sTime+"\r\n"
	"// 参数说明:  void\r\n"
	"// 返 回 值:  int, 0表示成功，其他表示失败\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"PLUGIN_EXPORT int SSP_Exit();\r\n"
	"\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"// 描    述:  取当前插件的名称\r\n"
	"// 作    者:  SspAssist\r\n"
	"// 创建时间:  "+sTime+"\r\n"
	"// 参数说明:  void\r\n"
	"// 返 回 值:  char*, 插件名称\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"PLUGIN_EXPORT const char* SSP_GetPluginName();\r\n"
	"\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"// 描    述:  取当前插件的版本号\r\n"
	"// 作    者:  SspAssist\r\n"
	"// 创建时间:  "+sTime+"\r\n"
	"// 参数说明:  void\r\n"
	"// 返 回 值:  char*, 插件版本号\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"PLUGIN_EXPORT const char* SSP_GetPluginVer();\r\n"
	"\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"// 描    述:  返回当前插件支持的所有功能点名称与描述\r\n"
	"// 作    者:  SspAssist\r\n"
	"// 创建时间:  "+sTime+"\r\n"
	"// 参数说明:  void\r\n"
	"// 返 回 值:  char*, 如： ia_homepage=智能告警主界面;ia_report=告警告警简报;\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"PLUGIN_EXPORT const char* SSP_FunPointSupported();\r\n"
	"\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"// 描    述:  返回当前插件支持的所有通用动作名称与描述\r\n"
	"// 作    者:  SspAssist\r\n"
	"// 创建时间:  "+sTime+"\r\n"
	"// 参数说明:  void\r\n"
	"// 返 回 值:  char*, 如：gact.nam.station.start=启动厂站网分通信;gact.nam.station.stop=停止厂站网分通信;\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"PLUGIN_EXPORT const char* SSP_GActSupported();\r\n"
	"\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"// 描    述:  创建新功能点窗口\r\n"
	"// 作    者:  SspAssist\r\n"
	"// 创建时间:  "+sTime+"\r\n"
	"// 参数说明:  @sFunName为待打开功能点名称\r\n"
	"//         :  @parentWidget为父窗口指针，真实类型为QWidget*\r\n"
	"// 返 回 值:  返回新打开的窗口指针，真实类型为CBaseView*，NULL表示非本插件对应的功能点\r\n"
	"//////////////////////////////////////////////////////////////////////////\r\n"
	"PLUGIN_EXPORT void* SSP_NewView(const char* sFunName,const void* parentWidget);\r\n"
	"\r\n"
	"};\r\n"
	"\r\n"
	"#endif // __"+sPrjName.toUpper()+"_H__\r\n"
	"\r\n";
	f.setFileName(sPath+sPrjName+".h");
	f.open(IO_Truncate);
	f.writeString(sText);
	f.close();

	sText = \
	"/**\r\n"
	" *\r\n"
	" * 文 件 名 : view_"+sPrjName+".h\r\n"
	" * 创建日期 : "+sTime+"\r\n"
	" * 作    者 : SspAssist(skt001@163.com)\r\n"
	" * 修改日期 : $Date: $\r\n"
	" * 当前版本 : $Revision: $\r\n"
	" * 功能描述 : "+sPrjDesc+"\r\n"
	" * 修改记录 : \r\n"
	" *            $Log: $\r\n"
	" *\r\n"
	" * Ver  Date        Author    Comments\r\n"
	" * ---  ----------  --------  -------------------------------------------\r\n"
	" * 001	 "+sDate+"	SspAssist　创建文件\r\n"
	" *\r\n"
	" **/\r\n"
	"\r\n"
	"#ifndef __VIEW_"+sPrjName.toUpper()+"_H__\r\n"
	"#define __VIEW_"+sPrjName.toUpper()+"_H__\r\n"
	"\r\n"
	"#include \"ssp_baseview.h\"\r\n"
	"#include \"ui_view_"+sPrjName+".h\"\r\n"
	"\r\n"
	"class view_"+sPrjName+" : public CBaseView\r\n"
	"{\r\n"
	"	Q_OBJECT\r\n"
	"\r\n"
	"public:\r\n"
	"	view_"+sPrjName+"(QWidget *parent = 0);\r\n"
	"	~view_"+sPrjName+"();\r\n"
	"\r\n"
	"private:\r\n"
	"	Ui::view_"+sPrjName+" ui;\r\n"
	"};\r\n"
	"\r\n"
	"#endif // __VIEW_"+sPrjName.toUpper()+"_H__\r\n"
	"\r\n";
	f.setFileName(sPath+"view_"+sPrjName+".h");
	f.open(IO_Truncate);
	f.writeString(sText);
	f.close();

	
	sText = \
	"/**\r\n"
	" *\r\n"
	" * 文 件 名 : view_"+sPrjName+".cpp\r\n"
	" * 创建日期 : "+sTime+"\r\n"
	" * 作    者 : SspAssist(skt001@163.com)\r\n"
	" * 修改日期 : $Date: $\r\n"
	" * 当前版本 : $Revision: $\r\n"
	" * 功能描述 : "+sPrjDesc+"\r\n"
	" * 修改记录 : \r\n"
	" *            $Log: $\r\n"
	" *\r\n"
	" * Ver  Date        Author    Comments\r\n"
	" * ---  ----------  --------  -------------------------------------------\r\n"
	" * 001	 "+sDate+"	SspAssist　创建文件\r\n"
	" *\r\n"
	" **/\r\n"
	"#include \"view_"+sPrjName+".h\"\r\n"
	"\r\n"
	"view_"+sPrjName+"::view_"+sPrjName+"(QWidget *parent)\r\n"
	": CBaseView(parent)\r\n"
	"{\r\n"
	"	ui.setupUi(this);\r\n"
	"}\r\n"
	"\r\n"
	"view_"+sPrjName+"::~view_"+sPrjName+"()\r\n"
	"{\r\n"
	"\r\n"
	"}\r\n"
	"\r\n";
	f.setFileName(sPath+"view_"+sPrjName+".cpp");
	f.open(IO_Truncate);
	f.writeString(sText);
	f.close();

	sText = \
	"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
	"<ui version=\"4.0\">\r\n"
	"<class>view_"+sPrjName+"</class>\r\n"
	"<widget class=\"CBaseView\" name=\"view_"+sPrjName+"\">\r\n"
	"<property name=\"geometry\">\r\n"
	"<rect>\r\n"
	"<x>0</x>\r\n"
	"<y>0</y>\r\n"
	"<width>400</width>\r\n"
	"<height>300</height>\r\n"
	"</rect>\r\n"
	"</property>\r\n"
	"<property name=\"windowTitle\">\r\n"
	"<string>view_"+sPrjName+"</string>\r\n"
	"</property>\r\n"
	"<layout class=\"QGridLayout\" name=\"gridLayout\"/>\r\n"
	"</widget>\r\n"
	"<layoutdefault spacing=\"6\" margin=\"11\"/>\r\n"
	"<customwidgets>\r\n"
	"<customwidget>\r\n"
	"<class>CBaseView</class>\r\n"
	"<extends>QWidget</extends>\r\n"
	"<header>ssp_baseview.h</header>\r\n"
	"<container>1</container>\r\n"
	"</customwidget>\r\n"
	"</customwidgets>\r\n"
	"<resources/>\r\n"
	"<connections/>\r\n"
	"</ui>\r\n";

	f.setFileName(sPath+"view_"+sPrjName+".ui");
	f.open(IO_Truncate);
	f.writeString(sText);
	f.close();

	sText = \
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
		"<Project DefaultTargets=\"Build\" ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\r\n"
		"<ItemGroup Label=\"ProjectConfigurations\">\r\n"
		"<ProjectConfiguration Include=\"Debug|Win32\">\r\n"
		"<Configuration>Debug</Configuration>\r\n"
		"<Platform>Win32</Platform>\r\n"
		"</ProjectConfiguration>\r\n"
		"<ProjectConfiguration Include=\"Release|Win32\">\r\n"
		"<Configuration>Release</Configuration>\r\n"
		"<Platform>Win32</Platform>\r\n"
		"</ProjectConfiguration>\r\n"
		"</ItemGroup>\r\n"
		"<PropertyGroup Label=\"Globals\">\r\n"
		"<ProjectGuid>{BABEAE12-D3BE-4322-"+SDateTime::currentDateTime().toString("sszzz").left(4)+"-"+SDateTime::currentDateTime().toString("yyyyMMddhhmm")+"}</ProjectGuid>\r\n"
		"<Keyword>Qt4VSv1.0</Keyword>\r\n"
		"</PropertyGroup>\r\n"
		"<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />\r\n"
		"<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\" Label=\"Configuration\">\r\n"
		"<ConfigurationType>DynamicLibrary</ConfigurationType>\r\n"
		"</PropertyGroup>\r\n"
		"<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\" Label=\"Configuration\">\r\n"
		"<ConfigurationType>DynamicLibrary</ConfigurationType>\r\n"
		"</PropertyGroup>\r\n"
		"<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />\r\n"
		"<ImportGroup Label=\"ExtensionSettings\">\r\n"
		"</ImportGroup>\r\n"
		"<ImportGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\" Label=\"PropertySheets\">\r\n"
		"<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />\r\n"
		"</ImportGroup>\r\n"
		"<ImportGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\" Label=\"PropertySheets\">\r\n"
		"<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />\r\n"
		"</ImportGroup>\r\n"
		"<PropertyGroup Label=\"UserMacros\" />\r\n"
		"<PropertyGroup>\r\n"
		"<_ProjectFileVersion>10.0.30319.1</_ProjectFileVersion>\r\n"
		"<CodeAnalysisRuleSet Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">AllRules.ruleset</CodeAnalysisRuleSet>\r\n"
		"<CodeAnalysisRules Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\" />\r\n"
		"<CodeAnalysisRuleAssemblies Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\" />\r\n"
		"<CodeAnalysisRuleSet Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">AllRules.ruleset</CodeAnalysisRuleSet>\r\n"
		"<CodeAnalysisRules Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\" />\r\n"
		"<CodeAnalysisRuleAssemblies Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\" />\r\n"
		"<OutDir Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">..\\..\\out_debug\\bin</OutDir>\r\n"
		"<OutDir Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">..\\..\\out\\bin</OutDir>\r\n"
		"</PropertyGroup>\r\n"
		"<ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">\r\n"
		"<ClCompile>\r\n"
		"<PreprocessorDefinitions>WIN32;QT_DLL;QT_CORE_LIB;QT_GUI_LIB;QT_WEBKIT_LIB;%(PreprocessorDefinitions)</PreprocessorDefinitions>\r\n"
		"<AdditionalIncludeDirectories>.\\GeneratedFiles;.;$(QTDIR)\\include;.\\GeneratedFiles\\$(ConfigurationName);$(QTDIR)\\include\\QtCore;$(QTDIR)\\include\\QtGui;$(QTDIR)\\include\\QtWebKit;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\r\n"
		"<Optimization>Disabled</Optimization>\r\n"
		"<DebugInformationFormat>ProgramDatabase</DebugInformationFormat>\r\n"
		"<RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>\r\n"
		"<TreatWChar_tAsBuiltInType>false</TreatWChar_tAsBuiltInType>\r\n"
		"</ClCompile>\r\n"
		"<Link>\r\n"
		"<SubSystem>Windows</SubSystem>\r\n"
		"<OutputFile>$(OutDir)\\$(ProjectName)d.dll</OutputFile>\r\n"
		"<AdditionalLibraryDirectories>$(QTDIR)\\lib;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>\r\n"
		"<GenerateDebugInformation>true</GenerateDebugInformation>\r\n"
		"<AdditionalDependencies>qtmaind.lib;QtCored4.lib;QtGuid4.lib;QtWebKitd4.lib;%(AdditionalDependencies)</AdditionalDependencies>\r\n"
		"</Link>\r\n"
		"</ItemDefinitionGroup>\r\n"
		"<ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">\r\n"
		"<ClCompile>\r\n"
		"<PreprocessorDefinitions>WIN32;QT_DLL;QT_NO_DEBUG;NDEBUG;QT_CORE_LIB;QT_GUI_LIB;QT_WEBKIT_LIB;%(PreprocessorDefinitions)</PreprocessorDefinitions>\r\n"
		"<AdditionalIncludeDirectories>.\\GeneratedFiles;.;$(QTDIR)\\include;.\\GeneratedFiles\\$(ConfigurationName);$(QTDIR)\\include\\QtCore;$(QTDIR)\\include\\QtGui;$(QTDIR)\\include\\QtWebKit;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\r\n"
		"<DebugInformationFormat>\r\n"
		"</DebugInformationFormat>\r\n"
		"<RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>\r\n"
		"<TreatWChar_tAsBuiltInType>false</TreatWChar_tAsBuiltInType>\r\n"
		"</ClCompile>\r\n"
		"<Link>\r\n"
		"<SubSystem>Windows</SubSystem>\r\n"
		"<OutputFile>$(OutDir)\\$(ProjectName).dll</OutputFile>\r\n"
		"<AdditionalLibraryDirectories>$(QTDIR)\\lib;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>\r\n"
		"<GenerateDebugInformation>false</GenerateDebugInformation>\r\n"
		"<AdditionalDependencies>qtmain.lib;QtCore4.lib;QtGui4.lib;QtWebKit4.lib;%(AdditionalDependencies)</AdditionalDependencies>\r\n"
		"</Link>\r\n"
		"</ItemDefinitionGroup>\r\n"
		"<ItemGroup>\r\n"
		"<ClCompile Include=\"GeneratedFiles\\Debug\\Moc_view_"+sPrjName+".cpp\">\r\n"
		"<ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">true</ExcludedFromBuild>\r\n"
		"</ClCompile>\r\n"
		"<ClCompile Include=\"GeneratedFiles\\qrc_"+sPrjName+".cpp\">\r\n"
		"<PrecompiledHeader Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">\r\n"
		"</PrecompiledHeader>\r\n"
		"<PrecompiledHeader Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">\r\n"
		"</PrecompiledHeader>\r\n"
		"</ClCompile>\r\n"
		"<ClCompile Include=\"GeneratedFiles\\Release\\Moc_view_"+sPrjName+".cpp\">\r\n"
		"<ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">true</ExcludedFromBuild>\r\n"
		"</ClCompile>\r\n"
		"<ClCompile Include=\""+sPrjName+".cpp\" />\r\n"
		"<ClCompile Include=\"view_"+sPrjName+".cpp\" />\r\n"
		"</ItemGroup>\r\n"
		"<ItemGroup>\r\n"
		"<ClInclude Include=\"GeneratedFiles\\ui_view_"+sPrjName+".h\" />\r\n"
		"<ClInclude Include=\""+sPrjName+".h\" />\r\n"
		"<CustomBuild Include=\"view_"+sPrjName+".h\">\r\n"
		"<AdditionalInputs Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">$(QTDIR)\\bin\\Moc.exe;%(FullPath)</AdditionalInputs>\r\n"
		"<Message Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">Moc%27ing view_"+sPrjName+".h...</Message>\r\n"
		"<Outputs Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">.\\GeneratedFiles\\$(ConfigurationName)\\Moc_%(Filename).cpp</Outputs>\r\n"
		"<Command Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">\"$(QTDIR)\\bin\\Moc.exe\"  \"%(FullPath)\" -o \".\\GeneratedFiles\\$(ConfigurationName)\\Moc_%(Filename).cpp\"  -DWIN32 -DQT_DLL -DQT_CORE_LIB -DQT_GUI_LIB -DQT_WEBKIT_LIB -D_WINDLL  \"-I.\\GeneratedFiles\" \"-I.\" \"-I$(QTDIR)\\include\" \"-I.\\GeneratedFiles\\$(ConfigurationName)\\.\" \"-I$(QTDIR)\\include\\QtCore\" \"-I$(QTDIR)\\include\\QtGui\" \"-I$(QTDIR)\\include\\QtWebKit\"</Command>\r\n"
		"<AdditionalInputs Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">$(QTDIR)\\bin\\Moc.exe;%(FullPath)</AdditionalInputs>\r\n"
		"<Message Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">Moc%27ing view_"+sPrjName+".h...</Message>\r\n"
		"<Outputs Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">.\\GeneratedFiles\\$(ConfigurationName)\\Moc_%(Filename).cpp</Outputs>\r\n"
		"<Command Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">\"$(QTDIR)\\bin\\Moc.exe\"  \"%(FullPath)\" -o \".\\GeneratedFiles\\$(ConfigurationName)\\Moc_%(Filename).cpp\"  -DWIN32 -DQT_DLL -DQT_NO_DEBUG -DNDEBUG -DQT_CORE_LIB -DQT_GUI_LIB -DQT_WEBKIT_LIB -D_WINDLL  \"-I.\\GeneratedFiles\" \"-I.\" \"-I$(QTDIR)\\include\" \"-I.\\GeneratedFiles\\$(ConfigurationName)\\.\" \"-I$(QTDIR)\\include\\QtCore\" \"-I$(QTDIR)\\include\\QtGui\" \"-I$(QTDIR)\\include\\QtWebKit\"</Command>\r\n"
		"</CustomBuild>\r\n"
		"</ItemGroup>\r\n"
		"<ItemGroup>\r\n"
		"<CustomBuild Include=\"view_"+sPrjName+".ui\">\r\n"
		"<FileType>Document</FileType>\r\n"
		"<AdditionalInputs Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">$(QTDIR)\\bin\\uic.exe;%(AdditionalInputs)</AdditionalInputs>\r\n"
		"<Message Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">Uic%27ing %(Identity)...</Message>\r\n"
		"<Outputs Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">.\\GeneratedFiles\\ui_%(Filename).h;%(Outputs)</Outputs>\r\n"
		"<Command Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">\"$(QTDIR)\\bin\\uic.exe\" -o \".\\GeneratedFiles\\ui_%(Filename).h\" \"%(FullPath)\"</Command>\r\n"
		"<AdditionalInputs Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">$(QTDIR)\\bin\\uic.exe;%(AdditionalInputs)</AdditionalInputs>\r\n"
		"<Message Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">Uic%27ing %(Identity)...</Message>\r\n"
		"<Outputs Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">.\\GeneratedFiles\\ui_%(Filename).h;%(Outputs)</Outputs>\r\n"
		"<Command Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">\"$(QTDIR)\\bin\\uic.exe\" -o \".\\GeneratedFiles\\ui_%(Filename).h\" \"%(FullPath)\"</Command>\r\n"
		"</CustomBuild>\r\n"
		"</ItemGroup>\r\n"
		"<ItemGroup>\r\n"
		"<CustomBuild Include=\""+sPrjName+".qrc\">\r\n"
		"<FileType>Document</FileType>\r\n"
		"<AdditionalInputs Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">%(FullPath);%(AdditionalInputs)</AdditionalInputs>\r\n"
		"<Message Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">Rcc%27ing %(Identity)...</Message>\r\n"
		"<Outputs Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">.\\GeneratedFiles\\qrc_%(Filename).cpp;%(Outputs)</Outputs>\r\n"
		"<Command Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">\"$(QTDIR)\\bin\\rcc.exe\" -name \"%(Filename)\" -no-compress \"%(FullPath)\" -o .\\GeneratedFiles\\qrc_%(Filename).cpp</Command>\r\n"
		"<AdditionalInputs Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">%(FullPath);%(AdditionalInputs)</AdditionalInputs>\r\n"
		"<Message Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">Rcc%27ing %(Identity)...</Message>\r\n"
		"<Outputs Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">.\\GeneratedFiles\\qrc_%(Filename).cpp;%(Outputs)</Outputs>\r\n"
		"<Command Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">\"$(QTDIR)\\bin\\rcc.exe\" -name \"%(Filename)\" -no-compress \"%(FullPath)\" -o .\\GeneratedFiles\\qrc_%(Filename).cpp</Command>\r\n"
		"</CustomBuild>\r\n"
		"</ItemGroup>\r\n"
		"<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />\r\n"
		"<ImportGroup Label=\"ExtensionTargets\">\r\n"
		"</ImportGroup>\r\n"
		"<ProjectExtensions>\r\n"
		"<VisualStudio>\r\n"
		"<UserProperties UicDir=\".\\GeneratedFiles\" MocDir=\".\\GeneratedFiles\\$(ConfigurationName)\" MocOptions=\"\" RccDir=\".\\GeneratedFiles\" lupdateOnBuild=\"0\" lupdateOptions=\"\" lreleaseOptions=\"\" QtVersion_x0020_Win32=\"4.8.5\" />\r\n"
		"</VisualStudio>\r\n"
		"</ProjectExtensions>\r\n"
		"</Project>\r\n";

	f.setFileName(sPath+sPrjName+".vcxproj");
	f.open(IO_Truncate);
	f.writeString(sText);
	f.close();

	sText = \
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
		"<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\r\n"
		"<ItemGroup>\r\n"
		"<Filter Include=\"Source Files\">\r\n"
		"<UniqueIdentifier>{4FC737F1-C7A5-4376-A066-2A32D752A2FF}</UniqueIdentifier>\r\n"
		"<Extensions>cpp;cxx;c;def</Extensions>\r\n"
		"</Filter>\r\n"
		"<Filter Include=\"Header Files\">\r\n"
		"<UniqueIdentifier>{93995380-89BD-4b04-88EB-625FBE52EBFB}</UniqueIdentifier>\r\n"
		"<Extensions>h</Extensions>\r\n"
		"</Filter>\r\n"
		"<Filter Include=\"Form Files\">\r\n"
		"<UniqueIdentifier>{99349809-55BA-4b9d-BF79-8FDBB0286EB3}</UniqueIdentifier>\r\n"
		"<Extensions>ui</Extensions>\r\n"
		"</Filter>\r\n"
		"<Filter Include=\"Resource Files\">\r\n"
		"<UniqueIdentifier>{D9D6E242-F8AF-46E4-B9FD-80ECBC20BA3E}</UniqueIdentifier>\r\n"
		"<Extensions>qrc;*</Extensions>\r\n"
		"<ParseFiles>false</ParseFiles>\r\n"
		"</Filter>\r\n"
		"<Filter Include=\"Generated Files\">\r\n"
		"<UniqueIdentifier>{71ED8ED8-ACB9-4CE9-BBE1-E00B30144E11}</UniqueIdentifier>\r\n"
		"<Extensions>moc;h;cpp</Extensions>\r\n"
		"<SourceControlFiles>False</SourceControlFiles>\r\n"
		"</Filter>\r\n"
		"<Filter Include=\"Generated Files\\Debug\">\r\n"
		"<UniqueIdentifier>{0797f28c-52a9-480e-8f5a-f8bce3b51ed4}</UniqueIdentifier>\r\n"
		"<Extensions>cpp;moc</Extensions>\r\n"
		"<SourceControlFiles>False</SourceControlFiles>\r\n"
		"</Filter>\r\n"
		"<Filter Include=\"Generated Files\\Release\">\r\n"
		"<UniqueIdentifier>{d2422345-8c40-4161-865f-43d1c7fc991f}</UniqueIdentifier>\r\n"
		"<Extensions>cpp;moc</Extensions>\r\n"
		"<SourceControlFiles>False</SourceControlFiles>\r\n"
		"</Filter>\r\n"
		"</ItemGroup>\r\n"
		"<ItemGroup>\r\n"
		"<ClCompile Include=\""+sPrjName+".cpp\">\r\n"
		"<Filter>Source Files</Filter>\r\n"
		"</ClCompile>\r\n"
		"<ClCompile Include=\"view_"+sPrjName+".cpp\">\r\n"
		"<Filter>Source Files</Filter>\r\n"
		"</ClCompile>\r\n"
		"<ClCompile Include=\"GeneratedFiles\\Debug\\moc_view_"+sPrjName+".cpp\">\r\n"
		"<Filter>Generated Files\\Debug</Filter>\r\n"
		"</ClCompile>\r\n"
		"<ClCompile Include=\"GeneratedFiles\\Release\\moc_view_"+sPrjName+".cpp\">\r\n"
		"<Filter>Generated Files\\Release</Filter>\r\n"
		"</ClCompile>\r\n"
		"<ClCompile Include=\"GeneratedFiles\\qrc_"+sPrjName+".cpp\">\r\n"
		"<Filter>Generated Files</Filter>\r\n"
		"</ClCompile>\r\n"
		"</ItemGroup>\r\n"
		"<ItemGroup>\r\n"
		"<ClInclude Include=\""+sPrjName+".h\">\r\n"
		"<Filter>Header Files</Filter>\r\n"
		"</ClInclude>\r\n"
		"<ClInclude Include=\"GeneratedFiles\\ui_view_"+sPrjName+".h\">\r\n"
		"<Filter>Generated Files</Filter>\r\n"
		"</ClInclude>\r\n"
		"</ItemGroup>\r\n"
		"<ItemGroup>\r\n"
		"<CustomBuild Include=\"view_"+sPrjName+".h\">\r\n"
		"<Filter>Header Files</Filter>\r\n"
		"</CustomBuild>\r\n"
		"<CustomBuild Include=\"view_"+sPrjName+".ui\">\r\n"
		"<Filter>Form Files</Filter>\r\n"
		"</CustomBuild>\r\n"
		"<CustomBuild Include=\""+sPrjName+".qrc\">\r\n"
		"<Filter>Resource Files</Filter>\r\n"
		"</CustomBuild>\r\n"
		"</ItemGroup>\r\n"
		"</Project>\r\n";

	f.setFileName(sPath+sPrjName+".vcxproj.filters");
	f.open(IO_Truncate);
	f.writeString(sText);
	f.close();

	sText = \
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
		"<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\r\n"
		"<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">\r\n"
		"<QTDIR>C:\\Qt\\4.8.5</QTDIR>\r\n"
		"<LocalDebuggerEnvironment>PATH=$(QTDIR)\\bin%3b$(PATH)</LocalDebuggerEnvironment>\r\n"
		"<LocalDebuggerCommand>..\\..\\out_debug\\bin\\oms_hmi.exe</LocalDebuggerCommand>\r\n"
		"<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>\r\n"
		"<LocalDebuggerWorkingDirectory>..\\..\\out_debug\\bin</LocalDebuggerWorkingDirectory>\r\n"
		"</PropertyGroup>\r\n"
		"<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">\r\n"
		"<QTDIR>C:\\Qt\\4.8.5</QTDIR>\r\n"
		"<LocalDebuggerEnvironment>PATH=$(QTDIR)\\bin%3b$(PATH)</LocalDebuggerEnvironment>\r\n"
		"<LocalDebuggerCommand>..\\..\\out\\bin\\oms_hmi.exe</LocalDebuggerCommand>\r\n"
		"<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>\r\n"
		"<LocalDebuggerWorkingDirectory>..\\..\\out\\bin</LocalDebuggerWorkingDirectory>\r\n"
		"</PropertyGroup>\r\n"
		"</Project>\r\n";

	f.setFileName(sPath+sPrjName+".vcxproj.user");
	f.open(IO_Truncate);
	f.writeString(sText);
	f.close();

	sText = \
		"<RCC>\r\n"
		"	<qresource prefix=\"/"+sPrjName+"\">\r\n"
		"	</qresource>\r\n"
		"</RCC>\r\n";
	f.setFileName(sPath+sPrjName+".qrc");
	f.open(IO_Truncate);
	f.writeString(sText);
	f.close();

	sText = \
	"# ----------------------------------------------------\r\n"
	"# This file is generated by the SspAssist(skt001@163.com).\r\n"
	"# ------------------------------------------------------\r\n"
	"\r\n"
	"CONFIG(debug, debug|release){\r\n"
	"		TARGET = "+sPrjName+"d\r\n"
	"} else {\r\n"
	"	TARGET = "+sPrjName+"\r\n"
	"}\r\n"
	"\r\n"
	"include(../../plugin_sln.pri)\r\n"
	"	QT += \r\n"
	"	DEFINES +=\r\n"
	"	include("+sPrjName+".pri)\r\n"
	"\r\n";
	f.setFileName(sPath+sPrjName+".pro");
	f.open(IO_Truncate);
	f.writeString(sText);
	f.close();

	sText = \
		"# ----------------------------------------------------\r\n"
		"# This file is generated by the SspAssist(skt001@163.com).\r\n"
		"# ------------------------------------------------------\r\n"
		"\r\n"
		"HEADERS += ./"+sPrjName+".h \\\r\n"
		"    ./view_"+sPrjName+".h\r\n"
		"SOURCES += ./"+sPrjName+".cpp \\\r\n"
		"    ./view_"+sPrjName+".cpp\r\n"
		"FORMS += ./view_"+sPrjName+".ui \r\n"
		"RESOURCES += "+sPrjName+".qrc \r\n"		
		"\r\n";
	f.setFileName(sPath+sPrjName+".pri");
	f.open(IO_Truncate);
	f.writeString(sText);
	f.close();

	SQt::ShowInfoBox("提示","项目创建成功!");
}