//surface.h header file of surface.cpp
//
// surface
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
//                                               Copyright    (C)    1999    Pin
//
#ifndef SCENE2D_H
#define SCENE2D_H

#pragma warning (disable : 4786)

#include <element2d.h>
#include <entity_pool.h>
//#include <tPath.h>

namespace GSO
{
    /**
     \class Scene2d
     \brief 
     轉換EntityPool，讓回傳的指標變成Element。 
     */
    class Scene2d : private EntityPool
    {
    public:
        /**
         * 建構子
         */
        Scene2d();

        /**
         * 解構子
         */
        virtual ~Scene2d();

        /**
         * 取得Element。
         * @param i 索引。
         * @return 如果超過範圍，會傳回NULL，否則傳回找到的Element。
         */
        Element2d*    Element(uint_t i)    { return (i < EntityCount())? ((Element2d *)(EntityPool::operator[])(i)) : 0; }

        /**
         * 取得Element。
         * @param i 索引。
         * @return 如果超過範圍，會傳回NULL，否則傳回找到的Element。
         */
        Element2d*    operator[] (uint_t i){ return Element(i); }

        /**
         * 傳回是否沒有內含物。
         */
        bool        IsElementEmpty(void);

        /**
         * 插入一個Element。
         * @param elem 要插入的Element指標。
         */
        void        InsertElement(Element2d* elem);

        /**
         * 刪除一個Element。
         * @param elem 要插入的Element指標。
         */
        void        RemoveElement(Element2d* elem);

        /**
         * 設定一個Element為focus。
         * @param elm 欲被設為focus的Element。
         */
        void        SetFocusElement(Element2d* elm);

        /**
         * 取得focus的Element。
         * @return 如果有focus的Element則傳回其指標，若無則傳回NULL。
         */
        Element2d*    GetFocusElement(void);

        /**
         * 以位置做為判斷，取得一個Element。
         * @param mapPos 地圖的座標。
         * @return 若有則傳回其指標，否則傳回NULL。
         */
        Element2d*    GetElemBySel(const LG::Point& mapPos);

        /**
         * 清空所有的Element。
         */
        void        ElementClear(void);

        /**
         * 取得目前所有的Element的數量。
         * @return 數量。
         */
        uint_t        ElementCount(void)    { return EntityCount(); }

        /**
         * 傳入一個callback function，對所有物件呼叫一個方法, 但是可以有一個除外(optional)。
         * @param udels 定義的回呼函式。型別為: uint_t (*OnForAllEntity)(unsigned int order, void* pEntity, void* pData)。
         * @param pData 使用者可以傳入自己的參數。
         * @param pExcept 例外的條件。
         */
        void        ForAllElement(OnEntityBuffer udels, void* pData);

        /**
         * 傳回這個元件的地圖矩型。
         * @return 包含這個元件的左上、右下矩型。
         */
        LG::Rect    GetRect(void);

        /**
         * 
         */
        void        Link(const tchar_t* fname);
    };
};//namespace GE
#endif


