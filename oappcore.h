#ifndef OAPPCORE_H
#define OAPPCORE_H

#include <QApplication>
#include <Windows.h>
#include "olibrary.h"

class CustomWindow;
class OHistoryCore;
class OBookmarksCore;
class ODownloadsCore;
class OSettingsCore;
class OAssistantCore;
class OStartPageCore;
class OTabPage;
class OWebView;
class OAppCore : public QApplication
{
    Q_OBJECT

public:
    explicit OAppCore(int argc, char *argv[]);
    ~OAppCore();
    void setHistoryOpened(bool isOpened);
    void setBookmarksOpened(bool isOpened);
    void setDownloadsOpened(bool isOpened);
    void setSettingsOpened(bool isOpened);
    void windowClosed();
    void updateWindowWidget(HWND hWnd, RECT &rcClient, int windowCmd);
    void closeWindowWidget(HWND hWnd);
    void moveTabToWindow(HWND hWnd);
    void openNormalSizeWindow(int openMode, WindowType type, const QPoint mousePos);
    void raiseWindow(CustomWindow *window);
    void setWindowState(HWND hWnd);
    void historyClosed();
    void bookmarksClosed();
    void downloadsClosed();
    void settingsClosed();
    void setOlolanTheme(QString themeColor);
    void setBookmarkBarVisibile(bool state);
    void setWebPageZoom(int value);
    void enableGeolocation(bool value);
    void setPasswordSaving(bool value);
    void setSiteAutoSignIn(bool value);
    void setPaymentSaveAndFill(bool value);
    void setDoNotTrack(bool value);
    void setEnableAssistant(bool value);
    void setAskDownloadPath(bool value);
    void setSyncActivated(bool value, const QString username, const QString password, const QString login);
    void updateProfile(bool value, const QString username);
    void setSearchEngine(const QString searchEngine);
    void setEnableMicrophone(bool state);
    void setEnableCamera(bool state);
    void openInWindow(const QString url);
    void openInPrivateWindow(const QString url);
    void synchronizeOlolan();
    void disconnectOlolan();
    void deleteAccount();
    void updateBookmarkBar(QAction *bookmarkItem);
    void updateBookmarkBar(const QString index, const QString newTitle, const QString newUrl);
    void deleteBookmarkBarItem(const QString bmkIndex);
    void processWebQuery(const QString query);
    void setDownloadState(bool state);
    void setWindowDPIChanged(HWND hWnd, bool changed);
    void warnForUpdates();
    bool isHistoryOpened();
    bool isBookmarksOpened();
    bool isDownloadsOpened();
    bool isSettingsOpened();
    bool isLogged();
    bool getWindowDPIChanged(HWND hWnd);
    const QString getAccountName();
    const QString getAccountEmail();
    const QString getAccountPassword();
    const int getCurrentWindowIndex();
    QList<CustomWindow*> allWindowsList();
    OHistoryCore *getHistoryManager();
    OBookmarksCore *getBookmarksManager();
    ODownloadsCore *getDownloadsManager();
    OSettingsCore *getSettingsManager();
    OAssistantCore *getAssistantManager();
    OStartPageCore *getStartPageManager();
    OTabPage *getActiveTabPage();
    OWebView *createNewWindow();

public slots:
    void exitFromBrowser();
    void openBrowserWindow();
    void openPrivateBrowserWindow();
    void openHistory();
    void openBookmarks();
    void openDownloads();
    void openSettings();

private:
    bool bookmarkOpened;
    bool downloadOpened;
    bool historyOpened;
    bool settingsOpened;
    bool forceExit;
    bool bookmarkBarVisibile;
    bool geolocation;
    bool passwordSaving;
    bool siteAutoSignIn;
    bool paymentSaveAndFill;
    bool doNotTrack;
    bool enableAssistant;
    bool askDownloadPath;
    bool enableMicrophone;
    bool enableCamera;
    bool loggedIn;
    bool isDownloading;
    double pageZoom;
    int historyWinIndex;
    int downloadsWinIndex;
    int bookmarksWinIndex;
    int settingsWinIndex;
    QString ololanSearchEngine;
    QString ololanTheme;
    QString webProtection;
    QString onStartup;
    QString accountName;
    QString accountEmail;
    QString accountPassword;
    QList<CustomWindow*> windowsList;
    QList<bool> windowStateList;
    QList<int> windowZOrder;
    QList<QAction*> closedTabList;
    QStringList appArguments;
    OHistoryCore *historyManager;
    OBookmarksCore *bookmarksManager;
    ODownloadsCore *downloadsManager;
    OSettingsCore *settingsManager;
    OAssistantCore *assistantManager;
    OStartPageCore *startPageManager;
};

#endif // OAPPCORE_H
