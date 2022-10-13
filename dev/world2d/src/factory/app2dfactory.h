//Elm_Editer.h headle file of Elm_Editer.cpp
//
//  element edit function
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
//                                                         Pin
//

#ifndef _ELEMENT2DFACTORY_H
#define _ELEMENT2DFACTORY_H

#include <stdio.h>
#include <vector>

#include <graphic/VMCGDI.h>     /*source*/
#include <game/HElem.h>               /*source*/
#include <game/Element2d.h>
#include <game/UI/Form2d.h>
#include <game/Terrain2d.h>

//#include "..\2DGame\UI\Form2d.h"
#include "WorldObserver.h"

typedef bool (*ForAllChildrenWidgetCB)(GSO::UI::IWidget2d* iwidget, void* pdata);


enum BlitMode
{
    _BLIT_M          =  0,
    _DARKEN_M        =  1,
    _LIGHTEN_M       =  2,
    _SEAM_M          =  3,
    _INTERLACTED_M   =  4,
    _WARP_M          =  5,
    _ALPHA_M         =  6,
    _ALPAH_DARKEN_M  =  7,
    _ALPHA_LIGHTEN_M =  8,
    _ALPHA_WARP_M    =  9,
    _ALPHA_RIPPLE_M  = 10
};



namespace App2dFactory
{
    //////////////////////////////////////////////////////////////////////////
    //

    class Element2d
    {
    public:

        Element2d();
        ~Element2d();
		
		static bool CheckFormat(const char* filename);

        static int  GetAllImage(GSO::Element2d* pElement, std::vector<CPIcon*>& Image);
        static int  GetAllImage(const char* filename, std::vector<CPIcon*>& Image);        
    };

    

    //////////////////////////////////////////////////////////////////////////
    //

    class Widget
    {

    public:

        Widget();
        ~Widget();

        //從上往下找
        static void      SetAllChildrenEnable(GSO::UI::IWidget2d* bu, bool v);
        static void      SetAllChildrenVisible(GSO::UI::IWidget2d* bu, bool v);
        static void      SetAllChildrenDragable(GSO::UI::IWidget2d* bu, bool v);
        static void      SetAllChildrenMoveable(GSO::UI::IWidget2d* bu, bool v);

        static bool      ForAllChildrenT2B(GSO::UI::IWidget2d* root, ForAllChildrenWidgetCB ccd, void* pdata);
        static bool      ForAllChildrenB2T(GSO::UI::IWidget2d* root, ForAllChildrenWidgetCB ccd, void* pdata);

        static LG::Point GetLClickRef(GSO::UI::IWidget2d* bu);
    };


    class Form2d: public GSO::UI::Form2d
    {
        GSO::UI::Dialog2d* m_pDialog;

    public:

        Form2d();
        ~Form2d();

        //const char* ObjectName() const { return "Form2d";}

        int                 LoadCfg(const char* filename);
        int                 SaveCfg(const char* filename);
        void                Destroy();
        const char*         GetCfgFilename();

        bool                IsCreate();

        bool                AttachItem(IWidget2d* item);

        void CALLBACK       ProcB(POINT point, UINT uMsg, UINT wParam);

        void                ScrollTo(GameVertex new_norm);

        void                SaveData(void);
        //void Save_Image(void); //傳回 存了幾個
        int                 SaveAllWidget(void);

        //GameVertex          Get_Relative_pos (void){ return GSO::World::IEntity2d::GetRefOrigin()-GetPos(); }

        GSO::UI::IWidget2d* GetEntityByFname(const char* fname);
        GSO::UI::IWidget2d* GetFocusEntity(void)   { return GetFocusWidget(); }
        UINT                WidgetCount(void)      { return GetItemCount(); }

        static GSO::UI::IWidget2d* CloneWidget(GSO::UI::IWidget2d* pWidget, bool bCopyFile=false);



        void                GetAllResFilename(std::vector<std::string>& fnamelist);
        void                GetAllAnmFilename(std::vector<std::string>& fnamelist);

        // By Id
        void                Sort();

        //static bool         s_bSelLock;

        //CP2DElement*  Object           (int id) { return CPEElm(operator[](id)); }
    };

    
    //////////////////////////////////////////////////////////////////////////
    //


    class Terrain2d: public GSO::World::Terrain2d
    {
        bool                           m_Askfileoverwrite;
        std::vector<CPEWorldObserver*> m_Observers;
        int                            m_Rdir;
        std::vector<int>               m_RotateDir;

    public:

        Terrain2d();
        virtual ~Terrain2d();

        //void Save_Image(void);
        int                 SaveAllEntity(void);
                            
        void CALLBACK       Scroll_Query(POINT point, UINT uMsg, UINT wParam);
        int  CALLBACK       Drag_Role_Query(POINT point, UINT uMsg, UINT wParam);
        void CALLBACK       Move_Role_Query(POINT point, UINT uMsg, UINT wParam);

        GSO::World::Role2d* PickingQuery(GameVertex screenPos);

        GSO::World::Role2d* Object(int id) { return (GSO::World::Role2d*)operator[](id); }

        virtual void        OnReleaseCell(int Id,
                                          GSO::StreamCellDescriptor pCellDescriptor,
                                          void* pData,
                                          void* pUserdata);

        void                HookObserver(CPEWorldObserver* observer) { m_Observers.push_back(observer); }
        void                UnhookObserver(CPEWorldObserver* observer) { /* TODO: Alex, who need this, implement it!!*/ }
        void                SetRotateDir(int dir);
        void                GetRotateDir(std::vector<int>& dirlist);
                           
        bool                GetAskOverwrite(void)   { return m_Askfileoverwrite; }
        void                SetAskOverwrite(bool f) { m_Askfileoverwrite=f;}

        GSO::World::IEntity2d* GetTriggerEntity(LG::Point);

        bool  m_Save;

    };


    //////////////////////////////////////////////////////////////////////////
    //

    class Role2d : public GSO::World::Role2d
    {

    public:

        //int                 Get_Judg_Dir             (GameVertex pt)   { return 0; }
        //int                 GetWantToDoSize        (void)      { return (m_WantToDo.empty()) ? 0  : m_WantToDo.size(); }
        //int                 GetWantToDo            (int i)     { return (m_WantToDo.empty()) ? -1 : m_WantToDo[i]; }
        //int                 GetNowDoIndex          (void)      { return (m_WantToDo.empty()) ? -1 : m_WantToDo.GetIndex(); }

        int                 GetRouteSize           (void)      { return m_Route.empty() ? 0 : m_Route.size(); }
        GameVertex          GetRouteSitus          (int i)     { GameVertex pt; return (m_Route.empty()) ? pt : m_Route[i]; }
        //pt::arraymap<int>*  GetWeightMap           (void)      { return &m_WeightMap; }


    };

    class Building2d : public GSO::World::Building2d
    {

    public:

        //int                 Get_Judg_Dir             (GameVertex pt)   { return 0; }
        //int                 GetWantToDo            (int i)     { return (m_WantToDo.empty()) ? -1 : m_WantToDo[i]; }
    };

    class Render2d : public LGB::VMCRender2d
    {

    public:

        Render2d();
        ~Render2d();

        LGB::VPage*  GetVMCDevice()           {return (LGB::VPage*)LGB::VMCRender2d::GetDrawDevice();}

        void         FullCanvas(DWORD color)  {GetVMCDevice()->FullCanvas(color);}

        Pmgc         GetMemGC(void)           {return GetVMCDevice()? GetVMCDevice()->GetMemGC(): 0;}

        LG::Rect     GetGCRect(void)		  {return GetVMCDevice()? GetVMCDevice()->PaintGCRect(): LG::Rect();}

        LG::Size     GetGCSize(void)   	      {return GetVMCDevice()? GetVMCDevice()->PaintGCSize(): LG::Size();}

        void         Flip(void* ScreenDC)                      {GetVMCDevice()? GetVMCDevice()->Flip(ScreenDC): 0;}
        void         Flip(void* ScreenDC, LG::Point point)     {GetVMCDevice()? GetVMCDevice()->Flip(ScreenDC, point): 0;}

        void         Text(int x, int y, COLORREF col, LPCTSTR string, int iBkMode=TRANSPARENT)    {GetVMCDevice()? GetVMCDevice()->Text(x, y, col, string, iBkMode): 0;}

        CPIcon*      GetCanvas(void)          {return LGB::VMCRender2d::GetCanvas();}


        int          AlphaScaleBlit(const Icon *Src, LG::Point point, LG::Size ssize);
        int          SpceialEffectBlit(CPIcon* Src, LG::Point point, BlitMode mode=_ALPHA_M, int argument1=0, int argument2=0);
    };
};




#endif