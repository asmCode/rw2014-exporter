#pragma once

class IProgressSubject;

class IProgressObserver
{
public:
	virtual void SetProgressSteps(IProgressSubject *sender, int steps) = 0;
	virtual void StepProgress(IProgressSubject *sender) = 0;
	virtual void Finish(IProgressSubject *sender) = 0;
};
