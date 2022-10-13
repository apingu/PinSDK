//
//  created:    2006/07/05
//  filename:   EMCommand.h
//  file path:
//  file base:
//  file ext:   h
//  author:     Pin
//
//  purpose:
//

#ifndef CSERPCOMMAND_H
#define CSERPCOMMAND_H

#pragma warning(disable:4786)

#include <CsDef.h>
#include <CsProtocol.h>
#include <CsErr.h>
#include <memio.inl>
#include <string>

#include "CsERPDef.h"


//#include "CsCommandFactory.h"

#define EMID_BASE   5000

//
enum ERPCOMMANDID
{   // 人事資料
    EMC_S_SELECT_PROFILE_ALLLIST_REQUEST  = EMID_BASE + 100,  // 通訊錄列表
    EMS_C_SELECT_PROFILE_ALLLIST_FEEDBACK = EMID_BASE + 101,  // 通訊錄列表  公司成員的ID+中文名子+英文名子 + b新增 + b刪除 + b修改
    
    EMC_S_SELECT_PROFILE_GRPLIST_REQUEST  = EMID_BASE + 104,  // 通訊錄列表
    EMS_C_SELECT_PROFILE_GRPLIST_FEEDBACK = EMID_BASE + 105,  // 通訊錄列表  公司成員的ID+中文名子+英文名子 + b新增 + b刪除 + b修改
    
    EMC_S_SELECT_PROFILE_REQUEST          = EMID_BASE + 110,  // 個人資料 人Id
    EMS_C_SELECT_PROFILE_FEEDBACK         = EMID_BASE + 111,  // 個人的資料  個人資料 Server篩選 (職員 非職員 )

    EMC_S_INSERT_PROFILE_REQUEST          = EMID_BASE + 114,
    EMS_C_INSERT_PROFILE_FEEDBACK         = EMID_BASE + 115,

    EMC_S_UPDATE_PROFILE_REQUEST          = EMID_BASE + 118,
    EMS_C_UPDATE_PROFILE_FEEDBACK         = EMID_BASE + 119,

    EMC_S_DELETE_PROFILE_REQUEST          = EMID_BASE + 122,
    EMS_C_DELETE_PROFILE_FEEDBACK         = EMID_BASE + 123,
    
    
    //工作日誌
    EMC_S_SELECT_DIARY_REQUEST            = EMID_BASE + 310,  // 工作日誌 人ID + 日期
    EMS_C_SELECT_DIARY_FEEDBACK           = EMID_BASE + 311,  //
    
    EMC_S_INSERT_DIARY_REQUEST            = EMID_BASE + 320,
    EMS_C_INSERT_DIARY_FEEDBACK           = EMID_BASE + 321,
    
    EMC_S_UPDATE_DIARY_REQUEST            = EMID_BASE + 330,
    EMS_C_UPDATE_DIARY_FEEDBACK           = EMID_BASE + 331,
    
    EMC_S_DELETE_DIARY_REQUEST            = EMID_BASE + 340,
    EMS_C_DELETE_DIARY_FEEDBACK           = EMID_BASE + 341,
    
    
    // 出缺勤
    EMC_S_SELECT_WORKTIME_REQUEST         = EMID_BASE + 300,  // 個人出缺勤資料  人+月份
    EMS_C_SELECT_WORKTIME_FEEDBACK        = EMID_BASE + 301,  // 個人出缺勤資料  要求月份資料
    
    //EMS_C_EMPLOYEE_FEEDBACK  = EMID_BASE + 102,  // 員工的資料
    
    // 公告資料
    EMC_S_SELECT_BULLETIN_REQUEST         = EMID_BASE + 1100,  // 要求公告資料 參數 開始:年/月/日~結束:年/月/日
    EMS_C_SELECT_BULLETIN_FEEDBACK        = EMID_BASE + 1101,  // 回傳公告資料
    
    EMC_S_PING_REQUEST                    = EMID_BASE + 5500,
    EMS_C_PING_FEEDBACK                   = EMID_BASE + 5501,
};

#if defined(_WINDOWS)
    #pragma pack (push, 1)
#endif
//#pragma pack (push,1)

namespace Cyberspace
{
    namespace ERP
    {
        ////////////////////////////////////////////////////////////////////////
        // 成員
        namespace Member
        {
            // 通訊列表
            struct RecordList
            {
                memberid_t  Id;
                wchar_t  ChName[10];
                wchar_t  EnName[20];
                byte_t   State;     //狀態  上線 下線 封鎖
            } __ALIGNED( 1 );
            
            /*
            
            // 公開資料
            struct PropertyA
            {
             PropertyA() {memset( this, 0, sizeof(PropertyA));}
             Cyberspace::ERP::PubProperty   Buffer;
            };
            
            // 完整資料
            struct PropertyB
            {
             PropertyB() {memset( this, 0, sizeof(PropertyB));}
             Cyberspace::ERP::PriProperty   Buffer;
            };
            */
        }
        
        
        ////////////////////////////////////////////////////////////////////////
        // 行事曆
        namespace Diary
        {
        
        };
        
        
        ////////////////////////////////////////////////////////////////////////
        // 工作
        namespace Work
        {
        
        };
        
        
        ////////////////////////////////////////////////////////////////////////
        // 公告
        namespace Bulletin
        {
            namespace Client
            {
                //use int client
                inline size32_t SetRequestMessage( CsResBuf& buf, channelid_t& cid, Cyberspace::ERP::time_t time_begin,  Cyberspace::ERP::time_t time_end )
                {
                    cid = EMC_S_SELECT_BULLETIN_REQUEST;
                    char sendstr[1024];
                    sprintf( sendstr, "%d-%d-%d~%d-%d-%d", time_begin.year, time_begin.month, time_begin.day, time_end.year, time_end.month, time_end.day );
                    buf.resize( strlen( sendstr ) + 1 );
                    byte_t* param = _GetResBufPtr( buf );
                    return CsFactory::MakeArgument( param, "%s", sendstr );
                }
            }
        }
    }
}





///////////////////////////////////////////////////////////////////////////////////////
// 個人行事曆
enum EMDIARYTYPE
{

};


#define _MAXDIARYCONTECT 100

struct emdiary_t
{
    emdiary_t() { memset( this, 0, sizeof( emdiary_t ) );}
    diaryid_t    DiaryId;
    memberid_t   MemberId;
    short        Type;   // 種類
    bool         Alarm;  // 提醒鈴
    Cyberspace::ERP::time_t  Time;
    wchar_t      Contect[_MAXDIARYCONTECT];
} __ALIGNED( 1 );;


///////////////////////////////////////////////////////////////////////////////////////
// 會計項目

// 登出入項目



// 專案項目






// 加班/請假申請

// 離職申請

// 請款申請






// 職位類別

// 帳號類別

// 部門類別


#if defined(_WINDOWS)
    #pragma pack (pop)
#endif



/*

namespace EMCommand
{
    namespace SelectMemberList
    {
        namespace Client
        {
            inline size_t SetRequest( CsResBuf& buf )
            {
//              msg->Header.Who = CsINVALID_ID;
//              msg->Header.CommandLen = sizeof(memberid_t);
//              msg->Header.CommandId = EMC_S_SELECT_MEMBER_LIST_REQUEST;
                buf.resize(1);
                char* param = _GetResBufPtr(buf);
                param[0]=0;
                return 1;
            }


            inline void GetFeedback( char* buff, std::vector<emmemblistrecord_t>& memblist )
            {
                size_t len = (*(size_t*)buff);
                buff+=sizeof(size_t);
                for( u_int i=0; i<len; i++ )
                {
                    emmemblistrecord_t* emmemb = (emmemblistrecord_t*)buff;
                    memblist.push_back((*emmemb));
                    buff+=sizeof(emmemblistrecord_t);
                }
            }

        };

        namespace Server
        {
            inline void GetRequest( const char* buff )
            {
                return;
            }

   /*
            inline void SetFeedback( CsResBuf& buf, std::vector<emmemblistrecord_t> memblist )
            {
                //msg->Header.Who = CsINVALID_ID;
                int len = sizeof(size_t)+(memblist.size()*sizeof(emmemblistrecord_t));
                buf.resize(len);
                char* param = _GetResBufPtr(buf);
                (*((size_t*)param))=memblist.size();
                char* buff = param;
                buff+=sizeof(size_t);
                for( u_int i =0; i<memblist.size(); i++ )
                {
                    memcpy( buff, &(memblist[i]), sizeof(emmemblistrecord_t) );
                    buff+=sizeof(emmemblistrecord_t);
                }
            }

        };
    };

    namespace SelectMember
    {
        namespace Client
        {


        }

        namespace Server
        {
            inline void SetFeedback( CsResBuf& buf, const emmember_t* memberdata )
            {
                //msg->Header.Who = CsINVALID_ID;
                //msg->Header.CommandId = EMS_C_SELECT_MEMBER_FEEDBACK;

                buf.resize( sizeof(emmember_t) );
                char* param = _GetResBufPtr(buf);
                IOMEMCPY( param, memberdata, sizeof(emmember_t) );
            }

        }
    }

    namespace InsertMember
    {
        namespace Client
        {
            inline void GetFeedback( char* buff,  memberid_t& mid, int& result  )
            {
                CsFactory::SplitArgument(buff, "%q%d", &mid, &result );
            }
        }

        namespace Server
        {

            inline void SetFeedback( CsResBuf& buf, memberid_t mid, int result )
            {
                //msg->Header.Who = mid;
                //msg->Header.CommandId = EMS_C_INSERT_MEMBER_FEEDBACK;

                buf.resize( sizeof(memberid_t)+sizeof(int) );
                char* param = _GetResBufPtr((buf);
                CsFactory::MakeArgument(param, "%q%d", mid, result );
            }

        }
    }

    namespace UpdateMember
    {
        namespace Client
        {
            inline void GetFeedback( char* buff,  memberid_t& mid, int& result  )
            {
                CsFactory::SplitArgument(buff, "%q%d", &mid, &result );
            }
        }

        namespace Server
        {
            inline void SetFeedback( CsResBuf& buf, memberid_t mid, int result )
            {
                //msg->Header.Who = mid;
                //msg->Header.CommandId = EMS_C_UPDATE_MEMBER_FEEDBACK;
                buf.resize( sizeof(memberid_t)+sizeof(int) );
                char* param = _GetResBufPtr((buf);
                CsFactory::MakeArgument(param, "%q%d", mid, result );
            }
        }
    }

    namespace DeleteMember
    {
        namespace Client
        {
            inline void GetFeedback( char* buff,  memberid_t& mid, int& result  )
            {
                CsFactory::SplitArgument(buff, "%q%d", &mid, &result );
            }
        }

        namespace Server
        {
            inline void SetFeedback( CsResBuf& buf, memberid_t mid, int result )
            {
                //msg->Header.Who = mid;
                //msg->Header.CommandId = EMS_C_DELETE_MEMBER_FEEDBACK;
                buf.resize( sizeof(memberid_t)+sizeof(int) );
                char* param = _GetResBufPtr((buf);
                CsFactory::MakeArgument(param, "%q%d", mid, result );
            }

        }
    }


    //----------------------------------------------------------------------------------------
    //
    namespace SelectDiary
    {
        namespace Client
        {
            inline void SetRequest( CsResBuf& buf, memberid_t mid, emdate_t day)
            {
                //msg->Header.Who = mid;
                //msg->Header.CommandId = EMC_S_SELECT_DIARY_REQUEST;
                buf.resize(sizeof(memberid_t)+sizeof(emdate_t));
                char* param = _GetResBufPtr((buf);
                (*((memberid_t*)param))=mid;
                param+=sizeof(memberid_t);
                IOMEMCPY( param, &day, sizeof(emdate_t) );
                //msg->Header.CommandLen = CsFactory::MakeArgument(msg->Buffer.Param, "%q%v", mid, day, sizeof(emdate_t) );
            }

            inline void GetFeedback( char* buff, std::vector<emdiary_t>& diarylist )
            {
                size_t len = (*(size_t*)buff);
                buff+=sizeof(size_t);
                for( u_int i=0; i<len; i++ )
                {
                    emdiary_t* emdiary = (emdiary_t*)buff;
                    diarylist.push_back((*emdiary));
                    buff+=sizeof(emdiary_t);
                }
            }
        };

        namespace Server
        {
            inline void GetRequest( const char* buff, memberid_t& mid, emdate_t& day )
            {
                int len=0;
                //CsFactory::SplitArgument((char*)buff, "%q%v", &mid, &day, len );
                mid=(*((memberid_t*)buff));
                buff+=sizeof(memberid_t);
                IOMEMCPY( &day, buff, sizeof(emdate_t) );
             return;
            }

            inline void SetFeedback( CsResBuf& buf, memberid_t mid, const std::vector<emdiary_t>* diarylist )
            {
                //msg->Header.Who = mid;
                //msg->Header.CommandId = EMS_C_SELECT_DIARY_FEEDBACK;
                //msg->Header.CommandLen = sizeof(size_t)+(diarylist.size()*sizeof(emdiary_t));
                buf.resize(sizeof(size_t)+(diarylist->size()*sizeof(emdiary_t)));

                char* param = _GetResBufPtr((buf);
                (*((size_t*)param))=diarylist->size();
                param+=sizeof(size_t);
                for( u_int i =0; i<diarylist->size(); i++ )
                {
                    IOMEMCPY( param, &((*diarylist)[i]), sizeof(emdiary_t) );
                    param+=sizeof(emdiary_t);
                }
            }
        };
    };


    namespace InsertDiary
    {
        namespace Client
        {
            inline void SetRequest( CsResBuf& buf, const emdiary_t* diary )
            {
                //msg->Header.Who = diary->mid;
                //msg->Header.CommandId = EMC_S_INSERT_DIARY_REQUEST;

                buf.resize(sizeof(emdiary_t));
                char* param = _GetResBufPtr(buf);
                IOMEMCPY( param, diary, sizeof(emdiary_t) );

    //msg->Header.CommandLen= CsFactory::MakeArgument(msg->Buffer.Param, "%v", diary, sizeof(emdiary_t) );
            }

            inline void GetFeedback( char* buff, memberid_t& mid, int& result  )
            {
                CsFactory::SplitArgument(buff, "%q%d", &mid, &result );
            }
        };

        namespace Server
        {
            inline void GetRequest( const char* buff, emdiary_t& diary )
            {
                int len=0;
                CsFactory::SplitArgument((char*)buff, "%v", &diary, len );
            }

            inline void SetFeedback( CsResBuf& buf, memberid_t mid, int result  )
            {
                //msg->Header.Who = mid;
                //msg->Header.CommandId = EMC_S_INSERT_DIARY_REQUEST;
                //msg->Header.CommandLen= CsFactory::MakeArgument(msg->Buffer.Param, "%q%d", mid, result );

                buf.resize(sizeof(memberid_t)+sizeof(int));
                char* param = _GetResBufPtr(buf);
                CsFactory::MakeArgument(param, "%q%d", mid, result );
            }
        };
    };

    namespace DeleteDiary
    {
        namespace Client
        {
            inline void SetRequest( CsResBuf& buf, diaryid_t did )
            {
                //msg->Header.Who = 0;
                //msg->Header.CommandId = EMC_S_DELETE_DIARY_REQUEST;
                //msg->Header.CommandLen= CsFactory::MakeArgument(msg->Buffer.Param, "%q", did );
                buf.resize(sizeof(diaryid_t));
                char* param = _GetResBufPtr(buf);
                CsFactory::MakeArgument(param, "%q", did );
            }

            inline void GetFeedback( char* buff,  diaryid_t& did, int& result  )
            {
                CsFactory::SplitArgument(buff, "%q%d", &did, &result );
            }
        };

        namespace Server
        {
            inline void GetRequest( const char* buff, diaryid_t& did )
            {
                CsFactory::SplitArgument((char*)buff, "%q", &did );
            }

            inline void SetFeedback( CsResBuf& buf, diaryid_t did, int result  )
            {
                //msg->Header.Who = 0;
                //msg->Header.CommandId = EMC_S_DELETE_DIARY_REQUEST;
                //msg->Header.CommandLen= CsFactory::MakeArgument(msg->Buffer.Param, "%q%q", did, result );
                buf.resize(sizeof(diaryid_t)+sizeof(int));
                char* param = _GetResBufPtr(buf);
                CsFactory::MakeArgument(param, "%q%d", did, result );
            }
        };
    };


    namespace UpdateDiary
    {
        namespace Client
        {
            inline void SetRequest( CsResBuf& buf, const emdiary_t* diary )
            {
                //msg->Header.Who = diary->mid;
                //msg->Header.CommandId = EMC_S_SELECT_DIARY_REQUEST;
                //msg->Header.CommandLen= CsFactory::MakeArgument(msg->Buffer.Param, "%v", diary, sizeof(emdiary_t) );
                buf.resize(sizeof(emdiary_t));
                char* param = _GetResBufPtr(buf);
                IOMEMCPY( param, diary, sizeof(emdiary_t) );
                //CsFactory::MakeArgument(param, "%q", "%q%q", did, result );

            }

            inline void GetFeedback( char* buff,  memberid_t& mid, int& result  )
            {
                CsFactory::SplitArgument(buff, "%q%d", &mid, &result );
            }
        };

        namespace Server
        {
            inline void GetRequest( const char* buff, emdiary_t& diary )
            {
                //int len=0;
                //CsFactory::SplitArgument((char*)buff, "%v", &diary, len );
                IOMEMCPY( &diary, buff, sizeof(emdiary_t) );
            }

            inline void SetFeedback( CsResBuf& buf, memberid_t mid, int result  )
            {
                //msg->Header.Who = mid;
                //msg->Header.CommandId = EMC_S_SELECT_DIARY_REQUEST;
                //msg->Header.CommandLen= CsFactory::MakeArgument(msg->Buffer.Param, "%q%q", mid, result );
                buf.resize(sizeof(memberid_t)+sizeof(int));
                char* param = _GetResBufPtr(buf);
                CsFactory::MakeArgument(param, "%q%d", mid, result );

            }
        };
    };
}
*/

#endif

