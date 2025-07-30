#include "otabpage.h"
#include "otabbar.h"
#include "oappcore.h"
#include "osmartinput.h"
#include "ofindinput.h"
#include "owebview.h"
#include "obrowsermenu.h"
#include "customwindow.h"
#include "ohistorycore.h"
#include "obookmarkscore.h"
#include "odownloadscore.h"
#include "odownloadwidget.h"
#include "osettingscore.h"
#include "customtoolbar.h"
#include "owebinspector.h"
#include "oinputcompleter.h"
#include "osyncwidget.h"
#include "oassistantcore.h"
#include "ostartpagecore.h"
#include "owebwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QMouseEvent>
#include <QWindow>
#include <QPropertyAnimation>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QFileDialog>
#include <QWebEngineDownloadRequest>
#include <QWindow>
#include <qpa/qplatformnativeinterface.h>
#include <QPrinter>
#include <QPrintDialog>
#include <QWebEngineCookieStore>
#include <QSplitter>
#include <QGroupBox>
#include <QUrl>
#include <QUrlQuery>
#include <QTimer>
#include <QStandardPaths>

#define QT_WA(unicode, ansi) unicode

OTabPage::OTabPage(CustomWindow *parentWind, WindowType type, QObject *parent, HWND hWnd)
    : QWidget(nullptr)
{
    if (parent)
        QObject::setParent(parent);

    initWidget(hWnd);
    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumSize(100, 100);

    zoomValues << 0.1 << 0.2 << 0.3 << 0.5 << 0.65 << 0.8 << 1.0 << 1.1 << 1.25 << 1.5 << 2.0 << 2.25 << 2.5 << 3.0 << 4.0 << 5.0;
    winType = type;
    parentWindowPtr = parentWind;
    isClosed = false;
    isDownloading = false;
    updatesAvailables = false;
    tabBar = new OTabBar(this);
    tabBar->setWinType(type);
    stackPage = new QStackedWidget(this);
    stackPage->setContentsMargins(0, 0, 0, 0);
    eina = new OWebWidget(this);
    eina->hide();

    connect(tabBar, &OTabBar::addButtonClicked, this, &OTabPage::openNewTab);
    connect(tabBar, &OTabBar::currentChanged, this, &OTabPage::setCurrentTabPage);
    connect(tabBar, &OTabBar::tabCloseRequested, this, &OTabPage::closeTabPage);
    connect(tabBar, &OTabBar::tabMoved, this, &OTabPage::moveTab);
    connect(tabBar, &OTabBar::closeButtonClicked, this, &OTabPage::close);
    connect(tabBar, &OTabBar::maxMinButtonClicked, this, &OTabPage::resizeWindow);
    connect(tabBar, &OTabBar::minimizeButtonClicked, this, &OTabPage::minimizeWindow);
    initActions();

    QHBoxLayout *HLayout = new QHBoxLayout();
    HLayout->setContentsMargins(0, 0, 0, 0);
    HLayout->setSpacing(0);
    HLayout->addWidget(stackPage);
    HLayout->addWidget(eina);

    QVBoxLayout *VLayout = new QVBoxLayout(this);
    VLayout->setContentsMargins(0, 0, 0, 0);
    VLayout->setSpacing(0);
    VLayout->addWidget(tabBar);
    VLayout->addLayout(HLayout);
    tabBar->setWinType(type);
    setLayout(VLayout);
}

void OTabPage::openNewTab()
{
    if (count() == -1)
        return;

    onTabAdding = true;
    setUpdatesEnabled(false);

    tabBar->addTab(QIcon(""), tr("New Tab"));
    stackPage->addWidget(setupPageWidget());

    int currentTab = count() - 1;
    tabBar->setCurrentTab(currentTab);

    if (winType == Normal)
    {
        stackPage->currentWidget()->findChild<OWebView*>()->load(QUrl("qrc:/web_applications/html/newtabView.html"));
        setOlolanTheme(ololanTheme);
    }
    else if (winType == Private)
    {
        stackPage->currentWidget()->findChild<OWebView*>()->load(QUrl("qrc:/web_applications/html/privatetabView.html"));
        setPrivateTheme();
    }

    //stackPage->currentWidget()->findChild<CustomToolBar*>()->getSmartBox()->setFocus();
    setUpdatesEnabled(true);
}

void OTabPage::grabTabPage()
{
    if (count() == -1)
        return;

    onTabAdding = true;
    setUpdatesEnabled(false);

    tabBar->grabTab(QIcon(""), tr("New Tab"));
    stackPage->addWidget(setupPageWidget());
    int currentTab = count() - 1;
    tabBar->setCurrentTab(currentTab);

    if (winType == Normal)
        setOlolanTheme(ololanTheme);
    else if (winType == Private)
        setPrivateTheme();

    setUpdatesEnabled(true);
}

void OTabPage::closeTabPage(int index)
{
    if (count() == -1)
        return;

    QWidget *ptr = processRemoving(index);
    setUpdatesEnabled(false);
    stackPage->removeWidget(ptr);
    tabBar->removeTab(index);
    setUpdatesEnabled(true);
    ptr->deleteLater();

    if ((index == 0) && (count() == 0))
        close();
}

void OTabPage::bruteRemoving(int index)
{
    if (count() == -1)
        return;

    QWidget *ptr = processRemoving(index);
    setUpdatesEnabled(false);
    stackPage->removeWidget(ptr);
    tabBar->bruteRemoveTab(index);
    setUpdatesEnabled(true);
    ptr->deleteLater();
}

QWidget* OTabPage::processRemoving(int &index)
{
    QWidget *ptr = stackPage->widget(index);
    QString pageUrl = ptr->findChild<OWebView*>()->url().toString();
    QString historyUrl = QLatin1String("qrc:/web_applications/html/historyView.html");
    QString bookmarkUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web_applications/html/settingsView.html");
    OAppCore *parentPtr = static_cast<OAppCore*>(parentWindowPtr->getParentApplication());

    if (historyUrl.compare(pageUrl) == 0)
        parentPtr->historyClosed();
    else if (bookmarkUrl.compare(pageUrl) == 0)
    {
        disconnect(ptr->findChild<OWebView*>(), &OWebView::loadFinished, parentPtr->getBookmarksManager(),
                   &OBookmarksCore::loadBookmarks);
        parentPtr->bookmarksClosed();
        parentPtr->getBookmarksManager()->setState(false);
    }
    else if (downloadsUrl.compare(pageUrl) == 0)
    {
        disconnect(ptr->findChild<OWebView*>(), &OWebView::loadFinished, parentPtr->getDownloadsManager(),
                   &ODownloadsCore::loadDownloadItem);
        parentPtr->downloadsClosed();
    }
    else if (settingsUrl.compare(pageUrl) == 0)
    {
        disconnect(ptr->findChild<OWebView*>(), &OWebView::loadFinished, parentPtr->getSettingsManager(),
                   &OSettingsCore::loadSettings);
        parentPtr->settingsClosed();
    }

    QString newTabUrl = QLatin1String("qrc:/web_applications/html/newtabView.html");
    QString privateTabUrl = QLatin1String("qrc:/web_applications/html/privatetabView.html");
    if ((pageUrl.compare(newTabUrl) != 0) && (pageUrl.compare(bookmarkUrl) != 0) && (pageUrl.compare(downloadsUrl) != 0) &&
        (pageUrl.compare(settingsUrl) != 0) && (pageUrl.compare(privateTabUrl) != 0) && (pageUrl.compare(historyUrl) != 0))
    {
        if (closedTabs->isEmpty())
        {
            OAppCore *parentPtr = static_cast<OAppCore*>(parentWindowPtr->getParentApplication());
            for (int i = 0; (parentPtr->getHistoryManager()->getHistoryItemsList().count() > i) && (i < 10); i++)
            {
                QLabel label = QLabel();
                QFontMetrics fontMetrics(label.fontMetrics());
                QAction *tabToClose = new QAction(fontMetrics.elidedText(parentPtr->getHistoryManager()->getHistoryItemsList().at(i)->getTitle(), Qt::ElideRight, 200));
                tabToClose->setData(QVariant(parentPtr->getHistoryManager()->getHistoryItemsList().at(i)->getUrl().toString()));

                if (parentPtr->getHistoryManager()->getHistoryItemsList().at(i)->getIcon() != "EmPtY64")
                {
                    QPixmap icon = QPixmap(16, 16);
                    icon.loadFromData(QByteArray::fromBase64(parentPtr->getHistoryManager()->getHistoryItemsList().at(i)->getIcon()), "PNG");
                    tabToClose->setIcon(icon);
                }
                else
                    tabToClose->setIcon(QIcon(":/images/webpage.png"));

                connect(tabToClose, &QAction::triggered, this, &OTabPage::openRecentTab);
                closedTabs->prepend(tabToClose);
            }
        }

        QLabel label = QLabel();
        QFontMetrics fontMetrics(label.fontMetrics());
        QAction *tabToClose = new QAction(fontMetrics.elidedText(ptr->findChild<OWebView*>()->title(), Qt::ElideRight, 200));
        tabToClose->setData(QVariant(pageUrl));
        tabToClose->setIcon(ptr->findChild<OWebView*>()->icon());
        connect(tabToClose, &QAction::triggered, this, &OTabPage::openRecentTab);
        closedTabs->prepend(tabToClose);

        while (closedTabs->count() > 10)
        {
            disconnect(closedTabs->last(), &QAction::triggered, this, &OTabPage::openRecentTab);
            closedTabs->removeLast();
        }
    }

    return ptr;
}

void OTabPage::openHistory()
{
    if (count() == -1)
        return;

    QString currentTabPageUrl = stackPage->currentWidget()->findChild<OWebView*>()->url().toString();
    if (currentTabPageUrl.compare(QLatin1String("qrc:/web_applications/html/newtabView.html")) == 0)
        stackPage->currentWidget()->findChild<OWebView*>()->load(QUrl("qrc:/web_applications/html/historyView.html"));
    else
    {
        setUpdatesEnabled(false);
        for (int i = 0; i < count(); i++)
        {
            QString pageUrl = stackPage->widget(i)->findChild<OWebView*>()->url().toString();
            if (pageUrl.compare(QLatin1String("qrc:/web_applications/html/newtabView.html")) == 0)
            {
                if (currentTabPage() != i)
                    tabBar->setCurrentTab(i);
                stackPage->widget(i)->findChild<OWebView*>()->load(QUrl("qrc:/web_applications/html/historyView.html"));
                break;
            }
            else if (i == (count()-1))
            {
                onTabAdding = true;
                tabBar->addTab(QIcon(""), tr("Ololan"));
                stackPage->addWidget(setupPageWidget());

                int currentTab = count() - 1;
                tabBar->setCurrentTab(currentTab);
                stackPage->currentWidget()->findChild<OWebView*>()->load(QUrl("qrc:/web_applications/html/historyView.html"));
                //stackPage->currentWidget()->findChild<CustomToolBar*>()->getSmartBox()->setFocus();

                if (winType == Normal)
                    setOlolanTheme(ololanTheme);
                else if (winType == Private)
                    setPrivateTheme();
                break;
            }
        }
        setUpdatesEnabled(true);
    }
}

void OTabPage::openBookmarks()
{
    if (count() == -1)
        return;

    OAppCore *app = static_cast<OAppCore*>(parentWindowPtr->getParentApplication());
    QString currentTabPageUrl = stackPage->currentWidget()->findChild<OWebView*>()->url().toString();
    if (currentTabPageUrl.compare(QLatin1String("qrc:/web_applications/html/newtabView.html")) == 0)
    {
        connect(stackPage->currentWidget()->findChild<OWebView*>(), &OWebView::loadFinished, app->getBookmarksManager(),
                &OBookmarksCore::loadBookmarks);
        stackPage->currentWidget()->findChild<OWebView*>()->load(QUrl("qrc:/web_applications/html/bookmarksView.html"));
    }
    else
    {
        setUpdatesEnabled(false);
        for (int i = 0; i < count(); i++)
        {
            QString pageUrl = stackPage->widget(i)->findChild<OWebView*>()->url().toString();
            if (pageUrl.compare(QLatin1String("qrc:/web_applications/html/newtabView.html")) == 0)
            {
                if (currentTabPage() != i)
                    tabBar->setCurrentTab(i);
                connect(stackPage->widget(i)->findChild<OWebView*>(), &OWebView::loadFinished, app->getBookmarksManager(),
                        &OBookmarksCore::loadBookmarks);
                stackPage->widget(i)->findChild<OWebView*>()->load(QUrl("qrc:/web_applications/html/bookmarksView.html"));
                break;
            }
            else if (i == (count()-1))
            {
                onTabAdding = true;
                tabBar->addTab(QIcon(""), tr("Ololan"));
                stackPage->addWidget(setupPageWidget());

                int currentTab = count() - 1;
                tabBar->setCurrentTab(currentTab);
                connect(stackPage->currentWidget()->findChild<OWebView*>(), &OWebView::loadFinished, app->getBookmarksManager(),
                        &OBookmarksCore::loadBookmarks);
                stackPage->currentWidget()->findChild<OWebView*>()->load(QUrl("qrc:/web_applications/html/bookmarksView.html"));
                //stackPage->currentWidget()->findChild<CustomToolBar*>()->getSmartBox()->setFocus();

                if (winType == Normal)
                    setOlolanTheme(ololanTheme);
                else if (winType == Private)
                    setPrivateTheme();
                break;
            }
        }
        setUpdatesEnabled(true);
    }
}

void OTabPage::openDownloads()
{
    if (count() == -1)
        return;

    OAppCore *app = static_cast<OAppCore*>(parentWindowPtr->getParentApplication());
    QString currentTabPageUrl = stackPage->currentWidget()->findChild<OWebView*>()->url().toString();
    if (currentTabPageUrl.compare(QLatin1String("qrc:/web_applications/html/newtabView.html")) == 0)
    {
        connect(stackPage->currentWidget()->findChild<OWebView*>(), &OWebView::loadFinished, app->getDownloadsManager(),
                &ODownloadsCore::loadDownloadItem);
        stackPage->currentWidget()->findChild<OWebView*>()->load(QUrl("qrc:/web_applications/html/downloadsView.html"));
    }
    else
    {
        setUpdatesEnabled(false);
        for (int i = 0; i < count(); i++)
        {
            QString pageUrl = stackPage->widget(i)->findChild<OWebView*>()->url().toString();
            if (pageUrl.compare(QLatin1String("qrc:/web_applications/html/newtabView.html")) == 0)
            {
                if (currentTabPage() != i)
                    tabBar->setCurrentTab(i);
                connect(stackPage->widget(i)->findChild<OWebView*>(), &OWebView::loadFinished, app->getDownloadsManager(),
                        &ODownloadsCore::loadDownloadItem);
                stackPage->widget(i)->findChild<OWebView*>()->load(QUrl("qrc:/web_applications/html/downloadsView.html"));
                break;
            }
            else if (i == (count()-1))
            {
                onTabAdding = true;
                tabBar->addTab(QIcon(""), tr("Ololan"));
                stackPage->addWidget(setupPageWidget());

                int currentTab = count() - 1;
                tabBar->setCurrentTab(currentTab);
                connect(stackPage->currentWidget()->findChild<OWebView*>(), &OWebView::loadFinished, app->getDownloadsManager(),
                        &ODownloadsCore::loadDownloadItem);
                stackPage->currentWidget()->findChild<OWebView*>()->load(QUrl("qrc:/web_applications/html/downloadsView.html"));
                //stackPage->currentWidget()->findChild<CustomToolBar*>()->getSmartBox()->setFocus();

                if (winType == Normal)
                    setOlolanTheme(ololanTheme);
                else if (winType == Private)
                    setPrivateTheme();
                break;
            }
        }
        setUpdatesEnabled(true);
    }
}

void OTabPage::openSettings()
{
    if (count() == -1)
        return;

    OAppCore *app = static_cast<OAppCore*>(parentWindowPtr->getParentApplication());
    QString currentTabPageUrl = stackPage->currentWidget()->findChild<OWebView*>()->url().toString();
    if (currentTabPageUrl.compare(QLatin1String("qrc:/web_applications/html/newtabView.html")) == 0)
    {
        connect(stackPage->currentWidget()->findChild<OWebView*>(), &OWebView::loadFinished, app->getSettingsManager(),
                &OSettingsCore::loadSettings);
        stackPage->currentWidget()->findChild<OWebView*>()->load(QUrl("qrc:/web_applications/html/settingsView.html"));
    }
    else
    {
        setUpdatesEnabled(false);
        for (int i = 0; i < count(); i++)
        {
            QString pageUrl = stackPage->widget(i)->findChild<OWebView*>()->url().toString();
            if (pageUrl.compare(QLatin1String("qrc:/web_applications/html/newtabView.html")) == 0)
            {
                if (currentTabPage() != i)
                    tabBar->setCurrentTab(i);
                connect(stackPage->widget(i)->findChild<OWebView*>(), &OWebView::loadFinished, app->getSettingsManager(),
                        &OSettingsCore::loadSettings);
                stackPage->widget(i)->findChild<OWebView*>()->load(QUrl("qrc:/web_applications/html/settingsView.html"));
                break;
            }
            else if (i == (count()-1))
            {
                onTabAdding = true;
                tabBar->addTab(QIcon(""), tr("Ololan"));
                stackPage->addWidget(setupPageWidget());

                int currentTab = count() - 1;
                tabBar->setCurrentTab(currentTab);
                connect(stackPage->currentWidget()->findChild<OWebView*>(), &OWebView::loadFinished, app->getSettingsManager(),
                        &OSettingsCore::loadSettings);
                stackPage->currentWidget()->findChild<OWebView*>()->load(QUrl("qrc:/web_applications/html/settingsView.html"));
                //stackPage->currentWidget()->findChild<CustomToolBar*>()->getSmartBox()->setFocus();

                if (winType == Normal)
                    setOlolanTheme(ololanTheme);
                else if (winType == Private)
                    setPrivateTheme();
                break;
            }
        }
        setUpdatesEnabled(true);
    }
}

void OTabPage::setCurrentTabPage(const int index)
{
    if(stackPage->currentIndex() != index)
        stackPage->setCurrentIndex(index);
}

void OTabPage::zoomIn()
{
    qreal zoom = stackPage->currentWidget()->findChild<OWebView*>()->getCurrentZoomFactor();
    for (int i = 0; i < zoomValues.count(); i++)
        if (zoom < zoomValues.at(i))
        {
            stackPage->currentWidget()->findChild<OWebView*>()->setCurrentZoomFactor(zoomValues.at(i));
            stackPage->currentWidget()->findChild<OWebView*>()->setZoomFactor(zoomValues.at(i));
            stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->getBrowserMenu()->setZoomValue(zoomValues.at(i));
            break;
        }
}

void OTabPage::zoomOut()
{
    qreal zoom = stackPage->currentWidget()->findChild<OWebView*>()->getCurrentZoomFactor();
    for (int i = (zoomValues.count()-1); i >= 0; i--)
        if (zoom > zoomValues.at(i))
        {
            stackPage->currentWidget()->findChild<OWebView*>()->setCurrentZoomFactor(zoomValues.at(i));
            stackPage->currentWidget()->findChild<OWebView*>()->setZoomFactor(zoomValues.at(i));
            stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->getBrowserMenu()->setZoomValue(zoomValues.at(i));
            break;
        }
}

void OTabPage::resetZoom()
{
    stackPage->currentWidget()->findChild<OWebView*>()->setCurrentZoomFactor(1.0);
    stackPage->currentWidget()->findChild<OWebView*>()->setZoomFactor(1.0);
    stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->getBrowserMenu()->setZoomValue(1.0);
}

QWidget* OTabPage::setupPageWidget()
{
    QWidget *page = new QWidget(stackPage);
    page->setStyleSheet(QLatin1String("OWidget {background-color: 1px solid #efeff1;}"));
    CustomToolBar *bar = new CustomToolBar(page);
    bar->setObjectName(QLatin1String("controlBar"));
    bar->setMovable(false);
    bar->setFloatable(false);
    bar->setFixedHeight(44);
    bar->setupControls(webSearchEngine, ololanAssistant);
    bar->getBrowserMenu()->setZoomValue(generalZoomFactor);
    bar->setDownloadState(isDownloading);


    CustomToolBar *bookmarkBar = new CustomToolBar(page);
    bookmarkBar->setControlBarState(true);
    bookmarkBar->setObjectName(QLatin1String("bookmarkBar"));
    bookmarkBar->setWinType(winType);
    bookmarkBar->setMovable(false);
    bookmarkBar->setFloatable(false);
    bookmarkBar->setFriendBar(bar);
    bookmarkBar->setContentsMargins(0, 2, 0, 2);
    bookmarkBar->setupMenu();

    if(winType == Private)
        bookmarkBar->setOlolanTheme("Private");
    else
        bookmarkBar->setOlolanTheme(ololanTheme);

    OAppCore *parentPtr = static_cast<OAppCore*>(parentWindowPtr->getParentApplication());
    bookmarkBar->addBookmarkItems(parentPtr->getBookmarksManager()->getBookmarkBarItemsList());

    if (bookmarBarVisible)
        bookmarkBar->setFixedHeight(32);
    else
    {
        bookmarkBar->setFixedHeight(1);
        bookmarkBar->hide();
    }

    int tabIndex = tabBar->count() - 1;
    if(tabIndex < 0)
        tabIndex = 0;

    OWebView *webView = new OWebView(page);
    webView->setObjectName(QLatin1String("pageView"));
    webView->setMinimumWidth(150);

    if (winType == Normal)
    {
        webView->setupWebPage(parentPtr->getSettingsManager()->getWebProfile());
        webView->setIsPrivateBrowsing(false);
    }
    else if (winType == Private)
    {
        webView->setupWebPage(parentPtr->getSettingsManager()->getPrivateWebProfile());
        webView->setIsPrivateBrowsing(true);
    }

    webView->setNextPageAction(bar->getNextButton());
    webView->setPreviousPageAction(bar->getBackButton());
    webView->setReloadPageAction(bar->getReloadButton());
    webView->setWebViewType(winType);
    webView->setDoNotTrack(doNotTrack);
    webView->enableGeolaction(geolocate);
    webView->setPaymentSaveAndFill(saveAndFillPayment);
    webView->setSiteAutoSignIn(websiteAutoSignIn);
    webView->setPasswordSaving(savePassword);
    webView->setOlolanTheme(ololanTheme);
    webView->setCurrentZoomFactor(generalZoomFactor);
    webView->setEnableCamera(enableCamera);
    webView->setEnableMicrophone(enableMicrophone);
    webView->setTabPage(this);

    QWidget *devPage = new QWidget(page);
    devPage->setObjectName(QLatin1String("devPage"));
    devPage->setVisible(false);

    OWebInspector *webInspector = new OWebInspector(devPage);
    webInspector->setObjectName(QLatin1String("devTools"));
    webInspector->setMinimumWidth(150);

    if (winType == Normal)
    {
        QWebEnginePage *viewPage = new QWebEnginePage(parentPtr->getSettingsManager()->getWebProfile());
        webInspector->setPage(viewPage);
    }
    else if (winType == Private)
    {
        QWebEnginePage *viewPage = new QWebEnginePage(parentPtr->getSettingsManager()->getPrivateWebProfile());
        webInspector->setPage(viewPage);;
    }

    QToolBar *devBar = new QToolBar(devPage);
    devBar->setStyleSheet(QLatin1String("QToolBar {background-color: #f9f9fb; border-bottom: 1px solid #efeff1;}"
                                        "QToolBar::separator {background-color: #f9f9fb; width: 4px;}"));
    devBar->setFixedHeight(32);
    devBar->setMovable(false);
    devBar->setFloatable(false);

    QToolButton *leftSide = new QToolButton(devBar);
    leftSide->setIcon(QIcon(":/images/dockedLeft.png"));
    leftSide->setObjectName("leftSide");
    leftSide->setCheckable(true);
    leftSide->setFixedSize(29, 28);
    leftSide->setToolTip("Align to the left");
    leftSide->setStyleSheet(QLatin1String("QToolButton {background-color: #f9f9fb; border: none; padding: 0px; border-radius: 3.5%;}"
                                          "QToolButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                          "QToolButton:checked {background-color: #dfdfe1; border: 2px solid #dfdfe1;}"));

    QToolButton *rightSide = new QToolButton(devBar);
    rightSide->setIcon(QIcon(":/images/dockedRight.png"));
    rightSide->setChecked(true);
    rightSide->setObjectName("rightSide");
    rightSide->setCheckable(true);
    rightSide->setToolTip("Align to the right");
    rightSide->setFixedSize(29, 28);
    rightSide->setStyleSheet(QLatin1String("QToolButton {background-color: #f9f9fb; border: none; padding: 0px; border-radius: 3.5%;}"
                                           "QToolButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                           "QToolButton:checked {background-color: #dfdfe1; border: 2px solid #dfdfe1;}"));

    QToolButton *bottomSide = new QToolButton(devBar);
    bottomSide->setIcon(QIcon(":/images/dockedBottom.png"));
    bottomSide->setObjectName("bottomSide");
    bottomSide->setCheckable(true);
    bottomSide->setToolTip("Align to the bottom");
    bottomSide->setFixedSize(29, 28);
    bottomSide->setStyleSheet(QLatin1String("QToolButton {background-color: #f9f9fb; border: none; padding: 0px; border-radius: 3.5%;}"
                                            "QToolButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                            "QToolButton:checked {background-color: #dfdfe1; border: 2px solid #dfdfe1;}"));

    QToolButton *closeDevTools = new QToolButton(devBar);
    closeDevTools->setIcon(QIcon(":/images/stop.png"));
    closeDevTools->setToolTip("Close developer tools");
    closeDevTools->setFixedSize(29, 28);
    closeDevTools->setStyleSheet(QLatin1String("QToolButton {background-color: #f9f9fb; border: none; padding: 0px; border-radius: 3.5%;}"
                                               "QToolButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                               "QToolButton:checked {background-color: #dfdfe1; border: 2px solid #dfdfe1;}"));

    connect(leftSide, &QToolButton::clicked, this, &OTabPage::alignDevToolLeft);
    connect(rightSide, &QToolButton::clicked, this, &OTabPage::alignDevToolRight);
    connect(bottomSide, &QToolButton::clicked, this, &OTabPage::alignDevToolBottom);
    connect(closeDevTools, &QToolButton::clicked, this, &OTabPage::webDev);

    devBar->addSeparator();
    devBar->addWidget(leftSide);
    devBar->addSeparator();
    devBar->addWidget(bottomSide);
    devBar->addSeparator();
    devBar->addWidget(rightSide);
    devBar->addSeparator();
    devBar->addWidget(closeDevTools);
    devBar->addSeparator();

    QVBoxLayout *devLayout = new QVBoxLayout(devPage);
    devLayout->setContentsMargins(0, 0, 0, 0);
    devLayout->setSpacing(0);
    devLayout->addWidget(devBar);
    devLayout->addWidget(webInspector);

    QSplitter *splitter = new QSplitter(page);
    splitter->setStyleSheet(QLatin1String("QSplitter::handle {background-color: #f9f9fb; border-left: 1px solid #efeff1;"
                                          "border-right: 1px solid #efeff1;}"));
    splitter->addWidget(webView);
    splitter->addWidget(devPage);
    splitter->setChildrenCollapsible(false);

    QVBoxLayout *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(0);
    pageLayout->addWidget(bar);
    pageLayout->addWidget(bookmarkBar);
    pageLayout->addWidget(splitter);

    OTab *tab = tabBar->getTabAt(tabIndex);
    setupConnection(bar, webView, tab);
    return page;
}

void OTabPage::initData(bool geolocation, bool passwordSaving, bool autoSignIn, bool paymentSaveAndFill, bool dNT, bool assistant,
                        bool askDwnPath, bool bmkBarVisibile, double pageZoom, QString theme, QString searchEngine, QList<QAction *> *closedTabList, bool camera, bool microphone, QString username)
{
    webSearchEngine = searchEngine;
    ololanTheme = theme;
    bookmarBarVisible = bmkBarVisibile;
    geolocate = geolocation;
    doNotTrack = dNT;
    ololanAssistant = assistant;
    askDownloadPath = askDwnPath;
    saveAndFillPayment = paymentSaveAndFill;
    savePassword = passwordSaving;
    websiteAutoSignIn = autoSignIn;
    generalZoomFactor = pageZoom;
    closedTabs = closedTabList;
    enableCamera = camera;
    enableMicrophone = microphone;
    accountName = username;
}

void OTabPage::setupConnection(CustomToolBar *toolBar, OWebView *webView, OTab *tab)
{
    OAppCore *parentPtr = static_cast<OAppCore*>(parentWindowPtr->getParentApplication());
    eina->setFixedWidth(parentPtr->primaryScreen()->size().width()*0.4);
    toolBar->getSmartBox()->setHistory(parentPtr->getHistoryManager());
    toolBar->getSmartBox()->setWebViewPage(static_cast<OWebPage*>(webView->page()));
    toolBar->getSmartBox()->getBookmarkWidget()->setBookmarkManager(parentPtr->getBookmarksManager());
    toolBar->getSmartBox()->getBookmarkWidget()->setTabPage(this);
    toolBar->getSyncWidget()->setTabPage(this);
    toolBar->getSyncWidget()->setOlolanTheme(ololanTheme);
    toolBar->getSyncWidget()->updateProfile(parentPtr->getAccountName(), parentPtr->isLogged());
    eina->setTabPage(this);
    eina->setupAssistant(parentPtr->getAssistantManager());
    toolBar->getSyncWidget()->setOlolanTheme(ololanTheme);
    toolBar->getSyncWidget()->updateProfile(parentPtr->getAccountName(), parentPtr->isLogged());

    connect(parentPtr->getBookmarksManager(), &OBookmarksCore::bookmarkDeleted, toolBar->getSmartBox(), &OSmartInput::uncheckDeletedBookmark);
    connect(toolBar->getSmartBox(), &OSmartInput::validated, webView, &OWebView::loadPage);
    connect(toolBar->getBackButton(), &QToolButton::clicked, webView, &OWebView::back);
    connect(toolBar->getNextButton(), &QToolButton::clicked, webView, &OWebView::forward);
    connect(toolBar->getReloadButton(), &QToolButton::clicked, webView, &OWebView::reloadOrStop);
    connect(toolBar->getBackButton(), &QToolButton::clicked, tabBar, &OTabBar::handleCompleter);
    connect(toolBar->getNextButton(), &QToolButton::clicked, tabBar, &OTabBar::handleCompleter);
    connect(toolBar->getReloadButton(), &QToolButton::clicked, tabBar, &OTabBar::handleCompleter);
    connect(toolBar->getUpdatesButton(), &QToolButton::clicked, this, &OTabPage::about);
    connect(webView, &OWebView::urlChanged, toolBar->getSmartBox(), &OSmartInput::updateUrl);
    connect(toolBar->getAssistantButton(), &QToolButton::clicked, this, &OTabPage::openEina);
    connect(webView, &OWebView::iconChanged, tab, &OTab::setTabIcon);
    connect(webView, &OWebView::titleChanged, tab, &OTab::setTabText);
    connect(webView, &OWebView::loadStarted, tab, &OTab::startLoadingAnimation);
    connect(webView, &OWebView::loadFinished, tab, &OTab::stopLoadingAnimation);
    connect(webView, &OWebView::loadProgress, tab, &OTab::restartLoadingAnimation);
    connect(webView, &OWebView::loadFinished, this, &OTabPage::pageReady);
    connect(webView, &OWebView::loadFinished, this, &OTabPage::manageBrowserApp);
    connect(webView, &OWebView::loadFinished, eina, &OWebWidget::setActiveProcessor);
    connect(webView, &OWebView::loadFinished, toolBar->getSmartBox(), &OSmartInput::resetHasCertificateError);
    connect(toolBar->getDownloadMenu(), &ODownloadWidget::aboutToShow, parentPtr->getDownloadsManager(), &ODownloadsCore::setDownloadWidget);
    connect(toolBar->getDownloadMenu(), &ODownloadWidget::aboutToHide, parentPtr->getDownloadsManager(), &ODownloadsCore::unsetDownloadWidget);
    connect(toolBar->getDownloadMenu(), &ODownloadWidget::openDownloadsManager, parentPtr, &OAppCore::openDownloads);
    connect(parentPtr->getSettingsManager()->getWebProfile(), &QWebEngineProfile::downloadRequested, this, &OTabPage::getDownload);
    connect(parentPtr->getSettingsManager()->getPrivateWebProfile(), &QWebEngineProfile::downloadRequested, this, &OTabPage::getDownload);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::fullscreenClicked, this, &OTabPage::showFullscreenMode);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::newTabClicked, this, &OTabPage::openNewTab);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::newWindowClicked, parentPtr, &OAppCore::openBrowserWindow);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::newPrivateWindowClicked, parentPtr, &OAppCore::openPrivateBrowserWindow);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::zoomInClicked, this, &OTabPage::zoomIn);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::zoomOutClicked, this, &OTabPage::zoomOut);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::resetZoomClicked, this, &OTabPage::resetZoom);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::historyClicked, parentPtr, &OAppCore::openHistory);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::downloadClicked, parentPtr, &OAppCore::openDownloads);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::bookmarkClicked, parentPtr, &OAppCore::openBookmarks);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::settingClicked, parentPtr, &OAppCore::openSettings);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::findTextClicked, this, &OTabPage::findText);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::savePageClicked, this, &OTabPage::savePage);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::pageCodeClicked, webView, &OWebView::showPageSource);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::webDevClicked, this, &OTabPage::webDev);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::helpCenterClicked, this, &OTabPage::helpCenter);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::reportClicked, this, &OTabPage::reportIssue);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::aboutClicked, this, &OTabPage::about);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::quitClicked, this, &OTabPage::quitBrowser);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::printClicked, this, &OTabPage::print);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::aboutToShow, this, &OTabPage::setupClosedTabsList);
    connect(toolBar->getBrowserMenu(), &OBrowserMenu::bookmarksBarRequested, parentPtr->getSettingsManager(), &OSettingsCore::manageBookmarkBar);
    connect(toolBar->getFindInput(), &OFindInput::textEdited, webView, &OWebView::findTypedText);
    connect(webView->page(), &QWebEnginePage::findTextFinished, toolBar, &CustomToolBar::updateFoundText);
    connect(webView->page(), &QWebEnginePage::certificateError, toolBar->getSmartBox(), &OSmartInput::certificateSecurity);
    connect(qobject_cast<OWebPage*>(webView->page()), &OWebPage::askFeaturePermission, toolBar->getSmartBox(), &OSmartInput::setPermissionInfo);

    if (winType == Normal)
    {
        connect(webView->page()->profile()->cookieStore(), &QWebEngineCookieStore::cookieAdded,
                parentPtr->getSettingsManager(), &OSettingsCore::removeGeneralCookie);
        connect(webView, &OWebView::loadFinished, parentPtr->getHistoryManager(), &OHistoryCore::pageLoadFinished);
        connect(webView, &OWebView::loadFinished, parentPtr->getStartPageManager(), &OStartPageCore::setupStartPage);
    }
    else if (winType == Private)
    {
        connect(webView->page()->profile()->cookieStore(), &QWebEngineCookieStore::cookieAdded,
                parentPtr->getSettingsManager(), &OSettingsCore::removeInPrivateCookie);
    }
}

WindowType OTabPage::getWindowType()
{
    return winType;
}

void OTabPage::resizeTabs(QResizeEvent *event)
{
    tabBar->updateSize(event);
}

void OTabPage::mousePressEvent(QMouseEvent *event)
{
    OSmartInput *smartInput = stackPage->currentWidget()->findChild<CustomToolBar*>()->getSmartBox();
    if (!smartInput->rect().contains(event->pos()) || !smartInput->getCompleter()->rect().contains(event->pos()))
    {
        if (smartInput->getCompleter()->isVisible())
            smartInput->getCompleter()->hide();
    }
    QWidget::mousePressEvent(event);
}

void OTabPage::handleMousePress(QMouseEvent *event)
{
    mousePressEvent(event);
}

void OTabPage::cut()
{
    //QMessageBox::information(nullptr, "OK", "it's work");
}

void OTabPage::copy()
{
    //QMessageBox::information(nullptr, "OK", "it's work");
}

void OTabPage::paste()
{
    //QMessageBox::information(nullptr, "OK", "it's work");
}

void OTabPage::unSetFullscreen()
{
    OWebView *view = static_cast<OWebView*>(qobject_cast<QAction*>(sender())->parent());
    disconnect(view, &OWebView::aboutToClose, this, &OTabPage::quitBrowser);
    stackPage->currentWidget()->findChild<OWebView*>()->setPage(view->page());
    view->close();
}

void OTabPage::showFullscreenMode()
{
    OWebView *fullscreenWindow = new OWebView();
    fullscreenWindow->setPage(stackPage->currentWidget()->findChild<OWebView*>()->page());

    QAction *exitFullscreen = new QAction(fullscreenWindow);
    exitFullscreen->setShortcut(QKeySequence(QLatin1String("F11")));
    connect(exitFullscreen, &QAction::triggered, this, &OTabPage::unSetFullscreen);
    connect(fullscreenWindow, &OWebView::aboutToClose, this, &OTabPage::quitBrowser);

    fullscreenWindow->addAction(exitFullscreen);
    fullscreenWindow->showFullScreen();
}

void OTabPage::reloadCurrentPage()
{
    stackPage->currentWidget()->findChild<OWebView*>()->reload();
}

void OTabPage::print()
{
    QPrinter *printer = new QPrinter(QPrinter::HighResolution);
    QPrintDialog *printDialog = new QPrintDialog(printer);

    if (printDialog->exec() == QDialog::Accepted)
    {
        OWebView *webView = stackPage->currentWidget()->findChild<OWebView*>();
        webView->page()->runJavaScript(QLatin1String("window.print();"));
        webView->print(printer);
    }
}

void OTabPage::webDev()
{
    QString pageUrl = stackPage->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"))->url().toString();
    QString historyUrl = QLatin1String("qrc:/web_applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web_applications/html/settingsView.html");
    QString newTabUrl = QLatin1String("qrc:/web_applications/html/newtabView.html");
    QString privateTabUrl = QLatin1String("qrc:/web_applications/html/privatetabView.html");

    QWidget *devPage = stackPage->currentWidget()->findChild<QWidget*>(QLatin1String("devPage"));
    OWebInspector *devTools = devPage->findChild<OWebInspector*>(QLatin1String("devTools"));
    QSplitter *splitter = stackPage->currentWidget()->findChild<QSplitter*>();

    if ((pageUrl.compare(newTabUrl) != 0) && (pageUrl.compare(bookmarksUrl) != 0) && (pageUrl.compare(downloadsUrl) != 0) &&
        (pageUrl.compare(settingsUrl) != 0) && (pageUrl.compare(historyUrl) != 0) && (pageUrl.compare(privateTabUrl) != 0))
    {
        if (!devPage->isVisible())
        {
            stackPage->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"))->page()->setDevToolsPage(devTools->page());
            devPage->setVisible(true);
            const int pageWidth = stackPage->currentWidget()->width();
            splitter->setSizes(QList<int>({(int)(pageWidth * 0.65), (int)(pageWidth * 0.35)}));

            if (splitter->orientation() == Qt::Orientation::Vertical)
                devPage->findChild<QToolButton*>(QLatin1String("bottomSide"))->setChecked(true);
            else if (splitter->layoutDirection() == Qt::LayoutDirection::LeftToRight)
                devPage->findChild<QToolButton*>(QLatin1String("rightSide"))->setChecked(true);
            else if (splitter->layoutDirection() == Qt::LayoutDirection::RightToLeft)
                devPage->findChild<QToolButton*>(QLatin1String("leftSide"))->setChecked(true);
        }
        else
        {
            devPage->setVisible(false);
            stackPage->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"))->page()->setDevToolsPage(nullptr);
        }
    }
}

void OTabPage::reportIssue()
{
    setUpdatesEnabled(false);
    for (int i = 0; i < count(); i++)
    {
        QString pageUrl = stackPage->widget(i)->findChild<OWebView*>()->url().toString();
        if (pageUrl.compare(QLatin1String("https://www.ololan.com/report")) == 0)
        {
            if (currentTabPage() != i)
                tabBar->setCurrentTab(i);
            stackPage->currentWidget()->findChild<OWebView*>()->setFocus();
            break;
        }
        else if (i == (count()-1))
        {
            for (int j = 0; j < count(); j++)
            {
                QString pageUrl = stackPage->widget(j)->findChild<OWebView*>()->url().toString();
                if ((pageUrl.compare(QLatin1String("qrc:/web_applications/html/newtabView.html")) == 0) ||
                    (pageUrl.compare(QLatin1String("qrc:/web_applications/html/privatetabView.html")) == 0))
                {
                    if (currentTabPage() != j)
                        tabBar->setCurrentTab(j);
                    stackPage->widget(j)->findChild<CustomToolBar*>("controlBar")->getSmartBox()->setText("https://www.ololan.com/report");
                    stackPage->widget(j)->findChild<CustomToolBar*>("controlBar")->getSmartBox()->validateInput();
                    stackPage->widget(j)->findChild<OWebView*>()->setFocus();
                    break;
                }
                else if (j == (count()-1))
                {
                    openNewTab();
                    stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->getSmartBox()->setText("https://www.ololan.com/report");
                    stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->getSmartBox()->validateInput();
                    stackPage->currentWidget()->findChild<OWebView*>()->setFocus();
                    i++;
                    j++;
                    break;
                }
            }
        }
    }
    setUpdatesEnabled(true);
}

void OTabPage::findText()
{
    stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->updateFindInputVisibility();
}

void OTabPage::findTextWith(const QString text)
{
    findText();
    stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->getFindInput()->setText(text);
    emit stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->getFindInput()->textEdited(text);
}

void OTabPage::quitBrowser()
{
    OAppCore *parentPtr = static_cast<OAppCore*>(parentWindowPtr->getParentApplication());
    parentPtr->exitFromBrowser();
}

void OTabPage::savePage()
{
    QWebEnginePage *webPage = stackPage->currentWidget()->findChild<OWebView*>()->page();
    QString path = QFileDialog::getSaveFileName(nullptr, QString("Save page"),
                                                webPage->profile()->downloadPath() + QString("/") + webPage->title() + QString(".html"),
                                                QLatin1String("Webpage, Complete (*.htm;*.html)"));
    if (!path.isEmpty())
        webPage->save(path, QWebEngineDownloadRequest::CompleteHtmlSaveFormat);
}

void OTabPage::about()
{
    OAppCore *parentPtr = static_cast<OAppCore*>(parentWindowPtr->getParentApplication());
    parentPtr->getSettingsManager()->setAboutRequested(true);

    if (parentPtr->isSettingsOpened())
    {
        parentPtr->openSettings();
        parentPtr->getSettingsManager()->selectAboutOlolan();
    }
    else
        parentPtr->openSettings();
}

void OTabPage::helpCenter()
{
    setUpdatesEnabled(false);
    for (int i = 0; i < count(); i++)
    {
        QString pageUrl = stackPage->widget(i)->findChild<OWebView*>()->url().toString();
        if (pageUrl.compare(QLatin1String("https://www.ololan.com/helps")) == 0)
        {
            if (currentTabPage() != i)
                tabBar->setCurrentTab(i);
            stackPage->currentWidget()->findChild<OWebView*>()->setFocus();
            break;
        }
        else if (i == (count()-1))
        {
            for (int j = 0; j < count(); j++)
            {
                QString pageUrl = stackPage->widget(j)->findChild<OWebView*>()->url().toString();
                if (pageUrl.compare(QLatin1String("qrc:/web_applications/html/newtabView.html")) == 0)
                {
                    if (currentTabPage() != j)
                        tabBar->setCurrentTab(j);
                    stackPage->widget(j)->findChild<CustomToolBar*>("controlBar")->getSmartBox()->setText("https://www.ololan.com/helps");
                    stackPage->widget(j)->findChild<CustomToolBar*>("controlBar")->getSmartBox()->validateInput();
                    stackPage->widget(j)->findChild<OWebView*>()->setFocus();
                    break;
                }
                else if (j == (count()-1))
                {
                    openNewTab();
                    stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->getSmartBox()->setText("https://www.ololan.com/helps");
                    stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->getSmartBox()->validateInput();
                    stackPage->currentWidget()->findChild<OWebView*>()->setFocus();
                    i++;
                    j++;
                    break;
                }
            }
        }
    }
    setUpdatesEnabled(true);
}

void OTabPage::showBookmarkBar()
{
    if (!bookmarBarVisible)
    {
        bookmarBarVisible = true;

        for (int i = 0; i < stackPage->count(); ++i)
        {
            CustomToolBar *bookmarkBar = stackPage->widget(i)->findChild<CustomToolBar*>(QLatin1String("bookmarkBar"));

            if (winType == Normal)
            {
                if (ololanTheme.compare("Light gray") == 0)
                    bookmarkBar->getFriendBar()->setLightGrayTheme(true);
                else if (ololanTheme.compare("Light turquoise") == 0)
                    bookmarkBar->getFriendBar()->setLightTurquoiseTheme(true);
                else if (ololanTheme.compare("Light brown") == 0)
                    bookmarkBar->getFriendBar()->setLightBrownTheme(true);
                else if (ololanTheme.compare("Dark gray") == 0)
                    bookmarkBar->getFriendBar()->setDarkGrayTheme(true);
                else if (ololanTheme.compare("Dark turquoise") == 0)
                    bookmarkBar->getFriendBar()->setDarkTurquoiseTheme(true);
                else if (ololanTheme.compare("Dark brown") == 0)
                    bookmarkBar->getFriendBar()->setDarkBrownTheme(true);
            }
            else if (winType == Private)
                bookmarkBar->getFriendBar()->setStyleSheet(QLatin1String("QToolBar {background-color: #5e4e6b; border-bottom: none;}"
                                                                         "QToolBar::separator {background-color: #5e4e6b; width: 4px;}"));

            if (currentTabPage() == i)
            {
                bookmarkBar->setVisible(true);
                bookmarkBar->startOnAnimation();
            }
            else
            {
                bookmarkBar->setVisible(true);
                bookmarkBar->setFixedHeight(32);
            }
        }
    }
    else
    {
        bookmarBarVisible = false;

        for (int i = 0; i < stackPage->count(); ++i)
        {
            CustomToolBar *bookmarkBar = stackPage->widget(i)->findChild<CustomToolBar*>(QLatin1String("bookmarkBar"));

            if (currentTabPage() == i)
                bookmarkBar->startOffAnimation();
            else
            {
                bookmarkBar->setFixedHeight(1);
                bookmarkBar->hide();

                if (winType == Normal)
                    bookmarkBar->setFriendBarStyle();
                else if (winType == Private)
                    bookmarkBar->getFriendBar()->setFriendBarStyle();
            }
        }
    }
}

QStackedWidget *OTabPage::getStackPage()
{
    return  stackPage;
}

OTabBar *OTabPage::getTabBar()
{
    return tabBar;
}

int OTabPage::count()
{
    return (tabBar->count() == stackPage->count() ? tabBar->count() : -1);
}

void OTabPage::tabDropEvent(QDropEvent *event)
{
    dropEvent(event);
}

void OTabPage::moveTab(const int from, const int to)
{
    QWidget *page = stackPage->widget(from);
    stackPage->setUpdatesEnabled(false);
    stackPage->removeWidget(page);
    stackPage->insertWidget(to, page);
    stackPage->setCurrentWidget(page);
    stackPage->setUpdatesEnabled(true);
}

void OTabPage::resizeEvent(QResizeEvent *event)
{
    event->accept();
    resizeTabs(event);
}

CustomWindow *OTabPage::getParentWindow()
{
    return parentWindowPtr;
}

void OTabPage::minimizeWindow()
{
    parentWindowPtr->showWindow(SW_SHOWMINIMIZED);
}

void OTabPage::resizeWindow()
{
    parentWindowPtr->resizeWindow();
}

void OTabPage::initWidget(HWND hWnd)
{
    Q_ASSERT(hWnd);

    if (hWnd)
    {
        QT_WA({SetWindowLong(reinterpret_cast<HWND>(winId()), GWL_STYLE, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);},
              {SetWindowLongA(reinterpret_cast<HWND>(winId()), GWL_STYLE, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);})

        setProperty("_q_embedded_native_parent_handle", reinterpret_cast<WId>(hWnd));
        HWND widgetHWnd = static_cast<HWND>(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("handle", windowHandle()));
        SetParent(widgetHWnd, hWnd);

        setWindowFlags(Qt::Widget);
        QEvent widgetEvent(QEvent::EmbeddingControl);
        QApplication::sendEvent(this, &widgetEvent);
    }
}

void OTabPage::closeEvent(QCloseEvent *event)
{
    event->accept();
    isClosed = true;

    while (stackPage->count() > 0)
        closeTabPage(stackPage->count() - 1);

    if (parentWindowPtr->getCanClose())
        parentWindowPtr->closeWindow();
}

bool OTabPage::getIsClosed()
{
    return isClosed;
}

void OTabPage::switchTab()
{
    QAction *origin = qobject_cast<QAction*>(sender());
    const QString actionName = origin->objectName();

    if ((actionName == QString("tabA1")) || (actionName == QString("tabA2")))
        tabBar->switchToTab(0);
    else if ((actionName == QString("tabB1")) || (actionName == QString("tabB2")))
        tabBar->switchToTab(1);
    else if ((actionName == QString("tabC1")) || (actionName == QString("tabC2")))
        tabBar->switchToTab(2);
    else if ((actionName == QString("tabD1")) || (actionName == QString("tabD2")))
        tabBar->switchToTab(3);
    else if ((actionName == QString("tabE1")) || (actionName == QString("tabE2")))
        tabBar->switchToTab(4);
    else if ((actionName == QString("tabF1")) || (actionName == QString("tabF2")))
        tabBar->switchToTab(5);
    else if ((actionName == QString("tabG1")) || (actionName == QString("tabG2")))
        tabBar->switchToTab(6);
    else if ((actionName == QString("tabH1")) || (actionName == QString("tabH2")))
        tabBar->switchToTab(7);
    else if ((actionName == QString("tabI1")) || (actionName == QString("tabI2")))
        tabBar->switchToTab(8);
}

void OTabPage::removeCurrentTab()
{
    if (currentTabPage() != -1)
        closeTabPage(currentTabPage());
}

int OTabPage::currentTabPage()
{
    if (tabBar->currentTab() == stackPage->currentIndex())
        return tabBar->currentTab();
    else
        return -1;
}

void OTabPage::initActions()
{
    QAction *tabA1 = new QAction(this);
    tabA1->setObjectName(QLatin1String("tabA1"));
    tabA1->setShortcut(QKeySequence(QLatin1String("Ctrl+&")));
    QAction *tabA2 = new QAction(this);
    tabA2->setObjectName(QLatin1String("tabA2"));
    tabA2->setShortcut(QKeySequence(QLatin1String("Ctrl+1")));
    QAction *tabB1 = new QAction(this);
    tabB1->setObjectName(QLatin1String("tabB1"));
    tabB1->setShortcut(QKeySequence(QString("Ctrl+é")));
    QAction *tabB2 = new QAction(this);
    tabB2->setObjectName(QLatin1String("tabB2"));
    tabB2->setShortcut(QKeySequence(QLatin1String("Ctrl+2")));
    QAction *tabC1 = new QAction(this);
    tabC1->setObjectName(QLatin1String("tabC1"));
    tabC1->setShortcut(QKeySequence(QLatin1String("Ctrl+\"")));
    QAction *tabC2 = new QAction(this);
    tabC2->setObjectName(QLatin1String("tabC2"));
    tabC2->setShortcut(QKeySequence(QLatin1String("Ctrl+3")));
    QAction *tabD1 = new QAction(this);
    tabD1->setObjectName(QLatin1String("tabD1"));
    tabD1->setShortcut(QKeySequence(QLatin1String("Ctrl+'")));
    QAction *tabD2 = new QAction(this);
    tabD2->setObjectName(QLatin1String("tabD2"));
    tabD2->setShortcut(QKeySequence(QLatin1String("Ctrl+4")));
    QAction *tabE1 = new QAction(this);
    tabE1->setObjectName(QLatin1String("tabE1"));
    tabE1->setShortcut(QKeySequence(QLatin1String("Ctrl+(")));
    QAction *tabE2 = new QAction(this);
    tabE2->setObjectName(QLatin1String("tabE2"));
    tabE2->setShortcut(QKeySequence(QLatin1String("Ctrl+5")));
    QAction *tabF1 = new QAction(this);
    tabF1->setObjectName(QLatin1String("tabF1"));
    tabF1->setShortcut(QKeySequence(QLatin1String("Ctrl+-")));
    QAction *tabF2 = new QAction(this);
    tabF2->setObjectName(QLatin1String("tabF2"));
    tabF2->setShortcut(QKeySequence(QLatin1String("Ctrl+6")));
    QAction *tabG1 = new QAction(this);
    tabG1->setObjectName(QLatin1String("tabG1"));
    tabG1->setShortcut(QKeySequence(QString("Ctrl+è")));
    QAction *tabG2 = new QAction(this);
    tabG2->setObjectName(QLatin1String("tabG2"));
    tabG2->setShortcut(QKeySequence(QLatin1String("Ctrl+7")));
    QAction *tabH1 = new QAction(this);
    tabH1->setObjectName(QLatin1String("tabH1"));
    tabH1->setShortcut(QKeySequence(QLatin1String("Ctrl+_")));
    QAction *tabH2 = new QAction(this);
    tabH2->setObjectName(QLatin1String("tabH2"));
    tabH2->setShortcut(QKeySequence(QLatin1String("Ctrl+8")));
    QAction *tabI1 = new QAction(this);
    tabI1->setObjectName(QLatin1String("tabI1"));
    tabI1->setShortcut(QKeySequence(QString("Ctrl+ç")));
    QAction *tabI2 = new QAction(this);
    tabI2->setObjectName(QLatin1String("tabI2"));
    tabI2->setShortcut(QKeySequence(QLatin1String("Ctrl+9")));
    QAction *closeCurrentTab = new QAction(this);
    closeCurrentTab->setShortcut(QKeySequence(QLatin1String("Ctrl+W")));
    QAction *reloadCurrentTab = new QAction(this);
    reloadCurrentTab->setShortcut(QKeySequence(QLatin1String("Ctrl+R")));

    connect(tabA1, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabA2, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabB1, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabB2, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabC1, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabC2, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabD1, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabD2, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabE1, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabE2, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabF1, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabF2, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabG1, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabG2, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabH1, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabH2, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabI1, &QAction::triggered, this, &OTabPage::switchTab);
    connect(tabI2, &QAction::triggered, this, &OTabPage::switchTab);
    connect(closeCurrentTab, &QAction::triggered, this, &OTabPage::removeCurrentTab);
    connect(reloadCurrentTab, &QAction::triggered, this, &OTabPage::reloadCurrentPage);

    addAction(tabA1);
    addAction(tabA2);
    addAction(tabB1);
    addAction(tabB2);
    addAction(tabC1);
    addAction(tabC2);
    addAction(tabD1);
    addAction(tabD2);
    addAction(tabE1);
    addAction(tabE2);
    addAction(tabF1);
    addAction(tabF2);
    addAction(tabG1);
    addAction(tabG2);
    addAction(tabH1);
    addAction(tabH2);
    addAction(tabI1);
    addAction(tabI2);
    addAction(closeCurrentTab);
    addAction(reloadCurrentTab);
}

bool OTabPage::event(QEvent *event)
{
    if (event->type() == QEvent::WindowActivate)
        static_cast<OAppCore*>(parentWindowPtr->getParentApplication())->raiseWindow(parentWindowPtr);
    return QWidget::event(event);
}

void OTabPage::setWebPageZoom(int value)
{
    for (int i = 0; i < count(); i++)
    {
        generalZoomFactor = (qreal)(value * 0.01);
        stackPage->widget(i)->findChild<OWebView*>(QLatin1String("pageView"))->setZoomFactor(generalZoomFactor);
        stackPage->widget(i)->findChild<OWebView*>(QLatin1String("pageView"))->setCurrentZoomFactor(generalZoomFactor);
    }
}

void OTabPage::manageBrowserApp(bool status)
{
    status = true;
    OWebView *webpage = qobject_cast<OWebView*>(sender());
    OAppCore *parentPtr = static_cast<OAppCore*>(parentWindowPtr->getParentApplication());
    QString historyUrl = QLatin1String("qrc:/web_applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web_applications/html/settingsView.html");
    QString pageUrl = webpage->url().toString();

    if (!parentPtr->isHistoryOpened() && (pageUrl.compare(historyUrl) == 0))
    {
        disconnect(webpage, &OWebView::loadFinished, this, &OTabPage::pageReady);
        parentPtr->setHistoryOpened(true);
        emit webpage->loadFinished(status);
        connect(webpage, &OWebView::loadFinished, this, &OTabPage::pageReady);
    }
    else if (!parentPtr->isBookmarksOpened() && (pageUrl.compare(bookmarksUrl) == 0))
    {
        disconnect(webpage, &OWebView::loadFinished, this, &OTabPage::pageReady);
        connect(webpage, &OWebView::loadFinished, parentPtr->getBookmarksManager(), &OBookmarksCore::loadBookmarks);
        parentPtr->setBookmarksOpened(true);
        emit webpage->loadFinished(status);
        connect(webpage, &OWebView::loadFinished, this, &OTabPage::pageReady);
    }
    else if (!parentPtr->isDownloadsOpened() && (pageUrl.compare(downloadsUrl) == 0))
    {
        disconnect(webpage, &OWebView::loadFinished, this, &OTabPage::pageReady);
        connect(webpage, &OWebView::loadFinished, parentPtr->getDownloadsManager(), &ODownloadsCore::loadDownloadItem);
        parentPtr->setDownloadsOpened(true);
        emit webpage->loadFinished(status);
        connect(webpage, &OWebView::loadFinished, this, &OTabPage::pageReady);
    }
    else if (!parentPtr->isSettingsOpened() && (pageUrl.compare(settingsUrl) == 0))
    {
        disconnect(webpage, &OWebView::loadFinished, this, &OTabPage::pageReady);
        connect(webpage, &OWebView::loadFinished, parentPtr->getSettingsManager(), &OSettingsCore::loadSettings);
        parentPtr->setSettingsOpened(true);
        emit webpage->loadFinished(status);
        connect(webpage, &OWebView::loadFinished, this, &OTabPage::pageReady);
    }
}

void OTabPage::pageReady(bool status)
{
    if (status && !updatesAvailables)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parentWindowPtr->getParentApplication());
        parentPtr->getSettingsManager()->checkForUpdates();
    }

    OWebView *webpage = qobject_cast<OWebView*>(sender());
    if (webpage->getCurrentZoomFactor() == generalZoomFactor)
        webpage->setZoomFactor(generalZoomFactor);
    else
        webpage->setZoomFactor(webpage->getCurrentZoomFactor());

    QString historyUrl = QLatin1String("qrc:/web_applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web_applications/html/settingsView.html");
    QString newTabUrl = QLatin1String("qrc:/web_applications/html/newtabView.html");
    QString privateTabUrl = QLatin1String("qrc:/web_applications/html/privatetabView.html");
    QString pageUrl = webpage->url().toString();

    if ((pageUrl.compare(newTabUrl) != 0) && (pageUrl.compare(bookmarksUrl) != 0) && (pageUrl.compare(downloadsUrl) != 0) &&
        (pageUrl.compare(settingsUrl) != 0) && (pageUrl.compare(historyUrl) != 0) && (pageUrl.compare("about:blank") != 0) &&
        (pageUrl.compare(privateTabUrl) != 0))
    {
        for (int i = 0; i < count(); i++)
            if (stackPage->widget(i)->findChild<OWebView*>(QLatin1String("pageView")) == webpage)
            {
                stackPage->widget(i)->findChild<CustomToolBar*>("controlBar")->getBrowserMenu()->enablePageActions();
                break;
            }
    }
    else
    {
        for (int i = 0; i < count(); i++)
            if (stackPage->widget(i)->findChild<OWebView*>(QLatin1String("pageView")) == webpage)
            {
                QWidget *devPage = stackPage->widget(i)->findChild<QWidget*>(QLatin1String("devPage"));
                if (devPage->isVisible())
                    devPage->hide();

                stackPage->widget(i)->findChild<OWebView*>(QLatin1String("pageView"))->page()->setDevToolsPage(nullptr);
                stackPage->widget(i)->findChild<CustomToolBar*>("controlBar")->getBrowserMenu()->disablePageActions();
                break;
            }
    }

    if (pageUrl.startsWith("", Qt::CaseInsensitive) && status)
    {
        QUrlQuery urlQuery(webpage->url());
        const QString username = urlQuery.queryItemValue("uname");
        const QString password = urlQuery.queryItemValue("upassword");
        const QString login = urlQuery.queryItemValue("ulogin");
        qDebug() << " pageready : " << username << " * " << password << " * " << login;
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parentWindowPtr->getParentApplication());
        parentPtr->setSyncActivated(true, username, password, login);
        parentPtr->updateProfile(true, username);
        webpage->loadPage("qrc:/web%20applications/html/newtabView.html");
    }
    else if (pageUrl.startsWith("", Qt::CaseInsensitive) && status)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parentWindowPtr->getParentApplication());
        parentPtr->setSyncActivated(false, QString("Guest User"), QString("guestpassword"), QString("guestuser@ololan.com"));
        parentPtr->updateProfile(false, QString("Guest User"));
        webpage->loadPage("https://accounts.ololan.com");
    }
}

void OTabPage::enableGeolocation(bool value)
{
    geolocate = value;
    for (int i = 0; i < count(); i++)
    {
        stackPage->widget(i)->findChild<OWebView*>()->enableGeolaction(value);
    }
}

void OTabPage::setEnableAssistant(bool value)
{
    ololanAssistant = value;
    for (int i = 0; i < count(); i++)
    {
        stackPage->widget(i)->findChild<CustomToolBar*>("controlBar")->setEnableAssistant(value);
    }
}

void OTabPage::setSearchEngine(const QString searchEngine)
{
    webSearchEngine = searchEngine;
    for (int i = 0; i < count(); i++)
    {
        stackPage->widget(i)->findChild<CustomToolBar*>("controlBar")->setSearchEngine(searchEngine);
    }
}

void OTabPage::setBookmarkBarVisibile(bool state)
{
    showBookmarkBar();
    if (bookmarBarVisible != state)
        bookmarBarVisible = state;
}

void OTabPage::setDoNotTrack(bool value)
{
    doNotTrack = value;
    for (int i = 0; i < count(); i++)
    {
        stackPage->widget(i)->findChild<OWebView*>()->setDoNotTrack(value);
    }
}

void OTabPage::setAskDownloadPath(bool value)
{
    askDownloadPath = value;
}

void OTabPage::getDownload(QWebEngineDownloadRequest *download)
{
    OAppCore *parentPtr = static_cast<OAppCore*>(parentWindowPtr->getParentApplication());
    QTimer *timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, &QTimer::timeout, this, [=, this]() {
        timer->stop();
        parentPtr->setDownloadState(true);
        stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->getDownloadButton()->click();
    });

    if (askDownloadPath)
    {
        QString path = QFileDialog::getExistingDirectory(nullptr, QString("Select location"),
                                                         parentPtr->getSettingsManager()->getWebProfile()->downloadPath(),
                                                         QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (!path.isEmpty())
        {
            timer->start();
            parentPtr->getSettingsManager()->getWebProfile()->setDownloadPath(path);
            parentPtr->getDownloadsManager()->acceptDownloadItem(download);
        }
    }
    else
    {
        timer->start();
        parentPtr->getDownloadsManager()->acceptDownloadItem(download);
    }
}

void OTabPage::setDownloadState(bool state)
{
    isDownloading = state;
    for (int i = 0; i < count(); i++)
        stackPage->widget(i)->findChild<CustomToolBar*>("controlBar")->setDownloadState(state);
}

void OTabPage::setPaymentSaveAndFill(bool value)
{
    saveAndFillPayment = value;
    /*for (int i = 0; i < count(); i++)
    {
        stackPage->widget(i)->findChild<OWebView*>()->setPaymentSaveAndFill(value);
    }*/
}

void OTabPage::setPasswordSaving(bool value)
{
    savePassword = value;
    for (int i = 0; i < count(); i++)
    {
        stackPage->widget(i)->findChild<OWebView*>()->setPasswordSaving(value);
    }
}

void OTabPage::setSiteAutoSignIn(bool value)
{
    websiteAutoSignIn = value;
    for (int i = 0; i < count(); i++)
    {
        stackPage->widget(i)->findChild<OWebView*>()->setSiteAutoSignIn(value);
    }
}

void OTabPage::setOlolanTheme(QString themeColor)
{
    ololanTheme = themeColor;
    if (themeColor.compare("Light gray") == 0)
        setLightGrayTheme();
    else if (themeColor.compare("Light turquoise") == 0)
        setLightTurquoiseTheme();
    else if (themeColor.compare("Light brown") == 0)
        setLightBrownTheme();
    else if (themeColor.compare("Dark gray") == 0)
        setDarkGrayTheme();
    else if (themeColor.compare("Dark turquoise") == 0)
        setDarkTurquoiseTheme();
    else if (themeColor.compare("Dark brown") == 0)
        setDarkBrownTheme();

    OAppCore *parentPtr = static_cast<OAppCore*>(parentWindowPtr->getParentApplication());

    if (parentPtr->isLogged())
    {
        QStringList idList = accountName.split(" ");
        QString firstname = idList.first();
        firstname = firstname.replace(firstname.at(0), firstname.at(0).toUpper());

        QString lastname = idList.last();
        lastname = lastname.replace(lastname.at(0), lastname.at(0).toUpper());

        QToolButton *button = qobject_cast<QToolButton*>(stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->getSyncWidget()->parent());
        button->setIcon(QIcon());
        button->setText(QString(firstname.at(0)) + QString(lastname.at(0)));
    }
}

void OTabPage::setLightGrayTheme()
{
    tabBar->setLightGrayTheme();
    eina->setLightGrayTheme();
    if (onTabAdding)
    {
        stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->setLightGrayTheme(bookmarBarVisible);
        stackPage->currentWidget()->findChild<CustomToolBar*>("bookmarkBar")->setBookmarkBarTheme("Light gray");
        stackPage->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"))->setOlolanTheme(ololanTheme);
        onTabAdding = false;
    }
    else
    {
        for (int i = 0; i < count(); i++)
        {
            stackPage->widget(i)->findChild<CustomToolBar*>("controlBar")->setLightGrayTheme(bookmarBarVisible);
            stackPage->widget(i)->findChild<CustomToolBar*>("bookmarkBar")->setBookmarkBarTheme("Light gray");
            stackPage->widget(i)->findChild<OWebView*>(QLatin1String("pageView"))->setOlolanTheme(ololanTheme);
        }
    }
}

void OTabPage::setLightTurquoiseTheme()
{
    tabBar->setLightTurquoiseTheme();
    eina->setLightTurquoiseTheme();
    if (onTabAdding)
    {
        stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->setLightTurquoiseTheme(bookmarBarVisible);
        stackPage->currentWidget()->findChild<CustomToolBar*>("bookmarkBar")->setBookmarkBarTheme("Light turquoise");
        stackPage->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"))->setOlolanTheme(ololanTheme);
        onTabAdding = false;
    }
    else
    {
        for (int i = 0; i < count(); i++)
        {
            stackPage->widget(i)->findChild<CustomToolBar*>("controlBar")->setLightTurquoiseTheme(bookmarBarVisible);
            stackPage->widget(i)->findChild<CustomToolBar*>("bookmarkBar")->setBookmarkBarTheme("Light turquoise");
            stackPage->widget(i)->findChild<OWebView*>(QLatin1String("pageView"))->setOlolanTheme(ololanTheme);
        }
    }
}

void OTabPage::setLightBrownTheme()
{
    tabBar->setLightBrownTheme();
    eina->setLightBrownTheme();
    if (onTabAdding)
    {
        stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->setLightBrownTheme(bookmarBarVisible);
        stackPage->currentWidget()->findChild<CustomToolBar*>("bookmarkBar")->setBookmarkBarTheme("Light brown");
        stackPage->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"))->setOlolanTheme(ololanTheme);
        onTabAdding = false;
    }
    else
    {
        for (int i = 0; i < count(); i++)
        {
            stackPage->widget(i)->findChild<CustomToolBar*>("controlBar")->setLightBrownTheme(bookmarBarVisible);
            stackPage->widget(i)->findChild<CustomToolBar*>("bookmarkBar")->setBookmarkBarTheme("Light brown");
            stackPage->widget(i)->findChild<OWebView*>(QLatin1String("pageView"))->setOlolanTheme(ololanTheme);
        }
    }
}

void OTabPage::setDarkGrayTheme()
{
    tabBar->setDarkGrayTheme();
    eina->setDarkGrayTheme();
    if (onTabAdding)
    {
        stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->setDarkGrayTheme(bookmarBarVisible);
        stackPage->currentWidget()->findChild<CustomToolBar*>("bookmarkBar")->setBookmarkBarTheme("Dark gray");
        stackPage->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"))->setOlolanTheme(ololanTheme);
        onTabAdding = false;
    }
    else
    {
        for (int i = 0; i < count(); i++)
        {
            stackPage->widget(i)->findChild<CustomToolBar*>("controlBar")->setDarkGrayTheme(bookmarBarVisible);
            stackPage->widget(i)->findChild<CustomToolBar*>("bookmarkBar")->setBookmarkBarTheme("Dark gray");
            stackPage->widget(i)->findChild<OWebView*>(QLatin1String("pageView"))->setOlolanTheme(ololanTheme);
        }
    }
}

void OTabPage::setDarkTurquoiseTheme()
{
    tabBar->setDarkTurquoiseTheme();
    eina->setDarkTurquoiseTheme();
    if (onTabAdding)
    {
        stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->setDarkTurquoiseTheme(bookmarBarVisible);
        stackPage->currentWidget()->findChild<CustomToolBar*>("bookmarkBar")->setBookmarkBarTheme("Dark turquoise");
        stackPage->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"))->setOlolanTheme(ololanTheme);
        onTabAdding = false;
    }
    else
    {
        for (int i = 0; i < count(); i++)
        {
            stackPage->widget(i)->findChild<CustomToolBar*>("controlBar")->setDarkTurquoiseTheme(bookmarBarVisible);
            stackPage->widget(i)->findChild<CustomToolBar*>("bookmarkBar")->setBookmarkBarTheme("Dark turquoise");
            stackPage->widget(i)->findChild<OWebView*>(QLatin1String("pageView"))->setOlolanTheme(ololanTheme);
        }
    }
}

void OTabPage::setDarkBrownTheme()
{
    tabBar->setDarkBrownTheme();
    eina->setDarkBrownTheme();
    if (onTabAdding)
    {
        stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->setDarkBrownTheme(bookmarBarVisible);
        stackPage->currentWidget()->findChild<CustomToolBar*>("bookmarkBar")->setBookmarkBarTheme("Dark brown");
        stackPage->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"))->setOlolanTheme(ololanTheme);
        onTabAdding = false;
    }
    else
    {
        for (int i = 0; i < count(); i++)
        {
            stackPage->widget(i)->findChild<CustomToolBar*>("controlBar")->setDarkBrownTheme(bookmarBarVisible);
            stackPage->widget(i)->findChild<CustomToolBar*>("bookmarkBar")->setBookmarkBarTheme("Dark brown");
            stackPage->widget(i)->findChild<OWebView*>(QLatin1String("pageView"))->setOlolanTheme(ololanTheme);
        }
    }
}

void OTabPage::setPrivateTheme()
{
    tabBar->setPrivateTheme();
    eina->setPrivateTheme();
    if (onTabAdding)
    {
        stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->setPrivateTheme(bookmarBarVisible);
        stackPage->currentWidget()->findChild<CustomToolBar*>("bookmarkBar")->setBookmarkBarTheme("Private");
        stackPage->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"))->setOlolanTheme(ololanTheme);
        onTabAdding = false;
    }
    else
    {
        for (int i = 0; i < count(); i++)
        {
            stackPage->widget(i)->findChild<CustomToolBar*>("controlBar")->setPrivateTheme(bookmarBarVisible);
            stackPage->widget(i)->findChild<CustomToolBar*>("bookmarkBar")->setBookmarkBarTheme("Private");
            stackPage->widget(i)->findChild<OWebView*>(QLatin1String("pageView"))->setOlolanTheme(ololanTheme);
        }
    }
}

bool OTabPage::isBookmarkBarVisible()
{
    return bookmarBarVisible;
}

void OTabPage::setupClosedTabsList()
{
    OBrowserMenu *menu = static_cast<OBrowserMenu*>(sender());
    menu->clearHistoryList();

    if (closedTabs->isEmpty())
    {
        OAppCore *parentPtr = static_cast<OAppCore*>(parentWindowPtr->getParentApplication());
        for (int i = 0; (parentPtr->getHistoryManager()->getHistoryItemsList().count() > i) && (i < 10); i++)
        {
            QLabel label = QLabel();
            QFontMetrics fontMetrics(label.fontMetrics());
            QAction *tabToClose = new QAction(fontMetrics.elidedText(parentPtr->getHistoryManager()->getHistoryItemsList().at(i)->getTitle(), Qt::ElideRight, 200));
            tabToClose->setData(QVariant(parentPtr->getHistoryManager()->getHistoryItemsList().at(i)->getUrl().toString()));

            if (parentPtr->getHistoryManager()->getHistoryItemsList().at(i)->getIcon() != "EmPtY64")
            {
                QPixmap icon = QPixmap(16, 16);
                icon.loadFromData(QByteArray::fromBase64(parentPtr->getHistoryManager()->getHistoryItemsList().at(i)->getIcon()), "PNG");
                tabToClose->setIcon(icon);
            }
            else
                tabToClose->setIcon(QIcon(":/images/webpage.png"));

            connect(tabToClose, &QAction::triggered, this, &OTabPage::openRecentTab);
            menu->loadRecentTab(tabToClose);
            closedTabs->append(tabToClose);
        }
    }
    else
    {
        for (int i = 0; i < closedTabs->count(); i++)
            menu->loadRecentTab(closedTabs->at(i));
    }
}

void OTabPage::openRecentTab()
{
    QAction *tab = static_cast<QAction*>(sender());
    disconnect(tab, &QAction::triggered, this, &OTabPage::openRecentTab);
    openNewTab();
    stackPage->currentWidget()->findChild<OWebView*>()->loadPage(tab->data().toString());
    closedTabs->removeOne(tab);
}

bool  OTabPage::openLastCloseTab()
{
    QAction *tab = closedTabs->last();
    if (tab->isEnabled())
    {
        disconnect(tab, &QAction::triggered, this, &OTabPage::openRecentTab);
        openNewTab();
        stackPage->currentWidget()->findChild<OWebView*>()->loadPage(tab->data().toString());
        closedTabs->removeOne(tab);
        return true;
    }
    else
        return false;
}

bool OTabPage::openFirstCloseTab()
{
    QAction *tab = closedTabs->first();
    if (tab->isEnabled())
    {
        disconnect(tab, &QAction::triggered, this, &OTabPage::openRecentTab);
        openNewTab();
        stackPage->currentWidget()->findChild<OWebView*>()->loadPage(tab->data().toString());
        closedTabs->removeOne(tab);
        return true;
    }
    else
        return false;
}

void OTabPage::synchronizeOlolan()
{
    OAppCore *parentPtr = qobject_cast<OAppCore*>(parentWindowPtr->getParentApplication());
    if (!parentPtr->isLogged())
    {
        openNewTab();
        stackPage->currentWidget()->findChild<OWebView*>()->loadPage(QString(""));
    }
    else
        disconnectOlolan();
}

void OTabPage::disconnectOlolan()
{
    OAppCore *parentPtr = qobject_cast<OAppCore*>(parentWindowPtr->getParentApplication());
    if (parentPtr->isLogged())
    {
        QString uname = parentPtr->getAccountName();
        QString upassword = parentPtr->getAccountPassword();
        QString ulogin = parentPtr->getAccountEmail();
        QStringList idList = uname.split(" ");
        QString uloginID = (QString(idList.first().at(0).toUpper()) + QString(idList.last().at(0).toUpper()));

        openNewTab();
        stackPage->currentWidget()->findChild<OWebView*>()->loadPage(QString());
    }
}

void OTabPage::deleteAccount()
{
    OAppCore *parentPtr = qobject_cast<OAppCore*>(parentWindowPtr->getParentApplication());
    if (parentPtr->isLogged())
    {
        QString uname = parentPtr->getAccountName();
        QString upassword = parentPtr->getAccountPassword();
        QString ulogin = parentPtr->getAccountEmail();
        QStringList idList = uname.split(" ");
        QString uloginID = (QString(idList.first().at(0).toUpper()) + QString(idList.last().at(0).toUpper()));

        openNewTab();
        stackPage->currentWidget()->findChild<OWebView*>()->loadPage(QString());
    }
}

void OTabPage::updateSyncProfile(const QString username, bool isLoggedIn)
{
    qDebug() << " OTabPage::updateSyncProfile : " << username;
    for (int i = 0; i < count(); i++)
        stackPage->widget(i)->findChild<CustomToolBar*>("controlBar")->getSyncWidget()->updateProfile(username, isLoggedIn);
}

void OTabPage::openInTab(const QString url)
{
    if (count() == -1)
        return;

    QString newUrl = url;
    setUpdatesEnabled(false);
    OAppCore *parentPtr = qobject_cast<OAppCore*>(parentWindowPtr->getParentApplication());

    if (newUrl.compare("ololan://history") == 0)
        parentPtr->openHistory();
    else if (newUrl.compare("ololan://bookmarks") == 0)
        parentPtr->openBookmarks();
    else if (newUrl.compare("ololan://downloads") == 0)
        parentPtr->openDownloads();
    else if (newUrl.compare("ololan://settings") == 0)
        parentPtr->openSettings();
    else if (newUrl.compare("ololan://home") == 0)
        openNewTab();
    else
    {
        tabBar->addTab(QIcon(""), tr("New Tab"));
        stackPage->addWidget(setupPageWidget());

        if (!newUrl.isEmpty())
            stackPage->widget(stackPage->count()-1)->findChild<OWebView*>()->loadPage(newUrl);

        if (winType == Normal)
            setOlolanTheme(ololanTheme);
        else if (winType == Private)
            setPrivateTheme();
    }

    setUpdatesEnabled(true);
}

void OTabPage::setEnableMicrophone(bool state)
{
    enableMicrophone = state;
    for (int i = 0; i < count(); i++)
    {
        stackPage->widget(i)->findChild<OWebView*>()->setEnableMicrophone(state);
    }
}

void OTabPage::setEnableCamera(bool state)
{
    enableCamera = state;
    for (int i = 0; i < count(); i++)
    {
        stackPage->widget(i)->findChild<OWebView*>()->setEnableCamera(state);
    }
}

void OTabPage::updateBookmarkBar(QAction *bookmarkItem)
{
    for (int i = 0; i < count(); i++)
    {
        stackPage->widget(i)->findChild<CustomToolBar*>("bookmarkBar")->addBookmark(bookmarkItem);
    }
}

void OTabPage::updateBookmarkBar(const QString index, const QString newTitle, const QString newUrl)
{
    for (int i = 0; i < count(); i++)
    {
        stackPage->widget(i)->findChild<CustomToolBar*>("bookmarkBar")->updateItem(index, newTitle, newUrl);
    }
}

void OTabPage::deleteBookmarkBarItem(const QString bmkIndex)
{
    for (int i = 0; i < count(); i++)
    {
        stackPage->widget(i)->findChild<CustomToolBar*>("bookmarkBar")->deleteItem(bmkIndex);
    }
}

void OTabPage::editBookmark(const QString index)
{
    OAppCore *parentPtr = static_cast<OAppCore*>(parentWindowPtr->getParentApplication());
    parentPtr->getBookmarksManager()->setEditingItem(index, true);
    openBookmarks();
}

OWebView *OTabPage::createNewTab()
{
    openInTab("");
    int currentTab = count() - 1;
    tabBar->setCurrentTab(currentTab);
    return stackPage->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"));
}

OWebView *OTabPage::createBackgroundTab()
{
    openInTab("");
    return stackPage->widget(count()-1)->findChild<OWebView*>(QLatin1String("pageView"));
}

void OTabPage::alignDevToolLeft()
{
    QToolButton *toolButton = qobject_cast<QToolButton*>(sender());
    toolButton->setChecked(true);

    QWidget *devPage = stackPage->currentWidget()->findChild<QWidget*>(QLatin1String("devPage"));
    devPage->findChild<QToolButton*>(QLatin1String("rightSide"))->setChecked(false);
    devPage->findChild<QToolButton*>(QLatin1String("bottomSide"))->setChecked(false);

    QToolBar *devBar = stackPage->currentWidget()->findChild<QWidget*>(QLatin1String("devPage"))->findChild<QToolBar*>();
    devBar->setStyleSheet(QLatin1String("QToolBar {background-color: #f9f9fb; border-bottom: 1px solid #efeff1;}"
                                        "QToolBar::separator {background-color: #f9f9fb; width: 4px;}"));

    QSplitter *splitter = stackPage->currentWidget()->findChild<QSplitter*>();
    splitter->setOrientation(Qt::Orientation::Horizontal);
    splitter->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
}

void OTabPage::alignDevToolRight()
{
    QToolButton *toolButton = qobject_cast<QToolButton*>(sender());
    toolButton->setChecked(true);

    QWidget *devPage = stackPage->currentWidget()->findChild<QWidget*>(QLatin1String("devPage"));
    devPage->findChild<QToolButton*>(QLatin1String("leftSide"))->setChecked(false);
    devPage->findChild<QToolButton*>(QLatin1String("bottomSide"))->setChecked(false);

    QToolBar *devBar = stackPage->currentWidget()->findChild<QWidget*>(QLatin1String("devPage"))->findChild<QToolBar*>();
    devBar->setStyleSheet(QLatin1String("QToolBar {background-color: #f9f9fb; border-bottom: 1px solid #efeff1;}"
                                        "QToolBar::separator {background-color: #f9f9fb; width: 4px;}"));

    QSplitter *splitter = stackPage->currentWidget()->findChild<QSplitter*>();
    splitter->setOrientation(Qt::Orientation::Horizontal);
    splitter->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
}

void OTabPage::alignDevToolBottom()
{
    QToolButton *toolButton = qobject_cast<QToolButton*>(sender());
    toolButton->setChecked(true);

    QWidget *devPage = stackPage->currentWidget()->findChild<QWidget*>(QLatin1String("devPage"));
    devPage->findChild<QToolButton*>(QLatin1String("leftSide"))->setChecked(false);
    devPage->findChild<QToolButton*>(QLatin1String("rightSide"))->setChecked(false);

    QToolBar *devBar = stackPage->currentWidget()->findChild<QWidget*>(QLatin1String("devPage"))->findChild<QToolBar*>();
    devBar->setStyleSheet(QLatin1String("QToolBar {background-color: #f9f9fb; border-bottom: 1px solid #efeff1;"
                                        "border-top: 1px solid #efeff1;}"
                                        "QToolBar::separator {background-color: #f9f9fb; width: 4px;}"));

    QSplitter *splitter = stackPage->currentWidget()->findChild<QSplitter*>();
    splitter->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
    splitter->setOrientation(Qt::Orientation::Vertical);
}

void OTabPage::connectAI()
{
    OAppCore *parentPtr = static_cast<OAppCore*>(parentWindowPtr->getParentApplication());
    parentPtr->getAssistantManager()->setTabPage(this);
}

void OTabPage::searchTheWebFor(const QString item)
{
    stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->getSmartBox()->setText(item);
    stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->getSmartBox()->validateInput();
}

void OTabPage::visitPage(const QString page)
{
    QUrl url = QUrl::fromUserInput(page);
    stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->getSmartBox()->setText(url.toEncoded());
    stackPage->currentWidget()->findChild<CustomToolBar*>("controlBar")->getSmartBox()->validateInput();
}

void OTabPage::warnForUpdates()
{
    updatesAvailables = true;
    for (int i = 0; i < count(); i++)
    {
        stackPage->widget(i)->findChild<CustomToolBar*>("controlBar")->warnForUpdates();
    }
}

void OTabPage::openEina()
{
    QToolButton *toggle = qobject_cast<QToolButton*>(sender());
    if (toggle->isChecked())
    {
        connectAI();
        eina->show();
    }
    else
    {
        eina->beforeHiding();
        eina->hide();
    }
}
