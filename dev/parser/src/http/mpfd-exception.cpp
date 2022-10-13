// This file is distributed under GPLv3 licence
// Author: Gorelov Grigory (gorelov@grigory.info)
//
// Contacts and other info are on the WEB page:  grigory.info/MPFDParser


#include "mpfd-exception.h"

mpfd::Exception::Exception( std::string error )
{
    Error = error;
}

mpfd::Exception::Exception( const mpfd::Exception& orig ) 
{
    Error = orig.Error;
}

mpfd::Exception::~Exception() 
{

}

std::string mpfd::Exception::GetError() 
{
    return Error;
}