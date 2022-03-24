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

	///the quest continues >> .cpp
	bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
};
