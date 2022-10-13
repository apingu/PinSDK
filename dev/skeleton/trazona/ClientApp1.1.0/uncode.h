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
//
int atow(char *strA, wchar_t *strW, int lenW)
{
  int len;
  char *pA;
  wchar_t *pW;

  // Start with len=1, not len=0, as string length returned
  // must include null terminator, as in MultiByteToWideChar()
  for(pA = strA, pW = strW, len = 1; lenW; pA++, pW++, lenW--, len++)
  {
    *pW = (lenW == 1) ? 0 : (wchar_t)(*pA);
    if(!(*pW)) break;
  }
  return len;
}

/*****************************************************************************/
int wtoa(wchar_t *strW, char *strA, int lenA)
{
   int len;
   char *pA;
   wchar_t *pW;

   /*
      Start with len=1, not len=0, as string length returned
      must include null terminator, as in WideCharToMultiByte()
   */
   for (pA=strA, pW=strW, len=1; lenA; pA++, pW++, lenA--, len++)
   {
      *pA = (lenA == 1) ? 0 : (char)(*pW);
      if (!(*pA))
         break;
   }
   return len;
}

/*****************************************************************************/
float wtof(wchar_t *strW)
{
  float rval = 0.0f;


  return rval;
}
