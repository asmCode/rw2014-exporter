#pragma once

#include <max.h>
#include <string>
#include "..\CommonIncludes\IProgressObserver.h"

struct ExporterThreadData
{
	std::string engineFileName;
	std::string outputFileName;
	ExpInterface *expInterface;
	Interface *maxInterface;
	IProgressObserver *progressObserver;
};
