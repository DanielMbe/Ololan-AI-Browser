#ifndef OTABPAGE_H
#define OTABPAGE_H

#include "olibrary.h"
#include <QStackedWidget>
#include <QToolButton>

class CustomWindow;
class OTabBar;
class QWebEngineDownloadRequest;
class OWebView;
class OTab;
class CustomToolBar;
class QWebEnginePage;
class OWebWidget;
class OTabPage : public QWidget
{
    Q_OBJECT

public:
    explicit OTabPage(CustomWindow *parentWind, WindowType type, QObject *parent, HWND hWnd);
    int count();
    int currentTabPage();
    bool getIsClosed();
    bool isBookmarkBarVisible();
    void resizeTabs(QResizeEvent *event);
    void tabDropEvent(QDropEvent *event);
    void initWidget(HWND hWnd);
    void initActions();
    void grabTabPage();
    void openHistory();
    void openBookmarks();
    void openDownloads();
    void openSettings();
    void openInTab(const QString url);
    void setupConnection(CustomToolBar *toolBar, OWebView *webView, OTab *tab);
    void initData(bool geolocation, bool passwordSaving, bool autoSignIn, bool paymentSaveAndFill, bool dNT, bool assistant,
                  bool askDwnPath, bool bmkBarVisibile, double pageZoom, QString theme, QString searchEngine,
                  QList<QAction *> *closedTabList, bool camera, bool microphone, QString username);
    void setWebPageZoom(int value);
    void enableGeolocation(bool value);
    void setEnableAssistant(bool value);
    void setSearchEngine(const QString searchEngine);
    void setBookmarkBarVisibile(bool state);
    void setDoNotTrack(bool value);
    void setAskDownloadPath(bool value);
    void setPaymentSaveAndFill(bool value);
    void setPasswordSaving(bool value);
    void setSiteAutoSignIn(bool value);
    void setOlolanTheme(QString themeColor);
    void setLightGrayTheme();
    void setLightTurquoiseTheme();
    void setLightBrownTheme();
    void setDarkGrayTheme();
    void setDarkTurquoiseTheme();
    void setDarkBrownTheme();
    void setPrivateTheme();
    void synchronizeOlolan();
    void disconnectOlolan();
    void deleteAccount();
    void setEnableMicrophone(bool state);
    void setEnableCamera(bool state);
    void updateBookmarkBar(QAction *bookmarkItem);
    void updateBookmarkBar(const QString index, const QString newTitle, const QString newUrl);
    void deleteBookmarkBarItem(const QString bmkIndex);
    void editBookmark(const QString index);
    bool openLastCloseTab();
    bool openFirstCloseTab();
    void findTextWith(const QString text);
    void visitPage(const QString page);
    void makeSearchFor(const QString search);
    void updateSyncProfile(const QString username, bool isLoggedIn);
    void setDownloadState(bool state);
    void bruteRemoving(int index);
    void warnForUpdates();
    QWidget *processRemoving(int &index);
    OWebView *createNewTab();
    OWebView *createBackgroundTab();
    WindowType getWindowType();
    QWidget *setupPageWidget();
    QStackedWidget *getStackPage();
    OTabBar *getTabBar();
    CustomWindow *getParentWindow();

public slots:
    void setCurrentTabPage(const int index);
    void closeTabPage(int index);
    void openNewTab();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void reloadCurrentPage();
    void cut();
    void copy();
    void paste();
    void showFullscreenMode();
    void unSetFullscreen();
    void print();
    void webDev();
    void reportIssue();
    void findText();
    void quitBrowser();
    void savePage();
    void about();
    void helpCenter();
    void showBookmarkBar();
    void moveTab(const int from, const int to);
    void resizeWindow();
    void minimizeWindow();
    void switchTab();
    void removeCurrentTab();
    void pageReady(bool status);
    void manageBrowserApp(bool status);
    void getDownload(QWebEngineDownloadRequest *download);
    void setupClosedTabsList();
    void openRecentTab();
    void handleMousePress(QMouseEvent *event);
    void alignDevToolLeft();
    void alignDevToolRight();
    void alignDevToolBottom();
    void connectAI();
    void searchTheWebFor(const QString item);
    void openEina();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    bool event(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    CustomWindow *parentWindowPtr;
    OWebWidget *eina;
    WindowType winType;
    OTabBar *tabBar;
    QStackedWidget *stackPage;
    QPoint clickPosition;
    qreal generalZoomFactor;
    QList<qreal> zoomValues;
    QString webSearchEngine;
    QString ololanTheme;
    QString accountName;
    QList<QAction*> *closedTabs;
    bool bookmarBarVisible;
    bool geolocate;
    bool doNotTrack;
    bool ololanAssistant;
    bool askDownloadPath;
    bool saveAndFillPayment;
    bool savePassword;
    bool websiteAutoSignIn;
    bool isClosed;
    bool onTabAdding;
    bool enableMicrophone;
    bool enableCamera;
    bool isDownloading;
    bool updatesAvailables;
    int winDistanceWidth;
    int winDistanceHeight;
};

#endif // OTABPAGE_H
