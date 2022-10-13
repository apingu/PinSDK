//game trigger
//
//
//
//
//
//
//
//
//
//
//
//                                               Copyright    (C)    2000    Pin
//                                                               by Apingu
//
#ifndef GTRIGGER_H
#define GTRIGGER_H

#include "GEvent.h"
#include <vector>


typedef UINT (*getConditionIDCallBack)( CPGEvent *event );
typedef UINT (*getActionIDCallBack)( CPGEvent *event );

typedef EventConditionCallBack (*getConditionFuncCallBack)( UINT conditionID );
typedef EventActionCallBack    (*getActionFuncCallBack)( UINT actionID );


typedef void GEVENT;



class CPGTriggerSys
{
	std::vector<CPGEvent*> m_pEventVec;

public:

	CPGTriggerSys();
	~CPGTriggerSys();

	GEVENT    RegisterEvent( UINT id, UINT type,
							 DWORD cycle=0, DWORD life=LIVE_FOREVER,// cycle is circuity event,
                             EventConditionCallBack condition=NULL,
			                 EventActionCallBack action=NULL );

	GEVENT    RegisterEvent( UINT id, UINT type,
		                     LG::Spheref bound,
							 DWORD cycle=0, DWORD life=LIVE_FOREVER,// cycle is circuity event
			                 EventConditionCallBack condition=NULL,
			                 EventActionCallBack    action=NULL);

	GEVENT    RegisterEvent( UINT id, UINT type,
		                     LG::BBoxf bound,
							 DWORD cycle=0, DWORD life=LIVE_FOREVER,// cycle is circuity event
			                 EventConditionCallBack condition=NULL,
			                 EventActionCallBack    action=NULL);

	GEVENT    RegisterEvent( UINT id, UINT type,
		                     LG::Plane3df bound,
							 DWORD cycle=0, DWORD life=LIVE_FOREVER,// cycle is circuity event,
                             EventConditionCallBack condition=NULL,
			                 EventActionCallBack action=NULL );


   	void      RemoveEvent( UINT id );

	UINT      GetEventCount( void );
	CPGEvent* GetEvent( int i );

	void      TouchTest( LG::Spheref s, void* who );
	void      Update( void );

	error_t    Save( Pcstr filename,
		            getConditionIDCallBack condition,
	                getActionIDCallBack    action );

	error_t    Load( Pcstr filename,
		            getConditionFuncCallBack condition,
					getActionFuncCallBack    action	);
	
};

#endif //GTRIGGER_H
