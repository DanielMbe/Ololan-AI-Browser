#ifdef QT3_SUPPORT
#undef QT3_SUPPORT
#endif

#ifdef UNICODE
#undef UNICODE
#endif

#ifdef QTWINMIGRATE_WITHMFC
#include <afxwin.h>
#endif

#include <qevent.h>
#include <qt_windows.h>
#include <QApplication>

#if QT_VERSION >= 0x050000
#include <QWindow>
#include <qpa/qplatformnativeinterface.h>
#define QT_WA(unicode, ansi) unicode
#endif
#include "obrowserwidget.h"

OBrowserWidget::OBrowserWidget(HWND hParentWnd, QObject *parent, Qt::WindowFlags flags)
    :QWidget(0, flags), hParent(hParentWnd), prevFocus(0), reenable_parent(false)
{
    if (parent)
        QObject::setParent(parent);
    init();
}

#ifdef QTWINMIGRATE_WITHMFC

OBrowserWidget::OBrowserWidget(CWnd *parentWnd, QObject *parent, Qt::WindowFlags flags)
: QWidget(0, flags), hParent(parentWnd ? parentWnd->m_hWnd : 0), prevFocus(0), reenable_parent(false)
{
    if (parent)
        QObject::setParent(parent);
    init();
}
#endif

void OBrowserWidget::init()
{
    Q_ASSERT(hParent);

    if (hParent)
    {
    // make the widget window style be WS_CHILD so SetParent will work
    QT_WA({SetWindowLong((HWND)winId(), GWL_STYLE, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);},
        {SetWindowLongA((HWND)winId(), GWL_STYLE, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);})
#if QT_VERSION >= 0x050000
        QWindow *window = windowHandle();
        window->setProperty("_q_embedded_native_parent_handle", (WId)hParent);
        HWND h = static_cast<HWND>(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("handle", window));
        SetParent(h, hParent);
        window->setFlags(Qt::FramelessWindowHint);
#else
        SetParent(winId(), hParent);
#endif
        QEvent event(QEvent::EmbeddingControl);
        QApplication::sendEvent(this, &event);
    }
}

OBrowserWidget::~OBrowserWidget()
{
}

/*!
    Returns the handle of the native Win32 parent window.
*/
HWND OBrowserWidget::parentWindow() const
{
    return hParent;
}

/*!
    \reimp
*/
void OBrowserWidget::childEvent(QChildEvent *event)
{
    QObject *object = event->child();
    if (object->isWidgetType())
    {
        if (event->added() && object->isWidgetType())
            object->installEventFilter(this);
        else if (event->removed() && reenable_parent)
        {
            reenable_parent = false;
            EnableWindow(hParent, true);
            object->removeEventFilter(this);
        }
    }
    QWidget::childEvent(event);
}

/*! \internal */
void OBrowserWidget::saveFocus()
{
    if (!prevFocus)
    prevFocus = GetFocus();

    if (!prevFocus)
    prevFocus = parentWindow();
}

/*!
    Shows this widget. Overrides QWidget::show().

    \sa showCentered()
*/
void OBrowserWidget::show()
{
    saveFocus();
    QWidget::show();
}

/*!
    Centers this widget over the native parent window. Use this
    function to have Qt toplevel windows (i.e. dialogs) positioned
    correctly over their native parent windows.

    \code
    QWinWidget qwin(hParent);
    qwin.center();

    QMessageBox::information(&qwin, "Caption", "Information Text");
    \endcode

    This will center the message box over the client area of hParent.
*/
void OBrowserWidget::center()
{
    const QWidget *child = findChild<QWidget*>();
    if (child && !child->isWindow())
    {
        qWarning("QWinWidget::center: Call this function only for QWinWidgets with toplevel children");
    }
    RECT rect;
    GetWindowRect(hParent, &rect);
    setGeometry((rect.right-rect.left)/2+rect.left, (rect.bottom-rect.top)/2+rect.top,0,0);
}

/*!
    \obsolete

    Call center() instead.
*/
void OBrowserWidget::showCentered()
{
    center();
    show();
}

/*!
    Sets the focus to the window that had the focus before this widget
    was shown, or if there was no previous window, sets the focus to
    the parent window.
*/
void OBrowserWidget::resetFocus()
{
    if (prevFocus)
        SetFocus(prevFocus);
    else
        SetFocus(parentWindow());
}

/*! \reimp
*/
#if QT_VERSION >= 0x050000
bool OBrowserWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)
#else
bool QWinWidget::winEvent(MSG *msg, long *result)
#endif
{
#if QT_VERSION >= 0x050000
    MSG *msg = (MSG *)message;
#endif
    if (msg->message == WM_SETFOCUS)
    {
        Qt::FocusReason reason;
        if ((GetKeyState(VK_LBUTTON) < 0) || (GetKeyState(VK_RBUTTON) < 0))
            reason = Qt::MouseFocusReason;
        else if (GetKeyState(VK_SHIFT) < 0)
            reason = Qt::BacktabFocusReason;
        else
            reason = Qt::TabFocusReason;
        QFocusEvent event(QEvent::FocusIn, reason);
        QApplication::sendEvent(this, &event);
    }

    return false;
}

/*!
    \reimp
*/
bool OBrowserWidget::eventFilter(QObject *object, QEvent *event)
{
    QWidget *widget = (QWidget*)object;
    switch (event->type())
    {
        case QEvent::WindowDeactivate:
            if (widget->isModal() && widget->isHidden())
            BringWindowToTop(hParent);
            break;

        case QEvent::Hide:
            if (reenable_parent)
            {
                EnableWindow(hParent, true);
                reenable_parent = false;
            }
            resetFocus();
            if (widget->testAttribute(Qt::WA_DeleteOnClose) && widget->isWindow())
                deleteLater();
            break;

        case QEvent::Show:
            if (widget->isWindow())
            {
                saveFocus();
                hide();
                if (widget->isModal() && !reenable_parent)
                {
                    EnableWindow(hParent, false);
                    reenable_parent = true;
                }
            }
            break;

        case QEvent::Close:
            SetActiveWindow(hParent);
            if (widget->testAttribute(Qt::WA_DeleteOnClose))
                deleteLater();
            break;

        default:
        break;
    }
    return QWidget::eventFilter(object, event);
}

/*! \reimp
*/
void OBrowserWidget::focusInEvent(QFocusEvent *event)
{
    QWidget *candidate = this;
    switch (event->reason())
    {
        case Qt::TabFocusReason:
        case Qt::BacktabFocusReason:
            while (!(candidate->focusPolicy() & Qt::TabFocus))
            {
                candidate = candidate->nextInFocusChain();
                if (candidate == this)
                {
                    candidate = 0;
                    break;
                }
            }
            if (candidate)
            {
                candidate->setFocus(event->reason());
                if (event->reason() == Qt::BacktabFocusReason || event->reason() == Qt::TabFocusReason)
                {
                    candidate->setAttribute(Qt::WA_KeyboardFocusChange);
                    candidate->window()->setAttribute(Qt::WA_KeyboardFocusChange);
                }
                if (event->reason() == Qt::BacktabFocusReason)
                    QWidget::focusNextPrevChild(false);
            }
            break;
        default:
            break;
    }
}

/*! \reimp
*/
bool OBrowserWidget::focusNextPrevChild(bool next)
{
    QWidget *curFocus = focusWidget();
    if (!next)
    {
        if (!curFocus->isWindow())
        {
            QWidget *nextFocus = curFocus->nextInFocusChain();
            QWidget *prevFocus = 0;
            QWidget *topLevel = 0;
            while (nextFocus != curFocus)
            {
                if (nextFocus->focusPolicy() & Qt::TabFocus)
                {
                    prevFocus = nextFocus;
                    topLevel = 0;
                }
                else if (nextFocus->isWindow())
                    topLevel = nextFocus;
                nextFocus = nextFocus->nextInFocusChain();
            }

            if (!topLevel)
                return QWidget::focusNextPrevChild(false);
        }
    }
    else
    {
        QWidget *nextFocus = curFocus;
        while (1)
        {
            nextFocus = nextFocus->nextInFocusChain();
            if (nextFocus->isWindow())
                break;
            if (nextFocus->focusPolicy() & Qt::TabFocus)
                return QWidget::focusNextPrevChild(true);
        }
    }

    SetFocus(hParent);
    return true;
}
