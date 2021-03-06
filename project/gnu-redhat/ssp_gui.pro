# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = lib
TARGET = ssp_gui

QT += core gui webkit
DEFINES += QT_DLL SSP_GUI_LIB QT_WEBKIT_LIB
INCLUDEPATH += ./GeneratedFiles \
    . \
    ./GeneratedFiles/Release \
    ./../../src/ssp_gui
	
UI_DIR += ssp_gui/GeneratedFiles
RCC_DIR += ssp_gui/GeneratedFiles
	
CONFIG(debug, debug|release) {
	TARGET = ssp_guid
	MOC_DIR += ssp_gui/GeneratedFiles/debug
	OBJECTS_DIR += ssp_gui/debug
} else {
	TARGET = ssp_gui
	MOC_DIR += ssp_gui/GeneratedFiles/release
	OBJECTS_DIR += ssp_gui/release
}

unix {	
	LIBS += -L/home/instantclient_12_1
	INCLUDEPATH += /home/skt/src/sbase/include \
		/home/skt/src/sbase/include/comm \
		/home/skt/src/sbase/include/db \
		/home/skt/src/sbase/include/db/mysql \
		/home/skt/src/sbase/include/db/oracle \
		/home/skt/src/sbase/include/db/mdb \
		/home/skt/src/sbase/include/gui \
		/home/skt/src/sbase/include/gui/qt \
		/home/skt/src/sbase/include/gui/svg \
		/home/skt/src/SSP01/include/ssp_base \
		/home/skt/src/SSP01/include/ssp_gui \
		/home/instantclient_12_1/include \
		/home/instantclient_12_1/sdk/include \
		/usr/include \
		/usr/include/mysql \
		/usr/local/include
	
	LIBS += -L/usr/appsoft/qt-4.8.5/lib -L/lib64 \
		-L/home/instantclient_12_1 -L/home/skt/src/sbase/lib/linux/ \
		-L/home/skt/src/SSP01/lib/linux -L/usr/X11R6/lib 
	
	exists( /etc/debian_version ) {
		LIBS += -L/home/skt/src/sbase/lib/linux-debian -L/ -L/home/skt/src/SSP01/lib/linux-debian
	} else:exists( /etc/redhat-release ) {
		LIBS += -L/home/skt/src/sbase/lib/linux-centos -L/home/skt/src/SSP01/lib/linux-centos -L/usr/lib64/mysql 
	} else {
		LIBS += -L/home/skt/src/sbase/lib/linux -L/home/skt/src/SSP01/lib/linux
	}
}

win32 {
	DESTDIR = ../../lib/win32
}

DEPENDPATH += .

include(ssp_gui.pri)
