#ifndef CSLOGINCLIENTDELEGATE_H
#define CSLOGINCLIENTDELEGATE_H


#include "CsDef.h"
//#include "CsGameDef.h"
#include "CsErr.h"


namespace Cyberspace
{
    class ServerRecord;
    
    class __SLAPI LoginClientDelegate
    {
    
    public:
    
        LoginClientDelegate() {}
        virtual ~LoginClientDelegate() {}
        
        
        virtual DWORD      OnGetVersion() = 0;
        
        
        virtual void	   OnLogin( CSERR_CODE code, Cyberspace::Client* pClient ) = 0;
        
        // server列表
        virtual void       OnListServer( const ServerRecord* pServerNode, int mt ) = 0;
        
        
        
        /*
        virtual void       OnClientConnect(void) = 0;
        //virtual void       OnClientKnockout(void) = 0;
        virtual void       OnClientDisconnection(void) = 0;
        
        
        // 建立一個新角色
        virtual void       OnGetCreateEntity(const char_t* name, const Cyberspace::privatebuf_t prop, size32_t len, short errcode) = 0;
        
        //1 刪除一個人的回答(name=NULL表示刪除失敗) (after step 3)
        virtual void       OnEntityDeleted(const char_t* name, int resultcode)		= 0;
        
        //! 當詢問所有 entity 獲得回答時(每隻一次,最後一隻以後還會傳name=NULL表示沒了)
        //! 或是要求造新的 entity 的回答(name=NULL 表示失敗,只會有一次)
        //! 請小心順序, 以上兩個運算不要混合了. (after step 1 or 3)
        virtual void       OnGetRecordEntity(const char_t* name, const Cyberspace::privatebuf_t prop, size32_t len) = 0;
        //! 選擇一個人的回答 -> 收到後可以開始遊戲 (after step 3)
        //! 將選擇的人的 eid 傳下來
        virtual void       OnGetSelectEntityId(entityid_t eid) = 0;
        
        //! 當有人進入遊戲，由Server主動通知
        //! 另外使用 EnterEntity 指令時, 如果傳回 entityId<0 則表示錯誤
        virtual void       OnJoinGame(entityid_t entityId, const Cyberspace::privatebuf_t prop, size32_t len) = 0;
        virtual void       OnExitGame(entityid_t entityId, const Cyberspace::privatebuf_t prop, size32_t len) = 0;
        
        //! 當有人進入身旁區域時，由Server主動通知
        //! 另外使用 EnterEntity 指令時, 如果傳回 entityId<0 則表示錯誤
        virtual void       OnEnterSphere(entityid_t entityId, const Cyberspace::publicbuf_t prop, size32_t len) = 0;
        //! 當有人離開身旁區域時，由Server主動通知
        virtual void       OnDepartSphere(entityid_t entityId) = 0;
        */
    };
};



#endif //CSLOGINCLIENTDELEGATE_H