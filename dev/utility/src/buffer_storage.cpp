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
//
//
//
//////////////////////////////////////////////////////////////////////////

//01. == C 系統文件/C++ 系統文件
#if defined(_WINDOWS) || defined(_XBOX) || defined(_LINUX)
#include <string.h>
#endif

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
//#include <time.h>
#include "pf-time.h"
#include "buffer_storage.h"


struct BufferBlock
{
    int      buflen;
    ubyte_t* bufptr;
    int      renewcnt;
    time_t   overlentime;     // 上一次使用超過一半大小的時間
};

#define _MAX_LONG_RENEW_BUFFER_TIME 3600
BufferStorage::BufferStorage()
{
    buffer_init_count_ = 0;
    buffer_min_length_ = 10240;
}

BufferStorage::~BufferStorage()
{
}

void BufferStorage::Create( int count, int minlen )
{
    buffer_init_count_ = count;

    if( minlen > 0 )
    {
        buffer_min_length_ = minlen;
    }
    else
    {
        buffer_min_length_ = 10240;
    }

    //m_BufferDeinfeSize =buflen;
    /*
    for(int i = 0; i<count; i++)
    {
     BufferBlock* bb = new BufferBlock;
     bb->buflen = buffer_min_length_;
     bb->bufptr = new char[buffer_min_length_];
     bb->renewcnt =0;
     if(bb->buflen>buffer_min_length_)
      time(&bb->overlentime);
     else
      bb->overlentime=0;
     buffer_used_.push_back(bb);
     buffer_pool_.push_back(bb);
    }
    */
}

void BufferStorage::Destroy()
{
    TS_Lock lock( &buffer_lock_ );

    for( unsigned int i = 0; i < buffer_pool_.size(); i++ )
    {
        BufferBlock* bb = buffer_pool_[ i ];
        delete[] bb->bufptr;
        delete bb;
    }

    buffer_used_.clear();
    buffer_pool_.clear();
}


ubyte_t* BufferStorage::Borrow( int buflen )
{
    TS_Lock lock( &buffer_lock_ );

    for( unsigned int i = 0; i < buffer_used_.size(); i++ )
    {
        BufferBlock* bb = buffer_used_[ i ];

        if( bb->buflen >= buflen )
        {
            // 要求使用超過目前長度一半的大小 紀錄一下時間
            if( buflen > buffer_min_length_ )
            {
                bb->overlentime = pftml::Time::GetSTime();
                //time( &bb->overlentime );
            }

            memset( bb->bufptr, 0, bb->buflen );
            return bb->bufptr;
        }
    }

    if( ( buffer_init_count_ == 0 ) || ( buffer_pool_.size() < buffer_init_count_ ) )
    {
        // 在預設值為0的狀況下 取不到 就建立新的
        BufferBlock* bb = new BufferBlock;
        bb->buflen = __max( buflen, buffer_min_length_ );
        bb->bufptr = new ubyte_t[ bb->buflen ];
        bb->renewcnt = 0;

        if( bb->buflen > buffer_min_length_ )
        {
            bb->overlentime = pftml::Time::GetSTime();
            //time( &bb->overlentime );
        }
        else
        {
            bb->overlentime = 0;
        }

        buffer_pool_.push_back( bb );
        memset( bb->bufptr, 0, bb->buflen );
        return bb->bufptr;
    }

    if( buffer_used_.size() > 0 )
    {
        // 沒有找到有足夠大小的buffer 因此 拿第一個出來 重新調整大小
        BufferBlock* bb = buffer_used_[ 0 ];
        buffer_used_.erase( buffer_used_.begin() );
        SAFE_DELETE_ARRAY( bb->bufptr );
        bb->buflen = __max( buflen, buffer_min_length_ );
        bb->bufptr = new ubyte_t[ bb->buflen ];
        bb->renewcnt++; // 因為 一開始是0 所以要矲前面
        bb->overlentime = pftml::Time::GetSTime();
        //time( &bb->overlentime );
        memset( bb->bufptr, 0, bb->buflen );
        return bb->bufptr;
    }

    return NULL;
}

void BufferStorage::Return( ubyte_t* bufptr )
{
    if( bufptr == NULL )
    {
        return;
    }

    TS_Lock lock( &buffer_lock_ );

    for( unsigned int i = 0; i < buffer_pool_.size(); i++ )
    {
        BufferBlock* bb = buffer_pool_[ i ];

        if( bb->bufptr == bufptr )
        {
            // 有被重新調整過
            if( bb->buflen > buffer_min_length_ )
            {
                time_t curr_time = pftml::Time::GetSTime();
                //time( &curr_time );

                // 檢查該不該調整 buffer的大小
                // 有時候buffer 過大 或許會浪費記憶體空間
                // 有重新配置過 且目前的大小 超過 平均值
                // 假如已經超過60分鐘沒有要求使用超過基本值大小的
                //
                if( ( curr_time - bb->overlentime ) > _MAX_LONG_RENEW_BUFFER_TIME )
                {
                    // 很久沒有用到這麼大的空間了 就把尺寸減半
                    SAFE_DELETE_ARRAY( bb->bufptr );
                    int freelen = buffer_min_length_;
                    bb->buflen = freelen;
                    bb->bufptr = new ubyte_t[ freelen ];
                    bb->renewcnt = 0;
                    bb->overlentime = 0;
                }
            }

            buffer_used_.push_back( bb );
            return;
        }
    }
}
