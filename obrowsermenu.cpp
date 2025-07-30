#include "obrowsermenu.h"
#include <QWidget>
#include <QWidgetAction>
#include <QAction>
#include <QToolBar>
#include <QToolButton>
#include <QLabel>
#include <QPropertyAnimation>
#include <QShowEvent>

OBrowserMenu::OBrowserMenu(QWidget *parent)
    :QMenu(parent)
{
    setWindowFlag(Qt::Widget);
    setFixedWidth(360);
}

void OBrowserMenu::setupView()
{
    //menu action item
    newTab = new QAction(tr("New Tab"), this);
    newTab->setShortcut(QKeySequence(QLatin1String("Ctrl+T")));
    newWindow = new QAction(tr("New Window"), this);
    newWindow->setShortcut(QKeySequence(QLatin1String("Ctrl+N")));
    newPrivateWindow = new QAction(tr("New Private Window"), this);
    newPrivateWindow->setShortcut(QKeySequence(QLatin1String("Ctrl+Shift+N")));

    historyMenu = new QMenu(tr("History"), this);
    historyMenu->setFixedWidth(290);
    history = new QAction(tr("History"), this);
    history->setShortcut(QKeySequence(QLatin1String("Ctrl+H")));
    historyBar = new QToolBar(this);
    historyBar->setMovable(false);
    historyBar->setFloatable(false);
    historyBar->setFixedHeight(34);
    recentTabsLabel = new QLabel("Closed tabs", historyBar);
    recentTabsLabel->setFixedWidth(100);   
    historyBar->addWidget(recentTabsLabel);
    QWidgetAction *historyBarAction = new QWidgetAction(this);
    historyBarAction->setDefaultWidget(historyBar);
    historyMenu->addAction(history);
    historyMenu->addSeparator();
    historyMenu->addAction(historyBarAction);
    historyMenu->addSeparator();

    download = new QAction(tr("Downloads"), this);
    download->setShortcut(QKeySequence(QLatin1String("Ctrl+J")));
    bookmarkMenu = new QMenu(tr("Bookmarks"), this);
    bookmarkMenu->setFixedWidth(290);
    bookmark = new QAction(tr("Bookmarks"), bookmarkMenu);
    bookmark->setShortcut(QKeySequence(QLatin1String("Ctrl+D")));
    markPage = new QAction(tr("Mark this page"), bookmarkMenu);
    markPage->setShortcut(QKeySequence(QLatin1String("Ctrl+B")));
    bookmarkBar = new QAction(tr("Show/Hide Bookmarks bar"), bookmarkMenu);
    bookmarkMenu->addAction(bookmark);
    bookmarkMenu->addSeparator();
    bookmarkMenu->addAction(markPage);
    bookmarkMenu->addAction(bookmarkBar);

    fullscreen = new QAction(tr("Fullscreen"), this);
    fullscreen->setShortcut(QKeySequence(QLatin1String("F11")));
    print = new QAction(tr("Print"), this);
    print->setShortcut(QKeySequence(QLatin1String("Ctrl+P")));
    find = new QAction(tr("Find..."), this);
    find->setShortcut(QKeySequence(QLatin1String("Ctrl+F")));
    webDeveloper = new QAction(tr("Web developer tools"), this);
    webDeveloper->setShortcut(QKeySequence(QLatin1String("Ctrl+Shift+I")));
    savePage = new QAction(tr("Save page as..."), this);
    savePage->setShortcut(QKeySequence(QLatin1String("Ctrl+S")));
    pageCode = new QAction(tr("Page source code"), this);
    pageCode->setShortcut(QKeySequence(QLatin1String("Ctrl+U")));
    settings = new QAction(tr("Settings"), this);
    settings->setShortcut(QKeySequence(QLatin1String("Alt+S")));
    helpCenter = new QAction(tr("Help center"), this);
    helpCenter->setShortcut(QKeySequence(QLatin1String("F1")));
    report = new QAction(tr("Report an issue"), this);
    report->setShortcut(QKeySequence(QLatin1String("Alt+R")));
    about = new QAction(tr("About Ololan browser"), this);
    quit = new QAction(tr("Quit"), this);
    quit->setShortcut(QKeySequence(QLatin1String("Ctrl+Q")));

    //zoom action item

    zoomActionBar = new QToolBar(this);
    zoomActionBar->setMovable(false);
    zoomActionBar->setFloatable(false);
    zoomActionBar->setFixedHeight(36);

    zoomLabel = new QLabel("Zoom", zoomActionBar);
    zoomLabel->setFixedWidth(86);
    zoomLabel->setAlignment(Qt::AlignCenter);
    zoomOut = new QToolButton(zoomActionBar);
    zoomOut->setFixedSize(48, 34);
    zoomIn = new QToolButton(zoomActionBar);
    zoomIn->setFixedSize(48, 34);
    resetZoom = new QToolButton(zoomActionBar);
    resetZoom->setFixedSize(69, 34);
    resetZoom->setText(QLatin1String("100%"));

    zoomActionBar->addWidget(zoomLabel);
    zoomActionBar->addSeparator();
    zoomActionBar->addWidget(zoomOut);
    zoomActionBar->addWidget(resetZoom);
    zoomActionBar->addWidget(zoomIn);

    QWidgetAction *zoomBarWidget = new QWidgetAction(this);
    zoomBarWidget->setDefaultWidget(zoomActionBar);

    addAction(newTab);
    addAction(newWindow);
    addAction(newPrivateWindow);
    addMenu(bookmarkMenu);
    addMenu(historyMenu);
    addAction(download);
    addSeparator();
    addAction(fullscreen);
    addAction(print);
    addAction(find);
    addAction(savePage);
    addAction(pageCode);
    addAction(webDeveloper);
    addSeparator();
    addAction(zoomBarWidget);
    addSeparator();
    addAction(settings);
    addAction(helpCenter);
    addAction(report);
    addAction(about);
    addAction(quit);
}

void OBrowserMenu::disablePageActions()
{
    if (pageCode->isEnabled())
        pageCode->setDisabled(true);
    if (webDeveloper->isEnabled())
        webDeveloper->setDisabled(true);
}

void OBrowserMenu::enablePageActions()
{
    if (!pageCode->isEnabled())
        pageCode->setEnabled(true);
    if (!webDeveloper->isEnabled())
        webDeveloper->setEnabled(true);
}

void OBrowserMenu::setupConnection()
{
    connect(newTab, &QAction::triggered, this, &OBrowserMenu::newTabClicked);
    connect(newWindow, &QAction::triggered, this, &OBrowserMenu::newWindowClicked);
    connect(newPrivateWindow, &QAction::triggered, this, &OBrowserMenu::newPrivateWindowClicked);
    connect(zoomIn, &QToolButton::clicked, this, &OBrowserMenu::zoomInClicked);
    connect(zoomOut, &QToolButton::clicked, this, &OBrowserMenu::zoomOutClicked);
    connect(resetZoom, &QToolButton::clicked, this, &OBrowserMenu::resetZoomClicked);
    connect(history, &QAction::triggered, this, &OBrowserMenu::historyClicked);
    connect(bookmark, &QAction::triggered, this, &OBrowserMenu::bookmarkClicked);
    connect(download, &QAction::triggered, this, &OBrowserMenu::downloadClicked);
    connect(fullscreen, &QAction::triggered, this, &OBrowserMenu::fullscreenClicked);
    connect(print, &QAction::triggered, this, &OBrowserMenu::printClicked);
    connect(find, &QAction::triggered, this, &OBrowserMenu::findTextClicked);
    connect(savePage, &QAction::triggered, this, &OBrowserMenu::savePageClicked);
    connect(pageCode, &QAction::triggered, this, &OBrowserMenu::pageCodeClicked);
    connect(webDeveloper, &QAction::triggered, this, &OBrowserMenu::webDevClicked);
    connect(settings, &QAction::triggered, this, &OBrowserMenu::settingClicked);
    connect(helpCenter, &QAction::triggered, this, &OBrowserMenu::helpCenterClicked);
    connect(report, &QAction::triggered, this, &OBrowserMenu::reportClicked);
    connect(about, &QAction::triggered, this, &OBrowserMenu::aboutClicked);
    connect(quit, &QAction::triggered, this, &OBrowserMenu::quitClicked);
    connect(bookmarkBar, &QAction::triggered, this, &OBrowserMenu::bookmarksBarRequested);
}

void OBrowserMenu::showEvent(QShowEvent *event)
{
    QMenu::showEvent(event);
    QRect startGeometry = geometry();
    startGeometry.setCoords(geometry().x(), geometry().y()-10, geometry().x()+width(), geometry().y()+height()-10);
    QRect endGeometry = geometry();
    endGeometry.setCoords(geometry().x(), geometry().y(), geometry().x()+width(), geometry().y()+height());

    setGeometry(startGeometry);
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry", this);
    animation->setDuration(100);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);
    animation->start();
}

void OBrowserMenu::openRecentTab()
{
    QAction *recentTab = static_cast<QAction*>(sender());
    historyMenu->removeAction(recentTab);
}

QMenu *OBrowserMenu::getHistoryMenu()
{
    return historyMenu;
}

void OBrowserMenu::loadRecentTab(QAction *recentTab)
{
    historyMenu->addAction(recentTab);
}

void OBrowserMenu::setLightGrayTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #e1e1e3;}"
                                "QMenu::item {color: #353535; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                "QMenu::item:selected {background-color: #e1e1e3; border-top: 1px solid #e1e1e3; border-bottom: 1px solid #e1e1e3;}"
                                "QMenu::item:!enabled {color: #bbbbbb;}"
                                "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                "QMenu::separator {background-color: #e1e1e3; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                "QMenu::icon {margin-left: 20px;}"));

    historyBar->setStyleSheet(QLatin1String("QToolBar {background-color: #ffffff; border: none; padding: 0px; padding-left: 5px;}"));
    recentTabsLabel->setStyleSheet(QLatin1String("QLabel {background-color: transparent; border: none; font-size: 12.5px; color: #888888;"
                                                 "padding-left: 28px;}"));

    zoomActionBar->setStyleSheet(QLatin1String("QToolBar {background-color: #ffffff; border: none; padding: 0px;}"
                                               "QToolBar::separator {background-color: #ffffff; width: 30px;}"));

    zoomLabel->setStyleSheet(QLatin1String("QLabel {background-color: transparent; border: none; font-size: 13px; color: #353535;"
                                           "padding-left: 19.6px;}"));

    zoomOut->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; border-radius: 3.5%;}"
                                         "QToolButton:hover {background-color: #e1e1e3; border: 1px solid #e1e1e3;}"
                                         "QToolButton:pressed {background-color: #c4c4c6; border: 1px solid #c4c4c6;}"));

    zoomIn->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; border-radius: 3.5%;}"
                                        "QToolButton:hover {background-color: #e1e1e3; border: 1px solid #e1e1e3;}"
                                        "QToolButton:pressed {background-color: #c4c4c6; border: 1px solid #c4c4c6;}"));

    resetZoom->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; border-radius: 3.5%; color: #353535; font-size: 12px;}"
                                           "QToolButton:hover {background-color: #e1e1e3; border: 1px solid #e1e1e3;}"
                                           "QToolButton:pressed {background-color: #c4c4c6; border: 1px solid #c4c4c6;}"));
    //icon setup
    newTab->setIcon(QIcon(":/images/newTab.png"));
    newWindow->setIcon(QIcon(":/images/newWindow.png"));
    newPrivateWindow->setIcon(QIcon(":/images/private.png"));
    history->setIcon(QIcon(":/images/history.png"));
    download->setIcon(QIcon(":/images/downloads.png"));
    bookmark->setIcon(QIcon(":/images/bookmarks.png"));
    markPage->setIcon(QIcon(":/images/bookmarkOff.png"));
    fullscreen->setIcon(QIcon(":/images/fullscreen.png"));
    print->setIcon(QIcon(":/images/print.png"));
    find->setIcon(QIcon(":/images/find.png"));
    webDeveloper->setIcon(QIcon(":/images/webDev.png"));
    savePage->setIcon(QIcon(":/images/savePage.png"));
    pageCode->setIcon(QIcon(":/images/pageCode.png"));
    settings->setIcon(QIcon(":/images/settings.png"));
    helpCenter->setIcon(QIcon(":/images/help.png"));
    quit->setIcon(QIcon(":/images/quit.png"));
    zoomOut->setIcon(QIcon(":/images/minus.png"));
    zoomIn->setIcon(QIcon(":/images/plus.png"));
    report->setIcon(QIcon(":/images/reportIssue.png"));
}

void OBrowserMenu::setLightTurquoiseTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"
                                "QMenu::item {color: #606060; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                "QMenu::item:selected {background-color: #c0ebee; border-top: 1px solid #c0ebee; border-bottom: 1px solid #c0ebee;}"
                                "QMenu::item:!enabled {color: #bbbbbb;}"
                                "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                "QMenu::separator {background-color: #efeff1; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                "QMenu::icon {margin-left: 20px;}"));

    historyBar->setStyleSheet(QLatin1String("QToolBar {background-color: #ffffff; border: none; padding: 0px; padding-left: 5px;}"));
    recentTabsLabel->setStyleSheet(QLatin1String("QLabel {background-color: transparent; border: none; font-size: 12.5px; color: #888888;"
                                                 "padding-left: 28px;}"));

    zoomActionBar->setStyleSheet(QLatin1String("QToolBar {background-color: #ffffff; border: none; padding: 0px;}"
                                               "QToolBar::separator {background-color: #ffffff; width: 30px;}"));

    zoomLabel->setStyleSheet(QLatin1String("QLabel {background-color: transparent; border: none; font-size: 13px; color: #656565;"
                                           "padding-left: 19.6px;}"));

    zoomOut->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; border-radius: 3.5%;}"
                                         "QToolButton:hover {background-color: #c0ebee; border: 1px solid #c0ebee;}"
                                         "QToolButton:pressed {background-color: #b0dadd; border: 1px solid #b0dadd;}"));

    zoomIn->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; border-radius: 3.5%;}"
                                        "QToolButton:hover {background-color: #c0ebee; border: 1px solid #c0ebee;}"
                                        "QToolButton:pressed {background-color: #b0dadd; border: 1px solid #b0dadd;}"));

    resetZoom->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; border-radius: 3.5%; color: #656565; font-size: 12px;}"
                                           "QToolButton:hover {background-color: #c0ebee; border: 1px solid #c0ebee;}"
                                           "QToolButton:pressed {background-color: #b0dadd; border: 1px solid #b0dadd;}"));
    //icon setup
    newTab->setIcon(QIcon(":/images/newTab.png"));
    newWindow->setIcon(QIcon(":/images/newWindow.png"));
    newPrivateWindow->setIcon(QIcon(":/images/private.png"));
    history->setIcon(QIcon(":/images/history.png"));
    download->setIcon(QIcon(":/images/downloads.png"));
    bookmark->setIcon(QIcon(":/images/bookmarks.png"));
    markPage->setIcon(QIcon(":/images/bookmarkOff.png"));
    fullscreen->setIcon(QIcon(":/images/fullscreen.png"));
    print->setIcon(QIcon(":/images/print.png"));
    find->setIcon(QIcon(":/images/find.png"));
    webDeveloper->setIcon(QIcon(":/images/webDev.png"));
    savePage->setIcon(QIcon(":/images/savePage.png"));
    pageCode->setIcon(QIcon(":/images/pageCode.png"));
    settings->setIcon(QIcon(":/images/settings.png"));
    helpCenter->setIcon(QIcon(":/images/help.png"));
    quit->setIcon(QIcon(":/images/quit.png"));
    zoomOut->setIcon(QIcon(":/images/minus.png"));
    zoomIn->setIcon(QIcon(":/images/plus.png"));
    report->setIcon(QIcon(":/images/reportIssue.png"));
}

void OBrowserMenu::setLightBrownTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"
                                "QMenu::item {color: #606060; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                "QMenu::item:selected {background-color: #ffd0ae; border-top: 1px solid #ffd0ae; border-bottom: 1px solid #ffd0ae;}"
                                "QMenu::item:!enabled {color: #bbbbbb;}"
                                "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                "QMenu::separator {background-color: #efeff1; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                "QMenu::icon {margin-left: 20px;}"));

    historyBar->setStyleSheet(QLatin1String("QToolBar {background-color: #ffffff; border: none; padding: 0px; padding-left: 5px;}"));
    recentTabsLabel->setStyleSheet(QLatin1String("QLabel {background-color: transparent; border: none; font-size: 12.5px; color: #888888;"
                                                 "padding-left: 28px;}"));

    zoomActionBar->setStyleSheet(QLatin1String("QToolBar {background-color: #ffffff; border: none; padding: 0px;}"
                                               "QToolBar::separator {background-color: #ffffff; width: 30px;}"));

    zoomLabel->setStyleSheet(QLatin1String("QLabel {background-color: transparent; border: none; font-size: 13px; color: #656565;"
                                           "padding-left: 19.6px;}"));

    zoomOut->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; border-radius: 3.5%;}"
                                         "QToolButton:hover {background-color: #ffd0ae; border: 1px solid #ffd0ae;}"
                                         "QToolButton:pressed {background-color: #ebbfa0; border: 1px solid #ebbfa0;}"));

    zoomIn->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; border-radius: 3.5%;}"
                                        "QToolButton:hover {background-color: #ffd0ae; border: 1px solid #ffd0ae;}"
                                        "QToolButton:pressed {background-color: #ebbfa0; border: 1px solid #ebbfa0;}"));

    resetZoom->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; border-radius: 3.5%; color: #656565; font-size: 12px;}"
                                           "QToolButton:hover {background-color: #ffd0ae; border: 1px solid #ffd0ae;}"
                                           "QToolButton:pressed {background-color: #ebbfa0; border: 1px solid #ebbfa0;}"));
    //icon setup
    newTab->setIcon(QIcon(":/images/newTab.png"));
    newWindow->setIcon(QIcon(":/images/newWindow.png"));
    newPrivateWindow->setIcon(QIcon(":/images/private.png"));
    history->setIcon(QIcon(":/images/history.png"));
    download->setIcon(QIcon(":/images/downloads.png"));
    bookmark->setIcon(QIcon(":/images/bookmarks.png"));
    markPage->setIcon(QIcon(":/images/bookmarkOff.png"));
    fullscreen->setIcon(QIcon(":/images/fullscreen.png"));
    print->setIcon(QIcon(":/images/print.png"));
    find->setIcon(QIcon(":/images/find.png"));
    webDeveloper->setIcon(QIcon(":/images/webDev.png"));
    savePage->setIcon(QIcon(":/images/savePage.png"));
    pageCode->setIcon(QIcon(":/images/pageCode.png"));
    settings->setIcon(QIcon(":/images/settings.png"));
    helpCenter->setIcon(QIcon(":/images/help.png"));
    quit->setIcon(QIcon(":/images/quit.png"));
    zoomOut->setIcon(QIcon(":/images/minus.png"));
    zoomIn->setIcon(QIcon(":/images/plus.png"));
    report->setIcon(QIcon(":/images/reportIssue.png"));
}

void OBrowserMenu::setDarkGrayTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #434344; padding: 0px; border: 1px solid #434344;}"
                                "QMenu::item {color: #dddddd; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                "QMenu::item:selected {background-color: #6b6b6b; border-top: 1px solid #6b6b6b; border-bottom: 1px solid #6b6b6b;}"
                                "QMenu::item:!enabled {color: #888888;}"
                                "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                "QMenu::separator {background-color: #6b6b6b; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                "QMenu::icon {margin-left: 20px;}"));

    historyBar->setStyleSheet(QLatin1String("QToolBar {background-color: #434344; border: none; padding: 0px; padding-left: 5px;}"));
    recentTabsLabel->setStyleSheet(QLatin1String("QLabel {background-color: transparent; border: none; font-size: 12.5px; color: #dddddd;"
                                                 "padding-left: 28px;}"));

    zoomActionBar->setStyleSheet(QLatin1String("QToolBar {background-color: #434344; border: none; padding: 0px;}"
                                               "QToolBar::separator {background-color: #434344; width: 30px;}"));

    zoomLabel->setStyleSheet(QLatin1String("QLabel {background-color: transparent; border: none; font-size: 13px; color: #dddddd;"
                                           "padding-left: 19.6px;}"));

    zoomOut->setStyleSheet(QLatin1String("QToolButton {background-color: #434344; border: 1px solid #434344; border-radius: 3.5%;}"
                                         "QToolButton:hover {background-color: #6b6b6b; border: 1px solid #6b6b6b;}"
                                         "QToolButton:pressed {background-color: #39393a; border: 1px solid #39393a;}"));

    zoomIn->setStyleSheet(QLatin1String("QToolButton {background-color: #434344; border: 1px solid #434344; border-radius: 3.5%;}"
                                        "QToolButton:hover {background-color: #6b6b6b; border: 1px solid #6b6b6b;}"
                                        "QToolButton:pressed {background-color: #39393a; border: 1px solid #39393a;}"));

    resetZoom->setStyleSheet(QLatin1String("QToolButton {background-color: #434344; border: 1px solid #434344; border-radius: 3.5%; color: #dddddd; font-size: 12px;}"
                                           "QToolButton:hover {background-color: #6b6b6b; border: 1px solid #6b6b6b;}"
                                           "QToolButton:pressed {background-color: #39393a; border: 1px solid #39393a;}"));

    //icon setup
    newTab->setIcon(QIcon(":/private_mode_images/newTabPrivateMode.png"));
    newWindow->setIcon(QIcon(":/private_mode_images/newWindowPrivateMode.png"));
    newPrivateWindow->setIcon(QIcon(":/private_mode_images/privatePrivateMode.png"));
    history->setIcon(QIcon(":/private_mode_images/historyPrivateMode.png"));
    download->setIcon(QIcon(":/private_mode_images/downloadsPrivateMode.png"));
    bookmark->setIcon(QIcon(":/private_mode_images/bookmarksPrivateMode.png"));
    markPage->setIcon(QIcon(":/private_mode_images/bookmarkOffPrivateMode.png"));
    fullscreen->setIcon(QIcon(":/private_mode_images/fullscreenPrivateMode.png"));
    print->setIcon(QIcon(":/private_mode_images/printPrivateMode.png"));
    find->setIcon(QIcon(":/private_mode_images/findPrivateMode.png"));
    webDeveloper->setIcon(QIcon(":/private_mode_images/webDevPrivateMode.png"));
    savePage->setIcon(QIcon(":/private_mode_images/savePagePrivateMode.png"));
    pageCode->setIcon(QIcon(":/private_mode_images/pageCodePrivateMode.png"));
    settings->setIcon(QIcon(":/private_mode_images/settingsPrivateMode.png"));
    helpCenter->setIcon(QIcon(":/private_mode_images/helpPrivateMode.png"));
    quit->setIcon(QIcon(":/private_mode_images/quitPrivateMode.png"));
    zoomOut->setIcon(QIcon(":/private_mode_images/minusPrivateMode.png"));
    zoomIn->setIcon(QIcon(":/private_mode_images/plusPrivateMode.png"));
    report->setIcon(QIcon(":/private_mode_images/reportIssuePrivateMode.png"));
}

void OBrowserMenu::setDarkTurquoiseTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #1e4446; padding: 0px; border: 1px solid #1e4446;}"
                                "QMenu::item {color: #dddddd; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                "QMenu::item:selected {background-color: #2e686b; border-top: 1px solid #2e686b; border-bottom: 1px solid #2e686b;}"
                                "QMenu::item:!enabled {color: #888888;}"
                                "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                "QMenu::separator {background-color: #2e686b; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                "QMenu::icon {margin-left: 20px;}"));

    historyBar->setStyleSheet(QLatin1String("QToolBar {background-color: #1e4446; border: none; padding: 0px; padding-left: 5px;}"));

    recentTabsLabel->setStyleSheet(QLatin1String("QLabel {background-color: transparent; border: none; font-size: 12.5px; color: #dddddd;"
                                                 "padding-left: 28px;}"));

    zoomActionBar->setStyleSheet(QLatin1String("QToolBar {background-color: #1e4446; border: none; padding: 0px;}"
                                               "QToolBar::separator {background-color: #1e4446; width: 30px;}"));

    zoomLabel->setStyleSheet(QLatin1String("QLabel {background-color: transparent; border: none; font-size: 13px; color: #dddddd;"
                                           "padding-left: 19.6px;}"));

    zoomOut->setStyleSheet(QLatin1String("QToolButton {background-color: #1e4446; border: 1px solid #1e4446; border-radius: 3.5%;}"
                                         "QToolButton:hover {background-color: #2e686b; border: 1px solid #2e686b;}"
                                         "QToolButton:pressed {background-color: #193a3b; border: 1px solid #193a3b;}"));

    zoomIn->setStyleSheet(QLatin1String("QToolButton {background-color: #1e4446; border: 1px solid #1e4446; border-radius: 3.5%;}"
                                        "QToolButton:hover {background-color: #2e686b; border: 1px solid #2e686b;}"
                                        "QToolButton:pressed {background-color: #193a3b; border: 1px solid #193a3b;}"));

    resetZoom->setStyleSheet(QLatin1String("QToolButton {background-color: #1e4446; border: 1px solid #1e4446; border-radius: 3.5%; color: #dddddd; font-size: 12px;}"
                                           "QToolButton:hover {background-color: #2e686b; border: 1px solid #2e686b;}"
                                           "QToolButton:pressed {background-color: #193a3b; border: 1px solid #193a3b;}"));

    //icon setup
    newTab->setIcon(QIcon(":/private_mode_images/newTabPrivateMode.png"));
    newWindow->setIcon(QIcon(":/private_mode_images/newWindowPrivateMode.png"));
    newPrivateWindow->setIcon(QIcon(":/private_mode_images/privatePrivateMode.png"));
    history->setIcon(QIcon(":/private_mode_images/historyPrivateMode.png"));
    download->setIcon(QIcon(":/private_mode_images/downloadsPrivateMode.png"));
    bookmark->setIcon(QIcon(":/private_mode_images/bookmarksPrivateMode.png"));
    markPage->setIcon(QIcon(":/private_mode_images/bookmarkOffPrivateMode.png"));
    fullscreen->setIcon(QIcon(":/private_mode_images/fullscreenPrivateMode.png"));
    print->setIcon(QIcon(":/private_mode_images/printPrivateMode.png"));
    find->setIcon(QIcon(":/private_mode_images/findPrivateMode.png"));
    webDeveloper->setIcon(QIcon(":/private_mode_images/webDevPrivateMode.png"));
    savePage->setIcon(QIcon(":/private_mode_images/savePagePrivateMode.png"));
    pageCode->setIcon(QIcon(":/private_mode_images/pageCodePrivateMode.png"));
    settings->setIcon(QIcon(":/private_mode_images/settingsPrivateMode.png"));
    helpCenter->setIcon(QIcon(":/private_mode_images/helpPrivateMode.png"));
    quit->setIcon(QIcon(":/private_mode_images/quitPrivateMode.png"));
    zoomOut->setIcon(QIcon(":/private_mode_images/minusPrivateMode.png"));
    zoomIn->setIcon(QIcon(":/private_mode_images/plusPrivateMode.png"));
    report->setIcon(QIcon(":/private_mode_images/reportIssuePrivateMode.png"));
}

void OBrowserMenu::setDarkBrownTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #462b21; padding: 0px; border: 1px solid #462b21;}"
                                "QMenu::item {color: #dddddd; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                "QMenu::item:selected {background-color: #6b4132; border-top: 1px solid #6b4132; border-bottom: 1px solid #6b4132;}"
                                "QMenu::item:!enabled {color: #888888;}"
                                "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                "QMenu::separator {background-color: #6b4132; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                "QMenu::icon {margin-left: 20px;}"));

    historyBar->setStyleSheet(QLatin1String("QToolBar {background-color: #462b21; border: none; padding: 0px; padding-left: 5px;}"));

    recentTabsLabel->setStyleSheet(QLatin1String("QLabel {background-color: transparent; border: none; font-size: 12.5px; color: #dddddd;"
                                                 "padding-left: 28px;}"));

    zoomActionBar->setStyleSheet(QLatin1String("QToolBar {background-color: #462b21; border: none; padding: 0px;}"
                                               "QToolBar::separator {background-color: #462b21; width: 30px;}"));

    zoomLabel->setStyleSheet(QLatin1String("QLabel {background-color: transparent; border: none; font-size: 13px; color: #dddddd;"
                                           "padding-left: 19.6px;}"));

    zoomOut->setStyleSheet(QLatin1String("QToolButton {background-color: #462b21; border: 1px solid #462b21; border-radius: 3.5%;}"
                                         "QToolButton:hover {background-color: #6b4132; border: 1px solid #6b4132;}"
                                         "QToolButton:pressed {background-color: #3b231c; border: 1px solid #3b231c;}"));

    zoomIn->setStyleSheet(QLatin1String("QToolButton {background-color: #462b21; border: 1px solid #462b21; border-radius: 3.5%;}"
                                        "QToolButton:hover {background-color: #6b4132; border: 1px solid #6b4132;}"
                                        "QToolButton:pressed {background-color: #3b231c; border: 1px solid #3b231c;}"));

    resetZoom->setStyleSheet(QLatin1String("QToolButton {background-color: #462b21; border: 1px solid #462b21; border-radius: 3.5%; color: #dddddd; font-size: 12px;}"
                                           "QToolButton:hover {background-color: #6b4132; border: 1px solid #6b4132;}"
                                           "QToolButton:pressed {background-color: #3b231c; border: 1px solid #3b231c;}"));

    //icon setup
    newTab->setIcon(QIcon(":/private_mode_images/newTabPrivateMode.png"));
    newWindow->setIcon(QIcon(":/private_mode_images/newWindowPrivateMode.png"));
    newPrivateWindow->setIcon(QIcon(":/private_mode_images/privatePrivateMode.png"));
    history->setIcon(QIcon(":/private_mode_images/historyPrivateMode.png"));
    download->setIcon(QIcon(":/private_mode_images/downloadsPrivateMode.png"));
    bookmark->setIcon(QIcon(":/private_mode_images/bookmarksPrivateMode.png"));
    markPage->setIcon(QIcon(":/private_mode_images/bookmarkOffPrivateMode.png"));
    fullscreen->setIcon(QIcon(":/private_mode_images/fullscreenPrivateMode.png"));
    print->setIcon(QIcon(":/private_mode_images/printPrivateMode.png"));
    find->setIcon(QIcon(":/private_mode_images/findPrivateMode.png"));
    webDeveloper->setIcon(QIcon(":/private_mode_images/webDevPrivateMode.png"));
    savePage->setIcon(QIcon(":/private_mode_images/savePagePrivateMode.png"));
    pageCode->setIcon(QIcon(":/private_mode_images/pageCodePrivateMode.png"));
    settings->setIcon(QIcon(":/private_mode_images/settingsPrivateMode.png"));
    helpCenter->setIcon(QIcon(":/private_mode_images/helpPrivateMode.png"));
    quit->setIcon(QIcon(":/private_mode_images/quitPrivateMode.png"));
    zoomOut->setIcon(QIcon(":/private_mode_images/minusPrivateMode.png"));
    zoomIn->setIcon(QIcon(":/private_mode_images/plusPrivateMode.png"));
    report->setIcon(QIcon(":/private_mode_images/reportIssuePrivateMode.png"));
}

void OBrowserMenu::setPrivateTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #3c3244; padding: 0px; border: 1px solid #3c3244;}"
                                "QMenu::item {color: #dddddd; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                "QMenu::item:selected {background-color: #5e4e6b; border-top: 1px solid #5e4e6b; border-bottom: 1px solid #5e4e6b;}"
                                "QMenu::item:!enabled {color: #888888;}"
                                "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                "QMenu::separator {background-color: #5e4e6b; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                "QMenu::icon {margin-left: 20px;}"));

    historyBar->setStyleSheet(QLatin1String("QToolBar {background-color: #3c3244; border: none; padding: 0px; padding-left: 5px;}"));

    recentTabsLabel->setStyleSheet(QLatin1String("QLabel {background-color: transparent; border: none; font-size: 12.5px; color: #dddddd;"
                                                 "padding-left: 28px;}"));

    zoomActionBar->setStyleSheet(QLatin1String("QToolBar {background-color: #3c3244; border: none; padding: 0px;}"
                                               "QToolBar::separator {background-color: #3c3244; width: 30px;}"));

    zoomLabel->setStyleSheet(QLatin1String("QLabel {background-color: transparent; border: none; font-size: 13px; color: #dddddd;"
                                           "padding-left: 19.6px;}"));

    zoomOut->setStyleSheet(QLatin1String("QToolButton {background-color: #3c3244; border: 1px solid #3c3244; border-radius: 3.5%;}"
                                         "QToolButton:hover {background-color: #5e4e6b; border: 1px solid #5e4e6b;}"
                                         "QToolButton:pressed {background-color: #332b3a; border: 1px solid #393041;}"));

    zoomIn->setStyleSheet(QLatin1String("QToolButton {background-color: #3c3244; border: 1px solid #3c3244; border-radius: 3.5%;}"
                                        "QToolButton:hover {background-color: #5e4e6b; border: 1px solid #5e4e6b;}"
                                        "QToolButton:pressed {background-color: #332b3a; border: 1px solid #393041;}"));

    resetZoom->setStyleSheet(QLatin1String("QToolButton {background-color: #3c3244; border: 1px solid #3c3244; border-radius: 3.5%; color: #dddddd; font-size: 12px;}"
                                           "QToolButton:hover {background-color: #5e4e6b; border: 1px solid #5e4e6b;}"
                                           "QToolButton:pressed {background-color: #332b3a; border: 1px solid #393041;}"));

    //icon setup
    newTab->setIcon(QIcon(":/private_mode_images/newTabPrivateMode.png"));
    newWindow->setIcon(QIcon(":/private_mode_images/newWindowPrivateMode.png"));
    newPrivateWindow->setIcon(QIcon(":/private_mode_images/privatePrivateMode.png"));
    history->setIcon(QIcon(":/private_mode_images/historyPrivateMode.png"));
    download->setIcon(QIcon(":/private_mode_images/downloadsPrivateMode.png"));
    bookmark->setIcon(QIcon(":/private_mode_images/bookmarksPrivateMode.png"));
    markPage->setIcon(QIcon(":/private_mode_images/bookmarkOffPrivateMode.png"));
    fullscreen->setIcon(QIcon(":/private_mode_images/fullscreenPrivateMode.png"));
    print->setIcon(QIcon(":/private_mode_images/printPrivateMode.png"));
    find->setIcon(QIcon(":/private_mode_images/findPrivateMode.png"));
    webDeveloper->setIcon(QIcon(":/private_mode_images/webDevPrivateMode.png"));
    savePage->setIcon(QIcon(":/private_mode_images/savePagePrivateMode.png"));
    pageCode->setIcon(QIcon(":/private_mode_images/pageCodePrivateMode.png"));
    settings->setIcon(QIcon(":/private_mode_images/settingsPrivateMode.png"));
    helpCenter->setIcon(QIcon(":/private_mode_images/helpPrivateMode.png"));
    quit->setIcon(QIcon(":/private_mode_images/quitPrivateMode.png"));
    zoomOut->setIcon(QIcon(":/private_mode_images/minusPrivateMode.png"));
    zoomIn->setIcon(QIcon(":/private_mode_images/plusPrivateMode.png"));
    report->setIcon(QIcon(":/private_mode_images/reportIssuePrivateMode.png"));
}

void OBrowserMenu::setZoomValue(qreal zoom)
{
    resetZoom->setText(QString::number(zoom*100)+QString("%"));
}

void OBrowserMenu::clearHistoryList()
{
    for (int i = 0; i < historyMenu->actions().count(); i++)
    {
        if (i > 3)
        {
            historyMenu->removeAction(historyMenu->actions().at(i));
            i--;
        }
    }
}
