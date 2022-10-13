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

//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-string.h>
#include <parser/pgsp_file.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "terrain2d.h"

namespace GSO
{
    namespace World
    {

        //=============================================================================
        // NAME: LoadScript()
        // DESC: Load script language file
        //=============================================================================
        class GspTerrainConfigure : public GspReadCallback
        {
            Terrain2d* pTerrain;
            
            struct SectorData
            {
                uint_t Id;
                //tchar_t zspFile[_MAX_LPATH];
            };

        public:
            GspTerrainConfigure(Terrain2d* terrain){ pTerrain=terrain; }
            ~GspTerrainConfigure(void){}

            void* OnCreateObject(const tchar_t* name, const tchar_t* description, void* parent)
            {
                if(strcmp(name, _T("2DTERRAIN")) == 0)
                {
                    return pTerrain;
                }
                if(strcmp(name, _T("2DZONE")) == 0)
                {
                    SectorData* s = new SectorData;
                    return s;
                }
                if(strcmp(name, _T("2DROLE")) == 0)
                {
                    GSO::World::Role2d* role = new GSO::World::Role2d;
                    return role;
                }
                return NULL;
            }
            
            void  OnReadAttribute(const tchar_t* name,
                                  void* obj,
                                  const tchar_t* attribute,
                                  const tchar_t* oper,
                                  const tchar_t* value)
            {
                if(strcmp(name, _T("2DTERRAIN")) == 0)
                {
                    //TerrainData* t = (TerrainData*) obj;
                    if(strcmp(attribute, _T("SEGMENT")) == 0)
                    {
                        tchar_t* buf  = new tchar_t[strlen(value)+1];
                        strcpy(buf, value);
                        int originX   = atoi(strtok(buf, ","));
                        int originY   = atoi(strtok(NULL, ","));
                        int cx        = atoi(strtok(NULL, ","));
                        int cy        = atoi(strtok(NULL, ","));
                        int rw        = atoi(strtok(NULL, ","));
                        int rh        = atoi(strtok(NULL, ","));
                        int gridW     = atoi(strtok(NULL, ","));
                        int gridH     = atoi(strtok(NULL, ","));
                        int gridShape = atoi(strtok(NULL, ","));

                        pTerrain->Create(LG::Point(originX, originY), cx, cy, rw, rh, gridW, gridH, gridShape);
                        delete [] buf;
                    }
                    else if(strcmp(attribute, _T("NAME")) == 0)
                    {
                        pTerrain->SetMapName(value);
                    }
                }
                else if(strcmp(name, _T("2DZONE")) == 0)
                {
                    SectorData* s = (SectorData*) obj;
                    if(strcmp(attribute, _T("ID")) == 0)
                    {
                        s->Id = atoi(value);
                    }
                    /*
                    else if(strcmp(attribute, "LOAD") == 0)
                    {
                        _makepath(s->zspFile, NULL, Sprite2d::GetCurrentDirectory(), value, NULL);
                        //mcurrentpath(s->zspFile);
                    }
                    */
                }
                else if(strcmp(name, _T("2DROLE")) == 0)
                {
                    GSO::World::Role2d* role =  (GSO::World::Role2d*)obj;
                    if(strcmp(attribute, _T("X")) == 0)
                    {
                        role->GetPos().x = atoi(value);
                    }
                    else if(strcmp(attribute, _T("Y")) == 0)
                    {
                        role->GetPos().y = atoi(value);
                    }
                    else if(strcmp(attribute, _T("LOADRES")) == 0)
                    {
                        role->LoadRes(value);
                    }
                    else if(strcmp(attribute, _T("LOADANM")) == 0)
                    {
                        role->GSO::Element2d::LoadAnm(value);
                    }

                    else if(strcmp(attribute, _T("DATABASE")) == 0)
                    {
                        tchar_t* buf = new tchar_t[strlen(value)+1];
                        strcpy(buf, value);
                        tchar_t* name  = strtok(buf, ",");
                        uint_t  entry = atoi(strtok(NULL, ","));
                        role->SetDataBaseName(name);
                        role->SetDataBaseEntry(entry);
                        delete [] buf;
                    }
                }
            };


            void OnCloseObject(const tchar_t* name, void* obj)
            {
                if(strcmp(name, _T("2DZONE")) == 0)
                {
                    SectorData* s = (SectorData*) obj;

                    /*
                    //代代換新節點此2斷為轉檔用
                    tchar_t buf[255];
                    CPGspParser* loader = new CPGspParser;
                    int result = loader->Load(s->zspFile);
                    CPGspParser* conf = (CPGspParser*)pTerrain->m_ConfigureIO;
                    GspNode* node = GetCurNode();
                    GspNode* nnode = (*loader)[0];
                    (*loader)[0]->WriteCommand("@ID", "=", itoa(s->Id, buf, 10));
                    conf->ReplaceNode(node,  loader);
                    delete node;
                    int ii = conf->GetChildrenCount();
                    */

                    //GspNode* node = GetCurNode();
        
                    pTerrain->DataStream::InsertCell(GetCurNode(), s->Id);
                    SAFE_DELETE(s);
                }

                if(strcmp(name, _T("2DROLE")) == 0)
                {
                    GSO::World::Role2d* role = (GSO::World::Role2d*)obj;
                    pTerrain->JoinEntity(role, role->GetPos());
                }
            };
        };
        
    };//World
};//GSO
