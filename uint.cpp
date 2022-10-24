#include "uinit.h"
#include <gdiplus.h>
#include "ucommon.h"

bool Unicorn::InitStatus = false;
Unicorn* Unicorn::pInstance_ = new Unicorn();
ULONG_PTR Unicorn::pGdiplusToken_ = NULL;
Unicorn::DeUnicorn Unicorn::deUnicorn_;
UWINDOW_MSG Unicorn::msg_ = UWINDOW_MSG::NOMSG;

bool Unicorn::Initialize() {
	if (InitStatus) {
		OutputDebugString(TEXT("error: Repeat initialization.\n"));
		return false;
	}

	//初始化gdi+
	Gdiplus::GdiplusStartupInput gdiplusStartupInput = 0;
	int ret = (int)Gdiplus::GdiplusStartup(&pGdiplusToken_, &gdiplusStartupInput, NULL);
    
	if (ret != 0) {
		OutputDebugString(TEXT("error: Initialize GDI+ failed.\n"));
		return false;
	}
	//初始化WIC
    ret = (int)CoInitialize(NULL);

	if (ret < 0) {
		OutputDebugString(TEXT("error: Initialize WIC failed.\n"));
		return false;
	}

	InitStatus = true;
	return true;
}

Unicorn::~Unicorn() {
	if (InitStatus) {
		Gdiplus::GdiplusShutdown(pGdiplusToken_);
	}
}

Unicorn* Unicorn::GetInstance() {
	return pInstance_;
}

void Unicorn::MessageProc() {

	static MSG msg = { 0 };

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		switch (msg.message) {
		default: {
			msg_ = UWINDOW_MSG::NOMSG;
			break;
		}
		case WM_MOUSEWHEEL: {
			//滑轮前HIWORD(msg.wParam)120, 后65416
			if (65416 == HIWORD(msg.wParam)) {
				msg_ = UWINDOW_MSG::MOUSEWHEEL_BACK;
			}
			else {
				msg_ = UWINDOW_MSG::MOUSEWHEEL_RUSH;
			}
			break;
		}
		case WM_LBUTTONDOWN: {
			msg_ = UWINDOW_MSG::MOUSE_LDOWN;
			break;
		}
		case WM_LBUTTONUP: {
			msg_ = UWINDOW_MSG::MOUSE_LUP;
			break;
		}
		case WM_RBUTTONDOWN: {
			msg_ = UWINDOW_MSG::MOUSE_RDOWN;
			break;
		}
		case WM_RBUTTONUP: {
			msg_ = UWINDOW_MSG::MOUSE_RUP;
			break;
		}
		case WM_MBUTTONDOWN: {
			msg_ = UWINDOW_MSG::MOUSE_MDOWN;
			break;
		}
		case WM_MBUTTONUP: {
			msg_ = UWINDOW_MSG::MOUSE_MUP;
			break;
		}
		case WM_LBUTTONDBLCLK: {
			msg_ = UWINDOW_MSG::MOUSE_LDCLICK;
			break;
		}
		case WM_RBUTTONDBLCLK: {
			msg_ = UWINDOW_MSG::MOUSE_RDCLICK;
			break;
		}
		case WM_MBUTTONDBLCLK: {
			msg_ = UWINDOW_MSG::MOUSE_MDCLICK;
			break;
		}
		case WM_QUIT: {
			msg_ = UWINDOW_MSG::QUIT;
			break;
		}
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}