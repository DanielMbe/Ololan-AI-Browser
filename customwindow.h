#ifndef CUSTOMWINDOW_H
#define CUSTOMWINDOW_H

#include "olibrary.h"
#include <windows.h>
#include <QApplication>

class OTabPage;

class CustomWindow
{
public:
    CustomWindow(QApplication *application, WindowType type, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);
    ~CustomWindow();
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CustomCaptionProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool *pfCallDWP);
    static LRESULT AppWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT HitTestNCA(HWND hWnd, LPARAM lParam);
    static void PaintCustomCaption(HWND hWnd, HDC hdc);
    void showWindow(int nCmdShow);
    void hideWindow();
    void closeWindow();
    void resizeWindow();
    void setCanClose(bool value);
    void setWidth(int winWidth);
    void setHeight(int winHeight);
    void setIsMaximized(bool state);
    void setDPIchanged(bool changed);
    bool getIsMaximized();
    bool getCanClose();
    int getWidth();
    int getHeight();
    bool getDPIchanged();
    QApplication *getParentApplication();
    QWidget *getCentralWidget();
    HWND getCustomWinHandle();
    WindowType getWindowType();

private:
    HWND hWnd;
    HINSTANCE hInstance;
    WindowType winType;
    bool canClose;
    bool isMaximized;
    bool DPIchanged;
    QWidget *centralWidget;
    OTabPage *tabPage;
    int width;
    int height;
};

#endif // CUSTOMWINDOW_H
