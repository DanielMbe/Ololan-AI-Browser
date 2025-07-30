#include "owebview.h"
#include "otabpage.h"
#include "customwindow.h"
#include "oappcore.h"
#include <QAction>
#include <QCloseEvent>
#include <QWebEngineHttpRequest>
#include <QLabel>
#include <QMimeData>
#include <QToolTip>
#include <QClipboard>

OWebView::OWebView(QWidget *parent)
    :QWebEngineView(parent)
{
    privateBrowsing = false;
    doNotTrack = true;
    currentZoom = 1.0;
    webViewMenu = new QMenu(this);
    webViewMenu->setFixedWidth(300);

    connect(this, &OWebView::loadFinished, this, &OWebView::changeBrowsingButtonState);
    connect(this, &OWebView::loadStarted, this, &OWebView::reloadButtonOn);
    setContentsMargins(0, 0, 0, 0);
    setToolTipDuration(15000);
}

void OWebView::setupWebPage(QWebEngineProfile *profile)
{
    webPage = new OWebPage(profile, this);
    setPage(webPage);
    connect(webPage, &QWebEnginePage::linkHovered, this, &OWebView::onLinkHovered);
}

void OWebView::loadPage(const QString url)
{
    QUrl pageUrl = QUrl::fromUserInput(url);
    QWebEngineHttpRequest request = QWebEngineHttpRequest(pageUrl, QWebEngineHttpRequest::Get);

    if (doNotTrack)
        request.setHeader(QByteArray("DNT"), QByteArray("1"));
    load(request);
}

void OWebView::setNextPageAction(QToolButton *action)
{
    nextPage = action;
}

void OWebView::setPreviousPageAction(QToolButton *action)
{
    previousPage = action;
}

void OWebView::setReloadPageAction(QToolButton *action)
{
    reloadPage = action;
}

void OWebView::changeBrowsingButtonState()
{
    if (page()->action(QWebEnginePage::Back)->isEnabled() && !previousPage->isEnabled())
        previousPage->setEnabled(true);
    else if (!page()->action(QWebEnginePage::Back)->isEnabled() && previousPage->isEnabled())
        previousPage->setEnabled(false);

    if (page()->action(QWebEnginePage::Forward)->isEnabled() && !nextPage->isEnabled())
        nextPage->setEnabled(true);
    else if (!page()->action(QWebEnginePage::Forward)->isEnabled() && nextPage->isEnabled())
        nextPage->setEnabled(false);

    reloadButtonOff();
}

void OWebView::reloadButtonOn()
{
    if (winType == Normal)
    {
        if (ololanTheme.compare("Light gray") == 0)
            reloadPage->setIcon(QIcon(":/images/stop.png"));
        else if (ololanTheme.compare("Light turquoise") == 0)
            reloadPage->setIcon(QIcon(":/images/stop.png"));
        else if (ololanTheme.compare("Light brown") == 0)
            reloadPage->setIcon(QIcon(":/images/stop.png"));
        else if (ololanTheme.compare("Dark gray") == 0)
            reloadPage->setIcon(QIcon(":/private_mode_images/stopPrivateMode.png"));
        else if (ololanTheme.compare("Dark turquoise") == 0)
            reloadPage->setIcon(QIcon(":/private_mode_images/stopPrivateMode.png"));
        else if (ololanTheme.compare("Dark brown") == 0)
            reloadPage->setIcon(QIcon(":/private_mode_images/stopPrivateMode.png"));
    }
    else if (winType == Private)
        reloadPage->setIcon(QIcon(":/private_mode_images/stopPrivateMode.png"));
    isLoading = true;
}

void OWebView::reloadButtonOff()
{
    if (winType == Normal)
    {
        if (ololanTheme.compare("Light gray") == 0)
            reloadPage->setIcon(QIcon(":/images/reload.png"));
        else if (ololanTheme.compare("Light turquoise") == 0)
            reloadPage->setIcon(QIcon(":/images/reload.png"));
        else if (ololanTheme.compare("Light brown") == 0)
            reloadPage->setIcon(QIcon(":/images/reload.png"));
        else if (ololanTheme.compare("Dark gray") == 0)
            reloadPage->setIcon(QIcon(":/private_mode_images/reloadPrivateMode.png"));
        else if (ololanTheme.compare("Dark turquoise") == 0)
            reloadPage->setIcon(QIcon(":/private_mode_images/reloadPrivateMode.png"));
        else if (ololanTheme.compare("Dark brown") == 0)
            reloadPage->setIcon(QIcon(":/private_mode_images/reloadPrivateMode.png"));
    }
    else if (winType == Private)
        reloadPage->setIcon(QIcon(":/private_mode_images/reloadPrivateMode.png"));
    isLoading = false;
}

void OWebView::setOlolanTheme(QString theme)
{
    ololanTheme = theme;
    if (winType == Normal)
    {
        if (ololanTheme.compare("Light gray") == 0)
        {
            webViewMenu->setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"
                                                     "QMenu::item {color: #888888; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                                     "QMenu::item:selected {background-color: #efeff1; border-top: 1px solid #efeff1; border-bottom: 1px solid #efeff1;}"
                                                     "QMenu::item:!enabled {color: #bbbbbb;}"
                                                     "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                                     "QMenu::separator {background-color: #efeff1; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                                     "QMenu::icon {margin-left: 20px;}"));
        }
        else if (ololanTheme.compare("Light turquoise") == 0)
        {
            webViewMenu->setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"
                                                     "QMenu::item {color: #888888; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                                     "QMenu::item:selected {background-color: #69e8f1; border-top: 1px solid #69e8f1; border-bottom: 1px solid #69e8f1;}"
                                                     "QMenu::item:!enabled {color: #bbbbbb;}"
                                                     "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                                     "QMenu::separator {background-color: #efeff1; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                                     "QMenu::icon {margin-left: 20px;}"));
        }
        else if (ololanTheme.compare("Light brown") == 0)
        {
            webViewMenu->setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"
                                                     "QMenu::item {color: #888888; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                                     "QMenu::item:selected {background-color: #ffa477; border-top: 1px solid #ffa477; border-bottom: 1px solid #ffa477;}"
                                                     "QMenu::item:!enabled {color: #bbbbbb;}"
                                                     "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                                     "QMenu::separator {background-color: #efeff1; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                                     "QMenu::icon {margin-left: 20px;}"));
        }
        else if (ololanTheme.compare("Dark gray") == 0)
        {
            webViewMenu->setStyleSheet(QLatin1String("QMenu {background-color: #434344; padding: 0px; border: 1px solid #434344;}"
                                                     "QMenu::item {color: #dddddd; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                                     "QMenu::item:selected {background-color: #6b6b6b; border-top: 1px solid #6b6b6b; border-bottom: 1px solid #6b6b6b;}"
                                                     "QMenu::item:!enabled {color: #888888;}"
                                                     "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                                     "QMenu::separator {background-color: #6b6b6b; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                                     "QMenu::icon {margin-left: 20px;}"));
        }
        else if (ololanTheme.compare("Dark turquoise") == 0)
        {
            webViewMenu->setStyleSheet(QLatin1String("QMenu {background-color: #1e4446; padding: 0px; border: 1px solid #1e4446;}"
                                                     "QMenu::item {color: #dddddd; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                                     "QMenu::item:selected {background-color: #2e686b; border-top: 1px solid #2e686b; border-bottom: 1px solid #2e686b;}"
                                                     "QMenu::item:!enabled {color: #888888;}"
                                                     "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                                     "QMenu::separator {background-color: #2e686b; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                                     "QMenu::icon {margin-left: 20px;}"));
        }
        else if (ololanTheme.compare("Dark brown") == 0)
        {
            webViewMenu->setStyleSheet(QLatin1String("QMenu {background-color: #462b21; padding: 0px; border: 1px solid #462b21;}"
                                                     "QMenu::item {color: #dddddd; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                                     "QMenu::item:selected {background-color: #6b4132; border-top: 1px solid #6b4132; border-bottom: 1px solid #6b4132;}"
                                                     "QMenu::item:!enabled {color: #888888;}"
                                                     "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                                     "QMenu::separator {background-color: #6b4132; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                                     "QMenu::icon {margin-left: 20px;}"));
        }
    }
    else if (winType == Private)
    {
        webViewMenu->setStyleSheet(QLatin1String("QMenu {background-color: #3c3244; padding: 0px; border: 1px solid #3c3244;}"
                                                 "QMenu::item {color: #dddddd; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                                 "QMenu::item:selected {background-color: #5e4e6b; border-top: 1px solid #5e4e6b; border-bottom: 1px solid #5e4e6b;}"
                                                 "QMenu::item:!enabled {color: #888888;}"
                                                 "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                                 "QMenu::separator {background-color: #5e4e6b; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                                 "QMenu::icon {margin-left: 20px;}"));
    }
}

void OWebView::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void OWebView::dropEvent(QDropEvent *event)
{
    QWebEngineView::dropEvent(event);
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls())
    {
        QUrl url = mimeData->urls().at(0);
        load(url);
    }
}

void OWebView::setWebViewType(WindowType webType)
{
    winType = webType;
}

void OWebView::reloadOrStop()
{
    if (!isLoading)
        reload();
    else
        stop();
}

void OWebView::setCurrentZoomFactor(qreal zoomFactor)
{
    currentZoom = zoomFactor;
}

qreal OWebView::getCurrentZoomFactor()
{
    return currentZoom;
}

void OWebView::enableGeolaction(bool value)
{
    webPage->enableGeolaction(value);
}

void OWebView::setDoNotTrack(bool value)
{
    doNotTrack = value;
}

void OWebView::setPaymentSaveAndFill(bool value = true)
{
    //webPage->setPaymentSaveAndFill(value);
}

void OWebView::setPasswordSaving(bool value)
{
    webPage->setPasswordSaving(value);
}

void OWebView::setSiteAutoSignIn(bool value)
{
    webPage->setSiteAutoSignIn(value);
}

void OWebView::setTabPage(OTabPage *pTabPage)
{
    parentTabPage = pTabPage;
}

OTabPage *OWebView::getTabPage()
{
    return parentTabPage;
}
void OWebView::setEnableMicrophone(bool state)
{
    webPage->setEnableMicrophone(state);
}

void OWebView::setEnableCamera(bool state)
{
    webPage->setEnableCamera(state);
}

void OWebView::closeEvent(QCloseEvent *event)
{
    emit aboutToClose();
    QWebEngineView::closeEvent(event);
}

void OWebView::findTypedText(const QString text)
{
    findText(text);
}

QWebEngineView *OWebView::createWindow(QWebEnginePage::WebWindowType type)
{
    switch (type)
    {
        case QWebEnginePage::WebBrowserTab:
        {
            return parentTabPage->createNewTab();
        }
        case QWebEnginePage::WebBrowserBackgroundTab:
        {
            return parentTabPage->createBackgroundTab();
        }
        case QWebEnginePage::WebBrowserWindow:
        {
            OAppCore *app = static_cast<OAppCore*>(parentTabPage->getParentWindow()->getParentApplication());
            return app->createNewWindow();
        }
        case QWebEnginePage::WebDialog:
        {
            QWebEngineView *popup = new QWebEngineView();
            popup->showNormal();
            return popup;
        }
    }
    return nullptr;
}

void OWebView::showPageSource()
{
    triggerPageAction(QWebEnginePage::ViewSource);
}

void OWebView::contextMenuEvent(QContextMenuEvent *event)
{
    QString historyUrl = QLatin1String("qrc:/web_applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web_applications/html/settingsView.html");
    QString newTabUrl = QLatin1String("qrc:/web_applications/html/newtabView.html");
    QString privateTabUrl = QLatin1String("qrc:/web_applications/html/privatetabView.html");

    if (url().toString().compare(historyUrl) == 0)
    {
        QMenu *standardMenu = createStandardContextMenu();
        if (hasSelection())
            setupTextSelectionContextMenu(true);
        else if (standardMenu->actions().contains(pageAction(QWebEnginePage::CopyLinkToClipboard)))
            setupHistoryContextMenu();
        else
            setupBoDoSeContextMenu();

        standardMenu->clear();
        standardMenu->deleteLater();
    }
    else if ((url().toString().compare(bookmarksUrl) == 0) || (url().toString().compare(downloadsUrl) == 0) ||
             (url().toString().compare(settingsUrl) == 0) || (url().toString().compare(newTabUrl) == 0) ||
             (url().toString().compare(privateTabUrl) == 0))
    {
        QMenu *standardMenu = createStandardContextMenu();
        if (hasSelection())
            setupTextSelectionContextMenu(true);
        else if (standardMenu->actions().contains(pageAction(QWebEnginePage::CopyLinkToClipboard)))
            setupHistoryContextMenu();
        else if (standardMenu->actions().contains(pageAction(QWebEnginePage::CopyImageToClipboard)))
            setupImageContextMenu(true);
        else
            setupBoDoSeContextMenu();

        standardMenu->clear();
        standardMenu->deleteLater();
    }
    else
    {
        QMenu *standardMenu = createStandardContextMenu();
        if (hasSelection())
            setupTextSelectionContextMenu(false);
        else if (standardMenu->actions().contains(pageAction(QWebEnginePage::CopyLinkToClipboard)))
            setupLinkContextMenu();
        else if (standardMenu->actions().contains(pageAction(QWebEnginePage::CopyImageToClipboard)))
            setupImageContextMenu(false);
        else
            setupPageContextMenu();

        standardMenu->clear();
        standardMenu->deleteLater();
    }

    webViewMenu->popup(event->globalPos());
}

void OWebView::setupImageContextMenu(bool isPrivate)
{
    webViewMenu->clear();
    QAction *openInNewtab = new QAction("Open image in new tab", webViewMenu);
    QAction *saveImage = new QAction("Save image", webViewMenu);
    QAction *copyImage = new QAction("Copy image", webViewMenu);
    QAction *copyAddress = new QAction("Copy image address", webViewMenu);
    QAction *inspectImage = new QAction("Inspect image", webViewMenu);

    connect(openInNewtab, &QAction::triggered, this, &OWebView::openImageInNewTab);
    connect(saveImage, &QAction::triggered, webPage->action(OWebPage::DownloadImageToDisk), &QAction::triggered);
    connect(copyImage, &QAction::triggered, webPage->action(OWebPage::CopyImageToClipboard), &QAction::triggered);
    connect(copyAddress, &QAction::triggered, webPage->action(OWebPage::CopyImageUrlToClipboard), &QAction::triggered);
    connect(inspectImage, &QAction::triggered, this, &OWebView::inspectItem);

    if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
        (ololanTheme.compare("Light brown") == 0))
    {
        openInNewtab->setIcon(QIcon(":/images/newTab.png"));
        inspectImage->setIcon(QIcon(":/images/webDev.png"));
        saveImage->setIcon(QIcon(":/images/savePage.png"));
    }
    else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
             (ololanTheme.compare("Dark brown") == 0) || (winType == Private))
    {
        openInNewtab->setIcon(QIcon(":/private_mode_images/newTabPrivateMode.png"));
        inspectImage->setIcon(QIcon(":/private_mode_images/webDevPrivateMode.png"));
        saveImage->setIcon(QIcon(":/private_mode_images/savePagePrivateMode.png"));
    }

    webViewMenu->addAction(openInNewtab);
    webViewMenu->addAction(copyImage);
    webViewMenu->addAction(copyAddress);
    webViewMenu->addAction(saveImage);

    if (!isPrivate)
    {
        webViewMenu->addSeparator();
        webViewMenu->addAction(inspectImage);
    }
}

void OWebView::setupLinkContextMenu()
{
    webViewMenu->clear();
    QAction *openInNewtab = new QAction("Open link in new tab", webViewMenu);
    QAction *openInNewWindow = new QAction("Open link in new window", webViewMenu);
    QAction *openInPrivateWindow = new QAction("Open link in private window", webViewMenu);
    QAction *saveLink = new QAction("Save link", webViewMenu);
    QAction *copyAddress = new QAction("Copy link address", webViewMenu);
    QAction *inspectLink = new QAction("Inspect link", webViewMenu);

    connect(openInNewtab, &QAction::triggered, webPage->action(OWebPage::OpenLinkInNewBackgroundTab), &QAction::triggered);
    connect(openInNewWindow, &QAction::triggered, webPage->action(OWebPage::OpenLinkInNewWindow), &QAction::triggered);
    connect(openInPrivateWindow, &QAction::triggered, this, &OWebView::openContentInPrivateTab);
    connect(saveLink, &QAction::triggered, webPage->action(OWebPage::DownloadLinkToDisk), &QAction::triggered);
    connect(copyAddress, &QAction::triggered, webPage->action(OWebPage::CopyLinkToClipboard), &QAction::triggered);
    connect(inspectLink, &QAction::triggered, this, &OWebView::inspectItem);

    if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
        (ololanTheme.compare("Light brown") == 0))
    {
        openInNewtab->setIcon(QIcon(":/images/newTab.png"));
        openInNewWindow->setIcon(QIcon(":/images/newWindow.png"));
        openInPrivateWindow->setIcon(QIcon(":/images/private.png"));
        inspectLink->setIcon(QIcon(":/images/webDev.png"));
        saveLink->setIcon(QIcon(":/images/savePage.png"));
    }
    else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
             (ololanTheme.compare("Dark brown") == 0) || (winType == Private))
    {
        openInNewtab->setIcon(QIcon(":/private_mode_images/newTabPrivateMode.png"));
        openInNewWindow->setIcon(QIcon(":/private_mode_images/newWindowPrivateMode.png"));
        openInPrivateWindow->setIcon(QIcon(":/private_mode_images/privatePrivateMode.png"));
        inspectLink->setIcon(QIcon(":/private_mode_images/webDevPrivateMode.png"));
        saveLink->setIcon(QIcon(":/private_mode_images/savePagePrivateMode.png"));
    }

    webViewMenu->addAction(openInNewtab);
    webViewMenu->addAction(openInNewWindow);
    webViewMenu->addAction(openInPrivateWindow);
    webViewMenu->addSeparator();
    webViewMenu->addAction(copyAddress);
    webViewMenu->addAction(saveLink);
    webViewMenu->addSeparator();
    webViewMenu->addAction(inspectLink);
}

void OWebView::setupTextSelectionContextMenu(bool isPrivate)
{
    webViewMenu->clear();
    QLabel label = QLabel();
    QFontMetrics fontMetrics(label.fontMetrics());
    const QString textElided= fontMetrics.elidedText(selectedText(), Qt::ElideRight, 140);

    QAction *copy = new QAction(QString("Copy"), webViewMenu);
    QAction *search = new QAction(QString("Search the web for \"") + textElided + QString("\""), webViewMenu);
    QAction *inspectLink = new QAction(QString("Inpect"), webViewMenu);

    connect(copy, &QAction::triggered, webPage->action(OWebPage::Copy), &QAction::triggered);
    connect(search, &QAction::triggered, this, &OWebView::searchTheWebFor);
    connect(inspectLink, &QAction::triggered, this, &OWebView::inspectItem);

    if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
        (ololanTheme.compare("Light brown") == 0))
    {
        search->setIcon(QIcon(":/images/find.png"));
        inspectLink->setIcon(QIcon(":/images/webDev.png"));
    }
    else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
             (ololanTheme.compare("Dark brown") == 0) || (winType == Private))
    {
        search->setIcon(QIcon(":/private_mode_images/findPrivateMode.png"));
        inspectLink->setIcon(QIcon(":/private_mode_images/webDevPrivateMode.png"));
    }

    webViewMenu->addAction(copy);
    webViewMenu->addAction(search);

    if (!isPrivate)
    {
        webViewMenu->addSeparator();
        webViewMenu->addAction(inspectLink);
    }
}

void OWebView::setupPageContextMenu()
{
    webViewMenu->clear();
    QAction *backward = new QAction("Previous", webViewMenu);
    QAction *forward = new QAction("Next", webViewMenu);
    QAction *reloadUrl = new QAction("Reload", webViewMenu);
    reloadUrl->setShortcut(QKeySequence(QLatin1String("Ctrl+R")));
    QAction *save = new QAction("Save as...", webViewMenu);
    save->setShortcut(QKeySequence(QLatin1String("Ctrl+S")));
    QAction *print = new QAction("Print", webViewMenu);
    print->setShortcut(QKeySequence(QLatin1String("Ctrl+P")));
    QAction *pageSource = new QAction("Page source code", webViewMenu);
    pageSource->setShortcut(QKeySequence(QLatin1String("Ctrl+U")));
    QAction *inspect = new QAction("Inspect", webViewMenu);
    inspect->setShortcut(QKeySequence(QLatin1String("Ctrl+Shift+I")));

    connect(backward, &QAction::triggered, webPage->action(OWebPage::Back), &QAction::triggered);
    connect(forward, &QAction::triggered, webPage->action(OWebPage::Forward), &QAction::triggered);

    if (isLoading)
        connect(reloadUrl, &QAction::triggered, webPage->action(OWebPage::Stop), &QAction::triggered);
    else
        connect(reloadUrl, &QAction::triggered, webPage->action(OWebPage::Reload), &QAction::triggered);

    connect(save, &QAction::triggered, this, &OWebView::savePage);
    connect(print, &QAction::triggered, this, &OWebView::printContent);
    connect(pageSource, &QAction::triggered, webPage->action(OWebPage::ViewSource), &QAction::triggered);
    connect(inspect, &QAction::triggered, this, &OWebView::inspectItem);

    if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
        (ololanTheme.compare("Light brown") == 0))
    {
        if (!page()->action(QWebEnginePage::Back)->isEnabled())
        {
            backward->setIcon(QIcon(":/images/deactivateBack.png"));
            backward->setEnabled(false);
        }
        else
            backward->setIcon(QIcon(":/images/backward.png"));

        if (!page()->action(QWebEnginePage::Forward)->isEnabled())
        {
            forward->setIcon(QIcon(":/images/deactivateNext.png"));
            forward->setEnabled(false);
        }
        else
            forward->setIcon(QIcon(":/images/forward.png"));

        if (isLoading)
            reloadUrl->setIcon(QIcon(":/images/stop.png"));
        else
            reloadUrl->setIcon(QIcon(":/images/reload.png"));

        save->setIcon(QIcon(":/images/savePage.png"));
        print->setIcon(QIcon(":/images/print.png"));
        pageSource->setIcon(QIcon(":/images/pageCode.png"));
        inspect->setIcon(QIcon(":/images/webDev.png"));
    }
    else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
             (ololanTheme.compare("Dark brown") == 0) || (winType == Private))
    {
        if (!page()->action(QWebEnginePage::Back)->isEnabled())
            backward->setEnabled(false);
        backward->setIcon(QIcon(":/private_mode_images/backwardPrivateMode.png"));

        if (!page()->action(QWebEnginePage::Forward)->isEnabled())
            forward->setEnabled(false);
        forward->setIcon(QIcon(":/private_mode_images/forwardPrivateMode.png"));

        if (isLoading)
            reloadUrl->setIcon(QIcon(":/private_mode_images/stopPrivateMode.png"));
        else
            reloadUrl->setIcon(QIcon(":/private_mode_images/reloadPrivateMode.png"));

        save->setIcon(QIcon(":/private_mode_images/savePagePrivateMode.png"));
        print->setIcon(QIcon(":/private_mode_images/printPrivateMode.png"));
        pageSource->setIcon(QIcon(":/private_mode_images/pageCodePrivateMode.png"));
        inspect->setIcon(QIcon(":/private_mode_images/webDevPrivateMode.png"));
    }

    webViewMenu->addAction(backward);
    webViewMenu->addAction(forward);
    webViewMenu->addAction(reloadUrl);
    webViewMenu->addSeparator();
    webViewMenu->addAction(save);
    webViewMenu->addAction(print);
    webViewMenu->addSeparator();
    webViewMenu->addAction(pageSource);
    webViewMenu->addAction(inspect);
}

void OWebView::setupHistoryContextMenu()
{
    webViewMenu->clear();
    QAction *openInNewtab = new QAction("Open in new tab", webViewMenu);
    QAction *openInNewWindow = new QAction("Open in new window", webViewMenu);
    QAction *openInPrivateWindow = new QAction("Open in private window", webViewMenu);
    QAction *saveLink = new QAction("Save link", webViewMenu);
    QAction *copyAddress = new QAction("Copy link address", webViewMenu);

    connect(openInNewtab, &QAction::triggered, webPage->action(OWebPage::OpenLinkInNewBackgroundTab), &QAction::triggered);
    connect(openInNewWindow, &QAction::triggered, webPage->action(OWebPage::OpenLinkInNewWindow), &QAction::triggered);
    connect(openInPrivateWindow, &QAction::triggered, this, &OWebView::openContentInPrivateTab);
    connect(saveLink, &QAction::triggered, webPage->action(OWebPage::DownloadLinkToDisk), &QAction::triggered);
    connect(copyAddress, &QAction::triggered, webPage->action(OWebPage::CopyLinkToClipboard), &QAction::triggered);

    if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
        (ololanTheme.compare("Light brown") == 0))
    {
        openInNewtab->setIcon(QIcon(":/images/newTab.png"));
        openInNewWindow->setIcon(QIcon(":/images/newWindow.png"));
        openInPrivateWindow->setIcon(QIcon(":/images/private.png"));
        saveLink->setIcon(QIcon(":/images/savePage.png"));
    }
    else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
             (ololanTheme.compare("Dark brown") == 0) || (winType == Private))
    {
        openInNewtab->setIcon(QIcon(":/private_mode_images/newTabPrivateMode.png"));
        openInNewWindow->setIcon(QIcon(":/private_mode_images/newWindowPrivateMode.png"));
        openInPrivateWindow->setIcon(QIcon(":/private_mode_images/privatePrivateMode.png"));
        saveLink->setIcon(QIcon(":/private_mode_images/savePagePrivateMode.png"));
    }

    webViewMenu->addAction(openInNewtab);
    webViewMenu->addAction(openInNewWindow);
    webViewMenu->addAction(openInPrivateWindow);
    webViewMenu->addSeparator();
    webViewMenu->addAction(copyAddress);
    webViewMenu->addAction(saveLink);
}

void OWebView::setupBoDoSeContextMenu()
{
    webViewMenu->clear();
    QAction *backward = new QAction("Previous", webViewMenu);
    QAction *forward = new QAction("Next", webViewMenu);
    QAction *reloadUrl = new QAction("Reload", webViewMenu);
    reloadUrl->setShortcut(QKeySequence(QLatin1String("Ctrl+R")));
    //QAction *save = new QAction("Save as...", webViewMenu);
    //save->setShortcut(QKeySequence(QLatin1String("Ctrl+S")));
    QAction *print = new QAction("Print", webViewMenu);
    print->setShortcut(QKeySequence(QLatin1String("Ctrl+P")));

    connect(backward, &QAction::triggered, webPage->action(OWebPage::Back), &QAction::triggered);
    connect(forward, &QAction::triggered, webPage->action(OWebPage::Forward), &QAction::triggered);

    if (isLoading)
        connect(reloadUrl, &QAction::triggered, webPage->action(OWebPage::Stop), &QAction::triggered);
    else
        connect(reloadUrl, &QAction::triggered, webPage->action(OWebPage::Reload), &QAction::triggered);

    //connect(save, &QAction::triggered, this, &OWebView::savePage);
    connect(print, &QAction::triggered, this, &OWebView::printContent);

    if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
        (ololanTheme.compare("Light brown") == 0))
    {
        if (!page()->action(QWebEnginePage::Back)->isEnabled())
            backward->setEnabled(false);
        backward->setIcon(QIcon(":/private_mode_images/backwardPrivateMode.png"));

        if (!page()->action(QWebEnginePage::Forward)->isEnabled())
            forward->setEnabled(false);
        forward->setIcon(QIcon(":/private_mode_images/forwardPrivateMode.png"));

        if (isLoading)
            reloadUrl->setIcon(QIcon(":/images/stop.png"));
        else
            reloadUrl->setIcon(QIcon(":/images/reload.png"));

        //save->setIcon(QIcon(":/images/savePage.png"));
        print->setIcon(QIcon(":/images/print.png"));
    }
    else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
             (ololanTheme.compare("Dark brown") == 0) || (winType == Private))
    {
        if (!page()->action(QWebEnginePage::Back)->isEnabled())
        {
            backward->setIcon(QIcon(":/images/deactivateBack.png"));
            backward->setEnabled(false);
        }
        else
            backward->setIcon(QIcon(":/private_mode_images/backwardPrivateMode.png"));

        if (!page()->action(QWebEnginePage::Forward)->isEnabled())
        {
            forward->setIcon(QIcon(":/images/deactivateNext.png"));
            forward->setEnabled(false);
        }
        else
            forward->setIcon(QIcon(":/private_mode_images/forwardPrivateMode.png"));

        if (isLoading)
            reloadUrl->setIcon(QIcon(":/private_mode_images/stopPrivateMode.png"));
        else
            reloadUrl->setIcon(QIcon(":/private_mode_images/reloadPrivateMode.png"));

        //save->setIcon(QIcon(":/private_mode_images/savePagePrivateMode.png"));
        print->setIcon(QIcon(":/private_mode_images/printPrivateMode.png"));
    }

    webViewMenu->addAction(backward);
    webViewMenu->addAction(forward);
    webViewMenu->addAction(reloadUrl);
    webViewMenu->addSeparator();
   // webViewMenu->addAction(save);
    webViewMenu->addAction(print);
}

void OWebView::inspectItem()
{
    parentTabPage->webDev();
    triggerPageAction(QWebEnginePage::InspectElement);
}

void OWebView::openContentInPrivateTab()
{
    if (!hoveredLink.isEmpty())
    {
        OAppCore *app = static_cast<OAppCore*>(parentTabPage->getParentWindow()->getParentApplication());
        app->openInPrivateWindow(hoveredLink);
    }
}

void OWebView::openImageInNewTab()
{
    webPage->action(OWebPage::CopyImageUrlToClipboard)->trigger();
    QClipboard *clipboard = QApplication::clipboard();
    QString link = clipboard->text();
    parentTabPage->openInTab(link);
}

void OWebView::printContent()
{
    parentTabPage->print();
}

void OWebView::searchTheWebFor()
{
    if (hasSelection())
    {
        parentTabPage->searchTheWebFor(selectedText());
    }
}

void OWebView::savePage()
{
    parentTabPage->savePage();
}

void OWebView::onLinkHovered(const QString &link)
{
    if (!link.isEmpty())
    {
        hoveredLink = link;
        QFontMetrics fontMetrics(this->fontMetrics());
        if (url().toEncoded().compare("qrc:/web_applications/html/newtabView.html") != 0)
            QToolTip::showText(mapToGlobal(QPoint(x(), height()-45)), fontMetrics.elidedText(link, Qt::ElideRight, width()*0.2), this);
    }
    else
        QToolTip::hideText();
}

void OWebView::mouseMoveEvent(QMouseEvent *event)
{
    QWebEngineView::mouseMoveEvent(event);
}

bool OWebView::isPrivateBrowsing()
{
    return privateBrowsing;
}

void OWebView::setIsPrivateBrowsing(bool value)
{
    privateBrowsing = value;
}
