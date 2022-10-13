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

//01. == C 系統文件/C++ 系統文件
#include <math.h>
#include <direct.h>
#include <stdio.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-string.h>
#include <gadget\file_operator.h>
#include <parser\pgsp_file.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "doll.h"


//#define PI 3.14159265359

namespace DOLL
{
    AvatarNode::AvatarNode()
    {
        id            = 0;
        weight        = 0;
        //memset(group, 0, MAX_STRLEN);
        //memset(filename, 0, MAX_STRLEN);
        direction    = 4;
        layer        = 0;
        fixed        = true;
        reserved    = 0;
        visible       = 0;
        for(int i = 0; i<16; i++)
        {
            switcher[i]=true;
        }
    }

    AvatarNode::~AvatarNode()
    {

    }


//     class CDollScript : public CPGSPFile
//     {
//         struct GroupData
//         {
//             tchar_t GroupName[MAX_STRLEN];
//         };
// 
//         GroupData * m_Tmp;
// 
//     public:
//         CItemList*      m_pItemList;
//         virtual void*    OnCreateObject    (const tchar_t* Name);
//         virtual void    OnReadAttribute    (const tchar_t* Name, void* obj, tchar_t* Attribute, tchar_t* Value);
//         virtual void    OnCloseObject    (const tchar_t* Name, void* obj);
//     };


    //=============================================================================
    // CDoll
    //=============================================================================
    CDoll::CDoll(void)
        :m_pSort(NULL)//, m_pItemList(NULL)
    {
        Init();
    }

    CDoll::~CDoll(void)
    {
        if (m_pSort)
        {
            delete m_pSort;
            m_pSort = NULL;
        }

        /*

        if (m_pItemList)
        {
            delete m_pItemList;
            m_pItemList = NULL;
        }
        */

    }

    //=============================================================================
    ///  啟始Doll子系統
    ///
    ///  啟始Doll子系統
    ///
    ///     \param 無
    ///  \return 傳回1表示成功
    //=============================================================================
    int CDoll::Init()
    {
        if (m_pSort != NULL)
            delete m_pSort;
        m_pSort = new CSorting;


        /*
        if (m_pItemList != NULL)
            delete m_pItemList;
        m_pItemList = new CItemList;
        */

        //m_pItemList->m_pSort = m_pSort;
        return 1;
    }


    //=============================================================================
    // CAvatarList
    //=============================================================================
    CAvatarList::CAvatarList()
    {
        memset(m_Database, 0, MAX_STRLEN);
        m_Database[0] = '\0';
    }


    CAvatarList::~CAvatarList()
    {
        std::map<longlong_t, AvatarNode*>::iterator iter = m_ItemMap.begin();
        while(iter!=m_ItemMap.end())
        {
            AvatarNode *item = iter->second; 
            SAFE_DELETE(item);
        }

        m_ItemMap.clear();
    }




    //=============================================================================
    // NAME: CItemList::Load
    // DESC: 將清單自檔案中讀出
    //=============================================================================
    int    CAvatarList::Load(const tchar_t* filename)
    {
        // 原來好不容易改好的，結果現在又不能用了，嗚嗚嗚
        //        CDollScript reader;
        //        reader.m_pItemList = this;
        //        return reader.Load(filename);

        // 現在又要新加的
        FileOperator f;
        if(f.open(filename, "rb") == NULL)
            return 0;

        strcpy(m_Database, filename);

        unsigned short size = 0;
        f.seek(0, SEEK_SET);
        f.read(&size,sizeof(unsigned short), 1);
        unsigned short len = 0;
        f.read(&len,sizeof(unsigned short), 1);

        AvatarNode *item = 0;
        for (int i = 0; i < size; ++i)
        {
            item = new AvatarNode;
            f.read(item, sizeof(AvatarNode), 1);

            if(GetItemInfo(item->id) == NULL)
                m_ItemMap.insert(std::map<longlong_t, AvatarNode*>::value_type(item->id, item));
            else
            {
                assert(0||"Double avatar item data");
                SAFE_DELETE(item);
            }
        }

        f.close();
        return 1;
    }

    //=============================================================================
    // NAME: CItemList::GetItemInfo
    // DESC: 取得item的資訊，傳入值是item的id，傳回值是排序資訊
    //=============================================================================
    AvatarNode* CAvatarList::GetItemInfo(longlong_t id)
    {
        if(m_ItemMap.empty())
            return NULL;

        std::map<longlong_t, AvatarNode*>::iterator iter = m_ItemMap.find(id);
        if(iter!=m_ItemMap.end())
            return iter->second;
        return NULL;
    }



    //=============================================================================
    // CItemList
    //=============================================================================
    /*
    CItemList::CItemList(void)
        :m_pTmpList(NULL), m_pItemList(NULL), m_IDCount(0), m_pSort(0)
    {
        _getcwd(m_Pathname, _MAX_LPATH);
        sprintf(m_Filename, "noname.itl");
        memset(m_Database, 0, MAX_STRLEN);
        m_Database[0] = '\0';
        m_ItemArray.clear();
    }

    CItemList::~CItemList(void)
    {
        m_pSort = 0;

        for (int i = 0; i < m_ItemArray.size(); ++i)
        {
            AvatarNode *item = m_ItemArray[i]; 
            delete item;
        }
        m_ItemArray.clear();

        if (m_pItemList != NULL)
            delete m_pItemList;

        if (m_pTmpList != NULL)
            delete m_pTmpList;
    }

    //=============================================================================
    ///  增加一筆item資料到清單中
    ///
    ///  增加一筆item資料到清單中
    ///  \param group 字串，用來找尋欲加入的group，如果沒有，則新增一個group
    ///  \param item 傳入已初始化好的AvatarNode
    ///  \return 傳回1表示成功
    //=============================================================================
    int    CItemList::Add(tchar_t*  group, AvatarNode *item)
    {
        SItemList *tmp = m_pItemList;

        m_ItemArray.push_back(item);

        if (group[0] == '\0')
            return 1;

        while (tmp != NULL)
        {
            if (strcmp(tmp->group, group) == 0)
                break;
            tmp = tmp->next;
        }

        if (tmp == NULL)
        {
            tmp = new SItemList;
            strcpy(tmp->group, group);
            tmp->next = m_pItemList;
            m_pItemList = tmp;
        }

        for (int i = 0; i < tmp->itemlist.size(); ++i)
            if (strcmp(tmp->itemlist[i]->filename, item->filename) == 0)
                return 0;

        strcpy(item->group, group);
        tmp->itemlist.push_back(item);

        return 1;
    }

    //=============================================================================
    // NAME: CItemList::Del
    // DESC: 刪除清單中一筆資料
    //=============================================================================
    int    CItemList::Del(int id)
    {
        if ((id > m_ItemArray.size()) || (id < 0)) 
            return 0;

        delete m_ItemArray[id];
        m_ItemArray[id] = new AvatarNode;
        return 0;
    }

    //=============================================================================
    // NAME: CItemList::Del
    // DESC: 刪除清單中一筆資料
    //=============================================================================
    int    CItemList::Del(const tchar_t* filename, const tchar_t* group)
    {
        if (m_pItemList == NULL)
            return NULL;

        if (group == NULL)
        {
            SItemList *tmp = m_pItemList;

            while (tmp != NULL)
            {
                for (ListType::iterator iter = tmp->itemlist.begin(); iter != tmp->itemlist.end(); ++iter)
                {
                    if (strcmp((*iter)->filename, filename) == 0)
                    {
                        //delete *(iter);
                        tmp->itemlist.erase(iter);
                        return 1;
                    }
                }
                tmp = tmp->next;
            }
        }

        ListType* tmp = Search(group);

        if (tmp == NULL)
            return 0;
        else
        {
            for (ListType::iterator iter = tmp->begin(); iter != tmp->end(); iter++)
                if (strcmp((*iter)->filename, filename) == 0)
                {
                    //delete *(iter);
                    tmp->erase(iter);
                    return 1;
                }
        }        
    
        return 0;
    }

    //=============================================================================
    // NAME: CItemList::Sort
    // DESC: 根據定義的群組做分類，無傳入值，僅單純的做重排的動作
    //=============================================================================
    int    CItemList::Sort()
    {
        // TODO: 這裡可以開始編寫程式了
        return 0;
    }

    //=============================================================================
    // NAME: CItemList::Save
    // DESC: 將清單存入檔案中
    //=============================================================================
    int    CItemList::Save(tchar_t* filename)
    {
        if (m_pItemList == NULL)
            return 0;
        {
            FileOperator f;
            if(f.open(filename, "wb") == NULL)
                return 0;

            unsigned short tt = m_ItemArray.size();
            f.seek(0, SEEK_SET);
            f.write(&tt, 2, 1);
            tt = sizeof(AvatarNode);
            f.seek(2, SEEK_SET);
            f.write(&tt, 2, 1);

            unsigned int i = 0;
            for (i = 0; i < m_ItemArray.size(); ++i)
            {
                m_ItemArray[i]->weight = 0;
                m_ItemArray[i]->id = i;
                f.seek(sizeof(AvatarNode) * i + 4, SEEK_SET);
                f.write(m_ItemArray[i], sizeof(AvatarNode), 1);
            }

            AvatarNode item;
            f.seek(sizeof(AvatarNode) * i + 4, SEEK_SET);
            f.write(&item, sizeof(AvatarNode), 1);

            f.close();
        }

        return 1;
    }

    //=============================================================================
    // NAME: CItemList::Load
    // DESC: 將清單自檔案中讀出
    //=============================================================================
    int    CItemList::Load(const tchar_t* filename)
    {
        // 原來好不容易改好的，結果現在又不能用了，嗚嗚嗚
//        CDollScript reader;
//        reader.m_pItemList = this;
//        return reader.Load(filename);

        // 現在又要新加的
        FileOperator f;
        if(f.open(filename, "rb") == NULL)
            return 0;

        unsigned short size = 0;
        f.seek(0, SEEK_SET);
        f.read(&size,sizeof(unsigned short), 1);
        unsigned short len = 0;
        f.read(&len,sizeof(unsigned short), 1);

        AvatarNode *item = 0;
        for (int i = 0; i < size; ++i)
        {
            item = new AvatarNode;
            //f.seek(sizeof(AvatarNode) * i + 4, SEEK_SET);
            f.read(item, sizeof(AvatarNode), 1);
            strtoupper(item->filename);
            Add(item->group, item);
        }

        f.close();
        return 1;
    }

    //=============================================================================
    // NAME: CItemList::SetPathname
    // DESC: 設定路徑
    //=============================================================================
    int    CItemList::SetIDCount(int count)
    {
        m_IDCount = count;
        return 1;
    }

    //=============================================================================
    // NAME: CItemList::SetPathname
    // DESC: 設定路徑
    //=============================================================================
    int    CItemList::GetIDCount()
    {
        return m_IDCount;
    }

    //=============================================================================
    // NAME: CItemList::SetPathname
    // DESC: 設定路徑
    //=============================================================================
    int CItemList::SetPathname(tchar_t*  pathname)
    {
        strcpy(m_Pathname, pathname);
        return 1;
    }

    //=============================================================================
    // NAME: CItemList::SetFilename
    // DESC: 設定檔名
    //=============================================================================
    int CItemList::SetFilename(tchar_t*  filename)
    {
        strcpy(m_Filename, filename);
        return 1;
    }

    //=============================================================================
    // NAME: CItemList::GetPathname
    // DESC: 取得路徑
    //=============================================================================
    int CItemList::GetPathname(tchar_t*  buf)
    {
        strcpy(buf, m_Pathname);
        return 1;
    }

    //=============================================================================
    // NAME: CItemList::GetFilename
    // DESC: 取得檔名
    //=============================================================================
    int CItemList::GetFilename(tchar_t*  buf)
    {
        strcpy(buf, m_Filename);
        return 1;
    }

    //=============================================================================
    // NAME: CItemList::Search
    // DESC: 搜尋，取出群組的清單，傳入值是群組名稱，傳回值是清單
    //             如果傳入null，則傳回全部的清單<--未實作
    //=============================================================================
    ListType *CItemList::Search(const tchar_t* group)
    {
        if (m_pItemList == NULL)
            return NULL;

        if (group == NULL)
            return NULL;

        SItemList *tmp = m_pItemList;

        while (tmp != NULL)
        {
            if (strcmp(tmp->group, group) == 0)
                return &(tmp->itemlist);
            tmp = tmp->next;
        }

        return NULL;
    }

    //=============================================================================
    // NAME: CItemList::GetItemInfo
    // DESC: 取得item的資訊，傳入值是item的id，傳回值是排序資訊
    //=============================================================================
    AvatarNode *CItemList::GetItemInfo(int id)
    {
        if ((id > m_ItemArray.size()) || (id < 0)) 
            return NULL;

        AvatarNode *tt = m_ItemArray[id];
        return tt;
    }

    //=============================================================================
    // NAME: CItemList::GetItemInfo
    // DESC: 取得item的資訊，傳入值是item的group及filename，傳回值是排序資訊
    //=============================================================================
    AvatarNode *CItemList::GetItemInfo(const tchar_t* filename, const tchar_t* group)
    {
        if (m_pItemList == NULL)
            return NULL;

        if (group == NULL)
        {
            SItemList *tmp = m_pItemList;

            while(tmp != NULL)
            {
                AvatarNode *item = tmp->GetItem(filename);
                if (item != NULL)
                    return item;
                tmp = tmp->next;
            }
        }

        ListType* tmp = Search(group);

        if (tmp == NULL)
            return NULL;
        else
        {
            for (ListType::iterator iter = tmp->begin(); iter != tmp->end(); iter++)
                if (strcmp((*iter)->filename, filename) == 0)
                    return (*iter);
        }        
    
        return NULL;
    }

    //=============================================================================
    // NAME: CItemList::GetNextGroup
    // DESC: 傳入值是group，傳回值是下一個group name，如果傳回NULL表清單結束
    //=============================================================================
    tchar_t* CItemList::GetNextGroup(const tchar_t* group) const
    {
        if (m_pItemList == NULL)
            return NULL;

        if (group == NULL)
            return m_pItemList->group;

        SItemList *tmp = m_pItemList;

        while (tmp != NULL)
        {
            if (strcmp(tmp->group, group) == 0)
            {
                if (tmp->next != NULL)
                    return tmp->next->group;
                else
                    return NULL;
            }
            tmp = tmp->next;
        }
    
        return NULL;
    }

    //=============================================================================
    // NAME: CItemList::GetGroup
    // DESC: 傳入值是group，傳回值是該group的指標，如果傳回NULL表沒有這東西
    //=============================================================================
    SItemList *CItemList::GetGroup(const tchar_t* group)
    {
        if (m_pItemList == NULL)
            return NULL;

        if (group == NULL)
            return m_pItemList;

        SItemList *tmp = m_pItemList;

        while (tmp != NULL)
        {
            if (strcmp(tmp->group, group) == 0)
                    return tmp;
            tmp = tmp->next;
        }
    
        return NULL;
    }
    */

    //=============================================================================
    // CSorting
    //=============================================================================
    CSorting::CSorting(void)
        :m_NowDirection(0)
    {
        Init();
    }

    CSorting::~CSorting(void)
    {
    }

    //=============================================================================
    // NAME: CSorting::Init
    // DESC: 啟始排序系統，主要是啟始排序表格
    //=============================================================================
    int CSorting::Init()
    {
        // m_SortInfo[layer][direction]
        // m_SortInfo[16][8];

        // 原始想法 還是 不管在任何角度 都是 身->頭......
        // 可以改變的想法是因為 已3D的想法
        int base=0;
        for (int layer = 0; layer < LAYERS; layer++)
        {
            for (int r = 0; r < DIRECTION; r++)
            {
                m_SortInfo[layer][r]=base;
            }
            base+=100;
        }

        //m_SortInfo[2][2] = -100;  //  -> 時左手在最下面
        //m_SortInfo[3][6] = -100;  //  <- 時右手在最下面


//         for (int i = 0; i < 8; ++i)
//             m_SortInfo[0][i] = 0;        
//         for (int i = 1; i < 16; ++i)
//         {
//             m_SortInfo[i][0] = m_SortInfo[i][1] = m_SortInfo[i][7] = i * (-100);
//             m_SortInfo[i][2] = m_SortInfo[i][3] = m_SortInfo[i][4] = m_SortInfo[i][5] =
//             m_SortInfo[i][6] = i * 100;
//         }

//        int shift = 6;
//        for (int layer = 0; layer < LAYERS; layer++)
//            for (int r = 0 + shift; r < DIRECTION + shift; r++)
//                m_SortInfo[layer][r - shift] = (int)(sin(PI * .25 * (r % DIRECTION)) * layer * 100/* 一個常數 */);

        return 1;
    }

    //=============================================================================
    // NAME: CSorting::GetSortInfo
    // DESC: 
    //=============================================================================
    int CSorting::GetSortInfo(int layer, int direct)
    {
        return m_SortInfo[layer][direct];
    }

    //=============================================================================
    // NAME: CSorting::SetSortInfo
    // DESC: 
    //=============================================================================
    void CSorting::SetSortInfo(int layer, int direct, int weight)
    {
        m_SortInfo[layer][direct] = weight;
    }

    int    CSorting::Save(const tchar_t* filename)
    {
        FileOperator pf;
        if (pf.open(filename, "w+b") == 0)
            return 0;
        short len0=4;
        short len1=16;
        pf.write(&len0, sizeof(short), 1);
        pf.write(&len1, sizeof(short), 1);
        pf.write((void *)m_SortInfo, LAYERS * DIRECTION * 4, 1);
        pf.close();
        return 1;
    }

    int    CSorting::Load(const tchar_t* filename)
    {
        FileOperator pf;
        if (pf.open(filename, "r+b") == 0)
            return 0;
        pf.seek(4, SEEK_SET);
        pf.read((void *)m_SortInfo, LAYERS * DIRECTION * 4, 1);
        pf.close();
        return 1;
    }

    //=============================================================================
    // NAME: CSorting::BeginSort
    // DESC: 把之前的資料清空，丟入方向，並準備要開始丟入item
    //=============================================================================
    void CSorting::BeginSort(int direct)
    {
        m_NowDirection = direct;        // 旋轉的方向
        m_Stack.clear();
        return;
    }

    //=============================================================================
    // NAME: CSorting::Item
    // DESC: 丟入待排序的item
    //=============================================================================
    void CSorting::Item(AvatarNode *item)
    {
        if (item == 0)
            return;

        GetWeight(item);
        m_Stack.push_back(item);
        return;
    }

    //=============================================================================
    // NAME: CSorting::EndSort
    // DESC: 結束排序並傳回排序好的順序
    //=============================================================================
    ListType* CSorting::EndSort()
    {
        AvatarNode *tmp;
        int size = (int)m_Stack.size(), flag;

        int i = 0;
        for (int i = 0; i < size; i++)
        {
            flag = 0;
            for (int j = 0; j < (size - i - 1); j++)
            {
                if (m_Stack[j]->weight > m_Stack[j + 1]->weight)
                {
                    flag = 1;
                    tmp = m_Stack[j];
                    m_Stack[j] = m_Stack[j + 1];
                    m_Stack[j + 1] = tmp;
                }
            }
            if (flag != 1)
                break;
        }
        
        for (i = 0; i < size; i++)
        {
            m_Stack[i]->reserved = i;
        }

        return &m_Stack;
    }

    //=============================================================================
    // NAME: CSorting::GetMapping
    // DESC: 取得排序資訊表格裡的權重, 後來的方向 = (原定的方向 + 旋轉的方向) % 8
    //             layer不會改變       
    //=============================================================================
    void CSorting::GetWeight(AvatarNode* item)
    {
        if (item == 0)
            return;

        item->weight = m_SortInfo[item->layer][m_NowDirection];
        
//         if (item->fixed)
//             item->weight = m_SortInfo[item->layer][item->direction];
//         else
//             item->weight = m_SortInfo[item->layer][(item->direction + m_NowDirection) % 8];

        return;
    }

//     //=============================================================================
//     //    CDollScript
//     //=============================================================================
//     void*    CDollScript::OnCreateObject(const tchar_t* Name)
//     {
//         if (strcmp(Name, "ITEMLIST") == 0)
//         {
//             return m_pItemList;
//         }
//         else if (strcmp(Name, "GROUP") == 0)
//         {
//             GroupData* groupdata = new GroupData;
//             return groupdata;
//         }
//         else if (strcmp(Name, "ITEM") == 0)
//         {
//             AvatarNode *item = new AvatarNode;
//             return item;
//         }
// 
//         return 0;
//     }
// 
//     void    CDollScript::OnReadAttribute(const tchar_t* Name, void* obj, tchar_t* Attribute, tchar_t* Value)
//     {
//         if (strcmp(Name, "ITEMLIST") == 0)
//         {
//             if (strcmp(Attribute, "IDCOUNT") == 0)
//                 m_pItemList->SetIDCount(atoi(strtok(Value, ",")));
//             else if (strcmp(Attribute, "FILENAME") == 0)
//                 m_pItemList->SetFilename(Value);
//         }
//         else if (strcmp(Name, "GROUP") == 0)
//         {
//             GroupData* groupdata = (GroupData*)obj;
//             if (strcmp(Attribute, "GROUPNAME") == 0)
//                 strcpy(groupdata->GroupName, Value);
//             m_Tmp = groupdata;
//         }
//         else if (strcmp(Name, "ITEM") == 0)
//         {
//             AvatarNode *item = (AvatarNode *)obj;
//             if (strcmp(Attribute, "FILENAME") == 0)
//                 strcpy(item->filename, Value);
//             if (strcmp(Attribute, "ID") == 0)
//             {
//                 item->id = atoi(strtok(Value,","));
//                 item->direction = atoi(strtok(NULL, ","));
//                 item->fixed = atoi(strtok(NULL, ","));
//                 item->weight = (atoi(strtok(NULL, ",")) > 0?true:false);
//                 item->layer = atoi(strtok(NULL, ","));
//             }
//         }
//     }
// 
//     void    CDollScript::OnCloseObject(const tchar_t* Name, void* obj)
//     {
//         if (strcmp(Name, "GROUP") == 0)
//         {
// //            strcpy(m_pItemList->GetGroup(m_Tmp->GroupName)->folder, m_Tmp->Folder);
//             delete m_Tmp;
//             m_Tmp = 0;
//         }
//         else if (strcmp(Name, "ITEM") == 0)
//             m_pItemList->Add(m_Tmp->GroupName, (AvatarNode *)obj);
//     }

};// namespace DOLL