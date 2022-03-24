#include "NewEventFilter.h"
#include "QString"

NewEventFilter::NewEventFilter()
{
}

NewEventFilter::~NewEventFilter()
{
}

///almost there >>

bool NewEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
	auto et = QString(eventType);
	if (eventType.toStdString() == "windows_generic_MSG")
	{
		///>> SimplePG.h
		res = (long*)WndProc(static_cast<MSG*>(message)->hwnd, static_cast<MSG*>(message)->message, static_cast<MSG*>(message)->wParam, static_cast<MSG*>(message)->lParam);
	}
	return false;
}
