#include "ssp_assist.h"

#include <QtGui>
#include <QResource>
#include "GuiApplication.h"

#define MOD_VERSION "1.0.0"

#ifndef MOD_DATE
#define MOD_DATE "2015-07-30"
#endif
#ifndef MOD_TIME
#define MOD_TIME "12:36:00"
#endif

//#ifdef _DEBUG
// #include "vld.h"
// #endif
int main(int argc, char *argv[])
{
	CGuiApplication g_app;
	QApplication::setColorSpec(QApplication::ManyColor);
	QCoreApplication::addLibraryPath("./plugins"); 
	QApplication::setStyle("Cleanlooks");
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));

	QApplication a(argc, argv);
	//	QMessageBox::information(NULL,"",str);

	bool b = QResource::registerResource("ssp_gui.rcc");
	g_app.SetQApplication(&a);
	g_app.SetModuleDesc("SSP平台开发助手");
	g_app.SetVersion(MOD_VERSION);
	g_app.Run(argc,argv,MOD_DATE,MOD_TIME,false);
}
