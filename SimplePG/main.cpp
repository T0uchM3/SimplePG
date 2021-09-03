#include "SimplePG.h"
#include <QtWidgets/QApplication>
#include "NewEventFilter.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SimplePG w;
	w.setWindowFlags(Qt::FramelessWindowHint);
	///the quest for making WndProc works start HERE >> NewEventFilter.h
	a.installNativeEventFilter(new NewEventFilter());
	w.show();
	return a.exec();
}