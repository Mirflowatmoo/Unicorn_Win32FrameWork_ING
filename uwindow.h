#pragma once
#include <Windows.h>
#include <map>
#include <memory>
#include "unicorn.h"

#define MAX_LOADSTRING 100

template <typename DERIVED_TYPE>
class UWindow {
public:
    UWindow() : hWnd_(NULL), pDrawFunc_(nullptr), hMemBmp_(NULL), hMemDC_(NULL) { };
    UWindow(const UWindow<DERIVED_TYPE>&) = delete;
    UWindow<DERIVED_TYPE>& operator=(const UWindow<DERIVED_TYPE>&) = delete;

    virtual ~UWindow() {
        if (hMemBmp_ != NULL) {
            DeleteObject(hMemBmp_);
        }
        if (hMemDC_ != NULL) {
            DeleteDC(hMemDC_);
        }
    };

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        
        DERIVED_TYPE* pThis = NULL;

        if (uMsg == WM_NCCREATE) {
            //CreateWindow发送WM_NCCREATE消息
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            pThis = reinterpret_cast<DERIVED_TYPE*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

            pThis->hWnd_ = hWnd;
        }
        else {
            //读取窗口的实例数据
            pThis = reinterpret_cast<DERIVED_TYPE*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)); 
        }
       
        if (pThis) {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }

    BOOL Show(int nCmdShow = SW_SHOW) const {
        /*SW_FORCEMINIMIZE：在WindowNT5.0中最小化窗口，即使拥有窗口的线程被挂起也会最小化。在从其他线程最小化窗口时才使用这个参数。
    　　SW_HIDE：隐藏窗口并激活其他窗口。
    　　SW_MAXIMIZE：最大化指定的窗口。
    　　SW_MINIMIZE：最小化指定的窗口并且激活在Z序中的下一个顶层窗口。
    　　SW_RESTORE：激活并显示窗口。如果窗口最小化或最大化，则系统将窗口恢复到原来的尺寸和位置。在恢复最小化窗口时，应用程序应该指定这个标志。
    　　SW_SHOW：在窗口原来的位置以原来的尺寸激活和显示窗口。
    　　SW_SHOWDEFAULT：依据在STARTUPINFO结构中指定的SW_FLAG标志设定显示状态，STARTUPINFO 结构是由启动应用程序的程序传递给CreateProcess函数的。
    　　SW_SHOWMAXIMIZED：激活窗口并将其最大化。
    　　SW_SHOWMINIMIZED：激活窗口并将其最小化。
    　　SW_SHOWMINNOACTIVE：窗口最小化，激活窗口仍然维持激活状态。*/
        return ShowWindow(hWnd_, nCmdShow);
    }

    //为窗口绑定场景(画布)
    void BindScene(void (*pDrawFunc)()) {

        pDrawFunc_ = pDrawFunc;
        observer_.Notify(NotifyType::INVISIBLE); //通知各子对象一个false消息，表示场景切换

        //使用白色画刷填充内存DC
        HBRUSH whiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH); 
        FillRect(hMemDC_, &clientRect_, whiteBrush);
        InvalidateRect(hWnd_, NULL, true);
    }

    HDC GetMemDC() const { return hMemDC_; }
    
    HWND GetHWnd() const { return hWnd_; }

    //设置鼠标指针图标
    bool SetMouseCursor(const wchar_t* cursorPath) {

        HCURSOR hCur = NULL;
        hCur = (HCURSOR)LoadImage(NULL, cursorPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);

        if (NULL == hCur) {
            return false;
        }

        DWORD rs = 0;
        rs = SetClassLong(hWnd_, GCLP_HCURSOR, (long)hCur);

        if (0 == rs) {
            return false;
        }

        return true;
    }

    //隐藏标题栏
    bool HideWindowTitleBar() {
        long lStyle = 0;
        lStyle = GetWindowLong(hWnd_, GWL_STYLE); //获取窗口风格

        if (0 == lStyle) {
            return false;
        }

        SetWindowLong(hWnd_, GWL_STYLE, lStyle & ~WS_CAPTION); //设置新的窗口风格
        GetWindowRect(hWnd_, &clientRect_);
        
        BOOL rs = FALSE;
        rs = SetWindowPos(hWnd_, NULL, clientRect_.left, clientRect_.top, clientRect_.right - clientRect_.left, clientRect_.bottom - clientRect_.top,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

        if (FALSE == rs) {
            return false;
        }

        return true;
    }

    //显示标题栏
    bool ShowWindowTitleBar() {
        long lStyle = 0;

        lStyle = GetWindowLong(hWnd_, GWL_STYLE); //获取窗口风格

        if (0 == lStyle) {
            return false;
        }

        SetWindowLong(hWnd_, GWL_STYLE, lStyle | WS_CAPTION); //设置新的窗口风格
        GetClientRect(hWnd_, &clientRect_);

        BOOL rs = FALSE;
        rs = SetWindowPos(hWnd_, NULL, 0, 0, 0, 0,
            SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);

        if (FALSE == rs) {
            return false;
        }

        return true;
    }

    //获取进程窗口总数量
    static UINT GetWindowsNum() { return windowsNum; }

    //获取观察者属性
    UObserver<NotifyType>& GetObserver() { return observer_; }

protected:

	HWND hWnd_;
    //HINSTANCE hInst_;                                // 当前实例
    //WCHAR szTitle_[MAX_LOADSTRING];                  // 标题栏文本
    //WCHAR szWindowClass_[MAX_LOADSTRING];            // 主窗口类名
    HBITMAP hMemBmp_;
    HDC hMemDC_;
    RECT clientRect_; 
    UObserver<NotifyType> observer_; //暂时使用bool信息
    void (*pDrawFunc_)();
    static UINT windowsNum; //记录uwindow的数量
    float posx_, posy_;
    unsigned int width_, height_;

protected:

    virtual PCWSTR ClassName() const = 0;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    static void IncWindow() {
        windowsNum++;
    }

    static void DescWindow() {
        windowsNum--;
    }
};

template<class DERIVED_TYPE>
UINT UWindow<DERIVED_TYPE>::windowsNum = 0;

class UMainWindow : public UWindow<UMainWindow> {
public:

    UMainWindow() = default;
    UMainWindow(const UMainWindow&) = delete;
    UMainWindow& operator = (const UMainWindow&) = delete;

    ~UMainWindow() { };

    BOOL Init(
        PCWSTR lpWindowName,
        int nWidth = 960,
        int nHeight = 540,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        DWORD dwStyle = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
        DWORD dwExStyle = 0,
        HWND hWndParent = 0,
        HMENU hMenu = 0
        );

    //刷新窗口状态
    void Refresh();
    //打印文字
    void PrintText(int posx, int posy, char text[]);
    //设置文本颜色
    void SetTextColor(COLORREF color);
    //设置字体尺寸
    void SetFontSize(int width, int height);

    PCWSTR ClassName() const { return L"Sample Window Class"; }

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:

    void Draw();
    
};

class UButton : public UWindow<UButton>, public UObserved<NotifyType> {
public:

    UButton(UMainWindow& uwindow, LPCTSTR buttonText, int posx = 0, int posy = 0, unsigned int width = 100, unsigned int height = 30);

    UButton(const UButton&) = delete;
    UButton& operator=(const UButton&) = delete;
    ~UButton() = default;

    void BindScene(void (*pDrawFunc)()) = delete;

    //设置背景图片
    void SetBkImage(const wchar_t* imgName);
    //绑定按钮事件
    bool BindEvent(const UBUTTON_MSG& btnMsg, void(*EventFunc)());

    friend static LRESULT CALLBACK UWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    
    int brushStyle_;
    COLORREF textColor_;
    RECT paintRect_;
    LPCTSTR buttonText_;
    WNDPROC oldProc_;
    std::shared_ptr<Gdiplus::Image> pImage_;
    std::map<UBUTTON_MSG, void(*)()> btnEventMap_;

private:
    bool Event(const UBUTTON_MSG& btnMsg) const; 
    virtual PCWSTR ClassName() const override { return TEXT("BUTTON"); };
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    virtual void ObservedFunc(NotifyType notify) override;
};

class UEdit : public UWindow<UEdit>, public UObserved<NotifyType> {

public:

    UEdit(UMainWindow& uwindow, int posx, int posy, unsigned int width, unsigned int height);
    UEdit(const UEdit&) = delete;
    UEdit& operator=(const UEdit&) = delete;
    ~UEdit();

    //绑定按钮事件
    void BindScene(void (*pDrawFunc)()) = delete;
    //获取编辑框文字内容
    char* GetText();
    void SetFontSize(unsigned int width, unsigned int height);

    friend static LRESULT CALLBACK UWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

    int fontSize_;
    int brushStyle_;
    char text_[128];
    COLORREF textColor_;
    COLORREF bkColor_;
    RECT paintRect_;
    HFONT hFont_;
    WNDPROC oldProc_;

protected:
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    virtual PCWSTR ClassName() const override { return TEXT("EDIT"); }
    virtual void ObservedFunc(NotifyType notify) override;
};