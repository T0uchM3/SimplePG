#include "SimplePG.h"
#include <QtWidgets/QApplication>
#include "NewEventFilter.h"
//#include "MyEventFilter.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	SimplePG w;
	//w.setWindowFlags(/*Qt::WindowCloseButtonHint Qt::Window |*/ Qt::FramelessWindowHint);
	a.installNativeEventFilter(new NewEventFilter());
	w.show();
	return a.exec();
}