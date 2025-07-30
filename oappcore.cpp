#include "oappcore.h"
#include "customwindow.h"
#include "otabpage.h"
#include "otabbar.h"
#include "owebview.h"
#include "ohistorycore.h"
#include "obookmarkscore.h"
#include "odownloadscore.h"
#include "osettingscore.h"
#include "oassistantcore.h"
#include "ostartpagecore.h"
#include "customtoolbar.h"
#include "osmartinput.h"
#include <QMouseEvent>
#include <QDir>
#include <QGuiApplication>
#include <QScreen>
#include <QTimer>

OAppCore::OAppCore(int argc, char *argv[])
    :QApplication(argc, argv)
{
    for (int i = 0; i < argc; i++)
        appArguments << argv[i];

    appArguments.removeFirst();
    appArguments.removeLast();

    setApplicationVersion(QLatin1String("2.1.662.16"));
    setApplicationName("Ololan Browser");
    setQuitOnLastWindowClosed(false);
    setHistoryOpened(false);
    setBookmarksOpened(false);
    setDownloadsOpened(false);
    setSettingsOpened(false);

    QDir appDirectory(applicationDirPath());
    if (!appDirectory.exists(QLatin1String("browserDBDir")))
        appDirectory.mkdir(QLatin1String("browserDBDir"));

    forceExit = false;
    isDownloading = false;
    historyWinIndex = -1;
    bookmarksWinIndex = -1;
    downloadsWinIndex = -1;
    settingsWinIndex = -1;
    assistantManager = new OAssistantCore(this);
    historyManager = new OHistoryCore(this);
    bookmarksManager = new OBookmarksCore(this);
    downloadsManager = new ODownloadsCore(this);
    settingsManager = new OSettingsCore(this);
    startPageManager = new OStartPageCore(this);
    geolocation = settingsManager->getEnableGeolocation();
    passwordSaving = settingsManager->getPasswordSaving();
    siteAutoSignIn = settingsManager->getSiteAutoSignIn();
    paymentSaveAndFill = settingsManager->getPaymentSaveAndFill();
    doNotTrack = settingsManager->getDoNotTrack();
    enableAssistant = settingsManager->getEnableAssistant();
    askDownloadPath = settingsManager->getAskDownloadPath();
    bookmarkBarVisibile = settingsManager->getBookmarkBarVisibile();
    pageZoom = settingsManager->getWebPageZoom();
    ololanTheme = settingsManager->getOlolanTheme();
    ololanSearchEngine = settingsManager->getSearchEngine();
    enableCamera = settingsManager->getEnableCamera();
    enableMicrophone = settingsManager->getEnableMicrophone();
    webProtection = settingsManager->getWebProtection();
    onStartup = settingsManager->getStartupOption();
    loggedIn = settingsManager->getLoggedStatus();
    accountName = settingsManager->getUsername();
    accountEmail = settingsManager->getEmail();
    accountPassword = settingsManager->getAccountPassword();
    startPageManager->setupLastItem(historyManager->getHistoryItemsList());
    startPageManager->setSearchEngineName(ololanSearchEngine);
    assistantManager->setHistory(historyManager);
    assistantManager->setBookmark(bookmarksManager);
    assistantManager->setDownload(downloadsManager);
}

OAppCore::~OAppCore()
{
    historyManager->saveDatabase();
    historyManager->deleteLater();
    bookmarksManager->saveDatabase();
    bookmarksManager->deleteLater();
    downloadsManager->saveDatabase();
    downloadsManager->deleteLater();
    settingsManager->saveDatabase();
    quit();
}

void OAppCore::openBrowserWindow()
{
    if (windowsList.count() > 0)
    {
        WINDOWPLACEMENT wp;
        GetWindowPlacement(windowsList.last()->getCustomWinHandle(), &wp);

        if (wp.showCmd == SW_SHOWMAXIMIZED)
        {
            CustomWindow *browserWindow = new CustomWindow(this, WindowType::Normal);
            OTabPage *centralWidget = static_cast<OTabPage*>(browserWindow->getCentralWidget());
            centralWidget->initData(geolocation, passwordSaving, siteAutoSignIn, paymentSaveAndFill, doNotTrack, enableAssistant,
                                    askDownloadPath, bookmarkBarVisibile, pageZoom, ololanTheme, ololanSearchEngine, &closedTabList,
                                    enableCamera, enableMicrophone, accountName);
            centralWidget->openNewTab();
            centralWidget->setDownloadState(isDownloading);
            windowsList.append(browserWindow);
            windowStateList.append(false);
            windowZOrder.prepend(windowsList.count()-1);
            browserWindow->showWindow(SW_SHOWMAXIMIZED);
            UpdateWindow(browserWindow->getCustomWinHandle());
        }
        else if (wp.showCmd == SW_SHOWNORMAL)
        {
            CustomWindow *browserWindow = new CustomWindow(this, WindowType::Normal);
            OTabPage *centralWidget = static_cast<OTabPage*>(browserWindow->getCentralWidget());
            centralWidget->initData(geolocation, passwordSaving, siteAutoSignIn, paymentSaveAndFill, doNotTrack, enableAssistant,
                                    askDownloadPath, bookmarkBarVisibile, pageZoom, ololanTheme, ololanSearchEngine, &closedTabList,
                                    enableCamera, enableMicrophone, accountName);
            centralWidget->openNewTab();
            centralWidget->setDownloadState(isDownloading);
            windowsList.append(browserWindow);
            windowStateList.append(false);
            windowZOrder.prepend(windowsList.count()-1);
            browserWindow->showWindow(SW_SHOW);
            UpdateWindow(browserWindow->getCustomWinHandle());
        }
    }
    else
    {
        CustomWindow *browserWindow = new CustomWindow(this, WindowType::Normal);
        OTabPage *centralWidget = static_cast<OTabPage*>(browserWindow->getCentralWidget());
        centralWidget->initData(geolocation, passwordSaving, siteAutoSignIn, paymentSaveAndFill, doNotTrack, enableAssistant,
                                askDownloadPath, bookmarkBarVisibile, pageZoom, ololanTheme, ololanSearchEngine, &closedTabList,
                                enableCamera, enableMicrophone, accountName);

        if (appArguments.count() > 0)
        {
            for (int i = 0; i < appArguments.count(); i++)
            {
                if (i == 0)
                {
                    centralWidget->openNewTab();
                    centralWidget->getStackPage()->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"))->loadPage(QUrl::fromUserInput(appArguments.at(i)).toEncoded());
                }
                else
                {
                    centralWidget->openInTab(QUrl::fromUserInput(appArguments.at(i)).toEncoded());
                }
            }
            centralWidget->setDownloadState(isDownloading);
        }
        else if (onStartup.compare("start page") == 0)
        {
            centralWidget->openNewTab();
            centralWidget->setDownloadState(isDownloading);
        }
        else if (onStartup.compare("left page") == 0)
        {
            QList<OHistoryItem*> leftPages = historyManager->getHistoryItemsList();
            for (int i = 0; (i < 10) && (i < leftPages.count()); i++)
            {
                if (i == 0)
                {
                    centralWidget->openNewTab();
                    centralWidget->setDownloadState(isDownloading);
                    centralWidget->getStackPage()->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"))->load(leftPages.at(i)->getUrl());
                }
                else
                {
                    centralWidget->openInTab(leftPages.at(i)->getUrl().toEncoded());
                    centralWidget->setDownloadState(isDownloading);
                }
            }

            if (leftPages.isEmpty())
            {
                centralWidget->openNewTab();
                centralWidget->setDownloadState(isDownloading);
            }
            else
            {
                QTimer *timer = new QTimer(this);
                timer->setInterval(1000);
                connect(timer, &QTimer::timeout, this, [=]() { centralWidget->getTabBar()->resizeTabs(); timer->stop(); });
                timer->start();
            }
        }
        else if (onStartup.compare("specific page") == 0)
        {
            QList<QString> pages = settingsManager->getSpecificPageList();
            for (int i = 0; i < pages.count(); i++)
            {
                if (i == 0)
                {
                    centralWidget->openNewTab();
                    centralWidget->setDownloadState(isDownloading);
                    centralWidget->getStackPage()->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"))->load(QUrl::fromUserInput(pages.at(i)));
                }
                else
                {
                    centralWidget->openInTab(pages.at(i));
                    centralWidget->setDownloadState(isDownloading);
                }
            }

            if (pages.isEmpty())
            {
                centralWidget->openNewTab();
                centralWidget->setDownloadState(isDownloading);
            }
            else
            {
                QTimer *timer = new QTimer(this);
                timer->setInterval(1000);
                connect(timer, &QTimer::timeout, this, [=]() { centralWidget->getTabBar()->resizeTabs(); timer->stop(); });
                timer->start();
            }
        }

        windowsList.append(browserWindow);
        windowStateList.append(false);
        windowZOrder.prepend(windowsList.count()-1);
        browserWindow->showWindow(SW_SHOWMAXIMIZED);
        UpdateWindow(browserWindow->getCustomWinHandle());
    }
}

void OAppCore::openPrivateBrowserWindow()
{
    WINDOWPLACEMENT wp;
    GetWindowPlacement(windowsList.last()->getCustomWinHandle(), &wp);

    if (wp.showCmd == SW_SHOWMAXIMIZED)
    {
        CustomWindow *privateBrowserWindow  = new CustomWindow(this, WindowType::Private);
        OTabPage *centralWidget = static_cast<OTabPage*>(privateBrowserWindow->getCentralWidget());
        centralWidget->initData(geolocation, passwordSaving, siteAutoSignIn, paymentSaveAndFill, doNotTrack, enableAssistant,
                                askDownloadPath, bookmarkBarVisibile, pageZoom, ololanTheme, ololanSearchEngine, &closedTabList,
                                enableCamera, enableMicrophone, accountName);
        centralWidget->openNewTab();
        centralWidget->setDownloadState(isDownloading);
        windowsList.append(privateBrowserWindow);
        windowStateList.append(false);
        windowZOrder.prepend(windowsList.count()-1);
        privateBrowserWindow->showWindow(SW_SHOWMAXIMIZED);
        UpdateWindow(privateBrowserWindow->getCustomWinHandle());
    }
    else if (wp.showCmd == SW_SHOWNORMAL)
    {
        CustomWindow *privateBrowserWindow = new CustomWindow(this, WindowType::Private);
        OTabPage *centralWidget = static_cast<OTabPage*>(privateBrowserWindow->getCentralWidget());
        centralWidget->initData(geolocation, passwordSaving, siteAutoSignIn, paymentSaveAndFill, doNotTrack, enableAssistant,
                                askDownloadPath, bookmarkBarVisibile, pageZoom, ololanTheme, ololanSearchEngine, &closedTabList,
                                enableCamera, enableMicrophone, accountName);
        centralWidget->openNewTab();
        centralWidget->setDownloadState(isDownloading);
        windowsList.append(privateBrowserWindow);
        windowStateList.append(false);
        windowZOrder.prepend(windowsList.count()-1);
        privateBrowserWindow->showWindow(SW_SHOW);
        UpdateWindow(privateBrowserWindow->getCustomWinHandle());
    }
}

void OAppCore::openNormalSizeWindow(int openMode, WindowType type, const QPoint mousePos)
{
    CustomWindow *normalSizeWindow = new CustomWindow(this, type, mousePos.x(), mousePos.y());
    OTabPage *centralWidget = static_cast<OTabPage*>(normalSizeWindow->getCentralWidget());
    centralWidget->initData(geolocation, passwordSaving, siteAutoSignIn, paymentSaveAndFill, doNotTrack, enableAssistant,
                            askDownloadPath, bookmarkBarVisibile, pageZoom, ololanTheme, ololanSearchEngine, &closedTabList,
                            enableCamera, enableMicrophone, accountName);
    centralWidget->grabTabPage();
    centralWidget->setDownloadState(isDownloading);
    windowsList.append(normalSizeWindow);
    windowStateList.append(false);
    windowZOrder.prepend(windowsList.count()-1);
    normalSizeWindow->showWindow(openMode);
    UpdateWindow(normalSizeWindow->getCustomWinHandle());
}

void OAppCore::exitFromBrowser()
{
    forceExit = true;

    while (windowsList.count() > 0)
        windowsList.first()->closeWindow();

    if(windowsList.count() < 1)
        exit(EXIT_SUCCESS);
    else
        exit(EXIT_FAILURE);
}

bool OAppCore::isHistoryOpened()
{
    return historyOpened;
}

bool OAppCore::isBookmarksOpened()
{
    return bookmarkOpened;
}

bool OAppCore::isDownloadsOpened()
{
    return downloadOpened;
}

bool OAppCore::isSettingsOpened()
{
    return settingsOpened;
}

void OAppCore::setHistoryOpened(bool isOpened)
{
    historyOpened = isOpened;
}

void OAppCore::setBookmarksOpened(bool isOpened)
{
    bookmarkOpened = isOpened;
}

void OAppCore::setDownloadsOpened(bool isOpened)
{
    downloadOpened = isOpened;
}

void OAppCore::setSettingsOpened(bool isOpened)
{
    settingsOpened = isOpened;
}

QList<CustomWindow*> OAppCore::allWindowsList()
{
    return windowsList;
}

void OAppCore::windowClosed()
{
    if ((windowsList.count() < 1) && (forceExit == false))
    {
        historyManager->saveDatabase();
        bookmarksManager->saveDatabase();
        exit(EXIT_SUCCESS);
    }
}

void OAppCore::updateWindowWidget(HWND hWnd, RECT &rcClient, int windowCmd)
{
    for (int i = 0; i < windowsList.count(); i++)
    {
        if (windowsList.at(i)->getCustomWinHandle() == hWnd)
         {
            OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
            tabPage->setUpdatesEnabled(false);

            if (windowCmd == SW_MAXIMIZE)
            {
                if (tabPage->getWindowType() == Normal)
                {
                    if (ololanTheme.compare("Light gray") == 0)
                        tabPage->findChild<QToolButton*>("maxMinButton")->setIcon(QIcon(":/images/restore.png"));
                    else if (ololanTheme.compare("Light turquoise") == 0)
                        tabPage->findChild<QToolButton*>("maxMinButton")->setIcon(QIcon(":/images/restore.png"));
                    else if (ololanTheme.compare("Light brown") == 0)
                        tabPage->findChild<QToolButton*>("maxMinButton")->setIcon(QIcon(":/images/restore.png"));
                    else if (ololanTheme.compare("Dark gray") == 0)
                        tabPage->findChild<QToolButton*>("maxMinButton")->setIcon(QIcon(":/private_mode_images/restorePrivateMode.png"));
                    else if (ololanTheme.compare("Dark turquoise") == 0)
                        tabPage->findChild<QToolButton*>("maxMinButton")->setIcon(QIcon(":/private_mode_images/restorePrivateMode.png"));
                    else if (ololanTheme.compare("Dark brown") == 0)
                        tabPage->findChild<QToolButton*>("maxMinButton")->setIcon(QIcon(":/private_mode_images/restorePrivateMode.png"));
                }
                else if (tabPage->getWindowType() == Private)
                    tabPage->findChild<QToolButton*>("maxMinButton")->setIcon(QIcon(":/private_mode_images/restorePrivateMode.png"));

                tabPage->setGeometry(rcClient.left+7, rcClient.top+8, (rcClient.right-rcClient.left)-14, (rcClient.bottom-(rcClient.top+8)));
                windowsList.at(i)->setIsMaximized(true);
            }
            else
            {
                if (tabPage->getWindowType() == Normal)
                {
                    if (ololanTheme.compare("Light gray") == 0)
                        tabPage->findChild<QToolButton*>("maxMinButton")->setIcon(QIcon(":/images/maximize.png"));
                    else if (ololanTheme.compare("Light turquoise") == 0)
                        tabPage->findChild<QToolButton*>("maxMinButton")->setIcon(QIcon(":/images/maximize.png"));
                    else if (ololanTheme.compare("Light brown") == 0)
                        tabPage->findChild<QToolButton*>("maxMinButton")->setIcon(QIcon(":/images/maximize.png"));
                    else if (ololanTheme.compare("Dark gray") == 0)
                        tabPage->findChild<QToolButton*>("maxMinButton")->setIcon(QIcon(":/private_mode_images/maximizePrivateMode.png"));
                    else if (ololanTheme.compare("Dark turquoise") == 0)
                        tabPage->findChild<QToolButton*>("maxMinButton")->setIcon(QIcon(":/private_mode_images/maximizePrivateMode.png"));
                    else if (ololanTheme.compare("Dark brown") == 0)
                        tabPage->findChild<QToolButton*>("maxMinButton")->setIcon(QIcon(":/private_mode_images/maximizePrivateMode.png"));
                }
                else if (tabPage->getWindowType() == Private)
                    tabPage->findChild<QToolButton*>("maxMinButton")->setIcon(QIcon(":/private_mode_images/maximizePrivateMode.png"));
                tabPage->setGeometry(rcClient.left+1, rcClient.top+1, (rcClient.right-rcClient.left)-2, (rcClient.bottom-rcClient.top)-2);
                windowsList.at(i)->setIsMaximized(false);
            }

            tabPage->setUpdatesEnabled(true);
            windowsList.at(i)->setWidth(rcClient.right-rcClient.left);
            windowsList.at(i)->setHeight(rcClient.bottom-rcClient.top);
            break;
        }
    }
}

void OAppCore::closeWindowWidget(HWND hWnd)
{
    for (int i = 0; i < windowsList.count(); i++)
    {
        if (windowsList.at(i)->getCustomWinHandle() == hWnd)
        {
            windowsList.at(i)->setCanClose(false);
            OTabPage *centralWidget = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());

            if (!centralWidget->getIsClosed())
                centralWidget->close();

            windowsList.removeAt(i);
            windowStateList.removeAt(i);
            windowZOrder.removeOne(i);

            if (i == historyWinIndex)
                historyClosed();

            if (i == bookmarksWinIndex)
                bookmarksClosed();

            if (i == downloadsWinIndex)
                downloadsClosed();

            if (i == settingsWinIndex)
                settingsClosed();

            for (int j = 0; j < windowZOrder.count(); j++)
                if (windowZOrder.at(j) > i)
                    windowZOrder.replace(j, windowZOrder.at(j)-1);
            break;
        }
    }
    windowClosed();
}

void OAppCore::moveTabToWindow(HWND hWnd)
{
    int firstIndex = -1;
    int secondIndex = -1;
    QPoint firstMousePos;
    QList<int> targetWindows;

    for (int i = 0; i < windowsList.count(); i++)
    {
        if (windowsList.at(i)->getCustomWinHandle() == hWnd)
        {
            firstIndex = i;
            firstMousePos = windowsList.at(i)->getCentralWidget()->cursor().pos();
            break;
        }
    }

    for (int i = 0; i < windowsList.count(); i++)
    {
        if (windowsList.at(i)->getCustomWinHandle() != hWnd)
        {
            OTabPage *targetCentralWidget = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
            QPoint secondMousePos = targetCentralWidget->mapFromGlobal(firstMousePos);
            QRect tabBarGeo = targetCentralWidget->getTabBar()->geometry();
            tabBarGeo.setWidth(tabBarGeo.width()-(45*3+30+3+2));

            if (tabBarGeo.contains(secondMousePos))
                targetWindows.append(i);
        }
    }

    for (int i = 0; i < targetWindows.count(); i++)
    {
        if ((i+1) < targetWindows.count())
        {
            if (i == 0)
                secondIndex = targetWindows.at(i);
            if (windowZOrder.indexOf(secondIndex) > windowZOrder.indexOf(targetWindows.at(i+1)))
                secondIndex = targetWindows.at(i+1);
        }
        else if (targetWindows.count() == 1)
        {
            QList<int> containingWindows;
            secondIndex = targetWindows.at(i);

            for (int j = 0; j < windowsList.count(); j++)
            {
                if ((windowsList.at(j)->getCustomWinHandle() != hWnd) && (windowsList.at(secondIndex)->getCustomWinHandle() != windowsList.at(j)->getCustomWinHandle()))
                {
                    OTabPage *containingWidget = static_cast<OTabPage*>(windowsList.at(j)->getCentralWidget());
                    QPoint secondMousePos = containingWidget->mapFromGlobal(firstMousePos);

                    if (containingWidget->getStackPage()->geometry().contains(secondMousePos) || containingWidget->getTabBar()->geometry().contains(secondMousePos))
                        containingWindows.append(j);
                }
            }

            if (containingWindows.count() > 0)
            {
                for (int k = 0; k < containingWindows.count(); k++)
                {
                    if (windowZOrder.indexOf(containingWindows.at(k)) < windowZOrder.indexOf(secondIndex))
                    {
                        secondIndex = -1;
                        break;
                    }
                }
            }
        }
    }

    if ((firstIndex > -1) && (secondIndex > -1) && (firstIndex != secondIndex))
    {
        OTabPage *originalCentralWidget = static_cast<OTabPage*>(windowsList.at(firstIndex)->getCentralWidget());

        if (originalCentralWidget->getTabBar()->getWindowMoved())
        {
            int distanceWidth = originalCentralWidget->mapFromGlobal(firstMousePos).x() - originalCentralWidget->getTabBar()->getTabAt(0)->x();
            QWidget *pageView = originalCentralWidget->getStackPage()->widget(originalCentralWidget->getTabBar()->currentTab());
            OTabPage *targetCentralWidget = static_cast<OTabPage*>(windowsList.at(secondIndex)->getCentralWidget());
            QPoint secondMousePos = targetCentralWidget->mapFromGlobal(firstMousePos);
            targetCentralWidget->grabTabPage();
            targetCentralWidget->getStackPage()->widget(targetCentralWidget->getTabBar()->currentTab())->findChild<OWebView*>()->setPage(pageView->findChild<OWebView*>()->page());
            targetCentralWidget->getStackPage()->widget(targetCentralWidget->getTabBar()->currentTab())->findChild<OWebView*>()->page()->setParent(targetCentralWidget->getStackPage()->widget(targetCentralWidget->getTabBar()->currentTab())->findChild<OWebView*>());
            targetCentralWidget->getTabBar()->setIsGrabbed(true);

            int newDistWidth = (targetCentralWidget->getTabBar()->getTabsWidth()*distanceWidth)/230;
            int occupiedSpace = targetCentralWidget->getTabBar()->getTabsWidth() * (targetCentralWidget->count() - 1);
            QPoint oldPosition = QPoint(occupiedSpace, targetCentralWidget->getTabBar()->getTabAt(targetCentralWidget->count()-2)->y());
            targetCentralWidget->getTabBar()->setTabToMove(targetCentralWidget->getTabBar()->getTabAt(targetCentralWidget->count()-1));
            targetCentralWidget->getTabBar()->setOldPosition(oldPosition);
            targetCentralWidget->getTabBar()->setDistanceWidth(newDistWidth);
            originalCentralWidget->close();

            QPoint mouseMovePos = QPoint(secondMousePos.x()-(targetCentralWidget->getTabBar()->getTabsWidth()*0.20), secondMousePos.y());
            QMouseEvent *mouseMove = new QMouseEvent(QEvent::MouseMove, mouseMovePos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            raiseWindow(targetCentralWidget->getParentWindow());
            SendMessage((HWND)targetCentralWidget->winId(), WM_LBUTTONDOWN, HTCLIENT, 0);
            postEvent(targetCentralWidget->getTabBar(), mouseMove);
        }
    }
}

void OAppCore::raiseWindow(CustomWindow *window)
{
    if (windowsList.count() != windowStateList.count())
        return;
    if (windowsList.count() < 1)
        return;

    const int index = windowsList.indexOf(window);

    if (index == -1)
        return;
    if (!windowStateList.at(index))
    {
        for (int i = 0; i < windowStateList.count(); i++)
        {
            if (i == index)
                windowStateList.replace(index, true);
            else if (windowStateList.at(i))
                windowStateList.replace(i, false);
        }

        windowZOrder.removeOne(index);
        windowZOrder.prepend(index);
        BringWindowToTop(window->getCustomWinHandle());
    }
}

void OAppCore::setWindowState(HWND hWnd)
{
    for (int i = 0; i < windowsList.count(); i++)
    {
        if (hWnd == windowsList.at(i)->getCustomWinHandle())
        {
            windowStateList.replace(i, false);
            break;
        }
    }
}

OHistoryCore *OAppCore::getHistoryManager()
{
    return historyManager;
}

OBookmarksCore *OAppCore::getBookmarksManager()
{
    return bookmarksManager;
}

ODownloadsCore *OAppCore::getDownloadsManager()
{
    return downloadsManager;
}

OSettingsCore *OAppCore::getSettingsManager()
{
    return settingsManager;
}

OAssistantCore *OAppCore::getAssistantManager()
{
    return assistantManager;
}

OStartPageCore *OAppCore::getStartPageManager()
{
    return startPageManager;
}

void OAppCore::openHistory()
{
    if (!isHistoryOpened())
    {
        setHistoryOpened(true);
        for (int i = 0; i < windowZOrder.count(); i++)
        {
            if (windowsList.at(windowZOrder.at(i))->getWindowType() == Normal)
            {
                historyWinIndex = windowZOrder.at(i);
                OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(historyWinIndex)->getCentralWidget());
                tabPage->openHistory();
                if (!tabPage->isActiveWindow())
                    postEvent(tabPage, new QEvent(QEvent::WindowActivate));
                break;
            }
            else if (i == (windowZOrder.count()-1))
            {
                openBrowserWindow();
                historyWinIndex = windowsList.count() - 1;
                OTabPage *tabPage = static_cast<OTabPage*>(windowsList.last()->getCentralWidget());
                tabPage->openHistory();
                break;
            }
        }
    }
    else
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(historyWinIndex)->getCentralWidget());
        QString historyUrl = QLatin1String("qrc:/web_applications/html/historyView.html");

        for (int i = 0; i < tabPage->count(); i++)
        {
            QString pageUrl = tabPage->getStackPage()->widget(i)->findChild<OWebView*>()->url().toString();

            if (historyUrl.compare(pageUrl) == 0)
            {
                tabPage->getTabBar()->setCurrentTab(i);
                if (!tabPage->isActiveWindow())
                    postEvent(tabPage, new QEvent(QEvent::WindowActivate));
                break;
            }
        }
    }
}

void OAppCore::openBookmarks()
{
    if (!isBookmarksOpened())
    {
        setBookmarksOpened(true);
        for (int i = 0; i < windowZOrder.count(); i++)
        {
            if (windowsList.at(windowZOrder.at(i))->getWindowType() == Normal)
            {
                bookmarksWinIndex = windowZOrder.at(i);
                OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(bookmarksWinIndex)->getCentralWidget());
                tabPage->openBookmarks();
                if (!tabPage->isActiveWindow())
                    postEvent(tabPage, new QEvent(QEvent::WindowActivate));
                break;
            }
            else if (i == (windowZOrder.count()-1))
            {
                openBrowserWindow();
                bookmarksWinIndex = windowsList.count() - 1;
                OTabPage *tabPage = static_cast<OTabPage*>(windowsList.last()->getCentralWidget());
                tabPage->openBookmarks();
                break;
            }
        }
    }
    else
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(bookmarksWinIndex)->getCentralWidget());
        QString bookmarkUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");

        for (int i = 0; i < tabPage->count(); i++)
        {
            QString pageUrl = tabPage->getStackPage()->widget(i)->findChild<OWebView*>()->url().toString();

            if (bookmarkUrl.compare(pageUrl) == 0)
            {
                tabPage->getTabBar()->setCurrentTab(i);
                if (!tabPage->isActiveWindow())
                    postEvent(tabPage, new QEvent(QEvent::WindowActivate));
                break;
            }
        }
    }
}

void OAppCore::openDownloads()
{
    if (!isDownloadsOpened())
    {
        setDownloadsOpened(true);
        for (int i = 0; i < windowZOrder.count(); i++)
        {
            if (windowsList.at(windowZOrder.at(i))->getWindowType() == Normal)
            {
                downloadsWinIndex = windowZOrder.at(i);
                OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(downloadsWinIndex)->getCentralWidget());
                tabPage->openDownloads();
                if (!tabPage->isActiveWindow())
                    postEvent(tabPage, new QEvent(QEvent::WindowActivate));
                break;
            }
            else if (i == (windowZOrder.count()-1))
            {
                openBrowserWindow();
                downloadsWinIndex = windowsList.count() - 1;
                OTabPage *tabPage = static_cast<OTabPage*>(windowsList.last()->getCentralWidget());
                tabPage->openDownloads();
                break;
            }
        }
    }
    else
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(downloadsWinIndex)->getCentralWidget());
        QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");

        for (int i = 0; i < tabPage->count(); i++)
        {
            QString pageUrl = tabPage->getStackPage()->widget(i)->findChild<OWebView*>()->url().toString();

            if (downloadsUrl.compare(pageUrl) == 0)
            {
                tabPage->getTabBar()->setCurrentTab(i);
                if (!tabPage->isActiveWindow())
                    postEvent(tabPage, new QEvent(QEvent::WindowActivate));
                break;
            }
        }
    }
}

void OAppCore::openSettings()
{
    if (!isSettingsOpened())
    {
        setSettingsOpened(true);
        for (int i = 0; i < windowZOrder.count(); i++)
        {
            if (windowsList.at(windowZOrder.at(i))->getWindowType() == Normal)
            {
                settingsWinIndex = windowZOrder.at(i);
                OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(settingsWinIndex)->getCentralWidget());
                tabPage->openSettings();
                if (!tabPage->isActiveWindow())
                    postEvent(tabPage, new QEvent(QEvent::WindowActivate));
                break;
            }
            else if (i == (windowZOrder.count()-1))
            {
                openBrowserWindow();
                settingsWinIndex = windowsList.count() - 1;
                OTabPage *tabPage = static_cast<OTabPage*>(windowsList.last()->getCentralWidget());
                tabPage->openSettings();
                break;
            }
        }
    }
    else
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(settingsWinIndex)->getCentralWidget());
        QString settingsUrl = QLatin1String("qrc:/web_applications/html/settingsView.html");

        for (int i = 0; i < tabPage->count(); i++)
        {
            QString pageUrl = tabPage->getStackPage()->widget(i)->findChild<OWebView*>()->url().toString();

            if (settingsUrl.compare(pageUrl) == 0)
            {
                tabPage->getTabBar()->setCurrentTab(i);
                if (!tabPage->isActiveWindow())
                    postEvent(tabPage, new QEvent(QEvent::WindowActivate));
                break;
            }
        }
    }
}

void OAppCore::historyClosed()
{
    setHistoryOpened(false);
    historyWinIndex = -1;
}

void OAppCore::bookmarksClosed()
{
    setBookmarksOpened(false);
    bookmarksWinIndex = -1;
}

void OAppCore::downloadsClosed()
{
    setDownloadsOpened(false);
    downloadsWinIndex = -1;
}

void OAppCore::settingsClosed()
{
    setSettingsOpened(false);
    settingsWinIndex = -1;
}

void OAppCore::setOlolanTheme(QString themeColor)
{
    ololanTheme = themeColor;
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        if (tabPage->getWindowType() == Normal)
            tabPage->setOlolanTheme(themeColor);
    }
}

void OAppCore::setBookmarkBarVisibile(bool state)
{
    bookmarkBarVisibile = state;
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->setBookmarkBarVisibile(state);
    }
}

void OAppCore::setWebPageZoom(int value)
{
    pageZoom = value;
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->setWebPageZoom(value);
    }
}

void OAppCore::enableGeolocation(bool value)
{
    geolocation = value;
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->enableGeolocation(value);
    }
}

void OAppCore::setPasswordSaving(bool value)
{
    passwordSaving = value;
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->setPasswordSaving(value);
    }
}

void OAppCore::setSiteAutoSignIn(bool value)
{
    siteAutoSignIn = value;
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->setSiteAutoSignIn(value);
    }
}

void OAppCore::setPaymentSaveAndFill(bool value)
{
    paymentSaveAndFill = value;
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->setPaymentSaveAndFill(value);
    }
}

void OAppCore::setDoNotTrack(bool value)
{
    doNotTrack = value;
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->setDoNotTrack(value);
    }
}

void OAppCore::setEnableAssistant(bool value)
{
    enableAssistant = value;
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->setEnableAssistant(value);
    }
}

void OAppCore::setAskDownloadPath(bool value)
{
    askDownloadPath = value;
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->setAskDownloadPath(value);
    }
}

void OAppCore::setSearchEngine(const QString searchEngine)
{
    ololanSearchEngine = searchEngine;
    startPageManager->setSearchEngineName(searchEngine);

    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->setSearchEngine(searchEngine);
    }
}

void OAppCore::openInWindow(const QString url)
{
    QString historyUrl = QLatin1String("qrc:/web_applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web_applications/html/settingsView.html");
    QString newTabUrl = QLatin1String("qrc:/web_applications/html/newtabView.html");
    QString newUrl = url;

    if (newUrl.compare("ololan://history") == 0)
        newUrl = historyUrl;

    if (newUrl.compare("ololan://bookmarks") == 0)
        newUrl = bookmarksUrl;

    if (newUrl.compare("ololan://downloads") == 0)
        newUrl = downloadsUrl;

    if (newUrl.compare("ololan://settings") == 0)
        newUrl = settingsUrl;

    if (newUrl.compare("ololan://home") == 0)
        newUrl = newTabUrl;

    openBrowserWindow();
    OTabPage *tabPage = static_cast<OTabPage*>(windowsList.last()->getCentralWidget());
    tabPage->getStackPage()->currentWidget()->findChild<OWebView*>()->loadPage(newUrl);
}

void OAppCore::openInPrivateWindow(const QString url)
{
    QString historyUrl = QLatin1String("qrc:/web_applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web_applications/html/settingsView.html");
    QString newTabUrl = QLatin1String("qrc:/web_applications/html/newtabView.html");
    QString newUrl = url;

    if (newUrl.compare("ololan://history") == 0)
        newUrl = historyUrl;

    if (newUrl.compare("ololan://bookmarks") == 0)
        newUrl = bookmarksUrl;

    if (newUrl.compare("ololan://downloads") == 0)
        newUrl = downloadsUrl;

    if (newUrl.compare("ololan://settings") == 0)
        newUrl = settingsUrl;

    if (newUrl.compare("ololan://home") == 0)
        newUrl = newTabUrl;

    openPrivateBrowserWindow();
    OTabPage *tabPage = static_cast<OTabPage*>(windowsList.last()->getCentralWidget());
    tabPage->getStackPage()->currentWidget()->findChild<OWebView*>()->loadPage(newUrl);
}

void OAppCore::synchronizeOlolan()
{
    OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(windowZOrder.first())->getCentralWidget());
    tabPage->synchronizeOlolan();
}

void OAppCore::disconnectOlolan()
{
    OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(windowZOrder.first())->getCentralWidget());
    tabPage->disconnectOlolan();
}

void OAppCore::deleteAccount()
{
    OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(windowZOrder.first())->getCentralWidget());
    tabPage->deleteAccount();
}

void OAppCore::setSyncActivated(bool value, const QString username, const QString password, const QString login)
{
    accountName = username;
    accountEmail = login;
    accountPassword = password;
    loggedIn = value;
    qDebug() << " OAppCore::setSyncActivated : " << username << " * " << password << " * " << login;
    settingsManager->updateSyncState(value, username, password, login);
}

void OAppCore::updateProfile(bool value, const QString username)
{
    qDebug() << " OAppCore::updateProfile : " << username;
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->updateSyncProfile(username, value);
    }
}

void OAppCore::setEnableMicrophone(bool state)
{
    enableMicrophone = state;
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->setEnableMicrophone(state);
    }
}

void OAppCore::setEnableCamera(bool state)
{
    enableCamera = state;
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->setEnableCamera(state);
    }
}

OTabPage *OAppCore::getActiveTabPage()
{
    return static_cast<OTabPage*>(windowsList.at(windowZOrder.first())->getCentralWidget());
}

void OAppCore::updateBookmarkBar(QAction *bookmarkItem)
{
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->updateBookmarkBar(bookmarkItem);
    }
}

void OAppCore::updateBookmarkBar(const QString index, const QString newTitle, const QString newUrl)
{
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->updateBookmarkBar(index, newTitle, newUrl);
    }
}

void OAppCore::deleteBookmarkBarItem(const QString bmkIndex)
{
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->deleteBookmarkBarItem(bmkIndex);
    }
}

OWebView *OAppCore::createNewWindow()
{
    openInWindow("");
    OTabPage *tabPage = static_cast<OTabPage*>(windowsList.last()->getCentralWidget());
    return tabPage->getStackPage()->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"));
}

void OAppCore::processWebQuery(const QString query)
{
    OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(windowZOrder.first())->getCentralWidget());
    OSmartInput *inputField = tabPage->getStackPage()->currentWidget()->findChild<CustomToolBar*>("controlBar")->getSmartBox();
    inputField->setText(query);
    inputField->validateInput();
}

const int OAppCore::getCurrentWindowIndex()
{
    return windowZOrder.first();
}

bool OAppCore::isLogged()
{
    return loggedIn;
}

const QString OAppCore::getAccountName()
{
    return accountName;
}

const QString OAppCore::getAccountEmail()
{
    return accountEmail;
}

const QString OAppCore::getAccountPassword()
{
    return accountPassword;
}

void OAppCore::setDownloadState(bool state)
{
    isDownloading = state;
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->setDownloadState(state);
    }
}

void OAppCore::warnForUpdates()
{
    for (int i = 0; i < windowsList.count(); i++)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(windowsList.at(i)->getCentralWidget());
        tabPage->warnForUpdates();
    }
}

void OAppCore::setWindowDPIChanged(HWND hWnd, bool changed)
{
    for (int i = 0; i < windowsList.count(); i++)
        if (windowsList.at(i)->getCustomWinHandle() == hWnd)
        {
            windowsList.at(i)->setDPIchanged(changed);
            break;
        }
}

bool OAppCore::getWindowDPIChanged(HWND hWnd)
{
    bool result = false;
    for (int i = 0; i < windowsList.count(); i++)
        if (windowsList.at(i)->getCustomWinHandle() == hWnd)
        {
            result = windowsList.at(i)->getDPIchanged();
            break;
        }
    return result;
}
