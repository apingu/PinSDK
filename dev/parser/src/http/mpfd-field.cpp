// This file is distributed under GPLv3 licence
// Author: Gorelov Grigory (gorelov@grigory.info)
//
// Contacts and other info are on the WEB page:  grigory.info/MPFDParser

#include "mpfd-field.h"
#include "mpfd-parser.h"

mpfd::Field::Field()
{
    type = 0;
    FieldContent = NULL;

    FieldContentLength = 0;

}

mpfd::Field::~Field()
{

    if( FieldContent )
    {
        delete FieldContent;
    }

    if( type == FileType )
    {
        if( file.is_open() )
        {
            file.close();
            remove( ( TempDir + "/" + TempFile ).c_str() );
        }

    }

}

void mpfd::Field::SetType( int type )
{
    if( ( type == TextType ) || ( type == FileType ) )
    {
        this->type = type;
    }
    else
    {
        throw mpfd::Exception( "Trying to set type of field, but type is incorrect." );
    }

}

int mpfd::Field::GetType()
{
    if( type > 0 )
    {
        return type;
    }
    else
    {
        throw mpfd::Exception( "Trying to get type of field, but no type was set." );
    }
}

void mpfd::Field::AcceptSomeData( char *data, int length )
{
    if( type == TextType )
    {
        if( FieldContent == NULL )
        {
            FieldContent = new char[ length + 1 ];
        }
        else
        {
            FieldContent = ( char* )realloc( FieldContent, FieldContentLength + length + 1 );
        }

        memcpy( FieldContent + FieldContentLength, data, length );
        FieldContent[ FieldContentLength + length ] = 0;
    }
    else if( type == FileType )
    {
        if( WhereToStoreUploadedFiles == Parser::StoreUploadedFilesInFilesystem )
        {
            if( TempDir.length() > 0 )
            {
                if( !file.is_open() )
                {
                    int i = 1;
                    std::ifstream testfile;
                    std::string tempfile;
                    do {
                        if( testfile.is_open() )
                        {
                            testfile.close();
                        }

                        std::stringstream ss;
                        ss << "MPFD_Temp_" << i;
                        TempFile = ss.str();

                        tempfile = TempDir + "/" + TempFile;

                        testfile.open( tempfile.c_str(), std::ios::in );
                        i++;
                    } while( testfile.is_open() );

                    file.open( tempfile.c_str(), std::ios::out | std::ios::binary | std::ios_base::trunc );
                }

                if( file.is_open() )
                {
                    file.write( data, length );
                    file.flush();
                }
                else
                {
                    throw Exception( std::string( "Cannot write to file " ) + TempDir + "/" + TempFile );
                }
            }
            else {
                throw mpfd::Exception( "Trying to AcceptSomeData for a file but no TempDir is set." );
            }
        }
        else { // If files are stored in memory
            if( FieldContent == NULL )
            {
                FieldContent = new char[ length ];
            }
            else
            {
                FieldContent = ( char* )realloc( FieldContent, FieldContentLength + length );
            }
            memcpy( FieldContent + FieldContentLength, data, length );
            FieldContentLength += length;
        }
    }
    else
    {
        throw mpfd::Exception( "Trying to AcceptSomeData but no type was set." );
    }
}

void mpfd::Field::SetTempDir( std::string dir )
{
    TempDir = dir;
}

unsigned int mpfd::Field::GetFileContentSize()
{
    if( type == 0 )
    {
        throw mpfd::Exception( "Trying to get file content size, but no type was set." );
    }
    else
    {
        if( type == FileType )
        {
            if( WhereToStoreUploadedFiles == Parser::StoreUploadedFilesInMemory )
            {
                return FieldContentLength;
            }
            else
            {
                throw mpfd::Exception( "Trying to get file content size, but uploaded files are stored in filesystem." );
            }
        }
        else
        {
            throw mpfd::Exception( "Trying to get file content size, but the type is not file." );
        }
    }
}

char * mpfd::Field::GetFileContent()
{
    if( type == 0 )
    {
        throw mpfd::Exception( "Trying to get file content, but no type was set." );
    }
    else
    {
        if( type == FileType )
        {
            if( WhereToStoreUploadedFiles == Parser::StoreUploadedFilesInMemory )
            {
                return FieldContent;
            }
            else
            {
                throw mpfd::Exception( "Trying to get file content, but uplaoded files are stored in filesystem." );
            }
        }
        else
        {
            throw mpfd::Exception( "Trying to get file content, but the type is not file." );
        }
    }
}

std::string mpfd::Field::GetTextTypeContent()
{
    if( type == 0 )
    {
        throw mpfd::Exception( "Trying to get text content of the field, but no type was set." );
    }
    else
    {
        if( type != TextType )
        {
            throw mpfd::Exception( "Trying to get content of the field, but the type is not text." );
        }
        else
        {
            if( FieldContent == NULL )
            {
                return std::string();
            }
            else
            {
                return std::string( FieldContent );
            }
        }
    }
}

std::string mpfd::Field::GetTempFilename()
{
    if( type == 0 )
    {
        throw mpfd::Exception( "Trying to get file temp name, but no type was set." );
    }
    else
    {
        if( type == FileType )
        {
            if( WhereToStoreUploadedFiles == Parser::StoreUploadedFilesInFilesystem )
            {
                return std::string( TempDir + "/" + TempFile );
            }
            else
            {
                throw mpfd::Exception( "Trying to get file temp name, but uplaoded files are stored in memory." );
            }
        }
        else
        {
            throw mpfd::Exception( "Trying to get file temp name, but the type is not file." );
        }
    }
    return std::string();
}

std::string mpfd::Field::GetFileName()
{
    if( type == 0 )
    {
        throw mpfd::Exception( "Trying to get file name, but no type was set." );
    }
    else
    {
        if( type == FileType )
        {
            return FileName;
        }
        else
        {
            throw mpfd::Exception( "Trying to get file name, but the type is not file." );
        }
    }
}

void mpfd::Field::SetFileName( std::string name )
{
    FileName = name;
}

void mpfd::Field::SetUploadedFilesStorage( int where )
{
    WhereToStoreUploadedFiles = where;
}

void mpfd::Field::SetFileContentType( std::string type )
{
    FileContentType = type;
}

std::string mpfd::Field::GetFileMimeType()
{
    if( type == 0 )
    {
        throw mpfd::Exception( "Trying to get mime type of file, but no type was set." );
    }
    else
    {
        if( type != FileType )
        {
            throw mpfd::Exception( "Trying to get mime type of the field, but the type is not File." );
        }
        else
        {
            return std::string( FileContentType );
        }
    }
}