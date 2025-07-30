#include "owebsiteinfo.h"
#include "osmartinput.h"
#include <QWidgetAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QHideEvent>

OWebsiteInfo::OWebsiteInfo(QWidget *parent)
    :QMenu(parent)
{
    ololanTheme = "Light gray";
    webRequest = "";
    panel = new QStackedWidget(this);
    widgetA = new QWidget(panel);
    widgetB = new QWidget(panel);
    widgetC = new QWidget(panel);
    website = new QLabel(widgetA);
    securityInfo = new QLabel(widgetA);
    websiteIcon = new QLabel(widgetA);
    securityInfoIcon = new QLabel(widgetA);
    webPermission = new QLabel(widgetB);
    username = new QLabel(widgetC);
    password = new QLabel(widgetC);
    allow = new QPushButton(QString("Allow"), widgetB);
    block = new QPushButton(QString("Block"), widgetB);
    savePwd = new QPushButton(QString("Save"), widgetC);
    cancelPwd = new QPushButton(QString("Cancel"), widgetC);

    website->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    securityInfo->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    websiteIcon->setAlignment(Qt::AlignLeft);
    securityInfoIcon->setAlignment(Qt::AlignLeft);
    webPermission->setAlignment(Qt::AlignLeft);
    username->setAlignment(Qt::AlignLeft);
    password->setAlignment(Qt::AlignLeft);

    setFixedWidth(350);
    panel->setFixedWidth(348);
    widgetA->setFixedWidth(348);
    widgetB->setFixedWidth(348);
    widgetC->setFixedWidth(348);
    allow->setFixedSize(90, 35);
    block->setFixedSize(90, 35);
    savePwd->setFixedSize(90, 35);
    cancelPwd->setFixedSize(90, 35);
    website->setFixedHeight(16);
    websiteIcon->setFixedWidth(16);
    securityInfo->setFixedHeight(16);
    securityInfoIcon->setFixedWidth(16);

    connect(allow, &QPushButton::clicked, this, &OWebsiteInfo::allowPermission);
    connect(block, &QPushButton::clicked, this, &OWebsiteInfo::blockPermission);
    connect(savePwd, &QPushButton::clicked, this, &OWebsiteInfo::saveCredentials);
    setupView();
}

void OWebsiteInfo::setupView()
{
    QHBoxLayout *websiteLayout = new QHBoxLayout();
    websiteLayout->setSpacing(5);
    websiteLayout->setContentsMargins(0, 0, 0, 0);
    websiteLayout->addWidget(websiteIcon);
    websiteLayout->addWidget(website);

    QHBoxLayout *securityInfoLayout = new QHBoxLayout();
    securityInfoLayout->setSpacing(5);
    securityInfoLayout->setContentsMargins(0, 0, 0, 0);
    securityInfoLayout->addWidget(securityInfoIcon);
    securityInfoLayout->addWidget(securityInfo);

    QVBoxLayout *vALayout = new QVBoxLayout(widgetA);
    vALayout->setSpacing(0);
    vALayout->setContentsMargins(15, 10, 10, 10);
    vALayout->addLayout(websiteLayout);
    vALayout->addSpacing(16);
    vALayout->addLayout(securityInfoLayout);

    QHBoxLayout *hBLayout = new QHBoxLayout();
    hBLayout->setSpacing(0);
    hBLayout->setContentsMargins(0, 0, 0, 0);
    hBLayout->addWidget(allow);
    hBLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    hBLayout->addWidget(block);

    QVBoxLayout *vBLayout = new QVBoxLayout(widgetB);
    vBLayout->setSpacing(0);
    vBLayout->setContentsMargins(10, 10, 10, 10);
    vBLayout->addWidget(webPermission);
    vBLayout->addSpacing(30);
    vBLayout->addLayout(hBLayout);

    QHBoxLayout *hCLayout = new QHBoxLayout();
    hCLayout->setSpacing(0);
    hCLayout->setContentsMargins(0, 0, 0, 0);
    hCLayout->addWidget(cancelPwd);
    hCLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    hCLayout->addWidget(savePwd);

    QVBoxLayout *vCLayout = new QVBoxLayout(widgetC);
    vCLayout->setSpacing(0);
    vCLayout->setContentsMargins(10, 10, 10, 10);
    vCLayout->addWidget(username);
    vCLayout->addSpacing(15);
    vCLayout->addWidget(password);
    vCLayout->addSpacing(15);
    vCLayout->addLayout(hCLayout);

    panel->addWidget(widgetA);
    panel->addWidget(widgetB);
    panel->addWidget(widgetC);

    QWidgetAction *wAction = new QWidgetAction(this);
    wAction->setDefaultWidget(panel);
    addAction(wAction);
}

void OWebsiteInfo::updateWebsiteInfo(const QString urlHost, bool isSecure)
{
    setFixedHeight(110);
    panel->setFixedHeight(108);
    widgetA->setFixedHeight(108);
    website->setText(QString("Connected to ") + urlHost);

    if (isSecure)
    {
        securityInfo->setText(QString("Secure connection"));

        if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
            (ololanTheme.compare("Light brown") == 0))
            securityInfoIcon->setPixmap(QPixmap(QLatin1String(":/images/secure.png")));
        else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
                 (ololanTheme.compare("Dark brown") == 0) || (ololanTheme.compare("Private") == 0))
            securityInfoIcon->setPixmap(QPixmap(QLatin1String(":/private_mode_images/securePrivateMode.png")));
    }
    else
    {
        securityInfo->setText(QString("Unsecure connection"));

        if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
            (ololanTheme.compare("Light brown") == 0))
            securityInfoIcon->setPixmap(QPixmap(QLatin1String(":/images/unsecure.png")));
        else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
                 (ololanTheme.compare("Dark brown") == 0) || (ololanTheme.compare("Private") == 0))
            securityInfoIcon->setPixmap(QPixmap(QLatin1String(":/images/unsecure.png")));
    }

    panel->setCurrentIndex(0);
}

void OWebsiteInfo::updatePermissionInfo(const QString website, const QString permission, const QUrl security)
{
    setFixedHeight(110);
    panel->setFixedHeight(108);
    widgetB->setFixedHeight(108);
    securityOrigin = security;
    webRequest = permission;

    if (permission.compare("location") == 0)
        webPermission->setText(website + QString(" wants to know your location"));
    else if (permission.compare("camera") == 0)
        webPermission->setText(website + QString(" wants to use your camera"));
    else if (permission.compare("microphone") == 0)
        webPermission->setText(website + QString(" wants to use your microphone"));
    else if (permission.compare("webcam") == 0)
        webPermission->setText(website + QString(" wants to use your webcam"));

    panel->setCurrentIndex(1);
}

void OWebsiteInfo::updateAuthentificationInfo(const QString user, const QUrl url)
{
    setFixedHeight(130);
    panel->setFixedHeight(128);
    widgetB->setFixedHeight(128);
    username->setText(user);
    password->setText("***************");
    setCredentialsUrl(url);
    panel->setCurrentIndex(2);
}

void OWebsiteInfo::setLightGrayTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"));
    widgetA->setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; padding: 0px; border: none;}"));
    widgetB->setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; padding: 0px; border: none;}"));
    widgetC->setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; padding: 0px; border: none;}"));

    website->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #909090;"
                                         "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    websiteIcon->setPixmap(QPixmap(QLatin1String(":/images/urlTyped.png")));

    securityInfo->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #909090;"
                                              "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    webPermission->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #909090;"
                                               "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    username->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #909090;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    password->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #909090;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    block->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;}"
                                       "QPushButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                       "QPushButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));

    allow->setStyleSheet(QLatin1String("QPushButton {color: #909090; background-color: #f9f9fb; border: none; padding: 0px; border-radius: 3.5%;}"
                                       "QPushButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                       "QPushButton:pressed {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"));

    savePwd->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;}"
                                         "QPushButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                         "QPushButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));

    cancelPwd->setStyleSheet(QLatin1String("QPushButton {color: #909090; background-color: #f9f9fb; border: none; padding: 0px; border-radius: 3.5%;}"
                                           "QPushButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                           "QPushButton:pressed {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"));
    ololanTheme = "Light gray";
}

void OWebsiteInfo::setLightTurquoiseTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"));
    widgetA->setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; padding: 0px; border: none;}"));
    widgetB->setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; padding: 0px; border: none;}"));
    widgetC->setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; padding: 0px; border: none;}"));

    website->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #909090;"
                                         "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    websiteIcon->setPixmap(QPixmap(QLatin1String(":/images/urlTyped.png")));

    securityInfo->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #909090;"
                                              "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    webPermission->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #909090;"
                                               "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    username->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #909090;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    password->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #909090;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    block->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;}"
                                       "QPushButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                       "QPushButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));

    allow->setStyleSheet(QLatin1String("QPushButton {color: #909090; background-color: #f9f9fb; border: none; padding: 0px; border-radius: 3.5%;}"
                                       "QPushButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                       "QPushButton:pressed {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"));

    savePwd->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;}"
                                         "QPushButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                         "QPushButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));

    cancelPwd->setStyleSheet(QLatin1String("QPushButton {color: #909090; background-color: #f9f9fb; border: none; padding: 0px; border-radius: 3.5%;}"
                                           "QPushButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                           "QPushButton:pressed {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"));
    ololanTheme = "Light turquoise";
}

void OWebsiteInfo::setLightBrownTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"));
    widgetA->setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; padding: 0px; border: none;}"));
    widgetB->setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; padding: 0px; border: none;}"));
    widgetC->setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; padding: 0px; border: none;}"));

    website->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #909090;"
                                         "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    websiteIcon->setPixmap(QPixmap(QLatin1String(":/images/urlTyped.png")));

    securityInfo->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #909090;"
                                              "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    webPermission->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #909090;"
                                               "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    username->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #909090;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    password->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #909090;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    block->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #ffa477; border: none; padding: 0px; border-radius: 3.5%;}"
                                       "QPushButton:hover {background-color: #e5926b; border: 1px solid #e5926b;}"
                                       "QPushButton:pressed {background-color: #d68864; border: 1px solid #d68864;}"));

    allow->setStyleSheet(QLatin1String("QPushButton {color: #909090; background-color: #f9f9fb; border: none; padding: 0px; border-radius: 3.5%;}"
                                       "QPushButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                       "QPushButton:pressed {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"));

    savePwd->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #ffa477; border: none; padding: 0px; border-radius: 3.5%;}"
                                         "QPushButton:hover {background-color: #e5926b; border: 1px solid #e5926b;}"
                                         "QPushButton:pressed {background-color: #d68864; border: 1px solid #d68864;}"));

    cancelPwd->setStyleSheet(QLatin1String("QPushButton {color: #909090; background-color: #f9f9fb; border: none; padding: 0px; border-radius: 3.5%;}"
                                           "QPushButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                           "QPushButton:pressed {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"));
    ololanTheme = "Light brown";
}

void OWebsiteInfo::setDarkGrayTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #434344; padding: 0px; border: 1px solid #434344;}"));
    widgetA->setStyleSheet(QLatin1String("QWidget {background-color: #434344; padding: 0px; border: none;}"));
    widgetB->setStyleSheet(QLatin1String("QWidget {background-color: #434344; padding: 0px; border: none;}"));
    widgetC->setStyleSheet(QLatin1String("QWidget {background-color: #434344; padding: 0px; border: none;}"));

    website->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                         "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    websiteIcon->setPixmap(QPixmap(QLatin1String(":/private_mode_images/urlTypedPrivateMode.png")));

    securityInfo->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                              "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    webPermission->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                               "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    username->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    password->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    block->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;}"
                                       "QPushButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                       "QPushButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));

    allow->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #6b6b6b; border: none; padding: 0px; border-radius: 3.5%;}"
                                       "QPushButton:hover {background-color: #777778; border: 1px solid #777778;}"
                                       "QPushButton:pressed {background-color: #888889; border: 2px solid #888889;}"));

    savePwd->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;}"
                                         "QPushButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                         "QPushButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));

    cancelPwd->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #6b6b6b; border: none; padding: 0px; border-radius: 3.5%;}"
                                           "QPushButton:hover {background-color: #777778; border: 1px solid #777778;}"
                                           "QPushButton:pressed {background-color: #888889; border: 2px solid #888889;}"));
    ololanTheme = "Dark gray";
}

void OWebsiteInfo::setDarkTurquoiseTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #1e4446; padding: 0px; border: 1px solid #1e4446;}"));
    widgetA->setStyleSheet(QLatin1String("QWidget {background-color: #1e4446; padding: 0px; border: none;}"));
    widgetB->setStyleSheet(QLatin1String("QWidget {background-color: #1e4446; padding: 0px; border: none;}"));
    widgetC->setStyleSheet(QLatin1String("QWidget {background-color: #1e4446; padding: 0px; border: none;}"));

    website->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                         "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    websiteIcon->setPixmap(QPixmap(QLatin1String(":/private_mode_images/urlTypedPrivateMode.png")));

    securityInfo->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                              "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    webPermission->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                               "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    username->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    password->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    block->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;}"
                                       "QPushButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                       "QPushButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));

    allow->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #2e686b; border: none; padding: 0px; border-radius: 3.5%;}"
                                       "QPushButton:hover {background-color: #347779; border: 1px solid #347779;}"
                                       "QPushButton:pressed {background-color: #448890; border: 1px solid #448890;}"));

    savePwd->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;}"
                                         "QPushButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                         "QPushButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));

    cancelPwd->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #2e686b; border: none; padding: 0px; border-radius: 3.5%;}"
                                           "QPushButton:hover {background-color: #347779; border: 1px solid #347779;}"
                                           "QPushButton:pressed {background-color: #448890; border: 1px solid #448890;}"));
    ololanTheme = "Dark turquoise";
}

void OWebsiteInfo::setDarkBrownTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #462c21; padding: 0px; border: 1px solid #462c21;}"));
    widgetA->setStyleSheet(QLatin1String("QWidget {background-color: #462c21; padding: 0px; border: none;}"));
    widgetB->setStyleSheet(QLatin1String("QWidget {background-color: #462c21; padding: 0px; border: none;}"));
    widgetC->setStyleSheet(QLatin1String("QWidget {background-color: #462c21; padding: 0px; border: none;}"));

    website->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                         "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    websiteIcon->setPixmap(QPixmap(QLatin1String(":/private_mode_images/urlTypedPrivateMode.png")));

    securityInfo->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                              "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    webPermission->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                               "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    username->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    password->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    block->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #ffa477; border: none; padding: 0px; border-radius: 3.5%;}"
                                       "QPushButton:hover {background-color: #e5926b; border: 1px solid #e5926b;}"
                                       "QPushButton:pressed {background-color: #d68864; border: 1px solid #d68864;}"));

    allow->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #6b4232; border: none; padding: 0px; border-radius: 3.5%;}"
                                       "QPushButton:hover {background-color: #794a38; border: 1px solid #794a38;}"
                                       "QPushButton:pressed {background-color: #905743; border: 1px solid #905743;}"));

    savePwd->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #ffa477; border: none; padding: 0px; border-radius: 3.5%;}"
                                         "QPushButton:hover {background-color: #e5926b; border: 1px solid #e5926b;}"
                                         "QPushButton:pressed {background-color: #d68864; border: 1px solid #d68864;}"));

    cancelPwd->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #6b4232; border: none; padding: 0px; border-radius: 3.5%;}"
                                           "QPushButton:hover {background-color: #794a38; border: 1px solid #794a38;}"
                                           "QPushButton:pressed {background-color: #905743; border: 1px solid #905743;}"));
    ololanTheme = "Dark brown";
}

void OWebsiteInfo::setPrivateTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #3c3244; padding: 0px; border: 1px solid #3c3244;}"));
    widgetA->setStyleSheet(QLatin1String("QWidget {background-color: #3c3244; padding: 0px; border: none;}"));
    widgetB->setStyleSheet(QLatin1String("QWidget {background-color: #3c3244; padding: 0px; border: none;}"));
    widgetC->setStyleSheet(QLatin1String("QWidget {background-color: #3c3244; padding: 0px; border: none;}"));

    website->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                         "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    websiteIcon->setPixmap(QPixmap(QLatin1String(":/private_mode_images/urlTypedPrivateMode.png")));

    securityInfo->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                              "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    webPermission->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                               "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    username->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));
    password->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    block->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #c2a1dd; border: none; padding: 0px; border-radius: 3.5%;}"
                                       "QPushButton:hover {background-color: #b595ce; border: 1px solid #b898d1;}"
                                       "QPushButton:pressed {background-color: #a68bbe; border: 2px solid #ad90c5;}"));

    allow->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #5e4e6b; border: none; padding: 0px; border-radius: 3.5%;}"
                                       "QPushButton:hover {background-color: #695878; border: 1px solid #695878;}"
                                       "QPushButton:pressed {background-color: #79668b; border: 2px solid #79668b;}"));

    savePwd->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #c2a1dd; border: none; padding: 0px; border-radius: 3.5%;}"
                                         "QPushButton:hover {background-color: #b595ce; border: 1px solid #b898d1;}"
                                         "QPushButton:pressed {background-color: #a68bbe; border: 2px solid #ad90c5;}"));

    cancelPwd->setStyleSheet(QLatin1String("QPushButton {color: #ffffff; background-color: #5e4e6b; border: none; padding: 0px; border-radius: 3.5%;}"
                                           "QPushButton:hover {background-color: #695878; border: 1px solid #695878;}"
                                           "QPushButton:pressed {background-color: #79668b; border: 2px solid #79668b;}"));
    ololanTheme = "Private";
}

void OWebsiteInfo::hideEvent(QHideEvent *event)
{
    QMenu::hideEvent(event);
    panel->setCurrentIndex(0);
    panel->clearFocus();
    event->accept();
}

void OWebsiteInfo::setCredentialsUrl(const QUrl url)
{
    credentialsUrl = url;
}

const QUrl OWebsiteInfo::getCredentialsUrl()
{
    return credentialsUrl;
}

void OWebsiteInfo::saveCredentials()
{
    static_cast<OSmartInput*>(parentWidget()->parentWidget())->saveUserCredentials();
    hide();
}

void OWebsiteInfo::allowPermission()
{
    static_cast<OSmartInput*>(parentWidget()->parentWidget())->setPermission(webRequest, true, securityOrigin);
    webRequest = "";
    securityOrigin = QUrl();
    hide();
}

void OWebsiteInfo::blockPermission()
{
    static_cast<OSmartInput*>(parentWidget()->parentWidget())->setPermission(webRequest, false, securityOrigin);
    hide();
}
