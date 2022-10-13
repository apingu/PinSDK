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
#ifndef	CHINA_INPUT__H
#define	CHINA_INPUT__H

#include <windows.h>
//#define Direct_Draw6
//enum APPMSGTYPE	{ MSG_NONE, MSGERR_APPMUSTEXIT,	MSGWARN_SWITCHTOSOFTWARE };


	///////////////////////////////////////////////////////
	// China.CPP
	///////////////////////////////////////////////////////

	int	IMM_Check(char *str,int	str_pt,int str_length);
	int	Key_In(char *str,int str_pt,int	str_length,BYTE	key_in_flag);
	void	Show_Key_In(char *str, int str_pt , int	str_length,HFONT font, int x, int y, int font_size,BYTE	key_in_fg );
	void	Show_Key_Text(char *str,HFONT font, int	x, int y, int font_size, BYTE key_in_now, BYTE password_fg );



#endif	// CHINA_INPUT__H
