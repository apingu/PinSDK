// Paper server class
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
//        Pin
//           2016/07/05
#pragma once

#pragma warning(disable:4786)

#include "cs-def.h"

#include <vector>
#include <map>


namespace cyberspace
{
    namespace Paper
    {
        class Client;
        class System
        {
        private:
        
            std::vector<cyberspace::Paper::Client*>   paper_agents_;
            
        public:
        
            System();
            ~System();
            
            // return : connect size
            int                    StartUp( _IN const std::map<_cs_string, unsigned short> hosts );
            void                   Shutdown();
            
            //
            _cs_string             Set( _IN _cs_string key, _IN _cs_string val, _IN int live_sec = 0 );

            _cs_string             Setnx( _IN _cs_string key, _IN _cs_string val, _IN int live_sec = 0 );
                        
            //  @val: return current value
            // return:  1 : success, 0 : no any value, -1 : value over time
            _cs_string             Get( _IN _cs_string key );
            
            //  @val: return delete value
            _cs_string             Del( _IN _cs_string key );
        };
        
    };
};