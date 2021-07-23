#include "NewEventFilter.h"
#include "QString"
//#include <stdexcept>
//#include "SimplePG.h"
NewEventFilter::NewEventFilter()
{
}

NewEventFilter::~NewEventFilter()
{
}

bool NewEventFilter::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
{
	//QString et = QString(eventType);
	if (eventType.toStdString() == "windows_generic_MSG") {
		res = (long*)WndProc(((MSG*)message)->hwnd, ((MSG*)message)->message, ((MSG*)message)->wParam, ((MSG*)message)->lParam);
	}
	//qDebug("nativeFilter");
	return false;
}