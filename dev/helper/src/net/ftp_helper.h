// FtpGet.h: interface for the CFtpGet class.

#if !defined(AFX_FTPGET_H__9A4CA560_F2F5_11D3_9FF0_0080C8F223E1__INCLUDED_)
#define AFX_FTPGET_H__9A4CA560_F2F5_11D3_9FF0_0080C8F223E1__INCLUDED_

#include "AFXINET.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace net
{
    class CFtpHelper
    {
    public:// function
        CFtpHelper();
        virtual ~CFtpHelper();

        // password & user name
        bool SetAccessRight( CString userName,
            CString userPass );

        // open the connection
        bool OpenConnection( CString server );

        // only one file
        bool GetFile( CString remoteFile,
            CString localFile );

        // a full CString array
        int GetMultipleFile( CStringArray *remoteArray,
            CStringArray *localArray,
            int number_file );

        // close the connection
        bool CloseConnection();

    };
}


#endif // !defined(AFX_FTPGET_H__9A4CA560_F2F5_11D3_9FF0_0080C8F223E1__INCLUDED_)
