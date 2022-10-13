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
#include "GEvent.h"
#include <aIntersect.h>



//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
CPGEvent::CPGEvent( UINT id, UINT type,
					DWORD cycle, DWORD live,
                    EventConditionCallBack condition,
                    EventActionCallBack    action)
{
	m_Testing = TRUE;
	initiate( id, type, cycle, live, condition, action );
	m_BShape.BeNull();
}


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
CPGEvent::CPGEvent( UINT id, UINT type,
				    LG::Spheref bound,
					DWORD cycle, DWORD live,
                    EventConditionCallBack condition,
                    EventActionCallBack    action)

{
	initiate( id, type, cycle, live, condition, action );
	m_BShape.BeSpheref( bound.center.x, bound.center.y, bound.center.z, bound.radius );
}


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
CPGEvent::CPGEvent( UINT id, UINT type,
				    LG::Cubef bound,
					DWORD cycle, DWORD live,
                    EventConditionCallBack condition,
                    EventActionCallBack    action)

{
	initiate( id, type, cycle, live, condition, action );
	m_BShape.BeCubef( bound.vertex[0].x, bound.vertex[0].y, bound.vertex[0].z,
		              bound.vertex[1].x, bound.vertex[1].y, bound.vertex[1].z	);
}

//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
CPGEvent::CPGEvent( UINT id, UINT type,
		            LG::Plane3df bound,
			        DWORD cycle, DWORD live,
		            EventConditionCallBack condition,
			        EventActionCallBack    action)
{
	initiate( id, type, cycle, live, condition, action );
	m_BShape.BePlane3df( bound.normal.x, 
		                 bound.normal.y,
					     bound.normal.z,
					     bound.cnst );
}



//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
CPGEvent::~CPGEvent()
{

}


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
void CPGEvent::initiate( UINT id, UINT type,
					     DWORD cycle, DWORD live,
                         EventConditionCallBack condition,
                         EventActionCallBack    action)
{
	m_ID = id;
	m_Type = type;
	m_Timer.SetCycle( cycle );
	m_LiveTime = live;
	m_Testing = TRUE;
	if( live != LIVE_FOREVER )
	    m_DeathTime = m_Timer.GetBirthTime() + m_LiveTime;
	else
		m_DeathTime = LIVE_FOREVER;

	m_funcCondition=condition;
	m_funcAction=action;

	//m_isAlive = EVENT_ALIVE;
}



//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
DWORD CPGEvent::isDeath( void )
{
	if( m_DeathTime != LIVE_FOREVER )
	{
		DWORD timeage = m_DeathTime - m_Timer.GetTickTime();
		if( timeage >= 0 )
		{
		    //m_isAlive = EVENT_ALIVE;
		    return timeage;
		}
		//return m_isAlive = EVENT_DEAD;
		return EVENT_DEAD;
	}

	//return m_isAlive = EVENT_ALIVE;
	return EVENT_ALIVE;
}

//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
BOOL CPGEvent::isTest( void )
{
	return m_Testing;
}


/*
//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
DWORD CPGEvent::interval( void )
{
	return m_Timer.Interval();
}
*/


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
LG::Shape* CPGEvent::GetShape( void )
{
	return &m_BShape;
}

//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
void CPGEvent::SetShape( LG::Spheref s )
{
	if( m_BShape.GetType() == _P_SPHEREF_ )
	{
		m_BShape.Spheref()->center = s.center;
	    m_BShape.Spheref()->radius = s.radius;
		return;
	}
	m_BShape.BeSpheref( s.center.x, s.center.y, s.center.z, s.radius );
	return;
}


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
void CPGEvent::SetShape( LG::Cubef c )
{
	if( m_BShape.GetType() == _P_CUBE_ )
	{
		m_BShape.Cubef()->vertex[0] = c.vertex[0];
		m_BShape.Cubef()->vertex[1] = c.vertex[1];
		return;
	}
	m_BShape.BeCubef( c.vertex[0].x, c.vertex[0].y, c.vertex[0].z,
		             c.vertex[1].x, c.vertex[1].y, c.vertex[1].z );
	return;

}

//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
void CPGEvent::Update( void )
{
	if( m_Timer.Interval() >= TIMER_INTERRUPT )
	{
		m_Testing = TRUE;
	}
	else
	{
		m_Testing = FALSE;
	}
	return;
}


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
UINT CPGEvent::isTouch( LG::Spheref from, void* who )
{
	//if( ( m_isAlive == EVENT_DEAD ) ||
	//	( !m_Testing ) )
	if( !m_Testing )
		return EVENT_NONE;

	switch( m_BShape.GetType() )
	{
	case _P_CUBE_:
		if( PA::Intersect::Test( &from, (m_BShape.Cubef()) ) >= 0 )
		{
			return Respond(who);
		}
		break;
	case _P_PLANE3DF_:
		if( PA::Intersect::Test( &from, m_BShape.Plane3df() ) >= 0 )
		{
			return Respond(who);
		}
	    break;
	case _P_SPHEREF_:
		if( PA::Intersect::Test( m_BShape.Spheref(), &from ) >= 0 )
		{
			return Respond(who);
		}
		break;
	case _P_SHAPELESS_:
		return Respond(who);

	}

	return EVENT_NONE;
}


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
UINT CPGEvent::isTouch( LG::Cubef from, void* who )
{
	//if( ( m_isAlive == EVENT_DEAD ) ||
	//	( !m_Testing ) )
	if( !m_Testing )
		return EVENT_NONE;

	switch( m_BShape.GetType() )
	{
	case _P_CUBE_:
		if( PA::Intersect::Test( m_BShape.Cubef(), &from ) >= 0 )
		{
			return Respond(who);
		}
		break;
	case _P_SPHEREF_:
		if( PA::Intersect::Test( m_BShape.Spheref(), &from ) >= 0 )
		{
			return Respond(who);
		}
		break;
	case _P_SHAPELESS_:
		return Respond(who);

	}

	return EVENT_NONE;
}


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
event_t CPGEvent::Respond( void* who )
{
    if( ( m_funcCondition!=NULL ) &&
		( m_funcCondition( this, who ) ) )
	{
		if( m_funcAction!=NULL )
			m_funcAction( this, who );
		return m_Type;
	}

	return EVENT_NONE;
}
