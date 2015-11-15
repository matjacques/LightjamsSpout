// GLContext.cpp : Implementation of CGLContext

#include "stdafx.h"
#include "GLContext.h"
#include "comutil.h"

// CGLContext

STDMETHODIMP CGLContext::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* const arr[] = 
	{
		&IID_IGLContext
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CGLContext::CGLContext()
{
	_isCreated = false;
	_hwnd = 0;
	_hdc = 0;
	_glContext = 0;
}

CGLContext::~CGLContext()
{
	DeleteOpenGL();
}

STDMETHODIMP CGLContext::Create()
{
	try
	{
		InitOpenGL();
		return S_OK;
	}
	catch (const std::exception &e)
	{
		return Error(e.what(), __uuidof(IGLContext), E_FAIL);
	}
}

STDMETHODIMP CGLContext::Delete()
{
	try
	{
		DeleteOpenGL();
		return S_OK;
	}
	catch (const std::exception &e)
	{
		return Error(e.what(), __uuidof(IGLContext), E_FAIL);
	}
}

void CGLContext::InitOpenGL()
{
	if (_isCreated == true)
	{
		throw std::exception("ERROR_ALREADY_CREATED");
	}

	HDC hdc = NULL;	
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
	_hwnd = hwndButton;
	_isCreated = true;
}

void CGLContext::DeleteOpenGL()
{
	if (!_isCreated) return;

	if (_hdc != 0)
	{
		wglMakeCurrent(_hdc, 0);
	}

	if (_glContext != 0)
	{
		wglDeleteContext(_glContext);
		_glContext = 0;
	}

	if (_hwnd != 0 && _hdc)
	{
		ReleaseDC(_hwnd, _hdc);
	}

	_hdc = 0;

	_isCreated = false;
}
