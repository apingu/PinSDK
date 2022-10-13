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
#ifndef IWIDGET_H
#define IWIDGET_H

#include <pf-cstring.h>
#include <element2d.h>
#include <text_node.h>
//#include "vmc-render2d.h"

class GspNode;

namespace GSO
{
    namespace UI
    {
        typedef int  uicomponent_t; //UI
        
        enum uibehave_t { kStateNormal = 0,
                          kStateCross,
                          kStatePressDown,
                          kStateDisable,
                          kStateMax,
                          kStateUserDef,
                        };
                        
        enum POPUP_TYPE
        {
            POPUP_TYPE_NONE  = 0,  // 一般
            POPUP_TYPE_FADE  = 1,  // 淡入淡出
            POPUP_TYPE_ZOOM  = 2,  // 縮放
        };
        
        class IWidget2d;
        
        class IEventDelegate
        {
        public:
            // 內部往上派送機制，客端程式僅能覆寫Dialog的以下界面
            // 任何子元件不需要覆寫，通常是在編寫複合元件時，才會有複寫的必要
            // 在複合元件的覆寫，是為了要攔劫自己的子元件被觸發時的訊息
            // 但還是要往上送，所以要再呼叫base_class的onxxx
            virtual void          OnNormal    ( IWidget2d* pWidget, LG::Point MouseSite ) {};
            virtual void          OnKeyDown   ( IWidget2d* pWidget, int KeyCode ) {};
            virtual void          OnKeyUp     ( IWidget2d* pWidget, int KeyCode ) {};
            virtual void          OnChar      ( IWidget2d* pWidget, unsigned nChar ) {};
            virtual void          OnMouseWheel( IWidget2d* pWidget, int key, LG::Point point ) {};
            virtual void          OnMouseOver ( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite ) {};
            virtual void          OnLDBClick  ( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite ) {};
            virtual void          OnRDBClick  ( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite ) {};
            virtual void          OnLMouseDown( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite ) {};
            virtual void          OnRMouseDown( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite ) {};
            virtual void          OnLMouseUp  ( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite ) {};
            virtual void          OnRMouseUp  ( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite ) {};
            virtual void          OnDrag      ( IWidget2d* pWidget, LG::Point MouseSite ) {};
            virtual void          OnDragDrop  ( IWidget2d* pWidget, LG::Point MouseSite ) {};
            virtual void          OnDock      ( IWidget2d* pWidget, dword_t docktime, LG::Point point ) {};
            virtual void          OnShow      ( IWidget2d* pWidget, bool f ) {};
            virtual void          OnUserNotify( IWidget2d* pWidget, uint_t uMsg, uint_t Param1, uint_t Param2, LG::Point point ) {};
            
            virtual void          OnMoving    ( IWidget2d* pWidget, const GameVertex pos ) {};
            
            //leap over, if want to use, must SetTestLODrag first
            virtual void          OnLODragIn  ( IWidget2d* pWidget, IWidget2d* sender, LG::Point MouseSite ) {};
            // Sean: 最後一個參數表示是否有被其它元件接到
            virtual void          OnLODragOut ( IWidget2d* pWidget, IWidget2d* sendobj, LG::Point MouseSite, bool is_catched ) {};
        };
        
        
        class WidgetEffect;
        
        class IWidget2d : public Element2d, public IEventDelegate, public cText_Node
        {
        private:
        
            typedef Element2d BaseClass;
            
            IWidget2d*        m_pHint;
            IWidget2d*        m_pDesc;
            
            bool              m_bTestLODrag;
            pfstd::CString    m_Caption;
            static IWidget2d* s_pFocusWidget;
            bool              m_bModal;
            
        public:
        
            WidgetEffect*     m_pWidgetEffect;
            
        public:
            IWidget2d();
            ~IWidget2d();
            
            const tchar_t*         ObjectName() const    { return _T( "IWidget2d" ); }
            
            // 若要更新資料，請覆寫Update函式，但請記得呼叫父類別的同一函式
            virtual int            Update( int count = -1 );
            
            virtual void           Clear( int s = 0 ); //release res
            
            //allow start the OnLODragIn, OnLODragOut funcion
            virtual void           SetTestLODrag( bool f );
            
            virtual void           OnRenderTo( rendercontainer_t& container );
            virtual void*          OnGetTextNode();
            
            //cText_Node
            virtual const tchar_t* GetText( int index = 0 );
            virtual void           SetText( const tchar_t* text, int index = 0 );
            // Unicode 的場合, 仍然有可能使用到ANSI字串. 所以提供這個
            // 但是, 只能轉譯成統 codepage
            // 至於 Ansi 版本和 GetText 是一樣的
            virtual const tchar_t* GetAnsiText( int index = 0 );
            virtual void           SetAnsiText( const tchar_t* txt, int index = 0 );
            
            // 讀取小黃標籤，就是那個滑鼠不動就會出現的那個啦
            void                   LoadHintRes( const tchar_t* filename );
            void                   LoadHintAnm( const tchar_t* filename );
            void                   SetHintState( uibehave_t state );
            
            void                   SetHintText( const tchar_t* text, int top = 0,
                                                int bottom = 16,
                                                int left = 0,
                                                int right = 100,
                                                int fg = 0xFF000008,
                                                int bg = 0x00,
                                                int sx = 0,
                                                int sy = 2 );
                                                
            const tchar_t*         GetHintText();
            
            
            
            // 說明，也許會有ui需要使用，不過，還沒想到什麼出現的方式最好
            void                   LoadDesc( const tchar_t* filename );
            void                   SetDescText( const tchar_t* text );
            
            
            //all child now we call item in ui system
            IWidget2d*             GetRootItem   ( void );
            IWidget2d*             GetParentItem ( void );
            virtual uint_t           GetItemCount  ( void ) const;
            virtual IWidget2d*       operator[]    ( uint_t i );
            virtual IWidget2d*       GetItem       ( entityid_t index );
            virtual bool           AttachItem    ( IWidget2d* item );
            virtual bool           DetachItem    ( IWidget2d* item );
            virtual void           DestroyAllItem( void );
            virtual bool           ReplaceItem   ( const IWidget2d* oldc, IWidget2d* newc );
            
            void                   SetFocusWidget( void );
            static void            SetFocusWidget( IWidget2d* pWidget );
            static IWidget2d*      GetFocusWidget( void );
            void                   DoModal       ( bool f );
            
            
            
            // 目前所謂的狀態，是以pin的底層對應anm來做，所以，
            // 在這裡要加入anm的filename+path(相對路徑，由目前應用程式的工作路徑算起)
            // 做法為，存在同一個動作的目錄下，但是方向的目錄設定為0,1,2~n，表示不同的狀態
            // 目前沒有實作讓它停在最後一張，然後不變的情形
            // 改變元件(顯示)狀態，請以WidghtState來設定
            void                   SetState( uibehave_t state );
            uibehave_t               GetState( void );
            
            virtual void           SetVisible( bool f );
            virtual void           SetEnable( bool f );
            
            
            virtual void           SetPopupType( POPUP_TYPE popuptype, dword_t popupmsec = 150 );
            virtual POPUP_TYPE     GetPopupType();
            virtual dword_t        GetPopupMsec();
            
            
            
            //! 編輯器存檔用, 要能將本類的屬性加入 node 中
            //! @return: true 表示底下的 node 需要繼續列舉, false 代表不要列舉
            virtual bool           SaveCfg( GspNode* toNode );
            
            bool                   IsStaticId( entityid_t wid );
            
            // 移到同一個層級的最上面
            void                   MoveTop();
            
        public:
            // 內部往上派送機制，客端程式僅能覆寫Dialog的以下界面
            // 任何子元件不需要覆寫，通常是在編寫複合元件時，才會有複寫的必要
            // 在複合元件的覆寫，是為了要攔劫自己的子元件被觸發時的訊息
            // 但還是要往上送，所以要再呼叫base_class的onxxx
            virtual void          OnNormal    ( IWidget2d* pWidget, LG::Point MouseSite );
            virtual void          OnKeyDown   ( IWidget2d* pWidget, int KeyCode );
            virtual void          OnKeyUp     ( IWidget2d* pWidget, int KeyCode );
            virtual void          OnChar         ( IWidget2d* pWidget, unsigned nChar );
            //virtual void OnMouseMove (IWidget2d* pWidget, int KeyCode, LG::Point MouseSite);
            virtual void          OnMouseWheel( IWidget2d* pWidget, int key, LG::Point point );
            virtual void          OnMouseOver ( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite );
            virtual void          OnLDBClick  ( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite );
            virtual void          OnRDBClick  ( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite );
            virtual void          OnLMouseDown( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite );
            virtual void          OnRMouseDown( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite );
            virtual void          OnLMouseUp  ( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite );
            virtual void          OnRMouseUp  ( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite );
            virtual void          OnDrag      ( IWidget2d* pWidget, LG::Point MouseSite );
            virtual void          OnDragDrop  ( IWidget2d* pWidget, LG::Point MouseSite );
            virtual void          OnDock      ( IWidget2d* pWidget, dword_t docktime, LG::Point point );
            virtual void          OnUserNotify( IWidget2d* pWidget, uint_t uMsg, uint_t Param1, uint_t Param2, LG::Point point ); //使用者自定用
            
            //leap over, if want to use, must SetTestLODrag first
            virtual void          OnLODragIn  ( IWidget2d* pWidget, IWidget2d* sender, LG::Point MouseSite );
            // Sean: 最後一個參數表示是否有被其它元件接到
            virtual void          OnLODragOut ( IWidget2d* pWidget, IWidget2d* sendobj, LG::Point MouseSite, bool is_catched );
            
            virtual void          OnShow      ( IWidget2d* pWidget, bool f );
            virtual void          OnMoving    ( IWidget2d* pWidget, const GameVertex pos );
            
        protected:
            // Input事件，客端程式請勿覆寫，寫底層子元件視情況覆寫
            // usually use to change the "image" style
            virtual void          OnNull      ( LG::Point point );
            virtual void          OnKeyDown   ( int key );
            virtual void          OnKeyUp     ( int key );
            virtual void          OnMouseOver ( int key, LG::Point point );
            virtual void          OnMouseWheel( int key, LG::Point point );
            virtual void          OnLMouseDown( int key, LG::Point point );
            virtual void          OnRMouseDown( int key, LG::Point point );
            virtual void          OnLMouseUp  ( int key, LG::Point point );
            virtual void          OnRMouseUp  ( int key, LG::Point point );
            virtual void          OnLDBClick  ( int key, LG::Point point );
            virtual void          OnRDBClick  ( int key, LG::Point point );
            virtual void          OnDrag      ( LG::Point point );
            virtual void          OnDragDrop  ( LG::Point point );
            virtual void          OnDock      ( dword_t docktime, LG::Point point );
            virtual void          OnChar      ( unsigned nChar );
            virtual void          OnUserNotify( uint_t uMsg, uint_t Param1, uint_t Param2, LG::Point point ); //使用者自定用
            
            virtual void          OnShow      ( bool f );
            virtual void          OnMoving    ( const GameVertex& pos ); // when show
            
        protected:
        
            virtual bool          OnCollision( GameVertex pos );
            // 只畫在平面上
            virtual LG::Point3    OnGetRenderPos( void )   { LG::Point pfstl = GetScreenPos(); return pfstl; }
            
        private:
        
            bool                  AttachChild( Element2d* child )  {Element2d::AttachChild( child );}
            bool                  DetachChild( Element2d* child )  {Element2d::DetachChild( child );}
            virtual Element2d*      GetChild( entityid_t index )     {return Element2d::GetChild( index );}
            virtual size32_t      GetChildSize( void ) const       {return Element2d::GetChildSize();}
            bool                  ReplaceChild( const Element2d* oldc, Element2d* newc ) {return Element2d::ReplaceChild( oldc, newc );};
        };
    };
};

#endif