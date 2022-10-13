//Element.h header file of Element.cpp
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

#ifndef ELEMENT2D_H
#define ELEMENT2D_H

#include <pf-event.h>
#include "draw_node.h"
#include "sprite2d.h"
#include "anim2d.h"


//class LGB::VMCText;
class HElem2d;

namespace GSO
{
    class Resource2d;
    class Element2d;
    
    typedef GSO::Element2d* ( _CALLBACK *OnElementChildren )( GSO::Element2d* pElem, void* pData ); // if return is not NULL it will not continue search
    
    //-----------------------------------------------------------------------------
    // NAME: class Element2d
    // DESC: base class in game world
    //------------------------------------------------------------------------------
    
    class Element2d : public Sprite2d, public LGB::DrawNode, public IPFEventDelegate
    {
        Element2d*              m_pParent;
        std::vector<Element2d*> m_pChildren;
        const CPIcon*           m_ImageCatch;
        int                     m_FrameCatch;
        
        short                   m_ImageCounter;    //動畫重複播放次數的計數器
        EventMessage                m_CurEventMsg;
        mstimen_t               m_DockTime;
        Resource2d*             m_pResource2d;
        pftml::Timer            m_ResRate;
        bool                    m_bDragChildren;   //移動的時候 連同子物件
        int                     m_AnimationPos;
        
        _tstring                m_Filename;
        _tstring                m_DataBaseName;
        uint_t                  m_DataBaseEntry;
        
    public:
        Anim2d                  m_Anim2d;          //動作的影像編號
        
    public:
    
        Element2d();
        virtual ~Element2d();
        
        // 取出物件名稱
        const tchar_t*        ObjectName() const { return _T( "Element2d" );}
        
        // 0 only release myself  1 with my child
        virtual void          Clear( int s = 0 ); //release res
        
        // Set the position in element it will affect it's child element
        virtual void          SetPos( GameVertex pos );
        
        // set the animation play rate
        void                  SetFlipRate( dword_t rate );
        mstimen_t             GetFlipRate()  const ;
        
        void                  ResetSpeed( void );
        entityid_t            CompareOrder( Element2d& element );
        
        
        // Set \ Get element2d data with HElem2d struct
        void                  SetHeader( const HElem2d* pHeader );
        void                  GetHeader( HElem2d* pHeader ) const;
        void                  SetDragChildren( bool b );
        bool                  GetDragChildren();
        
        void                  SetAllSelected( bool f );                              // set all children include me
        void                  GetAllSelected( std::vector<Element2d*>& SelectedList ); // set all children include me
        
        // Load load a .elem file to this element
        virtual error_t       LoadRes( const tchar_t* resname );      //open elem
        
        
        /////////////////////////////////////////////////////////////////////////
        //以下為改變狀態
        
        // Do load a .anm animation file to change the current animation of this element
        virtual error_t        LoadAnm( const tchar_t* anmname );                     //切換動作
        
        // Save save this element data to a .elem file
        virtual error_t        SaveRes( const tchar_t* elmpath = NULL );
        
        // FaceTo change a element animation face to
        virtual bool           FaceTo( const GameVertex& destpt );
        virtual bool           FaceTo( direction_t direct );
        
        // * must update on doit
        virtual int            Update( int count = -1 ); //effect children
        virtual int            Proc( LG::Point point, uint_t uMsg, uint_t wParam ); //effect children
        
        //Get now event message
        EventMessage               GetCurEvenMsg( void );
        
        
        /////////////////////////////////////////////////////////////////////////
        //以下為取得狀態
        virtual direction_t    GetNowAngle( void );
        
        
        /// GetImage Get a image by image id  @param the id of image that you want
        //virtual const CPIcon*  GetImage(uint_t id);                          //singled image
        
        
        /// Get current playing image
        virtual const CPIcon*  GetImage( int p = -1 );                         //singled image
        
        //
        uint_t                 GetImageCount() const;
        
        // Get current playing image and change to next image
        virtual const CPIcon*  Animation( int count = -1 );
        
        void                   SetAnimationPos( int p );
        int                    GetAnimationPos();
        
        //const _tstring       GetResfname(void) const;
        //const _tstring       GetAnmfname(void) const;
        const tchar_t*         GetResfname( void ) const;
        const tchar_t*         GetAnmfname( void ) const;
        
        //child control
        Element2d*             GetRootElement( void );
        Element2d*             GetParent( void );
        bool                   AttachChild( Element2d* child );
        bool                   DetachChild( Element2d* child );
        bool                   ReplaceChild( const Element2d* oldc, Element2d* newc );
        virtual Element2d*     GetChild( entityid_t index );
        virtual size32_t       GetChildSize( void ) const  { return ( size32_t )m_pChildren.size(); }
        virtual Element2d*     operator[]( uint_t i )      { return ( i < m_pChildren.size() ) ? m_pChildren[i] : NULL; }
        //! 對所有的子元件, 包含其子元件進行某工作. c2p: true=PreOrder(自己先做),false=PostOrder(子元件先做)
        //                                          b2t: true=從陣列後面的子元件往前問, false=前到後
        // 包含會呼叫到自己
        Element2d*             ForAllChildren( OnElementChildren func, void* pDate, bool c2p = true, bool b2t = true, bool self = true );
        
        //編輯資訊
        LG::Rect               Rect( void );              //邏輯範圍
        LG::Rect               ImageRect( void );         //邏輯範圍
        LG::Rect               ScreenRect( void );        //實體上的範圍
        LG::Rect               ScreenImageRect( void );   //實體上的範圍
        
        //圖形實體資訊
        uint_t                 GetWidth( void );  //寬
        uint_t                 GetHeight( void ); //高
        uint_t                 GetClipImageWidth( void );  //寬
        uint_t                 GetClipImageHeight( void ); //高
        
        //判斷
        virtual bool           IsStaticId( entityid_t wid );
        virtual bool           IsCollision( GameVertex pos );
        virtual bool           IsCollisionByBound( GameVertex pos );
        virtual bool           IsCollisionByImage( GameVertex pos );
        virtual bool           IsCollisionByImage( Element2d* srcobj );            //碰撞
        
        std::vector<Element2d*>* GetChildrenList() { return &m_pChildren; }        //子物件排序用
        
    protected:
    
        // 傳入一個方向 得到 是哪個點(單位1)
        GameVertex             JudgPosition( direction_t );
        // 傳入一個點 得到 該點是我的哪個方向
        direction_t            JudgDirection( GameVertex destpt );
        
    protected:
    
        virtual bool           OnCollision( GameVertex pos ) { return IsCollisionByImage( pos ); }
        virtual void           OnDraw( void ) {};
        virtual void           OnUpdate( void ) {};
        
        
        //call back function from drawnode
        //virtual LG::Rect       OnGetRenderRect(void)  { return ScreenImageRect(); }
        virtual const CPIcon*  OnGetRenderImage( void ) { return Animation(); }   //Animation + Imageplaying image
        virtual LG::Point3     OnGetRenderPos( void )   { return GetScreenPos(); }
        
    public:
    
        // Interfaces of DrawNode
        virtual void           OnRenderTo( rendercontainer_t& container );
        virtual LG::Point3     OnGetComparisonPos();
        //virtual void           OnBlitText(LGB::VMCText *font) {}
        virtual bool           OnIsOverlap( const LG::Point& pos );
        
        
        
        ////////////////////////////////////////////////////////////////////////////////////
        //3tj DataBase function group
        void                   SetDataBaseName( const tchar_t* fname ) { m_DataBaseName = fname; }
        const tchar_t*         GetDataBaseName( void )                { return ( m_DataBaseName.empty() ) ? NULL : m_DataBaseName.c_str(); }
        void                   SetDataBaseEntry( uint_t entry )       { m_DataBaseEntry = entry; }
        uint_t                 GetDataBaseEntry( void )               { return m_DataBaseEntry; }
        void                   GetRecord( void* pData, size32_t size );
    };
    
    typedef Element2d Elem2d;
    
}//namespace GE



#endif
