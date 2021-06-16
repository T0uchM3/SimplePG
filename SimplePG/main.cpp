#include "SimplePG.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	SimplePG w;
	w.setWindowFlags(/*Qt::WindowCloseButtonHint Qt::Window |*/ Qt::FramelessWindowHint);
	w.show();
	return a.exec();
}