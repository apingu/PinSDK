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

//01.== C 系統文件/C++ 系統文件

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <algorithmic/crypt-crc32.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-paper_system.h"
#include "cs-paper_client.h"


namespace cyberspace
{

    namespace Paper
    {
    
        //=============================================================================
        //
        //
        //=============================================================================
        System::System()
        {
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        System::~System()
        {
            Shutdown();
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        int System::StartUp( _IN const std::map<_cs_string, unsigned short> hosts )
        {
            Shutdown();

            for ( std::map<_cs_string, unsigned short>::const_iterator it = hosts.begin(); it != hosts.end(); it++ )
            {
                cyberspace::Paper::Client* paperclinet = new cyberspace::Paper::Client();
                
                if ( paperclinet->StartUp( it->first.c_str(), it->second ) == 1 )
                {
                    paper_agents_.push_back( paperclinet );
                }
            }
            
            return paper_agents_.size();
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        void System::Shutdown()
        {
            for ( unsigned int i = 0; i < paper_agents_.size(); i++ )
            {
                paper_agents_[ i ]->Shutdown();
                SAFE_DELETE( paper_agents_[ i ] );
            }
            
            paper_agents_.clear();
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        _cs_string System::Set( _cs_string key, _cs_string val, int live_sec )
        {
            if ( paper_agents_.empty() )
            {
                return ( "" );
            }
            
            int sid =   crc32::Encode( key.c_str(), key.size() );
            return paper_agents_[ sid % paper_agents_.size() ]->Set( key, val, live_sec );
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        _cs_string System::Setnx( _cs_string key, _cs_string val, int live_sec )
        {
            if ( paper_agents_.empty() )
            {
                return ( "" );
            }
            
            int sid =   crc32::Encode( key.c_str(), key.size() );
            return paper_agents_[ sid % paper_agents_.size() ]->Setnx( key, val, live_sec );
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        _cs_string System::Get( _cs_string key )
        {
            if ( paper_agents_.empty() )
            {
                return ( "" );
            }
            
            int sid =   crc32::Encode( key.c_str(), key.size() );
            return paper_agents_[ sid % paper_agents_.size() ]->Get( key );
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        _cs_string System::Del( _cs_string key )
        {
            if ( paper_agents_.empty() )
            {
                return ( "" );
            }
            
            int sid =   crc32::Encode( key.c_str(), key.size() );
            return paper_agents_[ sid % paper_agents_.size() ]->Del( key );
        }
        
    }
};

