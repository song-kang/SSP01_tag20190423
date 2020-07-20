#!/bin/sh

build_ssp_gui_release()
{
	qmake CONFIG+=release ssp_gui.pro -o Makefile_gui_release
	make -f Makefile_gui_release
}

build_ssp_gui_debug()
{
	qmake CONFIG+=debug ssp_gui.pro -o Makefile_gui_debug
	make -f Makefile_gui_debug
}

build_ssp_iaFloatView_release()
{
	qmake CONFIG+=release ssp_iaFloatView.pro -o Makefile_iaFloatView_release
	make -f Makefile_iaFloatView_release
}

build_ssp_iaFloatView_debug()
{
	qmake CONFIG+=debug ssp_iaFloatView.pro -o Makefile_iaFloatView_debug
	make -f Makefile_iaFloatView_debug
}

build_base_release()
{
	make -f Makefile_base release
}

build_base_debug()
{
	make -f Makefile_base debug
}


case "$1" in
  base)
	build_base_release
	build_base_debug
	;;

  gui)
	build_ssp_gui_release
	build_ssp_gui_debug
	;;

  iaFloatView)
	build_ssp_iaFloatView_release
	build_ssp_iaFloatView_debug
	;;
	
  clean)
	make -f Makefile_gui_release clean
	make -f Makefile_gui_debug clean
	make -f Makefile_ssp_iaFloatView_release clean
	make -f Makefile_ssp_iaFloatView_debug clean
	make -f Makefile_base clean
	;;
	
  *)
	build_base_release
	build_base_debug
	build_ssp_gui_release
	build_ssp_gui_debug
	build_ssp_iaFloatView_release
	build_ssp_iaFloatView_debug
	exit 0

esac
exit 0