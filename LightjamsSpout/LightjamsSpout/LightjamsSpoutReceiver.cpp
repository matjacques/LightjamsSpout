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

#include "stdafx.h"
#include "LightjamsSpoutReceiver.h"
#include "comutil.h"

// CLightjamsSpoutReceiver

CLightjamsSpoutReceiver::CLightjamsSpoutReceiver()
{	
	_isCreated = false;	
	_glTexture = 0;
	_glContext = 0;
	_hdc = 0;
}

CLightjamsSpoutReceiver::~CLightjamsSpoutReceiver()
{
	Disconnect();
}

STDMETHODIMP CLightjamsSpoutReceiver::Disconnect()
{	
	if (_hdc != 0)
	{	
		wglMakeCurrent(_hdc, 0);
	}

	if (_glTexture != 0)
	{
		glDeleteTextures(1, &_glTexture);
		_glTexture = 0;
	}
	
	if (_glContext != 0)
	{	
		wglDeleteContext(_glContext);
		_glContext = 0;
	}	
	
	_hdc = 0;
	
	if (_isCreated)
	{
		_receiver.ReleaseReceiver();
		_isCreated = false;
	}

	return S_OK;
}

STDMETHODIMP CLightjamsSpoutReceiver::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* const arr[] = 
	{
		&IID_ILightjamsSpoutReceiver
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

void CLightjamsSpoutReceiver::InitOpenGL()
{
	HDC hdc = NULL;
	HWND hwnd = NULL;
	HWND hwndButton = NULL;
	HGLRC hRc = NULL;

	HGLRC glContext = wglGetCurrentContext();

	if (glContext == NULL) {

		// We only need an OpenGL context with no render window because we don't draw to it
		// so create an invisible dummy button window. This is then independent from the host
		// program window (GetForegroundWindow). If SetPixelFormat has been called on the
		// host window it cannot be called again. This caused a problem in Mapio.
		// https://msdn.microsoft.com/en-us/library/windows/desktop/dd369049%28v=vs.85%29.aspx
		//
		if (!hwndButton || !IsWindow(hwndButton)) {
			hwndButton = CreateWindowA("BUTTON",
				"SpoutOpenGL",
				WS_OVERLAPPEDWINDOW,
				0, 0, 32, 32,
				NULL, NULL, NULL, NULL);
		}

		if (!hwndButton) {
			throw std::exception("ERROR_OPEN_GL_NO_WINDOW");			
		}

		
		
		hdc = GetDC(hwndButton);
		if (!hdc) {
			// printf("InitOpenGL error 2\n"); 
			throw std::exception("ERROR_OPEN_GL_NO_DC");
		}

		PIXELFORMATDESCRIPTOR pfd;
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 16;
		pfd.iLayerType = PFD_MAIN_PLANE;
		int iFormat = ChoosePixelFormat(hdc, &pfd);
		if (!iFormat) {
			// printf("InitOpenGL error 3\n"); 
			throw std::exception("ERROR_OPEN_GL_NO_PIXEL_FORMAT");
		}

		if (!SetPixelFormat(hdc, iFormat, &pfd)) {
			DWORD dwError = GetLastError();
			// printf("InitOpenGL error 4 (Error %d (%x))\n", dwError, dwError); 
			// 2000 (0x7D0) The pixel format is invalid.
			// Caused by repeated call of  the SetPixelFormat function
			throw std::exception("ERROR_OPEN_GL_SET_PIXEL_FORMAT");
		}

		hRc = wglCreateContext(hdc);
		if (!hRc) {
			// printf("InitOpenGL error 5\n"); 
			throw std::exception("ERROR_OPEN_GL_CREATE_CONTEXT");
		}

		wglMakeCurrent(hdc, hRc);
		if (wglGetCurrentContext() == NULL) {
			// printf("InitOpenGL error 6\n");
			throw std::exception("ERROR_OPEN_GL_NO_CURRENT_CONTEXT");
		}
	}

	_glContext = hRc;
	_hdc = hdc;	
}

GLuint CLightjamsSpoutReceiver::CreateTexture(GLenum GLformat, unsigned int width, unsigned int height)
{
	GLuint texID;

	// Create a texture buffer	
	glGenTextures(1, &texID);

	if (texID == 0)
	{
		throw std::exception("ERROR_INIT_TEXTURE");
	}

	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GLformat, width, height, 0, GLformat, GL_UNSIGNED_BYTE, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

STDMETHODIMP CLightjamsSpoutReceiver::NbSenders(int *pCount)
{	
	*pCount = _receiver.GetSenderCount();
	return S_OK;
}

STDMETHODIMP CLightjamsSpoutReceiver::GetSenderInfo(int index, BSTR *pName, int *width, int *height)
{	
	char name[256];
	
	unsigned int w, h;	

	if (!_receiver.GetSenderName(index, name))
	{
		return Error(_T("ERROR_SENDER_NOT_FOUND"), __uuidof(ILightjamsSpoutReceiver), E_FAIL);
	}		
	
	HANDLE handle;
	DWORD format;
	
	if (!_receiver.GetSenderInfo(name, w, h, handle, format))
	{
		return Error(_T("ERROR_SENDER_INFO"), __uuidof(ILightjamsSpoutReceiver), E_FAIL);
	}	
	
	*pName = _com_util::ConvertStringToBSTR(name);
	*width = w;
	*height = h;

	return S_OK;
}

STDMETHODIMP CLightjamsSpoutReceiver::Connect(BSTR senderName, int *width, int *height, BSTR *realName)
{	
	if (_isCreated)
	{
		return Error(_T("ERROR_ALREADY_CONNECTED"), __uuidof(ILightjamsSpoutReceiver), E_FAIL);
	}

	std::string requestedName(_com_util::ConvertBSTRToString(senderName));
	char name[256];
	name[0] = 0;

	strcpy_s(name, 256, requestedName.c_str());	

	bool useMemoryMode = false;
	unsigned int w, h;

	if (name[0] == 0)
	{
		if (!_receiver.GetImageSize(name, w, h, useMemoryMode))
		{
			return Error(_T("ERROR_SENDER_NOT_FOUND"), __uuidof(ILightjamsSpoutReceiver), E_FAIL);
		}
	}
	else
	{
		HANDLE handle;
		DWORD format;
		if (!_receiver.GetSenderInfo(name, w, h, handle, format))
		{
			return Error(_T("ERROR_SENDER_NOT_FOUND"), __uuidof(ILightjamsSpoutReceiver), E_FAIL);
		}
	}

	*realName = _com_util::ConvertStringToBSTR(name);

	if (useMemoryMode)
	{
		if (!_receiver.SetMemoryShareMode(true))
		{
			return Error(_T("ERROR_SET_MEMORY_MODE"), __uuidof(ILightjamsSpoutReceiver), E_FAIL);
		}
	}
	
	_width = w;
	_height = h;
	*width = w;
	*height = h;
	strcpy_s(_senderName, 256, name);

	return S_OK;
	
}

STDMETHODIMP CLightjamsSpoutReceiver::ReceiveImage(SAFEARRAY *bytes, EPixelFormat format)
{	
	try
	{
		if (bytes->cDims != 1)
		{
			return Error(_T("ERROR_ARRAY_MUST_BE_ONE_DIMENSION"), __uuidof(ILightjamsSpoutReceiver), E_FAIL);
		}

		// for .Net and (by default) for OpenGL, each bitmap row must be aligned to a 4 byte boundary
		const int bytesPerPixel = 3;	// RGB
		int stride = 4 * ((_width * bytesPerPixel + 3) / 4);

		if (bytes->rgsabound[0].cElements < stride * _height)
		{
			return Error(_T("ERROR_ARRAY_TOO_SMALL"), __uuidof(ILightjamsSpoutReceiver), E_FAIL);
		}		

		ReceiveImage(bytes->pvData, format);
		return S_OK;
	}
	catch (const std::exception &e)
	{
		return Error(e.what(), __uuidof(ILightjamsSpoutReceiver), E_FAIL);
	}	
}

STDMETHODIMP CLightjamsSpoutReceiver::ReceiveImageIntPtr(LONG_PTR bitmapIntPtr, EPixelFormat format)
{
	try
	{
		ReceiveImage((void*)bitmapIntPtr, format);
		return S_OK;
	}
	catch (const std::exception &e)
	{
		return Error(e.what(), __uuidof(ILightjamsSpoutReceiver), E_FAIL);
	}
}

void CLightjamsSpoutReceiver::ReceiveImage(void* buffer, EPixelFormat format)
{
	unsigned int w = _width, h = _height;

	if (!_isCreated)
	{
		// It's important to create the OpenGL context in the same thread that will be using it later.
		// Otherwise, it crashes at every OpenGL call!
		InitOpenGL();

		// the texture's format must be RGB. Creating with BGR gives blank images...
		_glTexture = CreateTexture(GL_RGB, _width, _height);
		
		if (!_receiver.CreateReceiver(_senderName, w, h))
		{
			throw std::exception("ERROR_CREATE_RECEIVER");			
		}
		
		_isCreated = true;		
	}
	
	if (_receiver.ReceiveTexture(_senderName, w, h, _glTexture, GL_TEXTURE_2D))
	{
		if (w != _width || h != _height)
		{
			// the caller needs to update his buffer... 
			throw std::exception("SIZE_CHANGED");			
		}
		else
		{
			GLenum glFormat = (format == EPixelFormat::RGB) ? GL_RGB : GL_BGR;
			glBindTexture(GL_TEXTURE_2D, _glTexture);
			glEnable(GL_TEXTURE_2D);
			glGetTexImage(GL_TEXTURE_2D, 0, glFormat, GL_UNSIGNED_BYTE, buffer);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
		}
	}
	else
	{
		throw std::exception("ERROR_SENDER_NOT_FOUND");		
	}	
}
