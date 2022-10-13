#ifndef VMCRENDERDEF_H
#define VMCRENDERDEF_H


/*
typedef int DeviceTypeId;
#define     _VMCTYPEID_2DDX  1
#define     _VMCTYPEID_2DGDI 2
#define     _VMCTYPEID_2DVL  4
#define     _VMCTYPEID_3DGL  11
*/


enum DeviceTypeId
{
    _VMCTYPEID_2DDX  = 1,
    _VMCTYPEID_2DGDI = 2,
    _VMCTYPEID_2DVL  = 4,
    _VMCTYPEID_3DGL  = 11,
};


// Definations of set in CreateFont() of Text
enum RENDER_FONTSET
{
    RENDER_FONTSET_SBCS =  0,
    RENDER_FONTSET_DBCS =  1,
};

// Definations of fmt in CreateFont() of Text
enum RENDER_FONTFMT
{
    RENDER_FONTFMT_0  =  0,   // font format 0 (bit font)   
    RENDER_FONTFMT_1  =  1,   // font format 1 (smooth font)
};




#endif //RENDERDEF_H