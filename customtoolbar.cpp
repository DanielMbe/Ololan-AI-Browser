#include "customtoolbar.h"
#include "odownloadwidget.h"
#include "osmartinput.h"
#include "ofindinput.h"
#include "obrowsermenu.h"
#include "osyncwidget.h"
#include "otabpage.h"
#include "customwindow.h"
#include "oappcore.h"
#include "obookmarkscore.h"
#include <QPropertyAnimation>
#include <QContextMenuEvent>

CustomToolBar::CustomToolBar(QWidget *parent)
    :QToolBar(parent)
{
    friendBar = nullptr;
    isBookmarkBar = false;
    isDownloading = false;
    selectedBookmark = nullptr;
}

void CustomToolBar::startOnAnimation()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "height", this);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setDuration(80);
    animation->setStartValue(1);
    animation->setEndValue(32);
    animation->start();
}

void CustomToolBar::startOffAnimation()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "height", this);
    connect(animation, &QPropertyAnimation::finished, this, &CustomToolBar::hide);
    connect(animation, &QPropertyAnimation::finished, this, &CustomToolBar::setFriendBarStyle);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setDuration(80);
    animation->setStartValue(32);
    animation->setEndValue(1);
    animation->start();
}

void CustomToolBar::setFriendBarStyle()
{
    if (ololanTheme.compare("Light gray") == 0)
        friendBar->setLightGrayTheme(false);
    else if (ololanTheme.compare("Light turquoise") == 0)
        friendBar->setLightTurquoiseTheme(false);
    else if (ololanTheme.compare("Light brown") == 0)
        friendBar->setLightBrownTheme(false);
    else if (ololanTheme.compare("Dark gray") == 0)
        friendBar->setDarkGrayTheme(false);
    else if (ololanTheme.compare("Dark turquoise") == 0)
        friendBar->setDarkTurquoiseTheme(false);
    else if (ololanTheme.compare("Dark brown") == 0)
        friendBar->setDarkBrownTheme(false);
    else if (ololanTheme.compare("Private") == 0)
        friendBar->setPrivateTheme(false);
}

void CustomToolBar::setDownloadState(bool state)
{
    isDownloading = state;

    if (state)
        downloadButton->setIcon(QIcon(":/web_applications/images/activeDownload.png"));
    else
    {
        if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) || (ololanTheme.compare("Light brown") == 0))
            downloadButton->setIcon(QIcon(":/images/downloads.png"));
        else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) || (ololanTheme.compare("Dark brown") == 0) || (ololanTheme.compare("Private") == 0))
            downloadButton->setIcon(QIcon(":/private_mode_images/downloadsPrivateMode.png"));
    }
}

void CustomToolBar::setFriendBar(CustomToolBar *bar)
{
    if (!friendBar)
        friendBar = bar;
}

CustomToolBar* CustomToolBar::getFriendBar()
{
    return friendBar;
}

OBrowserMenu *CustomToolBar::getBrowserMenu()
{
    return browserMenu;
}

OSmartInput *CustomToolBar::getSmartBox()
{
    return smartInput;
}

OFindInput *CustomToolBar::getFindInput()
{
    return findInput;
}

ODownloadWidget *CustomToolBar::getDownloadMenu()
{
    return downloadMenu;
}

QToolButton *CustomToolBar::getReloadButton()
{
    return reloadButton;
}

QToolButton *CustomToolBar::getNextButton()
{
    return nextButton;
}

QToolButton *CustomToolBar::getBackButton()
{
    return backButton;
}

QToolButton *CustomToolBar::getDownloadButton()
{
    return downloadButton;
}

QToolButton *CustomToolBar::getUpdatesButton()
{
    return updatesButton;
}

QToolButton *CustomToolBar::getAssistantButton()
{
    return assistantButton;
}

OSyncWidget *CustomToolBar::getSyncWidget()
{
    return syncMenu;
}

void CustomToolBar::setWinType(WindowType type)
{
    winType = type;
}

void CustomToolBar::setupControls(QString webSearchEngine = "", bool ololanAssistant = true)
{
    smartInput = new OSmartInput(this);
    smartInput->setObjectName(QLatin1String("smartInput"));
    smartInput->setSearchEngine(webSearchEngine);
    smartInput->setFixedHeight(35);

    findInput = new OFindInput(this);
    findInput->setObjectName(QLatin1String("findInput"));
    findInput->setFixedSize(1, 35);
    findInput->setVisible(false);
    connect(findInput->getCloseButton(), &CustomToolButton::clicked, this, &CustomToolBar::updateFindInputVisibility);

    foundText = new QLabel("0", this);
    foundText->setAlignment(Qt::AlignCenter);
    foundText->setFixedSize(70, 34);
    foundText->setVisible(false);

    backButton = new QToolButton(this);
    backButton->setObjectName(QLatin1String("goBack"));
    backButton->setFixedSize(36, 34);
    backButton->setEnabled(false);

    nextButton = new QToolButton(this);
    nextButton->setObjectName(QLatin1String("goNext"));
    nextButton->setFixedSize(36, 34);
    nextButton->setEnabled(false);

    reloadButton = new QToolButton(this);
    reloadButton->setObjectName(QLatin1String("reload"));
    reloadButton->setFixedSize(36, 34);

    assistantButton = new QToolButton(this);
    assistantButton->setCheckable(true);
    assistantButton->setObjectName(QLatin1String("eina"));
    assistantButton->setFixedSize(36, 34);

    updatesButton = new QToolButton(this);
    updatesButton->setObjectName(QLatin1String("updates"));
    updatesButton->setFixedSize(36, 34);
    updatesButton->setVisible(false);
    updatesButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ff1e1e; border: none; padding: 0px; border-radius: 3.5%;}"
                                               "QToolButton:hover {background-color: #f01c1c; border: 1px solid #f01c1c;}"
                                               "QToolButton:pressed {background-color: #df1a1a; border: 2px solid #df1a1a;}"));
    updatesButton->setIcon(QIcon(":/private_mode_images/downloadsPrivateMode.png"));

    menuButton = new QToolButton(this);
    menuButton->setObjectName(QLatin1String("browserMenuLauncher"));
    menuButton->setPopupMode(QToolButton::InstantPopup);
    menuButton->setFixedSize(36, 34);

    browserMenu = new OBrowserMenu(menuButton);
    browserMenu->setObjectName(QLatin1String("browserMenu"));
    browserMenu->setupView();
    browserMenu->setupConnection();
    browserMenu->disablePageActions();
    menuButton->setMenu(browserMenu);

    accountButton = new QToolButton(this);
    accountButton->setObjectName(QLatin1String("ololanAccount"));
    accountButton->setPopupMode(QToolButton::InstantPopup);
    accountButton->setFixedSize(36, 34);

    syncMenu = new OSyncWidget(accountButton);
    syncMenu->setObjectName(QLatin1String("accountMenu"));
    accountButton->setMenu(syncMenu);

    downloadButton = new QToolButton(this);
    downloadButton->setObjectName(QLatin1String("downloadMenuLauncher"));
    downloadButton->setPopupMode(QToolButton::InstantPopup);
    downloadButton->setFixedSize(36, 34);

    downloadMenu = new ODownloadWidget(downloadButton, winType);
    downloadButton->setObjectName(QLatin1String("downloadMenu"));
    downloadButton->setMenu(downloadMenu);

    addWidget(backButton);
    addSeparator();
    addWidget(nextButton);
    addSeparator();
    addWidget(reloadButton);
    addSeparator();
    addWidget(smartInput);
    inSeparator = addSeparator();
    findInput->setClassPtr(addWidget(findInput));
    fTextSeparator = addSeparator();
    foundTextAction = addWidget(foundText);
    addSeparator();
    addWidget(assistantButton);
    addSeparator();
    addWidget(downloadButton);
    addSeparator();
    addWidget(accountButton);
    addSeparator();
    updatesAction = addWidget(updatesButton);
    addSeparator();
    addWidget(menuButton);
    inSeparator->setVisible(false);
}

void CustomToolBar::setBookmarkBarTheme(QString themeColor)
{
    ololanTheme = themeColor;
    if (themeColor.compare("Light gray") == 0)
        setStyleSheet(QLatin1String("QToolBar {background-color: #ffffff; border-bottom: 1px solid #e1e1e3;}"
                                    "QToolBar::separator {background-color: #ffffff; width: 4px;}"));

    else if (themeColor.compare("Light turquoise") == 0)
        setStyleSheet(QLatin1String("QToolBar {background-color: #ffffff; border-bottom: 1px solid #efefe1;}"
                                    "QToolBar::separator {background-color: #ffffff; width: 4px;}"));

    else if (themeColor.compare("Light brown") == 0)
        setStyleSheet(QLatin1String("QToolBar {background-color: #ffffff; border-bottom: 1px solid #efefe1;}"
                                    "QToolBar::separator {background-color: #ffffff; width: 4px;}"));

    else if (themeColor.compare("Dark gray") == 0)
        setStyleSheet(QLatin1String("QToolBar {background-color: #6b6b6b; border-bottom: 1px solid #6b6b6b;}"
                                    "QToolBar::separator {background-color: #6b6b6b; width: 4px;}"));

    else if (themeColor.compare("Dark turquoise") == 0)
        setStyleSheet(QLatin1String("QToolBar {background-color: #2e686b; border-bottom: 1px solid #2e686b;}"
                                    "QToolBar::separator {background-color: #2e686b; width: 4px;}"));

    else if (themeColor.compare("Dark brown") == 0)
        setStyleSheet(QLatin1String("QToolBar {background-color: #6b4232; border-bottom: 1px solid #6b4232;}"
                                    "QToolBar::separator {background-color: #6b4232; width: 4px;}"));

    else if (themeColor.compare("Private") == 0)
        setStyleSheet(QLatin1String("QToolBar {background-color: #5e4e6b; border-bottom: 1px solid #5e4e6b;}"
                                    "QToolBar::separator {background-color: #5e4e6b; width: 4px;}"));

    setMenuTheme();
}

void CustomToolBar::setLightGrayTheme(bool bookmarBarVisible)
{
    if (bookmarBarVisible)
        setStyleSheet(QLatin1String("QToolBar {background-color: #ffffff; border-bottom: none;}"
                                    "QToolBar::separator {background-color: #ffffff; width: 4px;}"));
    else
        setStyleSheet(QLatin1String("QToolBar {background-color: #ffffff; border-bottom: 1px solid #e1e1e3;}"
                                    "QToolBar::separator {background-color: #ffffff; width: 4px;}"));

    backButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding: 0px; border-radius: 3.5%;"
                                            "background-image: url(:/images/backward.png); background-repeat: no-repeat;"
                                            "background-position: center;}"
                                            "QToolButton:hover {background-color: #e1e1e3; border: 1px solid #e1e1e3;}"
                                            "QToolButton:pressed {background-color: #c4c4c6; border: 2px solid #c4c4c6;}"
                                            "QToolButton:!enabled {background-image: url(:/images/deactivateBack.png);}"));

    nextButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding: 0px; border-radius: 3.5%;"
                                            "background-image: url(:/images/forward.png); background-repeat: no-repeat;"
                                            "background-position: center;}"
                                            "QToolButton:hover {background-color: #e1e1e3; border: 1px solid #e1e1e3;}"
                                            "QToolButton:pressed {background-color: #c4c4c6; border: 2px solid #c4c4c6;}"
                                            "QToolButton:!enabled {background-image: url(:/images/deactivateNext.png);}"));

    reloadButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding-top: 0px; border-radius: 3.5%;}"
                                              "QToolButton:hover {background-color: #e1e1e3; border: 1px solid #e1e1e3;}"
                                              "QToolButton:pressed {background-color: #c4c4c6; border: 2px solid #c4c4c6;}"));

    menuButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding: 0px; border-radius: 3.5%;}"
                                            "QToolButton:hover {background-color: #e1e1e3; border: 1px solid #e1e1e3;}"
                                            "QToolButton:pressed {background-color: #c4c4c6; border: 2px solid #c4c4c6;}"
                                            "QToolButton::menu-indicator {image: none;}"));

    accountButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding: 0px; border-radius: 3.5%; color: #656565;}"
                                               "QToolButton:hover {background-color: #e1e1e3; border: 1px solid #e1e1e3;}"
                                               "QToolButton:pressed {background-color: #c4c4c6; border: 2px solid #c4c4c6;}"
                                               "QToolButton::menu-indicator {image: none;}"));

    downloadButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding-top: 0px; border-radius: 3.5%;}"
                                                "QToolButton:hover {background-color: #e1e1e3; border: 1px solid #e1e1e3;}"
                                                "QToolButton:pressed {background-color: #c4c4c6; border: 2px solid #c4c4c6;}"
                                                "QToolButton::menu-indicator {image: none;}"));

    assistantButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; background-repeat: no-repeat;"
                                                 "background-image: url(:/images/chatDefault.png); background-position: center; border-radius: 3.5%;}"
                                                 "QToolButton:hover {background-image: url(:/images/chatFocused.png); background-color: #e1e1e3; border: 1px solid #e1e1e3;}"
                                                 "QToolButton:checked {background-image: url(:/images/chatOn.png); background-color: #c4c4c6; border: 2px solid #c4c4c6;}"
                                                 "QToolButton::menu-indicator {image: none;}"));

    foundText->setStyleSheet(QLatin1String("QLabel {border: 2px solid #e1e1e3; background-color: #e1e1e3; color: #353535;"
                                           "border-radius: 3.5%; font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    browserMenu->setLightGrayTheme();
    smartInput->setLightGrayTheme();
    smartInput->setSecurityTheme();
    findInput->setLightGrayTheme();
    syncMenu->setLightGrayTheme();
    downloadMenu->setLightGrayTheme();
    reloadButton->setIcon(QIcon(":/images/reload.png"));
    menuButton->setIcon(QIcon(":/images/menu.png"));
    accountButton->setIcon(QIcon(":/images/account.png"));

    if (!isDownloading)
        downloadButton->setIcon(QIcon(":/images/downloads.png"));
    ololanTheme = "Light gray";
}

void CustomToolBar::setLightTurquoiseTheme(bool bookmarBarVisible)
{
    if (bookmarBarVisible)
        setStyleSheet(QLatin1String("QToolBar {background-color: #ffffff; border-bottom: none;}"
                                    "QToolBar::separator {background-color: #ffffff; width: 4px;}"));
    else
        setStyleSheet(QLatin1String("QToolBar {background-color: #ffffff; border-bottom: 1px solid #56bcc5;}"
                                    "QToolBar::separator {background-color: #ffffff; width: 4px;}"));

    backButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding: 0px; border-radius: 3.5%;"
                                            "background-image: url(:/images/backward.png); background-repeat: no-repeat;"
                                            "background-position: center;}"
                                            "QToolButton:hover {background-color: #c0ebee; border: 1px solid #c0ebee;}"
                                            "QToolButton:pressed {background-color: #b0dadd; border: 2px solid #b0dadd;}"
                                            "QToolButton:!enabled {background-image: url(:/images/deactivateBack.png);}"));

    nextButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding: 0px; border-radius: 3.5%;"
                                            "background-image: url(:/images/forward.png); background-repeat: no-repeat;"
                                            "background-position: center;}"
                                            "QToolButton:hover {background-color: #c0ebee; border: 1px solid #c0ebee;}"
                                            "QToolButton:pressed {background-color: #b0dadd; border: 2px solid #b0dadd;}"
                                            "QToolButton:!enabled {background-image: url(:/images/deactivateNext.png);}"));

    reloadButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding-top: 0px; border-radius: 3.5%;}"
                                              "QToolButton:hover {background-color: #c0ebee; border: 1px solid #c0ebee;}"
                                              "QToolButton:pressed {background-color: #b0dadd; border: 2px solid #b0dadd;}"));

    menuButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding: 0px; border-radius: 3.5%;}"
                                            "QToolButton:hover {background-color: #c0ebee; border: 1px solid #c0ebee;}"
                                            "QToolButton:pressed {background-color: #b0dadd; border: 2px solid #b0dadd;}"
                                            "QToolButton::menu-indicator {image: none;}"));

    accountButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding: 0px; border-radius: 3.5%; color: #656565;}"
                                               "QToolButton:hover {background-color: #c0ebee; border: 1px solid #c0ebee;}"
                                               "QToolButton:pressed {background-color: #b0dadd; border: 2px solid #b0dadd;}"
                                               "QToolButton::menu-indicator {image: none;}"));

    downloadButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding-top: 0px; border-radius: 3.5%;}"
                                                "QToolButton:hover {background-color: #c0ebee; border: 1px solid #c0ebee;}"
                                                "QToolButton:pressed {background-color: #b0dadd; border: 2px solid #b0dadd;}"
                                                "QToolButton::menu-indicator {image: none;}"));

    assistantButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; background-repeat: no-repeat;"
                                                 "background-image: url(:/images/chatDefault.png); background-position: center; border-radius: 3.5%;}"
                                                 "QToolButton:hover {background-image: url(:/images/chatFocused.png); background-color: #c0ebee; border: 1px solid #c0ebee;}"
                                                 "QToolButton:checked {background-image: url(:/images/chatOn.png); background-color: #b0dadd; border: 2px solid #b0dadd;}"
                                                 "QToolButton::menu-indicator {image: none;}"));

    foundText->setStyleSheet(QLatin1String("QLabel {border: 2px solid #c0ebee; background-color: #c0ebee; color: #353535;"
                                           "border-radius: 3.5%; font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    browserMenu->setLightTurquoiseTheme();
    smartInput->setLightTurquoiseTheme();
    smartInput->setSecurityTheme();
    findInput->setLightTurquoiseTheme();
    syncMenu->setLightTurquoiseTheme();
    downloadMenu->setLightTurquoiseTheme();
    reloadButton->setIcon(QIcon(":/images/reload.png"));
    menuButton->setIcon(QIcon(":/images/menu.png"));
    accountButton->setIcon(QIcon(":/images/account.png"));

    if (!isDownloading)
        downloadButton->setIcon(QIcon(":/images/downloads.png"));
    ololanTheme = "Light turquoise";
}

void CustomToolBar::setLightBrownTheme(bool bookmarBarVisible)
{
    if (bookmarBarVisible)
        setStyleSheet(QLatin1String("QToolBar {background-color: #ffffff; border-bottom: none;}"
                                     "QToolBar::separator {background-color: #ffffff; width: 4px;}"));
    else
        setStyleSheet(QLatin1String("QToolBar {background-color: #ffffff; border-bottom: 1px solid #e3e3e5;}"
                                     "QToolBar::separator {background-color: #ffffff; width: 4px;}"));

    backButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding: 0px; border-radius: 3.5%;"
                                            "background-image: url(:/images/backward.png); background-repeat: no-repeat;"
                                            "background-position: center;}"
                                            "QToolButton:hover {background-color: #ffd0ae; border: 1px solid #ffd0ae;}"
                                            "QToolButton:pressed {background-color: #ebbfa0; border: 2px solid #ebbfa0;}"
                                            "QToolButton:!enabled {background-image: url(:/images/deactivateBack.png);}"));

    nextButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding: 0px; border-radius: 3.5%;"
                                            "background-image: url(:/images/forward.png); background-repeat: no-repeat;"
                                            "background-position: center;}"
                                            "QToolButton:hover {background-color: #ffd0ae; border: 1px solid #ffd0ae;}"
                                            "QToolButton:pressed {background-color: #ebbfa0; border: 2px solid #ebbfa0;}"
                                            "QToolButton:!enabled {background-image: url(:/images/deactivateNext.png);}"));

    reloadButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding-top: 0px; border-radius: 3.5%;}"
                                              "QToolButton:hover {background-color: #ffd0ae; border: 1px solid #ffd0ae;}"
                                              "QToolButton:pressed {background-color: #ebbfa0; border: 2px solid #ebbfa0;}"));

    menuButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding: 0px; border-radius: 3.5%;}"
                                            "QToolButton:hover {background-color: #ffd0ae; border: 1px solid #ffd0ae;}"
                                            "QToolButton:pressed {background-color: #ebbfa0; border: 2px solid #ebbfa0;}"
                                            "QToolButton::menu-indicator {image: none;}"));

    accountButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding: 0px; border-radius: 3.5%; color: #656565;}"
                                               "QToolButton:hover {background-color: #ffd0ae; border: 1px solid #ffd0ae;}"
                                               "QToolButton:pressed {background-color: #ebbfa0; border: 2px solid #ebbfa0;}"
                                               "QToolButton::menu-indicator {image: none;}"));

    downloadButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; padding-top: 0px; border-radius: 3.5%;}"
                                                "QToolButton:hover {background-color: #ffd0ae; border: 1px solid #ffd0ae;}"
                                                "QToolButton:pressed {background-color: #ebbfa0; border: 2px solid #ebbfa0;}"
                                                "QToolButton::menu-indicator {image: none;}"));

    assistantButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: none; background-repeat: no-repeat;"
                                                 "background-image: url(:/images/chatDefault.png); background-position: center; border-radius: 3.5%;}"
                                                 "QToolButton:hover {background-image: url(:/images/chatFocusedBrown.png); background-color: #ffd0ae; border: 1px solid #ffd0ae;}"
                                                 "QToolButton:checked {background-image: url(:/images/chatOnBrown.png); background-color: #ebbfa0; border: 2px solid #ebbfa0;}"
                                                 "QToolButton::menu-indicator {image: none;}"));

    foundText->setStyleSheet(QLatin1String("QLabel {border: 2px solid #ffd0ae; background-color: #ffd0ae; color: #909090;"
                                           "border-radius: 3.5%; font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    browserMenu->setLightBrownTheme();
    smartInput->setLightBrownTheme();
    smartInput->setSecurityTheme();
    findInput->setLightBrownTheme();
    syncMenu->setLightBrownTheme();
    downloadMenu->setLightBrownTheme();
    reloadButton->setIcon(QIcon(":/images/reload.png"));
    menuButton->setIcon(QIcon(":/images/menu.png"));
    accountButton->setIcon(QIcon(":/images/account.png"));

    if (!isDownloading)
        downloadButton->setIcon(QIcon(":/images/downloads.png"));
    ololanTheme = "Light brown";
}

void CustomToolBar::setDarkGrayTheme(bool bookmarBarVisible)
{
    if (bookmarBarVisible)
        setStyleSheet(QLatin1String("QToolBar {background-color: #6b6b6b; border-bottom: none;}"
                                     "QToolBar::separator {background-color: #6b6b6b; width: 4px;}"));
    else
        setStyleSheet(QLatin1String("QToolBar {background-color: #6b6b6b; border-bottom: 1px solid #6b6b6b;}"
                                     "QToolBar::separator {background-color: #6b6b6b; width: 4px;}"));

    backButton->setStyleSheet(QLatin1String("QToolButton {background-color: #6b6b6b; border: none; padding: 0px; border-radius: 3.5%;"
                                            "background-image: url(:/private_mode_images/backwardPrivateMode.png); background-repeat: no-repeat;"
                                            "background-position: center;}"
                                            "QToolButton:hover {background-color: #777778; border: 1px solid #777778;}"
                                            "QToolButton:pressed {background-color: #434344; border: 2px solid #434344;}"
                                            "QToolButton:!enabled {background-image: url(:/images/backward.png);}"));

    nextButton->setStyleSheet(QLatin1String("QToolButton {background-color: #6b6b6b; border: none; padding: 0px; border-radius: 3.5%;"
                                            "background-image: url(:/private_mode_images/forwardPrivateMode.png); background-repeat: no-repeat;"
                                            "background-position: center;}"
                                            "QToolButton:hover {background-color: #777778; border: 1px solid #777778;}"
                                            "QToolButton:pressed {background-color: #434344; border: 2px solid #434344;}"
                                            "QToolButton:!enabled {background-image: url(:/images/forward.png);}"));

    reloadButton->setStyleSheet(QLatin1String("QToolButton {background-color: #6b6b6b; border: none; padding-top: 0px; border-radius: 3.5%;}"
                                              "QToolButton:hover {background-color: #777778; border: 1px solid #777778;}"
                                              "QToolButton:pressed {background-color: #434344; border: 2px solid #434344;}"));

    menuButton->setStyleSheet(QLatin1String("QToolButton {background-color: #6b6b6b; border: none; padding: 0px; border-radius: 3.5%;}"
                                            "QToolButton:hover {background-color: #777778; border: 1px solid #777778;}"
                                            "QToolButton:pressed {background-color: #434344; border: 2px solid #434344;}"
                                            "QToolButton::menu-indicator {image: none;}"));

    accountButton->setStyleSheet(QLatin1String("QToolButton {background-color: #6b6b6b; border: none; padding: 0px; border-radius: 3.5%; color: #dddddd;}"
                                               "QToolButton:hover {background-color: #777778; border: 1px solid #777778;}"
                                               "QToolButton:pressed {background-color: #434344; border: 2px solid #434344;}"
                                               "QToolButton::menu-indicator {image: none;}"));

    downloadButton->setStyleSheet(QLatin1String("QToolButton {background-color: #6b6b6b; border: none; padding-top: 0px; border-radius: 3.5%;}"
                                                "QToolButton:hover {background-color: #777778; border: 1px solid #777778;}"
                                                "QToolButton:pressed {background-color: #434344; border: 2px solid #434344;}"
                                                "QToolButton::menu-indicator {image: none;}"));

    assistantButton->setStyleSheet(QLatin1String("QToolButton {background-color: #6b6b6b; border: none; background-repeat: no-repeat;"
                                                 "background-image: url(:/images/chatPrivateMode.png); background-position: center; border-radius: 3.5%;}"
                                                 "QToolButton:hover {background-image: url(:/images/chatFocused.png); background-color: #777778; border: 1px solid #777778;}"
                                                 "QToolButton:checked {background-image: url(:/images/chatOn.png); background-color: #434344; border: 2px solid #434344;}"
                                                 "QToolButton::menu-indicator {image: none;}"));

    foundText->setStyleSheet(QLatin1String("QLabel {border: 2px solid #434344; background-color: #434344; color: #dddddd;"
                                           "border-radius: 3.5%; font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    browserMenu->setDarkGrayTheme();
    smartInput->setDarkGrayTheme();
    smartInput->setSecurityTheme();
    findInput->setDarkGrayTheme();
    syncMenu->setDarkGrayTheme();
    downloadMenu->setDarkGrayTheme();
    reloadButton->setIcon(QIcon(":/private_mode_images/reloadPrivateMode.png"));
    menuButton->setIcon(QIcon(":/private_mode_images/menuPrivateMode.png"));
    accountButton->setIcon(QIcon(":/private_mode_images/accountPrivateMode.png"));

    if (!isDownloading)
        downloadButton->setIcon(QIcon(":/private_mode_images/downloadsPrivateMode.png"));
    ololanTheme = "Dark gray";
}

void CustomToolBar::setDarkTurquoiseTheme(bool bookmarBarVisible)
{
    if (bookmarBarVisible)
        setStyleSheet(QLatin1String("QToolBar {background-color: #2e686b; border-bottom: none;}"
                                     "QToolBar::separator {background-color: #2e686b; width: 4px;}"));
    else
        setStyleSheet(QLatin1String("QToolBar {background-color: #2e686b; border-bottom: 1px solid #2e686b;}"
                                     "QToolBar::separator {background-color: #2e686b; width: 4px;}"));

    backButton->setStyleSheet(QLatin1String("QToolButton {background-color: #2e686b; border: none; padding: 0px; border-radius: 3.5%;"
                                            "background-image: url(:/private_mode_images/backwardPrivateMode.png); background-repeat: no-repeat;"
                                            "background-position: center;}"
                                            "QToolButton:hover {background-color: #347779; border: 1px solid #347779;}"
                                            "QToolButton:pressed {background-color: #1e4546; border: 2px solid #1e4546;}"
                                            "QToolButton:!enabled {background-image: url(:/images/backward.png);}"));

    nextButton->setStyleSheet(QLatin1String("QToolButton {background-color: #2e686b; border: none; padding: 0px; border-radius: 3.5%;"
                                            "background-image: url(:/private_mode_images/forwardPrivateMode.png); background-repeat: no-repeat;"
                                            "background-position: center;}"
                                            "QToolButton:hover {background-color: #347779; border: 1px solid #347779;}"
                                            "QToolButton:pressed {background-color: #1e4546; border: 2px solid #1e4546;}"
                                            "QToolButton:!enabled {background-image: url(:/images/forward.png);}"));

    reloadButton->setStyleSheet(QLatin1String("QToolButton {background-color: #2e686b; border: none; padding-top: 0px; border-radius: 3.5%;}"
                                              "QToolButton:hover {background-color: #347779; border: 1px solid #347779;}"
                                              "QToolButton:pressed {background-color: #1e4546; border: 2px solid #1e4546;}"));

    menuButton->setStyleSheet(QLatin1String("QToolButton {background-color: #2e686b; border: none; padding: 0px; border-radius: 3.5%;}"
                                            "QToolButton:hover {background-color: #347779; border: 1px solid #347779;}"
                                            "QToolButton:pressed {background-color: #1e4546; border: 2px solid #1e4546;}"
                                            "QToolButton::menu-indicator {image: none;}"));

    accountButton->setStyleSheet(QLatin1String("QToolButton {background-color: #2e686b; border: none; padding: 0px; border-radius: 3.5%; color: #dddddd;}"
                                               "QToolButton:hover {background-color: #347779; border: 1px solid #347779;}"
                                               "QToolButton:pressed {background-color: #1e4546; border: 2px solid #1e4546;}"
                                               "QToolButton::menu-indicator {image: none;}"));

    downloadButton->setStyleSheet(QLatin1String("QToolButton {background-color: #2e686b; border: none; padding-top: 0px; border-radius: 3.5%;}"
                                                "QToolButton:hover {background-color: #347779; border: 1px solid #347779;}"
                                                "QToolButton:pressed {background-color: #1e4546; border: 2px solid #1e4546;}"
                                                "QToolButton::menu-indicator {image: none;}"));

    assistantButton->setStyleSheet(QLatin1String("QToolButton {background-color: #2e686b; border: none; background-repeat: no-repeat;"
                                                 "background-image: url(:/images/chatPrivateMode.png); background-position: center; border-radius: 3.5%;}"
                                                 "QToolButton:hover {background-image: url(:/images/chatFocused.png); background-color: #347779; border: 1px solid #347779;}"
                                                 "QToolButton:checked {background-image: url(:/images/chatOn.png); background-color: #1e4546; border: 2px solid #1e4546;}"
                                                 "QToolButton::menu-indicator {image: none;}"));

    foundText->setStyleSheet(QLatin1String("QLabel {border: 2px solid #1e4546; background-color: #1e4546; color: #dddddd;"
                                           "border-radius: 3.5%; font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    browserMenu->setDarkTurquoiseTheme();
    smartInput->setDarkTurquoiseTheme();
    smartInput->setSecurityTheme();
    findInput->setDarkTurquoiseTheme();
    syncMenu->setDarkTurquoiseTheme();
    downloadMenu->setDarkTurquoiseTheme();
    reloadButton->setIcon(QIcon(":/private_mode_images/reloadPrivateMode.png"));
    menuButton->setIcon(QIcon(":/private_mode_images/menuPrivateMode.png"));
    accountButton->setIcon(QIcon(":/private_mode_images/accountPrivateMode.png"));

    if (!isDownloading)
        downloadButton->setIcon(QIcon(":/private_mode_images/downloadsPrivateMode.png"));
    ololanTheme = "Dark turquoise";
}

void CustomToolBar::setDarkBrownTheme(bool bookmarBarVisible)
{
    if (bookmarBarVisible)
        setStyleSheet(QLatin1String("QToolBar {background-color: #6b4232; border-bottom: none;}"
                                     "QToolBar::separator {background-color: #6b4232; width: 4px;}"));
    else
        setStyleSheet(QLatin1String("QToolBar {background-color: #6b4232; border-bottom: 1px solid #6b4232;}"
                                     "QToolBar::separator {background-color: #6b4232; width: 4px;}"));

    backButton->setStyleSheet(QLatin1String("QToolButton {background-color: #6b4232; border: none; padding: 0px; border-radius: 3.5%;"
                                            "background-image: url(:/private_mode_images/backwardPrivateMode.png); background-repeat: no-repeat;"
                                            "background-position: center;}"
                                            "QToolButton:hover {background-color: #794a38; border: 1px solid #794a38;}"
                                            "QToolButton:pressed {background-color: #472b21; border: 2px solid #472b21;}"
                                            "QToolButton:!enabled {background-image: url(:/images/backward.png);}"));

    nextButton->setStyleSheet(QLatin1String("QToolButton {background-color: #6b4232; border: none; padding: 0px; border-radius: 3.5%;"
                                            "background-image: url(:/private_mode_images/forwardPrivateMode.png); background-repeat: no-repeat;"
                                            "background-position: center;}"
                                            "QToolButton:hover {background-color: #794a38; border: 1px solid #794a38;}"
                                            "QToolButton:pressed {background-color: #472b21; border: 2px solid #472b21;}"
                                            "QToolButton:!enabled {background-image: url(:/images/forward.png);}"));

    reloadButton->setStyleSheet(QLatin1String("QToolButton {background-color: #6b4232; border: none; padding-top: 0px; border-radius: 3.5%;}"
                                              "QToolButton:hover {background-color: #794a38; border: 1px solid #347779;}"
                                              "QToolButton:pressed {background-color: #472b21; border: 2px solid #472b21;}"));

    menuButton->setStyleSheet(QLatin1String("QToolButton {background-color: #6b4232; border: none; padding: 0px; border-radius: 3.5%;}"
                                            "QToolButton:hover {background-color: #794a38; border: 1px solid #794a38;}"
                                            "QToolButton:pressed {background-color: #472b21; border: 2px solid #472b21;}"
                                            "QToolButton::menu-indicator {image: none;}"));

    accountButton->setStyleSheet(QLatin1String("QToolButton {background-color: #6b4232; border: none; padding: 0px; border-radius: 3.5%; color: #dddddd;}"
                                               "QToolButton:hover {background-color: #794a38; border: 1px solid #794a38;}"
                                               "QToolButton:pressed {background-color: #472b21; border: 2px solid #472b21;}"
                                               "QToolButton::menu-indicator {image: none;}"));

    downloadButton->setStyleSheet(QLatin1String("QToolButton {background-color: #6b4232; border: none; padding-top: 0px; border-radius: 3.5%;}"
                                                "QToolButton:hover {background-color: #794a38; border: 1px solid #794a38;}"
                                                "QToolButton:pressed {background-color: #472b21; border: 2px solid #472b21;}"
                                                "QToolButton::menu-indicator {image: none;}"));

    assistantButton->setStyleSheet(QLatin1String("QToolButton {background-color: #6b4232; border: none; background-repeat: no-repeat;"
                                                 "background-image: url(:/images/chatPrivateMode.png); background-position: center; border-radius: 3.5%;}"
                                                 "QToolButton:hover {background-image: url(:/images/chatFocusedBrown.png); background-color: #794a38; border: 1px solid #794a38;}"
                                                 "QToolButton:checked {background-image: url(:/images/chatOnBrown.png); background-color: #472b21; border: 2px solid #472b21;}"
                                                 "QToolButton::menu-indicator {image: none;}"));

    foundText->setStyleSheet(QLatin1String("QLabel {border: 2px solid #472b21; background-color: #472b21; color: #dddddd;"
                                           "border-radius: 3.5%; font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    browserMenu->setDarkBrownTheme();
    smartInput->setDarkBrownTheme();
    smartInput->setSecurityTheme();
    findInput->setDarkBrownTheme();
    syncMenu->setDarkBrownTheme();
    downloadMenu->setDarkBrownTheme();
    reloadButton->setIcon(QIcon(":/private_mode_images/reloadPrivateMode.png"));
    menuButton->setIcon(QIcon(":/private_mode_images/menuPrivateMode.png"));
    accountButton->setIcon(QIcon(":/private_mode_images/accountPrivateMode.png"));

    if (!isDownloading)
        downloadButton->setIcon(QIcon(":/private_mode_images/downloadsPrivateMode.png"));
    ololanTheme = "Dark brown";
}

void CustomToolBar::setPrivateTheme(bool bookmarBarVisible)
{
    if (bookmarBarVisible)
        setStyleSheet(QLatin1String("QToolBar {background-color: #5e4e6b; border-bottom: none;}"
                                    "QToolBar::separator {background-color: #5e4e6b; width: 4px;}"));
    else
        setStyleSheet(QLatin1String("QToolBar {background-color: #5e4e6b; border-bottom: 1px solid #5e4e6b;}"
                                    "QToolBar::separator {background-color: #5e4e6b; width: 4px;}"));

    backButton->setStyleSheet(QLatin1String("QToolButton {background-color: #5e4e6b; border: none; padding: 0px; border-radius: 3.5%;"
                                            "background-image: url(:/private_mode_images/backwardPrivateMode.png); background-repeat: no-repeat;"
                                            "background-position: center;}"
                                            "QToolButton:hover {background-color: #695878; border: 1px solid #695878;}"
                                            "QToolButton:pressed {background-color: #3c3244; border: 2px solid #3c3244;}"
                                            "QToolButton:!enabled {background-image: url(:/images/backward.png);}"));

    nextButton->setStyleSheet(QLatin1String("QToolButton {background-color: #5e4e6b; border: none; padding: 0px; border-radius: 3.5%;"
                                            "background-image: url(:/private_mode_images/forwardPrivateMode.png); background-repeat: no-repeat;"
                                            "background-position: center;}"
                                            "QToolButton:hover {background-color: #695878; border: 1px solid #695878;}"
                                            "QToolButton:pressed {background-color: #3c3244; border: 2px solid #3c3244;}"
                                            "QToolButton:!enabled {background-image: url(:/images/forward.png);}"));

    reloadButton->setStyleSheet(QLatin1String("QToolButton {background-color: #5e4e6b; border: none; padding-top: 0px; border-radius: 3.5%}"
                                              "QToolButton:hover {background-color: #695878; border: 1px solid #695878;}"
                                              "QToolButton:pressed {background-color: #3c3244; border: 2px solid #3c3244;}"));

    menuButton->setStyleSheet(QLatin1String("QToolButton {background-color: #5e4e6b; border: none; padding: 0px; border-radius: 3.5%;}"
                                            "QToolButton:hover {background-color: #695878; border: 1px solid #695878;}"
                                            "QToolButton:pressed {background-color: #3c3244; border: 2px solid #3c3244;}"
                                            "QToolButton::menu-indicator {image: none;}"));

    accountButton->setStyleSheet(QLatin1String("QToolButton {background-color: #5e4e6b; border: none; padding: 0px; border-radius: 3.5%; color: #dddddd;}"
                                               "QToolButton:hover {background-color: #695878; border: 1px solid #695878;}"
                                               "QToolButton:pressed {background-color: #3c3244; border: 2px solid #3c3244;}"
                                               "QToolButton::menu-indicator {image: none;}"));

    downloadButton->setStyleSheet(QLatin1String("QToolButton {background-color: #5e4e6b; border: none; padding-top: 0px; border-radius: 3.5%;}"
                                                "QToolButton:hover {background-color: #695878; border: 1px solid #695878;}"
                                                "QToolButton:pressed {background-color: #3c3244; border: 2px solid #3c3244;}"
                                                "QToolButton::menu-indicator {image: none;}"));

    assistantButton->setStyleSheet(QLatin1String("QToolButton {background-color: #5e4e6b; border: none; background-repeat: no-repeat;"
                                                 "background-image: url(:/private_mode_images/chatPrivateMode.png); background-position: center; border-radius: 3.5%;}"
                                                 "QToolButton:hover {background-image: url(:/private_mode_images/chatFocusedPrivateMode.png); background-color: #695878; border: 1px solid #695878;}"
                                                 "QToolButton:checked {background-image: url(:/private_mode_images/chatOnPrivateMode.png); background-color: #3c3244; border: 2px solid #3c3244;}"
                                                 "QToolButton::menu-indicator {image: none;}"));

    foundText->setStyleSheet(QLatin1String("QLabel {border: 2px solid #3c3244; background-color: #3c3244; color: #dddddd;"
                                           "border-radius: 3.5%; font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    browserMenu->setPrivateTheme();
    smartInput->setPrivateTheme();
    smartInput->setSecurityTheme();
    findInput->setPrivateTheme();
    syncMenu->setPrivateTheme();
    downloadMenu->setPrivateTheme();
    reloadButton->setIcon(QIcon(":/private_mode_images/reloadPrivateMode.png"));
    menuButton->setIcon(QIcon(":/private_mode_images/menuPrivateMode.png"));
    accountButton->setIcon(QIcon(":/private_mode_images/accountPrivateMode.png"));

    if (!isDownloading)
        downloadButton->setIcon(QIcon(":/private_mode_images/downloadsPrivateMode.png"));
    ololanTheme = "Private";
}

void CustomToolBar::setSearchEngine(const QString searchEngine)
{
    smartInput->setSearchEngine(searchEngine);
}

void CustomToolBar::setEnableAssistant(bool value = true)
{
    //
}

void CustomToolBar::updateFindInputVisibility()
{
    if (findInput->isVisible())
    {
        inSeparator->setVisible(false);
        fTextSeparator->setVisible(false);
        foundTextAction->setVisible(false);
        findInput->hideFindInput();
        foundText->setText("0");
    }
    else
    {
        inSeparator->setVisible(true);
        fTextSeparator->setVisible(true);
        foundTextAction->setVisible(true);
        findInput->showFindInput();
    }
}

void CustomToolBar::updateFoundText(const QWebEngineFindTextResult &result)
{
    QFontMetrics fontMetrics(foundText->fontMetrics());
    foundText->setText(fontMetrics.elidedText(QString::number(result.numberOfMatches()), Qt::ElideRight, foundText->width()));
    foundText->setToolTip(QString::number(result.numberOfMatches()));
}

void CustomToolBar::addBookmarkItems(QList<QAction*> list)
{
    for (int i = 0; i < list.count(); i++)
    {
        QPushButton *item = new QPushButton(this);
        item->setFixedHeight(26);
        item->setMaximumWidth(128);
        item->setIcon(list.at(i)->icon());
        item->setIconSize(QSize(13, 13));
        QFontMetrics fontMetrics(item->fontMetrics());
        item->setText(fontMetrics.elidedText(list.at(i)->text(), Qt::ElideRight, item->width()-16));
        QStringList value = list.at(i)->toolTip().split("*x-z-x*");
        item->setToolTip(value.first() + "\n" + value.last());
        item->setObjectName(list.at(i)->data().toString());

        setItemTheme(item);
        addWidget(item);
        if (i < (list.count() - 1))
            addSeparator();

        connect(item, &QPushButton::clicked, this, &CustomToolBar::openBookmark);
    }
}

void CustomToolBar::addBookmark(QAction *item)
{
    QPushButton *bmkItem = new QPushButton(this);
    bmkItem->setFixedHeight(26);
    bmkItem->setMaximumWidth(128);
    bmkItem->setIcon(item->icon());
    bmkItem->setIconSize(QSize(13, 13));
    QFontMetrics fontMetrics(bmkItem->fontMetrics());
    bmkItem->setText(fontMetrics.elidedText(item->text(), Qt::ElideRight, bmkItem->width()-16));
    QStringList value = item->toolTip().split("*x-z-x*");
    bmkItem->setToolTip(value.first() + "\n" + value.last());
    bmkItem->setObjectName(item->data().toString());

    setItemTheme(bmkItem);
    addWidget(bmkItem);
    addSeparator();
    connect(bmkItem, &QPushButton::clicked, this, &CustomToolBar::openBookmark);
}

void CustomToolBar::updateItem(const QString index, const QString newTitle, const QString newUrl)
{
    QList<QObject*> list = children();
    for (int i = 0; i < list.count(); i++)
    {
        QPushButton *item = static_cast<QPushButton*>(list.at(i));
        if (item->objectName().compare(index) == 0)
        {
            item->setText(newTitle);
            item->setToolTip(newTitle + "\n" + newUrl);
            break;
        }
    }
}

void CustomToolBar::setItemTheme(QPushButton *item)
{
    if (ololanTheme.compare("Light gray") == 0)
        item->setStyleSheet(QLatin1String("QPushButton {background-color: #f9f9fb; border: 1px solid #f9f9fb; padding: 3px;"
                                          "border-radius: 4px; color: #656565; font-family: gadugi; font-size: 11px;}"
                                          "QPushButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                          "QPushButton:pressed {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"));

    else if (ololanTheme.compare("Light turquoise") == 0)
        item->setStyleSheet(QLatin1String("QPushButton {background-color: #f9f9fb; border: 1px solid #f9f9fb; padding: 3px;"
                                          "border-radius: 4px; color: #656565; font-family: gadugi; font-size: 11px;}"
                                          "QPushButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                          "QPushButton:pressed {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"));

    else if (ololanTheme.compare("Light brown") == 0)
        item->setStyleSheet(QLatin1String("QPushButton {background-color: #f9f9fb; border: 1px solid #f9f9fb; padding: 3px;"
                                          "border-radius: 3px; color: #656565; font-family: gadugi; font-size: 11px;}"
                                          "QPushButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                          "QPushButton:pressed {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"));

    else if (ololanTheme.compare("Dark gray") == 0)
        item->setStyleSheet(QLatin1String("QPushButton {background-color: #6b6b6b; border: 1px solid #6b6b6b; padding: 3px;"
                                          "border-radius: 4px; color: #dddddd; font-family: gadugi; font-size: 11px;}"
                                          "QPushButton:hover {background-color: #777778; border: 1px solid #777778;}"
                                          "QPushButton:pressed {background-color: #434344; border: 1px solid #434344;}"));

    else if (ololanTheme.compare("Dark turquoise") == 0)
        item->setStyleSheet(QLatin1String("QPushButton {background-color: #2e686b; border: 1px solid #2e686b; padding: 3px;"
                                          "border-radius: 4px; color: #dddddd; font-family: gadugi; font-size: 11px;}"
                                          "QPushButton:hover {background-color: #347779; border: 1px solid #347779;}"
                                          "QPushButton:pressed {background-color: #1e4546; border: 1px solid #1e4546;}"));

    else if (ololanTheme.compare("Dark brown") == 0)
        item->setStyleSheet(QLatin1String("QPushButton {background-color: #6b4232; border: 1px solid #6b4232; padding: 3px;"
                                          "border-radius: 4px; color: #dddddd; font-family: gadugi; font-size: 11px;}"
                                          "QPushButton:hover {background-color: #794a38; border: 1px solid #794a38;}"
                                          "QPushButton:pressed {background-color: #472b21; border: 1px solid #472b21;}"));

    else if (ololanTheme.compare("Private") == 0)
        item->setStyleSheet(QLatin1String("QPushButton {background-color: #5e4e6b; border: 1px solid #5e4e6b; padding: 3px;"
                                          "border-radius: 4px; color: #dddddd; font-family: gadugi; font-size: 11px;}"
                                          "QPushButton:hover {background-color: #695878; border: 1px solid #695878;}"
                                          "QPushButton:pressed {background-color: #3c3244; border: 1px solid #3c3244;}"));
}

void CustomToolBar::setOlolanTheme(QString themeColor)
{
    ololanTheme = themeColor;
}

void CustomToolBar::deleteItem(const QString bmkIndex)
{
    QList<QObject*> list = children();
    for (int i = 0; i < list.count(); i++)
    {
        QPushButton *item = static_cast<QPushButton*>(list.at(i));
        if (item->objectName().compare(bmkIndex) == 0)
        {
            removeAction(actionAt(item->pos()));
            item->deleteLater();
            break;
        }
    }
}

void CustomToolBar::openBookmark()
{
    QPushButton *bookmark = qobject_cast<QPushButton*>(sender());
    friendBar->getSmartBox()->setText(bookmark->toolTip().split("\n").last());
    friendBar->getSmartBox()->validateInput();

    if (bookmark->toolTip().split("\n").last().compare("ololan://home") == 0)
    {
        friendBar->getSmartBox()->clear();
        friendBar->getSmartBox()->setFocus();
    }
}

void CustomToolBar::openInTab()
{
    OTabPage *tabPage = static_cast<OTabPage*>(parentWidget()->parentWidget()->parentWidget());
    tabPage->openInTab(selectedBookmark->toolTip().split("\n").last());
    selectedBookmark = nullptr;
}

void CustomToolBar::openInWindow()
{
    OTabPage *tabPage = static_cast<OTabPage*>(parentWidget()->parentWidget()->parentWidget());
    OAppCore *appCore = static_cast<OAppCore*>(tabPage->getParentWindow()->getParentApplication());
    appCore->openInWindow(selectedBookmark->toolTip().split("\n").last());
    selectedBookmark = nullptr;
}

void CustomToolBar::openInPrivateWindow()
{
    OTabPage *tabPage = static_cast<OTabPage*>(parentWidget()->parentWidget()->parentWidget());
    OAppCore *appCore = static_cast<OAppCore*>(tabPage->getParentWindow()->getParentApplication());
    appCore->openInPrivateWindow(selectedBookmark->toolTip().split("\n").last());
    selectedBookmark = nullptr;
}

void CustomToolBar::editBookmark()
{
    OTabPage *tabPage = static_cast<OTabPage*>(parentWidget()->parentWidget()->parentWidget());
    tabPage->editBookmark(selectedBookmark->objectName());
}

void CustomToolBar::deleteBookmark()
{
    OTabPage *tabPage = static_cast<OTabPage*>(parentWidget()->parentWidget()->parentWidget());
    OAppCore *appCore = static_cast<OAppCore*>(tabPage->getParentWindow()->getParentApplication());
    appCore->getBookmarksManager()->deleteBmkFromBar(selectedBookmark->toolTip().split("\n").last(), QString("Bookmark bar"),
                                                     selectedBookmark->objectName());
}

void CustomToolBar::contextMenuEvent(QContextMenuEvent *event)
{
    if (isBookmarkBar)
    {
        selectedBookmark = static_cast<QPushButton*>(childAt(event->pos()));
        if (selectedBookmark != nullptr)
            itemMenu->exec(event->globalPos());
    }
    QToolBar::contextMenuEvent(event);
}

void CustomToolBar::setControlBarState(bool state)
{
    isBookmarkBar = state;
}

void CustomToolBar::setupMenu()
{
    itemMenu = new QMenu(this);
    itemMenu->setFixedWidth(300);

    inNewTab = new QAction("Open in new tab", itemMenu);
    inWindow = new QAction("Open in new window", itemMenu);
    inPrivate = new QAction("Open in private window", itemMenu);
    edit = new QAction("Edit", itemMenu);
    remove = new QAction("Delete", itemMenu);

    connect(inNewTab, &QAction::triggered, this, &CustomToolBar::openInTab);
    connect(inWindow, &QAction::triggered, this, &CustomToolBar::openInWindow);
    connect(inPrivate, &QAction::triggered, this, &CustomToolBar::openInPrivateWindow);
    connect(edit, &QAction::triggered, this, &CustomToolBar::editBookmark);
    connect(remove, &QAction::triggered, this, &CustomToolBar::deleteBookmark);

    itemMenu->addAction(inNewTab);
    itemMenu->addAction(inWindow);
    itemMenu->addAction(inPrivate);
    itemMenu->addSeparator();
    itemMenu->addAction(edit);
    itemMenu->addAction(remove);
}

void CustomToolBar::setMenuTheme()
{
    if (ololanTheme.compare("Light gray") == 0)
    {
        itemMenu->setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"
                                              "QMenu::item {color: #606060; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                              "QMenu::item:selected {background-color: #efeff1; border-top: 1px solid #efeff1; border-bottom: 1px solid #efeff1;}"
                                              "QMenu::item:!enabled {color: #bbbbbb;}"
                                              "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                              "QMenu::separator {background-color: #efeff1; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                              "QMenu::icon {margin-left: 20px;}"));

        inNewTab->setIcon(QIcon(":/images/newTab.png"));
        inWindow->setIcon(QIcon(":/images/newWindow.png"));
        inPrivate->setIcon(QIcon(":/images/private.png"));
    }

    else if (ololanTheme.compare("Light turquoise") == 0)
    {
        itemMenu->setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"
                                              "QMenu::item {color: #606060; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                              "QMenu::item:selected {background-color: #69e8f1; border-top: 1px solid #69e8f1; border-bottom: 1px solid #69e8f1;}"
                                              "QMenu::item:!enabled {color: #bbbbbb;}"
                                              "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                              "QMenu::separator {background-color: #efeff1; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                              "QMenu::icon {margin-left: 20px;}"));

        inNewTab->setIcon(QIcon(":/images/newTab.png"));
        inWindow->setIcon(QIcon(":/images/newWindow.png"));
        inPrivate->setIcon(QIcon(":/images/private.png"));
    }

    else if (ololanTheme.compare("Light brown") == 0)
    {
        itemMenu->setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"
                                              "QMenu::item {color: #606060; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                              "QMenu::item:selected {background-color: #ffa477; border-top: 1px solid #ffa477; border-bottom: 1px solid #ffa477;}"
                                              "QMenu::item:!enabled {color: #bbbbbb;}"
                                              "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                              "QMenu::separator {background-color: #efeff1; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                              "QMenu::icon {margin-left: 20px;}"));

        inNewTab->setIcon(QIcon(":/images/newTab.png"));
        inWindow->setIcon(QIcon(":/images/newWindow.png"));
        inPrivate->setIcon(QIcon(":/images/private.png"));
    }

    else if (ololanTheme.compare("Dark gray") == 0)
    {
        itemMenu->setStyleSheet(QLatin1String("QMenu {background-color: #434344; padding: 0px; border: 1px solid #434344;}"
                                              "QMenu::item {color: #dddddd; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                              "QMenu::item:selected {background-color: #6b6b6b; border-top: 1px solid #6b6b6b; border-bottom: 1px solid #6b6b6b;}"
                                              "QMenu::item:!enabled {color: #888888;}"
                                              "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                              "QMenu::separator {background-color: #6b6b6b; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                              "QMenu::icon {margin-left: 20px;}"));

        inNewTab->setIcon(QIcon(":/private_mode_images/newTabPrivateMode.png"));
        inWindow->setIcon(QIcon(":/private_mode_images/newWindowPrivateMode.png"));
        inPrivate->setIcon(QIcon(":/private_mode_images/privatePrivateMode.png"));
    }

    else if (ololanTheme.compare("Dark turquoise") == 0)
    {
        itemMenu->setStyleSheet(QLatin1String("QMenu {background-color: #1e4446; padding: 0px; border: 1px solid #1e4446;}"
                                              "QMenu::item {color: #dddddd; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                              "QMenu::item:selected {background-color: #2e686b; border-top: 1px solid #2e686b; border-bottom: 1px solid #2e686b;}"
                                              "QMenu::item:!enabled {color: #888888;}"
                                              "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                              "QMenu::separator {background-color: #2e686b; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                              "QMenu::icon {margin-left: 20px;}"));

        inNewTab->setIcon(QIcon(":/private_mode_images/newTabPrivateMode.png"));
        inWindow->setIcon(QIcon(":/private_mode_images/newWindowPrivateMode.png"));
        inPrivate->setIcon(QIcon(":/private_mode_images/privatePrivateMode.png"));
    }

    else if (ololanTheme.compare("Dark brown") == 0)
    {
        itemMenu->setStyleSheet(QLatin1String("QMenu {background-color: #462b21; padding: 0px; border: 1px solid #462b21;}"
                                              "QMenu::item {color: #dddddd; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                              "QMenu::item:selected {background-color: #6b4132; border-top: 1px solid #6b4132; border-bottom: 1px solid #6b4132;}"
                                              "QMenu::item:!enabled {color: #888888;}"
                                              "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                              "QMenu::separator {background-color: #6b4132; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                              "QMenu::icon {margin-left: 20px;}"));

        inNewTab->setIcon(QIcon(":/private_mode_images/newTabPrivateMode.png"));
        inWindow->setIcon(QIcon(":/private_mode_images/newWindowPrivateMode.png"));
        inPrivate->setIcon(QIcon(":/private_mode_images/privatePrivateMode.png"));
    }

    else if (ololanTheme.compare("Private") == 0)
    {
        itemMenu->setStyleSheet(QLatin1String("QMenu {background-color: #3c3244; padding: 0px; border: 1px solid #3c3244;}"
                                              "QMenu::item {color: #dddddd; height: 22px; padding-top: 6px; padding-bottom: 6px; padding-left: 16px; padding-right: 5px;}"
                                              "QMenu::item:selected {background-color: #5e4e6b; border-top: 1px solid #5e4e6b; border-bottom: 1px solid #5e4e6b;}"
                                              "QMenu::item:!enabled {color: #888888;}"
                                              "QMenu::item:!enabled:selected {background-color: transparent; border-top: transparent; border-bottom: transparent;}"
                                              "QMenu::separator {background-color: #5e4e6b; height: 1px; margin-top: 1px; margin-bottom: 1px;}"
                                              "QMenu::icon {margin-left: 20px;}"));

        inNewTab->setIcon(QIcon(":/private_mode_images/newTabPrivateMode.png"));
        inWindow->setIcon(QIcon(":/private_mode_images/newWindowPrivateMode.png"));
        inPrivate->setIcon(QIcon(":/private_mode_images/privatePrivateMode.png"));
    }
}

void CustomToolBar::warnForUpdates()
{
    updatesAction->setVisible(true);
}
