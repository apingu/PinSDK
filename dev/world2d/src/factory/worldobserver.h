#ifndef __WORLDOBSERVER_H
#define __WORLDOBSERVER_H
//-----------------------------------------------------------------------------
class CPEWorldObserver
{
public:
	CPEWorldObserver() {}
	virtual ~CPEWorldObserver() {}

	virtual void OnTargetChanged(void* data) = 0;
};

//-----------------------------------------------------------------------------
#endif