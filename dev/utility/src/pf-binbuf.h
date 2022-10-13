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

#pragma once

#include <pf-def.h>
#include <vector>
#include <string>

class __SLAPI PfBinBuf
{
    unsigned int   length_;
    ubyte_t*       buffer_;

public:

    PfBinBuf();
    PfBinBuf( _IN const PfBinBuf& bin );
    PfBinBuf( _IN const _binbuf& bin );    
    PfBinBuf( _IN const tchar_t* str );
    PfBinBuf( _IN _tstring& str );
    PfBinBuf( _IN const _tstring& str );    
    ~PfBinBuf();
    
    operator        ubyte_t* ( );
    operator const  ubyte_t* ( ) const;  // windows 需要
    operator        _binbuf ( );

    //operator const  tchar_t*();
    ubyte_t&        operator[]  ( _IN unsigned int pos );
    
    void            operator =  ( _IN const PfBinBuf& bin );
    void            operator =  ( _IN const _binbuf& bin );
    const tchar_t*  operator =  ( _IN const tchar_t* str );
    _tstring        operator =  ( _IN _tstring str );
        

    PfBinBuf        operator +  ( _IN const PfBinBuf& bin );
    void            operator += ( _IN const PfBinBuf& bin );
    //void   operator = (ubyte_t* buff);
    void            copy        ( _IN const ubyte_t* bin, unsigned int len );
    
    void            resize      ( _IN unsigned int len );
    unsigned int    size        ( void ) const;
    void            clear       ( void );
    bool            empty       ( void ) const;
    
    const tchar_t*  c_str       ( void ) const;
    void            setstring   ( _IN const tchar_t* str );
    //const tchar_t*  getstring   ( void );
    const tchar_t*  catstring   ( _IN const tchar_t* str ); //catenate string
};
