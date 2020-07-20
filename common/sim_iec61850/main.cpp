
#include "sim_iec61850.h"

#include <QtGui>
#include <QResource>
#include "GuiApplication.h"
#include "SimMmsServer.h"

#define MOD_VERSION "1.2.0"

#ifndef MOD_DATE
#define MOD_DATE "2016-09-02"
#endif
#ifndef MOD_TIME
#define MOD_TIME "12:36:00"
#endif

// #ifdef _DEBUG
// #include "vld.h"
// #endif
int main(int argc, char *argv[])
{
	unsigned int ui = 500000000;
	int i = 500078125;
	int sep = ui-i;
	sep = i-ui;
	CGuiApplication g_app;
	QApplication::setColorSpec(QApplication::ManyColor);
	QCoreApplication::addLibraryPath("./plugins"); 
	QApplication::setStyle("Cleanlooks");
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
	//InitConsoleWindow();
	QApplication a(argc, argv);
	//	QMessageBox::information(NULL,"",str);

	bool b = QResource::registerResource("ssp_gui.rcc");
	g_app.SetQApplication(&a);
	g_app.SetModuleDesc("IEC61850È«¾°Ä£ÄâÆ÷");
	g_app.SetVersion(MOD_VERSION);
	//InitConsoleWindow();
// 	CSimMmsServer mms;
// 	mms.AddIedByScd("auto.scd");
// 	mms.Start();
	g_app.Run(argc,argv,MOD_DATE,MOD_TIME,false);
//	mms.StopAndWait();
	SLog::WaitForLogEmpty();
	SLog::quitLog();
}
