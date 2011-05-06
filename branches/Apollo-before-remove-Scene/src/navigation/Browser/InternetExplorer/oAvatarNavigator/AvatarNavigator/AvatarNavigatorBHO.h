// AvatarNavigatorBHO.h : Declaration of the CAvatarNavigatorBHO

#pragma once
#include "resource.h"       // main symbols

#include "AvatarNavigator.h"
#include <shlguid.h>     // IID_IWebBrowser2, DIID_DWebBrowserEvents2, etc.
#include <exdispid.h> // DISPID_DOCUMENTCOMPLETE, etc.
#include <mshtml.h>         // DOM interfaces

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CAvatarNavigatorBHO

class ATL_NO_VTABLE CAvatarNavigatorBHO :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAvatarNavigatorBHO, &CLSID_AvatarNavigatorBHO>,
	public IObjectWithSiteImpl<CAvatarNavigatorBHO>,
	public IDispatchImpl<IAvatarNavigatorBHO, &IID_IAvatarNavigatorBHO, &LIBID_AvatarNavigatorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
  public IDispEventImpl<1, CAvatarNavigatorBHO, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 1>
{
public:
	CAvatarNavigatorBHO()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_AVATARNAVIGATORBHO)

DECLARE_NOT_AGGREGATABLE(CAvatarNavigatorBHO)

BEGIN_COM_MAP(CAvatarNavigatorBHO)
	COM_INTERFACE_ENTRY(IAvatarNavigatorBHO)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
  STDMETHOD(SetSite)(IUnknown *pUnkSite);

BEGIN_SINK_MAP(CAvatarNavigatorBHO)
  SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete)
END_SINK_MAP()

  // DWebBrowserEvents2
  void STDMETHODCALLTYPE OnDocumentComplete(IDispatch *pDisp, VARIANT *pvarURL); private:
  CComPtr<IWebBrowser2>  m_spWebBrowser;

private:
  void RemoveImages(IHTMLDocument2 *pDocument);

private:
  BOOL m_fAdvised;

};

OBJECT_ENTRY_AUTO(__uuidof(AvatarNavigatorBHO), CAvatarNavigatorBHO)
