#include "CsERPRules.h"


namespace Cyberspace
{
    namespace ERP
    {
        Rules::Rules()
        {
        
        }
        
        Rules::~Rules()
        {
        
        }
        
        void Rules::SetItem( rulesid_t ruleid, groupid_t grpid, bool b )
        {
            std::map<rulesid_t, groupmap>::iterator iter1 = m_RuleList.find( ruleid );
            if( iter1 != m_RuleList.end() )
            {
                std::map<groupid_t, bool>::iterator iter2 = iter1->second.find( grpid );
                if( iter2 != iter1->second.end() )
                {
                    iter2->second = b;
                }
                else
                {
                    iter1->second.insert( std::map<groupid_t, bool>::value_type( grpid, b ) );
                }
            }
            else
            {
                std::map<groupid_t, bool> grplist;
                grplist.insert( std::map<groupid_t, bool>::value_type( grpid, b ) );
                m_RuleList.insert( std::map<rulesid_t, groupmap>::value_type( ruleid, grplist ) );
            }
        }
        
        bool Rules::GetItem( rulesid_t ruleid, groupid_t grpid )
        {
            std::map<rulesid_t, groupmap>::iterator iter1 = m_RuleList.find( ruleid );
            if( iter1 != m_RuleList.end() )
            {
                std::map<groupid_t, bool>::iterator iter2 = iter1->second.find( grpid );
                if( iter2 != iter1->second.end() )
                {
                    return iter2->second;
                }
            }
            return false;
        }
    }
}
