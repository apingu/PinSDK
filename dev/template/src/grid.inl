/* ===========================================================================================
 *
 * grid Class
 *
 *
 *     Map Grid for store map element
 *
 *              2001 2/14   lover day
 * =========================================================================================*/
 //
 //
 //
 //
 //
 //////////////////////////////////////////////////////////////////////////
#pragma once

#pragma pack(push,1)

template <class _Type> class grid
{
private:

    _Type* value_;
    bool   is_empty_;

public:

    grid() { is_empty_ = true; }
    ~grid() {}

    void Store( _Type value )
    {
        *value_ = value;
        is_empty_ = false;
    }

    _Type Pop( _Type value )
    {
        value_ = 0;
        is_empty_ = true;
        return value_;
    }
};
#pragma pack(pop)