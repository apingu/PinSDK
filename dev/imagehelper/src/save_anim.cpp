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

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-string.h>
#include <gadget\file_operator.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "save_anim.h"


void SaveFormatAnim( const char* folder )
{
    /*
    int seq_count   = -1;
    CString nowanimname;
    CString nowanglename;
    //divpath(m_pImageList[0]->filename(), 1, stand_name);

    //std::vector<CString> m_seqname;

    for( UINT i = 0;i < m_pImageList.size();i++ )
    {
        GSO::Anim2d* nowanim = NULL; 
        char nowanimname[_MAX_LPATH];
        char nowanglename[_MAX_LPATH];

        //get anim name        //(LPCTSTR
        const char* filename = m_pImageList[ i ]->filename();
        divpath( filename, 2, nowanimname );
        divpath( filename, 1, nowanglename );

        //step 1: find the anim
        std::map<CString, GSO::Anim2d*>::iterator iter = m_pAnimList.find( nowanimname );

        //沒有這個動作 要建立一個新的
        if( iter == m_pAnimList.end() )
        {
            nowanim = new GSO::Anim2d;
            m_pAnimList.insert( std::map<CString, GSO::Anim2d*>::value_type(nowanimname, nowanim) );
        }
        else
        {
            nowanim = iter->second;
        }

        //step 2: find the angle
        //沒有這個方向 要建立一個新的
        Sequence_t* nowseq=nowanim->GetAnimSeq( atoi(nowanglename) );
        if( nowseq== NULL )
        {
            nowseq = nowanim->AddAnimSeq( atoi(nowanglename) );                
        }
        nowseq->push_back(i);
    }
    */
}