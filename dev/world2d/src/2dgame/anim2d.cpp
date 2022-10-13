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
//
//
//
//
//
#include <pf-debug.h>
#include <parser\xmlschema.h>
#include <template\ptrdlist.inl>

#if !defined(_WINDOWS)
    #include <pf-string.h> // _splitpath
#endif

#include "anim2d.h"
#include <gadget\file_operator.h>

namespace GSO
{
    class sequence_t : public ptrdlist<int>
    {
    public:
        sequence_t() : m_FlipRate( 150 ) {}
        ~sequence_t() {}
        
        dword_t  m_FlipRate;
    };
    
#define ACIMTABLESEQ(i)         (*(*m_Sequence)[m_Now_Seq_Ptr])
    //CPVertex _Map_norm;      //所在地圖的基準點
    
    static int s_Anim2dSaveFileType = 1;
    
    //=============================================================================
    // NAME:
    // DESC: construct
    //=============================================================================
    Anim2d::Anim2d()
    {
        m_Counter = 0;
        m_NowAngle = -1;
        m_PlayOrder = PLAYORDER_FORWARD;
        m_PlayMode = PLAYMODE_PLAYFOREVER;
        m_PlayInterruptFrame = -3;
        //m_MaxDirection = 0;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Anim2d::~Anim2d()
    {
        Clear();
        //m_MaxDirection = 0;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Anim2d::Empty( void )
    {
        return m_Sequences.empty();
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Anim2d::Clear( void )
    {
        //m_NowAngle = 0;不能清 否則下次動作會歸回0
        if ( !m_Sequences.empty() )
        {
            std::map< direction_t, sequence_t* >::iterator iter = m_Sequences.begin();
            
            while ( iter != m_Sequences.end() )
            {
                iter->second->clear();
                SAFE_DELETE( iter->second );
                iter++;
            }
        }
        
        m_Sequences.clear();
        m_Filename.clear();
    }
    
    
    /*
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Anim2d::IsNowAnimInFirst(void)
    {
        sequence_t* seq = GetAnimSeq(m_NowAngle);
        if(seq != NULL)
        {
            return seq->isBegin();
        }
        return false;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Anim2d::IsNowAnimInLast(void)
    {
        sequence_t* seq = GetAnimSeq(m_NowAngle);
        if(seq != NULL)
        {
            return seq->isEnd();
        }
        return false;
    }
    */
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Anim2d::IsNowAnimInFrame( short order )
    {
        sequence_t* seq = GetAnimSeq( m_NowAngle );
        
        if ( seq != NULL )
        {
            if ( order != -1 )
            { return seq->GetIndex() == order; }
            else
            {
                return seq->isLast();
            }
        }
        
        return false;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    error_t Anim2d::Load( const tchar_t* filename )
    {
        if ( filename == NULL )
        { return 0; }
        
        if ( ( !m_Filename.empty() ) &&
                ( m_Filename == filename ) ) //the same action
        { return 2; }
        
        Clear();
        FileOperator pf;
        
        if ( ( pf.open( filename, _T( "rb" ) ) ) == NULL ) //開檔如果-1則失敗，傳回-1
        {
            Pf_ERROR( "Anim2d::Load: load animation file %s failure -_-b", filename );
            return 0;
        }
        
        // 新增文字模式
        tchar_t drive[_MAX_DRIVE];
        tchar_t dir[_MAX_LDIR];
        tchar_t fname[_MAX_LFNAME];
        tchar_t ext[_MAX_LEXT];
        _splitpath( filename, drive, dir, fname, ext );
        char hc;
        pf.read( &hc, sizeof( char ), 1 ); //write act count
        pf.seek( 0 );
        
        if ( hc != '<' )
        {
            word_t maxdir = 0;
            pf.read( &maxdir, sizeof( word_t ), 1 ); //write act count
            
            for ( unsigned int i = 0; i < maxdir; i++ )
            {
                direction_t angle = 0;
                pf.read( &angle, sizeof( direction_t ), 1 );
                word_t animlen = 0;
                pf.read( &animlen, sizeof( word_t ), 1 );
                sequence_t* seq = new sequence_t;
                
                for ( int a = 0; a < animlen; a++ )
                {
                    dword_t index = 0;
                    pf.read( &index, sizeof( dword_t ), 1 );
                    seq->push_back( index );
                }
                
                seq->GoBegin();
                m_Sequences.insert( std::map<direction_t, sequence_t*>::value_type( angle, seq ) );
            }
        }
        else
        {
            int size = pf.flength();
            tchar_t* buf = new tchar_t[size + 1];
            pf.seek( 0 );
            pf.read( buf, 1, size );
            buf[size] = 0;
            InStrStream iss( buf );
            XMLNode xml;
            
            if ( xml.LoadFromStream( iss ) )
            {
                XMLNode* anmxml = xml.FindFirstNode( _T( "ANIMATION" ) );
                
                while ( anmxml )
                {
                    XMLNode* seqxml = anmxml->FindFirstNode( _T( "SEQUENCE" ) );
                    
                    while ( seqxml )
                    {
                        XMLAttribute* dir = seqxml->FindAttribute( "Dir" );
                        
                        if ( dir )
                        {
                            // 取出方向
                            direction_t angle = atoi( dir->value_.c_str() );
                            sequence_t* seq = new sequence_t;
                            tchar_t* seqstring = new tchar_t[strlen( seqxml->text_.c_str() ) + 1];
                            strcpy( seqstring, seqxml->text_.c_str() );
                            tchar_t* seqid = strtok( seqstring, "," );
                            
                            while ( seqid )
                            {
                                dword_t index = atoi( seqid );
                                seq->push_back( index );
                                seqid = strtok( NULL, "," );
                            }
                            
                            SAFE_DELETE_ARRAY( seqstring );
                            seq->GoBegin();
                            m_Sequences.insert( std::map<direction_t, sequence_t*>::value_type( angle, seq ) );
                        }
                        
                        XMLAttribute* fliprate = seqxml->FindAttribute( _T( "FlipRate" ) );
                        
                        if ( fliprate )
                        {
                            SetFlipRate( atoi( fliprate->value_.c_str() ) );
                        }
                        
                        seqxml = anmxml->FindNextNode( _T( "SEQUENCE" ) );
                    }
                    
                    anmxml = xml.FindNextNode( _T( "ANIMATION" ) );
                }
            }
            
            delete[] buf;
        }
        
        pf.close();
        m_Filename = filename;
        // 因為 如果是面對自己的時候 就會把方向給改變掉了
        //m_NowAngle = -1;
        return 1;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    error_t Anim2d::Save( const tchar_t* filename )
    {
        FileOperator pf;
        //FILE* pf;
        
        if ( ( pf.open( filename, _T( "wb" ) ) ) == NULL ) //開檔如果-1則失敗，傳回-1
        {
            Pf_ERROR( "Anim2d::Save: save animation file %s failure -_-b", filename );
            return 0;
        }
        
        if ( s_Anim2dSaveFileType == 0 )
        {
            word_t maxdir = m_Sequences.size();
            pf.write( &maxdir, sizeof( word_t ), 1 ); //write act count
            std::map<direction_t, sequence_t*>::iterator iter = m_Sequences.begin();
            
            while ( iter != m_Sequences.end() )
            {
                direction_t angle = iter->first;
                pf.write( &angle, sizeof( direction_t ), 1 );
                word_t animlenght = iter->second->size();
                pf.write( &animlenght, sizeof( word_t ), 1 );
                
                for ( unsigned int i = 0; i < animlenght; i++ )
                {
                    pf.write( &( *iter->second )[i], sizeof( dword_t ), 1 );
                }
                
                iter++;
            }
        }
        else
        {
            char drive[_MAX_DRIVE];
            char dir[_MAX_LDIR];
            char fname[_MAX_LFNAME];
            char ext[_MAX_LEXT];
            _splitpath( filename, drive, dir, fname, ext );
            std::string xamstring;
            char xmlbuff[512];
            sprintf( xmlbuff, "<AnmX_version_0.1>\n" );
            xamstring += xmlbuff;
            //pf.write(&xmlbuff, strlen(xmlbuff), 1);  //write act count
            sprintf( xmlbuff, "    <ANIMATION Name=\"%s\">\n", fname );
            xamstring += xmlbuff;
            std::map<direction_t, sequence_t*>::iterator iter = m_Sequences.begin();
            
            while ( iter != m_Sequences.end() )
            {
                sprintf( xmlbuff, "        <SEQUENCE Dir=\"%d\" FlipRate=\"%d\">", iter->first, iter->second->m_FlipRate );
                xamstring += xmlbuff;
                //pf.write(&xmlbuff, strlen(xmlbuff), 1);
                sequence_t* pAnmSeq = iter->second;
                
                for ( unsigned int i = 0; i < pAnmSeq->size(); i++ )
                {
                    sprintf( xmlbuff, "%d,", ( *pAnmSeq )[i] );
                    xamstring += xmlbuff;
                    //pf.write(&xmlbuff, strlen(xmlbuff), 1);
                }
                
                sprintf( xmlbuff, "</SEQUENCE>\n" );
                xamstring += xmlbuff;
                //pf.write(&xmlbuff, strlen(xmlbuff), 1);
                iter++;
            }
            
            sprintf( xmlbuff, "    </ANIMATION>\n" );
            xamstring += xmlbuff;
            sprintf( xmlbuff, "</AnmX_version_0.1>" );
            xamstring += xmlbuff;
            pf.write( xamstring.c_str(), strlen( xamstring.c_str() ), 1 );
        }
        
        pf.close();
        m_Counter = 0;
        return 1;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    const tchar_t* Anim2d::GetFilename( void ) const
    {
        return m_Filename.c_str();
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Anim2d::operator++( int )
    {
        sequence_t* seq = GetAnimSeq( m_NowAngle );
        
        if ( seq != NULL )
        {
            ( *seq )++;
        }
        
        return;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Anim2d::operator--( int )
    {
        sequence_t* seq = GetAnimSeq( m_NowAngle );
        
        if ( seq != NULL )
        {
            ( *seq )--;
        }
        
        return;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Anim2d::SetPlayMode( IMAGE_PLAY_MODE mode, short frame )
    {
        m_PlayMode = mode;
        
        if ( mode == PLAYMODE_PLAYFOREVER )
        { m_PlayInterruptFrame = -3; }
        else
        { m_PlayInterruptFrame = frame; }
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Anim2d::SetPlayOrder( IMAGE_PLAY_ORDER order )
    {
        m_PlayOrder = order;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Anim2d::IMAGE_PLAY_MODE Anim2d::GetPlayMode()
    {
        return  m_PlayMode;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    short Anim2d::GetPlayInterrupt()
    {
        return m_PlayInterruptFrame;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Anim2d::IMAGE_PLAY_ORDER Anim2d::GetPlayOrder()
    {
        return m_PlayOrder;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Anim2d::ChangeDirection( direction_t angle )
    {
        //         if(angle<0)
        //         {
        //             std::map< int, sequence_t* >::iterator iter = m_Sequences.begin();
        //             if(iter != m_Sequences.end())
        //             {
        //                 iter->second->GoBegin();
        //                 m_NowAngle = iter->first;
        //                 return true;
        //             }
        //             return false;
        //         }
        if ( m_NowAngle == angle )
        { return true; }
        
        if ( m_Sequences.empty() ) //沒有動作表
        {
            return false;
        }
        
        // 如果找不到動作 試著重新載入檔案3次
        for ( int i = 0; i < 3; i++ )
        {
            sequence_t* seq = GetAnimSeq( angle );
            
            if ( seq != NULL )
            {
                SetFlipRate( seq->m_FlipRate );
                seq->GoBegin();
                m_NowAngle = angle;
                return true;
            }
            else
            {
                _tstring str = GetFilename();
                Clear();
                Load( str.c_str() );
            }
        }
        
        // 怎麼找也找不到 設為第一個吧
        std::map< direction_t, sequence_t* >::iterator iter = m_Sequences.begin();
        
        if ( iter != m_Sequences.end() )
        {
            m_NowAngle = iter->first;
        }
        
        return false;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Anim2d::ChangeFrame( int fid )
    {
        if ( m_Sequences.empty() ) //沒有動作表
        {
            return false;
        }
        
        if ( GetNowFrame() == fid )
        { return true; }
        
        std::map<direction_t, sequence_t*>::iterator dir_iter = m_Sequences.begin();
        
        while ( dir_iter != m_Sequences.end() )
        {
            sequence_t* seq = GetAnimSeq( dir_iter->first );
            
            if ( seq )
            {
                int cc = 0;
                sequence_t::iterator seq_iter = seq->begin();
                
                while ( seq_iter != seq->end() )
                {
                    if ( ( *seq_iter ) == fid )
                    {
                        m_NowAngle = dir_iter->first;
                        seq->GoTo( cc );
                        return true;
                    }
                    
                    cc++;
                    seq_iter++;
                }
            }
            
            dir_iter++;
        }
        
        return false;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Anim2d::SetFlipRate( dword_t rate, int a )
    {
        m_FlipRate.SetCycle( rate );
        
        if ( a == 0 )
        {
            std::map< direction_t, sequence_t* >::iterator iter = m_Sequences.find( m_NowAngle );
            
            if ( iter != m_Sequences.end() )
            {
                iter->second->m_FlipRate = rate;
            }
        }
        else
        {
            std::map< direction_t, sequence_t* >::iterator iter = m_Sequences.begin();
            
            while ( iter != m_Sequences.end() )
            {
                iter->second->m_FlipRate = rate;
                iter++;
            }
        }
        
        return;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Anim2d::ResetFilpRate()
    {
        m_FlipRate.Reset();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    mstimen_t Anim2d::GetFlipRate( void ) const
    {
        return m_FlipRate.GetCycle();
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Anim2d::GetFilpInterval( void )
    {
        return m_FlipRate.IsInterval();
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    sequence_t* Anim2d::AddAnimSeq( direction_t angle )
    {
        std::map< int, sequence_t* >::iterator iter = m_Sequences.find( angle );
        
        if ( iter != m_Sequences.end() )
        {
            return iter->second;
        }
        
        sequence_t* seq = new sequence_t;
        m_Sequences.insert( std::map< int, sequence_t* >::value_type( angle, seq ) );
        return seq;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    unsigned short Anim2d::GetMaxDirection( void )
    {
        return m_Sequences.size();
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    sequence_t* Anim2d::GetAnimSeq( direction_t angle )
    {
        if ( m_Sequences.empty() )
        { return NULL; }
        
        std::map< direction_t, sequence_t* >::iterator iter = m_Sequences.find( angle );
        
        if ( iter != m_Sequences.end() )
        { return iter->second; }
        
        return NULL;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Anim2d::PushSeq( sequence_t* seq, uint_t e )
    {
        seq->push_back( e );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    uint_t Anim2d::GetSeqSize( sequence_t* seq )
    {
        return seq->size();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Anim2d::SetFrameId( direction_t angle, unsigned int step, unsigned int no )
    {
        sequence_t* seq = GetAnimSeq( angle );
        
        if ( seq != NULL )
        { seq->operator[]( step ) = no; }
        
        return 1;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Anim2d::GetSeqFrameId( sequence_t* seq, uint_t step )
    {
        return ( *seq )[step];
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Anim2d::GetFrameId( direction_t angle, unsigned int step )
    {
        sequence_t* seq = GetAnimSeq( m_NowAngle );
        
        if ( seq != NULL )
        {
            ( *seq )[step];
        }
        
        return 0;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    direction_t Anim2d::GetNowAngle( void )
    {
        if ( m_Sequences.empty() )
        { return m_NowAngle = -1; }
        
        if ( m_NowAngle == -1 )
        {
            std::map< direction_t, sequence_t* >::iterator iter = m_Sequences.begin();
            
            if ( iter != m_Sequences.end() )
            {
                return m_NowAngle = iter->first;
            }
        }
        
        return m_NowAngle;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    short Anim2d::GetNowPlayCount( void )
    {
        return m_Counter;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Anim2d::GetNowFrame( void )
    {
        sequence_t* seq = GetAnimSeq( m_NowAngle );
        
        if ( ( seq != NULL ) && ( !seq->empty() ) )
        {
            return ( *seq )();
        }
        
        std::map< direction_t, sequence_t* >::iterator iter = m_Sequences.begin();
        
        if ( iter != m_Sequences.end() )
        {
            seq = iter->second;
            
            if ( seq != NULL )
            {
                if ( seq->empty() )
                { return 0; }
                else
                { return ( *seq )(); }
            }
        }
        
        return 0;
        //用在特殊地方?沒有動作表
        //return m_NowAngle;
        //return m_NowAngle/45;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    short Anim2d::GetNowState( void )
    {
        return m_NowState;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    unsigned int Anim2d::GetNowAnimSum( void )
    {
        sequence_t* seq = GetAnimSeq( m_NowAngle );
        
        if ( seq != NULL )
        {
            return seq->size();
        }
        
        return 0;
    }
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Anim2d::GetNowFrameStep( void )
    {
        sequence_t* seq = GetAnimSeq( m_NowAngle );
        
        if ( seq != NULL )
        {
            return seq->GetIndex();
        }
        
        return 0;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Anim2d::SetNowFrameStep( int step )
    {
        sequence_t* seq = GetAnimSeq( m_NowAngle );
        
        if ( seq != NULL )
        {
            //the last one
            if ( step == -1 )
            {
                seq->GoTo( ( seq->size() - 1 ) );
            }
            else
            {
                seq->GoTo( step );
            }
        }
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    size32_t Anim2d::GetNowFrameCount( void )
    {
        return GetAngleFrameCount( m_NowAngle );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Anim2d::GetAngleFrameCount( direction_t angle )
    {
        sequence_t* seq = GetAnimSeq( angle );
        
        if ( seq != NULL )
        {
            return seq->size();
        }
        
        return 0;
    }
    
    //=============================================================================
    // 撥放一元素的動畫
    //
    //
    //                      -1 間隔時間
    //  連續撥放傳入 -1     -2 撥完一次    動作的頁次
    //  次數撥放傳入  n     -3 撥完 n次    動作的頁次
    //=============================================================================
    short Anim2d::Update( int count )
    {
        if ( Empty() )
        { return m_NowState = PLAY_STOP; }
        
        if ( count != -1 )
        {
            if ( m_Counter >= count )
            { return m_NowState = PLAY_STOP; }
        }
        
        if ( m_FlipRate.Interval() >= TIMER_INTERRUPT )
        {
            //check play stop flag
            switch ( m_PlayMode )
            {
            case PLAYMODE_STOP:
                {
                    return m_NowState = PLAY_INTERRUPT;
                }
                
            /*
            case PLAYMODE_STOP_ON_FIRST:
            {
                if(IsNowAnimInFirst())
                    return m_NowState = PLAY_INTERRUPT;
                break;
            }
            case PLAYMODE_STOP_ON_LAST:
            {
                if(IsNowAnimInLast())
                    return m_NowState = PLAY_INTERRUPT;
                break;
            }
            */
            case PLAYMODE_INTERRUPT:
                {
                    if ( IsNowAnimInFrame( m_PlayInterruptFrame ) )
                    { return m_NowState = PLAY_INTERRUPT; }
                    
                    break;
                }
            }
            
            bool bchecktime = false;
            
            //check play order
            switch ( m_PlayOrder )
            {
            case PLAYORDER_FORWARD:
                {
                    bchecktime = IsNowAnimInFrame( -1 );
                    operator++( 0 );
                    break;
                }
                
            case PLAYORDER_BACK:
                {
                    bchecktime = IsNowAnimInFrame( 0 );
                    operator--( 0 );
                    break;
                }
            }
            
            if ( bchecktime )
            {
                if ( count == -1 ) //連續撥放的狀態下，每撥完一回
                {
                    return m_NowState = PLAY_ONE_TIMES;
                }
                else
                {
                    m_Counter++;
                    return m_NowState = PLAY_N_TIMES;//already play n times
                }
            }
            
            return m_NowState = GetNowFrameStep();//ActionList(act)->Index();
        }
        
        return m_NowState = PLAY_ON_INTERVAL_TIMES;//no filp image to next
    }
    
    
    
    
    /*
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //  AnimTable
    
    //=============================================================================
    // NAME:
    // DESC: construct
    //=============================================================================
    AnimTable::AnimTable()
    {
        //m_NowAct         = 0;
        m_Now_Seq_Ptr = 0;
        m_Sequence    = 0;
        //MaxAspect()   = 0;
        //lp_Now_Frame      = 0;
        //m_MaxAspect  = 1;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    AnimTable::~AnimTable()
    {
        Clear();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void AnimTable::operator++(int)
    {
        //lp_Now_Frame ++;
        ACIMTABLESEQ(m_Now_Seq_Ptr)++;
        return;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AnimTable::operator()(void)
    {
        return (m_Sequence == NULL) ? 0 : ACIMTABLESEQ(m_Now_Seq_Ptr)();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    sequence_t* AnimTable::operator[](int actno)
    {
        return (*m_Sequence)[actno];
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    AnimTable &AnimTable::operator=(AnimTable &animtable)
    {
        Clear();
        //m_file_offSet = AToffSet;
        m_Sequence = new pfstl::Array<sequence_t*>;
    
        int StepImageCount  = 0; //check var all action step count must eq image count;
        m_Sequence->allot(animtable.SequenceCount());
        for(int aci = 0;aci < animtable.SequenceCount();aci++)
        {
            (*m_Sequence)[aci] = new sequence_t;
            for(int acsi = 0;acsi < animtable.FrameCount(aci);acsi++)
            {
                (*m_Sequence)[aci]->push_back(animtable.FrameID(aci,
                                                                      acsi));
                StepImageCount ++;
            }
            (*m_Sequence)[aci]->GoBegin();
        }
    
    
        return *this;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AnimTable::empty(void)
    {
        return (m_Sequence == NULL) ? 1 : 0;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AnimTable::IsNowAnimInLast(void)
    {
        return (*m_Sequence)[m_Now_Seq_Ptr]->isEnd();//((*m_Sequence)[m_Now_Seq_Ptr]->end() == lp_Now_Frame);
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AnimTable::SequenceCount(void)
    {
        if(m_Sequence == NULL)
            return 0;
        return m_Sequence->size();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AnimTable::GetNowSeqID(void)
    {
        return m_Now_Seq_Ptr;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AnimTable::GetNowFrameOrder(void)
    {
        return (*m_Sequence)[m_Now_Seq_Ptr]->GetIndex();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AnimTable::FrameID(int act, int step)
    {
        return (*(*m_Sequence)[act])[step];
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AnimTable::FrameCount(void)
    {
        return (*m_Sequence)[m_Now_Seq_Ptr]->size();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int AnimTable::FrameCount(int AnimNo)
    {
        return (*m_Sequence)[AnimNo]->size();
    }
    
    //=============================================================================
    // NAME: Release
    // DESC:
    //=============================================================================
    void AnimTable::Release(void)
    {
        if(m_Sequence == NULL)
            return;
    
        for(uint_t i = 0 ;i < m_Sequence->size();i++)
        {
            if((*m_Sequence)[i] != 0)
                (*m_Sequence)[i]->clear();
        }
        m_Sequence->release_element();
        m_Sequence->clear();
    
        SAFE_DELETE(m_Sequence);
    }
    
    
    //=============================================================================
    // NAME: Load
    // DESC:
    //=============================================================================
    int AnimTable::Load(const tchar_t* path)
    {
        tchar_t msg[_MAX_MSG];
        FileOperator pf;
    
        if((pf.open(path, "rb")) == NULL)
        {
            sprintf(msg, "open action table file %s failure -_-b", path);
            DebugMessage(msg, "AnimTable");
            return pf.ferrno();
        }
    
        Release();
        //m_file_offSet = AToffSet;
        m_Sequence = new pfstl::Array<sequence_t*>;
    
        int StepImageCount  = 0; //check var all action step count must eq image count;
    
        pf.seek(0);
        word_t ActCount  = 0;
        pf.read(&ActCount, sizeof(ActCount), 1);
    
        m_Sequence->allot(ActCount);
        for(int aci = 0;aci < ActCount;aci++)
        {
            word_t ActStepCount  = 0;
            pf.read(&ActStepCount, 1, sizeof(ActStepCount));
            (*m_Sequence)[aci] = new sequence_t;
            for(int acsi = 0;acsi < ActStepCount;acsi++)
            {
                Pdword_t StepNo   = 0;
                pf.read(&StepNo, 1, sizeof(StepNo));
                //fread(&StepNo, 1, sizeof(StepNo), pf);
                (*m_Sequence)[aci]->push_back(StepNo);
                StepImageCount ++;
                (*m_Sequence)[aci]->GoBegin();
            }
        }
    
        //lp_Now_Frame = empty()? 0 : (*m_Sequence)[0]->begin();
    
        return 1;
    }
    
    
    //=============================================================================
    // NAME: Save
    // DESC:
    //=============================================================================
    void AnimTable::Save(const tchar_t* path)
    {
        FileOperator pf;
        //FILE* pf;
    
        if((pf.open(path, "wb")) == NULL)//開檔如果-1則失敗，傳回-1
        {
            tchar_t msg[_MAX_MSG];
            sprintf(msg, "save action table file %s failure -_-b", path);
            DebugMessage(msg, "AnimTable");
            return;
        }
    
        word_t as= SequenceCount();
        pf.write(&as, sizeof(word_t), 1);  //write act count
    
        for(word_t Act = 0;Act < as;Act++)
        {
            word_t acts  = FrameCount(Act);
            pf.write(&acts, sizeof(word_t), 1);  //write step count
    
            for(int Step = 0;Step < FrameCount(Act);Step++)
            {
                int ActStep = FrameID(Act, Step);
                pf.write(&ActStep, sizeof(Pdword_t), 1);
            }
        }
        pf.close();
    }
    
    
    //=============================================================================
    // NAME: Change_Sequ_Iterator
    // DESC:
    //=============================================================================
    void AnimTable::Change_Sequ_Iterator(uint_t Act, uint_t MaxAct, uint_t dir)
    {
        //Act = (Act * GetMaxAspect()) + dir;
        //int act = Sequence_ID(Act, MaxAspect());
        if((m_Sequence == NULL) || (((int)(m_Now_Seq_Ptr/MaxAct)) == Act))  //沒有動作表 //已經在這個動作上
            return;
    
        /////////////////////
        //uint_t LastAct = (m_Now_Seq_Ptr * MaxAct) + dir;
        uint_t PlayAct = (Act * MaxAct) + dir;
        /////////////////////
        if(PlayAct > static_cast<int>(m_Sequence->size())) //沒有這個動作
        {
            PlayAct = dir;
            //Act = 0;
        }
        //m_Counter = 0;
        (*m_Sequence)[m_Now_Seq_Ptr]->GoBegin();
        m_Now_Seq_Ptr = PlayAct;
        (*m_Sequence)[m_Now_Seq_Ptr]->GoBegin();
        //lp_Now_Frame = (*m_Sequence)[Act]->begin();
        return;
    }
    */
    
    void Anim2d::SetSaveFileType( int t )
    {
        s_Anim2dSaveFileType = t;
    }
    
    
}//namespace GE
