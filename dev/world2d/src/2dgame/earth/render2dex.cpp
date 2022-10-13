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
//
#include <graphic\ivmc-draw2d.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "render2dex.h"
//#include <vmc-draw.h>



class CliperRenderfunctor: public LGB::DrawMethodFunctor
{
public:

    CliperRenderfunctor( LG::Rect clip_rect )
    {
        m_ZoneRect = clip_rect;
    }

    void operator()( LGB::DrawNode* node, LGB::IVMCDevice2d* pDrawDevice )
    {
        LG::Rect r( 0, 0, node->OnGetRenderImage()->GetWidth(), node->OnGetRenderImage()->GetHeight() );
        LG::Point pt = node->OnGetRenderPos();
        if( m_ZoneRect.top > r.top )
        {
            r.top += m_ZoneRect.top - r.top;
        }
        r.bottom = ( m_ZoneRect.bottom > r.bottom ) ? r.bottom : m_ZoneRect.bottom;

        if( m_ZoneRect.left > r.left )
        {
            r.left += m_ZoneRect.left - r.left;
        }
        r.right = ( m_ZoneRect.right > r.right ) ? r.right : m_ZoneRect.right;

        r.top -= pt.y;
        r.bottom -= pt.y;
        r.left -= pt.x;
        r.right -= pt.x;

        if( ( r.bottom < r.top ) || ( r.right < r.left ) )
            return;
        ( ( LGB::IVMCDraw2d* )pDrawDevice )->ClipperBlit( node->OnGetRenderImage(), r, pt );
    }

private:
    LG::Rect        m_ZoneRect;
};

class NormalBlitRenderfunctor: public LGB::DrawMethodFunctor
{
public:
    void operator()( LGB::DrawNode* node, LGB::IVMCDevice* pDrawDevice )
    {
        if( !node->m_Transparent )
        {
            //            render->Blit(node->OnGetRenderImage(), node->OnGetRenderPos(), canvas);
            ( ( LGB::IVMCDraw2d* )pDrawDevice )->AlphaBlit( node->OnGetRenderImage(), node->OnGetRenderPos() );
        }
        else // TODO: Alex, this should be transparent right now, we need more flags!
        {
            // Set true before calling AlphaBlit to let Animation function correctly
            node->m_Transparent = false;
            ( ( LGB::IVMCDraw2d* )pDrawDevice )->Alpha16DarkenBlit( node->OnGetRenderImage(), node->OnGetRenderPos() );
        }
    }
};


namespace LGB
{
    Render2dEX::Render2dEX(): m_RenderFlow( RFNormal )//, m_ParticalSystem(NULL)
    {
        m_ParticalSystem = NULL;
        m_pTerrainSystem = NULL;

        RemoveRenderFunc( ( LGB::DrawNode::DrawMethod )RSExClip );
        AddnewRenderFunc( ( LGB::DrawNode::DrawMethod )RSExClip, new CliperRenderfunctor( LG::Rect( 0, 120, 799, 599 ) ) );
    }

    Render2dEX::~Render2dEX()
    {
        m_ParticalSystem = NULL;
        m_pTerrainSystem = NULL;
    }

    void Render2dEX::SetRenderFlow( RenderFlow rf )
    {
        m_RenderFlow = rf;
        if( m_RenderFlow == RFPerspective )
        {
            if( m_pTerrainSystem )
                m_pTerrainSystem->SetZoneDrawState( ( LGB::DrawNode::DrawMethod )RSExClip );
        }
        else
        {
            if( m_pTerrainSystem )
                m_pTerrainSystem->SetZoneDrawState( LGB::DrawNode::RSNormal );
        }
    }


    //==========================================================================================
    // NAME:
    // DESC:
    //==========================================================================================
    void Render2dEX::BlitAll()
    {
        //     assert((GetRender() &&GetFont()) &&"Render2dEX::BlitAll: This instance should init before use!!");
        // 
        //     if (GetRender() == NULL)
        //         return;

        CPIcon* canvas = GetCanvas();


        if( m_RenderFlow == RFPerspective )
        {
            BlitImage( &m_BlitSky );
            BlitImage( &m_BlitFarCloud );
            BlitImage( &m_BlitFarCloud2 );
            BlitImage( &m_BlitMountain );
            BlitImage( &m_BlitMountain2 );
            BlitImage( &m_BlitCloud );
            BlitImage( &m_BlitCloud2 );

            BlitEntityBack();

            BlitTerrain();

            BlitImage( &m_BlitMask );

            BlitEntityFront();
        }
        else
        {
            BlitTerrain();
            BlitSurface();
            BlitEntity();
            BlitFX();
        }
        BlitPartical();
        BlitOnAir();
        BlitUI();
    }


    //==========================================================================================
    // NAME:
    // DESC:
    //==========================================================================================
    void Render2dEX::BlitPartical()
    {
        if( m_ParticalSystem == NULL )
            return;
        // TODO: Sean
        // 連天氣特效也不見囉
        //    m_ParticalSystem->BlitImage(m_pRender, canvas);
    }


    //==========================================================================================
    // NAME:
    // DESC:
    //==========================================================================================
    void Render2dEX::BlitEntityBack()
    {
        for( uint_t n = 0; n < m_BlitContainer[ LGB::DrawNode::kLayerRole ].size(); n++ )
        {
            LGB::DrawNode* pElem = m_BlitContainer[ LGB::DrawNode::kLayerRole ][ n ];
            if( pElem->OnGetRenderPos().y > 120 )
                continue;
            else
            {
                float ratio = ( float )( pElem->OnGetRenderPos().y + 270 ) / ( float )570;
                if( ratio < .1 )
                    continue;
                const CPIcon* icon = pElem->OnGetRenderImage();
                if( icon == NULL )
                    continue;
                LG::Point pt( pElem->OnGetRenderPos() );
                pt.y = 132 - pt.y / 10;
                LG::Size new_size( icon->GetWidth() * ratio, icon->GetHeight() * ratio );
                GetDrawDevice()->AlphaScaleBlit( icon, new_size, pt );
            }
        }
    }

    void Render2dEX::BlitEntityFront()
    {
        for( uint_t n = 0; n < m_BlitContainer[ LGB::DrawNode::kLayerRole ].size(); n++ )
        {
            LGB::DrawNode* pElem = m_BlitContainer[ LGB::DrawNode::kLayerRole ][ n ];

            if( pElem->OnGetRenderPos().y >= 300 )
            {
                BlitImage( pElem );
                BlitText( ( cText_Node* )pElem->OnGetTextNode() );
                //pElem->OnBlitText(m_pFontDevice);
            }
            else
            {
                float ratio = ( float )( pElem->OnGetRenderPos().y + 270 ) / ( float )570;
                if( ratio < .1 )
                    continue;
                LG::Point pt( pElem->OnGetRenderPos() );
                if( pt.y < 120 )
                    continue;
                const CPIcon* icon = pElem->OnGetRenderImage();
                if( icon == NULL )
                    continue;
                LG::Size new_size( icon->GetWidth() * ratio, icon->GetHeight() * ratio );
                GetDrawDevice()->AlphaScaleBlit( icon, new_size, pt );
            }
        }
    }

    void Render2dEX::UpdateBackground()
    {
        if( !m_pTerrainSystem )
            return;

        GSO::World::IEntity2d* pPlayer = m_pTerrainSystem->GetFocusEntity();
        if( pPlayer != NULL )
        {
            if( m_pTerrainSystem->GetMapCX() == GetDrawDevice()->GetWidth() && m_pTerrainSystem->GetMapCY() == GetDrawDevice()->GetHeight() )
            {
                GSO::World::IEntity2d::SetRefOrigin( GameVertex( 0, 0, 0 ) );
            }
            else
            {
                GameVertex pos;//=GSO::World::IEntity2d::GetRefOrigin();
                GameVertex playerScreenPos = pPlayer->GetPos() + GSO::World::IEntity2d::GetRefOrigin();
                float rate = 0.25;
                int wr = GetDrawDevice()->GetWidth()  * rate;
                int hr = GetDrawDevice()->GetHeight() * rate;
                if( playerScreenPos.x < 275 )
                    pos.x = -( pPlayer->GetPos().x - 275 );
                else if( playerScreenPos.x > 625 )
                    pos.x = -( pPlayer->GetPos().x - 625 );
                if( playerScreenPos.y < 300 )
                    pos.y = -( pPlayer->GetPos().y - 300 );
                else if( playerScreenPos.y > 375 )
                    pos.y = -( pPlayer->GetPos().y - 375 );


                //不能超過地圖
                if( GSO::World::IEntity2d::GetRefOrigin().x >= 0 )
                    pos.x = 0;

                if( GSO::World::IEntity2d::GetRefOrigin().y >= 0 )
                    pos.y = 0;

                int MaxCameraPosx = -( m_pTerrainSystem->GetMapCX() ) + GetDrawDevice()->GetWidth();
                if( pos.x <= MaxCameraPosx )
                    pos.x = MaxCameraPosx;

                int MaxCameraPosy = -( m_pTerrainSystem->GetMapCY() ) + GetDrawDevice()->GetHeight();
                if( pos.y <= MaxCameraPosy )
                    pos.y = MaxCameraPosy;

                //GSO::World::IEntity2d::SetRefOrigin(pos);

            }

            GameVertex offsetpos;// = GSO::World::IEntity2d::GetRefOrigin();

            m_BlitSky.SetPos( GameVertex( 0, 0, 0 ) - offsetpos );


            // 山的速度是人物移動速度的八分之一
            int mx = ( ( GSO::World::IEntity2d::GetRefOrigin().x / 8 ) % 1600 );
            m_BlitMountain.SetPos( GameVertex( mx, 0, 0 ) - offsetpos );
            mx += 1600;
            m_BlitMountain2.SetPos( GameVertex( mx, 0, 0 ) - offsetpos );

            // 比較遠的雲的速度是人物移動速度的16分之一
            mx = ( ( ( offsetpos.x / 16 ) ) % 1600 + ( GetTickCount() / 1000 % 1600 ) );
            m_BlitFarCloud.SetPos( GameVertex( mx, 20, 0 ) - offsetpos );
            mx -= 1600;
            m_BlitFarCloud2.SetPos( GameVertex( mx, 20, 0 ) - offsetpos );

            // 雲的速度是人物移動速度的四分之一
            mx = ( ( ( offsetpos.x / 4 ) ) % 1600 + ( GetTickCount() / 200 % 1600 ) );
            m_BlitCloud.SetPos( GameVertex( mx, 0, 0 ) - offsetpos );
            mx -= 1600;
            m_BlitCloud2.SetPos( GameVertex( mx, 0, 0 ) - offsetpos );

            // 遮罩
            m_BlitMask.SetPos( GameVertex( 0, 120, 0 ) - offsetpos );

            //((UIInGame*)g_pCurrentUI)->UpdateMapSite(pPlayer->GetPos().x / 10, pPlayer->GetPos().y / 10);
        }


    }
}

