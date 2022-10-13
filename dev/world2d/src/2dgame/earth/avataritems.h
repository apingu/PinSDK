// Super3TJ generated struct defination here, don't modify any comments plz! -OB
#ifndef AVATARITEMS_H
#define AVATARITEMS_H

#ifdef _WINDOWS
#pragma pack ( push, 1 )
#else
__attribute__ ( ( aligned ( 1 ) ) )
#endif

typedef struct _tagMyAvataritems    // 36 Bytes!
{
    longlong_t   id;                  // +0   0~0 BASE0 W50 RGB(F0F0FF) F0 編號
    long       weight;              // +8   0~0 BASE0 W50 RGB(FFFFFF) F0 加權
    char       direction;           // +12  0~0 BASE0 W50 RGB(F0F0FF) F0 方向
    char       layer;               // +13  0~0 BASE0 W50 RGB(FFFFFF) F0 層級
    bool       fixed;               // +14  0~0 BASE0 W50 RGB(FFFFFF) F0 層級
    char       reserved;            // +15  0~0 BASE0 W50 RGB(FFFFFF) F0 保留
    long       visible;             // +16  0~0 BASE0 W80 RGB(F0F0FF) F0 是否可見
    bool       switcher[16];        // +20  0~0 BASE0 W50 RGB(FFFFFF) F0 選擇
}Avataritems;

#ifdef _WINDOWS
#pragma pack ( pop )
#else
__attribute__ ( ( packed ) )
#endif


#endif //AVATARITEMS_H
// Default make 10000+ Records
// Super3TJ generated struct defination end.
