#include "..\Define\PGF.h"

////////////////////////////////////////////////////////////
//
//  轉圖
//

bool TransforFolder(char *path )
{
    char   *filepath;
	char   filename[100] , alphafile[100], pinfile[50] , tempstep[50] ;
    struct _finddata_t c_file;
    long   hFile;


    //要傳入轉檔的變數=================
    String alphapath, srcpath, destpath ;
    WORD AdtumMarkX = 0;
    WORD AdtumMarkY = 0;
    BYTE Act = 0;
    BYTE Direction = 0;
    BYTE Step = 0;
    WORD mode = 16;
    //=================================

    filepath =(char *) malloc(strlen(path));
    strcpy(filepath, path);
    strcat(filepath,"*.*");

 	/* Find first .c file in current directory */

    if( (hFile = _findfirst( filepath, &c_file )) == -1L )
       printf( "No *.c files in current directory!\n" );
    else
    {
            /* Find the rest of the .c files */

            while( _findnext( hFile, &c_file ) == 0 )
            {
                    //FrmMain->EditChg->Text = c_file.name;

                    if(strcmp(c_file.name, "..") || strcmp(c_file.name, "."))
                    {
                        continue;
                    }

                    if(c_file.attrib == _A_SUBDIR)
                    {
                        char subfolderpath[100];
						sprintf(subfolderpath, "%s%s%s", path, c_file.name, "\\");
                        TransforFolder(subfolderpath);
                        continue;
                    }

         	        strcpy(filename, c_file.name);
                    sprintf(pinfile  , "%s%s", filename, ".pin");
                    sprintf(alphafile, "%s%s", "A_", filename);
                    
                    
                    if((ExtractFileExt(FrmMain->EditChg->Text) != ".bmp") && (ExtractFileExt(FrmMain->EditChg->Text) != ".BMP"))
                    {
                         continue;
                    }
                    if(filename.SubString(1,2) == "A_")
                    {
                         continue;
                    }

                    alphapath = path + alphafile;
                    srcpath   = path + filename;
                    destpath  = path + pinfile;
                    bool ret = Translate(alphapath.c_str(), srcpath.c_str(), destpath.c_str(),AdtumMarkX, AdtumMarkY, Act, Direction, Step, mode);
                    if (ret == false)
                    {
                        char msg[30];
                        sprintf(msg,"%d 轉檔失敗 !!! !!!", srcpath.c_str());
                        MessageBox(NULL,msg,"轉檔失敗", MB_OK);

                    }


           }

       _findclose( hFile );

    }

   return true;
}

