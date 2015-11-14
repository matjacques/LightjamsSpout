/**
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Copyright (c) 2015, Mathieu Jacques, Lightjams inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"	AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE	ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
========================

*/

#pragma once
#include "resource.h"       // main symbols

#include "..\..\SpoutSDK\Spout.h"


#include "LightjamsSpout_i.h"



using namespace ATL;


// CLightjamsSpoutReceiver

class ATL_NO_VTABLE CLightjamsSpoutReceiver :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CLightjamsSpoutReceiver, &CLSID_LightjamsSpoutReceiver>,
	public ISupportErrorInfo,
	public IDispatchImpl<ILightjamsSpoutReceiver, &IID_ILightjamsSpoutReceiver, &LIBID_LightjamsSpoutLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CLightjamsSpoutReceiver();	
	virtual ~CLightjamsSpoutReceiver();

DECLARE_REGISTRY_RESOURCEID(IDR_LightjamsSpoutRECEIVER)


BEGIN_COM_MAP(CLightjamsSpoutReceiver)
	COM_INTERFACE_ENTRY(ILightjamsSpoutReceiver)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	STDMETHOD(NbSenders)(int *pCount);
	STDMETHOD(GetSenderInfo)(int index, BSTR *pName, int *width, int *height);
	STDMETHOD(Connect)(BSTR senderName, int *width, int *height, BSTR *realName);
	STDMETHOD(Disconnect)();
	STDMETHOD(ReceiveImage)(SAFEARRAY *bytes, EPixelFormat format);

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
	void InitTexture(GLuint &texID, GLenum GLformat, unsigned int width, unsigned int height);

	SpoutReceiver _receiver;
	bool _isCreated;
	int _width, _height;	
	char _senderName[256];
	GLuint _glTexture;
};

OBJECT_ENTRY_AUTO(__uuidof(LightjamsSpoutReceiver), CLightjamsSpoutReceiver)
