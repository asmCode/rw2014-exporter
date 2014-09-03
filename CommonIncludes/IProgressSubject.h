#pragma once

#include "IProgressObserver.h"

class IProgressSubject
{
public:
	virtual void RegisterObserver(IProgressObserver *observer) = 0;
	virtual void UnregisterObserver(IProgressObserver *observer) = 0;
};
