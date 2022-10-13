
#ifndef ITEMARRAY2D_H
#define ITEMARRAY2D_H


#include "Static2d.h"
#include "Button2d.h"

namespace GSO
{
    namespace UI
    {
        // 有規則排列的一堆button
        class ItemArray2d : public IWidget2d
        {
            typedef IWidget2d BaseClass;

        private:

            // 更新所有子元件的位置
             LG::Size                 m_Amount;            // 二維陣列的xy
            LG::Size                 m_Space;            // 每個子元件的間距，分x間距和y間距
            LG::Size                 m_ItemSize;        // 每個子元件的大小，以pixel計
            IWidget2d*               m_FocusItem;        // 上一個致能元件的id
            Button2d*                m_Mask;
            std::vector<IWidget2d*>  m_InsertSeq;

            virtual void        UpdateCellPos(void);
            // 複寫GetItem 以免取到他的子ITEM
            virtual Element2d*    GetChild(entityid_t index){ return NULL; }

            // 這裡不給用，因為兩個陣列
            bool                 AttachItem(IWidget2d* item);
            bool                 DetachItem(IWidget2d* item);

        public:
            
            ItemArray2d();
            ~ItemArray2d();
        
            const tchar_t*        ObjectName() const        { return _T("ItemArray2d");}

            // 設定其陣列大小，並初始空間
            void                Create(int x, int y, const char* subtype=NULL);
            bool                InsertCell(uint_t x, uint_t y, IWidget2d* w);
            void                RemoveCell(IWidget2d* w);
            IWidget2d*          operator[](uint_t i);


            IWidget2d*          GetCell(uint_t x, uint_t y);
            IWidget2d*          GetCell(GameVertex mousepos);
            bool                IsCell (IWidget2d* w);

            void                SetCellDragable(bool t);
            void                SetCellAcceptDragIn(bool t);


            // 把前景的圖全部release
            void                ClearAllCellImage(void);

            // 和create成對，將之前new出來的空間釋放
            void                DestroyAllItem(void);

            // 更新狀態
            int                 Update(int count);

            // 取得某個(by index)btn上的字
            void                SetText(uint_t x, uint_t y, tchar_t* str, uint_t index=0);

            // 取得某個(by index)btn上的字
            void                SetText(uint_t id, tchar_t* str, uint_t index=0);
            
            // 取得某個(by index)btn上的字
            const tchar_t*      GetText(uint_t x, uint_t y, uint_t index=0);

            // 將前景全部載入同一張圖
            virtual void        LoadAllCellRes(const tchar_t* filename);
            virtual void        LoadAllCellAnm(const tchar_t* filename);

            // 對指定對象進行載入的動作，如果傳入null字串，則釋放該指定對象(ps，如果傳空字串，則指定對象不改變)
            void                LoadCellRes(uint_t x, uint_t y, tchar_t* filename);
            void                LoadMaskRes(tchar_t* filename);
            void                LoadAllHint(tchar_t* resfname, tchar_t* anmfname);
            void                LoadAllDesc(tchar_t* filename);

            pfstd::CString      GetMaskRes(void);

            LG::Size            GetAmount(void);

            LG::Size            GetSize(void);
            
            // 設定子元件間距
            virtual void        SetSpace(int x, int y);
            LG::Size            GetSpace(void);

            virtual void        SetItemSize(int cx, int cy);
            LG::Size            GetItemSize(void);

            /**
             * 將自已推入待繪製的佇列中。
             * @param container 待繪製佇列。
             */
            virtual void        OnRenderTo(rendercontainer_t &container);

            // 取得致能子元件的index
            virtual entityid_t  GetFocusIndex(void);

            virtual bool        SaveCfg(GspNode *toNode);


        protected:
            // Input事件，客端程式請勿覆寫，寫底層子元件才可覆寫
//             virtual void        OnNull      (LG::Point point)            {}
//             virtual void        OnKeyDown    (int key)                    {} 
//             virtual void        OnKeyUp        (int key)                    {}
//             virtual void        OnMouseOver    (int key, LG::Point point){}
//             virtual void        OnLMouseDown(int key, LG::Point point){}
//             virtual void        OnRMouseDown(int key, LG::Point point){}
//             virtual void        OnLMouseUp    (int key, LG::Point point){}
//             virtual void        OnRMouseUp    (int key, LG::Point point){}
//             virtual void        OnLDBClick    (int key, LG::Point point){}
//             virtual void        OnRDBClick    (int key, LG::Point point){}
//             virtual void        OnDrag        (LG::Point point)            {}
//             virtual void        OnDragDrop    (LG::Point point)            {}

        private:
            // 內部往上派送機制，客端程式僅能覆寫Dialog的以下界面
            // 任何子元件不需要覆寫，通常是在編寫複合元件時，才會有複寫的必要
            // 在複合元件的覆寫，是為了要攔劫自己的子元件被觸發時的訊息
            // 但還是要往上送，所以要再呼叫base_class的onxxx
            virtual void        OnNormal    (IWidget2d* pWidget, LG::Point MouseSite);
            virtual void        OnKeyDown   (IWidget2d* pWidget, int KeyCode);
            virtual void        OnKeyUp     (IWidget2d* pWidget, int KeyCode);
            //virtual void        OnMouseMove (IWidget2d* pWidget, int KeyCode, LG::Point MouseSite);
            virtual void        OnMouseOver (IWidget2d* pWidget, int KeyCode, LG::Point MouseSite);
            virtual void        OnLDBClick  (IWidget2d* pWidget, int KeyCode, LG::Point MouseSite);
            virtual void        OnRDBClick  (IWidget2d* pWidget, int KeyCode, LG::Point MouseSite);
            virtual void        OnLMouseDown(IWidget2d* pWidget, int KeyCode, LG::Point MouseSite);
            virtual void        OnRMouseDown(IWidget2d* pWidget, int KeyCode, LG::Point MouseSite);
            virtual void        OnLMouseUp  (IWidget2d* pWidget, int KeyCode, LG::Point MouseSite);
            virtual void        OnRMouseUp  (IWidget2d* pWidget, int KeyCode, LG::Point MouseSite);
            virtual void        OnDrag      (IWidget2d* pWidget, LG::Point MouseSite);
            virtual void        OnDragDrop  (IWidget2d* pWidget, LG::Point MouseSite);

            virtual bool        OnCollision(GameVertex pos);

        };
    };
};




#endif