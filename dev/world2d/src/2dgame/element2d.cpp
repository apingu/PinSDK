//Element.cpp game world base class
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
//                                                              Pin
//                                                                  2000 11 08
//                                               Copyright    (C)    1999    Pin
//
//#define _WIN32_WINNT 0x0400
//#include <stdint.h>
//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <pf-string.h>
#include <pf-file.h>
#include <pf-time.h>
#include <pf-debug.h>
#include <gadget\file_operator.h>

//03. == 其他 使用 函式庫的.h 文件
//
#include <amath.inl>
#include <game\helem.h>
#include <graphic\icon.h>

//04. == 此專案內.h 文件
#include "element2d.h"
#include "resource2d.h"
#include <direction2d.inl>
#include <resourcemgr.h>



namespace GSO
{
#define HEADLENAME "HEADLE.RES"
#define IMAGENAME  "IMAGES.RES"

    //extern ResourceMgr gResource2dMgr;
#define MOUSE_DBCLK_TIME  250
    enum ControlKind { kLClick, kLDragClick0, kMaxControl = 5 };

    namespace Global
    {
        LG::Point     g_ClickPoint[ kMaxControl ];
        LG::Point     g_MousePos;
        dword_t       g_DockCycle = 100;
        dword_t       g_ClickIntervalTime = 250;
        bool          g_Element2dEffectChildren = true;
        pftml::Timer  g_ElemTimer;
    }

    GSO::Element2d* ShiftAllChildren( GSO::Element2d* elem, void* pDate );

    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // 介面組成Element2d
    //
    //////////////////////////////////////////////////////////////////////////////////////////////
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Element2d::Element2d()
    {
        int temp = 0;
        memcpy( &temp, "ELEM", sizeof( int ) );
        SetCId( temp );
        //SpeedUp(6);
        //m_Master = NULL;
        m_pResource2d = NULL;
        m_pParent = NULL;
        m_ImageCatch = NULL;
        m_FrameCatch = -1;
        m_DockTime = 0;
        RenderAlpha = _RENDER_FULLALPHA;
        RenderReserver = 0XFFFFFFFF;
        m_ResRate.SetCycle( 10000 + ( rand() % 10000 ) );
        m_DataBaseEntry = 0;
        m_bDragChildren = true;
        m_AnimationPos = -1;
        HElem2d header;
        SetHeader( &header );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Element2d::~Element2d()
    {
        //ClearDeletedChild();
        std::vector<Element2d*> tempchild = m_pChildren;
        m_pChildren.clear();

        for( uint_t i = 0; i < tempchild.size(); i++ )
        {
            tempchild[ i ]->m_pParent = NULL;
            SAFE_DELETE( tempchild[ i ] );
        }

        tempchild.clear();
        Clear();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    GSO::Element2d* ShiftAllChildren( GSO::Element2d* elem, void* pData )
    {
        GameVertex* shiftpos = ( GameVertex* )pData;
        elem->Sprite2d::SetPos( elem->Sprite2d::GetPos() + ( *shiftpos ) );
        return NULL;
    }

    void Element2d::SetPos( GameVertex pos )
    {
        GameVertex spos = pos - GetPos();

        // 不能這樣做 ==>//Sprite2d::SetPos(pos);
        if( m_bDragChildren )
        {
            ForAllChildren( ShiftAllChildren, &spos );
        }
        else
        {
            ShiftAllChildren( this, &pos );
        }
    }



    //=============================================================================
    // 撥放一元素的動畫
    //
    //
    //                      -1 間隔時間
    //  連續撥放傳入 -1     -2 撥完一次    動作的頁次
    //  次數撥放傳入  n     -3 撥完 n次    動作的頁次
    //=============================================================================
    int Element2d::Update( int count )
    {
        //ClearDeletedChild();
        if( !GetVisible() )
        {
            m_Anim2d.ResetFilpRate();
        }
        else
        {
            m_Anim2d.Update( count );
        }

        //TS_Lock lock(&m_Lock);
        if( Global::g_Element2dEffectChildren ) // maybe used in debug
        {
            for( uint_t i = 0; i < m_pChildren.size(); ++i )
            {
                //Element2d *e2d = ;
                m_pChildren[ i ]->Update();
            }
        }

        //後清除會照成 DEBUG時已取出的指標被刪除
        if( m_ResRate.IsInterval() ) // >= TIMER_INTERRUPT)
        {
            if( m_pResource2d != NULL )
            {
                m_pResource2d->Update();
            }
        }

        if( m_pResource2d )
        {
            m_ImageCatch = m_pResource2d->GetRes( m_FrameCatch = m_Anim2d.GetNowFrame() );
        }
        else
        {
            m_ImageCatch = NULL;
        }

        //OnSortChilds(m_pChildren);

        if( GetVisible() )
        {
            OnDraw();
        }

        OnUpdate();
        return 0;
    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================

    //void Element2d::ClearChildren(void)
    //{
    //    m_pChildren.clear();
    //}

    //=============================================================================
    // NAME: Data_Read()
    // DESC:
    //=============================================================================
    void Element2d::SetHeader( const HElem2d* pHeader )
    {
        //ID    Header      = data.ID;               //CPElement id
        //GID               = data.GID;              //CPElement group id
        //m_Job             = data.Job;
        //Sprite2d::SetVisible ((bool)(data.Visible));                   //可見
        //Sprite2d::SetDragable((bool)(data.Dragable));                  //可拖曳
        //Sprite2d::SetEnable  ((bool)(data.Enable));                    //可作用
        //Sprite2d::SetMoveable((bool)(data.Moveable));                  //可移動
        //m_AngleOfView = data.AngleOfView;
        //m_MaxAspect   = data.MaxAspect;
        m_Distance = pHeader->MoveDistance;              //移動點數
        m_Anim2d.SetFlipRate( pHeader->ClockCycle );
        SetMoveSpeed( pHeader->MoveDistance );
        //m_FlipRate.SetCycle(data.ClockCycle);      //動作的週期時間
        //
        m_InitFlipRate = pHeader->ClockCycle;
        m_InitDistance = pHeader->MoveDistance;                //移動點數
        //m_Position(data.InitSitusX, data.InitSitusY);     //出現時的位置
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Element2d::GetHeader( HElem2d* pHeader ) const
    {
        //data->ID       Header= ID;         //CPElement id
        //data->GID            = GID;        //CPElement group id
        //data->Job            = m_Job;
        //data->Identity       = m_Identity;
        pHeader->Visible = static_cast< bool >( GetVisible() ); //static_cast<bool>(Visible);       //可見
        pHeader->Dragable = static_cast< bool >( GetDragable() ); //static_cast<bool>(Dragable);      //可拖曳
        pHeader->Enable = static_cast< bool >( GetEnable() ); //static_cast<bool>(Enable);        //可作用
        pHeader->Moveable = static_cast< bool >( GetMoveable() ); //static_cast<bool>(Moveable);      //可移動
        //data->AngleOfView = m_AngleOfView;
        //data->MaxAspect   = m_MaxAspect;
        pHeader->MoveDistance = m_Distance;        //CPElement move distance
        pHeader->ClockCycle = m_Anim2d.GetFlipRate();  //動作的週期時間
        //data->MaxAspect   = m_Anim2d.GetMaxDirection();
        //data->InitSitusX     = m_Position.x;    //出現時的位置
        //data->InitSitusY     = m_Position.y;    //出現時的位置
        return;
    }
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Element2d::SetDragChildren( bool b )
    {
        m_bDragChildren = b;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Element2d::GetDragChildren()
    {
        return m_bDragChildren;
    }

    //=============================================================================
    // NAME:
    // DESC: set all children include me selected or not
    //=============================================================================
    GSO::Element2d* SetAllChildrenSelected( GSO::Element2d* elem, void* pData )
    {
        bool* f = ( bool* )pData;
        elem->Sprite2d::SetSelected( ( *f ) );
        return NULL;
    }

    void Element2d::SetAllSelected( bool f )
    {
        SetSelected( f );
        ForAllChildren( SetAllChildrenSelected, &f );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    GSO::Element2d* GetAllChildrenSelected( GSO::Element2d* elem, void* pData )
    {
        std::vector<Element2d*>* elemlist = ( std::vector<Element2d*>* )pData;

        if( elem->Sprite2d::GetSelected() )
        {
            elemlist->push_back( elem );
        }

        return NULL;
    }

    void Element2d::GetAllSelected( std::vector<Element2d*>& SelectedList )
    {
        if( GetSelected() )
        {
            SelectedList.push_back( this );
        }

        ForAllChildren( GetAllChildrenSelected, &SelectedList );
    }

    //=============================================================================
    // NAME: Do()
    // DESC:
    //=============================================================================

    //void Element2d::Do(int Act)
    //{
    //    m_NowAct = Act;
    //    m_AnimTable.Change_Sequ_Iterator(Act, GetMaxAspect(), m_NowAspect);
    //    return;
    //}

    //=============================================================================
    // NAME: FaceTo
    // DESC:
    //=============================================================================
    bool Element2d::FaceTo( const GameVertex& destpt )
    {
        //m_ImageCatch=NULL;
        int angle = JudgDirection( destpt );

        //朝向自己 以現在的角度為角度
        if( angle == -1 )
        {
            angle = m_Anim2d.GetNowAngle();
        }

        return m_Anim2d.ChangeDirection( angle );
    }

    //=============================================================================
    // NAME: FaceTo
    // DESC:
    //=============================================================================
    bool Element2d::FaceTo( direction_t direct )
    {
        m_ImageCatch = NULL;
        return m_Anim2d.ChangeDirection( direct );
    }
    //=============================================================================
    // NAME: FaceTo
    // DESC:
    //=============================================================================
    direction_t Element2d::GetNowAngle( void )
    {
        return m_Anim2d.GetNowAngle();
    }

    //=============================================================================
    // NAME: SetFlipRate()
    // DESC:
    //=============================================================================
    void Element2d::SetFlipRate( dword_t rate )
    {
        m_Anim2d.SetFlipRate( rate );
    }

    //=============================================================================
    // NAME: GetFlipRate()
    // DESC:
    //=============================================================================
    mstimen_t Element2d::GetFlipRate()  const
    {
        return m_Anim2d.GetFlipRate();
    }

    //=============================================================================
    // NAME: Restore_Speed()
    // DESC:
    //=============================================================================
    void Element2d::ResetSpeed( void )
    {
        m_Distance = m_InitDistance;
        m_Anim2d.SetFlipRate( m_InitFlipRate );
        //m_FlipRate.SetCycle(m_InitFlipRate);
    }


    //=============================================================================
    //    Name : Animation()
    //    Desc : go next image and return it
    //=============================================================================
    const CPIcon* Element2d::Animation( int count )
    {
        if( m_pResource2d == NULL )
        {
            return NULL;
        }

        //        int ret = Update(count);
        if( m_AnimationPos == -1 )
        {
            int ret = m_Anim2d.GetNowState();//m_Anim2d.Update(count);

            if( !m_Anim2d.Empty() && ( ret == PLAY_STOP ) )
            {
                return m_pResource2d->GetRes( 0 );
            }
        }

        const CPIcon* pIcon = GetImage( m_AnimationPos );

        if( pIcon )
        {
            return pIcon;
        }

        return m_pResource2d->GetRes( 0 );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    void Element2d::SetAnimationPos( int p )
    {
        m_AnimationPos = p;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    int Element2d::GetAnimationPos()
    {
        return m_AnimationPos;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================

    //const _tstring Element2d::GetResfname(void) const
    //{
    //    return m_Filename.c_str();
    //}
    const tchar_t* Element2d::GetResfname( void ) const
    {
        return m_Filename.c_str();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================

    //const _tstring Element2d::GetAnmfname(void) const
    //{
    //    return m_Anim2d.GetFilename();
    //}

    const tchar_t* Element2d::GetAnmfname() const
    {
        return m_Anim2d.GetFilename();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    EventMessage Element2d::GetCurEvenMsg( void )
    {
        return m_CurEventMsg;
    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Element2d::Clear( int s )
    {
        if( s == 1 )
        {
            for( uint_t i = 0; i < GetChildSize(); i++ )
            {
                GSO::Element2d* elem = ( GSO::Element2d* )operator[]( i );
                elem->Clear();
            }
        }

        if( !m_Filename.empty() )
        {
            m_Filename.clear();
        }

        //ClearDeletedChild();
        m_ImageCatch = NULL;
        m_FrameCatch = -1;
        Resource2d::Destroy( m_pResource2d );
        m_pResource2d = NULL;
        //SAFE_DELETE(m_pResource2d);
        m_Anim2d.Clear();
        Sprite2d::Clear();
    }

    /*
    void Element2d::ClearDeletedChild()
    {
        for (uint_t t = 0; t < m_pDeletedChild.size(); ++t)
        {
            DetachChild(m_pDeletedChild[t]);
            m_pDeletedChild[t]->m_pParent = NULL;
            SAFE_DELETE(m_pDeletedChild[t]);
        }
        m_pDeletedChild.clear();
    }


    void Element2d::DeleteChild(Element2d* child)
    {
        for (uint_t i = 0; i < m_pChildren.size(); ++i)
        {
            if (m_pChildren[i] == child)
            {
                m_pDeletedChild.push_back(child);
                return;
            }
        }
        assert(0 &&"Element2d::DeleteChild: not my child!");
    }
    */

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    /*
    Element2d* Element2d::breed(void)
    {
        Element2d* obj  = new Element2d;
        /*
        *obj = *this;
        obj->m_Parent = this;
        obj->ID = (((*(this->m_ReferCount)) - 1) * 1000 + this->ID);
        */
        //return obj;
        //}

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================

        //Element2d &Element2d::operator =(Element2d elm)
        //{
        /*
        memcpy(this, &elm, sizeof(Element2d));
        (*m_ReferCount) ++;
        m_Parent = 0;
        int ActCount = elm.m_AnimTable.Sequence_count();
        m_AnimTable.m_Sequence->allot(ActCount);
        for(int aci = 0; aci < ActCount; aci++)
        {
            word_t ActStepCount = elm.m_AnimTable[aci]->size();
            (*m_AnimTable.m_Sequence)[aci] = new Sequence;
            for(int acsi = 0; acsi < ActStepCount; acsi++)
            {
                Pword StepNo = elm.m_AnimTable.Frame_ID(aci, acsi);
                m_AnimTable[aci]->push_back(StepNo);
            }
            m_AnimTable[aci]->GoBegin();
        }
        if(m_AnimTable.is_empty())
            m_AnimTable.lp_Now_Frame = 0;
        else
            m_AnimTable.lp_Now_Frame = m_AnimTable[0]->begin();
        m_ImageList
        //m_TarGet.clear();
        */

        //return *this;
        //}

    //============================================================================================
    //   H.D   headle data
    //   A.T   action table
    //   I.L   image  list
    //   E.S   elm    string
    //============================================================================================
    error_t Element2d::LoadRes( const tchar_t* resname )
    {
        if( !resname || *resname == '\0' )
        {
            return false;
        }

        //tchar_t msg[_MAX_MSG];
        tchar_t openpath[ _MAX_LPATH ];
        int result = 0;
        tchar_t currpath[ _MAX_LPATH ];
        mcurrentpath( currpath, resname );

        // 先檔一次 這樣可以省去讀檔的時間
        if( m_pResource2d == NULL )
        {
            m_Filename.clear();
        }

        if( m_Filename == currpath )
        {
            return 1;
        }

        FileSystem pf;
        strcpy( openpath, currpath );

        if( pf.open( openpath ) == NULL )
        {
            sprintf( openpath, _T( "%s/%s" ), GetCurrentDirectory(), currpath );
            mcurrentpath( currpath, openpath );
            strcpy( openpath, currpath );

            if( pf.open( openpath ) == NULL )
            {
                //sprintf(msg, _T("open element file %s failure -_-b\n"), openpath);
                Pf_ERROR( "Element2d::LoadRes: open element file %s failure -_-b\n", openpath );
                //OutputDebugString(msg);
                return pf.ferrno();
            }
        }

        if( m_Filename == currpath )
        {
            return 1;
        }

        Clear();//??????
        m_Filename = openpath;
        /* is a element file? */
        //pf.Lock_On(offSet);
        word_t fmtt;
        char* fmt = ( char* )( &fmtt );
        fmt[ 0 ] = 'E';
        fmt[ 1 ] = 'M';

        if( pf.checkFormat( fmtt ) != _PF_OK )
        {
            //sprintf(msg, _T("%s not a element file -_-b"), openpath);
            Pf_ERROR( "Element2d::LoadRes: %s not a element file -_-b", openpath );
            return pf.ferrno();
        }

        /* load H.D */
        tchar_t hs[ _MAX_LFNAME ];
        sprintf( hs, _T( "%s%s%s" ), m_Filename.c_str(), _T( "?" ), HEADLENAME );

        if( pf.bfind( ( tchar_t* )hs ) == _ERR_FILE_NOT_FIND )
        {
            //sprintf(msg, _T("%s read H.D file failure -_-b"), openpath);
            //DebugMessage(_T("msg"), _T("Element2d"));
            Pf_ERROR( "Element2d::LoadRes: %s read H.D file failure -_-b", openpath );
            return pf.ferrno();
        }

        HElem2d elmdata;
        pf.read( &elmdata, sizeof( elmdata ), 1 );
        SetHeader( &elmdata );
        //pf.Lock_Return();
        /*============== load A.T ==============*/
        //pf.Lock_On(base);
        //CPString as(m_filename + "?A.T");
        //CPString as;
        /*
        tchar_t as[_MAX_LFNAME];
        sprintf(as, "%s%s%s", GetFileName(), "?", ACTIONNAME);
        result = m_AnimTable.Load((tchar_t*) as);
        if(result < 0)
            return result;
            */
            /*============== load I.L ==============*/
        tchar_t is[ _MAX_LFNAME ];
        sprintf( is, _T( "%s%s%s" ), m_Filename.c_str(), _T( "?" ), IMAGENAME );
        m_pResource2d = Resource2d::Create( ( tchar_t* )is );

        /*
        uint_t load_count = min(m_Anim2d.GetMaxDirection(), PRE_LOAD_SEQUENCE_COUNT);
        m_pResource2d->SetBaseLoadedCount(load_count);

        for(int c = 0;c < load_count;c++)
            SequenceInsert(c);
            */

        if( m_pResource2d == NULL )
        {
            //assert(m_pResource2d);
            //sprintf(msg, _T("Create resouce failure in %s"), is);
            //DebugMessage(msg, _T("Element2d"));
            Pf_ERROR( "Element2d::LoadRes: Create resouce failure in %s", is );
            return result;
        }

        /*============== load E.S ==============*/
        //pf.Lock_On(base);
        //CPString es(m_filename + "?E.S");
        //m_Squence_Name_Table.Load(es());
        /*=========== refer count ==============*/
        /*
        if(m_ReferCount == 0)
        {
            m_ReferCount = new dword_t;
            (*m_ReferCount) = 0;
        }
        (*m_ReferCount)++;
        */
        pf.close();
        return 1;
    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    error_t Element2d::LoadAnm( const tchar_t* name )                     //切換動作
    {
        //m_NowAct = Act;
        if( !name || *name == '\0' )
        {
            return false;
        }

        //tchar_t msg[_MAX_MSG];
        tchar_t currpath[ _MAX_LPATH ];
        tchar_t openpath[ _MAX_LPATH ];
        mcurrentpath( currpath, name );

        if( name == m_Anim2d.GetFilename() )
        {
            return 1;
        }

        strcpy( openpath, currpath );

        if( !pfsfl::FileExist( openpath ) )
        {
            sprintf( openpath, _T( "%s/%s" ), GetCurrentDirectory(), currpath );
            mcurrentpath( currpath, openpath );

            if( !pfsfl::FileExist( currpath ) )
            {
                //sprintf(msg, _T("open anm file %s failure -_-b"), currpath);
                Pf_ERROR( "Element2d::LoadAnm, open anm file %s failure -_-b", currpath );
                return false;
            }
        }

        error_t ret = m_Anim2d.Load( currpath );
        direction_t dirt = m_Anim2d.GetNowAngle();

        // 第一次載入?
        if( dirt == -1 )
        {
            dirt = 0;
        }

        m_Anim2d.ChangeDirection( dirt );
        return ret;
    }



    //============================================================================================
    // NAME: Save()
    // DESC: save as .pelm file
    //============================================================================================
    error_t Element2d::SaveRes( const tchar_t* elempath )
    {
        _tstring savepath = elempath;

        if( savepath.empty() )
        {
            savepath = m_Filename;
        }

        if( savepath.empty() )
        {
            return -1;
        }

        HElem2d data;
        GetHeader( &data );
        memset( data.ApReserved1, 0XFF, sizeof( data.ApReserved1 ) );
        memset( data.ApReserved2, 0XFF, sizeof( data.ApReserved2 ) );
        //tchar_t msg[_MAX_MSG];
        FileOperator pf;
        tchar_t s[ _MAX_LPATH ];
        sprintf( s, _T( "%s?%s" ), savepath.c_str(), HEADLENAME );

        if( pf.open( s, _T( "r+b" ) ) == NULL )
        {
            Pf_ERROR( "Element2d::SaveRes: file %s can't save element data -_-b", savepath );
            return pf.ferrno();
        }

        pf.seek( 0 );
        pf.write( &data, 1, sizeof( HElem2d ) );
        return 1;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    entityid_t Element2d::CompareOrder( Element2d& Element2d )
    {
        if( GetPos().y > Element2d.GetPos().y )
        {
            return GetId();
        }
        else
        {
            return Element2d.GetId();
        }
    }

    //=============================================================================
    //
    // is response end
    //=============================================================================
    /*
    bool Element2d::ResponseOver()
    {
        int count = 0;
        if(m_Now_Status != 0)
            count++;
        for(int i = 0 ; i < m_TarGet.size() ; i++)
            if(m_TarGet[i]->Get_Now_Status() != 0)
                count++;
        return (count != 0) ?FALSE :TRUE;
    }
    */
    /*
    //=============================================================================
    //
    // return object status   1:60  2:40  3:20  4:0
    //=============================================================================
    void Element2d::SpeedUp(int speed)
    {
        m_Distance = speed;
        switch(m_Distance)
        {
        case 1: m_Rate.Setcycle(700); break;
        case 2: m_Rate.Setcycle(550); break;
        case 3: m_Rate.Setcycle(100); break;
        case 4: m_Rate.Setcycle(30);  break;
        case 5: m_Rate.Setcycle(25);  break;
        case 6: m_Rate.Setcycle(20);  break;
        case 7: m_Rate.Setcycle(15);  break;
        case 8: m_Rate.Setcycle(10);  break;
        case 9: m_Rate.Setcycle(5);   break;
        default: m_Distance = -1; m_Rate.Setcycle(speed);
            break;
        }
    }
    */


    ///////////////////////////////////////////////////////////////////////////////////
    //子元件控制

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Element2d* Element2d::GetRootElement( void )
    {
        Element2d* elem = this;

        while( elem->GetParent() != NULL )
        {
            elem = elem->GetParent();
        }

        return elem;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Element2d* Element2d::GetParent( void )
    {
        return m_pParent;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    GSO::Element2d* CheckAllChild( GSO::Element2d* elem, void* pDate )
    {
        if( elem == pDate )
        {
            return elem;
        }

        return NULL;
    }

    bool Element2d::AttachChild( Element2d* child )
    {
        //TS_Lock lock(&m_Lock);
        Element2d* root = this;

        while( root->m_pParent != NULL )
        {
            root = root->m_pParent;
        }

        if( root->ForAllChildren( CheckAllChild, child ) != NULL )
        {
            return false;
        }

        //         if(child->GetId()!=-1)
        //         {
        //             if(root->FindChild(child->GetId()) !=NULL )
        //             {
        //                 char buff[255];
        //                 sprintf(buff, _T(" = = child attach %lld"), child->GetId());
        //                 assert(0 && buff);
        //                 return false;
        //
        //             }
        //         }
        child->m_pParent = this;
        m_pChildren.push_back( child );
        return true;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Element2d::DetachChild( Element2d* child )
    {
        //TS_Lock lock(&m_Lock);
        for( uint_t i = 0; i < m_pChildren.size(); ++i )
        {
            if( m_pChildren[ i ] == child )
            {
                m_pChildren[ i ] = NULL;
                m_pChildren.erase( m_pChildren.begin() + i );
                child->m_pParent = NULL;
                return true;
            }
        }

        for( uint_t i = 0; i < m_pChildren.size(); ++i )
        {
            if( m_pChildren[ i ]->DetachChild( child ) )
            {
                return true;
            }
        }

        return false;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Element2d::ReplaceChild( const Element2d* oldc, Element2d* newc )
    {
        //TS_Lock lock(&m_Lock);
        for( uint_t i = 0; i < m_pChildren.size(); i++ )
        {
            if( m_pChildren[ i ] == oldc )
            {
                m_pChildren[ i ] = newc;
                return true;
            }
        }

        return false;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Element2d* Element2d::GetChild( entityid_t index )
    {
        //TS_Lock lock(&m_Lock);
        for( unsigned int i = 0; i < GetChildSize(); ++i )
        {
            if( operator[]( i ) != 0 )
            {
                if( operator[]( i )->GetId() == index )
                {
                    return operator[]( i );
                }
            }
        }

        Element2d* temp = 0;

        for( unsigned int i = 0; i < GetChildSize(); ++i )
        {
            if( operator[]( i ) != 0 )
            {
                if( ( temp = operator[]( i )->GetChild( index ) ) != 0 )
                {
                    return temp;
                }
            }
        }

        return 0;
    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Element2d* Element2d::ForAllChildren( OnElementChildren func, void* pDate, bool c2p, bool b2t, bool self )
    {
        Element2d* retelem = NULL;

        if( c2p )
        {
            if( b2t )
            {
                for( uint_t i = m_pChildren.size(); i > 0; i-- )
                {
                    Element2d* elem = m_pChildren[ i - 1 ]->ForAllChildren( func, pDate, c2p, b2t );

                    if( elem != NULL )
                    {
                        return elem;
                    }
                }
            }
            else
            {
                for( uint_t i = 0; i < m_pChildren.size(); i++ )
                {
                    Element2d* elem = m_pChildren[ i ]->ForAllChildren( func, pDate, c2p, b2t );

                    if( elem != NULL )
                    {
                        return elem;
                    }
                }
            }

            if( ( func != NULL ) && ( self ) )
            {
                retelem = ( *func )( this, pDate );
            }
        }
        else
        {
            if( ( func != NULL ) && ( self ) )
            {
                retelem = ( *func )( this, pDate );
            }

            if( b2t )
            {
                for( uint_t i = m_pChildren.size(); i > 0; i-- )
                {
                    Element2d* elem = m_pChildren[ i - 1 ]->ForAllChildren( func, pDate, c2p, b2t );

                    if( elem != NULL )
                    {
                        return elem;
                    }
                }
            }
            else
            {
                for( uint_t i = 0; i < m_pChildren.size(); i++ )
                {
                    Element2d* elem = m_pChildren[ i ]->ForAllChildren( func, pDate, c2p, b2t );

                    if( elem != NULL )
                    {
                        return elem;
                    }
                }
            }
        }

        return retelem;
    }

    ///////////////////////////////////////////////////////////////////////////////////
    //編輯資訊

    //=============================================================================
    // NAME: Rect()
    // DESC: compute object ambit in screen site (norm = (0, 0))
    //=============================================================================
    LG::Rect Element2d::Rect( void )
    {
        const CPIcon* img = OnGetRenderImage();

        if( img == 0 )
        {
            LG::Rect rect;
            return rect;
        }
        return ( ( CPIcon* )img )->GetBoundingRect( GetPos() );
    }

    //=============================================================================
    // NAME: Screen_Rect()
    // DESC: compute object ambit in map site
    //=============================================================================
    LG::Rect Element2d::ScreenRect( void )
    {
        //GameVertex pfstl  = GetScreenPos();
        const CPIcon* img = OnGetRenderImage();

        if( img == 0 )
        {
            LG::Rect rect;
            return rect;
        }
        return img->GetBoundingRect( GetScreenPos() );
        //LG::Rect rect = Image()->Rect(GetPos());
        //LG::Rect opporect(rect.left  - pfstl.x, rect.top    - pfstl.y,
        //              rect.right - pfstl.x, rect.bottom - pfstl.y);
        //return opporect;
    }

    //=============================================================================
    // NAME: Image_Rect()
    // DESC: compute image ambit in screen site (norm = (0, 0))
    //=============================================================================
    LG::Rect Element2d::ImageRect( void )   //圖形範圍
    {
        const CPIcon* img = OnGetRenderImage();
        return ( img == 0 ) ? LG::Rect( 0, 0, 0, 0 ) : ( ( CPIcon* )img )->GetClipRect( GetPos() );
    }

    //=============================================================================
    // NAME: Image_Screen_Rect()
    // DESC: compute image ambit in screen site (norm = (0, 0))
    //=============================================================================
    LG::Rect Element2d::ScreenImageRect( void )   //圖形範圍
    {
        const CPIcon* img = OnGetRenderImage();
        return ( img == 0 ) ? LG::Rect( 0, 0, 0, 0 ) : ( ( CPIcon* )img )->GetClipRect( GetScreenPos() );
    }


    uint_t Element2d::GetWidth( void )
    {
        return ( OnGetRenderImage() == 0 ) ? 0 : ( ( CPIcon* )OnGetRenderImage() )->GetWidth();
    } //寬


    uint_t Element2d::GetHeight( void )
    {
        return ( OnGetRenderImage() == 0 ) ? 0 : ( ( CPIcon* )OnGetRenderImage() )->GetHeight();
    } //高


    uint_t Element2d::GetClipImageWidth( void )
    {
        return ( OnGetRenderImage() == 0 ) ? 0 : ( ( CPIcon* )OnGetRenderImage() )->GetClipWidth();
    } //寬


    uint_t Element2d::GetClipImageHeight( void )
    {
        return ( OnGetRenderImage() == 0 ) ? 0 : ( ( CPIcon* )OnGetRenderImage() )->GetClipHeight();
    } //高


    ///////////////////////////////////////////////////////////////////////////////////
    //判斷

    bool Element2d::IsStaticId( entityid_t wid )
    {
        return ( wid == GSO_STATIC_ID );
    }

    //=============================================================================
    //
    // 判斷一點是否一在實際範圍內
    //=============================================================================
    bool Element2d::IsCollisionByBound( GameVertex mpos )
    {
        const CPIcon* img = OnGetRenderImage();

        if( img == NULL )
        {
            //SetFocus(_LOST_FOCUS);
            return false;
        }
        else
        {
            //SetFocus(((CPIcon*)img)->IsCollisionByBound(mpos, GetPos().x, GetPos().y  - GetPos().z));
            return ( ( CPIcon* )img )->IsCollisionByBound( mpos, GetPos().x, GetPos().y - GetPos().z );
        }
        //return GetFocus();
    }

    //=============================================================================
    //
    // 判斷一點是否一在圖形範圍內
    //=============================================================================
    bool Element2d::IsCollision( GameVertex pos )
    {
        return OnCollision( pos );
    }

    //=============================================================================
    //
    // 判斷一點是否一在圖形範圍內
    //=============================================================================
    bool Element2d::IsCollisionByImage( GameVertex mpos )
    {
        if( GetVisible() == FALSE )
        {
            //SetFocus(_LOST_FOCUS);
            return false;
        }
        else
        {
            const CPIcon* img = OnGetRenderImage();

            if( img == NULL )
            {
                //SetFocus(_LOST_FOCUS);
                return false;
            }
            else
            {
                return ( ( CPIcon* )img )->IsCollisionByImage( mpos, GetPos().x, GetPos().y - GetPos().z );
            }
        }

        //return GetFocus();
    }

    //=============================================================================
    //
    // 接觸碰撞
    //=============================================================================
    bool Element2d::IsCollisionByImage( Element2d* srcobj )
    {
        if( GetVisible() == FALSE )
        {
            //return _LOST_FOCUS;
            return false;
        }

        const CPIcon* img = OnGetRenderImage();

        if( img == NULL )
        {
            //SetFocus(_LOST_FOCUS);
            return false;
        }
        else
        {
            //SetFocus(((CPIcon*)img)->IsCollisionByImage(srcobj->GetPos(), GetPos().x, GetPos().y - GetPos().z));
            return ( ( CPIcon* )img )->IsCollisionByImage( srcobj->GetPos(), GetPos().x, GetPos().y - GetPos().z );
        }

        //return GetFocus();
        /*
                GameVertex dist = srcobj->GetPos() - GetPos();

                if((dist.x*dist.x) < 2500 && (dist.y*dist.y) < 2500)
                {
                    return true;
                }
                return false;
        */
        /*
                int vertex_count = ((srcobj->GetImage()->Ambit_Shape)  &(0XF0));
                for(int i = 0; i < vertex_count; i++)
                {
                    //GameVertex pfstl =
                    //Image()->Collision(
                }
                CPIcon *img = GetImage();
                if(srcobj == 0)
                    return FALSE;
                if(img == 0)
                    return m_Focus = _LOST_FOCUS;
                return m_Focus = img->Is_Collision(srcobj->GetImage(), GetPos().x, GetPos().y);
        */
        /*
        LG::Rect Myrange = Map_Rect() ;
        LG::Rect Otrange = Otrange ;
           if ((((Myrange.left - Myrange.right) + (Otrange.left - Otrange.right)) > abs(Otrange.right - Myrange.left)) ||
               (((Myrange.bottom - Myrange.top) + (Otrange.bottom - Otrange.top)) > abs(Otrange.bottom - Myrange.top)))
        {
            return true;
        }
           */
        return FALSE;
    }



    //=============================================================================
    // NAME: DragTo()
    // DESC: compute new drag to situs (use for mouse drag)
    //=============================================================================
    /*
    void Element2d::DragTo(GameVertex mpos)
    {
        GameVertex shiftpos = mpos-Global::g_ClickPoint[kLDragClick0];
        ForAllChildren(ShiftAllChildren, &shiftpos);
        m_CurEventMsg = PE_MOUSEDRAG;
    }
    */


    //============================================================================================
    // NAME: Proc();
    // DESC: judgment event
    //============================================================================================
    int Element2d::Proc( LG::Point point, uint_t uMsg, uint_t wParam )
    {
        //ClearDeletedChild();
#if defined(_WINDOWS)
        LG::Point prevpt = Global::g_ClickPoint[ kLDragClick0 ];
        LG::Point currpt = point;

        if( ( GetEnable() == FALSE ) || ( GetVisible() == FALSE ) )
        {
            //SetFocus(FALSE);
            m_CurEventMsg = EventMessage::NONE;
            return ( int )m_CurEventMsg;
        }

        // By Sean
        // 鍵盤的輸入是全面的，也就是說，不能以滑鼠來做判斷
        switch( uMsg )
        {
        case  WM_KEYUP:
            {
                m_CurEventMsg = EventMessage::KEYUP;
                OnKeyUp( wParam );
                return ( int )m_CurEventMsg;
            }

        case WM_KEYDOWN:
            {
                m_CurEventMsg = EventMessage::KEYDOWN;
                OnKeyDown( wParam );
                return ( int )m_CurEventMsg;
            }

        case WM_CHAR:
        case WM_IME_CHAR:
            {
                // TODO: Sean
                // 先暫這樣送
                m_CurEventMsg = EventMessage::KEYDOWN;
                OnChar( wParam );
                return ( int )m_CurEventMsg;
            }

        case WM_MOUSELAST://WM_MOUSEWHEEL:
            {
                m_CurEventMsg = EventMessage::MOUSEWHEEL;
                OnMouseWheel( wParam, point );
                return ( int )m_CurEventMsg;
            }
        }

        if( Global::g_Element2dEffectChildren ) // maybe used in debug
        {
            for( uint_t i = m_pChildren.size(); i > 0; --i )
            {
                Element2d* elem = m_pChildren[ i - 1 ];
                int retproc = elem->Proc( point, uMsg, wParam );

                //                int retproc = m_pChildren[i-1]->Proc(point, uMsg, wParam);
                if( retproc != ( int )EventMessage::NONE )
                {
                    --i;

                    for( i = i; i > 0; --i )
                    {
                        elem = m_pChildren[ i - 1 ];

                        if( ( elem->GetEnable() == FALSE ) || ( elem->GetVisible() == FALSE ) )
                        {
                            continue;
                        }

                        elem->m_DockTime = 0;
                        elem->m_CurEventMsg = EventMessage::NONE;
                        elem->OnNull( point );
                    }

                    return retproc;
                }
            }
        }

        // 如果是地圖上的物件，則會以地圖的原點來進行運算
        // 如果是ui元件，則是以螢幕的左上角(0, 0)做為原點
        point = point - OnGetOrigin(); // change to map position
        //if((uMsg != WM_NULL) && (isCollision(point)) && (Enable))
        bool bOnArea = false;

        if( bOnArea = IsCollision( point ) )
            //if(true)
        {
            if( uMsg == WM_NULL )
            {
                //begin dock
                if( m_DockTime == 0 )
                {
                    //OutputDebugString("++++++++++++++++++++begin dock");
                    m_DockTime = pftml::Time::GetTickMSTime();
                }
                else
                {
                    //dword_t nowtime = ;
                    mstimen_t Minu = pftml::Time::GetTickMSTime() - m_DockTime;

                    if( Minu >= Global::g_DockCycle )
                    {
                        //tchar_t buff[255];
                        //sprintf(buff, "===========On dock %d\n", Minu);
                        //OutputDebugString(buff);
                        //會一直被呼叫 所以 一直傳入時間差
                        if( m_CurEventMsg == EventMessage::MOUSEOVER )
                        {
                            OnDock( 0, point );
                            m_CurEventMsg = EventMessage::MOUSEDOCK;
                            return ( int )m_CurEventMsg;
                        }
                        else if( m_CurEventMsg == EventMessage::MOUSEDOCK )
                        {
                            OnDock( Minu, point );
                            return ( int )m_CurEventMsg;
                        }
                    }
                }

                //return m_CurEventMsg;
            }
            else
            {
                m_DockTime = 0;

                switch( uMsg )
                {
                case WM_MOUSEMOVE:
                    {
                        //short LMouseAct = GetAsyncKeyState(VK_LBUTTON);
                        //if(GetKeyState((VK_LBUTTON)) &&
                        if( GetAsyncKeyState( ( VK_LBUTTON ) ) && ( m_CurEventMsg != EventMessage::LBUTTONUP ) )
                        {
                            if( ( GetDragable() ) &&
                                ( ( m_CurEventMsg == EventMessage::LBUTTONDOWN ) || ( m_CurEventMsg == EventMessage::MOUSEDRAG ) ) ) //begin drag
                            {
                                m_CurEventMsg = EventMessage::MOUSEDRAG;
                                //m_DragTime = g_ElemTimer.GetTickMSTime();
                                //DragTo(point-Global::g_ClickPoint[kLDragClick0]);
                                //SetPos(point+Global::g_ClickPoint[kLDragClick0]);
                                //托拉
                                GameVertex shiftpos = point - Global::g_ClickPoint[ kLDragClick0 ];

                                if( m_bDragChildren )
                                {
                                    ForAllChildren( ShiftAllChildren, &shiftpos );
                                }
                                else
                                {
                                    ShiftAllChildren( this, &shiftpos );
                                }

                                //DragTo(point);
                                Global::g_ClickPoint[ kLDragClick0 ] = point;
                                OnDrag( point );
                            }
                            else //button down and slip over
                            {
                                if( bOnArea = IsCollision( Global::g_ClickPoint[ kLDragClick0 ] ) )
                                {
                                    Global::g_ClickPoint[ kLDragClick0 ] = point;
                                    m_CurEventMsg = EventMessage::LSLIPDOWN;
                                    OnLMouseDown( wParam, point );
                                }
                                else
                                {
                                    m_CurEventMsg = EventMessage::MOUSEOVER;
                                    OnMouseOver( wParam, point );
                                }
                            }
                        }
                        else
                        {
                            if( ( m_CurEventMsg != EventMessage::MOUSEOVER ) && // 確保該函式只會在剛被切換的時候，喚起一次
                                ( Global::g_MousePos != point ) ) // 確保有移動
                            {
                                m_CurEventMsg = EventMessage::MOUSEOVER;
                                OnMouseOver( wParam, point );
                            }
                        }

                        Global::g_MousePos = point;
                        return ( int )m_CurEventMsg; //Drag Act
                    }

                case WM_LBUTTONDOWN:
                    {
                        Global::g_ClickPoint[ kLDragClick0 ] = point;
                        Global::g_ClickPoint[ kLClick ] = point;
                        m_CurEventMsg = EventMessage::LBUTTONDOWN;
                        OnLMouseDown( wParam, point );
                        //SetFocus(_GET_FOCUS);
                        return ( int )m_CurEventMsg;
                    }

                case WM_LBUTTONUP:
                    {
                        LG::Point a = currpt - prevpt;

                        //if(m_CurEventMsg==PE_MOUSEDRAG)//||
                        //GetDragable())
                        //(LengthXY(a)>2))
                        if( ( m_CurEventMsg == EventMessage::MOUSEDRAG ) || ( ( Global::g_ClickPoint[ kLClick ] != currpt ) && ( Global::g_ClickPoint[ kLClick ] != LG::Point( 0, 0 ) ) &&
                            GetDragable() ) )
                        {
                            m_CurEventMsg = EventMessage::MOUSEDRAGDROP;
                            Global::g_ClickPoint[ kLClick ] = LG::Point( 0, 0 );
                            Global::g_ClickPoint[ kLDragClick0 ] = LG::Point( 0, 0 );
                            OnDragDrop( point );
                        }
                        else
                        {
                            m_CurEventMsg = EventMessage::LBUTTONUP;
                            Global::g_ClickPoint[ kLClick ] = LG::Point( 0, 0 );
                            Global::g_ClickPoint[ kLDragClick0 ] = LG::Point( 0, 0 );
                            OnLMouseUp( wParam, point );
                        }

                        return ( int )m_CurEventMsg;
                    }

                case WM_LBUTTONDBLCLK:
                    {
                        m_CurEventMsg = EventMessage::LDBCLICK;
                        OnLDBClick( wParam, point );
                        return ( int )m_CurEventMsg;
                    }

                case WM_RBUTTONDOWN:
                    {
                        m_CurEventMsg = EventMessage::RBUTTONDOWN;
                        OnRMouseDown( wParam, point );
                        return ( int )m_CurEventMsg;
                    }

                case WM_RBUTTONUP:
                    {
                        m_CurEventMsg = EventMessage::RBUTTONUP;
                        OnRMouseUp( wParam, point );
                        return ( int )m_CurEventMsg;
                    }

                case WM_RBUTTONDBLCLK:
                    {
                        m_CurEventMsg = EventMessage::RDBCLICK;
                        OnRDBClick( wParam, point );
                        return ( int )m_CurEventMsg;
                    }

                    /*
                                    case WM_KEYDOWN:
                                        {
                                            m_CurEventMsg = PE_KEYDOWN;
                                            OnKeyDown(wParam);
                                            return m_CurEventMsg;
                                        }
                    */
                    /*
                                    case WM_KEYUP:
                                        {
                                            m_CurEventMsg = PE_KEYUP;
                                            OnKeyUp(wParam);
                                            return m_CurEventMsg;
                                        }
                    */
                    /*
                    default:
                    {
                        //if(Global::g_MousePos == point)
                        //{
                        dword_t nowtime = g_ElemTimer.GetTickMSTime();
                        dword_t Minu = nowtime - m_DockTime;
                        if(Minu >= Global::g_DockCycle)
                        {
                            OnDock(Minu, point);
                        }
                        //}
                        return PE_MOUSEOVER;
                    }
                    */
                }
            }

            //m_CurEventMsg = PE_MOUSEOVER;
            //OnMouseOver(wParam, point);
            return ( int )m_CurEventMsg;
        }

        if( !bOnArea )
            //else //outside area
        {
            GameVertex shiftpos = point - Global::g_ClickPoint[ kLDragClick0 ];
            //uint_t len = LengthXY(shiftpos);

            /*
            tchar_t buff[255];
            sprintf(buff, "len %d\n", len);
            OutputDebugString(buff);
            */

            if( ( ( m_CurEventMsg == EventMessage::MOUSEDRAG ) || ( ( prevpt != currpt ) && ( m_CurEventMsg == EventMessage::LBUTTONDOWN ) ) ) && ( GetDragable() ) && ( uMsg != WM_LBUTTONUP ) && ( uMsg != ( int )EventMessage::LSLIPDOWN ) )
            {
                m_DockTime = 0;

                //SetFocus(_GET_FOCUS);
                //托拉
                //GameVertex shiftpos = point-Global::g_ClickPoint[kLDragClick0];
                if( m_bDragChildren )
                {
                    ForAllChildren( ShiftAllChildren, &shiftpos );
                }
                else
                {
                    ShiftAllChildren( this, &shiftpos );
                }

                //
                Global::g_ClickPoint[ kLDragClick0 ] = point;
                OnDrag( point );
                m_CurEventMsg = EventMessage::MOUSEDRAG;
                return ( int )m_CurEventMsg;
            }

            m_DockTime = 0;
            // TODO: Pin FOCUS
            //SetFocus(_LOST_FOCUS);
            m_CurEventMsg = EventMessage::NONE;
            OnNull( point );
            return ( int )m_CurEventMsg;
        }

#endif
        return ( int )EventMessage::NONE;
    }

    /*
    //=============================================================================
    //    Name : GetImage()
    //    Desc : return now image point
    //=============================================================================
    const CPIcon* Element2d::GetImage(uint_t id)
    {
        if(m_pResource2d != NULL)
            return m_pResource2d->GetRes(id);
        return NULL;
    }
    */

    //=============================================================================
    //    Name : GetGetCurImage()
    //    Desc : return now image point
    //=============================================================================
    const CPIcon* Element2d::GetImage( int p )       //singled image
    {
        if( ( m_pResource2d == NULL ) || ( !GetVisible() ) )
        {
            return m_ImageCatch = NULL;
        }

        int curframe = p;

        if( curframe == -1 )
        {
            curframe = m_Anim2d.GetNowFrame();
        }

        if( m_FrameCatch != curframe )
        {
            m_ImageCatch = NULL;
        }

        if( m_ImageCatch == NULL )
        {
            m_ImageCatch = m_pResource2d->GetRes( m_FrameCatch = curframe );
        }

        return m_ImageCatch;//m_pResource2d->GetRes(m_Anim2d.GetNowFrame());
        //return m_pResource2d->GetRes(m_Anim2d.GetNowFrame());
        /*
        //沒有需要的圖形
        if(icon == NULL)
        {
            //清除多載入的圖檔
            m_pResource2d->ReleaseOverLoad();
            //預先載入兩組動作，超過則加入新圖形
            SequenceInsert(m_AnimTable.GetNowSeqID());
        }
        return m_pResource2d->m_IconBuf[i];
        */
    }


    //=============================================================================
    //
    //
    //=============================================================================
    uint_t Element2d::GetImageCount( void ) const
    {
        return ( m_pResource2d != NULL ) ? m_pResource2d->GetResCount() : 0;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    GameVertex Element2d::JudgPosition( direction_t dirt )
    {
        GameVertex pos = GetPos();

        if( BETWEEN( 0, dirt, 90 ) )         { pos.x += 1; pos.y -= 1; }
        else if( BETWEEN( 90, dirt, 180 ) )  { pos.x += 1; pos.y += 1; }
        else if( BETWEEN( 180, dirt, 270 ) ) { pos.x -= 1; pos.y += 1; }
        else if( BETWEEN( 180, dirt, 360 ) ) { pos.x -= 1; pos.y -= 1; }
        else if( ( dirt == 0 ) || ( dirt == 360 ) )  { pos.y -= 1; }
        else if( dirt == 90 )                { pos.x += 1; }
        else if( dirt == 180 )               { pos.y += 1; }
        else if( dirt == 270 )               { pos.x -= 1; }

        return pos;
    }

    //=============================================================================
    //
    // judgment m_NowAspect use slope
    //=============================================================================
    direction_t Element2d::JudgDirection( GameVertex destpt )
        //int Element2d::Faceto(GameVertex destpt)
    {
        return dir360_by_slope( GameVertex( GetPos().x, -GetPos().y, 0 ),
            GameVertex( destpt.x, -destpt.y, 0 ) );
        /*
        switch(GetMaxAspect())
        {
            case _1_Direction:
            {
                //return win_coordinate_axis_slope_8dir(GetPos(), destpt);
                return dir8_by_slope(GameVertex(GetPos().x, -GetPos().y),
                                      GameVertex(destpt.x, -destpt.y));
            }

            case _4_Direction:
            {
                //int ret = win_coordinate_axis_slope_4dir(GetPos(), destpt);
                return dir4_by_slope(GameVertex(GetPos().x, -GetPos().y),
                                      GameVertex(destpt.x, -destpt.y));
            }
            default://_8_Direction
            {
                ////int ret = win_coordinate_axis_point_8dir(GetPos(), destpt);
                //int ret = win_coordinate_axis_slope_8dir(GetPos(), destpt);
                return dir8_by_slope(GameVertex(GetPos().x, -GetPos().y),
                                      GameVertex(destpt.x, -destpt.y));
            }
        }
        */
        return -2;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // DrawNode funcion

    void Element2d::OnRenderTo( rendercontainer_t& container )
    {
        if( GetVisible() )
        {
            DrawNode::OnRenderTo( container );
        }

        for( uint_t i = 0; i < m_pChildren.size(); i++ )
        {
            if( m_pChildren[ i ]->GetVisible() )
            {
                m_pChildren[ i ]->OnRenderTo( container );
            }
        }
    }

    LG::Point3 Element2d::OnGetComparisonPos()
    {
        return GetPos();
    }

    bool Element2d::OnIsOverlap( const LG::Point& pos )
    {
        const CPIcon* img = OnGetRenderImage();

        if( img == 0 )
        {
            return false;
        }

        return ( ( CPIcon* )img )->IsCollisionByImage( pos, GetPos().x, GetPos().y );
    }



    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Element2d::GetRecord( void* pData, size32_t size )
    {
        FileOperator f;

        if( f.open( GetDataBaseName(), _T( "rb" ) ) == NULL )
        {
            return;
        }

        f.seek( size * GetDataBaseEntry(), SEEK_SET );
        f.read( pData, size, 1 );
        f.close();
        return;
    }

}//namespace GE
