#ifndef OBROWSERWIDGET_H
#define OBROWSERWIDGET_H

#include <QWidget>

class CWnd;

#if defined(Q_OS_WIN)
#  if !defined(QT_QTWINMIGRATE_EXPORT) && !defined(QT_QTWINMIGRATE_IMPORT)
#    define QT_QTWINMIGRATE_EXPORT
#  elif defined(QT_QTWINMIGRATE_IMPORT)
#    if defined(QT_QTWINMIGRATE_EXPORT)
#      undef QT_QTWINMIGRATE_EXPORT
#    endif
#    define QT_QTWINMIGRATE_EXPORT __declspec(dllimport)
#  elif defined(QT_QTWINMIGRATE_EXPORT)
#    undef QT_QTWINMIGRATE_EXPORT
#    define QT_QTWINMIGRATE_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTWINMIGRATE_EXPORT
#endif

class QT_QTWINMIGRATE_EXPORT OBrowserWidget : public QWidget
{
    Q_OBJECT
public:
    OBrowserWidget(HWND hParentWnd, QObject *parent = 0, Qt::WindowFlags flags = 0);
#ifdef QTWINMIGRATE_WITHMFC
    OBrowserWidget(CWnd *parnetWnd, QObject *parent = 0, Qt::WindowFlags f = 0);
#endif
    ~OBrowserWidget();

    void show();
    void center();
    void showCentered();
    HWND parentWindow() const;

protected:
    void childEvent(QChildEvent *event);
    bool eventFilter(QObject *object, QEvent *event);
    bool focusNextPrevChild(bool next);
    void focusInEvent(QFocusEvent *event);
#if QT_VERSION >= 0x050000
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#else
    bool winEvent(MSG *msg, long *result);
#endif

private:
    void init();
    void saveFocus();
    void resetFocus();
    HWND hParent;
    HWND prevFocus;
    bool reenable_parent;
};

#endif // OBROWSERWIDGET_H
