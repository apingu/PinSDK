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

//2
#include <graphic\vmc-dx.h>
#include <graphic\vmc-gdi.h>
#include <graphic\vmc-vl.h>
//#include <vmc-draw.h>
#include <graphic\vmc-text.h>
#include <graphic\icon.h>

//4
#include "vmc-render2d.h"


#ifdef _FSWINDOW_
    #include <fswindow.h>
#endif


//#pragma comment (lib, "VMCEngine.lib")

#include <aintersect.inl>
#include <assert.h>

#define _NOLICENSE_

#ifndef _NOLICENSE_
    CPIcon*   g_pLicense = NULL;
#endif

//////////////////////////////////////////////////////////////////////////
// Default Bliter
class AlphaDrawfunctor: public LGB::DrawMethodFunctor
{
public:
    void operator()( LGB::DrawNode* node, LGB::IVMCDevice2d* pDrawDevice )
    {
        LG::Point3 pos = node->OnGetRenderPos();
        pos.y -= pos.z;
        
        if ( !node->m_Transparent )
        {
            //LGB::VMCDraw::AlphaBlit(node->OnGetRenderImage(), pos, canvas);
            if ( node->RenderAlpha >= _RENDER_FULLALPHA )
            { pDrawDevice->AlphaBlit( node->OnGetRenderImage(), pos ); }
            else if ( node->RenderAlpha == 128 )
            { pDrawDevice->Alpha16DarkenBlit( node->OnGetRenderImage(), pos ); }
            else
            { pDrawDevice->Alpha16DarkenBlit( node->OnGetRenderImage(), pos, node->RenderAlpha ); }
        }
        else // TODO: Alex, this should be transparent right now, we need more flags!
        {
            // Set true before calling AlphaBlit to let Animation function correctly
            node->m_Transparent = false;
            pDrawDevice->Alpha16DarkenBlit( node->OnGetRenderImage(), pos );
        }
    }
};


class DarkenDrawfunctor: public LGB::DrawMethodFunctor
{
public:
    void operator()( LGB::DrawNode* node, LGB::IVMCDevice2d* pDrawDevice )
    {
        LG::Point3 pos = node->OnGetRenderPos();
        pos.y -= pos.z;
        
        if ( !node->m_Transparent )
        {
            if ( node->RenderAlpha >= _RENDER_FULLALPHA )
            { pDrawDevice->AlphaBlit( node->OnGetRenderImage(), pos ); }
            else if ( node->RenderAlpha == 128 )
            { pDrawDevice->Alpha16DarkenBlit( node->OnGetRenderImage(), pos ); }
            else
            { pDrawDevice->Alpha16DarkenBlit( node->OnGetRenderImage(), pos, node->RenderAlpha ); }
        }
        else // TODO: Alex, this should be transparent right now, we need more flags!
        {
            // Set true before calling AlphaBlit to let Animation function correctly
            node->m_Transparent = false;
            pDrawDevice->Alpha16DarkenBlit( node->OnGetRenderImage(), pos );
        }
    }
};

class AlphaCliperDrawfunctor: public LGB::DrawMethodFunctor
{
public:
    void operator()( LGB::DrawNode* node, LGB::IVMCDevice2d* pDrawDevice )
    {
        LG::Point3 pos = node->OnGetRenderPos();
        pos.y -= pos.z;
        pDrawDevice->AlphaClipperBlit( node->OnGetRenderImage(),
                                       LG::Rect( node->RenderLeft, node->RenderTop,  node->RenderRight, node->RenderBottom ),
                                       pos );
    }
};

// ???????????????
class AlphaScaleRenderfunctor: public LGB::DrawMethodFunctor
{
public:
    void operator()( LGB::DrawNode* node, LGB::IVMCDevice2d* pDrawDevice )
    {
        LG::Point3 pos = node->OnGetRenderPos();
        pos.y -= pos.z;
        pDrawDevice->AlphaScaleBlit( node->OnGetRenderImage(), LG::Size( node->RenderSizeCx, node->RenderSizeCy ), pos );
    }
};


//////////////////////////////////////////////////////////////////////////
// Default test bliter  ????????? ???????????????
class NormalFontMethodfunctor: public LGB::FontMethodFunctor
{
public:
    void operator()( cText_Node* node, LGB::VMCText* pTextDevice )
    {
        if ( node->emtpy() )
        { return; }
        
        LGB::VMCRender2d::DrawText( pTextDevice, node, &node->m_TextPosition );
    }
};

class ShadowFontMethodfunctor: public LGB::FontMethodFunctor
{
public:
    void operator()( cText_Node* node, LGB::VMCText* pTextDevice )
    {
        if ( node->emtpy() )
        { return; }
        
        cText_Node::tNode* n = node->GetTextNode( 0 );
        int fcolor = n->m_ForeColor;
        n->m_ForeColor = node->m_ShadowColor;
        LG::Point npos = node->m_TextPosition;// + GameVertex(-20, 20);
        //DrawText(font, &GameVertex(npos + GameVertex(1, 0)));
        LGB::VMCRender2d::DrawText( pTextDevice, node, &LG::Point( npos + LG::Point( 0, 1 ) ) );
        LGB::VMCRender2d::DrawText( pTextDevice, node, &LG::Point( npos + LG::Point( 1, 1 ) ) );
        n->m_ForeColor = fcolor;
        LGB::VMCRender2d::DrawText( pTextDevice, node, &npos );
        //DrawText(font, &GetPos());
    }
};

class RangeFontMethodfunctor: public LGB::FontMethodFunctor
{
public:
    void operator()( cText_Node* node, LGB::VMCText* pTextDevice )
    {
        LGB::VMCRender2d::DrawText( pTextDevice, node, node->m_DrawTextLine, node->m_DrawTextRect );
    }
};



//////////////////////////////////////////////////////////////////////////
//
namespace LGB  //game entity render
{
    /*
    bool VMCRender2d::NormSort::operator() (DrawNode* elem1, DrawNode* elem2) const
    {
        if(elem1->OnGetComparisonPos().y > elem2->OnGetComparisonPos().y)
        {
            return true;
        }
        else if(elem1->OnGetComparisonPos().y == elem2->OnGetComparisonPos().y)
        {
            if (elem1->OnGetComparisonPos().x > elem2->OnGetComparisonPos().x)
            {
                return true;
            }
        }
        return false;
    }
    */
    //int SoryByPosition(const void* elem1, const void* elem2)
    bool SoryByPosition ( DrawNode* elem1, DrawNode* elem2 )
    {
        //DrawNode* icon0 = ((DrawNode*)elem1);
        //DrawNode* icon1 = ((DrawNode*)elem2);
        if ( ( ( DrawNode* )elem1 )->OnGetComparisonPos().y < ( ( DrawNode* )elem2 )->OnGetComparisonPos().y )
        {
            return true;
        }
        else if ( ( ( DrawNode* )elem1 )->OnGetComparisonPos().y == ( ( DrawNode* )elem2 )->OnGetComparisonPos().y )
        {
            if ( ( ( DrawNode* )elem1 )->OnGetComparisonPos().x < ( ( DrawNode* )elem2 )->OnGetComparisonPos().x )
            {
                return true;
            }
        }
        
        return false;
    }
    
    bool SoryByDepth ( DrawNode* elem1, DrawNode* elem2 )
    {
        //DrawNode* icon0 = ((DrawNode*)elem1);
        //DrawNode* icon1 = ((DrawNode*)elem2);
        if ( ( ( DrawNode* )elem1 )->OnGetComparisonPos().z < ( ( DrawNode* )elem2 )->OnGetComparisonPos().z )
        {
            return true;
        }
        
        return false;
    }
    
    
    
    /*
        #define IS_CLOSE_THEN(p1,p2,p3)  ((p2.y-p1.y)*(p3.x-p1.x)-(p3.y-p1.y)*(p2.x-p1.x))
        int ZOrderSort(const void* elem1, const void* elem2)
        {
            CPIcon* icon0 = ((GSO::Element2d*)elem1)->GetCurImage();
            CPIcon* icon1 = ((GSO::Element2d*)elem2)->GetCurImage();
    
    
            //speedup
            //if (nZOrder &&obj->GetZOrder())
            //  return nZOrder - obj->GetZOrder();
            // Is my baseline closer to the user than obj's?
            if(icon0->ZLine[0].x <= icon1->ZLine[0].x)
            {
                return IS_CLOSE_THEN(icon0->ZLine[0],
                                      icon0->ZLine[1],
                                      icon1->ZLine[0]);
            }else
            {
                return -IS_CLOSE_THEN(icon1->ZLine[0],
                                       icon1->ZLine[1],
                                       icon0->ZLine[0]);
            }
    
        }
    */
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    VMCRender2d::VMCRender2d()
        : m_pFocusNode( 0 )
    {
        //m_pRender = NULL;
        m_DrawMethodFuncList.resize( DrawNode::RSMax );
        memset( &m_DrawMethodFuncList[0], 0, sizeof( DrawMethodFunctor* )*DrawNode::RSMax );
        m_DrawMethodFuncList[DrawNode::RSNormal]        = new AlphaDrawfunctor;
        m_DrawMethodFuncList[DrawNode::RSDarken]        = new DarkenDrawfunctor;
        m_DrawMethodFuncList[DrawNode::RSClipAlpha]     = new AlphaCliperDrawfunctor;
        m_DrawMethodFuncList[DrawNode::RSScaleAlpha]    = new AlphaScaleRenderfunctor;
        m_TextMethodFuncList.resize( cText_Node::TMMax );
        memset( &m_TextMethodFuncList[0], 0, sizeof( FontMethodFunctor* )*cText_Node::TMMax );
        m_TextMethodFuncList[cText_Node::TMNormal]      = new NormalFontMethodfunctor;
        m_TextMethodFuncList[cText_Node::TMShadow]      = new ShadowFontMethodfunctor;
        m_TextMethodFuncList[cText_Node::TMRange]       = new RangeFontMethodfunctor;
        /*
        for(int i = 0; i<DrawNode::MAX_DrawLayerS; i++)
        {
            // ??????DLL ????????????????????????
            m_BlitContainer[i].resize(1024);
            m_BlitContainer[i].clear();
        }
        */
        m_pFocusNode = NULL;
        m_pDrawDevice = NULL;  //image
        m_pFontDevice = NULL;     //m_pFontDevice
#ifndef _NOLICENSE_
        g_pLicense = new CPIcon;
        
        if ( !g_pLicense->Load( _T( "license" ) ) )
        { SAFE_DELETE( g_pLicense ); }
        
#endif
        //LGB::DrawNode::rendercontainer_t* m_BlitContainer[DrawNode::MAX_DrawLayerS];
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    VMCRender2d::~VMCRender2d()
    {
#ifndef _NOLICENSE_
        SAFE_DELETE( g_pLicense );
#endif
        
        for ( uint_t i = 0; i < m_DrawMethodFuncList.size(); ++i )
        {
            SAFE_DELETE( m_DrawMethodFuncList[i] );
        }
        
        for ( uint_t i = 0; i < m_TextMethodFuncList.size(); ++i )
        {
            SAFE_DELETE( m_TextMethodFuncList[i] );
        }
        
        Destroy();
    }
    
    
    //=============================================================================
    // NAME:
    // DESC: ??????????????? 1:DX 2:GDI
    //=============================================================================
    bool VMCRender2d::CreateDevice( DeviceTypeId type, void* hWnd, int x, int y, uint_t width, uint_t height, uint_t depth, bool fullscreen )
    {
        if ( m_pDrawDevice != NULL )
        {
            m_pDrawDevice->Destroy();
            SAFE_DELETE( m_pDrawDevice );
        }
        
        switch ( type )
        {
        case _VMCTYPEID_2DDX:
            m_pDrawDevice = new LGB::VMCDX;
            break;
            
        case _VMCTYPEID_2DGDI:
            m_pDrawDevice = new LGB::VPage;
            break;
            
        case _VMCTYPEID_2DVL:
            m_pDrawDevice = new LGB::VMCVL;
            break;
            
        default:
            assert( 0 && "create device type not exist!" );
            break;
        }
        
        m_DeivceType = type;
        
        if ( m_pDrawDevice )
        {
            m_pDrawDevice->Create( hWnd, x, y, width, height, depth, fullscreen );
            
            if ( m_pFontDevice )
            {
                m_pFontDevice->SetCanvas( GetCanvas() );
            }
            
#ifdef _FSWINDOW_
            
            if ( type == _VMCTYPEID_2DDX )
            {
                FSWindow_Init( ( Pwnd )hWnd,
                               ( LPDIRECTDRAW7 )( ( ( LGB::VMCDX* )m_pDrawDevice )->GetDirectDraw() ),
                               ( LPDIRECTDRAWSURFACE7 )( ( ( LGB::VMCDX* )m_pDrawDevice )->GetPrimSurface() ),
                               ( LPDIRECTDRAWSURFACE7 )( ( ( LGB::VMCDX* )m_pDrawDevice )->GetBackSurface() ) );
            }
            
#endif
            return true;
        }
        
        return false;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    CPIcon* VMCRender2d::GetCanvas()
    {
        CPIcon* canvas = ( CPIcon* )m_pDrawDevice->GetCanvas();
        return canvas;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int  VMCRender2d::CreateFont( RENDER_FONTSET set, const tchar_t* filename, int w, int h, RENDER_FONTFMT fmt, int cache )
    {
        if ( m_pDrawDevice == NULL )
        {
            assert( m_pDrawDevice && "create device first" );
            return -1;
        }
        
        if ( m_pFontDevice == NULL )
        { m_pFontDevice = new LGB::VMCText; }
        
        int ret = m_pFontDevice->LoadFont( set, filename, w, h, m_pDrawDevice->GetDepthb(), fmt, cache );
        
        if ( ret )
        { m_pFontDevice->SetCanvas( ( CPIcon* )GetCanvas() ); }
        
        return ret;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::Destroy()
    {
        SAFE_DELETE( m_pDrawDevice );
        SAFE_DELETE( m_pFontDevice );
    }
    
    
    //==========================================================================================
    // NAME:
    // DESC:
    //==========================================================================================
    bool VMCRender2d::BeginFrame( dword_t argv )
    {
        if ( !m_pDrawDevice )
        { return false; }
        
        // ???????????????????????????
        ProcessTransparency();
        
        // ???????????? ?????????????????????
        for ( uint_t i = 0; i < m_BlitContainer[DrawNode::kLayerBld].size(); i++ )
        { m_BlitContainer[DrawNode::kLayerRole].push_back( m_BlitContainer[DrawNode::kLayerBld][i] ); }
        
        if ( !m_BlitContainer[DrawNode::kLayerRole].empty() )
        {
            std::stable_sort( m_BlitContainer[DrawNode::kLayerRole].begin(),
                              m_BlitContainer[DrawNode::kLayerRole].end(),
                              SoryByPosition );
        }
        
        return m_pDrawDevice->BeginFrame( argv );
    }
    
    //==========================================================================================
    // NAME:
    // DESC:
    //==========================================================================================
    void VMCRender2d::EndFrame( void )
    {
        if ( m_pFocusNode )
        { ( ( LGB::IVMCDevice2d* )m_pDrawDevice )->AlphaBlit( m_pFocusNode->OnGetRenderImage(), m_pFocusNode->OnGetRenderPos() ); }
        
#ifndef _NOLICENSE_
        assert( g_pLicense && "no license file" );
        ( ( LGB::IVMCDevice2d* )m_pDrawDevice )->AlphaBlit( g_pLicense, LG::Point( 0, m_pDrawDevice->GetHeight() - g_pLicense->GetHeight() ) );
#endif
        m_pDrawDevice->EndFrame();
        
        // ????????????
        for ( int i = 0; i < LGB::DrawNode::MAX_DrawLayerS; ++i )
        {
            m_BlitContainer[i].clear();
        }
    }
    
    
    
    
    //==========================================================================================
    // NAME:
    // DESC:
    //==========================================================================================
    void VMCRender2d::BlitAll()
    {
        assert( m_pDrawDevice && "VMCRender2d::BlitAll should init before use!!" );
        BlitTerrain();
        BlitSurface();
        BlitEntity();
        BlitFX();
        BlitOnAir();
        BlitUI();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::Flip()
    {
        //LGB::VMCDraw::AlphaBlit(g_pLicense, m_pFocusNode->OnGetRenderPos(), GetCanvas());
        if ( m_pDrawDevice )
        {
#ifdef _FSWINDOW_
        
            if ( FSWindow_IsActive() )
            {
                FSWindow_Update();
            }
            else
            {
                m_pDrawDevice->Flip();
            }
            
#else
            m_pDrawDevice->Flip();
#endif
        }
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool VMCRender2d::GetFullScreen()
    {
        if ( m_pDrawDevice )
        { return m_pDrawDevice->GetFullScreen(); }
        
        return false;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::SetFontColor( int fg, int bg, int opacity )
    {
        if ( m_pFontDevice )
        { m_pFontDevice->SetColor( fg, bg, opacity ); }
    }
    void VMCRender2d::SetFontDropShadow( int color, int dist )
    {
        if ( m_pFontDevice )
        { m_pFontDevice->SetDropShadow( color, dist ); }
    }
    void VMCRender2d::SetFontSpace( int h, int v )
    {
        if ( m_pFontDevice )
        { m_pFontDevice->SetSpace( h, v ); }
    }
    
    LG::Size VMCRender2d::GetFontSize( RENDER_FONTSET set )
    {
        LG::Size ss;
        
        if ( m_pFontDevice )
        {
            ss.cx = m_pFontDevice->GetFontWidth( set );
            ss.cy = m_pFontDevice->GetFontWidth( set );
        }
        
        return ss;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::DoRender( DrawNode* pDrawNode )
    {
        //pDrawNode->RenderTo(*this);
        pDrawNode->OnRenderTo( ( m_BlitContainer[pDrawNode->RenderLayer] ) );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::SetFocus( DrawNode* focusNode )
    {
        m_pFocusNode = focusNode;
    }
    
    
    /*
    LGB::DrawNode::rendercontainer_t &VMCRender2d::GetContainer(LGB::DrawNode::DrawLayer layer)
    {
        return m_BlitContainer[layer];
    }
    */
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int VMCRender2d::AlphaBlit( const CPIcon* Src, LG::Point point )
    {
        if ( m_pDrawDevice )
        { return ( ( LGB::IVMCDevice2d* )m_pDrawDevice )->AlphaBlit( Src, point ); }
        
        return -1;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int VMCRender2d::Iconcpy( const CPIcon* Src, LG::Point point, Icon* Dest )
    {
        if ( m_pDrawDevice )
        { return ( ( LGB::IVMCDevice2d* )m_pDrawDevice )->Iconcpy( Src, point, Dest ); }
        
        return -1;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::PutsXY( int x, int y, const tchar_t* str )
    {
        if ( m_pFontDevice )
        { m_pFontDevice->PutsXY( x, y, str ); }
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    long VMCRender2d::ChangeCoopLevel( bool bfullscreen )
    {
        if ( m_pDrawDevice->DeviceType() == 1 )
        {
            int ret = ( ( LGB::VMCDX* )m_pDrawDevice )->ChangeCoopLevel( bfullscreen );
#ifdef _FSWINDOW_
            FSWindow_Init( ( Pwnd )m_pDrawDevice->GetWidget(),
                           ( LPDIRECTDRAW7 )( ( ( LGB::VMCDX* )m_pDrawDevice )->GetDirectDraw() ),
                           ( LPDIRECTDRAWSURFACE7 )( ( ( LGB::VMCDX* )m_pDrawDevice )->GetPrimSurface() ),
                           ( LPDIRECTDRAWSURFACE7 )( ( ( LGB::VMCDX* )m_pDrawDevice )->GetBackSurface() ) );
#endif
            return ret;
        }
        
        return 0;
    }
    
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::InsertTerrain( DrawNode* node )
    {
        m_BlitContainer[LGB::DrawNode::kLayerBG].push_back( node );
    }
    
    void VMCRender2d::InsertSurface( DrawNode* node )
    {
        m_BlitContainer[LGB::DrawNode::kLayerSurface].push_back( node );
    }
    
    void VMCRender2d::InsertEntity( DrawNode* node )
    {
        m_BlitContainer[LGB::DrawNode::kLayerRole].push_back( node );
    }
    
    void VMCRender2d::InsertFx( DrawNode* node )
    {
        m_BlitContainer[LGB::DrawNode::kLayerFX].push_back( node );
    }
    
    void VMCRender2d::InsertUI( DrawNode* node )
    {
        m_BlitContainer[LGB::DrawNode::kLayerUI].push_back( node );
    }
    
    
    void VMCRender2d::InsertCursor( DrawNode* node )
    {
        m_pFocusNode = node;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::InsertNode( DrawNode* node, DrawNode::DrawLayer layer )
    {
        if ( layer < DrawNode::MAX_DrawLayerS )
        {
            assert ( node != NULL && "VMCRender2d::InsertNode: A NULL node Pushed!!!" );
            /*
            if((layer == DrawNode::kLayerBld) || (layer == DrawNode::kLayerRole))
            {    // BG or UI would not be sorted
                // TODO: Alex, UI has been sorted before insert to renderer, it's not symmetry
                m_SortedEntities.push(node);
            }
            */
            m_BlitContainer[layer].push_back( node );
        }
    }
    
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::RemoveNode( DrawNode* node, DrawNode::DrawLayer layer )
    {
        if ( layer < DrawNode::MAX_DrawLayerS )
        {
            for ( uint_t j = 0; j < m_BlitContainer[layer].size(); j++ )
            {
                if ( m_BlitContainer[layer][j] == node )
                {
                    m_BlitContainer[layer].erase( m_BlitContainer[layer].begin() + j );
                    return;
                }
            }
        }
        
        return;
    }
    
    bool VMCRender2d::AddnewRenderFunc( DrawNode::DrawMethod id, LGB::DrawMethodFunctor* functor )
    {
        if ( id < ( int )m_DrawMethodFuncList.size() )
        {
            if ( m_DrawMethodFuncList[id] == NULL )
            {
                m_DrawMethodFuncList[id] = functor;
                return true;
            }
        }
        
        return false;
    }
    
    void VMCRender2d::RemoveRenderFunc( DrawNode::DrawMethod id )
    {
        if ( id < ( int )m_DrawMethodFuncList.size() )
        {
            SAFE_DELETE( m_DrawMethodFuncList[id] );
        }
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::RemoveNode( DrawNode* node )
    {
        for ( uint_t i = 0; i < DrawNode::MAX_DrawLayerS; i++ )
        {
            for ( uint_t j = 0; j < m_BlitContainer[i].size(); j++ )
            {
                if ( m_BlitContainer[i][j] == node )
                {
                    m_BlitContainer[i].erase( m_BlitContainer[i].begin() + j );
                    return;
                }
            }
        }
        
        return;
    }
    
    
    //==========================================================================================
    // NAME:
    // DESC:
    //==========================================================================================
    void VMCRender2d::BlitImage( DrawNode* node )
    {
        if ( m_DrawMethodFuncList[node->RenderMethod] != NULL )
        { ( *m_DrawMethodFuncList[node->RenderMethod] )( node, m_pDrawDevice ); }
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::BlitText( cText_Node* node )
    {
        if ( node )
        {
            if ( m_TextMethodFuncList[node->m_TextMethod] != NULL )
            { ( *m_TextMethodFuncList[node->m_TextMethod] )( node, m_pFontDevice ); }
        }
    }
    
    
#include <algorithm>
    
    //==========================================================================================
    // NAME:
    // DESC:
    //==========================================================================================
    void VMCRender2d::BlitTerrain()
    {
        for ( uint_t j = 0; j < m_BlitContainer[DrawNode::kLayerBG].size(); j++ )
        {
            BlitImage( m_BlitContainer[DrawNode::kLayerBG][j] );
        }
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::BlitSurface()
    {
        std::stable_sort( m_BlitContainer[DrawNode::kLayerSurface].begin(),
                          m_BlitContainer[DrawNode::kLayerSurface].end(),
                          SoryByPosition );
                          
        for ( uint_t n = 0; n < m_BlitContainer[DrawNode::kLayerSurface].size(); n++ )
        {
            DrawNode* pElem = m_BlitContainer[DrawNode::kLayerSurface][n];
            BlitImage( pElem );
            //pElem->BlitImage(m_pRender, canvas);
        }
    }
    
    void VMCRender2d::BlitEntity()
    {
        for ( uint_t n = 0; n < m_BlitContainer[DrawNode::kLayerRole].size(); n++ )
        {
            DrawNode* pElem = m_BlitContainer[DrawNode::kLayerRole][n];
            BlitImage( pElem );
            BlitText( ( cText_Node* )pElem->OnGetTextNode() );
        }
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::BlitFX()
    {
        for ( uint_t i = 0; i < m_BlitContainer[DrawNode::kLayerFX].size(); ++i )
        {
//            m_pRender->AlphaBlit(m_BlitContainer[DrawNode::kLayerFX][j]->Animation(), m_BlitContainer[DrawNode::kLayerFX][j]->GetScreenPos(), canvas);
            BlitImage( m_BlitContainer[DrawNode::kLayerFX][i] );
            //m_BlitContainer[DrawNode::kLayerFX][i]->BlitImage(m_pRender, canvas);
            ///////m_BlitContainer[DrawNode::kLayerFX][i]->OnBlitText(m_pFontDevice);
        }
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::BlitOnAir()
    {
        for ( uint_t i = 0; i < m_BlitContainer[DrawNode::kLayerOnAir].size(); ++i )
        {
            BlitImage( m_BlitContainer[DrawNode::kLayerOnAir][i] );
        }
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::BlitUI()
    {
        for ( uint_t j = 0; j < m_BlitContainer[DrawNode::kLayerUI].size(); ++j )
        {
            DrawNode* pElem = m_BlitContainer[DrawNode::kLayerUI][j];
            BlitImage( pElem );
            //m_BlitContainer[DrawNode::kLayerUI][j]->BlitImage(m_pRender, canvas);
            BlitText( ( cText_Node* )pElem->OnGetTextNode() );
        }
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::BlitTop()
    {
        for ( uint_t j = 0; j < m_BlitContainer[DrawNode::kLayerTop].size(); ++j )
        {
            DrawNode* pElem = m_BlitContainer[DrawNode::kLayerTop][j];
            BlitImage( pElem );
            //m_BlitContainer[DrawNode::kLayerUI][j]->BlitImage(m_pRender, canvas);
            BlitText( ( cText_Node* )pElem->OnGetTextNode() );
        }
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
#define _MAP_RECT_LIMIT_ 100
    
//     void VMCRender2d::Collide_Scroll(POINT point)
//     {
//
//         if (m_pRender->GetWidget() == NULL)
//             return;
//
//
//         /*
//
//         GameVertex mouse_pt;
//         GetCursorPos(&mouse_pt);
//         ScreenToClient((HWND) m_pRender->GetWidget(), &mouse_pt);
//
//         LG::Rect client_rect;
//         GetClientRect((HWND) m_pRender->GetWidget(), &client_rect);
//
//         if (PA::Intersect::Test(&client_rect, &mouse_pt))
//         {
//             if (point.x < _MAP_RECT_LIMIT_)
//             {
//                 if (GSO::Sprite2d::GetOrigin().x < 0)
//                 {
//                     GameVertex spt(GSO::Sprite2d::GetOrigin().x + 10,
//                                     GSO::Sprite2d::GetOrigin().y);
//                     GSO::Sprite2d::SetOrigin(spt);
//
//                     point.x = _MAP_RECT_LIMIT_;
//                 }
//             }
//             else if (point.x > m_pRender->GetWidth() - _MAP_RECT_LIMIT_)
//             {
//                 if (GSO::Sprite2d::GetOrigin().x >
//                      (m_pRender->GetWidth() - currZone->ImageWidth()))
//                 {
//                     GameVertex spt(GSO::Sprite2d::GetOrigin().x - 10,
//                                     GSO::Sprite2d::GetOrigin().y);
//                     GSO::Sprite2d::SetOrigin(spt);
//
//                     point.x = m_pRender->GetWidth() - _MAP_RECT_LIMIT_;
//                 }
//             }
//             if (point.y < _MAP_RECT_LIMIT_)
//             {
//                 if (GSO::Sprite2d::GetOrigin().y < 0)
//                 {
//                     GameVertex spt(GSO::Sprite2d::GetOrigin().x,
//                                     GSO::Sprite2d::GetOrigin().y + 10);
//                     GSO::Sprite2d::SetOrigin(spt);
//
//                     point.y = _MAP_RECT_LIMIT_;
//                 }
//             }
//             else if (point.y > (m_pRender->GetHeight() - _MAP_RECT_LIMIT_))
//             {
//                 if (currZone &&
//                      GSO::Sprite2d::GetOrigin().y >
//                      (m_pRender->GetHeight() - currZone->ImageHeight()))
//                 {
//                     GameVertex spt(GSO::Sprite2d::GetOrigin().x,
//                                     GSO::Sprite2d::GetOrigin().y - 10);
//                     GSO::Sprite2d::SetOrigin(spt);
//
//                     point.y = m_pRender->GetHeight() - _MAP_RECT_LIMIT_;
//                 }
//             }
//
//             ClientToScreen((HWND) m_pRender->GetWidget(), &point);
//             SetCursorPos(point.x, point.y);
//         }
//         */
//
//         return;
//     }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCRender2d::ProcessTransparency()
    {
        if ( m_pFocusNode != NULL )
        {
            for ( uint_t i = 0; i < m_BlitContainer[DrawNode::kLayerBld].size(); i++ )
            {
                if ( m_pFocusNode != m_BlitContainer[DrawNode::kLayerBld][i] &&
                        m_pFocusNode->OnGetComparisonPos().y < m_BlitContainer[DrawNode::kLayerBld][i]->OnGetComparisonPos().y &&
                        m_BlitContainer[DrawNode::kLayerBld][i]->OnIsOverlap( m_pFocusNode->OnGetComparisonPos() ) )
                {
                    m_BlitContainer[DrawNode::kLayerBld][i]->m_Transparent = true;
                }
            }
        }
    }
    
    
    
    
    
    
    //////////////////////////////////////////////////////////////////////////
    //???????????????????????????,????????????'\n'?????? ?????????????????????
    //////////////////////////////////////////////////////////////////////////
    int VMCRender2d::PushText( LGB::VMCText* pTextDevice, cText_Node::tNode* node, LG::Point* pos, bool bmask )
    {
        node->m_FontSize.cx = pTextDevice->GetFontWidth( FONT_SET_SBCS );
        node->m_FontSize.cy = pTextDevice->GetFontHeight( FONT_SET_SBCS );
#ifndef UNICODE
        BYTE*    s = ( BYTE* )node->m_Text.c_str();
#else
        wchar_t*    s = ( wchar_t* )node->m_Text.c_str();
#endif
        LG::Rect    rect( node->m_TextRect );
        int            bsd = 0;
        int            line = 0;
        int            count = 0;
        // pos ??????offset???????????????
        rect.top    += pos->y;
        rect.bottom    += pos->y;
        rect.left    += pos->x;
        rect.right    += pos->x;
        int x = rect.left;
        int y = rect.top;
        
        if ( ( ( uint_t )node->m_Text.size() ) > ( uint_t )node->m_StartPos )
        {
            s += node->m_StartPos;
        }
        else
        {
            // ?????????cursor
            if ( node->m_Cursor )
            {
#ifndef UNICODE
                pTextDevice->PutcXY( x, y, node->m_CursorSign );
#else
                pTextDevice->PutcXYW( x, y, node->m_CursorSign );
#endif
            }
            
            return line;
        }
        
        int fcx = node->m_FontSize.cx = pTextDevice->GetFontWidth( FONT_SET_SBCS ); //node->m_FontSize.cx
        int fcy = node->m_FontSize.cy = pTextDevice->GetFontHeight( FONT_SET_SBCS ); //node->m_FontSize.cy
        int lw = __max( pTextDevice->GetFontWidth( FONT_SET_SBCS ),  pTextDevice->GetFontWidth( FONT_SET_DBCS ) );
        int lh = __max( pTextDevice->GetFontHeight( FONT_SET_SBCS ), pTextDevice->GetFontHeight( FONT_SET_DBCS ) );
        ++line;
        
        // ?????????????????????
        while ( *s != 0 )
        {
            //if (((x + fcx) > rect.right) || ((*s)=='\n'))    // ??????????????????
            //if (((x + fcx) > rect.right))    // ??????????????????
            if ( ( ( x + fcx ) > rect.right ) )    // ??????????????????
            {
                if ( node->m_AutoNextLine )    // ?????????????????????
                {
                    x = rect.left;
                    y = y + lh + node->m_TextSpace.cy;
                    ++line;
                    count = 0;
                    
                    if ( y > rect.bottom )
                    { break; }
                }
                else    //
                {
                    while ( *s != 0 && *s != '\n' )
                    { ++s; }
                    
                    if ( *s == '\0' )
                    { break; }
                }
            }
            
            if ( y > rect.bottom )
            { break; }
            
            // Check if it's a SBCS tchar_t.
#ifndef UNICODE
            
            if ( ( ( *s ) > 0x19 ) && ( ( *s ) < 0x7F ) )
            {
                if ( node->m_Cursor && count == node->m_InsertPos )
                {
                    x += pTextDevice->PutcXY( x, y, node->m_CursorSign ) + node->m_TextSpace.cx;
                }
                else if ( node->m_DefaultText != 0 )
                {
                    x += pTextDevice->PutcXY( x, y, node->m_DefaultText ) + node->m_TextSpace.cx;
                }
                else
                {
                    if ( bmask )
                    { x += pTextDevice->PutcXY( x, y, '*' ) + node->m_TextSpace.cx; }
                    else
                    { x += pTextDevice->PutcXY( x, y, *s ) + node->m_TextSpace.cx; }
                }
                
                count++;
                s++;
                bsd = 1;
            }
            else // ??????????????????????????? DBCS
            {
                if ( *s == '\n' || *s == '\r' || *s == '\t' )
                {
                    while ( *s == '\n' || *s == '\r' || *s == '\t' )
                    { s++; }
                    
                    x = rect.left;
                    y = y + lh + node->m_TextSpace.cy;
                    line++;
                    count++;
                    continue;
                }
                else
                {
                    if ( node->m_Cursor && count == node->m_InsertPos )
                    {
                        x += pTextDevice->PutcXY( x, y, node->m_CursorSign, node->m_CursorSign ) + node->m_TextSpace.cx;
                    }
                    else if ( node->m_DefaultText != 0 )
                    {
                        x += pTextDevice->PutcXY( x, y, node->m_DefaultText, node->m_DefaultText ) + node->m_TextSpace.cx;
                    }
                    else
                    {
                        if ( bmask )
                        { x += pTextDevice->PutcXY( x, y, '*' ) + node->m_TextSpace.cx; }
                        else
                        { x += pTextDevice->PutcXY( x, y, *s, *( s + 1 ) ) + node->m_TextSpace.cx; }
                    }
                    
                    count += 2;
                    s += 2;
                    bsd = 2;
                }
            }
            
#else    // UNICODE
            // ??? MCS ?????????: ??????????????????????????????????????????, ???????????????????????????
            
            // ????????????cursor?????????????????????cursor?????????
            if ( node->m_Cursor && count == node->m_InsertPos )
            {
                x += pTextDevice->PutcXYW( x, y, node->m_CursorSign );
            
                if ( pTextDevice->isCharWide( *s ) )    // ??????
                { x += pTextDevice->PutcXYW( x, y, node->m_CursorSign ); }
            }
            else
            {
                if ( node->m_DefaultText != 0 )
                { x += pTextDevice->PutcXYW( x, y, node->m_DefaultText ); }
                else
                {
                    switch ( *s )
                    {
                    case '\n':    // ???????????? reset X
                        x = rect.left;
                        y = y + fcy + node->m_TextSpace.cy;
                        break;
            
                    case '\t':    // ????????????4?????????
                        {
                            int xc = ( x - rect.left ) / fcx;
                            xc = 4 * ( xc + 3 ) / 4;
                            x = rect.left + xc * fcx;
                        }
                        break;
            
                    case '\r':    // ??????????????? reset X
                        y = y + fcy + node->m_TextSpace.cy;
                        break;
            
                    case ' ':
                        {
                            x += pTextDevice->PutcXYW( x, y, *s );
                            wchar_t* next_space = wcschr( s + 1, L' ' ); // ???????????????????????????
            
                            if ( next_space != NULL )
                            {
                                int next_word_width = ( next_space - s );
            
                                if ( ( x + fcx * next_word_width ) > rect.right )
                                {
                                    x = rect.left;
                                    y = y + fcy + node->m_TextSpace.cy;
                                    ++line;
                                    count = 0;
                                }
            
                                break;
                            }
            
                            int to_end_length = wcslen( s + 1 );
            
                            if ( ( x + fcx * to_end_length ) > rect.right )
                            {
                                x = rect.left;
                                y = y + fcy + node->m_TextSpace.cy;
                                ++line;
                                count = 0;
                                break;
                            }
                        }
                        break;
            
                    default:
                        x += pTextDevice->PutcXYW( x, y, *s ) + node->m_TextSpace.cx;
                    }
                }
            }
            
            ++s;
            ++count;
#endif    // UNICODE
        }
        
        if ( node->m_Cursor && ( count == node->m_InsertPos ) )
        {
#ifndef UNICODE
            x += pTextDevice->PutcXY( x, y, node->m_CursorSign ) + node->m_TextSpace.cx;
#else
            x += pTextDevice->PutcXYW( x, y, node->m_CursorSign ) + node->m_TextSpace.cx;
#endif
        }
        
        return line;
    }
    
    
    
    //////////////////////////////////////////////////////////////////////////
    //?????????????????????????????????,????????????????????????
    //////////////////////////////////////////////////////////////////////////
    void VMCRender2d::DrawText( LGB::VMCText* pTextDevice, cText_Node* node, LG::Point* pos )
    {
        if ( node->m_TextNode.empty() )
        { return; }
        
        int fg, bg, nfg, nbg;
        pTextDevice->GetColor( fg, bg );
        
        for ( uint_t i = 0; i < node->m_TextNode.size() ; ++i )
        {
            nfg = CPIcon::MakePixel( node->m_TextNode[i]->m_ForeColor );
            nbg = CPIcon::MakePixel( node->m_TextNode[i]->m_BackColor );
            pTextDevice->SetColor( nfg, nbg );
            LGB::VMCRender2d::PushText( pTextDevice, node->m_TextNode[i], pos, node->GetMask() );
        }
        
        pTextDevice->SetColor( fg, bg );
    }
    
    //////////////////////////////////////////////////////////////////////////
    //?????????????????????????????????,????????????????????????
    //////////////////////////////////////////////////////////////////////////
    void VMCRender2d::DrawText( LGB::VMCText* pTextDevice, cText_Node* node, int start_line, LG::Rect rect )
    {
        if ( ( ( uint_t )node->m_TextNode.size() ) < ( uint_t )start_line )
        { return; }
        
        //TextNodeSelectRGBTransform();
        LG::Point    pos( rect.left, rect.top );
        int fg, bg, nfg, nbg;
        pTextDevice->GetColor( fg, bg );
        
        for ( uint_t i = 0, count = 0; i < node->m_TextNode.size(); ++i, ++count )
        {
            if ( ( ( int )count ) < start_line )
            { continue; }
            
            nfg = CPIcon::MakePixel( node->m_TextNode[i]->m_ForeColor );
            nbg = CPIcon::MakePixel( node->m_TextNode[i]->m_BackColor );
            pTextDevice->SetColor( nfg, nbg );
            //int lineheight=m_TextNode[i]->m_TextSpace.cy+m_TextNode[i]->m_FontSize.cy;
            int lineheight = node->m_TextNode[i]->m_TextSpace.cy + pTextDevice->GetFontHeight( FONT_SET_SBCS );
            
            if ( ( pos.y + lineheight ) < rect.bottom )
            { pos.y += ( LGB::VMCRender2d::PushText( pTextDevice, node->m_TextNode[i], &pos, node->GetMask() ) * ( lineheight ) ); } //+m_TextNode[i]->m_TextSpace.cy;
            else
            { break; }
        }
        
        pTextDevice->SetColor( fg, bg );
    }
};//namespace LGB


