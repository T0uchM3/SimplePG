#pragma once
#include "QAbstractNativeEventFilter"
#include "SimplePG.h"
#include <windows.h>

//class SimplePG;
class NewEventFilter : public QAbstractNativeEventFilter
{
public:
	NewEventFilter();
	~NewEventFilter();
	long* res;

	bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;
};
