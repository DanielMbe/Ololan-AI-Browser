#include "customwindow.h"
#include "oappcore.h"
#include "otabpage.h"
#include <dwmapi.h>
#include <windowsx.h>
#include <Windows.h>
#include <ShellScalingApi.h>
#include <QScreen>
#include <QMessageBox>

OAppCore *parentApp;
int windowIdNumber = 0;

CustomWindow::CustomWindow(QApplication *application, WindowType type, int x, int y)
{
    parentApp = static_cast<OAppCore*>(application);
    hInstance = HINSTANCE(application);
    std::string windowClassName = QString("WindowClassName%1").arg(++windowIdNumber).toStdString();
    canClose = true;
    isMaximized = true;
    DPIchanged = false;
    winType = type;

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra	= 0;
    wcex.cbWndExtra	= 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(hInstance, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = (wchar_t*)windowClassName.c_str();
    wcex.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        std::string error = QString("RegisterClassEx failed with error %1").arg(GetLastError()).toStdString();
        qDebug(error.c_str());
        parentApp->exit(1);
    }

    hWnd = CreateWindow(wcex.lpszClassName, L"", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, x, y, 600, 400, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        qDebug("Call to CreateWindow failed!");
        parentApp->exit(1);
    }

    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    LPCWSTR logoPath = L"C:\\Users\\DevTeam\\Desktop\\IT_Projects\\Ololan_IT_Projects\\Ololan_Desktop_Application_Projects\\OlolanPC\\images\\ololanBrowserLogo.ico";
    HICON logoIcon = (HICON)LoadImage(NULL, logoPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

    if (logoIcon)
    {
        SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)logoIcon);
        SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)logoIcon);
    }

    tabPage = new OTabPage(this, type, parentApp->parent(), hWnd);
    tabPage->show();
    centralWidget = static_cast<QWidget*>(tabPage);
}

CustomWindow::~CustomWindow()
{
    DestroyWindow(hWnd);
}

LRESULT CALLBACK CustomWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool fCallDWP = true;
    BOOL fDwmEnabled = FALSE;
    LRESULT lRet = 0;
    HRESULT hRet = S_OK;

    hRet = DwmIsCompositionEnabled(&fDwmEnabled);

    if (SUCCEEDED(hRet))
        lRet = CustomCaptionProc(hWnd, message, wParam, lParam, &fCallDWP);

    if (fCallDWP)
        lRet = AppWinProc(hWnd, message, wParam, lParam);

    return lRet;
}

LRESULT CustomWindow::CustomCaptionProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool *pfCallDWP)
{
    bool fCallDWP = true;
    LRESULT lRet = 0;
    HRESULT hRet = S_OK;

    fCallDWP = !DwmDefWindowProc(hWnd, message, wParam, lParam, &lRet);

    if (message == WM_CREATE)
    {
        RECT rcClient;
        GetWindowRect(hWnd, &rcClient);
        SetWindowPos(hWnd, HWND_TOP, rcClient.left, rcClient.top, (rcClient.right-rcClient.left), (rcClient.bottom-rcClient.top), SWP_FRAMECHANGED);
        fCallDWP = true;
        lRet = 0;
    }

    if (message == WM_ACTIVATE)
    {
        MARGINS margins;
        margins.cxLeftWidth = 0;
        margins.cxRightWidth = 0;
        margins.cyTopHeight = 0;
        margins.cyBottomHeight = 0;

        hRet = DwmExtendFrameIntoClientArea(hWnd, &margins);

        if (!SUCCEEDED(hRet))
        {
            qDebug("Extending frame into client area failed!");
            parentApp->exit(1);
        }

        fCallDWP = true;
        lRet = 0;
    }

    if ((message == WM_NCCALCSIZE) && (wParam == TRUE))
    {
        NCCALCSIZE_PARAMS *pncsp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
        pncsp->rgrc[0].left = pncsp->rgrc[0].left + 0;
        pncsp->rgrc[0].top = pncsp->rgrc[0].top + 0;
        pncsp->rgrc[0].right = pncsp->rgrc[0].right - 0;
        pncsp->rgrc[0].bottom = pncsp->rgrc[0].bottom - 0;

        lRet = 0;
        fCallDWP = false;
    }

    if (message == WM_PAINT)
    {
        HDC hdc;
        {
            PAINTSTRUCT ps;
            hdc = BeginPaint(hWnd, &ps);
            PaintCustomCaption(hWnd, hdc);
            EndPaint(hWnd, &ps);
        }

        fCallDWP = true;
        lRet = 0;
    }

    if ((message == WM_NCHITTEST) && (lRet == 0))
    {
        lRet = HitTestNCA(hWnd, lParam);

        if (lRet != HTNOWHERE)
            fCallDWP = false;
    }

    *pfCallDWP = fCallDWP;
    return lRet;
}

LRESULT CustomWindow::AppWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_CREATE:
            break;
        case WM_COMMAND:
        {
            if (LOWORD(wParam))
                return DefWindowProc(hWnd, message, wParam, lParam);
        }
            break;
        case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, &ps);
            PaintCustomCaption(hWnd, hdc);
            EndPaint(hWnd, &ps);
        }
            break;
        case WM_ACTIVATEAPP:
        {
            if (wParam == FALSE)
                parentApp->setWindowState(hWnd);

            return DefWindowProc(hWnd, message, wParam, lParam);
        }
            break;
        case WM_DESTROY:
            parentApp->closeWindowWidget(hWnd);
            break;
        case WM_SIZE:
        {
            HMONITOR hMonitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
            UINT dpiX, dpiY;
            GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

            double currentScalingX = (double)(dpiX / 96.0); // 96 DPI is 100% scaling
            double currentScalingY = (double)(dpiY / 96.0); // 96 DPI is 100% scaling

            RECT rcClient;
            GetClientRect(hWnd, &rcClient);

            rcClient.right = rcClient.right / currentScalingX;
            rcClient.bottom = rcClient.bottom / currentScalingY;

            WINDOWPLACEMENT wp;
            wp.length = sizeof(WINDOWPLACEMENT);
            GetWindowPlacement(hWnd, &wp);
            parentApp->updateWindowWidget(hWnd, rcClient, wp.showCmd);
        }
            break;
        case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO minMaxSize = (LPMINMAXINFO)lParam;
            minMaxSize->ptMinTrackSize.x = 687;
            minMaxSize->ptMinTrackSize.y = 449;

            RECT rcClient;
            GetClientRect(GetDesktopWindow(), &rcClient);

            UINT dpiX = GetDpiForWindow(hWnd);
            minMaxSize->ptMaxTrackSize.x = MulDiv(rcClient.right, dpiX, 96);
            minMaxSize->ptMaxTrackSize.y = MulDiv(rcClient.bottom, dpiX, 96);
        }
            break;
        case WM_MOVE:
        {
            if ((GetKeyState(VK_LBUTTON) & 0x8000) != 0)
                parentApp->moveTabToWindow(hWnd);
        }
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void CustomWindow::PaintCustomCaption(HWND hWnd, HDC hdc)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    HTHEME hTheme = OpenThemeData(NULL, L"CompositedWindow::Window");
    if (hTheme)
    {
        HDC hdcPaint = CreateCompatibleDC(hdc);

        if (hdcPaint)
        {
            int cx = (rcClient.right-rcClient.left);
            int cy = (rcClient.bottom-rcClient.top);

            // Define the BITMAPINFO structure used to draw text.
            // Note that biHeight is negative. This is done because
            // DrawThemeTextEx() needs the bitmap to be in top-to-bottom
            // order.
            BITMAPINFO dib = {};
            dib.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
            dib.bmiHeader.biWidth           = cx;
            dib.bmiHeader.biHeight          = -cy;
            dib.bmiHeader.biPlanes          = 1;
            dib.bmiHeader.biBitCount        = 1;
            dib.bmiHeader.biCompression     = BI_RGB;

            HBITMAP hbm = CreateDIBSection(hdc, &dib, DIB_RGB_COLORS, NULL, NULL, 0);
            if (hbm)
            {
                HBITMAP hbmOld = (HBITMAP)SelectObject(hdcPaint, hbm);

                // Setup the theme drawing options.
                DTTOPTS DttOpts;
                DttOpts.dwSize = {sizeof(DTTOPTS)};
                DttOpts.dwFlags = DTT_COMPOSITED | DTT_GLOWSIZE;
                DttOpts.iGlowSize = 15;

                // Select a font.
                LOGFONT lgFont;
                HFONT hFontOld = NULL;
                if (SUCCEEDED(GetThemeSysFont(hTheme, 1, &lgFont)))
                {
                    HFONT hFont = CreateFontIndirect(&lgFont);
                    hFontOld = (HFONT) SelectObject(hdcPaint, hFont);
                }

                // Draw the title.
                RECT rcPaint = rcClient;
                rcPaint.top += 2;
                rcPaint.right -= 2;
                rcPaint.left += 1;
                rcPaint.bottom -= 3;
                DrawThemeTextEx(hTheme, hdcPaint, 0, 0,  L"", -1, DT_LEFT | DT_WORD_ELLIPSIS, &rcPaint, &DttOpts);

                // Blit text to the frame.
                BitBlt(hdc, 0, 0, cx, cy, hdcPaint, 0, 0, SRCCOPY);

                SelectObject(hdcPaint, hbmOld);
                if (hFontOld)
                {
                    SelectObject(hdcPaint, hFontOld);
                }
                DeleteObject(hbm);
            }
            DeleteDC(hdcPaint);
        }
        CloseThemeData(hTheme);
    }
}

LRESULT CustomWindow::HitTestNCA(HWND hWnd, LPARAM lParam)
{
    // Get the point coordinates for the hit test.
    POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

    // Get the window rectangle.
    RECT rcWindow;
    GetWindowRect(hWnd, &rcWindow);

    // Get the frame rectangle, adjusted for the style without a caption.
    RECT rcFrame = {0, 0, 0, 0};
    AdjustWindowRectEx(&rcFrame, WS_OVERLAPPEDWINDOW & ~WS_CAPTION, FALSE, NULL);

    // Determine if the hit test is for resizing. Default middle (1,1).
    USHORT uRow = 1;
    USHORT uCol = 1;
    bool fOnResizeBorder = false;

    // Determine if the point is at the top or bottom of the window.
    if (ptMouse.y >= rcWindow.top && ptMouse.y < rcWindow.top + 1)
    {
        fOnResizeBorder = (ptMouse.y < (rcWindow.top - rcFrame.top));
        uRow = 0;
    }
    else if (ptMouse.y < rcWindow.bottom && ptMouse.y >= rcWindow.bottom - 1)
        uRow = 2;

    // Determine if the point is at the left or right of the window.
    if (ptMouse.x >= rcWindow.left && ptMouse.x < rcWindow.left + 1)
        uCol = 0; // left side

    else if (ptMouse.x < rcWindow.right && ptMouse.x >= rcWindow.right - 1)
        uCol = 2; // right side

    // Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
    LRESULT hitTests[3][3] =
    {
        {HTTOPLEFT,fOnResizeBorder ? HTTOP : HTCAPTION, HTTOPRIGHT},
        {HTLEFT, HTNOWHERE, HTRIGHT},
        {HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT},
    };

    return hitTests[uRow][uCol];
}

void CustomWindow::showWindow(int nCmdShow)
{
    ShowWindow(hWnd, nCmdShow);
}

void CustomWindow::hideWindow()
{
    ShowWindow(hWnd, SW_HIDE);
}

void CustomWindow::closeWindow()
{
    DestroyWindow(hWnd);
}

void CustomWindow::resizeWindow()
{
    WINDOWPLACEMENT wp;
    wp.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hWnd, &wp);

    if (wp.showCmd == SW_MAXIMIZE)
        showWindow(SW_SHOWNORMAL);
    else
        showWindow(SW_SHOWMAXIMIZED);

    UpdateWindow(hWnd);
}

QApplication *CustomWindow::getParentApplication()
{
    return static_cast<QApplication*>(parentApp);
}

QWidget *CustomWindow::getCentralWidget()
{
    return centralWidget;
}

HWND CustomWindow::getCustomWinHandle()
{
    return hWnd;
}

bool CustomWindow::getCanClose()
{
    return canClose;
}

void CustomWindow::setCanClose(bool value)
{
    canClose = value;
}

void CustomWindow::setWidth(int winWidth)
{
    width = winWidth;
}

void CustomWindow::setHeight(int winHeight)
{
    height = winHeight;
}

int CustomWindow::getWidth()
{
    return width;
}

int CustomWindow::getHeight()
{
    return height;
}

WindowType CustomWindow::getWindowType()
{
    return winType;
}

void CustomWindow::setIsMaximized(bool state)
{
    isMaximized = state;
}

bool CustomWindow::getIsMaximized()
{
    return isMaximized;
}
void CustomWindow::setDPIchanged(bool changed)
{
    DPIchanged = changed;
}

bool CustomWindow::getDPIchanged()
{
    return DPIchanged;
}
