//
///    File name:    Doll.h
///    By:            Sean Phoenix
///    Date:        2004/2/5
//    
/// Update:        2004/3/10

#ifndef DOLL_H
#define DOLL_H

#include <vector>
#include <map>
#include <pf-def.h>
#include "avataritems.h"
#include "avatarorder.h"

namespace DOLL
{
    class AvatarNode;
};


#define MAX_STRLEN        32
#define LAYERS            16        // 十六層
#define DIRECTION        8        // 八方向
#define ListType        std::vector<DOLL::AvatarNode*>

namespace DOLL
{
    class AvatarNode : public Avataritems
    {
    public:
        AvatarNode();
        ~AvatarNode();
    };


    class CSorting
    {
    public:
        CSorting(void);
        virtual ~CSorting(void);

        int                    Init();                                            // 啟始排序表格
        void                BeginSort(int direct);                            // 把之前的資料清空，丟入方向，並準備要開始丟入item
        void                Item(AvatarNode *item);                            // 丟入待排序的item
        ListType *            EndSort();                                        // 結束排序並傳回排序好的順序
        void                GetWeight(AvatarNode* item);                        // 取得排序資訊表格裡的權重
        int                 GetSortInfo(int layer, int direct);
        void                  SetSortInfo(int layer, int direct, int weight);
        int                    Save(const tchar_t* filename);
        int                    Load(const tchar_t* filename);

    protected:
        // attribute
        int            m_SortInfo[LAYERS][DIRECTION];                // 排序資訊表格，二維陣列，x表方向，y表層級，目前定義為八個方向十六層
        int            m_NowDirection;
        ListType    m_Stack;
        
    };

    class CDoll
    {
    public:
        CDoll(void);
        virtual ~CDoll(void);
        
        // method
        int                    Init();

        CSorting            *m_pSort;
        //CItemList            *m_pItemList;
    protected:
        // attribute

    };


    class CAvatarList
    {
        std::map<longlong_t, AvatarNode*>    m_ItemMap;                                /// 所有的物件
        tchar_t                            m_Database[MAX_STRLEN];                    // 

    public:

        CAvatarList();
        ~CAvatarList();

        int               Load        (const tchar_t* filename);                    // 讀檔
        AvatarNode*    GetItemInfo (longlong_t id);                               // 取得item的資訊
    };


    /*
    class CItemList
    {
    public:
    CItemList(void);
    virtual ~CItemList(void);

    // method
    int                    Add         (tchar_t*  group, AvatarNode *item);                // 增加一筆item資料到清單中
    int                    Del         (int id);                                        // 刪除清單中一筆資料
    int                    Del         (const tchar_t* filename, const tchar_t* group = NULL);                // 刪除清單中一筆資料
    int                    Sort        ();                                                // 根據定義的群組做分類
    int                    Save        (tchar_t* filename);                            // 存檔
    int                    Load        (const tchar_t* filename);                        // 讀檔
    int                    SetPathname (tchar_t*  pathname);
    int                    SetFilename (tchar_t*  filename);
    int                    GetPathname (tchar_t*  buf);
    int                    GetFilename (tchar_t*  buf);
    int                    SetIDCount  (int count);
    int                    GetIDCount  ();
    ListType*            Search      (const tchar_t* group = NULL);                // 搜尋，取出群組的清單
    tchar_t*             GetNextGroup(const tchar_t* group) const;                // 取得下一個group的名字
    SItemList *            GetGroup    (const tchar_t* group);                        // 取得group的指標
    AvatarNode *                GetItemInfo (int id);                                    // 取得item的資訊
    AvatarNode *                GetItemInfo (const tchar_t* filename, const tchar_t* group = NULL);        // 重載，並以item的檔名做搜尋

    protected:
    //attribute
    SItemList*            m_pItemList;                                    // 物件清單
    ListType*            m_pTmpList;                                        // 暫時的清單
    ListType            m_ItemArray;                                    /// 所有的物件
    CSorting*            m_pSort;
    tchar_t                m_Database[MAX_STRLEN];                            // 
    tchar_t                m_Filename[MAX_STRLEN];                            // 
    tchar_t                m_Pathname[_MAX_LPATH];                            // 
    int                    m_IDCount;
    };
    */

};
#endif


/*
struct AvatarNode                                /// 物件清單(single column)
{
    long        id;                            /// id of item、代號、別名...whatever
    long        weight;                        // 
    tchar_t        group[MAX_STRLEN];
    tchar_t        filename[MAX_STRLEN];        /// file name 對映此欄位的物件的檔名
    tchar_t        direction;                    /// sorting info 正向時定義的位置
    tchar_t        layer;                        /// 
    bool        fixed;                        /// 是否會跟著角色旋轉，true表固定不變
    tchar_t        reserved;                    /// 保留，別動它，在排序時有用
    long        visible;                    /// 是否可見，用來在排序時決定        
    long        switcher;

    AvatarNode()
    {
        id            = 0;
        weight        = 0;
        memset(group, 0, MAX_STRLEN);
        memset(filename, 0, MAX_STRLEN);
        direction    = 4;
        layer        = 0;
        fixed        = true;
        reserved    = 0;
        visible        = 0;
        switcher    = 0;
    }
};    // */
