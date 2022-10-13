//+------------------------------------------------------------+
//|                                                            |
//|              Miles's Game Assembly Functions.              |
//|                            By                              |
//|                        MiLeS  LEE                          |
//|                         李 超 群                           |
//|                         (TAIWAN)                           |
//|                                                            |
//|------------------------------------------------------------|
//|                      *!* Warning *!*                       |
//|        遊戲開發Engineer, 外人非經作者允許, 切勿使用!       |
//+------------------------------------------------------------+

typedef unsigned char*       POINTER;
typedef unsigned int*        DPOINTER;
typedef unsigned short int*  WPOINTER;
typedef unsigned char        BYTE;
typedef unsigned int         DBWORD;
typedef unsigned long int    DDWORD;
typedef int                  SWORD;
typedef unsigned short int   WORD;



extern "C"
{
 void Rol_Data(void*,  DBWORD,DBWORD);
//                   Pointer Size   Counter_Rol
 void Ror_Data(void*,DBWORD,DBWORD);
//                   Pointer Size   Counter_Ror
 void Not_Data(void*,DBWORD,DBWORD);
//                   Pointer Size   Counter_Not
 void Inc_File_Name(void*,DBWORD);
//                      File-Name  Last-Char
// filename= "I:FILE0000" -->> the Last-Char=9 (Inc by Number 0-9 )
// filename= "I:XXX_AAA" -->>  the Last-Char=8 (Inc by Char A-Z )
// filename= "I:XXX_aaa" -->>  the Last-Char=8 (Inc by Char a-z )

DBWORD Get_Key1(void*, DBWORD );

 DBWORD ConvertCurrentModeColor( DBWORD );
//                                       1
// Range: 0-255
// 1=ColorValue => 0xaabbcc   return is a DBWORD by two Same Word
//                   R G B


POINTER Get_Real_Pointer1(DBWORD,void*,void*);
//                          0      1      2
// 0= Number of you want to get (0,1,2...)
// 1= Table (4bytes/unit)
// 2= Data

 void DrawLine16(DBWORD,DBWORD,DBWORD,DBWORD,DBWORD, void* , DBWORD );
//                  0      1      2      3      4      5       6
// 0=X1   1=Y1   2=X2   3=Y2   4=Color(0xffffff)
//                                       RRGGBB
// 5=Buffer Pointer    6=Buffer's Wide

 void DrawPixel16(DBWORD,DBWORD,DBWORD,DBWORD);
//                  X      Y    Color  Alpha(0-63)
	

 void DrawLine16t2(DBWORD,DBWORD,DBWORD,DBWORD,DBWORD, void* , DBWORD );
//                   0      1      2      3      4      5       6
// 0=X1   1=Y1   2=X2   3=Y2   4=Color(0xffffff)
//                                       RRGGBB
// 5=Buffer Pointer    6=Buffer's Wide

 void LineBlock16( DBWORD , DBWORD , DBWORD , DBWORD , DBWORD , DBWORD ,   void* , DBWORD , DBWORD );
//                   X        Y        W        H   Pixel-Board Color   Buffer  Buffer-W  Show-Level
//                                                           (0xffffff)
//                                                              RRGGBB


 void ColorBlock16(DBWORD , DBWORD , DBWORD , DBWORD , DBWORD , void* , DBWORD );
//                   0        1        2        3       4        5       6
//  0=X  1=Y  2=Wide  3=High  4=Color  5=WorkBuffer  6=WorkBuffer's Wide(unit:Dot)

 void ColorBlock16MultiColor(DBWORD , DBWORD , DBWORD , DBWORD , DBWORD , DBWORD, void* , DBWORD );
//                             0        1        2        3       4        5       6        7
//  0=X  1=Y  2=Wide  3=High  4=Color1 5=Color2 6=WorkBuffer 7=WorkBuffer's Wide(unit:Dot)

 void ColorBlockAdd_R_16(DBWORD , DBWORD , DBWORD , DBWORD , DBWORD , void* , DBWORD );
//                              0        1        2        3       4        5       6
//  0=X  1=Y  2=Wide  3=High  4=Color  5=WorkBuffer  6=WorkBuffer's Wide(unit:Dot)
 void ColorBlockAdd_G_16(DBWORD , DBWORD , DBWORD , DBWORD , DBWORD , void* , DBWORD );
//                              0        1        2        3       4        5       6
//  0=X  1=Y  2=Wide  3=High  4=Color  5=WorkBuffer  6=WorkBuffer's Wide(unit:Dot)
 void ColorBlockAdd_B_16(DBWORD , DBWORD , DBWORD , DBWORD , DBWORD , void* , DBWORD );
//                              0        1        2        3       4        5       6
//  0=X  1=Y  2=Wide  3=High  4=Color  5=WorkBuffer  6=WorkBuffer's Wide(unit:Dot)

 void ColorBlock16Dark(DBWORD , DBWORD , DBWORD , DBWORD , void* , DBWORD , DBWORD);
//                        0        1        2        3       4        5        6
//  0=X  1=Y  2=Wide  3=High  4=WorkBuffer  5=WorkBuffer's Wide(unit:Dot)
//  6=Level(0-63)
//  The function will Dark Block zone
 void Set_MaxLimited( void* , void* , void* );
//                            0       1       2
//  0=MinPointer   1=MaxPointer   2=StartDrawPointer

 void putZp_NL_pmask8 (SWORD, SWORD, void*, void*, DBWORD, DBWORD);
//                                                           
//                            a1     a2     a3     a4     a5     a6
//       a1: X (可為負數)
//       a2: Y (可為負數)
//       a3: Source Pointer(需為 Zp 格式)
//       a4: Buffer Pointer
//       a5: Buffer's wide (不可為負數)
//       a6: Buffer's High (不可為負數)
 void putZp_pmask8 (SWORD, SWORD, void*, void*, DBWORD);
//                          0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void putZp_NL_pmask16 (SWORD, SWORD, void*, void*, DBWORD, DBWORD);


 void putZp_pmask16 (SWORD, SWORD, void*, void*, DBWORD);

//
// OFFSET = 含偏移值
//
//-------- 以下為自己有附調色盤!
 void put8_pmask16p(DBWORD,DBWORD,void*,void*,DBWORD);
//                    0      1      2     3     4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void put8_pmask16pt1(DBWORD,DBWORD,void*,void*,DBWORD);
//                      0      1      2     3     4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void putZp_pset16p (SWORD, SWORD, void*, void*, DBWORD);
//                     0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void putZp_pset16p_4m (SWORD, SWORD, void*, void*, DBWORD);
//                        0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
// can show 4 modes without AdjustX555...
 void putZp_pset16p_4mRC (SWORD, SWORD, void*, void*, DBWORD);
//                        0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
// can show 4 modes without AdjustX555... ,  Palette Table 只記載實際用色數
//
 void putZp_pset16pOffset (SWORD, SWORD, void*, void*, DBWORD);
//                           0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void putZp_pset16_64p (SWORD, SWORD, void*, void*, DBWORD);
//                        0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
// Only can use Color 0-63(128 BYTES Palette)
 void putZp_pset16_128p (SWORD, SWORD, void*, void*, DBWORD);
//                        0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
// Only can use Color 0-63(128 BYTES Palette)
//  Wide is Offset 256 ->> Wide | 1000h -> Must -> Wide & 7fffh = Real

 void putZp_pset16_64pRC (SWORD, SWORD, void*, void*, DBWORD);
//                        0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
// Only can use Color 0-63(128 BYTES Palette),  Palette Table 只記載實際用色數
 void putZp_pset16_128pRC (SWORD, SWORD, void*, void*, DBWORD);
//                        0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
// Only can use Color 0-63(128 BYTES Palette),  Palette Table 只記載實際用色數
//  Wide is Offset 256 ->> Wide | 1000h -> Must -> Wide & 7fffh = Real

 void putZp_pset16pv2 (SWORD, SWORD, void*, void*, DBWORD, DBWORD);
//                       0      1      2      3       4      5
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)  5=ShowLevel(0-63)
 void putZp_pset16pv2Offset (SWORD, SWORD, void*, void*, DBWORD, DBWORD);
//                             0      1      2      3       4      5
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)  5=ShowLevel(0-63)
 void putZp_pmask16p  (SWORD, SWORD, void*, void*, DBWORD);
//                       0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void putZp_pmask16Dark (SWORD, SWORD, void*, void*, DBWORD);
//                         0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void putZp_pmask16pv2 (SWORD, SWORD, void*, void*, DBWORD, DBWORD);
//                        0      1      2      3       4       5
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)  5=ShowLevel(0-63)
 void putZp_pshadow16pv2 (SWORD, SWORD, void*, void*, DBWORD);
//                          0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
// 圖素中不包含色盤表( Not include Palette(512BYTES) )，以 0-63 為顏色暗表
 void putZp_pshadow16pv2Offset (SWORD, SWORD, void*, void*, DBWORD);
//                                0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
// 圖素中不包含色盤表( Not include Palette(512BYTES) )，以 0-63 為顏色暗表
 void putZp_pmask16pv2Offset (SWORD, SWORD, void*, void*, DBWORD, DBWORD);
//                              0      1      2      3       4       5
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)  5=ShowLevel(0-63)
 void putZp_pmask16pv3Offset (SWORD, SWORD, void*, void*, DBWORD, DBWORD);
// 變亮                         0      1      2      3       4       5
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)  5=ShowLevel(0-63)
 void putZp_pmask16pOffset (SWORD, SWORD, void*, void*, DBWORD);
//                            0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void putZp_pmask16pOffset_RGB (SWORD, SWORD, void*, void*, DBWORD,DBWORD);
//                                0      1      2      3       4      5
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
// 5=Mode


//========================== MASK 透明 show ==========================
 void putZp_pmask16pt1   (SWORD, SWORD, void*, void*, DBWORD);
//                          0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void putZp_pmask16pt1v2 (SWORD, SWORD, void*, void*, DBWORD,DBWORD);
//                          0      1      2      3       4       5
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)  5=ShowLevel(0-63)
 void putZp_pmask16pt1Offset (SWORD, SWORD, void*, void*, DBWORD);
//                              0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void putZp_pmask16pt1Offset_Mirror (SWORD, SWORD, void*, void*, DBWORD);
//                                     0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void putZp_pmask16pt1v2Offset (SWORD, SWORD, void*, void*, DBWORD, DBWORD);
//                                0      1      2      3       4       5
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)  5=ShowLevel(0-63)
 void putZp_pmask16pt1v2Offset_Mirror (SWORD, SWORD, void*, void*, DBWORD, DBWORD);
//                                       0      1      2      3       4       5
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)  5=ShowLevel(0-63)

//========================== MASK 暈光 show ==========================
 void putZp_pmask16pt2       (SWORD, SWORD, void*, void*, DBWORD);
//                              0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void putZp_pmask16pt2v2     (SWORD, SWORD, void*, void*, DBWORD,DBWORD);
//                              0      1      2      3       4       5
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)  5=ShowLevel(0-63)
 void putZp_pmask16pt2Offset (SWORD, SWORD, void*, void*, DBWORD);
//                              0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void putZp_pmask16pt2Offset_Mirror (SWORD, SWORD, void*, void*, DBWORD);
//                                     0      1      2      3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void putZp_pmask16pt2aOffset (SWORD, SWORD, void*, void*, DBWORD);
//                               0      1      2      3       4
//  240 - 255 = Mask
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void putZp_pmask16pt2v2Offset (SWORD, SWORD, void*, void*, DBWORD, DBWORD);
//                                0      1      2      3       4       5
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)  5=ShowLevel(0-63)
 void putZp_pmask16pt2v2Offset_Mirror (SWORD, SWORD, void*, void*, DBWORD, DBWORD);
//                                       0      1      2      3       4       5
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)  5=ShowLevel(0-63)


void Dark16v2(void*,         DBWORD);
//            Source(PUT16)  ShowLevel(0-63)

//======== X - 255 = Smooth Shadow!
 void Set_SmoothShadowLevel(DBWORD);
//                          Level Value, Max is 16
 BYTE Get_SmoothShadowLevel(void);
//  Return a Value is x-255 , 0=Not Set or ...

 void putZp_pmask16pX2Offset (DBWORD,DBWORD,void*,void*,DBWORD);
//                              0      1     2     3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
// First, to call Set_SmoothShadowLevel
// Special Color No.:  X - 255 = Smooth Shadow
 void putZp_pmask16pX2Offset_Mirror (DBWORD,DBWORD,void*,void*,DBWORD);
//                                     0      1     2     3       4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
// First, to call Set_SmoothShadowLevel
// Special Color No.:  X - 255 = Smooth Shadow
// Will be Mirror
void putZp_pmask16pLMX2Offset(DBWORD,DBWORD,void*,void*,DBWORD,DBWORD);
//                              0      1     2     3      4      5
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
// 5=Level(1-5) to shift left value(add Light)
// First, to call Set_SmoothShadowLevel
// Special Color No.:  X - 255 = Smooth Shadow
void putZp_pmask16pLMX2Offset_Mirror(DBWORD,DBWORD,void*,void*,DBWORD,DBWORD);
//                                     0      1     2     3      4      5
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
// 5=Level(1-5) to shift left value(add Light)
// First, to call Set_SmoothShadowLevel
// Special Color No.:  X - 255 = Smooth Shadow

//======== 255 = Shadow
 void putZp_pmask16pX0Offset ( SWORD , SWORD, void*,void*,DBWORD);
//                               0       1      2     3     4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)
 void putZp_pmask16pX0OffsetTable ( SWORD , SWORD, void*,void*,DBWORD);
//                                    0       1      2     3     4
// 0=X  1=Y  2=ZP_Pointer  3=Work Buffer  4=Work Buffer's Wide(Dot)





 void Change_To_3DTB( DBWORD , DBWORD , DBWORD , DBWORD , void* , DBWORD , void* , DBWORD );
//                      0        1        2        3        4       5        6        7
// 0=X  1=Y  2=Wide  3=high  4=Buffer-Pointer  5=Buffer-Wide
// 6=ZP-Pointer(No Offset), if is (void*)0 then use Target backGround
// 7=Mode( 0=Not Press   1=Pressed )
 void Draw_3DTBlock0( DBWORD , DBWORD , DBWORD , DBWORD , void* , DBWORD , DBWORD , DBWORD , DBWORD );
//                      0        1        2        3        4       5        6        7        8
// 0=X  1=Y  2=Wide  3=high  4=WorkBuffer  5=WorkBuffer wide(dots)
// 6=R(0-63)  7=G(0-63)  8=B(0-63)       <*>  Min Size=6x5
//

 void Draw_3DBlock0(DBWORD,DBWORD,DBWORD,DBWORD,void*,DBWORD,DBWORD,DBWORD,DBWORD,DBWORD,void*,DBWORD);
//                     0      1      2      3     4      5      6      7      8      9     10    11
// 0=X  1=Y  2=Wide  3=high  4=WorkBuffer  5=WorkBuffer wide(dots)
// 6=R(0-63)  7=G(0-63)  8=B(0-63)   * Min Size=6x5
// 9=TitleBar (0=Off 1=On)
// 10=Draw Select Zone Struct Pointer0  ===>> (void*)0 = None
//    X(DWORD),Y(DWORD),W(DWORD),H(DWORD) ==>> Total 16bytes/per block
//       |
//    0xffffffff = END
//
// 11=BackGround Select 0-9 (call "Set_Bar_BG0(Pointer)" first!) 0xffffffff=None

 void Draw_RoundBlock0( DBWORD , DBWORD , DBWORD , DBWORD , void* , DBWORD , DBWORD , DBWORD , DBWORD , DBWORD);
//                        0       1        2        3        4       5        6        7        8        9
// 0=X  1=Y  2=Wide  3=high  4=WorkBuffer  5=WorkBuffer wide(dots)
// 6=R(0-63)  7=G(0-63)  8=B(0-63)  9=Show Switch(0/1)     * Min Size=14x13
 void Draw_SquareBlock0( DBWORD , DBWORD , DBWORD , DBWORD , void* , DBWORD , DBWORD , DBWORD , DBWORD , DBWORD);
//                         0       1        2        3        4       5        6        7        8        9
// 0=X  1=Y  2=Wide  3=high  4=WorkBuffer  5=WorkBuffer wide(dots)
// 6=R(0-63)  7=G(0-63)  8=B(0-63)  9=Show Switch(0/1)     * Min Size=6x5
 void Draw_SquareBlock1( DBWORD , DBWORD , DBWORD , DBWORD , void* , DBWORD , DBWORD , DBWORD , DBWORD , DBWORD);
//                         0       1        2        3        4       5        6        7        8        9
// 0=X  1=Y  2=Wide  3=high  4=WorkBuffer  5=WorkBuffer wide(dots)
// 6=R(0-63)  7=G(0-63)  8=B(0-63)  9=Show Switch(0/1)     * Min Size=8x7
// With Out-Line by Dark.
 void Draw_SquareBlockA( DBWORD , DBWORD , DBWORD , DBWORD , void* , DBWORD , DBWORD , DBWORD , DBWORD , DBWORD);
//                         0       1        2        3        4       5        6        7        8        9
// 0=X  1=Y  2=Wide  3=high  4=WorkBuffer  5=WorkBuffer wide(dots)
// 6=R(0-63)  7=G(0-63)  8=B(0-63)  9=Show Switch(0/1)     * Min Size=4x3
 void Draw_SquareBlockApp( DBWORD , DBWORD , DBWORD , DBWORD , void* , DBWORD , DBWORD , DBWORD , DBWORD , DBWORD, void*);
//                           0       1        2        3        4       5        6        7        8        9       10
// 0=X  1=Y  2=Wide  3=high  4=WorkBuffer  5=WorkBuffer wide(dots)
// 6=R(0-63)  7=G(0-63)  8=B(0-63)  9=Show Switch(0/1)     * Min Size=?x?
// 10 = 四邊的框框圖形   16-Bytes-Tables + Datas

 void Draw_SquareBlockB( DBWORD , DBWORD , DBWORD , DBWORD , void* , DBWORD , DBWORD , DBWORD , DBWORD);
//                         0       1        2        3        4       5        6        7        8
// 0=X  1=Y  2=Wide  3=high  4=WorkBuffer  5=WorkBuffer wide(dots)
// 6=R(0-63)  7=G(0-63)  8=B(0-63)                        * Min Size=2x1

//-------------- 原始
 void UnPz32Bits1( void* , void* );
//-------------- 原始 x2 縱向加黑線
 void UnPz32Bits2( void* , void* , DBWORD );
//-------------- 原始 x2
 void UnPz32Bits3( void* , void* , DBWORD );
//-------------- 原始 x2 縱向加暗線
 void UnPz32Bits4( void* , void* , DBWORD );
//-------------- 原始 x2 縱向加網線
 void UnPz32Bits5( void* , void* , DBWORD );
//-------------- 原始 x2 縱向加細漸層網線
 void UnPz32Bits6( void* , void* , DBWORD );
//-------------- 原始 x2 縱向加細中間網線
 void UnPz32Bits7( void* , void* , DBWORD );

 void PutfT2H_Alpha( DBWORD , DBWORD , void* ,  void* , DBWORD ,    void* ,    DBWORD);
//  Direct Pset        X        Y      Source   Buffer  Buffer-Wide Alpha-Ptr  自己改變旗標(1=Enable 0=Disable)
// Set the Bit-Mode first.             (24Bits) (16Bits)            (8Bits Not include W&H)
 void PutfT2H( DBWORD , DBWORD , void* , void* , DBWORD , DBWORD);
//  Direct Pset  X        Y      Source  Buffer Buffer-Wide Buffer-High
// Set the Bit-Mode first.       (24Bit) (16Bit)
 void PutfI2H( DBWORD , DBWORD , void* , void* , void* , DBWORD );
//  Direct Pset  X        Y      Source  Palette Buffer  Buffer-Wide
// Set the Bit-Mode first.       (8Bit)  0-255   (16Bit)

 void Putf16( DBWORD , DBWORD , void* , void* , DBWORD );
//  Direct Pset X        Y      Source  Buffer  Buffer-Wide
 void Putf16wave( DBWORD , DBWORD , void* , void* ,      DBWORD ,      void* );
//  Wave Show       X        Y     Buffr  Source-Buffer Buffer-Wide   WaveTable
 void Putft16( DBWORD , DBWORD , void* , void* , DBWORD );
//Direct Trans Pset X     Y      Source  Buffer  Buffer-Wide
 void Putf16v2( DBWORD , DBWORD , void* , void* , DBWORD ,    DBWORD);
//  Direct Pset X        Y        Source   Buffer  Buffer-Wide TransLevel
 void Putf16v2wave( DBWORD , DBWORD , void* , void* , DBWORD ,    DBWORD,     void*);
//  Direct Pset     X        Y        Source   Buffer  Buffer-Wide TransLevel  Wave-Table
 void Putf16t1v2( DBWORD , DBWORD , void* , void* , DBWORD ,    DBWORD);
//  Trans             X        Y    Source   Buffer  Buffer-Wide TransLevel
 void Putf16t1v2Single( DBWORD , DBWORD , DBWORD , DBWORD, DBWORD  , void* , DBWORD ,    DBWORD);
//  Trans                       X        Y      Wide     High    Color     Buffer  Buffer-Wide TransLevel
// 指定一個顏色(會轉成正確)與 Buffer 融合

 void Putf16t2v2( DBWORD , DBWORD , void* , void* , DBWORD ,    DBWORD);
//  LightPluset       X        Y        Source   Buffer  Buffer-Wide TransLevel


 void PutM16( DBWORD , DBWORD , void* , void* , DBWORD );
// 0000=ColorKey      X        Y      Source   Buffer  Buffer-Wide
 void PutMt16( DBWORD , DBWORD , void* , void* , DBWORD );
//Trans! 0000=ColorKey X        Y      Source   Buffer  Buffer-Wide
 void PutM16v2( DBWORD , DBWORD , void* , void* , DBWORD ,    DBWORD);
// 0000=ColorKey  X        Y     Source   Buffer  Buffer-Wide TransLevel
 void PutM16t1v2( DBWORD , DBWORD , void* , void* , DBWORD ,    DBWORD);
//  Trans!          X        Y     Source   Buffer  Buffer-Wide TransLevel(0-63)
 void PutM16t2v2( DBWORD , DBWORD , void* , void* , DBWORD ,    DBWORD);
//  LightPluset     X        Y     Source   Buffer  Buffer-Wide TransLevel
 void GetPic24_2TGA(DBWORD,DBWORD,DBWORD,DBWORD,DBWORD,  DBWORD,  void*, void*,  DBWORD);
//                  X ,    Y ,    W ,    H ,    Buffer_W,Buffer_H,Buffer,Source ,Source-Wide
 void GetPic24(DBWORD,DBWORD,DBWORD,DBWORD,void*, void*,  DBWORD);
//               X ,    Y ,    W ,    H ,  Buffer,Source ,Source-Wide
 void GetPic16( DBWORD , DBWORD , DBWORD , DBWORD , void* , void* , DBWORD );
//               X        Y        Wide     High     Buffer  Source  Source-Wide
 void GetPic8( DBWORD  , DBWORD , DBWORD , void* , void* , DBWORD );
//              XY       Wide     High     Buffer  Source  Source-Wide
 void Putf8( DBWORD,DBWORD, void*, void*, DBWORD );
//             X      Y     Source Buffer Buffer-Wide
 void PutM8( DBWORD,DBWORD, void*, void*, DBWORD );
//             X      Y     Source Buffer Buffer-Wide

 void GetPutM16(DBWORD,DBWORD,DBWORD,DBWORD,void*, DBWORD,     void*, DBWORD);
//              X      Y      Wide   High   Buffer Buffer-Wide Source Source-Wide
 void GetPutF16(DBWORD,DBWORD,DBWORD,DBWORD,void*, DBWORD,     void*, DBWORD);
//              X      Y      Wide   High   Buffer Buffer-Wide Source Source-Wide
 void GetPutAlpha16(DBWORD,DBWORD,DBWORD,DBWORD,void*, DBWORD,     void*, DBWORD,     void*);
//                  X      Y      Wide   High   Buffer Buffer-Wide Source Source-Wide Alpha(BYTE)


 void GetPutF24(DBWORD,DBWORD,DBWORD,DBWORD,void*, DBWORD,     void*, DBWORD);
//              X      Y      Wide   High   Buffer Buffer-Wide Source Source-Wide

 void Set_AscFont(void*);
// Default Font: 'NEWS'
// if want to change font, must Call first before use Put_string_XX.
// Format is Mono(Bit 0/1), exp: ASCFONT.15(3840 bytes),but just use 1920 bytes(128 chars)

 void Set_AppointFont_Pointer(void*,  void*,   DBWORD);
//                            Table   Data     to Store 0-4

 DBWORD Put_AppointString_16(DBWORD,DBWORD,void* ,void*, DBWORD,     DBWORD,             DBWORD);
//                             X,    Y,    String,Buffer,Buffer-Wide,UseAppoNumber(0-4), Add-Wide-Step
// return a total use Wide

 DBWORD Put_AppointNumber_16(DBWORD,DBWORD,DBWORD,void*, DBWORD,     DBWORD,             DBWORD);
//                             X,    Y,    Number,Buffer,Buffer-Wide,UseAppoNumber(0-4), Add-Wide-Step
// return a total use Wide

 DBWORD Put_AppointStringColorSet_16(DBWORD,DBWORD,void* ,void*, DBWORD,     DBWORD,             DBWORD,       DBWORD);
//                                     X,    Y,    String,Buffer,Buffer-Wide,UseAppoNumber(0-4), Add-Wide-Step Color(ffffff-000000)
// return a total use Wide  , 0ffh= Key Color want to Change

 DBWORD Put_AppointNumberColorSet_16(DBWORD,DBWORD,DBWORD,void*, DBWORD,     DBWORD,             DBWORD,       DBWORD);
//                                     X,    Y,    Number,Buffer,Buffer-Wide,UseAppoNumber(0-4), Add-Wide-Step Color(ffffff-000000)
// return a total use Wide  , 0ffh= Key Color want to Change

 void Put_Number_16( DBWORD , DBWORD , DBWORD , void* , DBWORD , DBWORD);
//                     0        1       2       3        4        5
//  0=X,  1=Y,  2=Number to show,  3=Work Buffer,  4=Work Buffer's Wide
// 5=Color(24bits) => Red=0xFF0000  Green=0x00FF00  Blue=0x0000FF
 void Put_String_16( DBWORD , DBWORD , void* , void* , DBWORD );
//                     0        1       2       3        4
//  0=X,  1=Y,  2=String Pointer,  3=Work Buffer,  4=Work Buffer's Wide
//  ColorValue= 0 - 3F   Exp: /3F3F3F_       (陰影部份為實黑(00 00 01))

 void Put_String_16t( DBWORD , DBWORD , void* , void* , DBWORD );
//                     0        1       2       3        4
//  0=X,  1=Y,  2=String Pointer,  3=Work Buffer,  4=Work Buffer's Wide
//  ColorValue= 0 - 3F   Exp: /3F3F3F_       (陰影部份為透明的)

 void Put_String_16_12( DBWORD , DBWORD , void* , void* , DBWORD );
//                        0        1       2       3        4
//  0=X,  1=Y,  2=String Pointer,  3=Work Buffer,  4=Work Buffer's Wide
//  ColorValue= 0 - 3F   Exp: /3F3F3F_

 void Put_String_Single_16_12( DBWORD , DBWORD , void* , void* , DBWORD , DBWORD );
//                               0        1       2       3        4        5
//  0=X,  1=Y,  2=String Pointer,  3=Work Buffer,  4=Work Buffer's Wide
//  5=ColorValue -> 0xFFFFFF(RRGGBB)


 void Put_String_16_Smooth( DBWORD , DBWORD , void* , void* , DBWORD );
//                            0        1       2       3        4
//  0=X,  1=Y,  2=String Pointer,  3=Work Buffer,  4=Work Buffer's Wide
//  ColorValue= 0 - 3F   Exp: /3F3F3F_

 void Put_String_Single_16_Smooth( DBWORD , DBWORD , void* , void* , DBWORD , DBWORD);
//                                   0        1       2       3        4        5
//  0=X,  1=Y,  2=String Pointer,  3=Work Buffer,  4=Work Buffer's Wide
//  5=ColorValue -> 0xFFFFFF(RRGGBB)

 void Put_Korea1_Single_16( DBWORD , DBWORD , void* , void* , DBWORD , DBWORD , DBWORD , DBWORD );

 void Put_Korea1_16( DBWORD , DBWORD , void* , void* , DBWORD );
//                     0        1       2       3        4
//  0=X,  1=Y,  2=String Pointer,  3=Work Buffer,  4=Work Buffer's Wide
//  ColorValue= 0 - 3F   Exp: /3F3F3F_
 void Put_String_16tv2( DBWORD , DBWORD , void* , void* , DBWORD , DBWORD);
//                        0        1       2       3        4        5
//  0=X,  1=Y,  2=String Pointer,  3=Work Buffer,  4=Work Buffer's Wide
//  5=Trans. level( 0 - 63 )
//  ColorValue= 0 - 3F   Exp: /3F3F3F_
 void Put_String_16MultiColor( DBWORD , DBWORD , void* , void* , DBWORD );
//                               0        1       2       3        4
//  0=X,  1=Y,  2=String Pointer,  3=Work Buffer,  4=Work Buffer's Wide
//  顏色以漸層表示 ColorValue= 0 - FF   Exp: /FFFFFF000000_
//                                            R G B R G B
//                                            First Second
 void Put_String_16MultiColorSet( DBWORD , DBWORD , void* , void* , DBWORD , DBWORD , DBWORD );
//                                  0        1       2       3        4        5        6
//  0=X,  1=Y,  2=String Pointer,  3=Work Buffer,  4=Work Buffer's Wide 
// 5=First Color(24bits) => Red=0xFF0000  Green=0x00FF00  Blue=0x0000FF
// 6=Second Color(24bits) => Red=0xFF0000  Green=0x00FF00  Blue=0x0000FF
// 字顏色以漸層表示
 void Put_String_16MultiColorSet2( DBWORD , DBWORD , void* , void* , DBWORD , DBWORD , DBWORD );
//                                   0        1       2       3        4        5        6
//  0=X,  1=Y,  2=String Pointer,  3=Work Buffer,  4=Work Buffer's Wide 
// 5=First Color(24bits) => Red=0xFF0000  Green=0x00FF00  Blue=0x0000FF
// 6=Second Color(24bits) => Red=0xFF0000  Green=0x00FF00  Blue=0x0000FF
// 字顏色以漸層表示，只是立體部份是黑色，不是黑透明
 void Put_Number_16MultiColor( DBWORD , DBWORD , DBWORD , void* , DBWORD , DBWORD , DBWORD);
//                               0        1       2       3        4        5         6
//  0=X,  1=Y,  2=Number to show,  3=Work Buffer,  4=Work Buffer's Wide
// 5=First Color(24bits) => Red=0xFF0000  Green=0x00FF00  Blue=0x0000FF
// 6=Second Color(24bits) => Red=0xFF0000  Green=0x00FF00  Blue=0x0000FF
// 數字顏色以漸層表示
 void Put_Korea1_16tv2( DBWORD , DBWORD , void* , void* , DBWORD , DBWORD);
//                        0        1       2       3        4        5
//  0=X,  1=Y,  2=String Pointer,  3=Work Buffer,  4=Work Buffer's Wide
//  5=Trans. level( 0 - 63 )
//  ColorValue= 0 - 3F   Exp: /3F3F3F_
 void Put_Korea1_16MultiColor( DBWORD , DBWORD , void* , void* , DBWORD );
//                               0        1       2       3        4
//  0=X,  1=Y,  2=String Pointer,  3=Work Buffer,  4=Work Buffer's Wide
//  顏色以漸層表示 ColorValue= 0 - FF   Exp: /FFFFFF000000_
//                                            R G B R G B
//                                            First Second
 void Put_Korea1_16MultiColorSet( DBWORD , DBWORD , void* , void* , DBWORD , DBWORD , DBWORD );
//                                  0        1       2       3        4        5        6
//  0=X,  1=Y,  2=String Pointer,  3=Work Buffer,  4=Work Buffer's Wide 
// 5=First Color(24bits) => Red=0xFF0000  Green=0x00FF00  Blue=0x0000FF
// 6=Second Color(24bits) => Red=0xFF0000  Green=0x00FF00  Blue=0x0000FF
// 字顏色以漸層表示
 void Put_Korea1_16MultiColorSet2( DBWORD , DBWORD , void* , void* , DBWORD , DBWORD , DBWORD );
//                                   0        1       2       3        4        5        6
//  0=X,  1=Y,  2=String Pointer,  3=Work Buffer,  4=Work Buffer's Wide 
// 5=First Color(24bits) => Red=0xFF0000  Green=0x00FF00  Blue=0x0000FF
// 6=Second Color(24bits) => Red=0xFF0000  Green=0x00FF00  Blue=0x0000FF
// 字顏色以漸層表示，只是立體部份是黑色，不是黑透明



 void Put_String_24( DBWORD , DBWORD , void* , void* , DBWORD , DBWORD );

 void Put_String_Single_16( DBWORD , DBWORD , void* , void* , DBWORD , DBWORD , DBWORD , DBWORD );
//                                                                     R(0-63)  G(0-63)  B(0-63)
 void Put_String_8( DBWORD , DBWORD , void* , void* , DBWORD );


 DBWORD Miles_atoh(void*);
 void Con10(DBWORD,void*);
 void Con10set(DBWORD,void*,DBWORD);
//                          ^^^^^^ 幾位數

 void Con10S(DBWORD,void*);
 void Con10D(DBWORD,void*);
 DBWORD Con10L(DBWORD,void*);

 void ConA10(void*,void*);// First Pointer is Number-Pointer (Will be show Double Word(4 BYTES)))
 void ConA10S(void*,void*);
 DBWORD ConA10L(void*,void*);

 void Set_ChineseBitMap_Ptr(void*);
// Must call first befor use Put_string_XX to show BIG-5.

void Set_Font_Information( DBWORD //AscFontWide     字寬
                          ,DBWORD //AscFontHigh     字高
                          ,void*  //AscFontBitBuffer
                          ,DBWORD //AscFontSize 一字的 BYTE 數
                          ,DBWORD //AscFont         字距 (點數 0,1...)
                          ,DBWORD //ChineseFontWide 字寬
                          ,DBWORD //ChineseFontHigh 字高
                          ,void*  //ChineseFontBitBuffer
                          ,DBWORD //ChineseFontSize 一字的 BYTE 數
                          ,DBWORD //ChineseFont     字距 (點數 0,1...)
                          );
void Set_SmoothFont_Information( DBWORD //AscFontWide     字寬
                                ,DBWORD //AscFontHigh     字高
                                ,void*  //AscFontBitBuffer
                                ,DBWORD //AscFontSize 一字的 BYTE 數
                                ,DBWORD //AscFont         字距 (點數 0,1...)
                                ,DBWORD //ChineseFontWide 字寬
                                ,DBWORD //ChineseFontHigh 字高
                                ,void*  //ChineseFontBitBuffer
                                ,DBWORD //ChineseFontSize 一字的 BYTE 數
                                ,DBWORD //ChineseFont     字距 (點數 0,1...)
                                );
// Must call first befor use Put_string_16_XX to show BIG-5.
void Adjust_Font_Information( DBWORD //AscFontWide     字寬
                             ,DBWORD //AscFontHigh     字高
                             ,DBWORD //AscFontSize 一字的 BYTE 數
                             ,DBWORD //AscFont         字距 (點數 0,1...)
                             ,DBWORD //ChineseFontWide 字寬
                             ,DBWORD //ChineseFontHigh 字高
                             ,DBWORD //ChineseFontSize 一字的 BYTE 數
                             ,DBWORD //ChineseFont     字距 (點數 0,1...)
                             );


 void Set_Korea1BitMap_Ptr(void*);
// Must call first befor use Put_Korea1_XX to show HanGul Font

 void Set_Bar_BG0(void*);
// Set Bar BackGround Picture for 'Draw_3DBlock0'

 void Create_16bits_Table0(void*,void*,DBWORD);
//                         Src   Targ  16-BitsMode(0-3)
// Convert Index256 data to 16-bits Table data

 DBWORD Get_Between0(void*,void*);
// Get between from two pointer
 void stoslcc(void*,DBWORD,DBWORD);
//                   0      1      2
// 0=Buffer Pointer  1=Length(byte)  2=Store Value (use DWORD)


 DBWORD MessageCopy(void*,void*,DBWORD);
//return the Numbers of chars    ^^^Max Chars(0=No limite)

 void copydata(void*,void*,DBWORD);
//             Src   Targ  Length(byte)
//

 void Swapdata(void*,void*,DBWORD);
//             Ptr1  Ptr2  Length(byte)

 void ptxtm    (DBWORD,DBWORD,void*,DBWORD);
//Display on MonoCard   x      y   String  color(0x07=Normal)
 void ptxtNom  (DBWORD,DBWORD,DBWORD,DBWORD);
//Display on MonoCard   x      y    Number  color(0x07=Normal)



 DBWORD Copy_TGA16_Window16(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD);
//                           0     1       2      3      4      5
// 0=Source of 16Bits G-Buffer   1=TGA Buffer
// 2=Copy-Wide(dot)  3=Copy-High(dot)   4=Real-Wide of G-Buffer(unit:BYTE)
// 5=TGA's Wide(dot,can not same as Copy-Wide, will paste center)
// Return value=TGA size (16Bits Format)
 DBWORD Copy_TGA_Window16(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD);
//                         0     1       2      3      4      5
// 0=Source of 16Bits G-Buffer   1=TGA Buffer
// 2=Copy-Wide(dot)  3=Copy-High(dot)   4=Real-Wide of G-Buffer(unit:BYTE)
// 5=TGA's Wide(dot,can not same as Copy-Wide, will paste center)
// Return value=TGA size
 DBWORD Copy_TGA_Window16_Half(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD);
//                              0     1       2      3      4      5
// 0=Source of 16Bits G-Buffer   1=TGA Buffer
// 2=Copy-Wide(dot)  3=Copy-High(dot)  Exp: 320 is copy 640 ,240 is copy 480
// Mean is scale 640x480 to 320x240
// 4=Real-Wide of G-Buffer(*unit:BYTE)
// 5=TGA's Wide(dot,can not same as Copy-Wide, will paste center)
// Return value=TGA size


 void CopyWindow16(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD);
//                 Src   Tag   W-Wide W-High S-Wide T-Wide
//                             U:WORD U:WORD U:WORD U:BYTE

 void CopyWindow16t1(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD);
//   Trans Copy      Src   Tag   W-Wide W-High S-Wide T-Wide
//                               U:WORD U:WORD U:WORD U:BYTE
 void CopyWindow16tm1(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD);
//  Mask Trans Copy   Src   Tag   W-Wide W-High S-Wide T-Wide
// ColorKey=0x0000                U:WORD U:WORD U:WORD U:BYTE
 void CopyWindow16t2(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD);
//   LightPlus Copy  Src   Tag   W-Wide W-High S-Wide T-Wide
//                               U:WORD U:WORD U:WORD U:BYTE
 void CopyWindow16tm2(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD);
//Mask lightPlus Copy Src   Tag   W-Wide W-High S-Wide T-Wide
// ColorKey=0x0000                U:WORD U:WORD U:WORD U:BYTE
 void CopyWindow16tm2db2(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD);
// Mask lightPlus Copy   Src   Tag   W-Wide W-High S-Wide T-Wide
//  ColorKey=0x0000                  U:WORD U:WORD U:WORD U:BYTE
//  Enlarge to 2x
 void CopyWindow16v2(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD,DBWORD);
//                   Src   Tag   W-Wide W-High S-Wide T-Wide Show-Level(0-63)
//                               U:WORD U:WORD U:WORD U:BYTE
 void CopyWindow16db2(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD);
//  Enlarge to 2x     Src   Tag   W-Wide W-High S-Wide T-Wide
//                                U:WORD U:WORD U:WORD U:BYTE
 void CopyWindow16db2v2(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD,DBWORD);
//  Enlarge to 2x       Src   Tag   W-Wide W-High S-Wide T-Wide Show-Level(0-63)
//  with Level show               U:WORD U:WORD U:WORD U:BYTE
 void CopyWindow16db2Smooth(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD);
//  Enlarge to 2x           Src   Tag   W-Wide W-High S-Wide T-Wide
//                                      U:WORD U:WORD U:WORD U:BYTE
 void CopyWindow16db2mt2Smooth(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD);
//  Enlarge to 2x and smooth   Src   Tag   W-Wide W-High S-Wide T-Wide
//                                         U:WORD U:WORD U:WORD U:BYTE
 void CopyWindow16db2SmoothPlus(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD);
//  Enlarge to 2x               Src   Tag   W-Wide W-High S-Wide T-Wide
//  計算更多的消鋸齒                        U:WORD U:WORD U:WORD U:BYTE

 void CopyWindow16Half(void*,void*,DBWORD,DBWORD,DBWORD,DBWORD);
//  Scale to 1/2       Src   Tag   W-Wide W-High S-Wide T-Wide
//                                 U:WORD U:WORD U:WORD U:BYTE


 void ColorBlockB16( DBWORD , DBWORD , DBWORD , DBWORD , void* , DBWORD );
//                        Position  Wide     Height   Color   Buffer  Buffer_Wide

 DBWORD toword(void*);
 void tobytes(DBWORD,void*);
 DBWORD to2word(void*);
 void to2bytes(DBWORD,void*);
 DBWORD to3word(void*);
 void to3bytes(DBWORD,void*);

 void UpperCase0(void*,void*,DBWORD);
//               Sourc Target Leght

 void AdjustDataPosition(void*,void*,DBWORD,DBWORD,DBWORD);
//                               0     1     2      3      4
// 0=Table Pointer  1=Data Pointer
// 2=Table load Size     3=Data load Size
// 4=16-Bits Mode
 void AdjustOnePosition(void*,DBWORD,DBWORD);
//                             0      1      2
// 0=Data Pointer  1=Data load Size   2=16-Bits Mode
 void AdjustData555(void*,void*,DBWORD,DBWORD,DBWORD);
//                  0     1     2      3
// 0=Table Pointer  1=Data Pointer
// 2=Table load Size  3=16-Bits Mode
// 4=Number of be ZERO Palette Exp: 5 = Since 251 to 255 will be ZERO, 0=None.
 void AdjustOneData555(void*,DBWORD,DBWORD);
//                       0      1     2
// 0=Data Pointer  1=16-Bits Mode  2=從後面算起，有多少顏色須變零(黑色)
 void AdjustOneData555_64(void*,DBWORD);
//                          0      1
// 0=Data Pointer  1=16-Bits Mode      for 64 colors



 void Set_HiColors_16bitMode0(DBWORD);
 void Set_Color_datas(void*,void*,void*); // For ------- xxv2 or xxv3
//------------> Needs 4096  49152 12288
//                                ^^^^Can be NULL*

void To_do_AND(void*,void*,DBWORD);
// AND         Buf1  Buf2  SIZE(BYTE)    will be change in Buf1

 void Conv_TGA24_2_PUT16(void*,void*,DBWORD,DBWORD,DBWORD);
//                               0    1      2      3       4
// 0=TGA Source  1=Convert to Buffer
// 2=Red 3=Green 4=Blue => to change same value to Zero(R0,G0,B0).
// TGA file must is 24 bits and no compress!
 DBWORD Get_Pzp_Half_Image0(void*,void*,void*);
//                                  0     1     2
//  0-Source PZP  1=Target PZP Buffer
//  2-Temp. Buffer (Must can allow uncompress size)

 DBWORD How_many1( void* , DBWORD );
//                         0       1
// 0-String Point  1=Size(BYTES)
// Count character number of String.
 Swap_Value0(void*,  DBWORD,    DBWORD,   DBWORD);
//           Pointer Size(BYTE) Value1    Value2
 Change_Value0(void*,  DBWORD,    DBWORD,   DBWORD);
//             Pointer Size(BYTE) Old_Value New_Value
 Change_Value1(void*,  DBWORD,    DBWORD,   DBWORD);
//             Pointer Size(BYTE) Value(!=) New_Value
// Change all '!=' to Special
DBWORD unPzf(void*,void*);
DBWORD doPzf(void*,void*,DBWORD);
DBWORD unPzp(void*,void*);
DBWORD unPzp64(void*,void*);
DBWORD unPzp128(void*,void*);
DBWORD GetZPformat0(void*,  DBWORD);
//                  ZPData  ZP's Size
DBWORD doPzp(void*,     void*,     DBWORD);
//           SrcPointer TagPointer Source Size(Not include W&H 4bytes)
DBWORD doPzp64(void*,     void*,     DBWORD);
//             SrcPointer TagPointer Source Size(Not include W&H 4bytes)
//   Color only in 64 (0-63) !
DBWORD doPzp128(void*,     void*,     DBWORD);
//              SrcPointer TagPointer Source Size(Not include W&H 4bytes)
//   Color only in 128 (0-127) !
DBWORD ScaleD2(void*,void*,DBWORD,DBWORD);
//              0     1      2      3
// 0=Source Pointer(16bits PUT format)
// 1=Targer Pointer
// 2=Wish Wide  3=Wish High
void ScaleD2Putf(DBWORD,DBWORD,void*,DBWORD,DBWORD,void*,DBWORD);
//                X      Y    Source Wish-W Wish-H Buffer  Buffer's wide
void ScaleD2Putm(DBWORD,DBWORD,void*,DBWORD,DBWORD,void*,DBWORD);
// Mask Show      X      Y    Source Wish-W Wish-H Buffer Buffer's wide
// Mask Show, ColorKey=0000
void ScaleD2Putmt1(DBWORD,DBWORD,void*,DBWORD,DBWORD,void*,DBWORD);
// Mask Show      X      Y    Source Wish-W Wish-H   Buffer Buffer's wide
// Trans show
// Mask Show, ColorKey=0000
void ScaleD2Putmt2(DBWORD,DBWORD,void*,DBWORD,DBWORD,void*,DBWORD);
// Mask Show         X      Y    Source Wish-W Wish-H Buffer Buffer's wide
// AddLight Show
// Mask Show, ColorKey=0000
void ScaleD2PutfPart(DBWORD,DBWORD,void*,DBWORD,DBWORD,void*, DBWORD,     DBWORD, DBWORD, DBWORD);
//                    X      Y    Source Wish-W Wish-H Buffer Buffer-wide Scale-W Scale-H Source-Wide

void PutfZP_ScaleD2Offset(DBWORD  ,DBWORD  ,void*                ,DBWORD           ,DBWORD           ,void*           ,DBWORD            );
//                       (DBWORD X,DBWORD Y,void *ZPSourcePointer,DBWORD wish-wide ,DBWORD wish-high ,void *ShowBuffer ,DBWORD Buffer-Wide);
void PutmZP_ScaleD2Offset(DBWORD  ,DBWORD  ,void*                ,DBWORD           ,DBWORD           ,void*           ,DBWORD            );
// 0=ColorKey            (  X     ,  Y     ,ZPSourcePointer,      wish-wide ,       wish-high ,      ShowBuffer ,      Buffer-Wide);
void Putmt1ZP_ScaleD2Offset(DBWORD  ,DBWORD  ,void*                ,DBWORD           ,DBWORD           ,void*           ,DBWORD            );
// 0=ColorKey            (DBWORD X,DBWORD Y,void *ZPSourcePointer,DBWORD wish-wide ,DBWORD wish-high ,void *ShowBuffer ,DBWORD Buffer-Wide);
void Putmt2ZP_ScaleD2Offset(DBWORD  ,DBWORD  ,void*                ,DBWORD           ,DBWORD           ,void*           ,DBWORD            );
// 0=ColorKey            (DBWORD X,DBWORD Y,void *ZPSourcePointer,DBWORD wish-wide ,DBWORD wish-high ,void *ShowBuffer ,DBWORD Buffer-Wide);
void PutfZP_ScaleD2      (DBWORD  ,DBWORD  ,void*                ,DBWORD           ,DBWORD           ,void*           ,DBWORD            );
//                       (DBWORD X,DBWORD Y,void *ZPSourcePointer,DBWORD wish-wide ,DBWORD wish-high ,void *ShowBuffer ,DBWORD Buffer-Wide);
void PutmZP_ScaleD2      (DBWORD  ,DBWORD  ,void*                ,DBWORD           ,DBWORD           ,void*           ,DBWORD            );
// 0=ColorKey            (  X     ,  Y     ,ZPSourcePointer,      wish-wide ,       wish-high ,      ShowBuffer ,      Buffer-Wide);
void Putmt1ZP_ScaleD2      (DBWORD  ,DBWORD  ,void*                ,DBWORD           ,DBWORD           ,void*           ,DBWORD            );
// 0=ColorKey            (DBWORD X,DBWORD Y,void *ZPSourcePointer,DBWORD wish-wide ,DBWORD wish-high ,void *ShowBuffer ,DBWORD Buffer-Wide);
void Putmt2ZP_ScaleD2      (DBWORD  ,DBWORD  ,void*                ,DBWORD           ,DBWORD           ,void*           ,DBWORD            );
// 0=ColorKey            (DBWORD X,DBWORD Y,void *ZPSourcePointer,DBWORD wish-wide ,DBWORD wish-high ,void *ShowBuffer ,DBWORD Buffer-Wide);


DBWORD Raw_to_ZP256OFFSET(void*,         void*,              SWORD,     SWORD,     void*);
//                     SourcePic(PUT) , Palette(768Bytes) , Offset-X , Offset-Y , ZP-Buffer
void   Build_Gray_Shadow(void*,      DBWORD,    void*);
//                   SourcePic(PUT)   Level     Buffer Size same as Source

void Set_AlphaLevelColor_Ptr(void*);

void Convert555to888(void*,void*);

void DeBMP256(void*,void*,void*);
void DeBMP256_24bits(void*,void*);
void DeTGA256(void*,void*,void*);
DBWORD Pcx_Compress256(void*,void*,void*);
//                      0     1     2
//0=Palette Pointer   1=Buffer for store Pcx Data   2=Put Format data






void Set_WorkBuffer_Range( DBWORD,            void* ,             DBWORD , DBWORD );
//                         StoreNumber        Pointer of Buffer   Wide     High
void Select_WR(DBWORD);
//             Number of want to select WorkBuffer


void Adjust_One8Palette( void*, void*, DBWORD);
//                         1      2       3
//1:Table's Pointer ( can be (void*)0 )
//2:Data's Pointer
//3:Table's Size    ( can be zero )

//##################################################################  
//--N8A(No Compresse) Format
//+0   'N@'
//+2   Palette(768)
//+770 Alpha? 1=Yes 0=No
//+771 Compress? 1=Yes 0=No
//+772 - 775 ?
//+776  Offset
//+780  Wide & High
//+784  Graphices Data
//##################################################################
//--N8A(Compressed) Format
//+0   'N@'
//+2   Palette(768)
//+770 Alpha? 1=Yes 0=No
//+771 Compress? 1=Yes 0=No
//+772 - 787 ?
//+788  基線 WORD(x1),WORD(y1)-WORD(x2),WORD(y2)
//+796  Old Offset
//+800  Wide & High
//+804  Graphices Data
//##################################################################
void Put_8b_Nc_Alpha(SWORD, SWORD,    void*);
//                   X(有號) Y(有號)  圖的Pointer


void PutPart_8b_Nc_Alpha(SWORD, SWORD, DBWORD ,DBWORD,DBWORD,DBWORD, void*);
//                       X(有號)Y(有號)   A       B     C      D    圖的Pointer
//A:要取的x（無號數）
//B:要取的y（無號數）
//C:要取的Wide（無號數）
//D:要取的High（無號數）

void PutPart_16B_ByExtraAlpha(SWORD,SWORD,DBWORD,DBWORD,void*,void*,DBWORD);
//                             0     1      2      3     4     5      6
// 0=要放的位置X(有號數）
// 1=要放的位置Y(有號數）
// 2=大圖要取的位置X（無號數）
// 3=大圖要取的位置Y（無號數）
// 4=大圖的Pointer   (16Bits Put Format)
// 5=Alpha圖的Pointer(8Bits Put Format)
// 6= 0=Normal  1=網點Show

void PutN8AScale(DBWORD,DBWORD,void*,DBWORD,DBWORD);
//                 X      Y   Pointer  W      H


void Put_8b_C_Alpha(SWORD, SWORD,    void*);
//                  X(有號) Y(有號)  圖的Pointer
void Put_8b_C_AlphaV2(SWORD, SWORD,    void*,      DBWORD);
//                    X(有號) Y(有號)  圖的Pointer  Level(0-63)



void GetPicf16_2_WorkBuffer(SWORD,SWORD,void*);
//                        0     1     2
// 0=要放的位置X(有號數）
// 1=要放的位置Y(有號數）
// 2=大圖的Pointer (16Bits Put Format)
             
void GetPutf16_2_WorkBuffer( SWORD,SWORD,DBWORD,DBWORD,DBWORD,DBWORD,void* );
//                             0     1     2      3      4      5     6
//0=要放的位置X(有號數）
//1=要放的位置Y(有號數）
//2:要取的x（無號數）
//3:要取的y（無號數）
//4:要取的Wide（無號數）
//5:要取的High（無號數）
//6:16Bits Source Pic (include W&H)
  
void Make_Alpha64LevelTable(void);


  

BYTE Get_Alpha_Value1(void*,      DBWORD,DBWORD);
// 取得alpha值        圖的Pointer    X      Y
  
  

//======== 有裁邊 Serial (含偏移值)
void PutZP_P2pset16p( SWORD , SWORD , void* );
//                    X(有號) Y(有號) 圖的Pointer
void PutZP_P2pset16pV2( SWORD , SWORD , void* ,      DBWORD);
//                      X(有號) Y(有號) 圖的Pointer  Level(0-63)

void PutZP_P2mask16p( SWORD , SWORD , void* );
//                    X(有號) Y(有號) 圖的Pointer
void PutZP_P2mask16pV2( SWORD , SWORD , void* ,      DBWORD);
//                      X(有號) Y(有號) 圖的Pointer  Level(0-63)
void PutZP_P2mask16pV3( SWORD , SWORD , void* ,      DBWORD,      DBWORD,       DWORD);
//                      X(有號) Y(有號) 圖的Pointer  RLevel(0-63) GLevel(0-63)  BLevel(0-63)

void PutZP_P2mask16pt1( SWORD , SWORD , void* );
//                      X(有號) Y(有號) 圖的Pointer
void PutZP_P2mask16pt1V2( SWORD , SWORD , void* ,    DBWORD);
//                      X(有號) Y(有號) 圖的Pointer  Level(0-63)
void PutZP_P2mask16pt1V3( SWORD , SWORD , void* ,      DBWORD,      DBWORD,       DWORD);
//                        X(有號) Y(有號) 圖的Pointer  RLevel(0-63) GLevel(0-63)  BLevel(0-63)

void PutZP_P2mask16pt2( SWORD , SWORD , void* );
//                      X(有號) Y(有號) 圖的Pointer
void PutZP_P2mask16pt2V2( SWORD , SWORD , void* ,    DBWORD);
//                      X(有號) Y(有號) 圖的Pointer  Level(0-63)
void PutZP_P2mask16pt2V3( SWORD , SWORD , void* ,      DBWORD,      DBWORD,       DWORD);
//                        X(有號) Y(有號) 圖的Pointer  RLevel(0-63) GLevel(0-63)  BLevel(0-63)

void PutZP_P2mask16pX2( SWORD , SWORD , void* );
//                      X(有號) Y(有號) 圖的Pointer
  
  
  
  
}


