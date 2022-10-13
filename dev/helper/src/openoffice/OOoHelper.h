// atlOOo.h 
//
// OpenOffice.org interface functions and classes for Win32 and WTL applications
//
// Copyright (c) Alain Rist 2008, 2009
// 
// Rev: 13 Feb 2009
//
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/osi3.0/licenses/cpl1.0.php)
// By using this software in any fashion, you are agreeing to be bound by
// the terms of this license. You must not remove this notice, or
// any other, from this software.

#ifndef __OOOHELPER_H__
#define __OOOHELPER_H__

#ifdef _OOHELPER_

#pragma once

#include <windows.h>
#include <crtdbg.h>
#include <string>
#include <map>

#ifndef WNT
    #define WNT
#endif

#define _SNPRINTF_H   // 因為會衝突，所以先把他mark掉

#ifndef _WIN32_WINNT
    #define _WIN32_WINNT
#endif


#ifndef CPPU_ENV
    #define CPPU_ENV msci
#endif

#pragma warning (push)
#pragma warning(disable : 4290 800)

#include <cppuhelper/bootstrap.hxx>  
#include <com/sun/star/lang/XMultiServiceFactory.hpp>  
#include <com/sun/star/lang/XMultiComponentFactory.hpp>  
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>  
#include <com/sun/star/beans/XPropertySet.hpp>  
#include <com/sun/star/util/Color.hpp>  
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>  
#include <com/sun/star/sheet/XSpreadsheet.hpp>  

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::frame;  
using namespace com::sun::star::sheet;  
using namespace com::sun::star::container;  
using namespace com::sun::star::table;  
using namespace com::sun::star::util;
using namespace rtl;  
using namespace cppu;  



//


///////////////////////////////////////////////////////////////////////////////
// Classes and functions in this file
//
// OOo functions:
//
// OOo::FindInstallAndSetPath enables delay loading of the OOo dlls, sets PATH environment variable to dll location.
// OOo::GetBootContext() bootstrap context accessor
// OOo::GetComponentFactory() main factory accessor 
// OOo::IsAvailable() runtime connection test
// OOo::Instance() uno service accessor
// OOo::GetURL() URL parser 
// OOo::GetPathURL() File URL parser 
// OOo::PropVal() PropertyValue helper
// OOo::SeqPropVal() single element Sequence<PropertyValue> helper
// OOo::HWNDToAny() HWND->Any helper
// OOo::AnyToHWND() Any->HWND helper
// OOo::CanLoad() file loadability test
//
// OOo classes:
//
// OOo::TerminationMonitor - monitors and prevents OOo shutdown
// OOo::DocWindow - Win32 embedded uno document holding window 
////////////////////////////////////////////////////////////////////////
//
// WTL class:
//
// WTL::COOoCtrlT - WTL wrapper for OOo::DocWindow
////////////////////////////////////////////////////////////////////////


namespace OOo 
{

#ifndef _UNICODE
    typedef  LPSTR            LPOSTR;
    typedef  LPCOSTR          LPCOSTR;
    typedef  CHAR             OCHAR;
    typedef  PWCHAR           POCHAR;
    typedef  std::string      OString;
#else
    typedef  LPWSTR           LPOSTR;
    typedef  LPCWSTR          LPCOSTR;
    typedef  WCHAR            OCHAR;
    typedef  PWCHAR           POCHAR; 
    typedef  std::wstring     OString;
#endif



    //using ::com::sun::star::lang::XMultiComponentFactory;
    //using ::com::sun::star::lang::XComponent;


    ///////////////////////////////////////////////////////////////////////////////


    /*
    // bootstrap context accessor
    inline Reference<XComponentContext>& GetBootContext() 
    {
        static Reference<XComponentContext> rCC = ::cppu::bootstrap();
        return rCC;
    }

    // main factory accessor
    inline Reference<XMultiComponentFactory>& GetComponentFactory() 
    {
        static Reference<XMultiComponentFactory> rMCF = GetBootContext()->getServiceManager();
        return rMCF;
    }

    // runtime connection test
    inline const bool IsAvailable()
    {
        static bool bAvail = Initiate() == ERROR_SUCCESS ? GetBootContext().is() : false;
        return bAvail;
    }

    // uno service accessor
    template <typename X>
    inline Reference<X> Instance(LPCOSTR sServiceName, Reference<XComponentContext> rContext = GetBootContext())
    {
        return Reference<X>(GetComponentFactory()->createInstanceWithContext(sServiceName, rContext), UNO_QUERY);
    }

    // URL parsers
    inline URL GetURL(LPCOSTR sUrl)
    {
        URL url;
        if (sUrl)
        {
            url.Complete = sUrl;
            using com::sun::star::util::XURLTransformer;
            Instance<XURLTransformer>("com.sun.star.util.URLTransformer")->parseStrict(url);
        }
        return url;
    }

    inline URL GetPathURL(LPCOSTR sPath)
    {
        OUString ousUrl, ousWorkingDir, ousPathUrl;
        osl_getProcessWorkingDir(&ousWorkingDir.pData);
        osl::FileBase::getFileURLFromSystemPath(sPath, ousPathUrl);
        osl::FileBase::getAbsoluteFileURL(ousWorkingDir, ousPathUrl, ousUrl);
        return GetURL(ousUrl);
    }

    // PropertyValue helpers
    template <typename T>
    PropertyValue PropVal(LPCOSTR sName, T t_Val)
    {
        using com::sun::star::beans::PropertyState_DIRECT_VALUE;
        return PropertyValue(sName, 0, Any(t_Val), PropertyState_DIRECT_VALUE);
    }

    template <typename T>
    Sequence<PropertyValue> SeqPropVal(LPCOSTR sName, T t_Val)
    {
        PropertyValue pv = PropVal(sName, t_Val);
        return Sequence<PropertyValue>(&pv, 1);
    }

    // Any <-> HWND helpers
    inline Any HWNDToAny(HWND hWnd)
    {
        return Any(reinterpret_cast<long>(hWnd));
    }

    inline HWND AnyToHWND(Any& any)
    {
        long lh = 0;
        any >>= lh;
        return reinterpret_cast<HWND>(lh);
    }

    // File loadability test
    inline bool CanLoad(LPCOSTR sPath)
    {
        Reference<XComponent> xDoc;
        using com::sun::star::frame::XComponentLoader;
        Reference<XComponentLoader> xLoader(Instance<XComponentLoader>("com.sun.star.frame.Desktop"));
        if (xLoader.is())
            try
        {
            using com::sun::star::frame::FrameSearchFlag::CREATE;
            xDoc = xLoader->loadComponentFromURL(GetPathURL(sPath).Complete, "_blank", CREATE, SeqPropVal("Hidden", true));
        }
        catch (Exception e)
        {}

        return xDoc.is() ? xDoc->dispose(), true : false;
    }

    // OOo::TerminationMonitor - monitors and prevents OOo shutdown
    //

    class TerminationMonitor :
        public ::cppu::WeakImplHelper1<XTerminateListener>
    {
    public:
        // Constrution and destruction
        TerminationMonitor(bool bAllow = false, bool bRegister = true) : 
            m_bAllowTermination(bAllow), m_bRegistered(false) 
        {
            if (bRegister)
                Register(true);
        }

        ~TerminationMonitor () 
        {
            Register(false);
        }

        // Operation
        bool IsTerminationAllowed()
        {
            return IsRegistered() ? m_bAllowTermination : true;
        }

        bool AllowTermination(bool bAllow)
        {
            return IsRegistered() ? m_bAllowTermination = bAllow : true;
        }

        bool IsRegistered()
        {
            return m_bRegistered;
        }

        bool Register(bool bRegister)
        {
            if (bRegister != m_bRegistered)
                try
            {
                using com::sun::star::frame::XDesktop;
                Reference<XDesktop> xDesktop(Instance<XDesktop>("com.sun.star.frame.Desktop"));

                if (bRegister)
                    xDesktop->addTerminateListener(this);
                else 
                    xDesktop->removeTerminateListener(this);

                m_bRegistered = bRegister;
            }
            catch (Exception e)
            {}

            return m_bRegistered;
        }

    protected:
        // Implementation data 
        bool m_bAllowTermination;
        bool m_bRegistered;

        // XEventListener
        virtual void SAL_CALL disposing(const EventObject& Source)
        {
            m_bRegistered = false;
        }

        // XTerminateListener
        virtual void SAL_CALL queryTermination(const EventObject& aEvent)
        {
            using com::sun::star::frame::TerminationVetoException;
            if (!m_bAllowTermination)
                throw TerminationVetoException();
        }

        virtual void SAL_CALL notifyTermination(const EventObject& aEvent)
        {}
    };


    // OOo::DocWindow - Win32 embedded uno document holding window 
    //

    class DocWindow 
    {
        typedef com::sun::star::awt::XWindow XWindow;
        typedef com::sun::star::frame::XFrame XFrame;
        typedef com::sun::star::frame::XStatusListener XStatusListener;
        Reference<XFrame> m_xFrame;
        Reference<XWindow> m_xWindow;
        Reference<XComponent> m_xDoc;

    public:
        // Window
        bool Create(HWND hWndParent, LPCOSTR sFrameName = NULL, bool bVisible = true)
        {
            using com::sun::star::awt::XWindowPeer;
            _ASSERT_EXPR(!m_xWindow.is(), "Window exists already");

            // create a uno window
            using com::sun::star::awt::XWindowPeer;
            using com::sun::star::awt::XSystemChildFactory;
            using com::sun::star::lang::SystemDependent::SYSTEM_WIN32;
            Reference<XWindowPeer> xPeer = Reference<XWindowPeer>(
                Instance<XSystemChildFactory>("com.sun.star.awt.Toolkit")->createSystemChild(
                HWNDToAny(hWndParent), Sequence<sal_Int8>(4), SYSTEM_WIN32), UNO_QUERY);
            if (xPeer.is())
                try 
            {
                m_xWindow = Reference<XWindow>(xPeer, UNO_QUERY);

                // create a uno frame
                m_xFrame = Instance<XFrame>("com.sun.star.frame.Frame");

                if (m_xWindow.is() && m_xFrame.is())
                {
                    // attach window to frame
                    m_xFrame->initialize(m_xWindow);

                    // set frame name
                    if (sFrameName)
                        m_xFrame->setName(sFrameName);

                    // append frame to desktop 
                    using com::sun::star::frame::XFrames;
                    using com::sun::star::frame::XFramesSupplier;
                    Reference<XFrames> xChildContainer = Reference<XFramesSupplier>(
                        Instance<XFramesSupplier>("com.sun.star.frame.Desktop"))->getFrames();
                    xChildContainer->append(m_xFrame);

                    // set window visibility
                    m_xWindow->setVisible(bVisible);
                }
            }
            catch (Exception e)
            {
                m_xWindow.clear();
            }
            return m_xWindow.is();
        }

        HWND GetHWND()
        {
            using com::sun::star::awt::XSystemDependentWindowPeer;
            Reference<XSystemDependentWindowPeer> xPeer(m_xWindow, UNO_QUERY);

            using com::sun::star::lang::SystemDependent::SYSTEM_WIN32;
            return xPeer.is() ? 
                AnyToHWND(xPeer->getWindowHandle(Sequence<sal_Int8>(4), SYSTEM_WIN32)) :
                NULL;
        }

        // window positioning
        void SetWindowRect(RECT& rect)
        {
            using com::sun::star::awt::PosSize::POSSIZE;
            if (m_xWindow.is())
                m_xWindow->setPosSize(rect.left, rect.top, 
                rect.right - rect.left, rect.bottom - rect.top, POSSIZE);
        }

        // UI elements visibility
        bool UIShow(LPCOSTR sURL, bool bShow)
        {
            using com::sun::star::frame::XLayoutManager;
            using com::sun::star::beans::XPropertySet;
            Reference<XLayoutManager> xLayoutManager(Reference<XPropertySet>(m_xFrame, UNO_QUERY)->getPropertyValue("LayoutManager"), UNO_QUERY);

            return bShow ? xLayoutManager->showElement(sURL) : xLayoutManager->hideElement(sURL) == sal_True;
        }

        // Document
        bool OpenDocument(LPCOSTR sURL, Sequence<PropertyValue>& sProperties)
        {
            using com::sun::star::frame::XComponentLoader;
            Reference<XComponentLoader> xLoader(Instance<XComponentLoader>("com.sun.star.frame.Desktop"));
            if (xLoader.is())
            {
                OUString 
                    ousName = m_xFrame->getName(),
                    ousMagic  = "odk_officedev_desk"; // magic name
                m_xFrame->setName(ousMagic);

                try
                {
                    using com::sun::star::frame::FrameSearchFlag::CHILDREN;
                    m_xDoc = xLoader->loadComponentFromURL(sURL, ousMagic, CHILDREN, sProperties);
                }
                catch (Exception e)
                {}

                if (m_xDoc.is())
                {
                    m_xFrame->setName(GetDocTitle());
                }
                else
                    m_xFrame->setName(ousName);
            }
            return m_xDoc.is();
        }

        bool OpenNewDocument(LPCOSTR sName, bool bReadOnly = true)
        {
            return OpenDocument(GetURL(sName).Complete, SeqPropVal("ReadOnly", bReadOnly));
        }

        bool OpenFileDocument(LPCOSTR sPath, bool bReadOnly = true)
        {
            return OpenDocument(GetPathURL(sPath).Complete, SeqPropVal("ReadOnly", bReadOnly));
        }

        bool CloseDocument()
        {
            if (m_xDoc.is())
                try
            {
                using com::sun::star::util::XModifiable;
                Reference<XModifiable>(m_xDoc, UNO_QUERY)->setModified(false);
                m_xDoc->dispose();
                m_xDoc.clear();
                return true;
            }
            catch (Exception e)
            {}

            return false;
        }

        bool HasDocument()
        {
            return m_xDoc.is();
        }

        LPCOSTR GetDocTitle()
        {
            using com::sun::star::frame::XTitle;
            return m_xDoc.is() ? Reference<XTitle>(m_xDoc, UNO_QUERY)->getTitle() : "";
        }

        bool ExecuteURL(URL& url, Sequence<PropertyValue>& sProperties, Reference<XStatusListener> xListener = NULL)
        {
            using com::sun::star::frame::XDispatch;
            using com::sun::star::frame::XDispatchProvider;
            using com::sun::star::frame::FrameSearchFlag::AUTO;
            Reference<XDispatch> xDispatcher = Reference<XDispatchProvider>(m_xFrame, UNO_QUERY)->queryDispatch(url, "", AUTO);
            if (xDispatcher.is())
                try 
            {
                if (xListener.is())
                    xDispatcher->addStatusListener(xListener, url);
                xDispatcher->dispatch(url, sProperties);
            }
            catch(RuntimeException e)
            {
                xDispatcher.clear();
            }
            return xDispatcher.is();
        }

    };
    */


    class SpreadsheetDocHelper 
    {

        // __  private members  ___________________________________________
        OOo::OString                              m_sDocFileName;

        static int                                s_prvCount;
        static Reference<XComponentContext>       s_xContext;
        static Reference<XMultiComponentFactory>  s_xMSFactory;
        Reference<XComponent>                     m_xCompoent;
        Reference<XSpreadsheetDocument>           m_xDocument;

        std::map<int, OOo::OString>               m_FunctionNames;
        bool                                      m_bDataChanged;
        bool                                      m_bReadOnly;

        // enable delay loading of the OOo dlls, sets PATH environment variable to dll location.
        static LRESULT                     Initiate();
        /** Connect to a running office that is accepting connections.
        @return  The ServiceManager to instantiate office components. */
        Reference<XMultiComponentFactory>  Connect();
        void                               Dispose();
        OOo::OString                       PathConverter(const OOo::OString file);
        Reference< XMultiServiceFactory >  ooConnect();

    public:

        SpreadsheetDocHelper();
        ~SpreadsheetDocHelper();

        bool                               isReadOnly()         { return m_bReadOnly; }
        OOo::OString                       GetFullName()        { return m_sDocFileName; }
        bool                               isChanged()          { return m_bDataChanged; }
        bool                               Changed(bool value)  { m_bDataChanged = value; }

        

        Reference< XSpreadsheetDocument >  Open(OOo::OString filename);
        void                               Store();

        void                               InvokeScript(OOo::OString marconame, Sequence< Any >& inArgs, Sequence< sal_Int16 >& outIndex, Sequence< Any >& outArgs);

        static inline const bool           IsAvailable();

        Reference<XSpreadsheet>            GetSpreadsheetByName(OOo::OString name);
        Reference< XSpreadsheet >          GetSpreadsheetByIndex(short index);

        Reference< XSpreadsheet >          InsertSpreadsheed(OOo::OString name, short index);

        static void                        InsertRows(Reference< XSpreadsheet > sheet, long nIndex, long nCount);
        static void                        RemoveRows(Reference< XSpreadsheet > sheet, long nIndex, long nCount);

        static void                        InsertCols(Reference< XSpreadsheet > sheet, long nIndex, long nCount);
        static void                        RemoveCols(Reference< XSpreadsheet > sheet, long nIndex, long nCount);

        static Reference< XCell >          GetCell(Reference< XSpreadsheet > sheet, int col, int row);

        static CellContentType             GetCellType(Reference< XCell > cell);

        static void                        SetValue(Reference< XCell > cell, double value);
        static double                      GetValue(Reference< XCell > cell);

        static void                        SetText(Reference< XCell > cell, OOo::OString text);
        static OOo::OString                GetText(Reference< XCell > cell);
        static OOo::OString                GetFormulaValue(Reference< XCell > cell);

        static void                        GetRowsCols(Reference< XSpreadsheet > sheet, int& rows, int& cols);

        static OUString                    ToOUString(OOo::OString s);
        static OOo::OString                ToOOoString(OUString s);

    };

}; // namespace OOo

#pragma warning (pop)

#ifdef _WTL_VER

namespace WTL {

    using namespace OOo;

    ///////////////////////////////////////////////////////////////////////
    // COOoCtrlT: WTL wrapper for OOo::DocWindow

    template <class TBase/* = ATL::CWindow*/>
    class COOoCtrlT : public TBase, public OOo::DocWindow
    {
    public:

        // Overridden CWindow methods to prevent usage
        void Attach(HWND hWndNew)
        {
            ATLASSERT(FALSE);
        }

        HWND Detach()
        {
            ATLASSERT(FALSE);
            return m_hWnd;
        }

        // Specific create method
        HWND Create(HWND hWndParent, LPCOSTR szWindowName = NULL, bool bVisible = true)
        {
            ATLASSERT(m_hWnd == NULL);
            ATLASSERT(::IsWindow(hWndParent));

            if (DocWindow::Create(hWndParent, szWindowName, bVisible))
                m_hWnd = GetHWND();

            ATLASSERT(IsWindow());

            return m_hWnd;
        }

        // Usual WTL control create method
        HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCOSTR szWindowName = NULL,
            DWORD dwStyle = 0, DWORD dwExStyle = 0,
            ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
        {
            return Create(hWndParent, szWindowName, (dwStyle & WS_VISIBLE) != 0);
        }

        // ATL::CWindow create method
        HWND Create(LPCOSTR lpstrWndClass, HWND hWndParent, _U_RECT rect = NULL, LPCOSTR szWindowName = NULL,
            DWORD dwStyle = 0, DWORD dwExStyle = 0,
            _U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
        {
            return Create(hWndParent, szWindowName, dwStyle & WS_VISIBLE != 0);
        }

        // Resource defined command execution for toolbar or menu commands
        // names in http://wiki.services.openoffice.org/wiki/Framework/Article/OpenOffice.org_2.x_Commands
        bool ExecuteId(UINT uIdOOoCmd)
        {
            CTempBuffer<OCHAR> sCmd(64);
            AtlLoadString(uIdOOoCmd, sCmd, 64);
            return ExecuteURL(GetURL(sCmd), Sequence<PropertyValue>());
        }

        bool UIShowId(UINT uIdOOoUIPart, bool bShow)
        {
            CTempBuffer<OCHAR> sUIPart(64);
            AtlLoadString(uIdOOoUIPart, sUIPart, 64);
            return UIShow(sUIPart, bShow);
        }

    };

    typedef COOoCtrlT<ATL::CWindow> COOoCtrl;

} // namespace WTL

#endif // _WTL_VER

#endif //_OOHELPER_
#endif //  __ATLOOo_H__

