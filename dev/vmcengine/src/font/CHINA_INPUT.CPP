#define	C_SIZE	12
#define	E_SIZE	6

char	STR[82];
int	Str_X	     = 10;
int	Str_Y	     = 480-32;
int	Font_Size    = E_SIZE;
int	KEY_IN_BYTES = 80;
int	KEY_IN_FG    = 1;  // 0	: NO Key input
			   // 1	: Normal
			   // 2	: Key input password

char	*STR_ADDR = STR;
int	STR_PT	  = 0;

char	Layout_str[81];
char	Comp_str[31];
char	Finsh_str[81];
char	Select_str[41];
BYTE	ASC_fg = 0;

BYTE	CHINA_INPUT_FG	     = 0;
BYTE	CHINA_SELECT_LIST_FG = 0;
int	Word_base_count	     = 0;
int	BK_Word_base_count   = 0;
///////////////////////////////////////////////////////////////////////////////

int	IMM_Check(char *str,int	str_pt,int str_length)
{
	HKL hkbs[25];
	// Get now Chinese Layout HKB
	HKL hkb	 = GetKeyboardLayout(0);
	// Get All Chinese Layout HKB
	int Layouts = GetKeyboardLayoutList( 25, &hkbs[0] );

	char Buff[80];
	char RetStr[40];
	int  RetCount;


	for(int	i=0; i<	Layouts; i++)
	{

		if( ImmIsIME(hkbs[i]) == 1)
		{

			CHINA_INPUT_FG = 1;
			MMX_ZeroMemory(Buff,sizeof(Buff));
			MMX_ZeroMemory(RetStr,sizeof(RetStr));

			// Get now Chinese Layout's String
			RetCount = ImmGetDescription( hkbs[i], Buff, 80);

			for(int	i=0;i<80;i++)
			{
			  RetStr[i] = Buff[i];
			  WORD *p  = (WORD *)&Buff[i];
			  WORD w = *p;

			  __asm	{
				  push	  ax
				  mov	  ax,w
				  xchg	  ah,al
				  mov	  w,ax
				  pop	  ax
				}

			  if(w == '法')
			  {
			    RetStr[i+1]	= Buff[i+1];
			    break;
			  }

			}
		}
		else
		{
			CHINA_INPUT_FG = 0;
			MMX_ZeroMemory(RetStr,sizeof(RetStr));
			Take_name( RetStr, "English (American)"	);

			MMX_ZeroMemory(Comp_str,sizeof(Comp_str));
			MMX_ZeroMemory(Select_str,sizeof(Select_str));

		}

		if( hkb	== hkbs[i])
		{
			MMX_ZeroMemory(Layout_str,sizeof(Layout_str));
			Take_name( Layout_str ,RetStr );
			break;
		}
	}

	////////////////////////////////////////////////////////////////////
	//ActivateKeyboardLayout(hkb,0);
	if( CHINA_INPUT_FG == 1)
	{

	    HIMC hIMC =	ImmGetContext(h_wnd);

	    // Get Word	base
	    MMX_ZeroMemory(Comp_str,sizeof(Comp_str));
	    BK_Word_base_count = Word_base_count;
	    Word_base_count = ImmGetCompositionString(hIMC,GCS_COMPSTR,Comp_str,30);


	    char *imc_buf;
	    DWORD dwSize;
	    CANDIDATELIST *lpCandList;

	    if (dwSize = ImmGetCandidateList(hIMC,0x0,NULL,0))
	    {

		CHINA_SELECT_LIST_FG = 1;
		imc_buf	=(char *)malloc( dwSize);
		lpCandList = (CANDIDATELIST *)&imc_buf[0];

		ImmGetCandidateList(hIMC,0x0,lpCandList,dwSize);

		if( lpCandList->dwCount	> 0 )
		{
		     int	 i;
		     char	 tab[10][2] = {	"1"
					       ,"2"
					       ,"3"
					       ,"4"
					       ,"5"
					       ,"6"
					       ,"7"
					       ,"8"
					       ,"9"
					       ,"0"};

		     if( !lstrcmpi( "新微軟注音輸入法",	RetStr ) )
		     {}
		     if( !lstrcmpi( "新微軟倉頡輸入法",	RetStr ) )
		     {}
		     if( !lstrcmpi( "注音輸入法", RetStr )	 )
		     {}
		     if( !lstrcmpi( "倉頡輸入法", RetStr )	 )
		     {}
		     if( !lstrcmpi( "嘸蝦米輸入法", RetStr )	 )
		     {
			 tab[0][0] = '0';
			 tab[1][0] = '1';
			 tab[2][0] = '2';
			 tab[3][0] = '3';
			 tab[4][0] = '4';
			 tab[5][0] = '5';
			 tab[6][0] = '6';
			 tab[7][0] = '7';
			 tab[8][0] = '8';
			 tab[9][0] = '9';
		     }
		     if( !lstrcmpi( "大易輸入法", RetStr )	 )
		     {
			 tab[0][0] = '0';
			 tab[1][0] = '1';
			 tab[2][0] = '2';
			 tab[3][0] = '3';
			 tab[4][0] = '4';
			 tab[5][0] = '5';
			 tab[6][0] = '6';
			 tab[7][0] = '7';
			 tab[8][0] = '8';
			 tab[9][0] = '9';
		     }
		     if( !lstrcmpi( "內碼輸入法", RetStr )	 )
		     {}
		     if( !lstrcmpi( "行列輸入法", RetStr )	 )
		     {}
		     if( !lstrcmpi( "速成輸入法", RetStr )	 )
		     {}
		     if( !lstrcmpi( "英數輸入法", RetStr )	 )
		     {}
		     if( !lstrcmpi( "自然輸入法", RetStr )	 )
		     {}

		     CString string;
		     CString str[10];
		     int j = lpCandList->dwCount - lpCandList->dwPageStart;
		     int k = lpCandList->dwPageSize;
		     if( j > k)	j = k;

		     for(i=0 ;i < j; i++ )
		     {
				str[i]	= &imc_buf[ lpCandList->dwOffset[lpCandList->dwPageStart + i] ];
		     }

		     for(i=0 ;i < j; i++ )
		     {
			 string	+= tab[i] +  str[i] + "　";
		     }

		     MMX_ZeroMemory(Select_str,sizeof(Select_str));

		     if(sizeof(string) > 80 )
		       memcpy(Select_str,string,80);
		     else
		       Take_name(Select_str,string);

		}
		else
		{
		    CHINA_SELECT_LIST_FG = 0;
		    MMX_ZeroMemory(Select_str,sizeof(Select_str));
		}


		free(imc_buf );

	    }
	    else
	    {
		CHINA_SELECT_LIST_FG = 0;
		MMX_ZeroMemory(Select_str,sizeof(Select_str));
	    }




	    // Get Finsh Chinese words
	    Bug_count1 = BK_Word_base_count;
	    Bug_count2 = Word_base_count;



	    char    Finsh_str2[81];
	    MMX_ZeroMemory( Finsh_str2,	sizeof(Finsh_str2) );
	    ImmGetCompositionString(hIMC,GCS_RESULTSTR,Finsh_str2,80);


	    if(
		  ( BK_Word_base_count != Word_base_count && Word_base_count ==	0 )
	       || (lstrcmpi( Finsh_str2, Finsh_str ))
	    )
	    {
		MMX_ZeroMemory(	Finsh_str, sizeof(Finsh_str) );
		int Finsh_str_count = ImmGetCompositionString(hIMC,GCS_RESULTSTR,Finsh_str,80);

		if( Finsh_str_count >= 2 )
		{
		    for(int i=0; i<80 ;i++)
		    {
			 if(  str_pt >=	str_length)
			 {
			    str_pt = str_length	- 1;
			    break;
			 }

			 if( Finsh_str[i] == 0)
			 {
			    break;
			 }

			 if( str[ str_pt ] != 0	)  // Ins char
			 {
			     int j;
			     for(j = str_length-1 ;j > str_pt  ;j--)
			     {
			       str[ j ]	= str[ j-1 ];
			     }

			     str[ str_pt] = Finsh_str[i];
			 }
			 else
			 {
			     str[ str_pt] = Finsh_str[i];
			 }

			 str_pt++;
		    }
		}
	    }


	    ImmReleaseContext(h_wnd,hIMC);
	}
	else
	{
	    MMX_ZeroMemory(Comp_str,sizeof(Comp_str));
	    MMX_ZeroMemory(Select_str,sizeof(Select_str));
	}



	return str_pt;

}
///////////////////////////////////////////////////////////////
int  Key_In(char *str,int str_pt,int str_length,BYTE key_in_flag)
{
	if( CHINA_INPUT_FG == 0	)
	{
		if( Key_[VK_BACK] == 1 )  //  Backspace
		{
		  int i=0;
		  str_pt--;
		  if( str_pt < 0) str_pt = 0;
		  for(i=str_pt;	i<str_length ;i++)
		  {
		      str[ i ] = str[ i+1 ];
		  }

		  if( str[ str_pt - 1 ]	& 128 )
		  {
		      str_pt--;
		      if( str_pt < 0) str_pt = 0;
		      for(i=str_pt; i<str_length ;i++)
		      {
			str[ i ] = str[	i+1 ];
		      }
		  }


		  ASC_fg = 0;
		  return str_pt;
		}

		if( Key_[VK_LEFT] == 1 )  //  Left
		{
		  str_pt--;
		  if( str_pt < 0) str_pt = 0;
		  if( str[ str_pt - 1 ]	& 128 )
		  {
		      str_pt--;
		      if( str_pt < 0) str_pt = 0;
		  }
		  ASC_fg = 0;
		  return str_pt;
		}

		if( Key_[VK_RIGHT] == 1	)  //  Right
		{
		  if( str[ str_pt ] & 128)
		  {
		    str_pt++;
		    int	 p1 = str_pt;
		    if(	p1 > 0)	p1 = p1-1;
		    if(	str[ p1	] == 0	) str_pt--;
		  }

		  str_pt++;

		  int  p = str_pt;
		  if( p	> 0) p = p-1;
		  if( str[ p ] == 0  ) str_pt--;


		  ASC_fg = 0;
		  return str_pt;
		}

		if(  0 > GetKeyState(VK_CONTROL) )
		{
		    ASC_fg = 0;
		    KeyAscii = 0;
		}


		if( ASC_fg == 1	)
		{
		    if(	   KeyAscii >= 32
			&& Key_[VK_LEFT] == 0
			&& Key_[VK_RIGHT] == 0
			&& Key_[VK_UP] == 0
			&& Key_[VK_DOWN] == 0
		    )
		    {
			if( str[ str_pt	] != 0 )  // Ins char
			{
			    for(int i=str_length-1 ;i >	str_pt	;i--)
			    {
			      str[ i ] = str[ i-1 ];
			    }
			      str[ str_pt ] = KeyAscii;
			}
			else
			{
			      str[ str_pt ] = KeyAscii;
			}

			str_pt++;
			if(  str_pt >= str_length) str_pt = str_length - 1;
			ASC_fg = 0;
			return str_pt;
		    }
		}
		ASC_fg = 0;
	}
	else
	{
	    if(	key_in_flag == 0)
	    {
		if( Key_[VK_BACK] == 1 )  //  Backspace
		{
		  int i=0;
		  str_pt--;
		  if( str_pt < 0) str_pt = 0;
		  for(i=str_pt;	i<str_length ;i++)
		  {
		    str[ i ] = str[ i+1	];
		  }
		  if( str[ str_pt - 1 ]	& 128 )
		  {
		      str_pt--;
		      if( str_pt < 0) str_pt = 0;
		      for(i=str_pt; i<str_length ;i++)
		      {
			str[ i ] = str[	i+1 ];
		      }
		  }
		  ASC_fg = 0;
		  return str_pt;
		}

		if( Key_[VK_LEFT] == 1 )  //  Left
		{
		  str_pt--;
		  if( str_pt < 0) str_pt = 0;
		  if( str[ str_pt - 1 ]	& 128 )
		  {
		      str_pt--;
		      if( str_pt < 0) str_pt = 0;
		  }

		  ASC_fg = 0;
		  return str_pt;
		}

		if( Key_[VK_RIGHT] == 1	)  //  Right
		{
		  if( str[ str_pt ] & 128)
		  {
		    str_pt++;
		    int	 p1 = str_pt;
		    if(	p1 > 0)	p1 = p1-1;
		    if(	str[ p1	] == 0	) str_pt--;
		  }
		  str_pt++;
		  int  p = str_pt;
		  if( p	> 0) p = p-1;
		  if( str[ p ] == 0  ) str_pt--;
		  ASC_fg = 0;
		  return str_pt;
		}
	    }
	}

	return str_pt;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Show_Key_In(char *str, int	str_pt , int str_length,HFONT font, int	x, int y, int font_size,BYTE key_in_fg)
{
	int len=0;
	char buf[256];



	if( key_in_fg == 2)
	{
	  char	str2[81];
	  MMX_ZeroMemory(str2,81);

	  for(char i=0;i<80;i++)
	  {
	    if(str[i] == 0)
	    {
		len=sprintf( buf, str2);
		break;
	    }
	    str2[i] = '*';
	  }

	}
	else
	  len=sprintf( buf, str);

	Show_String1( buf, x, y, ARGB(0,9,9,155), (WORD	*)pBuf,	12, 6 );
	Show_String1( buf, x+1,	y+1, ARGB(0,255,255,255), (WORD	*)pBuf,	12, 6 );


	if( key_in_fg >	0 )
	{
	    char	cursor_[2] = "-";
	    static BYTE	key_on_off = 0;
	    x += ( font_size + 1 ) * str_pt;

	    if(	Time_move[6] > 0 )
	    {
	      key_on_off++;
	      if( key_on_off > 1 ) key_on_off =	0;
	    }

	    if(	key_on_off == 1)
	    {
	      len=sprintf( buf,	cursor_);

	      y	+= 5;
	      Show_String1( buf, x, y, ARGB(0,9,9,155),	(WORD *)pBuf, 12, 6 );
	      Show_String1( buf, x+1, y+1, ARGB(0,255,255,255),	(WORD *)pBuf, 12, 6 );
	    }




	    // China Input Way
	    y =	480 - (	C_SIZE + 4 );
	    len=sprintf( buf, "%s ",Layout_str);
	    x =	640 - (	E_SIZE + 1) * len - 2;

	    Show_String1( buf, x, y, ARGB(0,9,9,9), (WORD *)pBuf, 12, 6	);
	    Show_String1( buf, x+1, y+1, ARGB(0,255,255,255), (WORD *)pBuf, 12,	6 );


	    if(	CHINA_SELECT_LIST_FG > 0 )
	    {
		// Select String

		y = 480	- ( C_SIZE + 4 );
		x = 2;
		len=sprintf( buf, "%s ",Select_str);

		if( len	> 1)
		{
		    Show_String1( buf, x, y, ARGB(0,9,9,155), (WORD *)pBuf, 12,	6 );
		    Show_String1( buf, x+1, y+1, ARGB(0,255,255,255), (WORD *)pBuf, 12,	6 );
		}

	    }
	    else
	    {
		// Word	Base

		y = 480	- ( C_SIZE + 4 );
		x = 2;
		len=sprintf( buf, "%s ",Comp_str);
		if( len	> 1)
		{
		    Show_String1( buf, x, y, ARGB(0,9,9,155), (WORD *)pBuf, 12,	6 );
		    Show_String1( buf, x+1, y+1, ARGB(0,255,255,255), (WORD *)pBuf, 12,	6 );
		}
	    }

	}



}

///////////////////////////////////////////////////////////////

void Show_Key_Text(char	*str,HFONT font, int x,	int y, int font_size, BYTE key_in_now, BYTE password_fg)
{
	int len	= 0;
	char buf[256];

	if( key_in_now == 0 )
	{

	    if(	password_fg == 1)
	    {
	      char  str2[81];
	      MMX_ZeroMemory(str2,81);

	      for(char i=0;i<80;i++)
	      {
		if(str[i] == 0)
		{
		    len=sprintf( buf, str2);
		    break;
		}

		str2[i]	= '*';
	      }

	    }
	    else
	      len=sprintf( buf,	str);

	    Show_String1( buf,	 x,   y,     ARGB(0,9,9,155), (WORD *)pBuf, 12,	6 );
	    Show_String1( buf, x+1, y+1, ARGB(0,255,255,255), (WORD *)pBuf, 12,	6 );

	}


}
///////////////////////////////////////////////////////////////
