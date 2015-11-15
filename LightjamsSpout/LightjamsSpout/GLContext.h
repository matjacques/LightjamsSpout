// GLContext.h : Declaration of the CGLContext

#pragma once
#include "resource.h"       // main symbols



#include "LightjamsSpout_i.h"
#include "..\..\SpoutSDK\Spout.h"


using namespace ATL;


// CGLContext

class ATL_NO_VTABLE CGLContext :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CGLContext, &CLSID_GLContext>,
	public ISupportErrorInfo,
	public IDispatchImpl<IGLContext, &IID_IGLContext, &LIBID_LightjamsSpoutLib, /*wMajor =*/ 1, /*wMinor =*/ 41>
{
public:
	CGLContext();	
	virtual ~CGLContext();

DECLARE_REGISTRY_RESOURCEID(IDR_GLCONTEXT)


BEGIN_COM_MAP(CGLContext)
	COM_INTERFACE_ENTRY(IGLContext)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);
	STDMETHOD(Create)();
	STDMETHOD(Delete)();	
	
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

private:
	void InitOpenGL();
	void DeleteOpenGL();

	bool _isCreated;
	HDC _hdc;
	HGLRC _glContext;
	HWND _hwnd;
};

OBJECT_ENTRY_AUTO(__uuidof(GLContext), CGLContext)
