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
//
//
//
#include <parser/pgsp_file.h>
#include "zone2d.h"
#include "terrain2d.h"


namespace GSO
{
    namespace World
    {
        //=============================================================================
        // NAME: LoadScript()
        // DESC: Load script language file
        //=============================================================================
        class GspZoneConfigure : public GspReadCallback
        {
            Zone2d*    m_pZone;
            Terrain2d* p_terrain_;

            /*
            struct SectorData
            {
                SectorData() {sprintf(bgFile,""); sprintf(walkFile,""); sprintf(eventFile,"");}
                uint_t Id;
                tchar_t bgFile[_MAX_LPATH];
                tchar_t walkFile[_MAX_LPATH];
                tchar_t eventFile[_MAX_LPATH];
            };
            */


        public :

            GspZoneConfigure(Zone2d* zone, Terrain2d* terrain){m_pZone=zone; p_terrain_=terrain; }
            ~GspZoneConfigure(){};


            void* OnCreateObject(const tchar_t* name, const tchar_t* description, void* parent)
            {
                if(strcmp(name, _T("2DZONE")) == 0)
                {
                    //SectorData* s = new SectorData;
                    return m_pZone;
                }
                if(strcmp(name, _T("BUILDING")) == 0)
                {
                    GSO::World::Building2d* role = new GSO::World::Building2d;
                    return role;
                }
                if(strcmp(name, _T("TRIGGER")) == 0)
                {
                    GSO::World::Trigger* event = new GSO::World::Trigger();
                    return event;
                }
                return NULL;
            }


            void OnReadAttribute(const tchar_t* name,
                                  void* object,
                                  const tchar_t* attribute,
                                  const tchar_t* oper,
                                  const tchar_t* value)
            {
                if(strcmp(name, _T("2DZONE")) == 0)
                {
                    //SectorData* zone = (SectorData*)object;
                    Zone2d* zone = (Zone2d*)object;
                    if(strcmp(attribute, _T("ID")) == 0)
                    {
                        zone->SetId(atoi(value));
                        //zone->Id = atoi(value);
                    }
                    else if(strcmp(attribute, _T("LOADRES")) == 0)
                    {
                        if(p_terrain_->m_bLoadRes)
                            zone->LoadRes(value);
                        //_makepath(zone->bgFile, NULL, Sprite2d::GetCurrentDirectory(), value, NULL);
                    }
                    else if(strcmp(attribute, _T("GDWALK")) == 0)
                    {
                        tchar_t walkFile[_MAX_LPATH];
                        _makepath(walkFile, NULL, Sprite2d::GetCurrentDirectory(), value, NULL);
                        zone->LoadBoard(WALK_BOARD, walkFile);
                        
                    }
                    else if(strcmp(attribute, _T("GDEVENT")) == 0)
                    {
                        tchar_t eventFile[_MAX_LPATH];
                        _makepath(eventFile, NULL, Sprite2d::GetCurrentDirectory(), value, NULL);
                        zone->LoadBoard(EVENT_BOARD, eventFile);
                    }
                    else if(strcmp(attribute, _T("ORIGIN")) == 0)
                    {
                        tchar_t postion[255];
                        strcpy(postion, value);
                        int x = atoi(strtok(postion, ","));
                        int y = atoi(strtok((tchar_t*) NULL, ","));
                        zone->Sprite2d::SetPos(GameVertex(x, y, 0));
                        //_makepath(zone->bgFile, NULL, Sprite2d::GetCurrentDirectory(), value, NULL);
                    }

                }
                else if(strcmp(name, _T("TRIGGER")) == 0)
                {
                    GSO::World::Trigger* event = (GSO::World::Trigger*)object;
                    if(strcmp(attribute, _T("TYPE")) == 0)
                    {
                        event->type = atoi(value);
                    }
                    else if(strcmp(attribute, _T("SHAPE")) == 0)
                    {
                        event->shape = atoi(value);
                    }
                    else if(strcmp(attribute, _T("LTX")) == 0)
                    {
                        event->range.left = atoi(value);
                    }
                    else if(strcmp(attribute, _T("LTY")) == 0)
                    {
                        event->range.top = atoi(value);
                    }
                    else if(strcmp(attribute, _T("BRX")) == 0)
                    {
                        event->range.right = atoi(value);
                    }
                    else if(strcmp(attribute, _T("BRY")) == 0)
                    {
                        event->range.bottom = atoi(value);
                    }
                    else if(strcmp(attribute, _T("PX")) == 0)
                    {
                        event->pos.x = atoi(value);
                    }
                    else if(strcmp(attribute, _T("PY")) == 0)
                    {
                        event->pos.y = atoi(value);
                    }
                    else if(strcmp(attribute, _T("DATA")) == 0)
                    {
                        strcpy(event->data, value);
                    }
                }
                else if(strcmp(name, _T("BUILDING")) == 0)
                {
                    GSO::World::Building2d* role =  (GSO::World::Building2d*)object;
                    if(strcmp(attribute, _T("ID")) == 0)
                    {
                        role->SetId(atoi(value));
                    }
                    else if(strcmp(attribute, _T("POSITION")) == 0)
                    {
                        tchar_t postion[255];
                        strcpy(postion, value);
                        int x = atoi(strtok(postion, _T(",")));
                        role->Sprite2d::SetPos(GameVertex(x, 
                                                            atoi(strtok(NULL, _T(","))), 0));
                    }
                    else if(strcmp(attribute, _T("X")) == 0)
                    {
                        role->GetPos().x = atoi(value);
                    }
                    else if(strcmp(attribute, _T("Y")) == 0)
                    {
                        role->GetPos().y = atoi(value);
                    }
                    
                    else if(strcmp(attribute, _T("DIR")) == 0)
                    {
                        role->FaceTo(atoi(value));
                    }
                    else if(strcmp(attribute, _T("LOADRES")) == 0)
                    {
                        role->LoadRes(value);
                    }
                    else if(strcmp(attribute, _T("LOADANM")) == 0)
                    {
                        role->LoadAnm(value);
                    }
                    else if(strcmp(attribute, _T("DATABASE")) == 0)
                    {
                        tchar_t* buf = new tchar_t[strlen(value)+1];
                        strcpy(buf, value);
                        tchar_t* name  = strtok(buf, _T(","));
                        uint_t  entry = atoi(strtok(NULL, _T(",")));
                        role->SetDataBaseName(name);
                        role->SetDataBaseEntry(entry);
                        delete [] buf;
                    }
                    else if(strcmp(attribute, _T("RENDERLAYER")) == 0)
                    {
                        role->RenderLayer = (LGB::DrawNode::DrawLayer)atoi(value);
                    }
                }
            }

            
            void OnCloseObject(const tchar_t* name, void* object)
            {
                //if(strcmp(name, "2DZONE") == 0)
                //{
                    //SectorData* s = (SectorData*)object;

                    //m_pZone->SetId(s->Id);
                    //m_pZone->LoadRes(s->bgFile);

                    // Init chessboard data of zone

                    /*
                    FileOperator bf;
                    if(bf.open(s->walkFile, "rb"))
                    {
                        static_cast<CPChessboard*>(m_pZone)->Load(WALK_BOARD, s->walkFile);
                    }

                    if(bf.open(s->eventFile, "rb"))
                    {
                        static_cast<CPChessboard*>(m_pZone)->Load(EVENT_BOARD, s->eventFile);
                    }
                    */

                    //delete s;
                //}
                if(strcmp(name, _T("BUILDING")) == 0)
                {
                    if((m_pZone !=NULL ) && (object !=NULL ))
                    {
                        GSO::World::Building2d* role =  (GSO::World::Building2d*)object;
                        p_terrain_->JoinEntity(role, role->GetPos());
                        // to fix
                        //m_pZone->JoinBuilding(role);
                    }
                }
                else if(strcmp(name, _T("TRIGGER")) == 0)
                {
                    if((m_pZone !=NULL ) && (object !=NULL ))
                    {
                        GSO::World::Trigger* pTrigger = (GSO::World::Trigger*)object;
                        p_terrain_->InsertTrigger(pTrigger);
                    }
                }

                return;
            }
        };
    };//World
};//GSO
