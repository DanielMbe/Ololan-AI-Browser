#ifndef CUSTOMTOOLBAR_H
#define CUSTOMTOOLBAR_H

#include "olibrary.h"
#include <QPushButton>
#include <QToolBar>
#include <QObject>
#include <QToolButton>
#include <QLabel>
#include <QWebEngineFindTextResult>

class OSmartInput;
class OFindInput;
class OBrowserMenu;
class ODownloadWidget;
class OSyncWidget;
class CustomToolBar : public QToolBar
{
    Q_OBJECT
    Q_PROPERTY(int height READ height WRITE setFixedHeight NOTIFY heightChanged)

public:
    explicit CustomToolBar(QWidget *parent);
    void startOnAnimation();
    void startOffAnimation();
    void setFriendBar(CustomToolBar *bar);
    void setWinType(WindowType type);
    void setupControls(QString webSearchEngine, bool ololanAssistant);
    void setLightGrayTheme(bool bookmarBarVisible);
    void setLightTurquoiseTheme(bool bookmarBarVisible);
    void setLightBrownTheme(bool bookmarBarVisible);
    void setDarkGrayTheme(bool bookmarBarVisible);
    void setDarkTurquoiseTheme(bool bookmarBarVisible);
    void setDarkBrownTheme(bool bookmarBarVisible);
    void setPrivateTheme(bool bookmarBarVisible);
    void setBookmarkBarTheme(QString theme);
    void setOlolanTheme(QString themeColor);
    void setSearchEngine(const QString searchEngine);
    void setEnableAssistant(bool value);
    void addBookmarkItems(QList<QAction*> list);
    void addBookmark(QAction *item);
    void setItemTheme(QPushButton *item);
    void setMenuTheme();
    void updateItem(const QString index, const QString newTitle, const QString newUrl);
    void deleteItem(const QString bmkIndex);
    void setControlBarState(bool state);
    void setupMenu();
    void setDownloadState(bool state);
    void warnForUpdates();
    CustomToolBar* getFriendBar();
    OBrowserMenu *getBrowserMenu();
    OSmartInput *getSmartBox();
    OFindInput *getFindInput();
    ODownloadWidget *getDownloadMenu();
    QToolButton *getReloadButton();
    QToolButton *getNextButton();
    QToolButton *getBackButton();
    QToolButton *getDownloadButton();
    QToolButton *getUpdatesButton();
    QToolButton *getAssistantButton();
    OSyncWidget *getSyncWidget();

signals:
    void heightChanged(int value);

public slots:
    void setFriendBarStyle();
    void updateFindInputVisibility();
    void updateFoundText(const QWebEngineFindTextResult &result);
    void openBookmark();
    void openInTab();
    void openInWindow();
    void openInPrivateWindow();
    void editBookmark();
    void deleteBookmark();

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private:
    WindowType winType;
    CustomToolBar *friendBar;
    OSmartInput *smartInput;
    OFindInput *findInput;
    OBrowserMenu *browserMenu;
    ODownloadWidget *downloadMenu;
    OSyncWidget *syncMenu;
    QToolButton *backButton;
    QToolButton *nextButton;
    QToolButton *reloadButton;
    QToolButton *menuButton;
    QToolButton *accountButton;
    QToolButton *downloadButton;
    QToolButton *updatesButton;
    QToolButton *assistantButton;
    QLabel *foundText;
    QAction *foundTextAction;
    QAction *inSeparator;
    QAction *fTextSeparator;
    QAction *updatesAction;
    QString ololanTheme;
    QPushButton *selectedBookmark;
    QMenu *itemMenu;
    QAction *inNewTab;
    QAction *inWindow;
    QAction *inPrivate;
    QAction *edit;
    QAction *remove;
    bool isBookmarkBar;
    bool isDownloading;
};

#endif // CUSTOMTOOLBAR_H
