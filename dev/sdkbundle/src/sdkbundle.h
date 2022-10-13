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
#pragma once

#include <pf-def.h>
#include <pf-file.h>
#include <pf-string.h>
#include <pf-thread.h>
#include <pf-stdlib.h>
#include <pf-system.h>
#include <pf-time.h>
#include <pf-timer.h>
#include <pf-debug.h>
#include <pf-math.h>
#include <Unicode.h>

#include <pf-ziplib.h>

#include <parser/json.h>
#include <parser/json_type.h>
#include <parser/xmlschema.h>
#include <parser/config_reader.h>
#include <parser/html-parser.h>

#include <gadget/raw_cells.h>
#include <gadget/file_operator.h>
#include <gadget/sentence_pattern.h>

#include <algorithmic/crypt-aes.h>
#include <algorithmic/crypt-md5.h>
#include <algorithmic/crypt-crc32.h>
#include <algorithmic/crypt-crc64.h>
#include <algorithmic/crypt-base32.h>
#include <algorithmic/crypt-base64.h>

#include <helper/xl_file.h>
#include <helper/xldb-sheet.h>
#include <helper/xldb-query.h>
#include <helper/nh-toolkit.h>
//#include <helper/excelfile.h>
//#include <helper/xlsfile.h>
//#include <helper/xlsdbsheet.h>
//#include <helper/xlsdbquery.h>
//#include <helper/dictionary.h>
//#include <helper/excel_xml.h>
//#include <helper/csbrowser.h>

#include <net/net-socket.h>
#include <net/cs-json.h>
#include <net/cs-helper.h>
#include <net/cs-http_server.h>
#include <net/cs-database_service.h>

#include <database/xdatabase.h>
//#include <database/xdb-mysqldb.h>
#include <database/xdb-helper.h>
#include <database/xdb-query.h>
#include <database/xdb-uid_catalog.h>


// use mfc
#ifdef VC_EXTRALEAN
#include <helper/mfc-treectrl.h>
#include <helper/mfc-txbutton.h>
#include <helper/mfc-sxbutton.h>
#include <helper/mfc-utility.h>
#include <helper/mfc-txdialog.h>
#include <helper/mfc-listgrid.h>
#include <helper/win32_utility.h>
#endif


#ifdef _WINDOWS
    #ifdef _LIB
        #ifdef _UNICODE
            #pragma comment (lib, "sdkbundle_u.lib")
        #else
            #pragma comment (lib, "sdkbundle.lib")
        #endif
    #endif
#endif

namespace sdkbundle
{
    const tchar_t* GetVersion();

    const tchar_t* GetBuildTime();
}