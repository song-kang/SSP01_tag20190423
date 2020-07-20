#include "clearData.h"


clearData::clearData(void)
{
}


clearData::~clearData(void)
{
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  清除表中无用数据
// 作    者:  褚冰冰
// 创建时间:  2015-7-30 10:30
// 参数说明:  @db为数据库连接对象指针
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void clearData::ClearUnusefulData(/*SDatabase * db*/)
{
	//BOOLCHECKRETURN(db->TestConnect(),QObject::tr("数据库连接失败！"),);
	FALSECHECKINFOSTART
	/**********清除用户表中多余数据***********/
	SString sSql;
	sSql = "delete from t_ssp_user where grp_code not in(select grp_code from t_ssp_user_group)";
	FALSECHECKINFORECORD(DB->ExecuteSQL(sSql),"t_ssp_user");
	/**********清除用户表中多余数据***********/


	/**********清除功能点表中多余数据***********/
// 	sSql = "delete from t_ssp_fun_point where p_fun_key not in (select fun_key  from (select fun_key from t_ssp_fun_point)a) and p_fun_key is not null";
// 	FALSECHECKINFORECORD(DB->ExecuteSQL(sSql),"t_ssp_fun_point");
	/**********清除功能点表中多余数据***********/


	/**********清除群组权限表中多余数据***********/
	sSql = "delete from t_ssp_usergroup_auth where grp_code not in(select grp_code from t_ssp_user_group)";
	FALSECHECKINFORECORD(DB->ExecuteSQL(sSql),"t_ssp_usergroup_auth");
	sSql = "delete from t_ssp_usergroup_auth where fun_key not in(select fun_key from t_ssp_fun_point)";
	FALSECHECKINFORECORD(DB->ExecuteSQL(sSql),"t_ssp_usergroup_auth");
	/**********清除群组权限表中多余数据***********/



	/**********清除用户权限表中多余数据***********/
	sSql = "delete from t_ssp_user_auth where usr_sn not in(select usr_sn from t_ssp_user)";
	FALSECHECKINFORECORD(DB->ExecuteSQL(sSql),"t_ssp_usergroup_auth");
	sSql = "delete from t_ssp_user_auth where fun_key not in(select fun_key from t_ssp_fun_point)";
	FALSECHECKINFORECORD(DB->ExecuteSQL(sSql),"t_ssp_usergroup_auth");
	/**********清除用户权限表中多余数据***********/
	// FALSECHECKINFOEND("清理失败！检查数据库连接或者数据库是否发生改变");
}