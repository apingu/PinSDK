#ifndef _GRASP_H_
#define _GRASP_H_

extern int    nUnpackSize;
extern char  *pUnpack;
extern int    nPackSize;
extern char  *pPack;

void ReallocFontBuff(int width, int height, int smooth);
void FreeFontBuff(void);
void GraspFontToBuff(int offsetX, int offsetY);

#endif  // _GRASP_H_