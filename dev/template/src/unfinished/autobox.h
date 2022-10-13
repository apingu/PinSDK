#ifndef AUTOBOX_H_
#define AUTOBOX_H_

#include <memory>
#include <math.h>
#include <assert.h>


namespace PC
{
    template <class T>
    class AutoBox
    {
        typedef struct tagNODE
        {
            T       Value;
        }NODE;


        NODE*   ListPtr;

        NODE* NewNode( void )
        {
            NODE*   newNode         = new NODE;
            return newNode;
        }

    public:

        AutoBox()
        {
            ListPtr = 0;
        }
        ~AutoBox()
        {
            clear();
        }

        bool empty( void )
        {
            return ( ListPtr == 0 ) ? true : false;
        }

        void clear( void )
        {
            if( empty() )
                return ;
            delete ListPtr;
            ListPtr = 0;
            return;
        }


        void push( T Value )
        {
            clear();
            NODE*   pNewNode        = NewNode();
            pNewNode->Value = Value;
            ListPtr = pNewNode;
            //GoTail();
        }


        T operator()( void )
        {
            return ListPtr->Value;
        }//  catch(...){return NULL;}} //ok
    };
    // End RLIST
    /////////////////////////////////////////////////////////////////////////////////////////////////
}
#endif
