########################################################################
#*
#* 文 件 名 : db_ssp.sql
#* 创建日期 : 2015-10-10 10:50
#* 作    者 : 邵凯田(skt001@163.com)
#* 修改日期 : $Date: 2015-10-10 10:50
#* 当前版本 : $Revision: 002$
#* 功能描述 : SSP01系统类软件平台数据模型数据库建库脚本
#* 修改记录 : 
#*            $Log: $
#*
#* Ver  Date        Author  Comments
#* ---  ----------  ------  -------------------------------------------
#* 001	2015-10-30  邵凯田　初始版本
#* 002	2015-11-11  邵凯田　增加通用标签文本表，数据窗口表中增加转置与交叉配置项
#*
########################################################################

/*!40101 SET NAMES utf8 */;

#
# Source for table "t_ssp_db_info"
#

DROP TABLE IF EXISTS `t_ssp_db_info`;
CREATE TABLE `t_ssp_db_info` (
  `db_sn` numeric(12,0) NOT NULL,
  `db_ver` varchar(16) NOT NULL,
  `ver_desc` varchar(64) NOT NULL,
  `upgrade_time` varchar(24) DEFAULT NULL,
  PRIMARY KEY (`db_sn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Data for table "t_ssp_db_info"
#

#
# Source for table "t_ssp_fun_point"
#

DROP TABLE IF EXISTS `t_ssp_fun_point`;
CREATE TABLE `t_ssp_fun_point` (
  `fun_key` varchar(32) NOT NULL DEFAULT '',
  `p_fun_key` varchar(32) DEFAULT NULL,
  `name` varchar(32) DEFAULT NULL,
  `idx` numeric(3,0) DEFAULT NULL,
  `auth` numeric(1,0) DEFAULT NULL,
  `type` numeric(1,0) DEFAULT NULL,
  `ref_sn` numeric(8,0) DEFAULT NULL,
  `dsc` varchar(128) DEFAULT NULL,
  `path` varchar(512) DEFAULT NULL,
  PRIMARY KEY (`fun_key`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Data for table "t_ssp_fun_point"
#

#
# Source for table "t_ssp_setup"
#

DROP TABLE IF EXISTS `t_ssp_setup`;
CREATE TABLE `t_ssp_setup` (
  `set_key` varchar(32) NOT NULL,
  `type_id` numeric(5,0) DEFAULT NULL,
  `set_desc` varchar(64) NOT NULL,
  `set_default` varchar(128) DEFAULT NULL,
  `set_value` varchar(128) DEFAULT NULL,
  `set_dime` varchar(8) DEFAULT NULL,
  `set_class` numeric(1,0) NOT NULL,
  `val_type` numeric(1,0) NOT NULL,
  `val_fmt` varchar(256) DEFAULT NULL,
  PRIMARY KEY (`set_key`),
  KEY `idx_setup_type` (`type_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Data for table "t_ssp_setup"
#


#
# Source for table "t_ssp_setup_type"
#

DROP TABLE IF EXISTS `t_ssp_setup_type`;
CREATE TABLE `t_ssp_setup_type` (
  `type_id` numeric(5,0) NOT NULL,
  `type_name` varchar(32) NOT NULL,
  PRIMARY KEY (`type_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Data for table "t_ssp_setup_type"
#


#
# Source for table "t_ssp_uicfg_wnd"
#

DROP TABLE IF EXISTS t_ssp_uicfg_wnd;
CREATE TABLE `t_ssp_uicfg_wnd` (
  `wnd_sn` numeric(8,0) NOT NULL DEFAULT '0',
  `wnd_name` varchar(32) DEFAULT NULL,
  `create_author` varchar(16) DEFAULT NULL,
  `create_time` int(4) DEFAULT NULL,
  `modify_author` varchar(16) DEFAULT NULL,
  `modify_time` int(4) DEFAULT NULL,
  `wnd_type` numeric(1,0) DEFAULT NULL,
  `svg_file` longblob,
  `dataset` varchar(32) DEFAULT NULL,
  `ds_cond` varchar(128) DEFAULT NULL,
  `refresh_sec` numeric(5,0) default 0,
  `cls_name` varchar(32) default '',
  PRIMARY KEY (`wnd_sn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Data for table "t_ssp_uicfg_wnd"
#

#
# Source for table "t_ssp_uicfg_wnd_ref"
#

DROP TABLE IF EXISTS t_ssp_uicfg_wnd_ref;
CREATE TABLE `t_ssp_uicfg_wnd_ref` (
  `id1` int(11) NOT NULL DEFAULT '0',
  `id2` int(11) NOT NULL DEFAULT '0',
  `id3` int(11) NOT NULL DEFAULT '0',
  `wnd_sn` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id1`,`id2`,`id3`,`wnd_sn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


#
# Source for table "t_ssp_svglib_type"
#

DROP TABLE IF EXISTS t_ssp_svglib_type;
CREATE TABLE t_ssp_svglib_type (
  `svgtype_sn` numeric(8,0) NOT NULL DEFAULT '0',
  `svgtype_name` varchar(32) NOT NULL,
  PRIMARY KEY (`svgtype_sn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Data for table "t_ssp_svglib_type"
#

#
# Source for table "t_ssp_svglib_item"
#

DROP TABLE IF EXISTS t_ssp_svglib_item;
CREATE TABLE t_ssp_svglib_item (
  `svg_sn` numeric(8,0) NOT NULL DEFAULT '0',
  `svgtype_sn` numeric(8,0) NOT NULL DEFAULT '0',
  `svg_name` varchar(32) NOT NULL,
  `svg_file` longblob,
  PRIMARY KEY (`svg_sn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Data for table "t_ssp_svglib_item"
#

#
# Source for table "t_ssp_data_window"
#

DROP TABLE IF EXISTS `t_ssp_data_window`;
CREATE TABLE `t_ssp_data_window` (
  `dw_sn` numeric(8,0) NOT NULL DEFAULT '0',
  `dstype` numeric(1,0) DEFAULT NULL,
  `name` varchar(32) DEFAULT NULL,
  `dbtype` numeric(1,0) DEFAULT NULL,
  `dw_desc` varchar(32) DEFAULT NULL,
  `dw_select` varchar(4000) DEFAULT NULL,
  `transform_type` numeric(1,0) DEFAULT NULL,
  `cross_refname` varchar(32) DEFAULT NULL,
  `cross_column` numeric(3,0) DEFAULT NULL,
  `cls_name` varchar(32) default '',
  `ext_attr` varchar(256) default '',
  PRIMARY KEY (`dw_sn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Source for table "t_ssp_data_window_col"
#

DROP TABLE IF EXISTS `t_ssp_data_window_col`;
CREATE TABLE `t_ssp_data_window_col` (
  `dw_sn` numeric(8,0) NOT NULL DEFAULT '0',
  `col_sn` numeric(5,0) NOT NULL DEFAULT '0',
  `col_name` varchar(32) DEFAULT NULL,
  `col_vtype` varchar(16) DEFAULT NULL,
  `col_width` numeric(5,0) DEFAULT NULL,
  `ref_name` varchar(32) DEFAULT NULL,
  `conv` varchar(32) DEFAULT NULL,
  `pkey` numeric(1,0) DEFAULT NULL,
  `isnull` numeric(1,0) DEFAULT NULL,
  `ukey` numeric(1,0) DEFAULT NULL,
  `ext_attr` varchar(256) default '',
  PRIMARY KEY (`dw_sn`,`col_sn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Source for table "t_ssp_data_window_row"
#

DROP TABLE IF EXISTS `t_ssp_data_window_row`;
CREATE TABLE `t_ssp_data_window_row` (
  `dw_sn` numeric(8,0) NOT NULL DEFAULT '0',
  `row_sn` numeric(5,0) NOT NULL DEFAULT '0',
  `row_id` varchar(32) DEFAULT NULL,
  `row_name` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`dw_sn`,`row_sn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Source for table "t_ssp_data_window_condition"
#

DROP TABLE IF EXISTS t_ssp_data_window_condition;
CREATE TABLE t_ssp_data_window_condition (
  `dw_sn` numeric(8,0) NOT NULL DEFAULT '0',
  `con_sn` numeric(5,0) NOT NULL DEFAULT '0',
  `con_name` varchar(32) DEFAULT NULL,
  `is_option` numeric(1,0) NOT NULL DEFAULT '0',
  `cmp_type` numeric(2,0) NOT NULL DEFAULT '0',
  `input_type` numeric(2,0) NOT NULL DEFAULT '0',
  `ref_name` varchar(32) DEFAULT NULL,
  `def_value` varchar(32) DEFAULT NULL,
  `dyn_where` varchar(1024) DEFAULT NULL,
  PRIMARY KEY (`dw_sn`,`con_sn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


#
# Source for table "t_ssp_general_action"
#

DROP TABLE IF EXISTS `t_ssp_general_action`;
CREATE TABLE `t_ssp_general_action` (
  `act_sn` numeric(8,0) NOT NULL DEFAULT '0',
  `act_type` numeric(1,0) DEFAULT NULL,
  `act_name` varchar(32) DEFAULT NULL,
  `act_desc` varchar(64) DEFAULT NULL,
  `act_tip` varchar(64),
  `succ_tip` varchar(64),
  `fail_tip` varchar(64),
  `act_sql` varchar(1024),
  `cls_name` varchar(32) default '',
  PRIMARY KEY (`act_sn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Source for table "t_ssp_user"
#

DROP TABLE IF EXISTS `t_ssp_user`;
CREATE TABLE `t_ssp_user` (
  `usr_sn` numeric(8,0) NOT NULL DEFAULT '0',
  `usr_code` varchar(16) NOT NULL DEFAULT '',
  `grp_code` varchar(16) DEFAULT NULL,
  `name` varchar(32) DEFAULT NULL,
  `pwd` varchar(32) DEFAULT NULL,
  `dsc` varchar(128) DEFAULT NULL,
  `email` varchar(64) DEFAULT NULL,
  `mobile` varchar(16) DEFAULT NULL,
  `create_time` int(4) DEFAULT NULL,
  `login_timeout` int(4) DEFAULT NULL,
  PRIMARY KEY (`usr_sn`),
  KEY `idx_ssp_user_grp_code` (`grp_code`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Data for table "t_ssp_user"
#

#
# Source for table "t_ssp_user_auth"
#

DROP TABLE IF EXISTS `t_ssp_user_auth`;
CREATE TABLE `t_ssp_user_auth` (
  `usr_sn` numeric(8,0) NOT NULL DEFAULT '0',
  `fun_key` varchar(16) NOT NULL DEFAULT '',
  `auth` numeric(1,0) DEFAULT NULL,
  PRIMARY KEY (`usr_sn`,`fun_key`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Data for table "t_ssp_user_auth"
#

#
# Source for table "t_ssp_user_group"
#

DROP TABLE IF EXISTS `t_ssp_user_group`;
CREATE TABLE `t_ssp_user_group` (
  `grp_code` varchar(16) NOT NULL DEFAULT '',
  `name` varchar(32) DEFAULT NULL,
  `dsc` varchar(128) DEFAULT NULL,
  `login_timeout` int(4) DEFAULT NULL,
  PRIMARY KEY (`grp_code`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Data for table "t_ssp_user_group"
#

#
# Source for table "t_ssp_usergroup_auth"
#

DROP TABLE IF EXISTS `t_ssp_usergroup_auth`;
CREATE TABLE `t_ssp_usergroup_auth` (
  `grp_code` varchar(16) NOT NULL DEFAULT '',
  `fun_key` varchar(16) NOT NULL DEFAULT '',
  `auth` numeric(1,0) DEFAULT NULL,
  PRIMARY KEY (`grp_code`,`fun_key`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Data for table "t_ssp_usergroup_auth"
#

DROP TABLE IF EXISTS `t_ssp_syslog`;
CREATE TABLE `t_ssp_syslog` (
  `soc` int NOT NULL,
  `usec` int NOT NULL,
  `usr_sn` int NOT NULL,
  `log_type` numeric(3,0) NOT NULL,
  `log_level` numeric(3,0) NOT NULL,
  `id1` numeric(5,0) NOT NULL,
  `id2` numeric(5,0) NOT NULL,
  `id3` numeric(5,0) NOT NULL,
  `id4` numeric(5,0) NOT NULL,
  `log_text` varchar(255)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Data for table "`t_ssp_tagged_text`"
#

DROP TABLE IF EXISTS `t_ssp_tagged_text`;
CREATE TABLE `t_ssp_tagged_text` (
  `tag_no` int NOT NULL,
  `tag_name` varchar(32) NOT NULL,
  `tag_text` varchar(256) NOT NULL,
  `tag_color` int,
  PRIMARY KEY (`tag_no`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

#
# Data for table "`t_ssp_tagged_text`"
#

