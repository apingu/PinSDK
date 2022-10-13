
#define WSA_ACCEPT   (WM_USER + 0)
#define	WSA_READ     (WM_USER +	1)




//--------------------------------------------------------------------------------

#pragma	comment	(lib,"Wsock32.lib")
#pragma	comment	(lib,"Version.lib")
#pragma	comment	(lib,"Ctl3d32.lib")
#pragma	comment	(lib,"Ctl3d32s.lib")
#pragma	comment	(lib,"LIBCMT.LIB")
//#pragma comment (lib,"MSVCRT.LIB")
 
//--------------------------------------------------------------------------------

//HANDLE hInst;

SOCKET	sock;

int	status;
int	Wait_Read_Count	= -1;
BOOL	On_Line_fg = FALSE;

//u_short Port_number =	2000;
//char	  IP_address[ 80 ] = "203.79.249.100";
//char	  User_name[ 80	]  = "12";
//char	  Password[ 80 ]   = "12";

DWORD	TS_bk_time;
DWORD	Lan_time;



#define	MAX_PENDING_CONNECTS 4	  /* The backlog allowed for listen() */
#define	NO_FLAGS_SET	     0	  /* Used with recv()/send()	      */
#define	MY_MSG_LENGTH	    512	  /* msg buffer	sent back and forth   */


//-----------------------------------------------------------------------------

BOOL FillAddr( HWND hWnd, PSOCKADDR_IN psin)
{
	PHOSTENT phe;
	char szTemp[200];

	psin->sin_family = AF_INET;

	phe = gethostbyname(IP_address);

	if (phe	== NULL)
	{
	   sprintf(szTemp, "%d is the error. Make sure '%s' is listed in the hosts file.", WSAGetLastError(), IP_address);

	   MessageBox(hWnd, szTemp, "gethostbyname() failed.", MB_OK);
	   return FALSE;
	}

	memcpy((char FAR *)&(psin->sin_addr), phe->h_addr, phe->h_length);

	psin->sin_port = htons(Port_number);

	return TRUE;
}


//-----------------------------------------------------------------------------
BOOL Game_Connect(HWND hWnd)
{
	WSADATA	WSAData;
	char szTemp[80];

	if ((status = WSAStartup(MAKEWORD(1,1),	&WSAData)) == 0)
	{
	   MessageBox( hWnd, WSAData.szDescription, WSAData.szSystemStatus, MB_OK);
	}
	else {
	   sprintf(szTemp, "%d is the err", status);
	   MessageBox( hWnd, szTemp, "Error", MB_OK);
	   return FALSE;
	}



	SOCKADDR_IN dest_sin;

	sock = socket( AF_INET,	SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
	   MessageBox(hWnd, "socket() failed", "Error",	MB_OK);
	   return FALSE;
	}

	if (!FillAddr( hWnd, &dest_sin)) {
	   closesocket(	sock );
	   return FALSE;
	}


	if (connect( sock, (PSOCKADDR) &dest_sin, sizeof( dest_sin)) < 0)
	{
	   closesocket(	sock );
	   MessageBox(hWnd, "connect() failed",	"Error", MB_OK);
	   return FALSE;
	}


//	MessageBox(hWnd, "connect() worked!", "Success!", MB_OK);
//	SetWindowText( hWnd, "Connected");


	if ((status = WSAAsyncSelect( sock, hWnd, WSA_READ, FD_READ | FD_CLOSE )) > 0)
	{
	   wsprintf(IP_address,	"%d (0x%x)");
	   MessageBox( hWnd, "Error on WSAAsyncSelect()", IP_address, MB_OK);
	   closesocket(	sock );
	}


	return TRUE;

}
//-----------------------------------------------------------------------------
void Destroy_WSOCK()
{
	if( On_Line_fg == TRUE )
	{

	    Man_Link* p	= Find_Man_num(	Lan_My_number );
	    if(	p != NULL)
	    {
		if( p->m.fg > 0	)
		{
		    Send_Data("ED|",4);
		    On_Line_fg = FALSE;
		    Flip_To_Page1();
		    MessageBox(	h_wnd, "Connection End", "End",	MB_OK);
		}
	    }

	    WSACleanup();
	    First_lan =	0;

	}
	On_Line_fg = FALSE;
}
//-----------------------------------------------------------------------------
void Send_Data(char *buf,int buf_size)
{
	int iSentLength;

	if( On_Line_fg == TRUE)
	{
//	  send(sock, buf, buf_size, NO_FLAGS_SET );

	  do {
		  iSentLength =	send(sock, buf,	buf_size, NO_FLAGS_SET);
		  if (iSentLength != SOCKET_ERROR)
		  {
			  buf += iSentLength;
			  buf_size -= iSentLength;
		  }
	  } while (iSentLength != SOCKET_ERROR &&
		  buf_size > 0);

	  Wait_Read_Count = 0;
	}
}
//-----------------------------------------------------------------------------
void Read_Data(	HWND hWnd, WPARAM wParam, LPARAM lParam	)
{
	Wait_Read_Count	= -1;

	char szTemp[ MY_MSG_LENGTH ];
	int  buf_size;

	if (WSAGETSELECTEVENT(lParam) == FD_READ)
	{
	   buf_size = recv((SOCKET)wParam, szTemp, MY_MSG_LENGTH, NO_FLAGS_SET );

	   if (buf_size)
	   {
	      szTemp[ buf_size ] = '\0';
	      Check_Read_Data(szTemp,buf_size);
	   }
	   else
	   {
	      Flip_To_Page1();
	      MessageBox( hWnd,	"Connection broken", "Error", MB_OK);
	      Destroy_WSOCK();
	   }
	}
	else
	{
	   Flip_To_Page1();
	   MessageBox( hWnd, "Connection lost",	"WSA_READ", MB_OK);
	   Destroy_WSOCK();
	}
}
//-----------------------------------------------------------------------------
int	copy_command(char* d_buf,char* s_buf,int d_buf_size,int	seek)
{
	int i=0;
	MMX_ZeroMemory(d_buf,d_buf_size);
	while(1)
	{
	   if(s_buf[seek] == '|') break;
	   if( i > d_buf_size )	return -1;

	   d_buf[i] = s_buf[seek];

	   seek++;i++;
	}
	return seek;
}

//-----------------------------------------------------------------------------
BOOL Check_Read_Data(char* buf ,int buf_size)
{
	int	i=0;
	char	Command[30];



	while(i<buf_size)
	{
	    if(	i < 0 )	return FALSE;

	    if(	buf[i] == '|' )
	    {
	       MMX_ZeroMemory(Command,30);
	       i++;

	       i = copy_command(Command,buf, 30, i );
	       if( i ==	-1)
		 return	FALSE;

	    }
	    else
	      break;


      ///////////////////////////////////////////////////////

	    if(	!lstrcmpi( Command,"L1") )
	    {
		i++;
		char	My_num_str[10];
	

		i = copy_command( My_num_str,buf, 10, i	);
		if( i == -1)
		  return FALSE;

		// string TO int
		Lan_My_number =	atoi((const char *)My_num_str);

		CString	str;
		// int TO string
		MMX_ZeroMemory(	My_num_str,10);
		_itoa( Lan_My_number,My_num_str,10);

		CString	str1 = My_num_str;
		CString	str2 = User_name;
		CString	str3 = Password;
		str += "LG|" + str1  + "|" + str2 + "|"	+ str3 + "|";

		char szBuff[256];
		Take_name(szBuff ,str);

		Send_Data(szBuff, strlen(szBuff));


	    }

      ///////////////////////////////////////////////////////

	    if(	!lstrcmpi( Command,"LG") )
	    {
		i++;

		Man_Link* p = Find_Man_num( Lan_My_number );
		if( p != NULL )
		{
		    i =	copy_command( p->m.Name,buf, 80, i );
		    if(	i == -1)
		      return FALSE;

		    if(	!lstrcmpi( p->m.Name,"*") )
		    {
		       Flip_To_Page1();
		       Destroy_WSOCK();

		       Flip_To_Page1();
		       MessageBox( h_wnd, " Password or	UserID Error !", "WSA_READ LG",	MB_OK);
		       return FALSE;
		    }
		}


	    }

      ///////////////////////////////////////////////////////

	    if(	!lstrcmpi( Command,"S1") )
	    {
		int k;
		char num_str[12];
		char fg[6];
		char x[12];
		char y[12];
		char UDLR[6];
		char Command[6];
		char Name[80];


		MMX_ZeroMemory(	fg     ,sizeof(fg));
		MMX_ZeroMemory(	x,sizeof(x));
		MMX_ZeroMemory(	y,sizeof(y));
		MMX_ZeroMemory(	UDLR,sizeof(UDLR));
		MMX_ZeroMemory(	Command,sizeof(Command));
		MMX_ZeroMemory(	Name,sizeof(Name));


		i++;
		i = copy_command( num_str,buf, 12, i );
		if( i == -1)
		  return FALSE;
		i++;

		i = copy_command( fg,buf, 6, i );
		if( i == -1)
		  return FALSE;
		i++;

		i = copy_command( x,buf, 12, i );
		if( i == -1)
		  return FALSE;
		i++;

		i = copy_command( y,buf, 12, i );
		if( i == -1)
		  return FALSE;
		i++;

		i = copy_command( UDLR,buf, 12,	i );
		if( i == -1)
		  return FALSE;
		i++;

		i = copy_command( Command,buf, 6, i );
		if( i == -1)
		  return FALSE;
		i++;

		i = copy_command( Name,buf, 80,	i );
		if( i == -1)
		  return FALSE;
//		i++;


		// String TO int

		int num	= atoi(	(const char *) num_str );  // num    = man number


		Man_Link* p = Find_Man_num( num	);
		if( p != NULL )
		{

		    p->m.fg = 1;

		    if(	First_lan > 0 )
		    {
			for(int	j=0;j<80;j++)
			{
			    p->m.Name[j] = Name[j];
			}

			p->m.x = atoi( (const  char *)	 x );
			p->m.y = atoi( (const  char *)	 y );
			p->m.UDLR = atoi( (const  char *)   UDLR );

			k = atoi( (const char *)  Command );
			p->m.Command = (WORD)k;

			p->m.tgt_x = p->m.x;
			p->m.tgt_y = p->m.y;
		    }
		}



		if(First_lan ==	0)
		{
		    Command_S1();
		    First_lan++;
		}

	    }

//Flip_To_Page1();
//MessageBox( h_wnd, "S1 SEND",	"WSA_READ", MB_OK);
      ///////////////////////////////////////////////////////

	    if(	!lstrcmpi( Command,"SD") )
	    {
		int j,k;
		char num_str[12];
		char x[12];
		char y[12];
		char UDLR[6];
		char Command[6];

		MMX_ZeroMemory(	num_str,sizeof(num_str));
		MMX_ZeroMemory(	x,sizeof(x));
		MMX_ZeroMemory(	y,sizeof(y));
		MMX_ZeroMemory(	UDLR,sizeof(UDLR));
		MMX_ZeroMemory(	Command,sizeof(Command));

		i++;

		i = copy_command( num_str,buf, 12, i );
		if( i == -1)
		  return FALSE;
		i++;


		i = copy_command( x,buf, 12, i );
		if( i == -1)
		  return FALSE;
		i++;

		i = copy_command( y,buf, 12, i );
		if( i == -1)
		  return FALSE;
		i++;

		i = copy_command( UDLR,buf, 12,	i );
		if( i == -1)
		  return FALSE;
		i++;

		i = copy_command( Command,buf, 6, i );
		if( i == -1)
		  return FALSE;
//		i++;


		// String TO int

		j = atoi( (const char *) num_str );  //	j    = man number


		Man_Link* p = Find_Man_num( j );
		if( p != NULL )
		{
		    p->m.tgt_x = atoi( (const  char *)	 x );
		    p->m.tgt_y = atoi( (const  char *)	 y );
		    p->m.UDLR =	atoi( (const  char *)	UDLR );

		    k =	atoi( (const char *)  Command );
		    p->m.Command = (WORD)k;
		}

	    }

      ///////////////////////////////////////////////////////
	    if(	!lstrcmpi( Command,"ED") )
	    {
		char fg[6];

		i++;
		i = copy_command( fg, buf, 6, i	);
		if( i == -1)
		  return FALSE;

		int k =	atoi( (const char *)  fg    );

		Man_Link* p = Find_Man_num( k );
		if( p != NULL )
		{
		    p->m.fg = 0;
		 }

	    }
      ///////////////////////////////////////////////////////
	    if(	!lstrcmpi( Command,"OK") )
	    {
	    }
      ///////////////////////////////////////////////////////
	    if(	!lstrcmpi( Command,"TS") )
	    {
		char	Test_data[48];

		i++;
		i = copy_command( Test_data, buf, 48, i	);
		if( i == -1)
		  return FALSE;

		Send_Data("TT|",4);

	    }
      ///////////////////////////////////////////////////////
	    if(	!lstrcmpi( Command,"TA") )
	    {
		Tk_data[19] = Tk_data[18];
		Tk_data[18] = Tk_data[17];
		Tk_data[17] = Tk_data[16];
		Tk_data[16] = Tk_data[15];
		Tk_data[15] = Tk_data[14];
		Tk_data[14] = Tk_data[13];
		Tk_data[13] = Tk_data[12];
		Tk_data[12] = Tk_data[11];
		Tk_data[11] = Tk_data[10];
		Tk_data[10] = Tk_data[ 9];
		Tk_data[ 9] = Tk_data[ 8];
		Tk_data[ 8] = Tk_data[ 7];
		Tk_data[ 7] = Tk_data[ 6];
		Tk_data[ 6] = Tk_data[ 5];
		Tk_data[ 5] = Tk_data[ 4];
		Tk_data[ 4] = Tk_data[ 3];
		Tk_data[ 3] = Tk_data[ 2];
		Tk_data[ 2] = Tk_data[ 1];
		Tk_data[ 1] = Tk_data[ 0];

		i++;
		i = copy_command( Tk_data[0].Txt, buf, 80, i );
		if( i == -1)
		  return FALSE;


		Tk_data[0].count = 10;
	    }
      ///////////////////////////////////////////////////////
	    if(	!lstrcmpi( Command,"TT") )
	    {
		Lan_time = GetTickCount() - TS_bk_time;
	    }
      ///////////////////////////////////////////////////////
	    if(	!lstrcmpi( Command,"TI") )
	    {
		char t[24];

		MMX_ZeroMemory(	t,sizeof(t));

		_i64toa( Lan_time,t , 10 );

		CString	    str	= t;

		Send_Data( t,	str.GetLength());


	    }
      ///////////////////////////////////////////////////////




	}

      ///////////////////////////////////////////////////////
	Send_Data("OK|",4);

	return TRUE;
}

//-----------------------------------------------------------------------------
//	Send My	ALL data To Server
//-----------------------------------------------------------------------------
void	Command_S1()
{
	char fg[6];
	char x[12];
	char y[12];
	char UDLR[6];
	char Command[6];

	// If first Lan	then set basic data
	Man_Link* p = Find_Man_num( Lan_My_number );
	if( p != NULL )
	{
	    p->m.fg = 1;
	    if(	First_lan == 0 )
	    {
		Man_Link* p2 = Find_Man_num( 1 );
		if( p2 != NULL )
		{
		    p->m.x  = p2->m.x;
		    p->m.y  = p2->m.y;
		    p->m.UDLR  = p2->m.UDLR;
		    p->m.Command= 0;
		}
	    }

	    _itoa( p->m.fg, fg,	10);
	    _itoa( p->m.x, x, 10 );
	    _itoa( p->m.y, y, 10 );
	    _itoa( p->m.UDLR, UDLR, 10 );
	    _itoa( p->m.Command	,Command, 10);

	    CString str;
	    CString str1 = fg;
	    CString str2 = x;
	    CString str3 = y;
	    CString str4 = UDLR;
	    CString str5 = Command;
	    CString str6 = p->m.Name;

	    str	+= "S1|"+ str1;
	    str	+= "|"	+ str2 + "|" + str3 ;
	    str	+= "|" + str4 +	"|" + str5 + "|" + str6	+ "|";

	    char szBuf[256];
	    MMX_ZeroMemory( szBuf, 256);

	    Take_name(szBuf ,str);

	    Send_Data(szBuf,str.GetLength());

	    p->m.fg = 0;
	    p->m.x  = 0;
	    p->m.y  = 0;
	    p->m.Command= 0;
	    Take_name(p->m.Name	,"");
	}
}
//-----------------------------------------------------------------------------
//	Send (x,y,Command) data	To Server
//-----------------------------------------------------------------------------
void	Command_SD()
{

	char x[12];
	char y[12];
	char UDLR[6];
	char Command[6];

	Man_Link* p = Find_Man_num( Lan_My_number );
	if( p != NULL )
	{


	    Man_Link* p2 = Find_Man_num( 1 );
	    if(	p2 != NULL )
	    {
		_itoa( p2->m.tgt_x, x, 10 );
		_itoa( p2->m.tgt_y, y, 10 );
		_itoa( p->m.UDLR, UDLR,	10 );
		_itoa( p->m.Command ,Command, 10);
	    }

	    CString str;
	    CString str1 = x;
	    CString str2 = y;
	    CString str3 = UDLR;
	    CString str4 = Command;
	    str	+= "SD|"+ str1;
	    str	+= "|"	+ str2 + "|" + str3 + "|";
	    str	+= str4	+ "|";


	    char szBuf[64];
	    MMX_ZeroMemory( szBuf, 64);
	    Take_name(szBuf ,str);

	    Send_Data(szBuf,str.GetLength());
	}

}
//-----------------------------------------------------------------------------
//	Test Lan Speed
//-----------------------------------------------------------------------------
void	Command_TS()
{
	char szBuf[32] = "TS|1234567890123456789012345678";
	TS_bk_time = GetTickCount();
	Send_Data(szBuf,32);


}

//-----------------------------------------------------------------------------
//	Talk To	ALL
//-----------------------------------------------------------------------------
void	Command_TA(char	*talk_data)
{
	CString	str;
	CString	str1 = talk_data;

	str = "TA|"+ str1 + "|";
	char szBuf[128];
	MMX_ZeroMemory(	szBuf, 128);
	Take_name(szBuf	,str);

	Send_Data(szBuf,str.GetLength());
}

