// This file is distributed under GPLv3 licence
// Author: Gorelov Grigory (gorelov@grigory.info)
//
// Contacts and other info are on the WEB page:  grigory.info/MPFDParser



#ifndef _MPFDPARSER_H
#define _MPFDPARSER_H

#include <iostream>
#include <string>
#include <map>
#include "mpfd-exception.h"
#include "mpfd-field.h"
#include <string.h>
#include <stdlib.h>

namespace mpfd
{
    class Parser
    {
    public:
        static const int StoreUploadedFilesInFilesystem = 1, StoreUploadedFilesInMemory = 2;


        Parser();
        ~Parser();

        void SetContentType( const std::string type );
        void SetBoundary( const std::string bd );

        void AcceptSomeData( const char* data, const int length );

        void SetMaxCollectedDataLength( int max );
        void SetTempDirForFileUpload( std::string dir );
        void SetUploadedFilesStorage( int where );

        std::map<std::string, Field*> GetFieldsMap();

    private:
        int WhereToStoreUploadedFiles;

        std::map<std::string, Field*> Fields;

        std::string TempDirForFileUpload;
        int CurrentStatus;

        // Work statuses
        static int const Status_LookingForStartingBoundary = 1;
        static int const Status_ProcessingHeaders = 2;
        static int const Status_ProcessingContentOfTheField = 3;

        std::string Boundary;
        std::string ProcessingFieldName;
        bool _HeadersOfTheFieldAreProcessed;
        int   ContentLength;
        char* DataCollector;
        int   DataCollectorLength, MaxDataCollectorLength;
        bool  FindStartingBoundaryAndTruncData();
        void  _ProcessData();
        void  _ParseHeaders( std::string headers );
        bool  WaitForHeadersEndAndParseThem();
        void  TruncateDataCollectorFromTheBeginning( int n );
        int   BoundaryPositionInDataCollector();
        bool  ProcessContentOfTheField();
    };
}

#endif  /* _PARSER_H */

