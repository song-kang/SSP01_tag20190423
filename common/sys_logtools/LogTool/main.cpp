#include "logtool.h"
#include <QtGui/QApplication>
#include "vld.h"
int main(int argc, char *argv[])
{
#ifdef WIN32
	QTextCodec *codec = QTextCodec::codecForName("system");
#else
	QTextCodec *codec = QTextCodec::codecForName("GBK");
#endif

	QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
	QTextCodec::setCodecForTr(codec);	
	QTextCodec::setCodecForCStrings(codec);	

	QApplication a(argc, argv);
	LogTool w;
	w.show();
	return a.exec();
}
