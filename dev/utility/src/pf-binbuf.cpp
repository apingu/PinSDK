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
//////////////////////////////////////////////////////////////////////////

//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "pf-stdlib.h"
#include "pf-binbuf.h"


#define PfBinBufExtSize   4

//=============================================================================
//
//
//=============================================================================
PfBinBuf::PfBinBuf()
{
    length_ = 0;
    buffer_ = NULL;
}

//=============================================================================
//
//
//=============================================================================
PfBinBuf::PfBinBuf( const PfBinBuf& bin )
{
    /*
    length_ = 0;
    buffer_ = NULL;
    operator = (res);
    */
    // 這樣作法很奇怪 雖然 可以加速 但 再dll 底下應該是會當掉的 可是 這邊 卻 不會 所以先這樣用
    // 等到 當掉再改吧
    //length_ = res.length_;
    //buffer_ = res.buffer_;
    //m_restyp = 0;
    length_ = 0;
    buffer_ = NULL;
    operator = ( bin );
    //res.length_ = 0;
    //res.buffer_ = NULL;
}

//=============================================================================
//
//
//=============================================================================
PfBinBuf::PfBinBuf( const _binbuf& bin )
{
    length_ = 0;
    buffer_ = NULL;
    operator = ( bin );
    //res.length_ = 0;
    //res.buffer_ = NULL;
}

//=============================================================================
//
//
//=============================================================================
PfBinBuf::PfBinBuf( const tchar_t* str )
{
    length_ = 0;
    buffer_ = NULL;
    operator = ( str );
}

//=============================================================================
//
//
//=============================================================================
PfBinBuf::PfBinBuf( _tstring& str )
{
    length_ = 0;
    buffer_ = NULL;
    operator = ( str );
    //res.length_ = 0;
    //res.buffer_ = NULL;
}

//=============================================================================
//
//
//=============================================================================
PfBinBuf::PfBinBuf( const _tstring& str )
{
    length_ = 0;
    buffer_ = NULL;
    operator = ( str );
    //res.length_ = 0;
    //res.buffer_ = NULL;
}

//=============================================================================
//
//
//=============================================================================
PfBinBuf::~PfBinBuf()
{
    clear();
}

//=============================================================================
//
//
//=============================================================================
PfBinBuf::operator ubyte_t* ( )
{
    return buffer_;
}

//=============================================================================
//
//
//=============================================================================
PfBinBuf::operator const ubyte_t* ( ) const
{
    return buffer_;
}

//=============================================================================
//
//
//=============================================================================
PfBinBuf::operator _binbuf ()
{
    _binbuf bin;
    bin.resize( length_ );
    pfstd::memcpy( &bin[ 0 ], buffer_, length_ );
    return bin;
}

//=============================================================================
//
//
//=============================================================================
ubyte_t& PfBinBuf::operator[] ( unsigned int pos )
{
    return buffer_[ pos ];
}

//=============================================================================
//
//
//=============================================================================
void PfBinBuf::operator = ( const PfBinBuf& bin )
{
    if ( ( bin.buffer_ == NULL ) || ( bin.length_ == 0 ) )
    {
        clear();
        return;
    }

    resize( bin.size() );
    pfstd::memcpy( buffer_, bin.buffer_, length_ );
}

//=============================================================================
//
//
//=============================================================================
void PfBinBuf::operator = ( const _binbuf& bin )
{
    if ( bin.empty() )
    {
        clear();
        return;
    }

    resize( bin.size() );
    pfstd::memcpy( buffer_, &bin[ 0 ], length_ );
}

//=============================================================================
//
//
//=============================================================================
const tchar_t* PfBinBuf::operator = ( const tchar_t* str )
{
    resize( (unsigned int)( ( _tcslen( str ) + 1 ) * sizeof( tchar_t ) ) );
    _tcscpy( (tchar_t*)buffer_, str );
    return (const tchar_t*)buffer_;
}

//=============================================================================
//
//
//=============================================================================
_tstring PfBinBuf::operator = ( _tstring str )
{
    resize( (unsigned int)( ( ( str.size() + 1 ) * sizeof( tchar_t ) ) ) );
    _tcscpy( (tchar_t*)buffer_, str.c_str() );
    return (const tchar_t*)buffer_;
}

//=============================================================================
//
//
//=============================================================================
PfBinBuf PfBinBuf::operator +  ( const PfBinBuf& res )
{
    PfBinBuf out;
    out.resize( length_ + res.length_ );
    pfstd::memcpy( out.buffer_, buffer_, length_ );
    pfstd::memcpy( out.buffer_ + length_, res.buffer_, res.length_ );
    return out;
}

//=============================================================================
//
//
//=============================================================================
void PfBinBuf::operator += ( const PfBinBuf& res )
{
    int len = length_ + res.length_;
    ubyte_t* buf = new ubyte_t[ len ];
    pfstd::memcpy( buf, buffer_, length_ );
    pfstd::memcpy( buf + length_, res.buffer_, res.length_ );
    copy( buf, len );
    SAFE_DELETE_ARRAY( buf );
}

//=============================================================================
//
//
//=============================================================================
void PfBinBuf::copy( const ubyte_t* buff, unsigned int len )
{
    if ( ( buff == NULL ) || ( len == 0 ) )
    {
        clear();
        return;
    }

    resize( len );
    pfstd::memcpy( buffer_, buff, length_ );
}
//  void PfBinBuf::operator = (ubyte_t* buff)
//  {
//      if(buff == NULL)
//          return;
//      if((buffer_ == NULL) || (length_ == 0))
//      {
//          assert(_T("alloc resbuf first"));
//          return;
//      }
//      __pfmemcpy(buffer_, buff, length_);
//  }

//=============================================================================
//
//
//=============================================================================
void PfBinBuf::resize( unsigned int len )
{
    if ( size() != len )
    {
        clear();

        if ( len > 0 )
        {
            buffer_ = new ubyte_t[ len + PfBinBufExtSize ];
            length_ = len;
            pfstd::memset( buffer_, 0, len + PfBinBufExtSize );
        }
    }
}

//=============================================================================
//
//
//=============================================================================
unsigned int PfBinBuf::size( void ) const
{
    return length_;
}

//=============================================================================
//
//
//=============================================================================
void PfBinBuf::clear( void )
{
    length_ = 0;
    SAFE_DELETE_ARRAY( buffer_ );
}

//=============================================================================
//
//
//=============================================================================
bool PfBinBuf::empty( void ) const
{
    return ( size() == 0 ) ? true : false;
}

//=============================================================================
//
//
//=============================================================================
const tchar_t* PfBinBuf::c_str( void ) const
{
    if ( buffer_ == NULL )
    {
        return _T( "" );
    }
    return (const tchar_t*)buffer_;
}

//=============================================================================
//
//
//=============================================================================
void PfBinBuf::setstring( const tchar_t* str )
{
    if ( str == NULL )
    {
        clear();
        return;
    }
    size32_t len = (size32_t)_tcslen( str );
    resize( len );
    pfstd::memcpy( buffer_, str, (unsigned int)( _tcslen( str ) * sizeof( tchar_t ) ) );
    //_tcscpy( ( tchar_t*)buffer_, str );
}

//=============================================================================
//
//
//=============================================================================
/*
const tchar_t* PfBinBuf::to_str( const ubyte_t* buff, unsigned int len )
{
    int reslen = len;
    if ( buff[ len - 1 ] != 0 )
    {
        reslen++;
    }

    resize( reslen );
    memcpy( buffer_, buff, len );
    buffer_[ reslen - 1 ] = 0;
    return buffer_;
}
*/

//=============================================================================
//
//
//=============================================================================
//const tchar_t* PfBinBuf::getstring( void )
//{
//    return c_str();
//}

//=============================================================================
//
//
//=============================================================================
const tchar_t* PfBinBuf::catstring( const tchar_t* str )
{
    size32_t len = length_ + ( (unsigned int)_tcslen( str ) ) + 1;
    tchar_t* newstr = new tchar_t[ len ];
    _stprintf( newstr, _T( "%s%s" ), buffer_, str );
    setstring( newstr );
    SAFE_DELETE_ARRAY( newstr );
    return (const tchar_t*)buffer_;
}
