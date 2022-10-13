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

#include "Edit2d.h"
#include <pf-time.h>

namespace GSO
{
    namespace UI
    {

        tchar_t  Edit2d::s_CursorSign = '_';
        //-----------------------------------------------------------------------------
        //  Name:   Constructor 
        //  Desc:
        //-----------------------------------------------------------------------------
        Edit2d::Edit2d()
        {
            m_InsertPos = 0;
            m_CursorSign = '_';
            m_Cursor = false;
            m_AcceptDBCS = true;
            m_CaretTime = 500;
            m_LastTime = 0;
            //  AddTextNode(_T("");
        }

        //-----------------------------------------------------------------------------
        //  Name:   Destructor  
        //  Desc:
        //-----------------------------------------------------------------------------
        Edit2d::~Edit2d()
        {

        }

        //-----------------------------------------------------------------------------
        //  Name:
        //  Desc:
        //-----------------------------------------------------------------------------
        error_t Edit2d::LoadRes( const tchar_t* filename )
        {
            if( *filename == '\0' )
            {
                Clear();
                return false;
            }

            Element2d::LoadRes( filename );
            return true;
        }


        //-----------------------------------------------------------------------------
        //  Name:       
        //  Desc:
        //-----------------------------------------------------------------------------
        void  Edit2d::SetCaretTime( mstimen_t time )
        {
            m_CaretTime = time;
        }


        //-----------------------------------------------------------------------------
        //  Name:       
        //  Desc:
        //----------------------------------------------------------------------------- 
        void Edit2d::SetCursorSign( tchar_t c )
        {
            s_CursorSign = c;
        }


        //-----------------------------------------------------------------------------
        //  Name:       
        //  Desc:
        //----------------------------------------------------------------------------- 
        void Edit2d::SetMask( bool b )
        {
            cText_Node::SetMask( b );
        }


        //-----------------------------------------------------------------------------
        //  Name:       
        //  Desc:
        //-----------------------------------------------------------------------------
        int Edit2d::Update( int count )
        {
            //JudgeCaretTime(GetTickCount());

            //pftml::Timer timer;

            mstimen_t time = pftml::Time::GetTickMSTime();
            // 用來計算字元游標(caret)的閃爍時間
            if( GetFocusWidget() == this )
            {
                if( time - m_LastTime > m_CaretTime )
                {
                    m_LastTime = time;
                    ReverseCursor( 0 );
                }
            }
            else
                SetTextNodeState( 0, ( TCHAR* )NULL, false, -1, 1, s_CursorSign );

            return BaseClass::Update( count );
        }

        /*
        //-----------------------------------------------------------------------------
        //  Name:   JudgeCaretTime
        //  Desc:   用來計算字元游標(caret)的閃爍時間
        //-----------------------------------------------------------------------------
        void Edit2d::JudgeCaretTime(int time)
        {
            if (GetFocusWidget() == this)
            {
                if (time - m_LastTime > CURSOR_TIME)
                {
                    m_LastTime = time;
                    ReverseCursor(0);
                }
            }
            else
                SetTextNodeState(0, (Tchar*)NULL, false);
        }
        */

        //-----------------------------------------------------------------------------
        //  Name:       
        //  Desc:
        //-----------------------------------------------------------------------------
        void Edit2d::OnLMouseUp( int key, LG::Point point )
        {
            SetFocusWidget();
            SetState( kStateNormal );
            ( ( IWidget2d * )GetParent() )->OnLMouseUp( this, key, point );
        }

        void Edit2d::OnKeyDown( IWidget2d* pWidget, int KeyCode )
        {
            if( m_TextNode.empty() )
                SetText( "" );

            tNode* node = ( *m_TextNode.begin() );
            if( node != 0 )
            {
                if( KeyCode == VK_LEFT )
                {
                    if( node->m_InsertPos > 0 )
                        node->m_InsertPos--;
                    else if( node->m_StartPos > 0 )
                        node->m_StartPos--;
                }
                else if( KeyCode == VK_RIGHT )
                {
                    if( node->m_InsertPos < node->m_Text.size() )
                    {
                        node->m_InsertPos++;
                        if( node->m_InsertPos > node->m_MaxLength - node->m_StartPos )
                            node->m_StartPos++;
                    }
                }
            }
            IWidget2d::OnKeyDown( pWidget, KeyCode );
        }

        void Edit2d::OnChar( IWidget2d* pWidget, unsigned nChar )
        {
            //            OutputDebugStringA("Edit2d::OnChar\n");
            static bool    BSD = false;
            int            nTmpDec = 0;

            // TOFIX: Sean
            // 如果是unicode的，表示不需要用到三個，只要一個
            tchar_t        buff[ 3 ] = { 0, 0, 0 };
            if( m_TextNode.empty() )
                SetText( "" );


            tNode* node = ( *m_TextNode.begin() );

            if( node == NULL )
                return;

            switch( nChar )
            {
            case '\t':
                //                nChar = PE_KEY_TAB;
                break;
            case '\r':
                //                nChar = PE_KEY_ENTER;
                break;
            case '\b':
                {
                    // 1) 不用講，大家都知道
                    // 2) 如果cursor在起頭的地方
                    if( ( node == NULL ) || ( node->m_InsertPos == 0 ) )
                        return;

#ifndef UNICODE
                    // 取要刪除的字的字元長度(byte數)
                    int temp = GetMBCSCharLength( node->m_Text.c_str(), node->m_InsertPos - 1 );
                    // 
                    if( temp == node->m_InsertPos )
                        temp -= 2;
#else
                    // utf-16都平等
                    int temp = node->m_InsertPos - 1;
#endif

                    // 從temp處，刪node->m_InsertPos - temp個
                    node->m_Text.erase( temp, node->m_InsertPos - temp );
                    node->m_InsertPos = temp;
                }
                //cText_Node::EraseBackspace(node);
                break;
            default:
                {

                    if( nChar < TCHAR( 0x20 ) )
                        break;

                    // 不接受dbcs, by Wood
                    if( !m_AcceptDBCS && ( nChar >= 128 ) )
                        break;

                    // 長度超過 不准輸入
                    if( node->m_Text.size() >= node->m_MaxLength )
                        break;

#ifndef UNICODE    // single byte at once                
                    buff[ 0 ] = ( char )( nChar >> 8 );
                    buff[ 1 ] = ( char )nChar;
                    nTmpDec = cText_Node::GetMBCSCharLength( buff, 1 );
                    int shiftlen = 2;
                    if( buff[ 0 ] == 0 )
                    {
                        buff[ 0 ] = buff[ 1 ];
                        buff[ 1 ] = 0;
                        shiftlen = 1;
                    }

                    if( !BSD && ( ( node->m_Text.size() + nTmpDec ) > ( node->m_MaxLength + node->m_StartPos ) ) )
                    {
                        if( node->m_OverMaxLen )
                        {
                            node->m_Text.insert( node->m_InsertPos, buff );
                            node->m_InsertPos += shiftlen;
                            if( node->m_InsertPos > ( node->m_StartPos + node->m_MaxLength ) )
                                node->m_StartPos++;
                        }
                    }
                    else
                    {
                        node->m_Text.insert( node->m_InsertPos, buff );
                        node->m_InsertPos += shiftlen;
                    }

                    if( nTmpDec == 2 && !BSD )
                        BSD = true;
                    else
                        BSD = false;

                    break;
#else    // UNICODE, WCHAR sent at once
                    // by Wood but not verified.
                    buff[ 0 ] = ( TCHAR )nChar;
                    if( ( ( node->m_Text.size() + 1 ) > ( node->m_MaxLength + node->m_StartPos ) ) )
                    {
                        if( node->m_OverMaxLen )
                        {
                            node->m_Text.insert( node->m_InsertPos, buff );
                            node->m_InsertPos++;
                            if( node->m_InsertPos > ( node->m_StartPos + node->m_MaxLength ) )
                                node->m_StartPos++;
                        }
                    }
                    else
                    {
                        node->m_Text.insert( node->m_InsertPos, buff );
                        node->m_InsertPos++;
                    }
#endif
                }
            }

        }
    };
};