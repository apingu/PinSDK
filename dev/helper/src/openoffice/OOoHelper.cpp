#ifdef _OOHELPER_

#ifdef _UNICODE

#include <Win32/win32_utility.h>
#include <tchar.h>
#include "OOoHelper.h"

#include <com/sun/star/lang/SystemDependent.hpp>

#include <com/sun/star/util/XURLTransformer.hpp>
#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/util/CloseVetoException.hpp>
#include <com/sun/star/util/URL.hpp>

#include <com/sun/star/awt/WindowAttribute.hpp>
#include <com/sun/star/awt/PosSize.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/awt/XSystemDependentWindowPeer.hpp>
#include <com/sun/star/awt/XSystemChildFactory.hpp>

#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#include <com/sun/star/frame/XLayoutManager.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XDispatchHelper.hpp>
#include <com/sun/star/frame/DispatchResultEvent.hpp>
#include <com/sun/star/frame/XStatusListener.hpp>
#include <com/sun/star/frame/XTitle.hpp>
#include <com/sun/star/frame/XTerminateListener.hpp>

#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/NamedValue.hpp>


#pragma comment (lib, "isal.lib")
#pragma comment (lib, "icppu.lib")
#pragma comment (lib, "icppuhelper.lib")
#pragma comment (lib, "isalhelper.lib")
#pragma comment (lib, "ireg.lib")
#pragma comment (lib, "ipurpenvhelper3MSC.lib")
#pragma comment (lib, "istore.lib")
#pragma comment (lib, "stlport_vc71.lib")


// calc need
//#include <com/sun/star/container/XIndexAccess.hpp>  
//#include <com/sun/star/container/XNameAccess.hpp>  
//#include <com/sun/star/container/XNamed.hpp>  
#include <com/sun/star/frame/XStorable.hpp>  

#include "OOoHelper.h"

#include <cppuhelper/bootstrap.hxx>
#include <cppuhelper/implbase1.hxx> 
#include <osl/file.hxx>
#include <osl/process.h>


#include <com/sun/star/sheet/XUsedAreaCursor.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/table/XTableRows.hpp>
#include <com/sun/star/table/XColumnRowRange.hpp>

#include <com/sun/star/document/MacroExecMode.hpp>
#include <com/sun/star/script/provider/XScriptProviderFactory.hpp>
#include <com/sun/star/script/provider/XScriptProviderSupplier.hpp>
#include <com/sun/star/script/provider/XScriptProvider.hpp>
#include <com/sun/star/script/provider/XScript.hpp>

#include <T_Lock.h>
#include <pf-system.h>
#include <map>



using namespace com::sun::star::text;
using namespace com::sun::star::table;
using namespace com::sun::star::script::provider;
using namespace com::sun::star::document;

#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
using namespace com::sun::star::bridge;


using rtl::OUString;
using com::sun::star::util::URL;
using com::sun::star::beans::PropertyValue;

using ::com::sun::star::lang::EventObject;
using ::com::sun::star::frame::XTerminateListener;


namespace OOo 
{
    T_Lock                             s_utlock;
    int                                SpreadsheetDocHelper::s_prvCount = 0;
    Reference<XComponentContext>       SpreadsheetDocHelper::s_xContext;
    Reference<XMultiComponentFactory>  SpreadsheetDocHelper::s_xMSFactory;

    struct ComponetnRefer
    {
        Reference<XComponent>   Component;
        UINT                    ReferCount;
    };
    std::map<OOo::OString, ComponetnRefer > s_OpenFileList;

    //=================================================================================
    //
    //
    //=================================================================================
    SpreadsheetDocHelper::SpreadsheetDocHelper()
    {
        m_sDocFileName = _TEXT("Data");
        m_bDataChanged = false;
        m_bReadOnly    = false;

        Initiate();
        Connect();
        assert( (s_xMSFactory != NULL) && "connect to openoffice failure" );
    }

    //=================================================================================
    //
    //
    //=================================================================================
    SpreadsheetDocHelper::~SpreadsheetDocHelper()
    {
        Dispose();
    }

    //=================================================================================
    //
    //
    //=================================================================================
    static inline Reference<XComponentContext>& GetBootContext() 
    {
        static Reference<XComponentContext> rCC = ::cppu::bootstrap();
        return rCC;
    }

    //=================================================================================
    //
    //
    //=================================================================================
    LRESULT SpreadsheetDocHelper::Initiate()
    {
        // OpenOffice.org 3 installation key
        static LPCOSTR sSubKey = _TEXT("SOFTWARE\\Wow6432Node\\OpenOffice.org\\Layers_\\URE\\1");
        HKEY hk = NULL;

        // check for machine install
        LRESULT lRes = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hk);

        // check for user install
        if (lRes != ERROR_SUCCESS)
            if (::RegOpenCurrentUser(KEY_READ|KEY_WOW64_64KEY, &hk) == ERROR_SUCCESS)
                lRes = ::RegOpenKeyEx(hk, sSubKey, 0, KEY_READ|KEY_WOW64_64KEY, &hk);

        if (lRes != ERROR_SUCCESS)
            return lRes;

        // Allocate new PATH environment variable 
        PVOID pNewPath = calloc(MAX_PATH, sizeof(OCHAR));
        if (!pNewPath)
            return GetLastError();

        // Get the UREINSTALLLOCATION directory 
        DWORD dwsize = MAX_PATH;
        POCHAR sNewPath = (POCHAR)pNewPath;
        if (lRes = RegQueryValueEx(hk, _TEXT("UREINSTALLLOCATION"), NULL, NULL, (LPBYTE)sNewPath, &dwsize) == ERROR_SUCCESS)
            lRes = _tcsncat_s(sNewPath + _tcsclen(sNewPath), MAX_PATH - _tcsclen(sNewPath), _TEXT("bin;"), 4);
        RegCloseKey(hk);

        // Return if no UREINSTALLLOCATION directory 
        if (lRes != ERROR_SUCCESS)
        {
            free(pNewPath);
            return lRes;
        }

        // Get the old PATH environment variable
        size_t sizeOldPath;
        lRes = _tgetenv_s(&sizeOldPath, NULL, 0, _TEXT("PATH"));
        LPOSTR sOldPath = (POCHAR)malloc(sizeOldPath * sizeof(OCHAR));
        if (sOldPath && SUCCEEDED(lRes))
            lRes = _tgetenv_s(&sizeOldPath, sOldPath, sizeOldPath, _TEXT("PATH"));

        // Add directory to PATH if not already there
        if (SUCCEEDED(lRes) && !_tcsstr(sOldPath, sNewPath))
        {
            // Resize mem for new PATH environment variable
            if (sNewPath = (POCHAR)(pNewPath = realloc(pNewPath, (_tcsclen(sNewPath) + sizeOldPath) * sizeof(OCHAR))))
                // Append old PATH content to new
                    if SUCCEEDED(lRes = _tcsncat_s(sNewPath + _tcsclen(sNewPath), sizeOldPath, sOldPath, sizeOldPath))
                        // Change PATH environment variable
                            lRes = _tputenv_s(_TEXT("PATH"), sNewPath);
        }

        // Cleanup
        free(sOldPath);
        free(pNewPath);

        return lRes;
    }

    /*
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>

    void killProcessByName(TCHAR* filename)
    {
        HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
        PROCESSENTRY32 pEntry;
        pEntry.dwSize = sizeof (pEntry);
        BOOL hRes = Process32First(hSnapShot, &pEntry);
        while (hRes)
        {
            if (wcscmp(pEntry.szExeFile, filename) == 0)
            {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                    (DWORD) pEntry.th32ProcessID);
                if (hProcess != NULL)
                {
                    TerminateProcess(hProcess, 9);
                    CloseHandle(hProcess);
                }
            }
            hRes = Process32Next(hSnapShot, &pEntry);
        }
        CloseHandle(hSnapShot);
    }
    */

    //=================================================================================
    //
    //
    //=================================================================================
    /** Connect to a running office that is accepting connections.
    @return  The ServiceManager to instantiate office components. */
    Reference<XMultiComponentFactory> SpreadsheetDocHelper::Connect()
    {
        TS_Lock lock(&s_utlock);
        s_prvCount++;
        if (s_prvCount > 0 && s_xMSFactory != NULL)
            return s_xMSFactory;
        if (s_xContext == NULL)
        {
            killProcessByName(_T("soffice.bin"));
            killProcessByName(_T("soffice.exe"));
            s_xContext = bootstrap();
            //s_xContext = defaultBootstrap_InitialComponentContext();
        }
        //Reference<XMultiComponentFactory> factory;
        if( s_xMSFactory == NULL )
            s_xMSFactory = s_xContext->getServiceManager();
        return s_xMSFactory;
    }

    //=================================================================================
    //
    //
    //=================================================================================
    void SpreadsheetDocHelper::Dispose()
    {
        //Reference<XComponent> com = NULL;
        /*
        if (mxNumberFormatsSupplier != null)
        {
            com = (XComponent)mxNumberFormatsSupplier;
            com.dispose();
            mxNumberFormatsSupplier = null;
        }
        if (mxStyleFamiliesSupplier != null)
        {
            mxStyleFamiliesSupplier = null;
        }
        if (mxRecentFunctions != null)
        {
            //com = (XComponent)mxRecentFunctions;
            //com.dispose();
            mxRecentFunctions = null;
        }
        if (mxFunctionAccess != null)
        {
            //com = (XComponent)mxFunctionAccess;
            //com.dispose();
            mxFunctionAccess = null;
        }
        */

        /*
        if (m_xDocument != NULL)
        {
//            com = (XComponent)mxDocument;
//            com.dispose();
            m_xDocument->dispose();  
            m_xDocument = NULL;
        }
        */
        s_prvCount--;

        std::map<OOo::OString, ComponetnRefer >::iterator iter = s_OpenFileList.find(m_sDocFileName);
        if( iter != s_OpenFileList.end())
        {
            iter->second.ReferCount--;
            if(iter->second.ReferCount == 0)
            {
                iter->second.Component->dispose();
                m_xCompoent = NULL;
                s_OpenFileList.erase(iter);
            }
        }

        //if (m_xCompoent != NULL)
        //{
        //    m_xCompoent->dispose();
        //    //m_xCompoent->release();
        //    //m_xCompoent = NULL;
        //}

        //if (s_prvCount == 0 && s_xMSFactory != NULL)
        //{
        //    s_xMSFactory->release();
        //    //s_xMSFactory = NULL;
        //}

        //if (s_prvCount == 0 && s_xContext != NULL)
        //{
        //    s_xContext->release();
        //    //s_xContext = NULL;
        //}

        if (s_prvCount == 0)
        {
            Reference< XComponent >::query( s_xMSFactory )->dispose();
            killProcessByName(_T("soffice.bin"));
            killProcessByName(_T("soffice.exe"));
            s_xContext = NULL;
            s_xMSFactory = NULL;
        }
     }

    //=================================================================================
    //
    //
    //=================================================================================
    OOo::OString SpreadsheetDocHelper::PathConverter(const OOo::OString file)
    {
        OOo::OString text = file;
        unsigned int pos = 0;
        while((pos = text.find(_T("\\"), pos)) != text.npos)
            text.replace(pos, 1, _T("/"));
        return OOo::OString(_TEXT("file:///")) + text;
    }

    //=================================================================================
    //
    //
    //=================================================================================
     Reference<XSpreadsheetDocument> SpreadsheetDocHelper::Open(OOo::OString filename)
     {
         if (s_xMSFactory == NULL)
         {
             Dispose();
             return NULL;
         }
         m_bDataChanged   = false;
         m_sDocFileName = filename;

         //Reference<XComponentContext>      xContext;
         //Reference<XMultiComponentFactory> xMSFactory;
         //Reference<XComponent>             xDocument;
         // get the remote office component context  
         //s_xContext = bootstrap();  
         // get the remote office service manager  
         //s_xMSFactory = s_xContext->getServiceManager();  
         // get an instance of the remote office desktop UNO service  
         // and query the XComponentLoader interface  
         Reference<XInterface> desktop = s_xMSFactory->createInstanceWithContext(ToOUString(_TEXT("com.sun.star.frame.Desktop")), s_xContext );  
         //Reference< XInterface > desktop = s_xMSFactory->createInstanceWithContext(ToOUString(_TEXT("com.sun.star.bridge.UnoUrlResolver")), s_xContext );

         Reference<XComponentLoader> rComponentLoader( desktop, UNO_QUERY_THROW ); 
         //Reference<XComponentLoader> rComponentLoader( desktop, UNO_QUERY ); 
         // the boolean property Hidden tells the office to open a file in hidden mode  
         Sequence<PropertyValue> loadProps(2);  
         loadProps[0].Name = ToOUString(_TEXT("Hidden"));
         loadProps[0].Value = Any(true);//new Boolean(true);  
         loadProps[1].Name = ToOUString(_TEXT("MacroExecMode"));
         loadProps[1].Value <<= com::sun::star::document::MacroExecMode::ALWAYS_EXECUTE_NO_WARN; ;//Any(true);// Any(2);//new Boolean(true);  

         //s_utlock.UnLock();

         OUString sAbsoluteDocUrl, sWorkingDir, sDocPathUrl, sArgDocUrl;

         if(filename != _TEXT(""))
         {
             sDocPathUrl = ToOUString(filename);
             osl_getProcessWorkingDir(&sWorkingDir.pData);
             osl::FileBase::getFileURLFromSystemPath( sArgDocUrl, sDocPathUrl);
             osl::FileBase::getAbsoluteFileURL( sWorkingDir, sDocPathUrl, sAbsoluteDocUrl);
         }
         else
         {
             sAbsoluteDocUrl = ToOUString(_TEXT("private:factory/scalc"));
         }


         std::map<OOo::OString, ComponetnRefer >::iterator iter = s_OpenFileList.find(filename);
         if( iter == s_OpenFileList.end())
         {
             //get an instance of the spreadsheet document  
             m_xCompoent = rComponentLoader->loadComponentFromURL( sAbsoluteDocUrl, ToOUString(_TEXT("_blank")), 0, loadProps);  

             ComponetnRefer refer;
             refer.Component = m_xCompoent;
             refer.ReferCount = 1;
             s_OpenFileList.insert(std::map<OOo::OString, ComponetnRefer >::value_type(filename, refer));
         }
         else
         {
             m_xCompoent = iter->second.Component;
             iter->second.ReferCount++;
         }

         //
         assert( (m_xCompoent != NULL) && "load OOo file failure" );

         Reference< XSpreadsheetDocument > rSheetDoc (m_xCompoent, UNO_QUERY);         
         return m_xDocument = rSheetDoc;
     }

     //=================================================================================
     //
     //
     //=================================================================================
     void SpreadsheetDocHelper::Store()
     {
         if((m_sDocFileName == _TEXT("")) || /*(m_bDataChanged == false) ||*/ (m_xCompoent == NULL) )
             return;

         //if(!IsAvailable())
         //   return;

         Reference<XStorable> rStore (m_xDocument, UNO_QUERY);  
         Sequence<PropertyValue> storeProps(1);  
         storeProps[0].Name  = OUString::createFromAscii("Overwrite");//ToOUString(_TEXT("Overwrite"));
         storeProps[0].Value = Any( true );

         OUString sAbsoluteDocUrl, sWorkingDir, sDocPathUrl, sArgDocUrl;
         OOo::OString filename = m_sDocFileName;
         sDocPathUrl = ToOUString(filename);
         osl_getProcessWorkingDir(&sWorkingDir.pData);
         osl::FileBase::getFileURLFromSystemPath( sArgDocUrl, sDocPathUrl);
         osl::FileBase::getAbsoluteFileURL( sWorkingDir, sDocPathUrl, sAbsoluteDocUrl);
         rStore->storeAsURL( sAbsoluteDocUrl, storeProps ); 
         //rStore->storeAsURL( _TEXT("file:///c:/MyTest.ods"), storeProps );
         m_bDataChanged = false;
     }

     //=================================================================================
     //
     //
     //=================================================================================
     /*
     Reference< XMultiServiceFactory > SpreadsheetDocHelper::ooConnect()
     {
         // create the initial component context
         //Reference< XComponentContext > rComponentContext = defaultBootstrap_InitialComponentContext();

         // retrieve the servicemanager from the context
         //Reference< XMultiComponentFactory > rServiceManager = rComponentContext->getServiceManager();

         // instantiate a sample service with the servicemanager.
         Reference< XInterface > rInstance =  s_xMSFactory->createInstanceWithContext(_TEXT("com.sun.star.bridge.UnoUrlResolver" ), s_xContext );

         // Query for the XUnoUrlResolver interface
         Reference< XUnoUrlResolver > rResolver( rInstance, UNO_QUERY );

         if( ! rResolver.is() )
         {
             printf( "Error: Couldn't instantiate com.sun.star.bridge.UnoUrlResolver service\n" );
             return NULL;
         }
         try 
         {
             // resolve the uno-url
             rInstance = rResolver->resolve( _TEXT("uno:socket,host=localhost,port=2083;urp;StarOffice.ServiceManager" ) );

             if( ! rInstance.is() ){
                 printf( "StarOffice.ServiceManager is not exported from remote counterpart\n" );
                 return NULL;
             }

             // query for the simpler XMultiServiceFactory interface, sufficient for scripting
             Reference< XMultiServiceFactory > rOfficeServiceManager (rInstance, UNO_QUERY);

             if( ! rOfficeServiceManager.is() ){
                 printf( "XMultiServiceFactory interface is not exported for StarOffice.ServiceManager\n" );
                 return NULL;
             }       
             return rOfficeServiceManager;
         }
         catch( Exception &e )
         {
             return NULL;
         }
         return NULL;
     } 
     */

     void SpreadsheetDocHelper::InvokeScript(OOo::OString marconame, Sequence< Any >& inArgs, Sequence< sal_Int16 >& outIndex, Sequence< Any >& outArgs)
     {
         if((s_xContext == NULL) || (m_xCompoent == NULL) )
             return;
         /*
         //retrieve an instance of the remote service manager
         Reference< XMultiServiceFactory > rOfficeServiceManager(s_xMSFactory, UNO_QUERY);// = ooConnect();
         //Reference< XMultiServiceFactory > rOfficeServiceManager = ooConnect();
         if( rOfficeServiceManager.is() )
         {
             //printf( _TEXT("Connected sucessfully to the office\n" ));
             int kkk = 0;
         }

         //get the desktop service using createInstance returns an XInterface type
         Reference< XInterface  > Desktop = rOfficeServiceManager->createInstance(_TEXT( "com.sun.star.frame.Desktop" ));

         //query for the XComponentLoader interface
         //Reference< XComponentLoader > rComponentLoader (Desktop, UNO_QUERY);
         //if( rComponentLoader.is() )
         //{
             //printf( _TEXT("XComponentloader successfully instanciated\n") );
         //}

         Reference< XDesktop > rDesktop(Desktop,UNO_QUERY);

         // Don't forget to add : #include <com/sun/star/frame/XDispatchHelper.hpp>
         // Don't forget to add "com.sun.star.frame.XDispatchHelper \" in the makefile
         // Query the XDispatcher Interface
         Reference< XDispatchHelper > rDispatchHelper = Reference< XDispatchHelper >( rOfficeServiceManager->createInstance( _TEXT("com.sun.star.frame.DispatchHelper" )), UNO_QUERY );


         // Don't forget to add : #include <com/sun/star/frame/XDispatchProvider.hpp>
         // Don't forget to add "com.sun.star.frame.XDispatchProvider \" in the makefile
         // Query the XDispatchProvider Interface
         Reference< XDispatchProvider > rDispatchProvider(rDesktop, UNO_QUERY);
         
         Any any = rDispatchHelper->executeDispatch(rDispatchProvider,
             _TEXT("macro:///Standard.SqTableGenerator.OutputLinkFile()"),
             _TEXT(""), 0, Sequence < ::com::sun::star::beans::PropertyValue > ());
         //file:///D:/WorkBench/Projects/OniSoul/programmer/src/tools/bin/TableMaker/table/reward/reward08_ranking.ods
                 //_TEXT("macro:///Standard.SqTableGenerator.OutputLinkFile"), _TEXT(""), 0, Sequence < ::com::sun::star::beans::PropertyValue > ());
         //SaySomething();


         return;
         */

         //OUString macroName = ToOUString(marconame + _TEXT("?language=Basic&location=document"));//"Standard.Module1.MYMACRONAME?language=Basic&location=application";

         //Reference <XInterface> iFactory = s_xMSFactory->createInstanceWithContext(OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.script.provider.MasterScriptProviderFactory" )), s_xContext);
         //Any aCtx;
         //aCtx <<= OUString();
         //Reference< XScriptProviderFactory > xFactory(iFactory, UNO_QUERY);
         //Reference<XScriptProvider> xProvider = xFactory->createScriptProvider(aCtx);
         

         Reference<XScriptProviderSupplier> xScriptPS(m_xCompoent, UNO_QUERY);
         Reference<XScriptProvider>         xScriptProvider = xScriptPS->getScriptProvider(); 

         OUString scriptname = ToOUString(_TEXT("vnd.sun.star.script:") + marconame + _TEXT("?language=Basic&location=document"));
         //Reference<XScript> xScript = xScriptProvider->getScript(OUString( _TEXT("vnd.sun.star.script:Standard.SqTableGenerator.OutputLinkFile?language=Basic&location=document")));
         Reference<XScript> xScript = xScriptProvider->getScript(scriptname);


         //Sequence< Any >        aParams;
         //Sequence< sal_Int16 > aOutParamIndex;
         //Sequence< Any >        aOutParam;

         //Sequence< Any > inArgs( 0 );
         //Sequence< Any > outArgs( 0 );
         //Sequence< sal_Int16 > outIndex;

         Any result = xScript->invoke(inArgs, outIndex, outArgs);
     }

     //=================================================================================
     //
     //
     //=================================================================================
     const bool SpreadsheetDocHelper::IsAvailable()
     {
         static bool bAvail = Initiate() == ERROR_SUCCESS ? GetBootContext().is() : false;
         return bAvail;
     }

     //=================================================================================
     //
     //
     //=================================================================================
     Reference< XSpreadsheet > SpreadsheetDocHelper::GetSpreadsheetByName(OOo::OString name)
     {
         if((s_xContext == NULL) || (m_xCompoent == NULL) )
             return NULL;

         Reference< XSpreadsheets > rSheets = m_xDocument->getSheets();
         Any rSheet = rSheets->getByName( ToOUString(name) );
         
         Reference< XSpreadsheet >  rSpSheet (rSheet, UNO_QUERY);
         Reference<XSpreadsheet>    m_xSheet;
         m_xSheet = rSpSheet;
         
         Reference< XModel > m(m_xDocument, UNO_QUERY);
         Reference< XController > controller = m->getCurrentController();
         Reference< XSpreadsheetView > view(controller, UNO_QUERY);
         view->setActiveSheet(m_xSheet); 

         return m_xSheet;
     }

     //=================================================================================
     //
     //
     //=================================================================================
     Reference< XSpreadsheet > SpreadsheetDocHelper::GetSpreadsheetByIndex(short index)
     {
         if((s_xContext == NULL) || (m_xCompoent == NULL) )
             return NULL;

         Reference< XSpreadsheets > rSheets = m_xDocument->getSheets();
         /*
         Any rSheet = rSheets->getByIndex( index );
         Reference< XSpreadsheet > rSpSheet (rSheet, UNO_QUERY);
         */
         Reference<XSpreadsheet> m_xSheet;
         //m_xSheet = rSpSheet;         
         return m_xSheet;
     }

     //=================================================================================
     //
     //
     //=================================================================================
     Reference< XSpreadsheet > SpreadsheetDocHelper::InsertSpreadsheed(OOo::OString name, short index)
     {
         if((s_xContext == NULL) || (m_xCompoent == NULL) )
             return NULL;

         m_bDataChanged = true;
         Reference< XSpreadsheets > rSheets = m_xDocument->getSheets();
         rSheets->insertNewByName(ToOUString(name), index);
         return GetSpreadsheetByName(name);
     }

     //=================================================================================
     //
     //
     //=================================================================================
     void SpreadsheetDocHelper::InsertRows(Reference< XSpreadsheet > sheet, long nIndex, long nCount)
     {
         if((s_xContext == NULL) || (sheet == NULL))
             return;

         Reference< XColumnRowRange > xCRRange(sheet, UNO_QUERY);
         Reference< XTableRows > xRows = xCRRange->getRows();  
         xRows->insertByIndex(nIndex, nCount);
     }

     //=================================================================================
     //
     //
     //=================================================================================
     void SpreadsheetDocHelper::RemoveRows(Reference< XSpreadsheet > sheet, long nIndex, long nCount)
     {
         if((s_xContext == NULL) || (sheet == NULL))
             return;

         Reference< XColumnRowRange > xCRRange(sheet, UNO_QUERY);
         Reference< XTableRows > xRows = xCRRange->getRows();
         xRows->removeByIndex(nIndex, nCount);
     }

     //=================================================================================
     //
     //
     //=================================================================================
     void SpreadsheetDocHelper::InsertCols(Reference< XSpreadsheet > sheet, long nIndex, long nCount)
     {
         if((s_xContext == NULL) || (sheet == NULL))
             return;

         Reference< XColumnRowRange > xCRRange(sheet, UNO_QUERY);
         Reference< XTableColumns > xCols = xCRRange->getColumns();  
         xCols->insertByIndex(nIndex, nCount);
     }

     //=================================================================================
     //
     //
     //=================================================================================
     void SpreadsheetDocHelper::RemoveCols(Reference< XSpreadsheet > sheet, long nIndex, long nCount)
     {
         if((s_xContext == NULL) || (sheet == NULL))
             return;

         Reference< XColumnRowRange > xCRRange(sheet, UNO_QUERY);
         Reference< XTableColumns > xCols = xCRRange->getColumns();  
         xCols->removeByIndex(nIndex, nCount);
     }

     //=================================================================================
     //
     //
     //=================================================================================
     CellContentType SpreadsheetDocHelper::GetCellType(Reference< XCell > cell)
     {
         if( (s_xContext == NULL) || (cell == NULL) )
             return CellContentType_EMPTY;
         return cell->getType();
     }

     //=================================================================================
     //
     //
     //=================================================================================
     Reference< XCell > SpreadsheetDocHelper::GetCell(Reference< XSpreadsheet > sheet, int col, int row)
     {
         if( (s_xContext == NULL) || (sheet == NULL) )
             return NULL;
         return sheet->getCellByPosition(col, row);
     }

     //=================================================================================
     //
     //
     //=================================================================================
     void SpreadsheetDocHelper::SetValue(Reference< XCell > cell, double value)
     {
         if( (s_xContext == NULL) || (cell == NULL) )
             return;
         return cell->setValue(value);
     }

     //=================================================================================
     //
     //
     //=================================================================================
     double SpreadsheetDocHelper::GetValue(Reference< XCell > cell)
     {
         if( (s_xContext == NULL) || (cell == NULL) )
             return 0;
         return cell->getValue();
     }

     //=================================================================================
     //
     //
     //=================================================================================
     void SpreadsheetDocHelper::SetText(Reference< XCell > cell, OOo::OString text)
     {
         if( (s_xContext == NULL) || (cell == NULL) )
             return;
         OUString formula_bout = cell->getFormula();
         OUString formula_in = ToOUString(text);
         cell->setFormula(formula_in);
         OUString formula_aout = cell->getFormula();
         return;
     }

     //=================================================================================
     //
     //
     //=================================================================================
     OOo::OString  SpreadsheetDocHelper::GetText(Reference< XCell > cell)
     {
         if( (s_xContext == NULL) || (cell == NULL) )
             return _TEXT("");
         return ToOOoString( cell->getFormula() );
     }

     //=================================================================================
     //
     //
     //=================================================================================
     OOo::OString SpreadsheetDocHelper::GetFormulaValue(Reference< XCell > cell)
     {
         if( (s_xContext == NULL) || (cell == NULL) )
             return _TEXT("");
         /*
         switch (cell->getType())
         {
         case CellContentType_EMPTY:
             break;
         case CellContentType_VALUE:
             break;
         case CellContentType_TEXT:
             break;
         case CellContentType_FORMULA:
             break;
         default: //CellContentType_MAKE_FIXED_SIZE = SAL_MAX_ENUM
             break;
         }
         */

         OUString formula = cell->getFormula();
         // 取得運算後的字串值，非運算式則傳回字串值
         Reference< XText > xText(cell, UNO_QUERY);
         OUString text = xText->getString();
         return ToOOoString(text);
     }

     //=================================================================================
     //
     //
     //=================================================================================
     void SpreadsheetDocHelper::GetRowsCols(Reference< XSpreadsheet > sheet, int& rows, int& cols)
     {
         if( (s_xContext == NULL) || (sheet == NULL) )
             return;

         // 取得Cursor
         Reference< XSheetCellCursor > xCursor = sheet->createCursor();

         Reference< XCellRange > rSheetCellRange (sheet, UNO_QUERY);

         // 轉換XUsedAreaCursor,並指定取得有使用的區域
         //XUsedAreaCursor xUsedCursor = (XUsedAreaCursor) s_xMSFactory-> queryInterface(XUsedAreaCursor.class, xCursor);
         Reference< XUsedAreaCursor > xUsedCursor(xCursor, UNO_QUERY);

         xUsedCursor->gotoEndOfUsedArea(false);
         xUsedCursor->gotoEndOfUsedArea(true);
         Reference< XCellRangeAddressable > xSheetRange(xCursor, UNO_QUERY);

         CellRangeAddress cellRangeAddress = xSheetRange->getRangeAddress();

         // System.out.println(cellRangeAddress.StartRow);
         // System.out.println(cellRangeAddress.StartColumn);
         // System.out.println(cellRangeAddress.EndRow);
         // System.out.println(cellRangeAddress.EndColumn);
         rows = cellRangeAddress.EndRow + 1;
         cols = cellRangeAddress.EndColumn + 1;



         /*
         for (int row = 0; row < rowCount; row++) 
         {
             for (int col = 0; col < colCount; col++) 
             {
                 Reference< XCell > cell = xCursor->getCellByPosition(col, row);
                 // 取得運算式或字串值
                 OUString formula = cell->getFormula();
                 // 取得運算後的字串值，非運算式則傳回字串值
                 Reference< XText > xText(cell, UNO_QUERY);
                 OUString text = xText->getString();
                 //System.out.println("\t" + row + "," + col + "," + formula + ","+ text);
             }
         }
         */
     }

     //=================================================================================
     //
     //
     //=================================================================================
     OUString SpreadsheetDocHelper::ToOUString(OOo::OString s)
     {
     #ifdef _UNICODE
         return s.c_str();
     #else
         return ToOUString(.c_str());
     #endif
     }

     //=================================================================================
     //
     //
     //=================================================================================
     OOo::OString SpreadsheetDocHelper::ToOOoString(OUString s)
     {
#ifdef _UNICODE
         return s.getStr();
#else
         return rtl::OUStringToOString( s, RTL_TEXTENCODING_UTF8 ).getStr();
#endif
     }
}

#endif

#endif