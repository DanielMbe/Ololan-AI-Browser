#include "otabbar.h"
#include "otabpage.h"
#include "owebview.h"
#include "customwindow.h"
#include "oappcore.h"
#include "osmartinput.h"
#include "oinputcompleter.h"
#include "customtoolbar.h"
#include <QSpacerItem>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QStyleOption>
#include <QPaintEvent>
#include <QPainter>
#include <QMimeData>
#include <QDrag>
#include <QVariantAnimation>
#include <QTimer>
#include <QPixmap>
#include <QVariant>

//tab class definition

OTab::OTab(QWidget *parent)
    :QWidget(parent)
{
    setMinimumSize(39, 38);
    setMaximumSize(230, 38);

    tabIndex = -1;
    isLoading = false;
    isSelectedTab = false;

    tabText = new QLabel(this);
    tabText->setFixedHeight(24);

    tabText->setScaledContents(true);
    tabText->setToolTipDuration(1800000);
    tabText->setToolTip(tr("New Tab"));

    tabIconButton = new QToolButton();
    tabIconButton->setFixedSize(1, 16);
    tabIconButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none;}"
                                               "QToolButton:pressed {padding: 0px;}"));
    tabCloseButton = new QToolButton(this);
    tabCloseButton->setFixedSize(16, 16);

    animationGroup = new QSequentialAnimationGroup(this);
    connect(animationGroup, &QSequentialAnimationGroup::finished, this, &OTab::restartLoading);

    for (int i = 0; i < 4; i++)
    {
        QVariantAnimation *animation = new QVariantAnimation(this);
        connect(animation, &QVariantAnimation::valueChanged, this, &OTab::rotateLoadingIcon);
        animation->setEasingCurve(QEasingCurve::Linear);
        animation->setDuration(650);
        animation->setStartValue(0.00);
        animation->setEndValue(359.00);
        animationGroup->addAnimation(animation);
    }

    QHBoxLayout *tabLayout = new QHBoxLayout(this);
    tabLayout->setSpacing(5);
    tabLayout->setContentsMargins(0, 0, 0, 0);
    tabLayout->addSpacing(8);
    tabLayout->addWidget(tabIconButton);
    tabLayout->addWidget(tabText);
    tabLayout->addWidget(tabCloseButton);
    tabLayout->addSpacing(8);

    connect(tabCloseButton, &QToolButton::clicked, this, &OTab::closeTab);
    connect(tabIconButton, &QToolButton::clicked, this, &OTab::tabIconClick);
}

void OTab::hideContent()
{
    tabText->hide();
    tabIconButton->hide();
    tabCloseButton->hide();
}

void OTab::showContent()
{
    tabText->show();
    tabIconButton->show();
    tabCloseButton->show();
}

QIcon OTab::getTabIcon()
{
    return tabIconButton->icon();
}

QString OTab::getTabText()
{
    return tabText->toolTip();
}

void OTab::setTabIcon(const QIcon &icon)
{
    if(tabIconButton->size() != QSize(16, 16))
        tabIconButton->setFixedWidth(16);
    tabIconButton->setIcon(icon);
}

QToolButton* OTab::getTabCloseButton()
{
    return tabCloseButton;
}

void OTab::setTabText(const QString &tabTextString)
{
    QString historyUrl = QLatin1String("qrc:/web_applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web_applications/html/settingsView.html");
    QString newTabUrl = QLatin1String("qrc:/web_applications/html/newtabView.html");
    QString privateTabUrl = QLatin1String("qrc:/web_applications/html/privatetabView.html");
    QString textToSet = tabTextString;

    if (tabTextString.compare(historyUrl) == 0)
        textToSet = "ololan://history";

    if (tabTextString.compare(bookmarksUrl) == 0)
        textToSet = "ololan://bookmarks";

    if (tabTextString.compare(downloadsUrl) == 0)
        textToSet = "ololan://downloads";

    if (tabTextString.compare(settingsUrl) == 0)
        textToSet = "ololan://settings";

    if ((tabTextString.compare(newTabUrl) == 0) || (tabTextString.compare(privateTabUrl) == 0))
        textToSet = "ololan://home";

    QFontMetrics fontMetrics(tabText->fontMetrics());
    tabText->setText(fontMetrics.elidedText(textToSet, Qt::ElideRight, tabText->width()));
    tabText->setToolTip(textToSet);

    if (isSelectedTab)
    {
        OTabPage *tabpage = qobject_cast<OTabPage*>(parent()->parent());
        QString appTitle = tabTextString + " - Ololan Browser";
        LPCTSTR title = (LPCTSTR)appTitle.utf16();
        SetWindowText(tabpage->getParentWindow()->getCustomWinHandle(), title);
    }
}

void OTab::closeTab()
{
    emit tabIndexSignal(tabIndex);
}

void OTab::tabIconClick()
{
    emit tabClicked(tabIndex);
}

void OTab::setTabIndex(int index)
{
    tabIndex = index;
}

int OTab::getTabIndex()
{
    return tabIndex;
}

void OTab::setSelectedStyle(QString themeColor)
{
    isSelectedTab = true;
    tabThemeColor = themeColor;
    if (themeColor.compare("Light gray") == 0)
        setLightGrayTheme(true);
    else if (themeColor.compare("Light turquoise") == 0)
        setLightTurquoiseTheme(true);
    else if (themeColor.compare("Light brown") == 0)
        setLightBrownTheme(true);
    else if (themeColor.compare("Dark gray") == 0)
        setDarkGrayTheme(true);
    else if (themeColor.compare("Dark turquoise") == 0)
        setDarkTurquoiseTheme(true);
    else if (themeColor.compare("Dark brown") == 0)
        setDarkBrownTheme(true);
    else if (themeColor.compare("Private") == 0)
        setPrivateTheme(true);

    OTabPage *tabpage = qobject_cast<OTabPage*>(parent()->parent());
    QString appTitle = getTabText() + " - Ololan Browser";
    LPCTSTR title = (LPCTSTR)appTitle.utf16();
    SetWindowText(tabpage->getParentWindow()->getCustomWinHandle(), title);
}

void OTab::setDeselectedStyle(QString themeColor)
{
    isSelectedTab = false;
    tabThemeColor = themeColor;
    if (themeColor.compare("Light gray") == 0)
        setLightGrayTheme(false);
    else if (themeColor.compare("Light turquoise") == 0)
        setLightTurquoiseTheme(false);
    else if (themeColor.compare("Light brown") == 0)
        setLightBrownTheme(false);
    else if (themeColor.compare("Dark gray") == 0)
        setDarkGrayTheme(false);
    else if (themeColor.compare("Dark turquoise") == 0)
        setDarkTurquoiseTheme(false);
    else if (themeColor.compare("Dark brown") == 0)
        setDarkBrownTheme(false);
    else if (themeColor.compare("Private") == 0)
        setPrivateTheme(false);
}

void OTab::setLightGrayTheme(bool isSelected)
{
    tabText->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #353535;"
                                         "font-family: gadugi; font-size: 12px; padding-bottom: 1px;}"));

    tabCloseButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/images/closeOne.png); background-position: center;}"
                                                "QToolButton:hover {background-image: url(:/images/closeTwo.png);}"));
    if (isSelected)
        setStyleSheet(QLatin1String("QWidget {background-color: #e1e1e3; border: 2px solid #56bcc5; border-radius: 6%;}"));
    else
        setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; border: 1px solid #ffffff; border-right: 2px solid #d1d1d3; border-radius: 0px;}"
                                    "QWidget:hover {background-color: #f0f0f2; border: 1px solid #f0f0f2; border-radius: 6%;}"));
}

void OTab::setLightTurquoiseTheme(bool isSelected)
{
    tabText->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #353535;"
                                         "font-family: gadugi; font-size: 12px; padding-bottom: 1px;}"));

    tabCloseButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/images/closeOne.png); background-position: center;}"
                                                "QToolButton:hover {background-image: url(:/images/closeTwo.png);}"));
    if (isSelected)
        setStyleSheet(QLatin1String("QWidget {background-color: #c0ebee; border: 2px solid #56bcc5; border-radius: 6%;}"));
    else
        setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; border: 1px solid #ffffff; border-right: 2px solid #56bcc5; border-radius: 0px;}"
                                    "QWidget:hover {background-color: #d0fcff; border: 1px solid #d0fcff;}"));
}

void OTab::setLightBrownTheme(bool isSelected)
{
    tabText->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #353535;"
                                         "font-family: gadugi; font-size: 12px; padding-bottom: 1px;}"));

    tabCloseButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/images/closeOne.png); background-position: center;}"
                                                "QToolButton:hover {background-image: url(:/images/closeTwo.png);}"));
    if (isSelected)
        setStyleSheet(QLatin1String("QWidget {background-color: #ffd0ae; border: 2px solid #ffa477; border-radius: 6%;}"));
    else
        setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; border: 1px solid #ffffff; border-right: 2px solid #ffa477; border-radius: 0px;}"
                                    "QWidget:hover {background-color: #ffe2ce; border: 1px solid #ffe2ce;}"));
}

void OTab::setDarkGrayTheme(bool isSelected)
{
    tabText->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                         "font-family: gadugi; font-size: 12px; padding-bottom: 1px;}"));

    tabCloseButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/private_mode_images/closeOnePrivateMode.png); background-position: center;}"
                                                "QToolButton:hover {background-image: url(:/private_mode_images/closeTwoPrivateMode.png);}"));
    if (isSelected)
        setStyleSheet(QLatin1String("QWidget {background-color: #6b6b6b; border: 1px solid #434344; border-radius: 6%;}"));
    else
        setStyleSheet(QLatin1String("QWidget {background-color: #434344; border-right: 2px solid #dddddd; border-radius: 0px;}"
                                    "QWidget:hover {background-color: #59595a; border: 1px solid #434344;}"));
}

void OTab::setDarkTurquoiseTheme(bool isSelected)
{
    tabText->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                         "font-family: gadugi; font-size: 12px; padding-bottom: 1px;}"));

    tabCloseButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/private_mode_images/closeOnePrivateMode.png); background-position: center;}"
                                                "QToolButton:hover {background-image: url(:/private_mode_images/closeTwoPrivateMode.png);}"));
    if (isSelected)
        setStyleSheet(QLatin1String("QWidget {background-color: #2e686b; border: 1px solid #1e4446; border-radius: 6%;}"));
    else
        setStyleSheet(QLatin1String("QWidget {background-color: #1e4446; border-right: 2px solid #dddddd; border-radius: 0px;}"
                                    "QWidget:hover {background-color: #275a5b; border: 1px solid #1e4446;}"));
}

void OTab::setDarkBrownTheme(bool isSelected)
{
    tabText->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                         "font-family: gadugi; font-size: 12px; padding-bottom: 1px;}"));

    tabCloseButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/private_mode_images/closeOnePrivateMode.png); background-position: center;}"
                                                "QToolButton:hover {background-image: url(:/private_mode_images/closeTwoPrivateMode.png);}"));
    if (isSelected)
        setStyleSheet(QLatin1String("QWidget {background-color: #6b4232; border: 1px solid #462c21; border-radius: 6%;}"));
    else
        setStyleSheet(QLatin1String("QWidget {background-color: #462c21; border-right: 2px solid #dddddd; border-radius: 0px;}"
                                    "QWidget:hover {background-color: #5b382b; border: 1px solid #462c21;}"));
}

void OTab::setPrivateTheme(bool isSelected)
{
    tabText->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                         "font-family: gadugi; font-size: 12px; padding-bottom: 1px;}"));

    tabCloseButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/private_mode_images/closeOnePrivateMode.png); background-position: center;}"
                                                "QToolButton:hover {background-image: url(:/private_mode_images/closeTwoPrivateMode.png);}"));
    if (isSelected)
        setStyleSheet(QLatin1String("QWidget {background-color: #5e4e6b; border: 1px solid #3c3244;  border-radius: 6%;}"));
    else
        setStyleSheet(QLatin1String("QWidget {background-color: #3c3244; border-right: 2px solid #dddddd; border-radius: 0px;}"
                                    "QWidget:hover {background-color: #50425b; border: 1px solid #3c3244;}"));
}

void OTab::paintEvent(QPaintEvent *event)
{
    event->accept();
    QStyleOption option;
    option.initFrom(this);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
}

void OTab::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        emit tabClicked(tabIndex);
    qobject_cast<OTabBar*>(parentWidget())->handleCompleter();
}

void OTab::startLoadingAnimation()
{
    if (!isLoading)
    {
        isLoading = true;
        animationGroup->start();
    }
}

void OTab::stopLoadingAnimation(bool state)
{
    if (isLoading)
        isLoading = false;

    animationGroup->stop();
    OWebView *webView = qobject_cast<OWebView*>(sender());

    if (state)
        setTabIcon(webView->icon());
    else
    {
        if (tabThemeColor.compare("Light gray") == 0)
            setTabIcon(QIcon(":/images/webpage.png"));
        else if (tabThemeColor.compare("Light turquoise") == 0)
            setTabIcon(QIcon(":/images/webpageTyping.png"));
        else if (tabThemeColor.compare("Light brown") == 0)
            setTabIcon(QIcon(":/images/webpageTypingBrown.png"));
        else if (tabThemeColor.compare("Dark gray") == 0)
            setTabIcon(QIcon(":/private_mode_images/webpagePrivateMode.png"));
        else if (tabThemeColor.compare("Dark turquoise") == 0)
            setTabIcon(QIcon(":/private_mode_images/webpagePrivateMode.png"));
        else if (tabThemeColor.compare("Dark brown") == 0)
            setTabIcon(QIcon(":/private_mode_images/webpagePrivateMode.png"));
        else if (tabThemeColor.compare("Private") == 0)
            setTabIcon(QIcon(":/private_mode_images/webpagePrivateMode.png"));
    }
}

void OTab::restartLoadingAnimation(int value)
{
    if ((value < 100) && !isLoading)
        startLoadingAnimation();
}

void OTab::rotateLoadingIcon(QVariant value)
{   
    QPixmap rotatedPixmap(16, 16);
    rotatedPixmap.fill(QColor::fromRgb(0, 0, 0, 0));
    QPixmap originalPixmap(16, 16);

    if (tabThemeColor.compare("Light gray") == 0)
        originalPixmap.load(":/images/loading.png");
    else if (tabThemeColor.compare("Light turquoise") == 0)
    {
        if (isSelectedTab)
            originalPixmap.load(":/images/loading.png");
        else
            originalPixmap.load(":/images/loadingGray.png");
    }
    else if (tabThemeColor.compare("Light brown") == 0)
    {
        if (isSelectedTab)
            originalPixmap.load(":/images/loadingBrown.png");
        else
            originalPixmap.load(":/images/loadingGray.png");
    }
    else if (tabThemeColor.compare("Dark gray") == 0)
        originalPixmap.load(":/private_mode_images/loadingPrivateMode.png");
    else if (tabThemeColor.compare("Dark turquoise") == 0)
        originalPixmap.load(":/private_mode_images/loadingPrivateMode.png");
    else if (tabThemeColor.compare("Dark brown") == 0)
        originalPixmap.load(":/private_mode_images/loadingPrivateMode.png");
    else if (tabThemeColor.compare("Private") == 0)
        originalPixmap.load(":/private_mode_images/loadingPrivateMode.png");

    QPainter pixmapPainter(&rotatedPixmap);
    pixmapPainter.setRenderHint(QPainter::SmoothPixmapTransform);
    pixmapPainter.translate(8, 8);
    pixmapPainter.rotate(value.toReal());
    pixmapPainter.translate(-8, -8);
    pixmapPainter.drawPixmap(0, 0, originalPixmap);
    pixmapPainter.end();

    setTabIcon(QIcon(rotatedPixmap));
}

void OTab::restartLoading()
{
    if (isLoading)
    {
        isLoading = false;
        startLoadingAnimation();
    }
}

//tab bar class definition

OTabBar::OTabBar(QWidget *parent)
    :QWidget(parent)
{
    setAcceptDrops(true);
    setMinimumSize(230, 44);

    activeTab = 0;
    tabsWidth = 230;
    distanceWidth = 0;
    oldPosition = QPoint(0, 0);
    clickPosition = QPoint(0, 0);
    windowMoved = false;
    isGrabbed = false;
    moveDirection = TabMoveDirection::NotMoved;
    tabToRemove = nullptr;
    tabToMove = nullptr;

    setupWidget();

    connect(addButton, &CustomToolButton::clicked, this, &OTabBar::addButtonClicked);
    connect(nextTabButton, &CustomToolButton::clicked, this, &OTabBar::selectNextTab);
    connect(closeButton, &CustomToolButton::clicked, this, &OTabBar::closeButtonClicked);
    connect(maxMinButton, &CustomToolButton::clicked, this, &OTabBar::maxMinButtonClicked);
    connect(maxMinButton, &CustomToolButton::clicked, this, &OTabBar::handleCompleter);
    connect(previousTabButton, &CustomToolButton::clicked, this, &OTabBar::selectPreviousTab);
    connect(minimizeButton, &CustomToolButton::clicked, this, &OTabBar::minimizeButtonClicked);
}

void OTabBar::handleCompleter()
{
    OTabPage *tabPage = qobject_cast<OTabPage*>(parentWidget());
    OSmartInput *smartInput = tabPage->getStackPage()->currentWidget()->findChild<CustomToolBar*>()->getSmartBox();

    if (smartInput->getCompleter()->isVisible())
        smartInput->getCompleter()->hide();
}

void OTabBar::setupWidget()
{
    addButton = new CustomToolButton(this);
    addButton->setFixedSize(QSize(30, 27));

    minimizeButton = new CustomToolButton(this);
    minimizeButton->setFixedSize(QSize(45, 27));

    maxMinButton = new CustomToolButton(this);
    maxMinButton->setObjectName(QLatin1String("maxMinButton"));
    maxMinButton->setFixedSize(QSize(45, 27));

    closeButton = new CustomToolButton(this);
    closeButton->setFixedSize(QSize(45, 27));

    nextTabButton = new CustomToolButton(this);
    nextTabButton->setFixedSize(QSize(32, 35));
    nextTabButton->setHidden(true);

    previousTabButton = new CustomToolButton(this);
    previousTabButton->setFixedSize(QSize(32, 35));
    previousTabButton->setHidden(true);

    tabsLayout = new QHBoxLayout();
    tabsLayout->setSizeConstraint(QHBoxLayout::SetFixedSize);
    tabsLayout->setContentsMargins(2, 0, 0, 0);
    tabsLayout->setSpacing(2);

    QHBoxLayout *captionLayout = new QHBoxLayout(this);
    captionLayout->setContentsMargins(0, 0, 0, 0);
    captionLayout->setSpacing(0);
    captionLayout->addWidget(previousTabButton);
    captionLayout->addSpacing(1);
    captionLayout->addLayout(tabsLayout);
    captionLayout->addSpacing(1);
    captionLayout->addWidget(nextTabButton);
    captionLayout->addSpacing(3);
    captionLayout->addWidget(addButton);
    captionLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));
    captionLayout->addWidget(minimizeButton, 0, Qt::AlignTop);
    captionLayout->addWidget(maxMinButton, 0, Qt::AlignTop);
    captionLayout->addWidget(closeButton, 0, Qt::AlignTop);
}

int OTabBar::currentTab()
{
    return activeTab;
}

void OTabBar::setCurrentTab(const int index)
{
    if (count() <= index)
        return;

    activeTab = index;
    setUpdatesEnabled(false);
    for (int i = 0; i < count(); i++)
    {
        if (i == index)
            getTabAt(i)->setSelectedStyle(tabBarThemeColor);
        else
            getTabAt(i)->setDeselectedStyle(tabBarThemeColor);
    }
    setUpdatesEnabled(true);
    emit currentChanged(index);
}

void OTabBar::addTab(QIcon icon, QString tabTitle)
{
    bool resized = false;

    if ((count() > 0) && (tabsWidth >= 45))
    {
        resized = resizeOnAdding();
        if (!resized)
            return;
    }
    else if (tabsWidth < 45)
        hideTab();

    OTab *tab = new OTab(this);
    tab->setTabIcon(icon);
    tab->setTabText(tabTitle);
    tabsList.append(tab);
    tab->setTabIndex(tabsList.indexOf(tab));

    if (tab->getTabIndex() == -1)
        tab->setTabIndex(count()-1);

    connect(tab, &OTab::tabIndexSignal, this, &OTabBar::tabCloseRequested);
    connect(tab, &OTab::tabClicked, this, &OTabBar::setCurrentTab);
    tabsLayout->addWidget(tab);
    addTabAnimation(tab);
}

void OTabBar::grabTab(QIcon icon, QString tabTitle)
{
    bool resized = false;

    if ((count() > 0) && (tabsWidth >= 45))
    {
        resized = resizeOnGrabbing();
        if (!resized)
            return;
    }
    else if (tabsWidth < 45)
        hideTab(true);

    OTab *tab = new OTab(this);
    tab->setTabIcon(icon);
    tab->setTabText(tabTitle);
    tabsList.append(tab);
    tab->setTabIndex(tabsList.indexOf(tab));
    tab->setFixedWidth(tabsWidth);
    tab->setSelectedStyle(tabBarThemeColor);

    if (tab->getTabIndex() == -1)
        tab->setTabIndex(count()-1);

    connect(tab, &OTab::tabIndexSignal, this, &OTabBar::tabCloseRequested);
    connect(tab, &OTab::tabClicked, this, &OTabBar::setCurrentTab);
    tabsLayout->addWidget(tab);
}

void OTabBar::addTabAnimation(OTab *tab)
{
    QPropertyAnimation *animation = new QPropertyAnimation(tab, "width", tab);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setDuration(100);
    animation->setStartValue(39);
    animation->setEndValue(tabsWidth);
    animation->start();
}

void OTabBar::removeTab(int &index)
{
    removeTabAnimation(getTabAt(index));
    tabsList.removeAt(index);
}

void OTabBar::bruteRemoveTab(int &index)
{
    tabsLayout->removeWidget(getTabAt(index));

    if (nextTabButton->isVisible() || previousTabButton->isVisible())
    {
        showTab();
    }

    if ((index == activeTab) && (activeTab != 0))
    {
        int tabIndex = activeTab-1;
        setCurrentTab(tabIndex);
        int tab = currentTab();
        emit currentChanged(tab);
    }
    else if ((index == activeTab) && (activeTab == 0) && (count() > 0))
    {
        setCurrentTab(activeTab);
        int tab = currentTab();
        emit currentChanged(tab);
    }
    else if (index < activeTab)
        activeTab -= 1;

    getTabAt(index)->deleteLater();
    tabsList.removeAt(index);

    for (int i = 0; i < tabsList.count(); i++)
    {
        tabsList.at(i)->setTabIndex(i);
    }
}

void OTabBar::removeTabAnimation(OTab *tab)
{
    tab->hideContent();
    tabToRemove = tab;
    QPropertyAnimation *animation = new QPropertyAnimation(tab, "width", tab);
    connect(animation, &QPropertyAnimation::finished, this, &OTabBar::removingEnded);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setDuration(95);
    animation->setStartValue(tabsWidth);
    animation->setEndValue(39);
    animation->start();
}

void OTabBar::removingEnded()
{
    bool resized = false;
    tabsLayout->removeWidget(tabToRemove);

    if (nextTabButton->isVisible() || previousTabButton->isVisible())
    {
        showTab();
        resized = true;
    }
    else
        resized = resizeOnRemoving();

    if (!resized)
        return;

    if ((tabToRemove->getTabIndex() == activeTab) && (activeTab != 0))
    {
        int tabIndex = activeTab-1;
        setCurrentTab(tabIndex);
        int tab = currentTab();
        emit currentChanged(tab);
    }
    else if ((tabToRemove->getTabIndex() == activeTab) && (activeTab == 0) && (count() > 0))
    {
        setCurrentTab(activeTab);
        int tab = currentTab();
        emit currentChanged(tab);
    }
    else if (tabToRemove->getTabIndex() < activeTab)
        activeTab -= 1;

    tabToRemove->deleteLater();
    tabToRemove = nullptr;

    for (int i = 0; i < tabsList.count(); i++)
    {
        tabsList.at(i)->setTabIndex(i);
    }

    if (tabToMove)
        tabToMove = nullptr;
}

int OTabBar::count()
{
    return tabsList.count();
}

OTab* OTabBar::getTabAt(int index)
{
    return tabsList.at(index);
}

int OTabBar::getTabsWidth()
{
    return tabsWidth;
}

void OTabBar::setTabsWidth(int &width)
{
    tabsWidth = width;
}

bool OTabBar::resizeTabs()
{
    setUpdatesEnabled(false);

    bool resized = false;
    bool resizeUnnecessary = true;
    int parentWidth = parentWidget()->width();
    int tabSpaceWidth = 0;
    int newTabsWidth = 0;
    int remainingSpace = 0;

    for (int i = 0; i < count(); i++)
    {
        tabSpaceWidth += tabsList.at(i)->width();
    }

    int occupiedSpace = tabSpaceWidth+45*3+30+3+2+2+((count()-1)*2);

    if (previousTabButton->isVisible())
        occupiedSpace +=32;
    if (nextTabButton->isVisible())
        occupiedSpace +=32;

    remainingSpace = parentWidth-occupiedSpace;

    if (count() > 0)
    {
        if (remainingSpace < tabsWidth)
        {
            if (nextTabButton->isVisible() || previousTabButton->isVisible())
            {
                int visibleTabsCount = 0;

                for (int i = 0; i < count(); i++)
                {
                    if (getTabAt(i)->isVisible())
                        visibleTabsCount++;
                }

                if (visibleTabsCount == 0)
                    visibleTabsCount = count();

                newTabsWidth = (tabSpaceWidth+remainingSpace)/(visibleTabsCount);
            }
            else
                newTabsWidth = (tabSpaceWidth+remainingSpace)/(count());;

            if (newTabsWidth <= 230)
            {
                setTabsWidth(newTabsWidth);

                for (int i = 0; i < count(); i++)
                {
                    getTabAt(i)->setFixedWidth(newTabsWidth);
                    getTabAt(i)->setTabText(tabsList.at(i)->getTabText());
                }
                resized = true;
                resizeUnnecessary = false;
            }
        }
        else if ((remainingSpace > tabsWidth) && (tabsWidth < 230))
        {
            newTabsWidth = (tabSpaceWidth+remainingSpace)/(count());

            if (newTabsWidth > 230)
                newTabsWidth = 230;

            setTabsWidth(newTabsWidth);

            for (int i = 0; i < count(); i++)
            {
                getTabAt(i)->setFixedWidth(newTabsWidth);
                getTabAt(i)->setTabText(tabsList.at(i)->getTabText());
            }
            resized = true;
            resizeUnnecessary = false;
        }
    }
    setUpdatesEnabled(true);

    if (resized && !resizeUnnecessary)
        return resized;
    else
        return resizeUnnecessary;
}

bool OTabBar::resizeOnAdding()
{
    setUpdatesEnabled(false);

    bool resized = false;
    bool resizeUnnecessary = true;
    int parentWidth = parentWidget()->width();
    int tabSpaceWidth = 0;
    int newTabsWidth = 0;
    int remainingSpace = 0;

    for (int i = 0; i < count(); i++)
    {
        tabSpaceWidth += tabsList.at(i)->width();
    }

    int occupiedSpace = tabSpaceWidth+45*3+30+3+2+2+((count()-1)*2);

    if (previousTabButton->isVisible())
        occupiedSpace +=32;
    if (nextTabButton->isVisible())
        occupiedSpace +=32;

    remainingSpace = parentWidth-occupiedSpace;

    if (remainingSpace < tabsWidth)
    {
        newTabsWidth = (tabSpaceWidth+remainingSpace)/(count()+1);
        setTabsWidth(newTabsWidth);
        QParallelAnimationGroup *groupAnimation = new QParallelAnimationGroup;

        for (int i = 0; i < count(); i++)
        {
            QPropertyAnimation *animation = new QPropertyAnimation(getTabAt(i), "width", getTabAt(i));
            animation->setEasingCurve(QEasingCurve::InOutQuad);
            animation->setDuration(100);
            animation->setStartValue(getTabAt(i)->width());
            animation->setEndValue(newTabsWidth);
            groupAnimation->addAnimation(animation);
            getTabAt(i)->setTabText(tabsList.at(i)->getTabText());
        }

        groupAnimation->start();
        resized = true;
        resizeUnnecessary = false;
    }

    setUpdatesEnabled(true);

    if (resized && !resizeUnnecessary)
        return resized;
    else
        return resizeUnnecessary;
}

bool OTabBar::resizeOnRemoving()
{
    setUpdatesEnabled(false);

    bool resized = false;
    bool resizeUnnecessary = true;
    int parentWidth = parentWidget()->width();
    int tabSpaceWidth = 0;
    int newTabsWidth = 0;
    int remainingSpace = 0;

    for (int i = 0; i < count(); i++)
    {
        tabSpaceWidth += tabsList.at(i)->width();
    }

    int occupiedSpace = tabSpaceWidth+45*3+30+3+2+2+((count()-1)*2);

    if (previousTabButton->isVisible())
        occupiedSpace +=32;
    if (nextTabButton->isVisible())
        occupiedSpace +=32;

    remainingSpace = parentWidth-occupiedSpace;

    if (remainingSpace > 1)
    {
        newTabsWidth = (tabSpaceWidth+remainingSpace)/(count());

        if (newTabsWidth > 230)
            newTabsWidth = 230;

        setTabsWidth(newTabsWidth);
        QParallelAnimationGroup *groupAnimation = new QParallelAnimationGroup;

        for (int i = 0; i < count(); i++)
        {
            QPropertyAnimation *animation = new QPropertyAnimation(getTabAt(i), "width", getTabAt(i));
            animation->setEasingCurve(QEasingCurve::InOutQuad);
            animation->setDuration(100);
            animation->setStartValue(getTabAt(i)->width());
            animation->setEndValue(newTabsWidth);
            groupAnimation->addAnimation(animation);
            getTabAt(i)->setTabText(tabsList.at(i)->getTabText());
        }

        groupAnimation->start();
        resized = true;
        resizeUnnecessary = false;
    }
    setUpdatesEnabled(true);

    if (resized && !resizeUnnecessary)
        return resized;
    else
        return resizeUnnecessary;
}

bool OTabBar::resizeOnGrabbing()
{
    setUpdatesEnabled(false);

    bool resized = false;
    bool resizeUnnecessary = true;
    int parentWidth = parentWidget()->width();
    int tabSpaceWidth = 0;
    int newTabsWidth = 0;
    int remainingSpace = 0;

    for (int i = 0; i < count(); i++)
    {
        tabSpaceWidth += tabsList.at(i)->width();
    }

    int occupiedSpace = tabSpaceWidth+45*3+30+3+2+2+((count()-1)*2);

    if (previousTabButton->isVisible())
        occupiedSpace +=32;
    if (nextTabButton->isVisible())
        occupiedSpace +=32;

    remainingSpace = parentWidth-occupiedSpace;

    if (remainingSpace < tabsWidth)
    {
        newTabsWidth = (tabSpaceWidth+remainingSpace)/(count()+1);
        setTabsWidth(newTabsWidth);

        for (int i = 0; i < count(); i++)
        {
            getTabAt(i)->setFixedWidth(newTabsWidth);
            getTabAt(i)->setTabText(tabsList.at(i)->getTabText());
        }

        resized = true;
        resizeUnnecessary = false;
    }

    setUpdatesEnabled(true);

    if (resized && !resizeUnnecessary)
        return resized;
    else
        return resizeUnnecessary;
}

void OTabBar::updateSize(QResizeEvent *event)
{
    event->accept();
    resizeTabs();
}
void OTabBar::setWinType(WindowType tabType)
{
    winType = tabType;
}

void OTabBar::grabTabWindow()
{
    OTabPage *tabPage = static_cast<OTabPage*>(parentWidget());
    QWidget *pageView = tabPage->getStackPage()->widget(currentTab());
    OAppCore *browserApp = static_cast<OAppCore*>(tabPage->getParentWindow()->getParentApplication());
    int newDistWidth = (230*distanceWidth)/tabsWidth;

    browserApp->openNormalSizeWindow(SW_SHOW, winType, QPoint(cursor().pos().x()-newDistWidth, cursor().pos().y()-16));

    OTabPage *newTabPage = static_cast<OTabPage*>(browserApp->allWindowsList().at(browserApp->allWindowsList().count()-1)->getCentralWidget());
    newTabPage->getStackPage()->widget(0)->findChild<OWebView*>()->setPage(pageView->findChild<OWebView*>()->page());
    newTabPage->getStackPage()->widget(0)->findChild<OWebView*>()->page()->setParent(newTabPage->getStackPage()->widget(0)->findChild<OWebView*>());
    newTabPage->getTabBar()->setTabToMove(newTabPage->getTabBar()->getTabAt(0));
    tabPage->closeTabPage(currentTab());

    QMouseEvent mouseMove = QMouseEvent(QEvent::MouseMove, newTabPage->mapFromGlobal(cursor().pos()), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    newTabPage->getTabBar()->mouseTabMoveEvent(&mouseMove);

    if (newTabPage->isVisible())
    {
        QMouseEvent mouseRelease = QMouseEvent(QEvent::MouseButtonRelease, tabPage->getTabBar()->geometry().center(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        OAppCore::sendEvent(tabPage->getTabBar(), &mouseRelease);
    }
}

void OTabBar::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if (!childAt(event->pos()) && !tabToMove)
    {
        OTabPage *tabPage = static_cast<OTabPage*>(parentWidget());

        ReleaseCapture();
        SendMessage(tabPage->getParentWindow()->getCustomWinHandle(), WM_NCLBUTTONDOWN, HTCAPTION, 0);
        ReleaseCapture();
        SendMessage((HWND)tabPage->winId(), WM_NCMBUTTONDOWN, HTCAPTION, 0);
        SetFocus((HWND)tabPage->winId());
    }
    else
    {
        if (count() == 1)
        {
            windowMoved = true;
            OTabPage *tabPage = static_cast<OTabPage*>(parentWidget());

            ReleaseCapture();
            SendMessage(tabPage->getParentWindow()->getCustomWinHandle(), WM_NCLBUTTONDOWN, HTCAPTION, 0);           
            ReleaseCapture();
            SendMessage((HWND)tabPage->winId(), WM_NCMBUTTONDOWN, HTCAPTION, 0);
            SetFocus((HWND)tabPage->winId());

            windowMoved = false;

            if (tabToMove)
                tabToMove = nullptr;
        }
        else if ((event->pos().y() > 44) || (event->pos().y() < 0) || (event->pos().x() < 1) || (event->pos().x() > (width()-(45*3+30+3+2))))
            grabTabWindow();
        else if ((event->pos() - clickPosition).manhattanLength() > (tabsWidth*0.15))
        {
            if (!tabToMove)
            {
                tabToMove = tabsList.at(currentTab());

                if (mapFromGlobal(cursor().pos()).x() > tabToMove->pos().x())
                    distanceWidth = mapFromGlobal(cursor().pos()).x() - tabToMove->pos().x();

                if ((tabToMove->pos().x()+tabsWidth) > (width()-(45*3+30+3+2)))
                {
                    int occupiedSpace = 45*3+30+3+2;

                    if (previousTabButton->isVisible() || nextTabButton->isVisible())
                        occupiedSpace +=32;

                    oldPosition = QPoint(width()-(occupiedSpace+tabsWidth), tabToMove->y());
                }
                else
                    oldPosition = tabToMove->pos();

                tabToMove->raise();
            }
            tabMoveEvent();
        }
    }
}

void OTabBar::mousePressEvent(QMouseEvent *event)
{
    if (event->pos() == QPoint(0, 0))
        clickPosition = mapFromGlobal(cursor().pos());
    else
        clickPosition = event->pos();

    if ((event->type() == QMouseEvent::MouseButtonDblClick) && (event->button() == Qt::LeftButton))
        if (!childAt(event->pos()))
        {
            OTabPage *tabPage = static_cast<OTabPage*>(parentWidget());
            tabPage->getParentWindow()->resizeWindow();
        }
    handleCompleter();
}

void OTabBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (tabToMove)
    {
        QPropertyAnimation *animation = new QPropertyAnimation(tabToMove, "geometry", tabToMove);
        connect(animation, &QPropertyAnimation::finished, this, &OTabBar::tabReindexing);
        animation->setDuration(125);
        animation->setStartValue(tabToMove->geometry());
        animation->setEndValue(QRect(oldPosition.x(), oldPosition.y(), tabToMove->width(), tabToMove->height()));
        animation->start();
    }
    else if (!tabToMove && !addButton->isVisible())
        addButton->setVisible(true);

    event->accept();
}

void OTabBar::tabReindexing()
{
    if (moveDirection == MoveToLeft)
    {
        tabsLayout->removeWidget(tabToMove);
        tabsLayout->insertWidget(tabsLayout->indexOf(tabsList.at(tabToMove->getTabIndex()+1)), tabToMove);
    }
    else if (moveDirection == MoveToRight)
    {
        tabsLayout->removeWidget(tabToMove);

        if (tabToMove->getTabIndex() == (count()-1))
            tabsLayout->addWidget(tabToMove);
        else if (tabToMove->getTabIndex() < (count()-1))
            tabsLayout->insertWidget(tabsLayout->indexOf(tabsList.at(tabToMove->getTabIndex()+1)), tabToMove);
    }

    moveDirection = TabMoveDirection::NotMoved;
    tabToMove = nullptr;

    if (!addButton->isVisible())
        addButton->setVisible(true);
    ajustTabSize();
}

void OTabBar::tabMoveEvent()
{
    if (!tabToMove)
        return;

    if (addButton->isVisible())
        addButton->setVisible(false);

    int unusedSpace = 45*3+30+3+2;
    QPoint cursorPos = mapFromGlobal(cursor().pos());

    if (nextTabButton->isVisible())
        unusedSpace +=32;

    if (previousTabButton->isVisible())
    {
        if (((tabToMove->pos().x()+tabsWidth) <= (width()-unusedSpace)) && (tabToMove->pos().x() >= (pos().x()+32)))
            tabToMove->move(cursorPos.x()-distanceWidth, tabToMove->pos().y());
    }
    else if (((tabToMove->pos().x()+tabsWidth) <= (width()-unusedSpace)) && (tabToMove->pos().x() >= pos().x()))
        tabToMove->move(cursorPos.x()-distanceWidth, tabToMove->pos().y());

    if ((tabToMove->pos().x()+tabsWidth) >= (width()-unusedSpace))
        tabToMove->move((width()-unusedSpace)-tabsWidth, tabToMove->pos().y());

    if (previousTabButton->isVisible())
    {
        if (tabToMove->pos().x() <= (pos().x()+32))
            tabToMove->move((pos().x()+32), tabToMove->pos().y());
    }
    else if (tabToMove->pos().x() <= pos().x())
        tabToMove->move(pos().x(), tabToMove->pos().y());

    if ((tabToMove->pos().x() > oldPosition.x()) && ((tabToMove->pos() - oldPosition).manhattanLength() > tabsWidth*0.6))
    {
        if (isGrabbed)
            swipeTabLeft(false);
        else
            swipeTabLeft(true);

        if (getIsGrabbed())
            setIsGrabbed(false);
    }
    else if ((tabToMove->pos().x() < oldPosition.x()) && ((tabToMove->pos() - oldPosition).manhattanLength() > tabsWidth*0.6))
    {
        if (isGrabbed)
            swipeTabRight(false);
        else
            swipeTabRight(true);

        if (getIsGrabbed())
            setIsGrabbed(false);
    }
}

void OTabBar::swipeTabLeft(bool animate)
{
    if (tabToMove->getTabIndex() < (count()-1))
    {
        OTab *nextTab = tabsList.at(tabToMove->getTabIndex()+1);

        if (animate)
            if (nextTab->getTabIndex() == (tabToMove->getTabIndex()+1))
            {
                moveDirection = TabMoveDirection::MoveToRight;
                nextTab->setTabIndex(tabToMove->getTabIndex());
                tabToMove->setTabIndex(tabToMove->getTabIndex()+1);
                tabsList.removeOne(nextTab);
                tabsList.insert(nextTab->getTabIndex(), nextTab);
                oldPosition = nextTab->pos();
                activeTab++;
                emit tabMoved(nextTab->getTabIndex(), tabToMove->getTabIndex());

                QPropertyAnimation *animation = new QPropertyAnimation(nextTab, "geometry", nextTab);
                animation->setDuration(125);
                animation->setStartValue(nextTab->geometry());
                animation->setEndValue(QRect(nextTab->pos().x()-tabsWidth, nextTab->pos().y(), nextTab->width(), nextTab->height()));
                animation->start();
            }
    }
}

void OTabBar::swipeTabRight(bool animate)
{
    if (tabToMove->getTabIndex() > 0)
    {
        if (animate)
        {
            OTab *previousTab = tabsList.at(tabToMove->getTabIndex()-1);

            if (previousTab->getTabIndex() == (tabToMove->getTabIndex()-1))
            {
                moveDirection = TabMoveDirection::MoveToLeft;
                previousTab->setTabIndex(tabToMove->getTabIndex());
                tabToMove->setTabIndex(tabToMove->getTabIndex()-1);
                tabsList.removeOne(previousTab);
                tabsList.insert(previousTab->getTabIndex(), previousTab);
                oldPosition = previousTab->pos();
                activeTab--;
                emit tabMoved(previousTab->getTabIndex(), tabToMove->getTabIndex());

                QPropertyAnimation *animation = new QPropertyAnimation(previousTab, "geometry", previousTab);
                animation->setDuration(125);
                animation->setStartValue(previousTab->geometry());
                animation->setEndValue(QRect(previousTab->pos().x()+tabsWidth, previousTab->pos().y(), previousTab->width(), previousTab->height()));
                animation->start();
            }
        }
        else
        {
            QParallelAnimationGroup *groupAnimation = new QParallelAnimationGroup;
            moveDirection = TabMoveDirection::MoveToLeft;

            while ((tabToMove->pos().x() < oldPosition.x()) && ((tabToMove->pos() - oldPosition).manhattanLength() > tabsWidth*0.6))
            {
                OTab *previousTab = tabsList.at(tabToMove->getTabIndex()-1);

                if (previousTab->getTabIndex() == (tabToMove->getTabIndex()-1))
                {
                    previousTab->setTabIndex(tabToMove->getTabIndex());
                    tabToMove->setTabIndex(tabToMove->getTabIndex()-1);
                    tabsList.removeOne(previousTab);
                    tabsList.insert(previousTab->getTabIndex(), previousTab);
                    oldPosition = previousTab->pos();
                    activeTab--;
                    emit tabMoved(previousTab->getTabIndex(), tabToMove->getTabIndex());

                    QPropertyAnimation *animation = new QPropertyAnimation(previousTab, "geometry", previousTab);
                    animation->setDuration(125);
                    animation->setStartValue(previousTab->geometry());
                    animation->setEndValue(QRect(previousTab->pos().x()+tabsWidth, previousTab->pos().y(), previousTab->width(), previousTab->height()));
                    groupAnimation->addAnimation(animation);
                }
            }
            groupAnimation->start();
        }
    }
}

void OTabBar::paintEvent(QPaintEvent *event)
{
    event->accept();
    QStyleOption option;
    option.initFrom(this);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
}

void OTabBar::mouseTabMoveEvent(QMouseEvent *event)
{
    mouseMoveEvent(event);
}

void OTabBar::setTabToMove(OTab *tab)
{
    tabToMove = tab;
    tab->raise();
}

void OTabBar::setOldPosition(const QPoint position)
{
    oldPosition = position;
}

void OTabBar::setDistanceWidth(const int distance)
{
    distanceWidth = distance;
}

bool OTabBar::getWindowMoved()
{
    return windowMoved;
}

OTab *OTabBar::getTabToMove()
{
    return tabToMove;
}

void OTabBar::setWindowMoved(bool isMoved)
{
    windowMoved = isMoved;
}

void OTabBar::hideTab()
{
    if (previousTabButton->isHidden())
        previousTabButton->setVisible(true);
    if (nextTabButton->isVisible())
        moveToLastTab();
    else
        for (int i = 0; i < count(); i++)
            if (getTabAt(i)->isVisible())
            {
                getTabAt(i)->hideContent();
                QPropertyAnimation *animation = new QPropertyAnimation(getTabAt(i), "width", getTabAt(i));
                connect(animation, &QPropertyAnimation::finished, getTabAt(i), &OTab::hide);
                connect(animation, &QPropertyAnimation::finished, this, &OTabBar::ajustTabSize);
                animation->setEasingCurve(QEasingCurve::InOutQuad);
                animation->setDuration(100);
                animation->setStartValue(getTabAt(i)->width());
                animation->setEndValue(39);
                animation->start();
                break;
            }
}

void OTabBar::hideTab(bool onGrab)
{
    if (!onGrab)
        return;
    if (previousTabButton->isHidden())
        previousTabButton->setVisible(true);
    if (nextTabButton->isVisible())
        moveToLastTab(true);
    else
        for (int i = 0; i < count(); i++)
            if (getTabAt(i)->isVisible())
            {
                getTabAt(i)->hideContent();
                getTabAt(i)->setFixedWidth(39);
                getTabAt(i)->hide();
                break;
            }
}

void OTabBar::moveToLastTab()
{
    nextTabButton->setHidden(true);
    int hiddenTabs = 0;
    int visibleTabs = 0;
    int firstVisibleTabIndex = 0;

    for (int i = (count()-1); i >= 0; i--)
        if (getTabAt(i)->isHidden())
            hiddenTabs++;
        else
            break;

    for (int i = 0; i < count(); i++)
    {
        if (getTabAt(i)->isVisible())
            visibleTabs++;
        if (visibleTabs == 1)
            firstVisibleTabIndex = i;
    }

    if (hiddenTabs == 0)
        return;

    int i = firstVisibleTabIndex;
    QSequentialAnimationGroup *sGroupAnimation = new QSequentialAnimationGroup;

    if (getTabAt(i)->isVisible())
        for (int j = (count()-hiddenTabs); j < count(); j++)
            if (getTabAt(j)->isHidden())
            {
                getTabAt(i)->hideContent();
                QPropertyAnimation *animation1 = new QPropertyAnimation(getTabAt(i), "width", getTabAt(i));
                connect(animation1, &QPropertyAnimation::finished, getTabAt(i), &OTab::hide);
                animation1->setEasingCurve(QEasingCurve::InOutQuad);
                animation1->setDuration(1500/hiddenTabs);
                animation1->setStartValue(tabsWidth);
                animation1->setEndValue(39);

                getTabAt(j)->showContent();
                QPropertyAnimation *animation2 = new QPropertyAnimation(getTabAt(j), "width", getTabAt(j));
                animation2->setEasingCurve(QEasingCurve::InOutQuad);
                animation2->setDuration(1500/hiddenTabs);
                animation2->setStartValue(39);
                animation2->setEndValue(tabsWidth);

                if (j == (count()-hiddenTabs))
                    getTabAt(j)->setVisible(true);
                if (j == (count()-1))
                    connect(animation2, &QPropertyAnimation::finished, this, &OTabBar::ajustTabSize);
                if (j < (count()-1))
                {
                    connect(animation2, &QPropertyAnimation::finished, getTabAt(j+1), &OTab::show);
                    getTabAt(j+1)->showContent();
                }
                i++;

                QParallelAnimationGroup *groupAnimation = new QParallelAnimationGroup;
                groupAnimation->addAnimation(animation1);
                groupAnimation->addAnimation(animation2);
                sGroupAnimation->addAnimation(groupAnimation);
            }

    sGroupAnimation->start();
}

void OTabBar::moveToLastTab(bool onGrab)
{
    if (!onGrab)
        return;

    nextTabButton->setHidden(true);
    int hiddenTabs = 0;
    int visibleTabs = 0;
    int firstVisibleTabIndex = 0;

    for (int i = (count()-1); i >= 0; i--)
        if (getTabAt(i)->isHidden())
            hiddenTabs++;
        else
            break;

    for (int i = 0; i < count(); i++)
    {
        if (getTabAt(i)->isVisible())
            visibleTabs++;
        if (visibleTabs == 1)
            firstVisibleTabIndex = i;
    }

    if (hiddenTabs == 0)
        return;

    int i = firstVisibleTabIndex;

    if (getTabAt(i)->isVisible())
        for (int j = (count()-hiddenTabs); j < count(); j++)
            if (getTabAt(j)->isHidden())
            {
                getTabAt(i)->hideContent();
                getTabAt(i)->setFixedWidth(39);
                getTabAt(i)->hide();
                getTabAt(j)->setFixedWidth(tabsWidth);
                getTabAt(j)->setVisible(true);
                i++;
            }
}

void OTabBar::switchToTab(const int index)
{
    if (index == 0)
        previousTabButton->setHidden(true);
    if (nextTabButton->isHidden())
        nextTabButton->setVisible(true);

    setCurrentTab(index);

    int hiddenTabs = 0;
    int visibleTabs = 0;
    int lastVisibleTabIndex = 0;

    for (int i = index; i < count(); i++)
        if (getTabAt(i)->isHidden())
            hiddenTabs++;
        else
            break;

    for (int i = 0; i < count(); i++)
        if (getTabAt(i)->isVisible())
        {
            visibleTabs++;
            lastVisibleTabIndex = i;
        }

    if (hiddenTabs == 0)
        return;

    int i = lastVisibleTabIndex;
    QSequentialAnimationGroup *sGroupAnimation = new QSequentialAnimationGroup;

    if (getTabAt(i)->isVisible())
        for (int j = (index+(hiddenTabs-1)); j >= index; j--)
            if (getTabAt(j)->isHidden())
            {
                getTabAt(i)->hideContent();
                QPropertyAnimation *animation1 = new QPropertyAnimation(getTabAt(i), "width", getTabAt(i));
                connect(animation1, &QPropertyAnimation::finished, getTabAt(i), &OTab::hide);
                animation1->setEasingCurve(QEasingCurve::InOutQuad);
                animation1->setDuration(1500/hiddenTabs);
                animation1->setStartValue(tabsWidth);
                animation1->setEndValue(39);

                getTabAt(j)->showContent();
                QPropertyAnimation *animation2 = new QPropertyAnimation(getTabAt(j), "width", getTabAt(j));
                animation2->setEasingCurve(QEasingCurve::InOutQuad);
                animation2->setDuration(1500/hiddenTabs);
                animation2->setStartValue(39);
                animation2->setEndValue(tabsWidth);

                if (j == (index+(hiddenTabs-1)))
                    getTabAt(j)->setVisible(true);
                if (j == index)
                    connect(animation2, &QPropertyAnimation::finished, this, &OTabBar::ajustTabSize);
                if (j > index)
                {
                    connect(animation2, &QPropertyAnimation::finished, getTabAt(j-1), &OTab::show);
                    getTabAt(j-1)->showContent();
                }
                i--;

                QParallelAnimationGroup *groupAnimation = new QParallelAnimationGroup;
                groupAnimation->addAnimation(animation1);
                groupAnimation->addAnimation(animation2);
                sGroupAnimation->addAnimation(groupAnimation);
            }

    sGroupAnimation->start();
}

void OTabBar::showTab()
{
    bool shown = false;

    for (int i = (count()-1); i >= 0; i--)
        if (getTabAt(i)->isVisible())
        {
            if (i == (count()-2))
                nextTabButton->setHidden(true);
            if (i == (count()-1))
            {
                shown = true;
                break;
            }

            i++;
            getTabAt(i)->setVisible(true);
            getTabAt(i)->showContent();
            QPropertyAnimation *animation = new QPropertyAnimation(getTabAt(i), "width", getTabAt(i));
            connect(animation, &QPropertyAnimation::finished, this, &OTabBar::ajustTabSize);
            animation->setEasingCurve(QEasingCurve::InOutQuad);
            animation->setDuration(100);
            animation->setStartValue(39);
            animation->setEndValue(tabsWidth);
            animation->start();
            break;
        }

    if (shown)
        for (int i = 0; i < count(); i++)
            if (getTabAt(i)->isVisible())
            {
                if (i == 1)
                    previousTabButton->setHidden(true);
                if (i == 0)
                    break;

                i--;
                getTabAt(i)->setVisible(true);
                getTabAt(i)->showContent();
                QPropertyAnimation *animation = new QPropertyAnimation(getTabAt(i), "width", getTabAt(i));
                connect(animation, &QPropertyAnimation::finished, this, &OTabBar::ajustTabSize);
                animation->setEasingCurve(QEasingCurve::InOutQuad);
                animation->setDuration(100);
                animation->setStartValue(39);
                animation->setEndValue(tabsWidth);
                animation->start();
                break;
            }
}


void OTabBar::selectPreviousTab()
{
    if (getTabAt(0)->isVisible())
        return;

    if (nextTabButton->isHidden())
        nextTabButton->setVisible(true);

    QParallelAnimationGroup *groupAnimation = new QParallelAnimationGroup;

    for (int i = (count()-1); i >= 0; i--)
        if (getTabAt(i)->isVisible())
        {
            getTabAt(i)->hideContent();
            QPropertyAnimation *animation = new QPropertyAnimation(getTabAt(i), "width", getTabAt(i));
            connect(animation, &QPropertyAnimation::finished, getTabAt(i), &OTab::hide);
            animation->setEasingCurve(QEasingCurve::InOutQuad);
            animation->setDuration(100);
            animation->setStartValue(getTabAt(i)->width());
            animation->setEndValue(39);
            groupAnimation->addAnimation(animation);
            break;
        }

    for (int i = 0; i < count(); i++)
        if (getTabAt(i)->isVisible())
        {
            if (i == 1)
                previousTabButton->setHidden(true);
            if (i == 0)
                break;

            i--;
            getTabAt(i)->setVisible(true);
            getTabAt(i)->showContent();
            QPropertyAnimation *animation = new QPropertyAnimation(getTabAt(i), "width", getTabAt(i));
            connect(animation, &QPropertyAnimation::finished, this, &OTabBar::ajustTabSize);
            animation->setEasingCurve(QEasingCurve::InOutQuad);
            animation->setDuration(100);
            animation->setStartValue(39);
            animation->setEndValue(tabsWidth);
            groupAnimation->addAnimation(animation);
            break;
        }

    groupAnimation->start();
}

void OTabBar::selectNextTab()
{
    if (getTabAt(count()-1)->isVisible())
        return;

    if (previousTabButton->isHidden())
        previousTabButton->setVisible(true);

    QParallelAnimationGroup *groupAnimation = new QParallelAnimationGroup;

    for (int i = 0; i < count(); i++)
    {
        if (getTabAt(i)->isVisible())
        {
            getTabAt(i)->hideContent();
            QPropertyAnimation *animation = new QPropertyAnimation(getTabAt(i), "width", getTabAt(i));
            connect(animation, &QPropertyAnimation::finished, getTabAt(i), &OTab::hide);
            animation->setEasingCurve(QEasingCurve::InOutQuad);
            animation->setDuration(100);
            animation->setStartValue(getTabAt(i)->width());
            animation->setEndValue(39);
            groupAnimation->addAnimation(animation);
            break;
        }
    }

    for (int i = (count()-1); i >= 0; i--)
    {
        if (getTabAt(i)->isVisible())
        {
            if (i == (count()-2))
                nextTabButton->setHidden(true);
            if (i == (count()-1))
                break;

            i++;
            getTabAt(i)->setVisible(true);
            getTabAt(i)->showContent();
            QPropertyAnimation *animation = new QPropertyAnimation(getTabAt(i), "width", getTabAt(i));
            connect(animation, &QPropertyAnimation::finished, this, &OTabBar::ajustTabSize);
            animation->setEasingCurve(QEasingCurve::InOutQuad);
            animation->setDuration(100);
            animation->setStartValue(39);
            animation->setEndValue(tabsWidth);
            groupAnimation->addAnimation(animation);
            break;
        }
    }

    groupAnimation->start();
}

void OTabBar::ajustTabSize()
{
    resizeTabs();
}

void OTabBar::setIsGrabbed(bool state)
{
    isGrabbed = state;
}

bool OTabBar::getIsGrabbed()
{
    return isGrabbed;
}

void OTabBar::setLightGrayTheme()
{
    tabBarThemeColor = "Light gray";
    setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; border-bottom: 1px solid #ffffff; border-top: none;"
                                "border-left: none; border-right: none;}"));

    addButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; padding: 0px; border-radius: 2px;}"
                                           "QToolButton:hover {background-color: #e1e1e3; border: 1px solid #e1e1e3;}"
                                           "QToolButton:pressed {background-color: #c4c4c6; border: 2px solid #c4c4c6;}"));

    minimizeButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; padding: 0px; border-radius: 0px;}"
                                                "QToolButton:hover {background-color: #e1e1e3; border: 1px solid #e1e1e3;}"
                                                "QToolButton:pressed {background-color: #c4c4c6; border: 2px solid #c4c4c6;}"));

    maxMinButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; padding: 0px; border-radius: 0px;}"
                                              "QToolButton:hover {background-color: #e1e1e3; border: 1px solid #e1e1e3;}"
                                              "QToolButton:pressed {background-color: #c4c4c6; border: 2px solid #c4c4c6;}"));

    closeButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; padding: 0px; border-radius: 0px;"
                                             "background-image: url(:/images/close.png); background-repeat: no-repeat; background-position: center;}"
                                             "QToolButton:hover {background-color: red; border: 1px solid red;"
                                             "background-image: url(:/images/closeHover.png);}"));

    nextTabButton->setStyleSheet(QLatin1String("QToolButton {background-color: #f3f3f5; border: 1px solid #f3f3f5;"
                                               "padding: 0px;  border-radius: 3.5%;}"
                                               "QToolButton:hover {background-color: #e1e1e3;}"
                                               "QToolButton:pressed {background-color: #c4c4c6;}"));

    previousTabButton->setStyleSheet(QLatin1String("QToolButton {background-color: #f3f3f5; border: 1px solid #f3f3f5;"
                                                   "padding: 0px;  border-radius: 3.5%;}"
                                                   "QToolButton:hover {background-color: #e1e1e3;}"
                                                   "QToolButton:pressed {background-color: #c4c4c6;}"));

    if (static_cast<OTabPage*>(parentWidget())->getParentWindow()->getIsMaximized())
        maxMinButton->setIcon(QIcon(":/images/restore.png"));
    else
        maxMinButton->setIcon(QIcon(":/images/maximize.png"));

    addButton->setIcon(QIcon(":/images/plus.png"));
    minimizeButton->setIcon(QIcon(":/images/minimize.png"));
    nextTabButton->setIcon(QIcon(":/images/nextTab.png"));
    previousTabButton->setIcon(QIcon(":/images/previousTab.png"));

    for (int i = 0; i < count(); i++)
    {
        if (i == activeTab)
            getTabAt(i)->setSelectedStyle(tabBarThemeColor);
        else
            getTabAt(i)->setDeselectedStyle(tabBarThemeColor);
    }
}

void OTabBar::setLightTurquoiseTheme()
{
    tabBarThemeColor = "Light turquoise";
    setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; border-bottom: 1px solid #ffffff; border-top: none;"
                                "border-left: none; border-right: none;}"));

    addButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; padding: 0px; border-radius: 2px;}"
                                           "QToolButton:hover {background-color: #c0ebee; border: 1px solid #c0ebee;}"
                                           "QToolButton:pressed {background-color: #b0dadd; border: 2px solid #b0dadd;}"));

    minimizeButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; padding: 0px; border-radius: 0px;}"
                                                "QToolButton:hover {background-color: #c0ebee; border: 1px solid #c0ebee;}"
                                                "QToolButton:pressed {background-color: #b0dadd; border: 2px solid #b0dadd;}"));

    maxMinButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; padding: 0px; border-radius: 0px;}"
                                              "QToolButton:hover {background-color: #c0ebee; border: 1px solid #c0ebee;}"
                                              "QToolButton:pressed {background-color: #b0dadd; border: 2px solid #b0dadd;}"));

    closeButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; padding: 0px; border-radius: 0px;"
                                             "background-image: url(:/images/close.png); background-repeat: no-repeat; background-position: center;}"
                                             "QToolButton:hover {background-color: red; border: 1px solid red;"
                                             "background-image: url(:/images/closeHover.png);}"));

    nextTabButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff;"
                                               "padding: 0px;  border-radius: 3.5%;}"
                                               "QToolButton:hover {background-color: #c0ebee;}"
                                               "QToolButton:pressed {background-color: #b0dadd;}"));

    previousTabButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff;"
                                                   "padding: 0px;  border-radius: 3.5%;}"
                                                   "QToolButton:hover {background-color: #c0ebee;}"
                                                   "QToolButton:pressed {background-color: #b0dadd;}"));

    if (static_cast<OTabPage*>(parentWidget())->getParentWindow()->getIsMaximized())
        maxMinButton->setIcon(QIcon(":/images/restore.png"));
    else
        maxMinButton->setIcon(QIcon(":/images/maximize.png"));

    addButton->setIcon(QIcon(":/images/plus.png"));
    minimizeButton->setIcon(QIcon(":/images/minimize.png"));
    nextTabButton->setIcon(QIcon(":/images/nextTab.png"));
    previousTabButton->setIcon(QIcon(":/images/previousTab.png"));

    for (int i = 0; i < count(); i++)
    {
        if (i == activeTab)
            getTabAt(i)->setSelectedStyle(tabBarThemeColor);
        else
            getTabAt(i)->setDeselectedStyle(tabBarThemeColor);
    }
}

void OTabBar::setLightBrownTheme()
{
    tabBarThemeColor = "Light brown";
    setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; border-bottom: 1px solid #ffffff; border-top: none;"
                                "border-left: none; border-right: none;}"));

    addButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; padding: 0px; border-radius: 2px;}"
                                           "QToolButton:hover {background-color: #ffd0ae; border: 1px solid #ffd0ae;}"
                                           "QToolButton:pressed {background-color: #ebbfa0; border: 2px solid #ebbfa0;}"));

    minimizeButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; padding: 0px; border-radius: 0px;}"
                                                "QToolButton:hover {background-color: #ffd0ae; border: 1px solid #ffd0ae;}"
                                                "QToolButton:pressed {background-color: #ebbfa0; border: 2px solid #ebbfa0;}"));

    maxMinButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; padding: 0px; border-radius: 0px;}"
                                              "QToolButton:hover {background-color: #ffd0ae; border: 1px solid #ffd0ae;}"
                                              "QToolButton:pressed {background-color: #ebbfa0; border: 2px solid #ebbfa0;}"));

    closeButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff; padding: 0px; border-radius: 0px;"
                                             "background-image: url(:/images/close.png); background-repeat: no-repeat; background-position: center;}"
                                             "QToolButton:hover {background-color: red; border: 1px solid red;"
                                             "background-image: url(:/images/closeHover.png);}"));

    nextTabButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff;"
                                               "padding: 0px;  border-radius: 3.5%;}"
                                               "QToolButton:hover {background-color: #ffd0ae;}"
                                               "QToolButton:pressed {background-color: #ebbfa0;}"));

    previousTabButton->setStyleSheet(QLatin1String("QToolButton {background-color: #ffffff; border: 1px solid #ffffff;"
                                                   "padding: 0px;  border-radius: 3.5%;}"
                                                   "QToolButton:hover {background-color: #ffd0ae;}"
                                                   "QToolButton:pressed {background-color: #ebbfa0;}"));

    if (static_cast<OTabPage*>(parentWidget())->getParentWindow()->getIsMaximized())
        maxMinButton->setIcon(QIcon(":/images/restore.png"));
    else
        maxMinButton->setIcon(QIcon(":/images/maximize.png"));

    addButton->setIcon(QIcon(":/images/plus.png"));
    minimizeButton->setIcon(QIcon(":/images/minimize.png"));
    nextTabButton->setIcon(QIcon(":/images/nextTab.png"));
    previousTabButton->setIcon(QIcon(":/images/previousTab.png"));

    for (int i = 0; i < count(); i++)
    {
        if (i == activeTab)
            getTabAt(i)->setSelectedStyle(tabBarThemeColor);
        else
            getTabAt(i)->setDeselectedStyle(tabBarThemeColor);
    }
}

void OTabBar::setDarkGrayTheme()
{
    tabBarThemeColor = "Dark gray";
    setStyleSheet(QLatin1String("QWidget {background-color: #434344; border-bottom: 1px solid #434344; border-top: none;"
                                "border-left: none; border-right: none;}"));

    addButton->setStyleSheet(QLatin1String("QToolButton {background-color: #434344; border: 1px solid #434344; padding: 0px; border-radius: 2px;}"
                                           "QToolButton:hover {background-color: #59595a; border: 1px solid #59595a;}"
                                           "QToolButton:pressed {background-color: #505051; border: 2px solid #505051;}"));

    minimizeButton->setStyleSheet(QLatin1String("QToolButton {background-color: #434344; border: 1px solid #434344; padding: 0px; border-radius: 0px;}"
                                                "QToolButton:hover {background-color: #59595a; border: 1px solid #59595a;}"
                                                "QToolButton:pressed {background-color: #505051; border: 2px solid #505051;}"));

    maxMinButton->setStyleSheet(QLatin1String("QToolButton {background-color: #434344; border: 1px solid #434344; padding: 0px; border-radius: 0px;}"
                                              "QToolButton:hover {background-color: #59595a; border: 1px solid #59595a;}"
                                              "QToolButton:pressed {background-color: #505051; border: 2px solid #505051;}"));

    closeButton->setStyleSheet(QLatin1String("QToolButton {background-color: #434344; border: 1px solid #434344; padding: 0px; border-radius: 0px;"
                                             "background-image: url(:/images/closeHover.png); background-repeat: no-repeat; background-position: center;}"
                                             "QToolButton:hover {background-color: red; border: 1px solid red;"
                                             "background-image: url(:/images/closeHover.png);}"));

    nextTabButton->setStyleSheet(QLatin1String("QToolButton {background-color: #434344; border: 1px solid #434344;"
                                               "padding: 0px;  border-radius: 3.5%;}"
                                               "QToolButton:hover {background-color: #59595a;}"
                                               "QToolButton:pressed {background-color: #505051;}"));

    previousTabButton->setStyleSheet(QLatin1String("QToolButton {background-color: #434344; border: 1px solid #434344;"
                                                   "padding: 0px;  border-radius: 3.5%;}"
                                                   "QToolButton:hover {background-color: #59595a;}"
                                                   "QToolButton:pressed {background-color: #505051;}"));

    if (static_cast<OTabPage*>(parentWidget())->getParentWindow()->getIsMaximized())
        maxMinButton->setIcon(QIcon(":/private_mode_images/restorePrivateMode.png"));
    else
        maxMinButton->setIcon(QIcon(":/private_mode_images/maximizePrivateMode.png"));

    addButton->setIcon(QIcon(":/private_mode_images/plusPrivateMode.png"));
    minimizeButton->setIcon(QIcon(":/private_mode_images/minimizePrivateMode.png"));
    nextTabButton->setIcon(QIcon(":/private_mode_images/nextTabPrivateMode.png"));
    previousTabButton->setIcon(QIcon(":/private_mode_images/previousTabPrivateMode.png"));

    for (int i = 0; i < count(); i++)
    {
        if (i == activeTab)
            getTabAt(i)->setSelectedStyle(tabBarThemeColor);
        else
            getTabAt(i)->setDeselectedStyle(tabBarThemeColor);
    }
}

void OTabBar::setDarkTurquoiseTheme()
{
    tabBarThemeColor = "Dark turquoise";
    setStyleSheet(QLatin1String("QWidget {background-color: #1e4446; border-bottom: 1px solid #1e4446; border-top: none;"
                                "border-left: none; border-right: none;}"));

    addButton->setStyleSheet(QLatin1String("QToolButton {background-color: #1e4446; border: none; padding: 0px; border-radius: 2px;}"
                                           "QToolButton:hover {background-color: #27585b; border: 1px solid #27585b;}"
                                           "QToolButton:pressed {background-color: #234f51; border: 2px solid #234f51;}"));

    minimizeButton->setStyleSheet(QLatin1String("QToolButton {background-color: #1e4446; border: none; padding: 0px; border-radius: 0px;}"
                                                "QToolButton:hover {background-color: #27585b; border: 1px solid #27585b;}"
                                                "QToolButton:pressed {background-color: #234f51; border: 2px solid #234f51;}"));

    maxMinButton->setStyleSheet(QLatin1String("QToolButton {background-color: #1e4446; border: none; padding: 0px; border-radius: 0px;}"
                                              "QToolButton:hover {background-color: #27585b; border: 1px solid #27585b;}"
                                              "QToolButton:pressed {background-color: #234f51; border: 2px solid #234f51;}"));

    closeButton->setStyleSheet(QLatin1String("QToolButton {background-color: #1e4446; border: none; padding: 0px; border-radius: 0px;"
                                             "background-image: url(:/images/closeHover.png); background-repeat: no-repeat; background-position: center;}"
                                             "QToolButton:hover {background-color: red; border: 1px solid red;"
                                             "background-image: url(:/images/closeHover.png);}"));

    nextTabButton->setStyleSheet(QLatin1String("QToolButton {background-color: #1e4446; border: 1px solid #1e4446;"
                                               "padding: 0px;  border-radius: 3.5%;}"
                                               "QToolButton:hover {background-color: #27585b;}"
                                               "QToolButton:pressed {background-color: #234f51;}"));

    previousTabButton->setStyleSheet(QLatin1String("QToolButton {background-color: #1e4446; border: 1px solid #1e4446;"
                                                   "padding: 0px;  border-radius: 3.5%;}"
                                                   "QToolButton:hover {background-color: #27585b;}"
                                                   "QToolButton:pressed {background-color: #234f51;}"));

    if (static_cast<OTabPage*>(parentWidget())->getParentWindow()->getIsMaximized())
        maxMinButton->setIcon(QIcon(":/private_mode_images/restorePrivateMode.png"));
    else
        maxMinButton->setIcon(QIcon(":/private_mode_images/maximizePrivateMode.png"));

    addButton->setIcon(QIcon(":/private_mode_images/plusPrivateMode.png"));
    minimizeButton->setIcon(QIcon(":/private_mode_images/minimizePrivateMode.png"));
    nextTabButton->setIcon(QIcon(":/private_mode_images/nextTabPrivateMode.png"));
    previousTabButton->setIcon(QIcon(":/private_mode_images/previousTabPrivateMode.png"));

    for (int i = 0; i < count(); i++)
    {
        if (i == activeTab)
            getTabAt(i)->setSelectedStyle(tabBarThemeColor);
        else
            getTabAt(i)->setDeselectedStyle(tabBarThemeColor);
    }
}

void OTabBar::setDarkBrownTheme()
{
    tabBarThemeColor = "Dark brown";
    setStyleSheet(QLatin1String("QWidget {background-color: #462c21; border-bottom: 1px solid #462c21; border-top: none;"
                                "border-left: none; border-right: none;}"));

    addButton->setStyleSheet(QLatin1String("QToolButton {background-color: #462c21; border: none; padding: 0px; border-radius: 2px;}"
                                           "QToolButton:hover {background-color: #5b382b; border: 1px solid #5b382b;}"
                                           "QToolButton:pressed {background-color: #513226; border: 2px solid #513226;}"));

    minimizeButton->setStyleSheet(QLatin1String("QToolButton {background-color: #462c21; border: none; padding: 0px; border-radius: 0px;}"
                                                "QToolButton:hover {background-color: #5b382b; border: 1px solid #5b382b;}"
                                                "QToolButton:pressed {background-color: #513226; border: 2px solid #513226;}"));

    maxMinButton->setStyleSheet(QLatin1String("QToolButton {background-color: #462c21; border: none; padding: 0px; border-radius: 0px;}"
                                              "QToolButton:hover {background-color: #5b382b; border: 1px solid #5b382b;}"
                                              "QToolButton:pressed {background-color: #513226; border: 2px solid #513226;}"));

    closeButton->setStyleSheet(QLatin1String("QToolButton {background-color: #462c21; border: none; padding: 0px; border-radius: 0px;"
                                             "background-image: url(:/images/closeHover.png); background-repeat: no-repeat; background-position: center;}"
                                             "QToolButton:hover {background-color: red; border: 1px solid red;"
                                             "background-image: url(:/images/closeHover.png);}"));

    nextTabButton->setStyleSheet(QLatin1String("QToolButton {background-color: #462c21; border: 1px solid #462c21;"
                                               "padding: 0px;  border-radius: 3.5%;}"
                                               "QToolButton:hover {background-color: #5b382b;}"
                                               "QToolButton:pressed {background-color: #513226;}"));

    previousTabButton->setStyleSheet(QLatin1String("QToolButton {background-color: #462c21; border: 1px solid #462c21;"
                                                   "padding: 0px;  border-radius: 3.5%;}"
                                                   "QToolButton:hover {background-color: #5b382b;}"
                                                   "QToolButton:pressed {background-color: #513226;}"));

    if (static_cast<OTabPage*>(parentWidget())->getParentWindow()->getIsMaximized())
        maxMinButton->setIcon(QIcon(":/private_mode_images/restorePrivateMode.png"));
    else
        maxMinButton->setIcon(QIcon(":/private_mode_images/maximizePrivateMode.png"));

    addButton->setIcon(QIcon(":/private_mode_images/plusPrivateMode.png"));
    minimizeButton->setIcon(QIcon(":/private_mode_images/minimizePrivateMode.png"));
    nextTabButton->setIcon(QIcon(":/private_mode_images/nextTabPrivateMode.png"));
    previousTabButton->setIcon(QIcon(":/private_mode_images/previousTabPrivateMode.png"));

    for (int i = 0; i < count(); i++)
    {
        if (i == activeTab)
            getTabAt(i)->setSelectedStyle(tabBarThemeColor);
        else
            getTabAt(i)->setDeselectedStyle(tabBarThemeColor);
    }
}

void OTabBar::setPrivateTheme()
{
    tabBarThemeColor = "Private";
    setStyleSheet(QLatin1String("QWidget {background-color: #3c3244; border-bottom: 1px solid #3c3244; border-top: none;"
                                    "border-left: none; border-right: none;}"));

    addButton->setStyleSheet(QLatin1String("QToolButton {background-color: #3c3244; border: none; padding: 0px; border-radius: 2px;}"
                                           "QToolButton:hover {background-color: #50425b; border: 1px solid #50425b;}"
                                           "QToolButton:pressed {background-color: #473b51; border: 2px solid #473b51;}"));

    minimizeButton->setStyleSheet(QLatin1String("QToolButton {background-color: #3c3244; border: none; padding: 0px; border-radius: 0px;}"
                                                "QToolButton:hover {background-color: #50425b; border: 1px solid #50425b;}"
                                                "QToolButton:pressed {background-color: #473b51; border: 2px solid #473b51;}"));

    maxMinButton->setStyleSheet(QLatin1String("QToolButton {background-color: #3c3244; border: none; padding: 0px; border-radius: 0px;}"
                                              "QToolButton:hover {background-color: #50425b; border: 1px solid #50425b;}"
                                              "QToolButton:pressed {background-color: #473b51; border: 2px solid #473b51;}"));

    closeButton->setStyleSheet(QLatin1String("QToolButton {background-color: #3c3244; border: none; padding: 0px; border-radius: 0px;"
                                             "background-image: url(:/images/closeHover.png); background-repeat: no-repeat; background-position: center;}"
                                             "QToolButton:hover {background-color: red; border: 1px solid red;"
                                             "background-image: url(:/images/closeHover.png);}"));

    nextTabButton->setStyleSheet(QLatin1String("QToolButton {background-color: #3c3244; border: 1px solid #3c3244;"
                                               "padding: 0px;  border-radius: 3.5%;}"
                                               "QToolButton:hover {background-color: #50425b;}"
                                               "QToolButton:pressed {background-color: #473b51;}"));

    previousTabButton->setStyleSheet(QLatin1String("QToolButton {background-color: #3c3244; border: 1px solid #3c3244;"
                                                   "padding: 0px;  border-radius: 3.5%;}"
                                                   "QToolButton:hover {background-color: #50425b;}"
                                                   "QToolButton:pressed {background-color: #473b51;}"));

    if (static_cast<OTabPage*>(parentWidget())->getParentWindow()->getIsMaximized())
        maxMinButton->setIcon(QIcon(":/private_mode_images/restorePrivateMode.png"));
    else
        maxMinButton->setIcon(QIcon(":/private_mode_images/maximizePrivateMode.png"));

    addButton->setIcon(QIcon(":/private_mode_images/plusPrivateMode.png"));
    minimizeButton->setIcon(QIcon(":/private_mode_images/minimizePrivateMode.png"));
    nextTabButton->setIcon(QIcon(":/private_mode_images/nextTabPrivateMode.png"));
    previousTabButton->setIcon(QIcon(":/private_mode_images/previousTabPrivateMode.png"));

    for (int i = 0; i < count(); i++)
    {
        if (i == activeTab)
            getTabAt(i)->setSelectedStyle(tabBarThemeColor);
        else
            getTabAt(i)->setDeselectedStyle(tabBarThemeColor);
    }
}
