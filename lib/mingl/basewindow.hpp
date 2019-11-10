#ifndef MINGL_BASE_WINDOW_INCLUDED
#define MINGL_BASE_WINDOW_INCLUDED

#include <string>
#include <iomanip>

#include <windows.h>

/* If no graphics needed, define NO_GDIPLUS to disable gdiplus library. */
#ifndef NO_GDIPLUS

#include <gdiplus/gdiplus.h>

#endif

#include <mingl/message.hpp>
#include <mingl/guiobject.hpp>

/**
 * Base window template to be derived. Creates window by following the process:
 * <ol>
 * <li>declare</li>
 * <li>initialize()</li>
 * <li>create()</li>
 * <li>show()</li>
 * </ol>
 *
 * Define USE_WNDCLASSEX macro to allow BaseWindow class to store WNDCLASSEX instead of WNDCLASS.
 * WNDCLASSEX (extended window class) has extra variable hIconSm and cbSize which needs to be 'size(WNDCLASSEX)'.
 * To create WNDCLASSEX use method CreateWindowEx instead of CreateWindow,
 * which needs optional window styles argument passed.
 */
template<class DerivedWindow>
class BaseWindow : public GUIObject {
private:
    bool initialized{false};

#ifdef USE_WNDCLASSEX
    WNDCLASSEX
#else
    WNDCLASS
#endif
            wc{};

#ifndef NO_GDIPLUS
    Gdiplus::GdiplusStartupInput gdiplusStartupInput{};
    ULONG_PTR gdiplusToken{};
#endif

    /**
     * Window procedure. Windows OS callback function. Windows OS sends messages to be handled by
     * DerivedWindow::handleMessage(Message).
     *
     * @param hwnd
     * @param msg
     * @param wParam
     * @param lParam
     * @returns result of handled message.
     */
    static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        DerivedWindow *pThis = nullptr;

        if (msg == WM_NCCREATE) {
            auto pCreate = (CREATESTRUCT *) lParam;
            pThis = (DerivedWindow *) pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) pThis);

            pThis->hwnd = hwnd;
        } else pThis = (DerivedWindow *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

        if (msg == WM_DESTROY) {
            PostQuitMessage(0);
            return 0;
        }

        if (pThis) return pThis->handleMessage(Message(msg, wParam, lParam));
        else return DefWindowProc(hwnd, msg, wParam, lParam);
    }

protected:
    /** This method needs to be derived by subclass window. */
    virtual LRESULT handleMessage(Message) = 0;

public:
    BaseWindow() : BaseWindow("") {}

    BaseWindow(const char *title) : BaseWindow(title, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT) {}

    BaseWindow(int x, int y, int width, int height) : BaseWindow("", x, y, width, height) {}

    BaseWindow(const char *title, int x, int y, int width, int height)
            : GUIObject("CustomWindowClass", title, x, y, width, height, nullptr, this) {
        style |= WS_OVERLAPPEDWINDOW;
    }

    virtual void initialize() final {
        if (initialized) return;
#ifndef NO_GDIPLUS
        Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
#endif
        wc.lpszClassName = className;
        wc.hInstance = GetModuleHandle(nullptr);
        hinstance = wc.hInstance;
        wc.lpfnWndProc = DerivedWindow::wndProc;
        setDefaultWindowAttributes();
#ifdef USE_WNDCLASSEX
        wc.cbSize = sizeof(WNDCLASSEX);
        if(RegisterClassEx(&wc))
#else
        if (RegisterClass(&wc))
#endif
            initialized = true;
    }

    virtual void setDefaultWindowAttributes() {
        setIcon(nullptr, IDI_APPLICATION);
#ifdef USE_WNDCLASSEX
        setSmallIcon(nullptr, IDI_APPLICATION);
#endif
        setCursor(nullptr, IDI_APPLICATION);
        setBackground((HBRUSH) (COLOR_WINDOW + 1));
        setMenuName(nullptr);
        setStyle(0);
        setWindowExtraBytes(0);
        setClassExtraBytes(0);
    }

    /**
     * Shows the window and creates a loop to fetch messages. After window destruction, shuts down gdiplus if enabled.
     *
     * @returns false when not created otherwise true after the window was destroyed.
     */
    virtual bool show() final {
        if (!isCreated()) return false;
        MSG msg{};
        while (GetMessage(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
#ifndef NO_GDIPLUS
        Gdiplus::GdiplusShutdown(gdiplusToken);
#endif
        return true;
    }

    /* GETTERS */

    /** @returns window class of type either WNDCLASS or WNDCLASSEX if USE_WNDCLASSEX defined. */
    virtual
#ifdef USE_WNDCLASSEX
    WNDCLASSEX
#else
    WNDCLASS
#endif
    getWindowClass() const final {
        return wc;
    }

    virtual const char *getTitle() const final {
        return name;
    }

    /* SETTERS */

    virtual void setTitle(char const *title) final {
        name = title;
    }

    virtual void setIcon(HINSTANCE hinstance, LPCSTR icon_name) final {
        if (!initialized) wc.hIcon = LoadIcon(hinstance, icon_name);
    }

#ifdef USE_WNDCLASSEX
    virtual void setSmallIcon(HINSTANCE hinstance, LPCSTR icon_name) final {
        if (!initialized) wc.hIconSm = LoadIcon(hinstance, icon_name);
    }
#endif

    virtual void setCursor(HINSTANCE hinstance, LPCSTR cursor_name) final {
        if (!initialized) wc.hCursor = LoadCursor(hinstance, cursor_name);
    }

    virtual void setMenuName(LPCSTR menu_name) final {
        if (!initialized) wc.lpszMenuName = menu_name;
    }

    virtual void setBackground(HBRUSH color) final {
        if (!initialized) wc.hbrBackground = color;
    }

    virtual void setStyle(UINT style) final {
        if (!initialized) wc.style = style;
    }

    virtual void setWindowExtraBytes(int n_bytes) final {
        if (!initialized) wc.cbWndExtra = n_bytes;
    }

    virtual void setClassExtraBytes(int n_bytes) final {
        if (!initialized) wc.cbClsExtra = n_bytes;
    }
};

#endif
