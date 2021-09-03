#pragma once
#include "QAbstractNativeEventFilter"
#include "SimplePG.h"
#include <windows.h>

//class SimplePG;
class NewEventFilter : public QAbstractNativeEventFilter
{
public:
	NewEventFilter();
	~NewEventFilter() override;
	long *res;

	///the quest continue >> .cpp
	bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
};
