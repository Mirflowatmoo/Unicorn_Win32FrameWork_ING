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
            //CreateWindow����WM_NCCREATE��Ϣ
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            pThis = reinterpret_cast<DERIVED_TYPE*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

            pThis->hWnd_ = hWnd;
        }
        else {
            //��ȡ���ڵ�ʵ������
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
        /*SW_FORCEMINIMIZE����WindowNT5.0����С�����ڣ���ʹӵ�д��ڵ��̱߳�����Ҳ����С�����ڴ������߳���С������ʱ��ʹ�����������
    ����SW_HIDE�����ش��ڲ������������ڡ�
    ����SW_MAXIMIZE�����ָ���Ĵ��ڡ�
    ����SW_MINIMIZE����С��ָ���Ĵ��ڲ��Ҽ�����Z���е���һ�����㴰�ڡ�
    ����SW_RESTORE�������ʾ���ڡ����������С������󻯣���ϵͳ�����ڻָ���ԭ���ĳߴ��λ�á��ڻָ���С������ʱ��Ӧ�ó���Ӧ��ָ�������־��
    ����SW_SHOW���ڴ���ԭ����λ����ԭ���ĳߴ缤�����ʾ���ڡ�
    ����SW_SHOWDEFAULT��������STARTUPINFO�ṹ��ָ����SW_FLAG��־�趨��ʾ״̬��STARTUPINFO �ṹ��������Ӧ�ó���ĳ��򴫵ݸ�CreateProcess�����ġ�
    ����SW_SHOWMAXIMIZED������ڲ�������󻯡�
    ����SW_SHOWMINIMIZED������ڲ�������С����
    ����SW_SHOWMINNOACTIVE��������С�����������Ȼά�ּ���״̬��*/
        return ShowWindow(hWnd_, nCmdShow);
    }

    //Ϊ���ڰ󶨳���(����)
    void BindScene(void (*pDrawFunc)()) {

        pDrawFunc_ = pDrawFunc;
        observer_.Notify(NotifyType::INVISIBLE); //֪ͨ���Ӷ���һ��false��Ϣ����ʾ�����л�

        //ʹ�ð�ɫ��ˢ����ڴ�DC
        HBRUSH whiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH); 
        FillRect(hMemDC_, &clientRect_, whiteBrush);
        InvalidateRect(hWnd_, NULL, true);
    }

    HDC GetMemDC() const { return hMemDC_; }
    
    HWND GetHWnd() const { return hWnd_; }

    //�������ָ��ͼ��
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

    //���ر�����
    bool HideWindowTitleBar() {
        long lStyle = 0;
        lStyle = GetWindowLong(hWnd_, GWL_STYLE); //��ȡ���ڷ��

        if (0 == lStyle) {
            return false;
        }

        SetWindowLong(hWnd_, GWL_STYLE, lStyle & ~WS_CAPTION); //�����µĴ��ڷ��
        GetWindowRect(hWnd_, &clientRect_);
        
        BOOL rs = FALSE;
        rs = SetWindowPos(hWnd_, NULL, clientRect_.left, clientRect_.top, clientRect_.right - clientRect_.left, clientRect_.bottom - clientRect_.top,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

        if (FALSE == rs) {
            return false;
        }

        return true;
    }

    //��ʾ������
    bool ShowWindowTitleBar() {
        long lStyle = 0;

        lStyle = GetWindowLong(hWnd_, GWL_STYLE); //��ȡ���ڷ��

        if (0 == lStyle) {
            return false;
        }

        SetWindowLong(hWnd_, GWL_STYLE, lStyle | WS_CAPTION); //�����µĴ��ڷ��
        GetClientRect(hWnd_, &clientRect_);

        BOOL rs = FALSE;
        rs = SetWindowPos(hWnd_, NULL, 0, 0, 0, 0,
            SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);

        if (FALSE == rs) {
            return false;
        }

        return true;
    }

    //��ȡ���̴���������
    static UINT GetWindowsNum() { return windowsNum; }

    //��ȡ�۲�������
    UObserver<NotifyType>& GetObserver() { return observer_; }

protected:

	HWND hWnd_;
    //HINSTANCE hInst_;                                // ��ǰʵ��
    //WCHAR szTitle_[MAX_LOADSTRING];                  // �������ı�
    //WCHAR szWindowClass_[MAX_LOADSTRING];            // ����������
    HBITMAP hMemBmp_;
    HDC hMemDC_;
    RECT clientRect_; 
    UObserver<NotifyType> observer_; //��ʱʹ��bool��Ϣ
    void (*pDrawFunc_)();
    static UINT windowsNum; //��¼uwindow������
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

    //ˢ�´���״̬
    void Refresh();
    //��ӡ����
    void PrintText(int posx, int posy, char text[]);
    //�����ı���ɫ
    void SetTextColor(COLORREF color);
    //��������ߴ�
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

    //���ñ���ͼƬ
    void SetBkImage(const wchar_t* imgName);
    //�󶨰�ť�¼�
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

    //�󶨰�ť�¼�
    void BindScene(void (*pDrawFunc)()) = delete;
    //��ȡ�༭����������
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