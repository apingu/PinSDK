#ifndef _VMCGLOBAL
#define _VMCGLOBAL

#include <pf-def.h>
#include <set>

extern "C"
{
    __SLAPI std::set<CPIcon*>* GetIconList(void);
    __SLAPI PixelFormat           GetPixelFmt(void);
    __SLAPI void               SetPixelFmt(PixelFormat fmt);
}

#endif