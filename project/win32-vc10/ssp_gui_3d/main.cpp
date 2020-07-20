
#include <QtGui/QApplication>
#include "ssp_3d_widget.h"
#ifndef SSP_GUI_3D_LIB
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CSsp3DWidget w;
	w.show();
	return a.exec();
}
#endif
