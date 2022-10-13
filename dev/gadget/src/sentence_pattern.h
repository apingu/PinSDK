//
//
//   all king of string patten
//   建立常用的字串格式
//
//
//
//
//
//
#pragma once

#include <pf-def.h>
#include <vector>
#include <map>
#include <string>

namespace sentence_pattern
{
    // 'A','DDD','CCC','E'  <== [A, DDD, CCC, E] + delim(')
    // e.g.
    // in  :  @values   A   DAD   CCC   E  4個字串    
    //        @edge     '                  1個前後範圍符號    
    //        @delim    ,                  連結各自串的符號
    // out :           'A','DAD','CCC','E' 1個組合後的字串
    //          
    //_tstring SeriesString01( _IN const _tcsvector& values, _IN const tchar_t* edge = _T("") );

    // 同上
    _tstring SeriesString04( _IN const _tcsvector& values, _IN const tchar_t* edge = _T( "" ), _IN const tchar_t* delim = _T( ", " ) );


    // A = V1, B = V2      <== [A, B, C] + [V1, V2, V3] + delim() 空格要自己留
    // e.g.
    // in  :  @fields   A    B    C    字串陣列
    //        @values   V4   V9   V7   字串陣列
    //        @edge     #              1個前後範圍符號    
    //        @delim    +              連結各自串的符號
    // out : A=#v4#+B=#v9#+C=#v7#
    _tstring SeriesString07( _IN const _tcsvector& fields, _IN const _tcsvector& values, _IN const tchar_t* edge = _T( "" ), _IN const tchar_t* delim = _T( ", " ) );


    // 依照mask 合併字串 只有mask 不為""的才會填入
    // 用在建立SQL查詢字串
    // 傳出 value 去掉 被遮蔽之後
    // mask 用字串的原因是 可以直接 傳入欄位 之類的
    // A, C, D             <==      [1,"",1,1,""] + [ A, B, C, D, E ]
    // e.g.
    // in :   @mask     ST   ""   BB  1    ""   5個遮罩用字串
    //        @values   eat  hit  B   no   ok   5個字串
    //        @edge     [                       1個前後範圍符號
    //        @delim    +                       連結各自串的符號
    // out :  @out_vec  [eat[   [B[    [no[     3個字串
    //        @return01 [eat[+[B[+[no[          1個結合後字串
    _tstring SeriesMaskString01( _OUT _tcsvector& out_vec, _IN const _tcsvector& masks, _IN const _tcsvector& values, _IN const tchar_t* edge = _T( "" ), _IN const tchar_t* delim = _T( ", " ) );


    // 有mask的 寫入 'AA' 沒有的使用B  用在需要完整輸入   'val01', 'val02', field03, fieldl04  原本的資料就不會被蓋過
    // e.g.
    // in :   @mask     ST   ""   BB  1    ""   5個遮罩用字串
    //        @fields   AA   DD   KO  NN   BB   5個字串
    //        @vakyes   eat  hit  B   no   ok   5個字串
    //        @edge     [                       1個前後範圍符號  
    //        @delim    +                       連結各自串的符號
    // out :  @out_vec  [eat[   [B[    [no[     3個字串
    //        @return01 [eat[+DD+[B[+[no[+BB    1個結合後字串
    _tstring SeriesMaskString05( _OUT _tcsvector& out_vec, _IN const _tcsvector& masks, _IN const _tcsvector& fields, _IN const _tcsvector& values, _IN const tchar_t* edge = _T( "" ), _IN const tchar_t* delim = _T( ", " ) );

    // 有值的 清除 但不會拿掉
    // 用在建立SQL查詢字串
    // "", B, "", "", E    <==      [1,"",1,1,""] + [ A, B, C, D, E ]
    // e.g.
    // in :   @mask     ST   ""   BB  1    ""           5個遮罩用字串
    //        @fields   AA   DD   KO  NN   BB           5個字串
    //        @vakyes   eat  hit  B   no   ok           5個字串
    //        @edge     [                               1個前後範圍符號
    //        @delim    &                               連結各自串的符號
    // out :  @out_vec  [eat[  ""   [B[  [no[  ""       5個字串
    //        @return01 [eat[,"",[B[,[no[,""            1個結合後字串
    _tstring SeriesMaskString12( _OUT _tcsvector& out_vec, _IN const _tcsvector& masks, _IN const _tcsvector& values, _IN const tchar_t* edge = _T( "" ), _IN const tchar_t* delim = _T( ", " ) );

    // 清空字串前後的 空格
    const char_t*  ExtraEmpty( _INOUT _mstring& s );
    const wchar_t* ExtraEmpty( _INOUT _wstring& s );

    // 夾在 ' AA,A ' 當成同一個字  字串邊界用 ' or "
    // 分解字串 與是哪種分隔符號 () [] {} 如果有 括號 "" 就當成同一個字串  
    // 01) AAAA, FFFF(BBBB,CCCCC) DDDDD  
    // 02) , ;             <== AAAA,FFFF(BBBB,CCCCC);DDDD  delim = , or ;
    // 比 SplitStringVec 多了 括號 () [] {} 在 括號 裡面的 " " 將會被當成原本的"" 而不視為 一個字串 e.g.  "AAAAA", "DDDDD(ABCD"GGG"EEEEE)JJJJ", "BBBBBB"  => 3區段
    void SplitString01( _OUT _mbsvector& out_vec, _OUT std::vector<char_t>& delim_vec,  _IN const _mstring instr, _IN const char_t* delim = ",;" );
    void SplitString01( _OUT _wcsvector& out_vec, _OUT std::vector<wchar_t>& delim_vec, _IN const _wstring instr, _IN const wchar_t* delim = L",;" );

    // split string by substring
    // 不管 ""  () [] {}
    void SplitString02( _OUT _tcsvector& out_vec, _IN const _tstring instr, _IN const _tstring sub = _T("/*!*/;") );

    // AAA; BBC => 用 (;,) 變成 vector  
    // cash; credit 變成 vector; or ,
    void SplitStringVec( _OUT _tcsvector& out_vec, _OUT std::vector<tchar_t>& delim_vec, _IN const _tstring instr, _IN const tchar_t* delim = _T( ",;" ) );

    // AAA = 1; BBC = 1; DDH = 2  => 用 (;,) 區分 變成 map 
    // cash = 1;後付款 = 1; credit = 2  => 變成 map  ; or ,
    void SplitStringMap( _OUT _tcsmap&  out_map, _OUT std::vector<tchar_t>& delim_vec, _IN const _tstring instr, _IN const tchar_t* delim = _T( ",;" ) );


    // 替換字串
    // e.g.   將字串中 有巨集的地方 實際替換成 代表的實際字串
    //        \dir01\dir02\dir03\$(my_dir)\%dir04\file.txt         $(my_dir) == 45676
    //        \dir01\dir02\dir03\45676\%dir04\file.txt
    _tstring ReplaceString( _IN _tstring instr, _IN const _tcsmap* repstr, _IN _tstring fstr = _T( "$(" ), _IN _tstring estr = _T( ")" ) );
}