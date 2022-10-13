#ifndef CSERPDEF_H
#define CSERPDEF_H

//#include <basedef.h>
#include <CsDef.h>

typedef channelid_t           rulesid_t;
                              
typedef sessionid_t           memberid_t;
                              
typedef unsigned int          companyid_t;
typedef unsigned int          departid_t;
typedef unsigned int          teamid_t;
                              
typedef unsigned int          appointmentid_t;  // 職位
                              
typedef uniqueid_t            bulletinid_t;
                              
typedef uniqueid_t            projectid_t;
typedef uniqueid_t            taskid_t;
typedef uniqueid_t            reportid_t;
                              
                              
typedef uniqueid_t            diaryid_t;


#define EMCH_NOUSE            0
#define EMCH_EMPLOYEE         1
#define EMCH_ASSOCIATE        2

#define EMDB_OFFLINE_ACCOUNT  0
#define EMDB_ONLINE_ACCOUNT   1
#define EMDB_BAN_ACCOUNT      2

#define _MAX_NAME_BUFFER      32
#define _MAX_PROPERTYBUFFER   10240

#define _MAX_C_BUFFER         1024


#if defined(_WINDOWS)
    #pragma pack (push, 1)
#endif


namespace Cyberspace
{
    namespace ERP
    {
        struct time_t
        {
            short    year;
            byte_t   month;
            byte_t   day;
            byte_t   hours;
            byte_t   minute;
            byte_t   second;
        } __ALIGNED( 1 );
        
        ////////////////////////////////////////////////////////////////////////
        // 職員資料
        namespace Member
        {
            struct PubProperty
            {
                memberid_t  Id;
                wchar_t  ChName[_MAX_NAME_BUFFER];
                wchar_t  EnName[_MAX_NAME_BUFFER];
                wchar_t  PubContentText[_MAX_PROPERTYBUFFER]; // 以XML 或其他文字格式儲存資料 以達到自訂
            } __ALIGNED( 1 );
            
            struct PriProperty : public PubProperty
            {
                wchar_t  PriContentText[_MAX_PROPERTYBUFFER];
            } __ALIGNED( 1 );
            
            /*
            // 基本
            struct Identity_t
            {
             memberid_t  Id;
             wchar_t  ChName[32];
             wchar_t  EnName[32];
             wchar_t  NickName[32];
             emdate_t    Birthday;
             byte_t     Blood;
             byte_t     Sex;
             byte_t     State;   //狀態  上線 下線 封鎖
             bool     Marry;
             wchar_t  Nationality[32];
             wchar_t  Graduate_School[32];
             wchar_t  Photo[32];
            }__ALIGNED(1);
            
            // 聯絡
            struct Address_t
            {
             wchar_t  Phone1[16];
             wchar_t  Phone2[16];
             wchar_t  Phone3[16];
             wchar_t  E_Mail1[64];
             wchar_t  E_Mail2[64];
             wchar_t  Address1[64];
             wchar_t  Address2[64];
            }__ALIGNED(1);
            
            // 公開
            struct PublicData_t
            {
             wchar_t  Company[32];
             wchar_t  Department[32];
             wchar_t  Team[32];
             wchar_t  Appointment[32];
             memberid_t  AgentId;
             byte_t     Capacity;    //身分  員工 ....
            }__ALIGNED(1);
            
            // 隱私資料
            struct PrivateData_t
            {
             wchar_t  IDNember[32];
             emdate_t    Arrive;
             emdate_t    Leave;
             u_int    Salary;
             u_int    Bouns;
            }__ALIGNED(1);
            */
        };
        
        
        ////////////////////////////////////////////////////////////////////////
        // 公告資料
        namespace Bulletin
        {
            struct Record
            {
                bulletinid_t   Id;
                Cyberspace::ERP::time_t Build_Time;
                int            Type;  //
                wchar_t        WriterName[_MAX_NAME_BUFFER];
                wchar_t        ContentText[_MAX_C_BUFFER];
            } __ALIGNED( 1 );
        };
        
        ////////////////////////////////////////////////////////////////////////
        // 工作
        namespace Work
        {
            struct EProject
            {
                projectid_t     Id;
                Cyberspace::ERP::time_t  Build_Time;
                wchar_t         OwnerName[_MAX_NAME_BUFFER];  // 負責人
                wchar_t         CompanyName[_MAX_NAME_BUFFER];   // 負責公司
                wchar_t         CustomerName[_MAX_NAME_BUFFER];  // 客戶對象
                Cyberspace::ERP::time_t  Start_Time;
                Cyberspace::ERP::time_t  Close_Time;
                wchar_t         ContentText[_MAX_C_BUFFER];
            } __ALIGNED( 1 );
            
            struct ETask
            {
                projectid_t     Id;
                Cyberspace::ERP::time_t  Build_Time;
                projectid_t     ProjectId;
                wchar_t         OwnerName[_MAX_NAME_BUFFER];  // 負責人
                Cyberspace::ERP::time_t  Start_Time;
                Cyberspace::ERP::time_t  Close_Time;
                int             State;
                wchar_t         ContentText[_MAX_C_BUFFER];
            } __ALIGNED( 1 );
            
            struct EReport
            {
                reportid_t      Id;
                Cyberspace::ERP::time_t  Build_Time;
                taskid_t        TaskId;
                wchar_t         OwnerName[_MAX_NAME_BUFFER];  // 負責人
                int             State;
                wchar_t         ContentText[_MAX_C_BUFFER];
            } __ALIGNED( 1 );
        }
    }
}

//__ALIGNED_POP
//#pragma pack (pop)
//__attribute__ ((packed));
#if defined(_WINDOWS)
    #pragma pack (pop)
#endif

#endif