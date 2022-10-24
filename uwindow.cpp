#include "uwindow.h"
#include <Windows.h>
#include <iostream>
#include <vector>

BOOL UMainWindow::Init(
    PCWSTR lpWindowName,
    int nWidth,
    int nHeight,
    int x,
    int y,
    DWORD dwStyle,
    DWORD dwExStyle,
    HWND hWndParent,
    HMENU hMenu 
    ) 
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(NULL);;
    //wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    //wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WIN32);
    wcex.lpszClassName = ClassName();
    //wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    hWnd_ = CreateWindowEx(
        dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
        nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
    );

    BOOL rst = hWnd_ != NULL ? TRUE : FALSE;

    if (rst) {
        IncWindow();
    }

    SetBkMode(hMemDC_, TRANSPARENT);
    ::SetTextColor(hMemDC_, RGB(0, 0, 0));

    //��������
    LOGFONT logfont;
    ZeroMemory(&logfont, sizeof(LOGFONT));
    logfont.lfCharSet = DEFAULT_CHARSET;
    logfont.lfHeight = 40;
    logfont.lfWidth = 40;

    HFONT hFont = CreateFontIndirect(&logfont);
    SelectObject(hMemDC_, hFont);
    DeleteObject(hFont);

    return rst;
}

//GDI+ DrawImageЧ��̫�ÿ��һ֡��Ҫ30ms����
void UMainWindow::Draw() {

    if (nullptr == pDrawFunc_ || NULL == hWnd_) {
        return;
    }
    
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd_, &ps);

    //FillRect(hMemDC_, &clientRect_, (HBRUSH)BLACK_PEN);
    pDrawFunc_();

    BitBlt(hdc, 0, 0, clientRect_.right - clientRect_.left, clientRect_.bottom - clientRect_.top, hMemDC_, 0, 0, SRCCOPY);

    EndPaint(hWnd_, &ps);
}

void UMainWindow::Refresh() {
    //InvalidateRect(hWnd_, NULL, FALSE);
    observer_.Notify(NotifyType::REFRESH_ONLY);
    UpdateWindow(hWnd_);
}

void UMainWindow::PrintText(int posx, int posy, char text[]) {
    TextOutA(hMemDC_, posx, posy, text, strlen(text));
}

void UMainWindow::SetTextColor(COLORREF color) {
    ::SetTextColor(hMemDC_, color);
}

void UMainWindow::SetFontSize(int width, int height) {
    //��������
    LOGFONT logfont;
    ZeroMemory(&logfont, sizeof(LOGFONT));
    logfont.lfCharSet = DEFAULT_CHARSET;
    logfont.lfHeight = height;
    logfont.lfWidth = width;

    HFONT hFont = CreateFontIndirect(&logfont);
    SelectObject(hMemDC_, hFont);
    DeleteObject(hFont);
}

LRESULT UMainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        GetClientRect(hWnd_, &clientRect_);
        hMemDC_ = CreateCompatibleDC(GetDC(hWnd_));
        hMemBmp_ = CreateCompatibleBitmap(GetDC(hWnd_), clientRect_.right - clientRect_.left, clientRect_.bottom - clientRect_.top);
        SelectObject(hMemDC_, hMemBmp_);

        SetTimer(hWnd_, 1, 1000, NULL);

        return DefWindowProc(hWnd_, uMsg, wParam, lParam);
    }
    case WM_TIMER: {
        InvalidateRect(hWnd_, NULL, TRUE);
        return 0;
    }
    case WM_CLOSE: {
        DescWindow();
        DestroyWindow(hWnd_);
        return 0;
    }
    case WM_DESTROY: {
        //uwindowȫ�����رպ󣬲ŷ���WM_QUIT��Ϣ�˳�����
        KillTimer(hWnd_, 1);
        if (0 == UMainWindow::windowsNum) {
            PostQuitMessage(0);
        }
        return 0;
    }
    case WM_SIZE: {
        GetClientRect(hWnd_, &clientRect_);
        return DefWindowProc(hWnd_, uMsg, wParam, lParam);
    }
    case WM_PAINT: {
        //FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        Draw();
        /*Show(SW_HIDE);
        Show(SW_SHOW);*/
        return 0;
    }
    case WM_ERASEBKGND: {
        return 0;
    }
    case WM_CTLCOLOREDIT: {
        SendMessage((HWND)lParam, uMsg, wParam, lParam);
        return 0;
    }
    case WM_CTLCOLORBTN: {
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }
    case WM_DRAWITEM: {
        return DefWindowProc(hWnd_, uMsg, wParam, lParam);
    }
    case WM_COMMAND: {
        SendMessage((HWND)lParam, uMsg, wParam, lParam);
        return 0;
    }
    default:
        return DefWindowProc(hWnd_, uMsg, wParam, lParam);
    }
    return TRUE;
}

UButton::UButton(UMainWindow& uwindow, LPCTSTR buttonText, int posx, int posy, unsigned int width, unsigned int height) :
    pImage_(nullptr), textColor_(RGB(255, 255, 255)) 
{

    //������ť�ؼ�
    hWnd_ = CreateWindow(
        TEXT("BUTTON"),
        buttonText,
        WS_CHILD | BS_OWNERDRAW | BS_BITMAP,
        posx, posy, width, height,
        uwindow.GetHWnd(),
        (HMENU)IDC::IDC_BUTTON,
        NULL,
        this
    );

    /*ÿ�����ڶ������Լ���ʵ�����ݣ���˿��Դ���������ڣ���Ϊÿ����
    ���ṩ���Լ������ݽṹ��ʵ���� ����㶨��һ�������ಢ���������
    ������ڣ����磬��������Զ���ؼ��ࣩ����˷����ر����á�*/

    oldProc_ = NULL;

    if (hWnd_ != NULL) {
        SetWindowLongPtr(hWnd_, GWLP_USERDATA, (LONG_PTR)this); //��������ָ����Ϣ�洢�ڴ��ڵ�ʵ������
        oldProc_ = (WNDPROC)SetWindowLong(hWnd_, GWL_WNDPROC, (LONG)WindowProc);
    }

    brushStyle_ = LTGRAY_BRUSH;
    buttonText_ = buttonText;
    posx_ = static_cast<float>(posx);
    posy_ = static_cast<float>(posy);
    width_ = width;
    height_ = height;
    paintRect_ = { posx, posy, long(posx + width), long(posy + height) };

    uwindow.GetObserver().AddObserved(this); //���Լ���Ϊ�����ڵı��۲���
}

void UButton::SetBkImage(const wchar_t* imgName) {
    pImage_ = std::make_shared<Gdiplus::Image>(imgName);
    //��ͼƬֱ�Ӱ󶨵���ť�ؼ��������Ϊ����
    //��ť�������ƣ�ò��Ҳ�ὫͼƬ������ǰ��Ȼ�󱻰�ť�Լ��ڵ�
    //����ͨ������һЩ����ģ����ŵĿ����λͼ��ͼ�꣨BITMAP AND ICON����ʵ�֣�����ȷ���Ըߣ������ƻ��ܹ�
    //����ʵ�ַ�����ͨ����ͼƬ�󶨵��˰�ť�ؼ��ĸ������ϣ�������ͳߴ��ƥ�䰴ť
}

bool UButton::BindEvent(const UBUTTON_MSG& btnMsg, void(*EventFunc)()) {
    //���_btnMsg�Ƿ���UBUTTON_MSG��ö��ֵ
    if (btnMsg < UBUTTON_MSG::NOMSG || btnMsg > UBUTTON_MSG::MOUSE_RUP) {
        return false;
    }

    btnEventMap_[btnMsg] = EventFunc;

    return true;
}

bool UButton::Event(const UBUTTON_MSG& btnMsg) const {
    try {
        btnEventMap_.at(btnMsg)();
        return true;
    }
    catch (std::out_of_range e) {
        return false;
    }
}

void UButton::ObservedFunc(NotifyType notify) {

    switch (notify) {
    case NotifyType::INVISIBLE: {
        ShowWindow(hWnd_, false);
        break;
    }
    case NotifyType::VISIBLE: {
        ShowWindow(hWnd_, true);
        break;
    }
    default:
        break;
    }
}

LRESULT UButton::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static bool mouseLeave = true; //��ʾ����Ƿ��뿪�˰�ť

    switch (uMsg)
    {
    case WM_KEYDOWN: {
        return 0;
    }
    case WM_LBUTTONDOWN: {
        
        if (nullptr != pImage_) {

        }
        else {
            brushStyle_ = DKGRAY_BRUSH;
            textColor_ = RGB(0, 0, 0);
            InvalidateRect(hWnd_, &paintRect_, true);
            UpdateWindow(hWnd_);
        }

        Event(UBUTTON_MSG::MOUSE_LDOWN);

        return 0;
    }
    case WM_LBUTTONUP: {
        if (nullptr != pImage_) {

        }
        else {
            brushStyle_ = GRAY_BRUSH;
            textColor_ = RGB(0, 0, 0);
            InvalidateRect(hWnd_, &paintRect_, true);
            UpdateWindow(hWnd_);
        }

        Event(UBUTTON_MSG::MOUSE_LUP);

        return 0;
    }
    case WM_MOUSELEAVE: {

        if (nullptr != pImage_) {

        }
        else {
            brushStyle_ = LTGRAY_BRUSH;
            textColor_ = RGB(255, 255, 255);
            InvalidateRect(hWnd_, &paintRect_, true);
            UpdateWindow(hWnd_);
        }

        mouseLeave = true;

        return 0;
    }
    case WM_MOUSEMOVE: {

        if (!mouseLeave) {
            return 0;
        }

        if (nullptr != pImage_) {
          
        }
        else {
            //׷�ٴ˴�����ƶ�
            TRACKMOUSEEVENT trackLeave;
            trackLeave.cbSize = sizeof(TRACKMOUSEEVENT);
            trackLeave.dwFlags = TME_LEAVE | TME_HOVER;
            trackLeave.hwndTrack = hWnd_;
            TrackMouseEvent(&trackLeave);

            brushStyle_ = GRAY_BRUSH;
            textColor_ = RGB(0, 0, 0);

            InvalidateRect(hWnd_, &paintRect_, true);
            UpdateWindow(hWnd_);
        }

        return 0;
    }
    case WM_PAINT: {
        //gdi+ + ʹ����Ӧ��ťHDC���ڴ滺����ʵ��˫����
        /*HDC hMemDC = CreateCompatibleDC(hdc);
        Gdiplus::Graphics g(hMemDC);
        Gdiplus::Image img(L"button1.png");

        g.DrawImage(&img, 0, 0, width_, height_);

        StretchBlt(hdc, 0, 0, width_, height_, hMemDC, 0, 0, width_, height_, SRCCOPY);*/

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd_, &ps);
        //gdi+ + ʹ��λͼ��Ϊ�󱸻�������˫����
        paintRect_ = ps.rcPaint;

        if (nullptr != pImage_) {
            Gdiplus::Bitmap bm(width_, height_);
            Gdiplus::Graphics g(&bm);

            g.DrawImage(pImage_.get(), 0, 0, width_, height_);

            Gdiplus::Graphics g2(hdc);
            g2.DrawImage(&bm, 0, 0);

            g.ReleaseHDC(hdc);
        }
        else {
            FillRect(hdc, &ps.rcPaint, (HBRUSH)GetStockObject(brushStyle_));
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, textColor_);
            DrawText(hdc, buttonText_, -1, &ps.rcPaint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        EndPaint(hWnd_, &ps);

        //return CallWindowProc(oldProc_, hWnd_, uMsg, wParam, lParam);
        return 0;
    }
    default:

        return CallWindowProc(oldProc_, hWnd_, uMsg, wParam, lParam);
    }
    return TRUE;
};

UEdit::UEdit(UMainWindow& uwindow, int posx, int posy, unsigned int width, unsigned int height) :
    textColor_(RGB(0, 0, 0)), bkColor_(RGB(0, 0, 255)) {

    //�����Ի��Ƶ���EDIT�ؼ�
    hWnd_ = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        TEXT("EDIT"),
        TEXT(""),
        WS_CHILD | BS_OWNERDRAW,
        posx, posy, width, height,
        uwindow.GetHWnd(),
        (HMENU)IDC::IDC_EDIT,
        NULL,
        this
    );

    memset(text_, 0, sizeof(text_));

    oldProc_ = NULL;

    if (hWnd_ != NULL) {
        SetWindowLongPtr(hWnd_, GWLP_USERDATA, (LONG_PTR)this); //��������ָ����Ϣ�洢�ڴ��ڵ�ʵ������
        oldProc_ = (WNDPROC)SetWindowLong(hWnd_, GWL_WNDPROC, (LONG)WindowProc);
    }

    brushStyle_ = LTGRAY_BRUSH;
    posx_ = static_cast<float>(posx);
    posy_ = static_cast<float>(posy);
    width_ = width;
    height_ = height;
    paintRect_ = { posx, posy, long(posx + width), long(posy + height) };
    
    uwindow.GetObserver().AddObserved(this); //���Լ���Ϊ�����ڵı��۲���

    //��������
    LOGFONT logfont;
    ZeroMemory(&logfont, sizeof(LOGFONT));
    logfont.lfCharSet = DEFAULT_CHARSET;
    logfont.lfHeight = height_ - 10;
    logfont.lfWidth = height_ - 18;
    hFont_ = CreateFontIndirect(&logfont);

    //������Ϣʹ�༭��ʹ������
    SendMessage(hWnd_, WM_SETFONT, (WPARAM)hFont_, NULL); 
}

UEdit::~UEdit() {
    DeleteObject(hFont_);
}

char* UEdit::GetText() {
    return text_;
}

void UEdit::SetFontSize(unsigned int width, unsigned int height) {
    //��������
    LOGFONT logfont;
    ZeroMemory(&logfont, sizeof(LOGFONT));
    logfont.lfCharSet = DEFAULT_CHARSET;
    logfont.lfHeight = height;
    logfont.lfWidth = width;
    hFont_ = CreateFontIndirect(&logfont);
    fontSize_ = height;
    //������Ϣʹ�༭��ʹ������
    SendMessage(hWnd_, WM_SETFONT, (WPARAM)hFont_, NULL);
}

void UEdit::ObservedFunc(NotifyType notify) {
    switch (notify) {
    case NotifyType::INVISIBLE: {
        ShowWindow(hWnd_, false);
        break;
    }
    case NotifyType::VISIBLE: {
        ShowWindow(hWnd_, true);
        break;
    }
    default:
        break;
    }
}

LRESULT UEdit::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg)
    {
    case WM_PAINT: {

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd_, &ps);

        //paintRect_ = ps.rcPaint;
        SetBkMode(hdc, TRANSPARENT);
        SetBkColor(hdc, bkColor_);
        SetTextColor(hdc, textColor_);
        SelectObject(hdc, hFont_);
        
        FillRect(hdc, &ps.rcPaint, (HBRUSH)GetStockObject(WHITE_PEN));
        //���������Խ���༭���ı�����ʾ����(EDIT�Ի�������ܶ࣬ʮ���鷳)
        //TextOutA(hdc, 0, 0, text_, 0);
        DrawTextA(hdc, text_, -1, &ps.rcPaint, NULL);
        EndPaint(hWnd_, &ps);

        return 0;
    }
    case WM_COMMAND: {

        switch (HIWORD(wParam))
        {
        case EN_CHANGE: {
            SendMessageA(hWnd_, WM_GETTEXT, 128, (LPARAM)text_); //��ȡ�༭���ı�������������
            break;
        }
        default:
            break;
        }
        return 0;
    }
    case WM_CTLCOLOREDIT: {
        
        return 0;
    }
    default:
        
        return CallWindowProc(oldProc_, hWnd_, uMsg, wParam, lParam);
    }

    return 0;
}