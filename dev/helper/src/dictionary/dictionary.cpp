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
#include <set>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-string.h>
#include <Unicode.h>
#include <pf-debug.h>
#include <parser\xmlschema.h>
#include <parser\html-parser.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "dictionary.h"
#include <office/excel_xml.h>
#include "excite_translater.h"
#include "web_bing_client.h"
#include "bing_translate.h"

namespace helper
{
    char_t   NumCH = '0';
    // 作為斷字依據的字元
    UInt16 s_SeparationWordTable[] =
    {
        0, /*' ',*/ ',', ';', ':', '(', ')', '.', '{', '}', '[', ']', '\r', '\n',
        /*0X3000,// '  ',*/
        0X3001,// '、',
        0X3002,// '。',
        0X3010,// '【',
        0X3011,// '】',
        0x30fb,// '・',
        /*0XFF00,// '  ',*/
        0XFF07,// '、',
        0XFF08,// '（',
        0XFF09,// '）',
        0XFF0C,// '，',
        0XFF0E,// '．',
        0XFF1A,// '：',
        0XFF1B,// '；',
        0XFF3B,// '〔',
        0XFF3D,// '〕',
        0XFF5B,// '｛',
        0XFF5D // '｝',
    };
    std::set<UInt16>     SeparationTable;

    // 句子中要忽略的字元
#define _MAX_ESCAPESTRING 6
    UInt16 s_EscapeWordTable[ _MAX_ESCAPESTRING ][ 10 ] =
    {
        {0X005C, 0X006E, 0X0000},     // \n
        {0X000D, 0X0000},             // \r
        {0X000A, 0X0000},             // \n
        {0X2000, 0X0000},             // ' ',
        {0X3000, 0X0000},             // '  ',
        {0XFF00, 0X0000}              // '  ',
    };

    // 前後可忽略的字元
    UInt16 s_ExtractWordTable[] =
    {
        0, ' ', '\r', '\n', 0X3000, 0XFF00
    };

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    Dictionary::Dictionary() //: m_SkipChar(" \r\n")
    {
    }


    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    Dictionary::~Dictionary()
    {
    }

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    _mstring GetDataNode( XMLNode* cell )
    {
        _mstring datacontent;
        XMLNode* data = cell->FindFirstNode( "Data" );

        if( data != NULL )
        {
            datacontent = data->text_;
        }
        else
        {
            data = cell->FindFirstNode( "ss:Data" );

            if( data != NULL )
            {
                XMLNode* font = data->FindFirstNode( "Font" );

                while( font )
                {
                    datacontent = datacontent + font->text_;
                    font = data->FindNextNode( "Font" );
                }
            }
        }

        return html::Parser::Decode( datacontent.c_str() );
    }

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    void Dictionary::SetSkipChar( const char_t* ins )
    {
        //m_SkipChar = ins;
    }

    /*
    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    void Dictionary::Append(const char_t* xmlfile)
    {
        XMLNode rootxml;
        if(rootxml.LoadFromFile(xmlfile))
        {
            XMLNode* workbook = rootxml.FindNode("Workbook");
            if(workbook)
            {
                XMLNode* worksheet = workbook->FindNode("Worksheet");
                if(worksheet)
                {
                    XMLNode* table = worksheet->FindNode("Table");
                    if(table)
                    {
                        XMLNode* row = table->FindFirstNode("Row");
                        while(row)
                        {
                            XMLNode* cell00 = row->FindFirstNode("Cell");
                            XMLNode* cell01 = row->FindNextNode("Cell");
                            if((cell00 !=NULL ) && (cell01 !=NULL))
                            {
                                _mstring data00 = GetDataNode(cell00);
                                _mstring data01 = GetDataNode(cell01);

                                AddString(data00.c_str(), data01.c_str());
                            }
                            row = table->FindNextNode("Row");
                        }
                    }
                }
            }
        }
    }
    */
    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    void Dictionary::AddSeparator( const char_t* ins )
    {
        size_t buflen = ( strlen( ins ) + 1 ) * 4;
        UInt16* szUnicodeA = new UInt16[ buflen ];
        UTF8ToUnicode( szUnicodeA, ins, buflen );
        size_t unilen = wcslen( ( wchar_t* )szUnicodeA );

        for( int i = 0; i > unilen; i++ )
        {
            SeparationTable.insert( std::set<UInt16>::value_type( szUnicodeA[ i ] ) );
        }

        SAFE_DELETE_ARRAY( szUnicodeA );
    }

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    bool IsSeparationWord( UInt16 szUnicode )
    {
        //if( IsSymbol(szUnicode) == 0)
        //  return false;
        if( SeparationTable.empty() )
        {
            int ll = sizeof( s_SeparationWordTable ) / sizeof( UInt16 );

            for( int i = 0; i < ll; i++ )
            {
                SeparationTable.insert( std::set<UInt16>::value_type( s_SeparationWordTable[ i ] ) );
            }
        }

        std::set<UInt16>::iterator iter = SeparationTable.find( szUnicode );

        if( iter != SeparationTable.end() )
        {
            return true;
        }

        return false;
    }

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    bool IsExtractWord( UInt16 szUnicode )
    {
        int ll = sizeof( s_ExtractWordTable ) / sizeof( UInt16 );

        for( int i = 0; i < ll; i++ )
        {
            if( szUnicode == s_ExtractWordTable[ i ] )
            {
                return 1;
            }
        }

        return 0;
    }

    //=====================================================================================
    // NAME:
    // DESC: 用在輸入字串用 只能砍頭砍尾 否則會比對不回去
    //=====================================================================================
    // 去頭去尾
    void ExtractString( _INOUT UInt16* szUnicodeA )
    {
        int outlen = wcslen( ( wchar_t* )szUnicodeA ) + 1;
        UInt16* szUnicodeB = new UInt16[ outlen ];
        wcscpy( ( wchar_t* )szUnicodeB, ( wchar_t* )szUnicodeA );
        //memcpy(szUnicodeB, szUnicodeA, outlen * sizeof(UInt16));
        int i = 0;
        int be = 1;
        int bi = 0;

        for( i = 0; i < outlen; i++ )
        {
            if( be == 1 )
            {
                // 數字符號不可以去掉
                //if( (IsSymbol(szUnicodeB[i]) == 1) &&
                //  (szUnicodeB[i] != NumCH) )
                if( IsExtractWord( szUnicodeB[ i ] ) )
                {
                    continue;
                }

                be = 0;
            }

            szUnicodeA[ bi ] = szUnicodeB[ i ];
            bi++;
        }

        if( bi < outlen )
        {
            outlen = wcslen( ( wchar_t* )szUnicodeA );
            bi = ( outlen - 1 );

            for( int i = ( outlen - 1 ); i >= 0; i-- )
            {
                // 數字符號不可以去掉
                //if( (IsSymbol(szUnicodeB[i]) == 1) &&
                //  (szUnicodeB[i] != NumCH) )
                if( IsExtractWord( szUnicodeA[ i ] ) )
                {
                    szUnicodeA[ bi ] = 0;
                    bi--;
                    continue;
                }

                break;
            }
        }

        if( wcslen( ( wchar_t* )szUnicodeA ) == 0 )
        {
            wcscpy( ( wchar_t* )szUnicodeA, ( wchar_t* )szUnicodeB );
        }

        SAFE_DELETE_ARRAY( szUnicodeB );
    }

    void ExtractString( _INOUT char_t* szUTF8 )
    {
        int buflen = ( strlen( szUTF8 ) + 1 ) * 4;
        UInt16* szUnicodeA = new UInt16[ buflen + 1 ];
        UTF8ToUnicode( szUnicodeA, szUTF8, buflen );
        helper::ExtractString( szUnicodeA );
        UnicodeToUTF8( szUTF8, szUnicodeA, buflen );
        SAFE_DELETE_ARRAY( szUnicodeA );
    }

    //=====================================================================================
    // NAME:
    // DESC: 如果字串中有換行符號 \n 字串 讓他也可以對的到
    //       把符號都去掉 增加 比對的命中率
    //=====================================================================================
    int SimplifyString( _INOUT UInt16* szUnicodeA )
    {
        int buflen = wcslen( ( wchar_t* )szUnicodeA ) + 1;
        UInt16* srcUnicode = new UInt16[ buflen ];
        wcscpy( ( wchar_t* )srcUnicode, ( wchar_t* )szUnicodeA );
        UInt16* szUnicodeB = new UInt16[ buflen ];
        int j = 0;

        for( int i = 0; i < buflen; i++ )
        {
            if( !IsSeparationWord( szUnicodeA[ i ] ) )
            {
                szUnicodeB[ j ] = szUnicodeA[ i ];
                j++;
            }
        }

        szUnicodeB[ j ] = 0;
        UInt16* instr = szUnicodeB;
        UInt16* outstr = szUnicodeA;

        for( int i = 0; i < _MAX_ESCAPESTRING; i++ )
        {
            wcsdumpallstr( ( wchar_t* )outstr, buflen, ( wchar_t* )instr, ( wchar_t* )s_EscapeWordTable[ i ] );
            SWAPUS( instr, outstr );
        }

        if( outstr != szUnicodeA )
        {
            wcscpy( ( wchar_t* )szUnicodeA, ( wchar_t* )outstr );
        }

        // 整個字串都被砍掉了，有可能 是一個符號
        int retlen = wcslen( ( wchar_t* )szUnicodeA );

        if( retlen == 0 )
        {
            wcscpy( ( wchar_t* )szUnicodeA, ( wchar_t* )srcUnicode );
        }

        SAFE_DELETE_ARRAY( srcUnicode );
        SAFE_DELETE_ARRAY( szUnicodeB );
        int idx = retlen;
        return idx;
    }

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    int SimplifyString( _INOUT char_t* szUTF8 )
    {
        int buflen = ( strlen( szUTF8 ) + 1 ) * 4;
        UInt16* szUnicodeA = new UInt16[ buflen + 1 ];
        UTF8ToUnicode( szUnicodeA, szUTF8, buflen );
        int r = helper::SimplifyString( szUnicodeA );
        UnicodeToUTF8( szUTF8, szUnicodeA, buflen );
        SAFE_DELETE_ARRAY( szUnicodeA );
        return r;
    }

    /*
    void CutDownStringB(_INOUT char_t* ins1, _OUT std::vector<_mstring>& nums)
    {
        int buflen = strlen(ins1) + 1;
        UInt16* szUnicodeA = new UInt16[buflen];
        UTF8ToUnicode(szUnicodeA, ins1, buflen);
        int unilen = wcslen(szUnicodeA);

        //  AABB10234%LLLL  => AABB#%LLLL

        int numi = 0;
        UInt16* numA = new UInt16[buflen];
        char_t*   numB = new char_t[buflen];

        bool bnum = false;
        UInt16* szUnicodeB = new UInt16[buflen];
        int j = 0;
        for(int i = 0; i < unilen; i++)
        {
            // 所有並排數字都縮成一個符號
            if(IsNumber(szUnicodeA[i]) == 1)
            {
                numA[numi] = szUnicodeA[i];
                numi++;
                // 第一個數字字元
                if(bnum == false)
                {
                    szUnicodeB[j] = '#';
                    j++;
                    bnum = true;
                }
            }
            else
            {
                numA[numi] = 0;
                numi = 0;
                // 最後一個數字字元
                if(bnum == true)
                {
                    UnicodeToUTF8(numB, numA , buflen);
                    nums.push_back(numB);
                    bnum = false;
                }
            }

            if( (IsSymbol(szUnicodeA[i]) == 0) ||
                (IsNumberSign(szUnicodeA[i]) == 1) )
            {
                szUnicodeB[j] = szUnicodeA[i];
                j++;
                continue;
            }
        }

        szUnicodeB[j] = 0;

        UnicodeToUTF8(ins1, szUnicodeB , buflen);

        SAFE_DELETE_ARRAY(numA);
        SAFE_DELETE_ARRAY(numB);
        SAFE_DELETE_ARRAY(szUnicodeA);
        SAFE_DELETE_ARRAY(szUnicodeB);


        //FILE* f = fopen("testA.txt", "ab");
        //fwrite(ins1, strlen(ins1), 1, f);
        //char_t buff[128] = {"\r\n"};
        //fwrite(buff, strlen(buff), 1, f);
        //fclose(f);
    }
    */
    struct SplitWord
    {
        _mstring               before_text;  // 原本的字
        _mstring               after_text;  // 換成 # 的字
        std::vector<_mstring>  numbs;
        int                       strn;   //字長度
    };

    // 第一個是轉換過的字  第二個是原始的字
    void SplitString( _OUT std::vector<SplitWord>& fwords, _IN const char_t* ins1 )
    {
        int buflen = ( strlen( ins1 ) + 1 ) * 4;
        UInt16* szUnicodeA = new UInt16[ buflen ];
        UTF8ToUnicode( szUnicodeA, ins1, buflen );
        int unilen = wcslen( ( wchar_t* )szUnicodeA ) + 1;
        UInt16* numA = new UInt16[ unilen ];
        char_t*   numB = new char_t[ unilen * 4 ];  //utf8
        UInt16* szUnicodeB = new UInt16[ buflen ];
        char_t*   utf8buff = new char_t[ buflen * 4 ];
        UInt16* szUnicodeC = new UInt16[ buflen ];
        int begin = 0;

        for( int i = 0; i < unilen; i++ )
        {
            if( IsSeparationWord( szUnicodeA[ i ] ) )
            {
                int ll = ( i - begin );

                if( ll > 0 )
                {
                    // 為了檢查這個字是不是全部都是數字或符號
                    int symbolcnt = 0;
                    {
                        // 字的開頭
                        UInt16*   beUnicodeA = szUnicodeA + begin;

                        for( int ui = 0; ui < ll; ui++ )
                        {
                            szUnicodeC[ ui ] = beUnicodeA[ ui ];

                            // 為了檢查這個字是不是全部都是數字或符號
                            if( IsSymbol( szUnicodeC[ ui ] ) == 1 )
                            {
                                symbolcnt++;
                            }
                        }

                        szUnicodeC[ ll ] = 0;
                        ExtractString( szUnicodeC ); // 去頭去尾
                        //SimplifyString(szUnicodeC);  // 整能去頭去尾 裡面的\n不能去掉 否則，在compose的時候 找原始字串十 會找不到該字
                    }
                    SplitWord sw;
                    // 將A傳入B並將數字過濾掉
                    //  AABB10234%LLLL  => AABB#%LLLL
                    int   uj = 0;
                    int   numi = 0;
                    bool  bnum = false;

                    for( int ui = 0; ui < ll; ui++ )
                    {
                        // 所有並排數字都縮成一個符號
                        // 半形數字
                        // 全形數字
                        if( ( IsNumber( szUnicodeC[ ui ] ) == 1 ) ||
                            ( IsFullWidthNumber( szUnicodeC[ ui ] ) == 1 ) )
                        {
                            numA[ numi ] = szUnicodeC[ ui ];
                            numi++;

                            // 第一個數字字元
                            if( bnum == false )
                            {
                                szUnicodeB[ uj ] = NumCH;
                                uj++;
                                bnum = true;
                            }
                        }
                        else
                        {
                            numA[ numi ] = 0;
                            numi = 0;

                            // 最後一個數字字元
                            if( bnum == true )
                            {
                                UnicodeToUTF8( numB, numA, buflen );
                                sw.numbs.push_back( numB );
                                bnum = false;
                            }

                            szUnicodeB[ uj ] = szUnicodeC[ ui ];
                            uj++;
                        }
                    }

                    //如果B全都是數字或符號 就跳過
                    if( symbolcnt == ll )
                    {
                    }
                    //當成一個字典存入
                    else
                    {
                        //memcpy(szUnicodeB, szUnicodeA + begin, ll * symbolcntzeof(UInt16));
                        szUnicodeB[ uj ] = 0;
                        UnicodeToUTF8( utf8buff, szUnicodeB, buflen );
                        sw.after_text = utf8buff;
                        //{
                        //  FILE* f = fopen("testA4.txt", "ab");
                        //  char_t buffB[32] = {"    ==>    "};
                        //  fwrite(utf8buff, strlen(utf8buff), 1, f);
                        //  fwrite(buffB, strlen(buffB), 1, f);
                        //  fclose(f);
                        //}
                        szUnicodeC[ ll ] = 0;
                        UnicodeToUTF8( utf8buff, szUnicodeC, buflen );
                        sw.before_text = utf8buff;
                        sw.strn = ll;
                        //{
                        //  FILE* f = fopen("testA4.txt", "ab");
                        //  char_t buffB[32] = {"\r\n"};
                        //  fwrite(utf8buff, strlen(utf8buff), 1, f);
                        //  fwrite(buffB, strlen(buffB), 1, f);
                        //  fclose(f);
                        //}
                        fwords.push_back( sw );
                    }

                    uj = 0;         // 重設B的buffer
                }

                begin = i + 1;  // 下一個字
            }
        }

        SAFE_DELETE_ARRAY( utf8buff );
        SAFE_DELETE_ARRAY( szUnicodeB );
        SAFE_DELETE_ARRAY( numA );
        SAFE_DELETE_ARRAY( numB );
        SAFE_DELETE_ARRAY( szUnicodeA );
        SAFE_DELETE_ARRAY( szUnicodeC );
    }

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    void MakeWord( _OUT _mstring& word, _IN const char_t* ins, _IN std::vector<_mstring> nums )
    {
        if( nums.size() == 0 )
        {
            word = ins;
            return;
        }

        int buflenA = ( strlen( ins ) + 1 ) * 4;
        UInt16* szUnicodeA = new UInt16[ buflenA ];
        int buflenB = ( strlen( ins ) + 1 ) * 4;

        for( int i = 0; i < nums.size(); i++ )
        {
            buflenB += nums[ i ].size();
        }

        buflenB++;
        UInt16* szUnicodeB = new UInt16[ buflenB ];
        UTF8ToUnicode( szUnicodeA, ins, buflenA * sizeof( UInt16 ) );
        int unilen = wcslen( ( wchar_t* )szUnicodeA );
        int ub = 0;
        int ni = 0;   // 數字的順序

        for( int ui = 0; ui < unilen; ui++ )
        {
            if( szUnicodeA[ ui ] == NumCH )
            {
                for( unsigned int nn = 0; nn < nums[ ni ].size(); nn++ )
                {
                    szUnicodeB[ ub ] = nums[ ni ][ nn ];
                    ub++;
                }

                ni++;
            }
            else
            {
                szUnicodeB[ ub ] = szUnicodeA[ ui ];
                ub++;
            }
        }

        szUnicodeB[ ub ] = 0;
        int b2 = ( ub * 4 ) + 2;
        char_t* utf8buff = new char_t[ b2 ];
        UnicodeToUTF8( utf8buff, szUnicodeB, b2 );
        word = utf8buff;
        SAFE_DELETE_ARRAY( utf8buff );
        SAFE_DELETE_ARRAY( szUnicodeA );
        SAFE_DELETE_ARRAY( szUnicodeB );
    }


    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    /*
    void ExtractString(_INOUT char_t* ins1, _IN const char_t* espstr)
    {
        //char_t* escbuff = new char_t[strlen(espstr) + 32];
        //strcpy(escbuff, "\r\n");
        //strcat(escbuff, espstr);
        strextractstring(ins1, espstr);
        //SAFE_DELETE_ARRAY(escbuff);
    }
    */

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    void Dictionary::AddWord( const char_t* ins1, const char_t* ins2 )
    {
        if( strcmp( ins1, "" ) == 0 )
        {
            return;
        }

        if( strcmp( ins2, "" ) == 0 )
        {
            return;
        }

        //int ss = m_SkipChar.size();
        char_t* inouts00 = new char_t[ ( strlen( ins1 ) + 1 ) * 4 ];
        strcpy( inouts00, ins1 );
        // 先將字串中不需要的字(符號)去除
        //strextractstring(inouts00, m_SkipChar.c_str());
        //CutDownStringA(inouts00);
        int idx = SimplifyString( inouts00 ) % _MAX_DIC; // 去頭去尾
        /*
        {
            FILE* f = fopen("DebugDicIdx1.txt", "ab");
            char_t buffA[128] = {"\r\n"};
            char_t buffB[128] = {"===>"};
            char_t bufid[128];
            fwrite(inouts00, strlen(inouts00), 1, f);
            fwrite(buffB, strlen(buffB), 1, f);
            sprintf(bufid, "%d", idx);
            fwrite(bufid, strlen(bufid), 1, f);
            fwrite(buffA, strlen(buffA), 1, f);
            fwrite(buffA, strlen(buffA), 1, f);
            fclose(f);
        }
        */
        //int idx = strlen(inouts00) % _MAX_DIC;
        // 以後加入的字為主
        std::map<_mstring, TranslateData>::iterator iter = dictionary_words_[ idx ].find( inouts00 );

        if( iter != dictionary_words_[ idx ].end() )
        {
            dictionary_words_[ idx ].erase( inouts00 );
        }

        //if(iter == dictionary_words_.end())
        {
            TranslateData ts;
            //ts.String = inouts01;
            char_t* inouts02 = new char_t[ ( strlen( ins2 ) + 1 ) * 4 ];
            strcpy( inouts02, ins2 );
            //int r1 = strlen(inouts02);
            //ExtractString(inouts02);    // 去頭去尾 不去頭去尾 因為 像是  \n\n AAACCCCDDDD \n\n 會比對不道
            //int r2 = strlen(inouts02);
            ts.text = inouts02;
            ts.num_used = 0;
            dictionary_words_[ idx ].insert( std::map<_mstring, TranslateData>::value_type( inouts00, ts ) );
            /*
            {
                FILE* f = fopen("DebugDicList.txt", "ab");
                char_t buffA[128] = {"\r\n"};
                char_t buffB[128] = {"===>"};
                fwrite(ins1, strlen(ins1), 1, f);
                fwrite(buffB, strlen(buffB), 1, f);
                fwrite(inouts00, strlen(inouts00), 1, f);
                fwrite(buffA, strlen(buffA), 1, f);
                fwrite(inouts02, strlen(inouts02), 1, f);
                fwrite(buffA, strlen(buffA), 1, f);
                fwrite(buffA, strlen(buffA), 1, f);
                fclose(f);
            }
            */
            SAFE_DELETE_ARRAY( inouts02 );
        }
        std::vector<SplitWord> wordsA;
        SplitString( wordsA, ins1 );
        std::vector<SplitWord> wordsB;
        SplitString( wordsB, ins2 );

        /*
        {
            FILE* f = fopen("testA1.txt", "ab");
            char_t buff[128] = {"\r\n"};

            for(int i = 0; i < wordsA.size(); i++)
            {
                fwrite(wordsA[i].before_text.c_str(), wordsA[i].before_text.size(), 1, f);
                fwrite(buff, strlen(buff), 1, f);

                fwrite(wordsA[i].after_text.c_str(), wordsA[i].after_text.size(), 1, f);
                fwrite(buff, strlen(buff), 1, f);

                for(int j = 0; j < wordsA[i].numbs.size(); j++)
                {
                    fwrite(wordsA[i].numbs[j].c_str(), wordsA[i].numbs[j].size(), 1, f);
                }
                fwrite(buff, strlen(buff), 1, f);
            }
            fclose(f);
        }

        {
            FILE* f = fopen("testA2.txt", "ab");
            char_t buff[128] = {"\r\n"};

            for(int i = 0; i < wordsB.size(); i++)
            {
                fwrite(wordsB[i].before_text.c_str(), wordsB[i].before_text.size(), 1, f);
                fwrite(buff, strlen(buff), 1, f);

                fwrite(wordsB[i].after_text.c_str(), wordsB[i].after_text.size(), 1, f);
                fwrite(buff, strlen(buff), 1, f);

                for(int j = 0; j < wordsB[i].numbs.size(); j++)
                {
                    fwrite(wordsB[i].numbs[j].c_str(), wordsB[i].numbs[j].size(), 1, f);
                }
                fwrite(buff, strlen(buff), 1, f);
            }
            fclose(f);
        }
        */
        if( wordsA.size() == wordsB.size() )
        {
            for( int i = 0; i < wordsA.size(); i++ )
            {
                char_t* splitinouts = new char_t[ ( strlen( wordsA[ i ].after_text.c_str() ) + 1 ) * 4 ];
                strcpy( splitinouts, wordsA[ i ].after_text.c_str() );
                int idxA = SimplifyString( splitinouts ) % _MAX_DIC; // 去頭去尾
                //int idxA = strlen(splitinouts) % _MAX_DIC;
                std::map<_mstring, TranslateData>::iterator iter = decompose_words_[ idxA ].find( splitinouts );

                if( iter == decompose_words_[ idxA ].end() )
                {
                    TranslateData ts;
                    //ts.String = inouts01;
                    ts.text = wordsB[ i ].after_text;
                    ts.num_used = 0;
                    decompose_words_[ idxA ].insert( std::map<_mstring, TranslateData>::value_type( splitinouts, ts ) );
                    /*
                    {
                        FILE* f = fopen("DebugDicList2.txt", "ab");
                        char_t buff[128] = {"\r\n"};
                        char_t buffA[32] = {"    ==>    "};
                        fwrite(wordsA[i].after_text.c_str(), wordsA[i].after_text.size(), 1, f);
                        fwrite(buffA, strlen(buffA), 1, f);
                        fwrite(wordsB[i].after_text.c_str(), wordsB[i].after_text.size(), 1, f);
                        fwrite(buff, strlen(buff), 1, f);
                        fclose(f);
                    }
                    */
                }

                SAFE_DELETE_ARRAY( splitinouts );
            }
        }

        SAFE_DELETE_ARRAY( inouts00 );
    }

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    _mstring Dictionary::Translate( const char_t* ins )
    {
        if( strcmp( ins, "" ) == 0 )
        {
            return "";
        }

        char_t* inouts = new char_t[ ( strlen( ins ) + 1 ) * 4 ];
        strcpy( inouts, ins );
        //strextractstring(inouts, m_SkipChar.c_str());
        //CutDownStringA(inouts);
        int idx = SimplifyString( inouts ) % _MAX_DIC; // 去頭去尾
        /*
        {
            FILE* f = fopen("TranslateWord.txt", "ab");
            char_t buff[128] = {"\r\n"};
            fwrite(ins, strlen(ins), 1, f);
            fwrite(buff, strlen(buff), 1, f);
            fwrite(inouts, strlen(inouts), 1, f);
            fwrite(buff, strlen(buff), 1, f);
            fclose(f);
        }
        */
        //int idx = strlen(inouts) % _MAX_DIC;
        /*
        {
            FILE* f = fopen("DebugDicIdx2.txt", "ab");
            char_t buffA[128] = {"\r\n"};
            char_t buffB[128] = {"===>"};
            char_t bufid[128];
            fwrite(inouts, strlen(inouts), 1, f);
            fwrite(buffB, strlen(buffB), 1, f);
            sprintf(bufid, "%d", idx);
            fwrite(bufid, strlen(bufid), 1, f);
            fwrite(buffA, strlen(buffA), 1, f);
            fwrite(buffA, strlen(buffA), 1, f);
            fclose(f);
        }
        */
        _mstring outstring = "";
        std::map<_mstring, TranslateData>::iterator iter = dictionary_words_[ idx ].find( inouts );

        if( iter != dictionary_words_[ idx ].end() )
        {
            int outl = ( strlen( ins ) + iter->second.text.size() + 1 ) * 4;
            char_t* outs = new char_t[ outl ];
            outs[ 0 ] = 0;
            char_t* rep = new char_t[ ( strlen( ins ) + 1 ) * 4 ];
            strcpy( rep, ins );
            //ExtractString(rep);
            strreplace( outs, outl, ins, rep, iter->second.text.c_str() );
            iter->second.num_used++;
            outstring = outs;
            SAFE_DELETE_ARRAY( rep );
            SAFE_DELETE_ARRAY( outs );
        }

        SAFE_DELETE_ARRAY( inouts );
        return outstring;
    }


    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    _mstring Dictionary::TranslateByCompose( const char_t* ins )
    {
        if( strcmp( ins, "" ) == 0 )
        {
            return "";
        }

        std::vector<_mstring> smakestringwords;
        std::vector<_mstring> rmakestringwords;
        int numsl = 0;
        int nuwrd = 0;
        // get each word
        {
            char_t* inouts00 = new char_t[ ( strlen( ins ) + 1 ) * 4 ];
            std::vector<SplitWord>   from_words;
            SplitString( from_words, ins );

            for( uint_t i = 0; i < from_words.size(); i++ )
            {
                char_t bufn[ 32 ] = { "    ==>    " };
                char_t bufe[ 128 ] = { "\r\n" };
                //{
                //  FILE* f = fopen("testD1.txt", "ab");
                //  char_t buff[128] = {"\r\n"};
                //  fwrite(from_words[i].after_text.c_str(), from_words[i].after_text.size(), 1, f);
                //  fwrite(bufn, strlen(bufn), 1, f);
                //  fclose(f);
                //}
                char_t* splitinouts = new char_t[ ( strlen( from_words[ i ].after_text.c_str() ) + 1 ) * 4 ];
                strcpy( splitinouts, from_words[ i ].after_text.c_str() );
                int idx = SimplifyString( splitinouts ) % _MAX_DIC; // 去頭去尾
                //int idx = strlen(splitinouts) % _MAX_DIC;
                //strcpy(inouts00, from_words[i].c_str());
                std::map<_mstring, TranslateData>::iterator iter = decompose_words_[ idx ].find( splitinouts );

                if( iter != decompose_words_[ idx ].end() )
                {
                    //{
                    //  FILE* f = fopen("testD1.txt", "ab");
                    //  fwrite(iter->second.String.c_str(), iter->second.String.size(), 1, f);
                    //  fwrite(bufe, strlen(bufe), 1, f);
                    //  fclose(f);
                    //}
                    _mstring oword;
                    MakeWord( oword, iter->second.text.c_str(), from_words[ i ].numbs );
                    //{
                    //  FILE* f = fopen("testD1.txt", "ab");
                    //  char_t buff[128] = {"\r\n"};
                    //  fwrite(from_words[i].before_text.c_str(), from_words[i].before_text.size(), 1, f);
                    //  fwrite(bufn, strlen(bufn), 1, f);
                    //  fclose(f);
                    //}
                    //{
                    //  FILE* f = fopen("testD1.txt", "ab");
                    //  char_t buff[128] = {"\r\n"};
                    //  fwrite(oword.c_str(), oword.size(), 1, f);
                    //  fwrite(buff, strlen(buff), 1, f);
                    //  fwrite(buff, strlen(buff), 1, f);
                    //  fwrite(buff, strlen(buff), 1, f);
                    //  fclose(f);
                    //}
                    smakestringwords.push_back( from_words[ i ].before_text );
                    rmakestringwords.push_back( oword );

                    for( uint_t j = 0; j < from_words[ i ].numbs.size(); j++ )
                    {
                        numsl += from_words[ i ].numbs[ j ].size();
                    }
                }
                else
                {
                    smakestringwords.push_back( from_words[ i ].before_text );
                    int idx01 = from_words[ i ].before_text.size() % _MAX_DIC;
                    std::map<_mstring, _mstring>::iterator iter = uninterpretable_words_[ idx01 ].find( from_words[ i ].before_text );

                    if( iter != uninterpretable_words_[ idx01 ].end() )
                    {
                        rmakestringwords.push_back( iter->second );
                        nuwrd += iter->second.size();
                    }
                    else
                    {
                        char_t* trans = new char_t[ 512 ];
                        sprintf( trans, "##=%06d_%04d?=##", uninterpretable_words_[ idx01 ].size(), from_words[ i ].strn );
                        rmakestringwords.push_back( trans );
                        uninterpretable_words_[ idx01 ].insert( std::map<_mstring, _mstring>::value_type( from_words[ i ].before_text, trans ) );
                        nuwrd += strlen( trans );
                        SAFE_DELETE_ARRAY( trans );
                    }

                    //{
                    //  FILE* f = fopen("testD1.txt", "ab");
                    //  char_t buff[128] = {"\r\n"};
                    //  fwrite(from_words[i].before_text.c_str(), from_words[i].before_text.size(), 1, f);
                    //  fwrite(buff, strlen(buff), 1, f);
                    //  fwrite(trans, strlen(trans), 1, f);
                    //  fwrite(buff, strlen(buff), 1, f);
                    //  fwrite(buff, strlen(buff), 1, f);
                    //  fwrite(buff, strlen(buff), 1, f);
                    //  fclose(f);
                    //}
                }

                SAFE_DELETE_ARRAY( splitinouts );
            }

            SAFE_DELETE_ARRAY( inouts00 );
        }
        _mstring outstring = "";
        {
            //int   outl = strlen(ins) + numsl + nuwrd + 256;
            int   outl = ( strlen( ins ) + ( smakestringwords.size() * 14 ) + numsl + 256 + 1024 ) * 4;
            char_t* outsA = new char_t[ outl ];
            char_t* outsB = new char_t[ outl ];
            char_t* outs = outsA;
            char_t* inss = outsB;
            strcpy( outs, ins );
            //{
            //  FILE* f = fopen("testD5.txt", "ab");
            //  char_t buff[128] = {"\r\n"};
            //  fwrite(outs,  strlen(outs),  1, f);
            //  fwrite(buff, strlen(buff), 1, f);
            //  fclose(f);          //}
            int q = 0;

            for( uint_t i = 0; i < rmakestringwords.size(); i++ )
            {
                SWAPSS( outs, inss );
                //int r = strreplace(outs, outl, ins, smakestringwords[i].c_str(), rmakestringwords[i].c_str(), true);
                q = strreplace( outs, outl, inss, smakestringwords[ i ].c_str(), rmakestringwords[ i ].c_str(), false );

                if( q == 0 )
                {
                    Pf_INFO( "Dictionary::TranslateByCompose: replace word %d\n", q );
                    //FILE* f = fopen("testD6.txt", "ab");
                    //char_t buff[128] = {"\r\n"};
                    //char_t bufn[32] = {"    ==>    "};
                    //fwrite(smakestringwords[i].c_str(), smakestringwords[i].size(), 1, f);
                    //fwrite(bufn, strlen(bufn), 1, f);
                    //fwrite(rmakestringwords[i].c_str(), rmakestringwords[i].size(), 1, f);
                    //fwrite(buff, strlen(buff), 1, f);
                    //fwrite(inss, strlen(inss), 1, f);
                    //fwrite(buff, strlen(buff), 1, f);
                    //fwrite(buff, strlen(buff), 1, f);
                    //fclose(f);
                }

                //{
                //  FILE* f = fopen("testD2.txt", "ab");
                //  char_t buff[128] = {"\r\n"};
                //  char_t bufn[32] = {"    ==>    "};
                //  fwrite(smakestringwords[i].c_str(), smakestringwords[i].size(), 1, f);
                //  fwrite(bufn, strlen(bufn), 1, f);
                //  fwrite(rmakestringwords[i].c_str(), rmakestringwords[i].size(), 1, f);
                //  fwrite(buff, strlen(buff), 1, f);
                //  //fwrite(ins, strlen(ins), 1, f);
                //  //fwrite(buff, strlen(buff), 1, f);
                //  fwrite(outs, strlen(outs), 1, f);
                //  fwrite(buff, strlen(buff), 1, f);
                //  fwrite(buff, strlen(buff), 1, f);
                //  fclose(f);
                //}
            }

            //{
            //  FILE* f = fopen("testD4.txt", "ab");
            //  char_t buff[128] = {"\r\n"};
            //  fwrite(ins,  strlen(ins),  1, f);
            //  fwrite(buff, strlen(buff), 1, f);
            //  fwrite(outs, strlen(outs), 1, f);
            //  fwrite(buff, strlen(buff), 1, f);
            //  fclose(f);
            //}
            outstring = outs;
            //if(outstring == "")
            //{
            //  int h = 0;
            //}
            SAFE_DELETE_ARRAY( outsA );
            SAFE_DELETE_ARRAY( outsB );
        }
        return outstring;
    }

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    void Dictionary::SetTranslate( LanguageSet original, LanguageSet rranslate )
    {
        original_ = original;
        translate_ = rranslate;
    }

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    _mstring GetExciteLanguageString( LanguageSet set )
    {
        switch( set )
        {
        case ls_tw:
            return "CH";

        case ls_jp:
            return "JA";

        default:
            return "";
        }
    }

    _mstring Dictionary::TranslateFromExcite( const char_t* ins )
    {
        if( strcmp( ins, "" ) == 0 )
        {
            return "";
        }

        char_t* inouts = new char_t[ ( strlen( ins ) + 1 ) * 4 ];
        strcpy( inouts, ins );
        //strextractstring(inouts, m_SkipChar.c_str());
        ExtractString( inouts );
        ExciteTranslater*  pExciteTranslater = new helper::ExciteTranslater();
        _mstring transs = pExciteTranslater->Translate( inouts, -1, GetExciteLanguageString( original_ ).c_str(),
            GetExciteLanguageString( translate_ ).c_str() ).c_str();
        delete pExciteTranslater;
        _mstring outstring = "";

        if( transs != "" )
        {
            int outl = ( strlen( ins ) + transs.size() + 1 ) * 4;
            char_t* outs = new char_t[ outl ];
            strreplace( outs, outl, ins, inouts, transs.c_str() );
            outstring = outs;
            SAFE_DELETE_ARRAY( outs );
        }

        SAFE_DELETE_ARRAY( inouts );
        return outstring;
    }

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
#define MY_ID  "Apingu"
#define MY_KEY "PinPinApinGu1120Apingu"
    _mstring GetBingLanguageString( LanguageSet set )
    {
        switch( set )
        {
        case ls_tw:
            return "zh-cht";

        case ls_jp:
            return "ja";

        default:
            return "";
        }
    }

    _mstring Dictionary::TranslateFromBing( const char_t* ins )
    {
        if( strcmp( ins, "" ) == 0 )
        {
            return "";
        }

        /*
        char_t* inouts = new char_t[strlen(ins)+1];
        strcpy(inouts, ins);
        strextractstring(inouts, m_SkipChar.c_str());

        WebBingClient*  webbingclient = new WebBingClient();
        _mstring transs=webbingclient->Translate(ins, -1, GetBingLanguageString(original_).c_str(), GetBingLanguageString(translate_).c_str()).c_str();
        delete webbingclient;

        _mstring outstring="";
        if(transs!="")
        {
            int outl = strlen(ins) +  transs.size() + 1;
            char_t* outs = new char_t[outl];
            strreplace(outs, outl, ins, inouts, transs.c_str() );
            outstring = outs;
            SAFE_DELETE_ARRAY(outs);
        }

        SAFE_DELETE_ARRAY(inouts);
        return outstring;
        */
        // Can connect to the Bing Translator?
        _mstring outstring = "";
        bing_translate::RESULT Result;

        if( bing_translate::Translator::CanConnect( Result ) )
        {
            // Instance translator
            bing_translate::Translator BingX( MY_ID, MY_KEY, Result );

            if( Result.IsSuccess() )
            {
                char_t* inouts = new char_t[ ( strlen( ins ) + 1 ) * 4 ];
                strcpy( inouts, ins );
                //strextractstring(inouts, m_SkipChar.c_str());
                ExtractString( inouts );

                // Translate string
                if( !BingX.Translate( inouts, -1, GetBingLanguageString( original_ ).c_str(), GetBingLanguageString( translate_ ).c_str(), Result ) )
                {
                }
                else
                {
                    if( Result.Info != "" )
                    {
                        int outl = ( strlen( ins ) + Result.Info.size() + 1 ) * 4;
                        char_t* outs = new char_t[ outl ];
                        strreplace( outs, outl, ins, inouts, Result.Info.c_str() );
                        outstring = outs;
                        SAFE_DELETE_ARRAY( outs );
                    }
                }

                SAFE_DELETE_ARRAY( inouts );
            }
        }

        return outstring;//Result.Info.c_str();
    }


    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    void Dictionary::ClearContent()
    {
        for( int i = 0; i < _MAX_DIC; i++ )
        {
            dictionary_words_[ i ].clear();
            decompose_words_[ i ].clear();
            uninterpretable_words_[ i ].clear();
        }
    }

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    //void Dictionary::GetContent(std::vector<_mstring> &st, std::vector<_mstring> &tt, std::vector<int> &ct)
    void Dictionary::GetContent( std::vector<stringlist_t>& nodes )
    {
        char_t buff[ 128 ];
        nodes.clear();

        for( int i = 0; i < _MAX_DIC; i++ )
        {
            std::map<_mstring, TranslateData>::iterator iter = dictionary_words_[ i ].begin();

            while( iter != dictionary_words_[ i ].end() )
            {
                stringlist_t ww;
                ww.push_back( iter->first );
                ww.push_back( iter->second.text );
                ww.push_back( itoa( iter->second.num_used, buff, 10 ) );
                nodes.push_back( ww );
                iter++;
            }
        }

        /*
        st.clear();
        tt.clear();
        ct.clear();

        std::map<_mstring, TranslateData>::iterator iter = dictionary_words_.begin();
        while(iter!=dictionary_words_.end())
        {
            st.push_back(iter->first);
            tt.push_back(iter->second.String);
            ct.push_back(iter->second.UseCnt);
            iter++;
        }
        */
    }


    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    void Dictionary::GetUninterpretableWords( std::vector<stringlist_t>& words )
    {
        for( int i = 0; i < _MAX_DIC; i++ )
        {
            std::map<_mstring, _mstring>::iterator iter = uninterpretable_words_[ i ].begin();

            while( iter != uninterpretable_words_[ i ].end() )
            {
                stringlist_t ww;
                ww.push_back( iter->second );
                ww.push_back( iter->first );
                words.push_back( ww );
                iter++;
            }
        }
    }
}

