#ifndef CSDEMONCOMMAND_H
#define CSDEMONCOMMAND_H

#include <CsDef.h>
#include "CsProtocol.h"
#include "CstSDataDef.h"


#define CsID_DEMON   (_CsMAXCHANNEL+15100)


enum CsDEMONCOMMAND { CsC_S_REQUEST_SERVERCOMMAND = CsID_DEMON + 1,          //
                      CsS_C_FEEDBACK_SERVERCOMMAND,                          //
                    };


#define CsSCSID_DEMON   (_CsMAXCHANNEL+15100)

enum CsSCSDEMONCOMMAND { CsSCSC_S_REQUEST_SERVERCOMMAND = CsSCSID_DEMON + 1, //
                         CsSCSS_C_FEEDBACK_SERVERCOMMAND,                    //
                       };


namespace CsDemonCommand
{
    namespace Server
    {
        inline size32_t GetRequestMessage( const byte_t* buff, CsResBuf& outbuf )
        {
            outbuf.setstring( ( const tchar_t* )buff );
            return outbuf.size();
        }
        
        inline size32_t SetFeedbackMessage( CsResBuf& outbuf, const char* resultstring )
        {
            outbuf.setstring( resultstring );
            return outbuf.size();
        }
    }
    
    
    namespace Client
    {
        inline size32_t SetRequestMessage( CsResBuf& outbuf, const char* commandstring )
        {
            outbuf.setstring( commandstring );
            return outbuf.size();
        }
        
        inline size32_t GetFeedbackMessage( const byte_t* buff, CsResBuf& outbuf )
        {
            outbuf.setstring( ( const tchar_t* )buff );
            return outbuf.size();
        }
    }
}


namespace CsSCSDemonCommand
{
    namespace Server
    {
        inline size32_t GetRequestMessage( const byte_t* buff, CsResBuf& outbuf )
        {
            outbuf.setstring( ( const tchar_t* )buff );
            return outbuf.size();
        }
        
        inline size32_t SetFeedbackMessage( CsResBuf& outbuf, const char* resultstring )
        {
            outbuf.setstring( resultstring );
            return outbuf.size();
        }
    }
    
    
    namespace Client
    {
        inline size32_t SetRequestMessage( CsResBuf& outbuf, const char* commandstring )
        {
            outbuf.setstring( commandstring );
            return outbuf.size();
        }
        
        inline size32_t GetFeedbackMessage( const byte_t* buff, CsResBuf& outbuf )
        {
            outbuf.setstring( ( const tchar_t* )buff );
            return outbuf.size();
        }
    }
}


#endif