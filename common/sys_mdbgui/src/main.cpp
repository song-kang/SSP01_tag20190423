#include "sys_mdbgui.h"
#include "enterdlg.h"
#include <QtGui/QApplication>
#include <QTextCodec>
#include <QTranslator>
#include "vld.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTextCodec *codec = QTextCodec::codecForName("System");  
	QTextCodec::setCodecForLocale(codec);  
	QTextCodec::setCodecForTr(codec);  
	QTextCodec::setCodecForCStrings(codec); 
	a.setWindowIcon(QIcon(":/Resources/icon/0.ico"));
	QTranslator translator;   
	translator.load(":/Resources/qt_zh_CN.qm");   
	a.installTranslator(&translator);  
	sys_mdbgui w;
	w.show();
	return a.exec();
}
