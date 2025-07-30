#ifndef OBROWSERMENU_H
#define OBROWSERMENU_H

#include <QMenu>
#include <QWidget>
#include <QToolBar>
#include <QLabel>

class OBrowserMenu : public QMenu
{
    Q_OBJECT

public:
    explicit OBrowserMenu(QWidget *parent = nullptr);
    void setupView();
    void setLightGrayTheme();
    void setLightTurquoiseTheme();
    void setLightBrownTheme();
    void setDarkGrayTheme();
    void setDarkTurquoiseTheme();
    void setDarkBrownTheme();
    void setPrivateTheme();
    void setupConnection();
    void setZoomValue(qreal zoom);
    void disablePageActions();
    void enablePageActions();
    QMenu *getHistoryMenu();

signals:
    void newTabClicked();
    void newWindowClicked();
    void newPrivateWindowClicked();
    void historyClicked();
    void bookmarkClicked();
    void downloadClicked();
    void settingClicked();
    void fullscreenClicked();
    void findTextClicked();
    void printClicked();
    void webDevClicked();
    void savePageClicked();
    void pageCodeClicked();
    void helpCenterClicked();
    void aboutClicked();
    void reportClicked();
    void quitClicked();
    void zoomOutClicked();
    void zoomInClicked();
    void resetZoomClicked();
    void bookmarksBarRequested();

public slots:
    void openRecentTab();
    void loadRecentTab(QAction *recentTab);
    void clearHistoryList();

protected:
    void showEvent(QShowEvent *event) override;

private:
    QAction *newTab;
    QAction *newWindow;
    QAction *newPrivateWindow;
    QMenu *historyMenu;
    QAction *history;
    QToolBar *historyBar;
    QLabel *recentTabsLabel;
    QAction *download;
    QMenu *bookmarkMenu;
    QAction *bookmark;
    QAction *markPage;
    QAction *bookmarkBar;
    QAction *fullscreen;
    QAction *print;
    QAction *find;
    QAction *webDeveloper;
    QAction *savePage;
    QAction *pageCode;
    QAction *settings;
    QAction *helpCenter;
    QAction *report;
    QAction *about;
    QAction *quit;
    QToolBar *zoomActionBar;
    QLabel *zoomLabel;
    QToolButton *zoomOut;
    QToolButton *zoomIn;
    QToolButton *resetZoom;
};

#endif // OBROWSERMENU_H
