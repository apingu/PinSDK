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
#include "GTrigger.h"
//#include "PFile.h"
#include <PGSPFile.h>

namespace TriggerSys
{
    DWORD Name = (DWORD)("TRIG");

    struct HEvent
	{
	    UINT       id;
	    event_t    type;
	    DWORD      liveTime;	        // When trigger expires.
	    DWORD      cycleTime;
		LG::Shape  shape;
		EventConditionCallBack  m_funcCondition;
	    EventActionCallBack     m_funcAction;

	    //UINT       conditionID;
	    //UINT       actionID;
	    //shape_t    shapetype;
	    //BYTE       shapedata[100];
	};

};




//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
CPGTriggerSys::CPGTriggerSys()
{
	//m_TestingTimes = 0;
}


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
CPGTriggerSys::~CPGTriggerSys()
{
    for( UINT i=0; i< m_pEventVec.size(); i++ )
		delete m_pEventVec[i];
	m_pEventVec.clear();
}



//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
GEVENT CPGTriggerSys::RegisterEvent( UINT id, UINT type,
								   DWORD cycle, DWORD life,
                                   EventConditionCallBack condition,
			                       EventActionCallBack action )// cycle is circuity event
{
	CPGEvent* e = new CPGEvent( id, type, 
								cycle, life,
								condition, action );
	m_pEventVec.push_back( e );

}

//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
GEVENT CPGTriggerSys::RegisterEvent( UINT id, UINT type,
							         LG::BBoxf bound,
								     DWORD cycle, DWORD life,
                                     EventConditionCallBack condition,
			                         EventActionCallBack action )// cycle is circuity event

{
	CPGEvent* e = new CPGEvent( id, type, 
		                        bound, 
								cycle, life,
								condition, action );
	m_pEventVec.push_back( e );
	return;
}

//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
GEVENT CPGTriggerSys::RegisterEvent( UINT id, UINT type,
							         LG::Spheref bound,
								     DWORD cycle, DWORD life,
                                     EventConditionCallBack condition,
			                         EventActionCallBack action )// cycle is circuity event

{
	CPGEvent* e = new CPGEvent( id, type, 
		                        bound, 
								cycle, life,
								condition, action );
	m_pEventVec.push_back( e );
	return;
}


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
GEVENT CPGTriggerSys::RegisterEvent( UINT id, UINT type,
	                                 LG::Plane3df bound,
                                     DWORD cycle, DWORD life,
                                     EventConditionCallBack condition,
			                         EventActionCallBack action )// cycle is circuity event
{
	CPGEvent* e = new CPGEvent( id, type, 
		                        bound, 
								cycle, life,
								condition, action );
	m_pEventVec.push_back( e );
	return;


}



//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
void CPGTriggerSys::RemoveEvent( UINT id )
{
    for( UINT i=0; i< m_pEventVec.size(); i++ )
	{
		if( m_pEventVec[i]->GetID() == id )
		{
			delete m_pEventVec[i];
		    m_pEventVec.erase( m_pEventVec.begin()+i );
		}
	}
}


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
UINT CPGTriggerSys::GetEventCount( void )
{
	return m_pEventVec.size();
}

//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
CPGEvent* CPGTriggerSys::GetEvent( int i )
{
	return m_pEventVec[i];
}


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
void CPGTriggerSys::TouchTest( LG::Spheref s, void* who )
{
    for( UINT i=0; i< m_pEventVec.size(); i++ )
	{
		event_t event=m_pEventVec[i]->isTouch(s, who);
	}
}


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
void CPGTriggerSys::Update( void )
{
	for( UINT i=0; i< m_pEventVec.size(); i++ )
	{
		if( !m_pEventVec[i]->isDeath() )
		{
			delete m_pEventVec[i];
			m_pEventVec.erase( m_pEventVec.begin()+i );
			i--;
			continue;
		}
		m_pEventVec[i]->Update();
	}
}


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================

class CPTriggerScript : public CPGSPFile
{
public :

	CPGTriggerSys* m_TriggerSys;
	getConditionFuncCallBack  m_ConditionCB;
	getActionFuncCallBack     m_ActionCB;

    virtual void* OnCreateObject( const char* Name )
	{
		if( strcmp( Name, "EVENT" ) == 0 )
		{
		    TriggerSys::HEvent* e = new TriggerSys::HEvent;
			return e;
		}

		if( strcmp( Name, "TRIGGER" ) == 0 )
		{
		    return m_TriggerSys;
		}
		return NULL;
	}


    virtual bool OnReadAttribute( const char* Name,
                                  void* obj,
                                  char* Attribute,
                                  char* Value )
	{

		if( strcmp( Name, "EVENT" ) == 0 )
		{
			TriggerSys::HEvent* e = (TriggerSys::HEvent*)obj;
			if( strcmp( Attribute, "ID" ) == 0 )
			{
				e->id = atoi( Value );
			}
			else if( strcmp( Attribute, "TYPE" ) == 0 )
			{
				e->type = atoi( Value );
			}
			else if( strcmp( Attribute, "LIVETIME" ) == 0 )
			{
				if( strcmp( Value, "LIVE_FOREVER" ) == 0 )
					e->liveTime = LIVE_FOREVER;
				else
					e->liveTime = atoi( Value );
			}
			else if( strcmp( Attribute, "CYCLETIME" ) == 0 )
			{
				e->cycleTime = atoi( Value );
			}
			else if( strcmp( Attribute, "CONDITIONID" ) == 0 )
			{
				if( m_ConditionCB != NULL )
				    e->m_funcCondition  = m_ConditionCB( atoi( Value ) );
			}
			else if( strcmp( Attribute, "ACTIONID" ) == 0 )
			{
				if( m_ActionCB != NULL )
				    e->m_funcAction  = m_ActionCB( atoi( Value ) );

			}
			else if( strcmp( Attribute, "SPHERE" ) == 0 )
			{
				float cx = (float)atof( strtok( Value, "," ) );
				float cy = (float)atof( strtok( NULL,  "," ) );
				float cz = (float)atof( strtok( NULL,  "," ) );
				float r  = (float)atof( strtok( NULL,  "," ) );
				e->shape.BeSpheref( cx, cy, cz, r );
			}

			else if( strcmp( Attribute, "BBOX" ) == 0 )
			{
				float x0 = (float)atof( strtok( Value, "," ) );
				float y0 = (float)atof( strtok( NULL,  "," ) );
				float z0 = (float)atof( strtok( NULL,  "," ) );
				float x1 = (float)atof( strtok( NULL, "," ) );
				float y1 = (float)atof( strtok( NULL,  "," ) );
				float z1 = (float)atof( strtok( NULL,  "," ) );
				e->shape.BeCubef( x0, y0, z0, x1, y1, z1 );
			}

			else if( strcmp( Attribute, "PLANE" ) == 0 )
			{
				float nx = (float)atof( strtok( Value, "," ) );
				float ny = (float)atof( strtok( NULL,  "," ) );
				float nz = (float)atof( strtok( NULL,  "," ) );
				float c  = (float)atof( strtok( NULL,  "," ) );
				e->shape.BePlane3df( nx, ny, nz, c );
			}

		}
		return true;

	}


	virtual void OnCloseObject( const char* Name, void* obj )
	{
		if( strcmp( Name, "EVENT" ) == 0 )
		{
			TriggerSys::HEvent* e = (TriggerSys::HEvent*)obj;
			if( m_TriggerSys!=NULL )
			{
				switch( e->shape.GetType() )
				{
				case _P_SPHEREF_:
				m_TriggerSys->RegisterEvent( e->id, e->type,
					                         *( e->shape.Spheref() ),
		                                     e->cycleTime, e->liveTime,
											 e->m_funcCondition,
											 e->m_funcAction );
				break;

				case _P_CUBE_:
				m_TriggerSys->RegisterEvent( e->id, e->type,
					                         *( e->shape.Cubef() ),
		                                     e->cycleTime, e->liveTime,
											 e->m_funcCondition,
											 e->m_funcAction );
				break;

				case _P_PLANE3DF_:
					{
				        m_TriggerSys->RegisterEvent( e->id, e->type,
							                         *( e->shape.Plane3df() ),
		                                             e->cycleTime, e->liveTime,
								                     e->m_funcCondition,
											         e->m_funcAction );
					}
				break;
				}



			}
		}
	
	}

};


error_t CPGTriggerSys::Save( Pcstr filename,
		                    getConditionIDCallBack condition,
	                        getActionIDCallBack    action )

{

	char message[255];
	CPTriggerScript savef;

	savef.Open( filename );

	savef.CreateObject( "TRIGGER" );

	for( UINT i = 0; i < GetEventCount(); i++ )
	{
		savef.CreateObject( "EVENT" );
		CPGEvent* event = GetEvent(i);

#ifdef _PS2
		//marked by gino. PS2 lack _itoa func.
		sprintf(message, "%d", event->GetID());			savef.SetReadAttribute( "ID", message );  		
		sprintf(message, "%d", event->GetType());		savef.SetReadAttribute( "TYPE", message);
		sprintf(message, "%d", event->GetLiveTime());	savef.SetReadAttribute( "LIVETIME", message);
		sprintf(message, "%d", event->GetCycleTime());	savef.SetReadAttribute( "CYCLETIME", message);
		sprintf(message, "%d", condition( event ));		savef.SetReadAttribute( "CONDITIONID", message);
		sprintf(message, "%d", action( event ));		savef.SetReadAttribute( "ACTIONID", message);
#else
		savef.SetReadAttribute( "ID", itoa( event->GetID(), message, 10 ) ); 
		savef.SetReadAttribute( "TYPE", itoa( event->GetType(), message, 10 ) ); 
		savef.SetReadAttribute( "LIVETIME", itoa( event->GetLiveTime(), message, 10 ) ); 
		savef.SetReadAttribute( "CYCLETIME", itoa( event->GetCycleTime(), message, 10 ) ); 
		savef.SetReadAttribute( "CONDITIONID", itoa( condition( event ), message, 10 ) ); 
		savef.SetReadAttribute( "ACTIONID",    itoa( action( event ), message, 10 ) ); 
#endif	
		//write evnet shape
		switch( event->GetShape()->GetType() )
		{
		case _P_SPHEREF_:
			sprintf( message, "%.2f, %.2f, %.2f, %.2f", event->GetShape()->Spheref()->center.x,
				                                    event->GetShape()->Spheref()->center.y,
													event->GetShape()->Spheref()->center.z,
													event->GetShape()->Spheref()->radius );
			savef.SetReadAttribute( "SPHERE",  message ); 
			break;
		case _P_CUBE_:
			sprintf( message, "%.2f, %.2f, %.2f, %.2f, %.2f, %.2f",
				              event->GetShape()->Cubef()->vertex[0].x,
				              event->GetShape()->Cubef()->vertex[0].y,
							  event->GetShape()->Cubef()->vertex[0].z,
  				              event->GetShape()->Cubef()->vertex[1].x,
				              event->GetShape()->Cubef()->vertex[1].y,
							  event->GetShape()->Cubef()->vertex[1].z );
			savef.SetReadAttribute( "BBOX", message ); 
			break;
		case _P_PLANE3DF_:
			sprintf( message, "%.2f, %.2f, %.2f, %.2f", event->GetShape()->Plane3df()->normal.x,
				                                    event->GetShape()->Plane3df()->normal.y,
													event->GetShape()->Plane3df()->normal.z,
													event->GetShape()->Plane3df()->cnst );
			savef.SetReadAttribute( "PLANE",  message ); 
			break;
		}

		savef.CloseObject();
	}
	savef.CloseObject();

	/*
	CPFile savef;
	
	savef.open( filename, "wb" );

	savef.write( &TriggerSys::Name, sizeof( DWORD ), 1 );

	UINT eventcount = m_pEventVec.size();
	savef.write( &eventcount, sizeof( UINT ), 1 );

	for( UINT i = 0; i < m_pEventVec.size(); i++ )
	{
		TriggerSystem::HEvent event;
		memset( &event, 0XFF, sizeof( TriggerSystem::HEvent ) );
		event.ID = m_pEventVec[i]->GetID();
		event.type = m_pEventVec[i]->getType();
		event.LiveTime = m_pEventVec[i]->getLiveTime();
		event.cycleTime = m_pEventVec[i]->getCycleTime();

		if( condition != NULL )
		    event.conditionID = condition( m_pEventVec[i] );

		if( action != NULL )
		    event.actionID = action( m_pEventVec[i] );

		event.shapetype = m_pEventVec[i]->getType();

		//write evnet shape
		switch( event.shapetype )
		{
		case _P_SPHERE_:
			memcpy( event.shapedata, m_pEventVec[i]->getShape()->Spheref(), sizeof( LG::Spheref ) );
			//savef.write( m_pEventVec[i]->getShape()->Spheref(), sizeof( LG::Spheref ), 1 );
			break;
		case _P_CUBE_:
			memcpy( event.shapedata, m_pEventVec[i]->getShape()->Cube(), sizeof( CPCube ) );
			//savef.write( m_pEventVec[i]->getShape()->Cube(), sizeof( CPCube ), 1 );
			break;
		}

		savef.write( &event, sizeof( TriggerSystem::HEvent ), 1 );
	}
	savef.close();
	*/

	return 1;
}


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
error_t CPGTriggerSys::Load( Pcstr filename,
		                    getConditionFuncCallBack condition,
					        getActionFuncCallBack    action	)

{

	CPTriggerScript tf;
	tf.m_ConditionCB = condition;
	tf.m_ActionCB    = action;
	tf.m_TriggerSys = this;
	tf.Load( filename );
	return 1;
	/*
	CPFile loadf;

	savef.open( filename, "rb" );

	DWORD name;
	loadf.read( &name, sizeof( DWORD ), 1 );
	if( name != TriggerSys::Name )
		return -1;

	UINT eventcount=0;
	loadf.read( &eventcount, sizeof( UINT ), 1 );

	for( UINT i = 0; i < eventcount; i++ )
	{
		TriggerSystem::HEvent event;

		loadf.read( &event, sizeof( TriggerSystem::HEvent ), 1 );

  		switch( m_pEventVec.getType() )
		{
		case _P_SPHERE_:
			savef.write( _P_SPHERE_, sizeof( DWORD ), 1 );
			savef.write( m_pEventVec[i]->getShape()->Spheref(), sizeof( LG::Spheref ), 1 );
			break;
		case _P_CUBE_:
			savef.write( _P_CUBE_, sizeof( DWORD ), 1 );
			savef.write( m_pEventVec[i]->getShape()->Cube(), sizeof( CPCube ), 1 );
			break;
		case default:
			savef.write( _P_SHAPELESS_, sizeof( DWORD ), 1 );
			break;
		}


		CPGEvent* e = new CPGEvent( event.ID, event.Type, 
								    event.cycle, event.LiveTime,
								condition, action );
        m_pEventVec.push_back( e );

	}

	savef.close();
	*/
	return 1;

}

