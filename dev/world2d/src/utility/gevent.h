//game event
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
//
//                                               Copyright    (C)    1999    Pin
//
#ifndef GEVENT_H
#define GEVENT_H

//#include <pdef.h>
#include <PTimer.h>
#include <tShape.h>

#define EVENT_NONE 0
#define LIVE_FOREVER 0

#define EVENT_ALIVE 1
#define EVENT_DEAD  0

typedef UINT event_t;


class CPGEvent;

typedef BOOL (*EventConditionCallBack)( CPGEvent* event, void* who );
typedef void (*EventActionCallBack)( CPGEvent* event, void* who );


class CPGEvent
{
	UINT            m_ID;
	event_t         m_Type;
	DWORD       	m_LiveTime;

	DWORD       	m_DeathTime;	    // When trigger expires.
	//int             m_isAlive;
	BOOL            m_Testing;

	//float			fRadius;			// Distance from source that trigger affects.
	//bool			fDynamicSourcePos;	// Lookup position of trigger source every update.
	//bool          fRepeat;            // Repeat or not
	//static Pulong gNextTriggerID;     // Next unique Registration ID for trigger.
	
	CPTimer                 m_Timer;
	LG::Shape               m_BShape;
	EventConditionCallBack  m_funcCondition;
	EventActionCallBack     m_funcAction;

	void      initiate( UINT id, event_t type,
	                    DWORD cycle=0, DWORD live=LIVE_FOREVER,// cycle is circuity event ( send message n/per x )
		                EventConditionCallBack condition=NULL,
			            EventActionCallBack action=NULL );

	event_t   Respond( void* who );

public:

	CPGEvent( UINT id, event_t type,
			  DWORD cycle=0, DWORD live=LIVE_FOREVER,// cycle is circuity event ( send message n/per x )
		      EventConditionCallBack condition=NULL,
			  EventActionCallBack    action=NULL);


	CPGEvent( UINT id, event_t type,
		      LG::Spheref bound,
			  DWORD cycle=0, DWORD live=LIVE_FOREVER,// cycle is circuity event ( send message n/per x )
		      EventConditionCallBack condition=NULL,
			  EventActionCallBack    action=NULL);


	CPGEvent( UINT id, event_t type,
		      LG::BBoxf bound,
			  DWORD cycle=0, DWORD live=LIVE_FOREVER,
		      EventConditionCallBack condition=NULL,
			  EventActionCallBack    action=NULL);


	CPGEvent( UINT id, event_t type,
		      LG::Plane3df bound,
			  DWORD cycle=0, DWORD live=LIVE_FOREVER,
		      EventConditionCallBack condition=NULL,
			  EventActionCallBack    action=NULL);


	~CPGEvent();

	UINT     GetID       ( void ){ return m_ID; }
	UINT     GetType     ( void ){ return m_Type; }
	DWORD    GetLiveTime ( void ){ return m_LiveTime; }
	DWORD    GetCycleTime( void ){ return m_Timer.GetCycle(); }
	EventConditionCallBack    GetCondition ( void ){ return m_funcCondition; }
	EventActionCallBack       GetAction    ( void ){ return m_funcAction; }



	LG::Shape* GetShape( void );

	void     SetShape( LG::Spheref s );
	void     SetShape( LG::BBoxf   c );

	void     Update( void );

	DWORD    isDeath   ( void );
	BOOL     isTest    ( void );
	event_t  isTouch ( LG::Spheref from, void* who=NULL );
	event_t  isTouch ( LG::BBoxf   from, void* who=NULL );
};

#endif



