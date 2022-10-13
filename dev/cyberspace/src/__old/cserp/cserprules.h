#ifndef CSERPRULES_H
#define CSERPRULES_H

#include "CsERPDef.h"
#include <map>
#include <set>

namespace Cyberspace
{
    namespace ERP
    {
        class Rules
        {
            typedef std::map<groupid_t, bool>  groupmap;
            std::map<rulesid_t, groupmap>  m_RuleList;
            
        public:
            Rules();
            ~Rules();
            
            void SetItem( rulesid_t ruleid, groupid_t grpid, bool b );
            bool GetItem( rulesid_t ruleid, groupid_t grpid );
        };
    };
};



#endif