#include "osyncwidget.h"
#include "otabpage.h"
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QPropertyAnimation>
#include <QGuiApplication>
#include <QScreen>
#include <QCloseEvent>

OSyncWidget::OSyncWidget(QWidget *parent)
    :QMenu(parent)
{
    signIn = false;
    setFixedSize(350, 350);
    setupControl();
    connect(syncButton, &QToolButton::clicked, this, &OSyncWidget::synchronize);
}

void OSyncWidget::setupControl()
{
    widgetForm = new QWidget(this);
    widgetForm->setFixedSize(348, 348);

    userUI = new QToolButton(this);
    userUI->setFixedSize(120, 120);
    userUI->setText("GU");

    username = new QLabel(this);
    username->setText("Guest user");

    syncButton = new QToolButton(this);
    syncButton->setFixedSize(160, 37);
    syncButton->setText("Sign In");

    labelInfo = new QLabel(this);
    labelInfo->setText("Sign in to enable all Ololan services");

    QVBoxLayout *vLayout = new QVBoxLayout(widgetForm);
    vLayout->setAlignment(Qt::AlignCenter);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    vLayout->addWidget(userUI, 0, Qt::AlignHCenter);
    vLayout->setSpacing(5);
    vLayout->addWidget(username, 0, Qt::AlignHCenter);
    vLayout->setSpacing(20);
    vLayout->addWidget(syncButton, 0, Qt::AlignHCenter);
    vLayout->setSpacing(20);
    vLayout->addWidget(labelInfo, 0, Qt::AlignHCenter);

    QWidgetAction *widgetAction = new QWidgetAction(this);
    widgetAction->setDefaultWidget(widgetForm);
    addAction(widgetAction);
}

void OSyncWidget::showEvent(QShowEvent *event)
{
    signIn = false;
    QMenu::showEvent(event);

    QRect screenSize = QGuiApplication::primaryScreen()->geometry();
    QRect parentRect = parentWidget()->geometry();
    int distance = screenSize.width() - parentRect.x();
    QRect startGeometry = geometry();
    QRect endGeometry = geometry();

    if (width() < distance)
    {
        if (cursor().pos().x() <= 1050)
        {
            startGeometry.setCoords(geometry().x()-width()+36, geometry().y()-10, geometry().x()+width(), geometry().y()+height()-10);
            endGeometry.setCoords(geometry().x()-width()+36, geometry().y(), geometry().x()+width(), geometry().y()+height());
        }
        else
        {
            distance = screenSize.width() - cursor().pos().x() + parentWidget()->mapFromGlobal(cursor().pos()).x() - 3;
            startGeometry.setCoords(geometry().x()-distance+36, geometry().y()-10, geometry().x()+width(), geometry().y()+height()-10);
            endGeometry.setCoords(geometry().x()-distance+36, geometry().y(), geometry().x()+width(), geometry().y()+height());
        }
    }
    else if (width() >= distance)
    {
        startGeometry.setCoords(geometry().x()-distance+36, geometry().y()-10, geometry().x()+width(), geometry().y()+height()-10);
        endGeometry.setCoords(geometry().x()-distance+36, geometry().y(), geometry().x()+width(), geometry().y()+height());
    }

    setGeometry(startGeometry);
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry", this);
    animation->setDuration(100);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);
    animation->start();
}

void OSyncWidget::closeEvent(QCloseEvent *event)
{
    QMenu::closeEvent(event);
    QPoint mousePos = mapFromGlobal(cursor().pos());

    if (signIn || !rect().contains(mousePos))
        event->accept();
    else
        event->ignore();
}

void OSyncWidget::synchronize()
{
    signIn = true;
    hide();
    tabPage->synchronizeOlolan();
}

void OSyncWidget::setLightGrayTheme()
{
    themeColor = "Light gray";
    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"));
    widgetForm->setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; padding: 0px; border: none;}"));
    userUI->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #dfdfe1; border: 1px solid #dfdfe1; border-radius: 60px;"
                                        "font-family: gadugi; font-size: 50px; font-weight: bold; padding-bottom: 2px;}"
                                        "QToolButton:hover {color: #ffffff; background-color: #dfdfe1; border: 1px solid #dfdfe1;}"
                                        "QToolButton:pressed {color: #ffffff; background-color: #dfdfe1; border: 1px solid #dfdfe1;}"));

    username->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #656565;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    labelInfo->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #656565;"
                                           "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    syncButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;"
                                            "font-family: gadugi; font-size: 14px; font-weight: bold; padding-bottom: 2px;}"
                                            "QToolButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                            "QToolButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));
}

void OSyncWidget::setLightTurquoiseTheme()
{
    themeColor = "Light turquoise";
    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"));
    widgetForm->setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; padding: 0px; border: none;}"));
    userUI->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #5cd1d3; border: 1px solid #5cd1d3; border-radius: 60px;"
                                        "font-family: gadugi; font-size: 50px; font-weight: bold; padding-bottom: 2px;}"
                                        "QToolButton:hover {color: #ffffff; background-color: #5cd1d3; border: 1px solid #5cd1d3;}"
                                        "QToolButton:pressed {color: #ffffff; background-color: #5cd1d3; border: 1px solid #5cd1d3;}"));

    username->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #656565;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    labelInfo->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #656565;"
                                           "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    syncButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;"
                                            "font-family: gadugi; font-size: 14px; font-weight: bold; padding-bottom: 2px;}"
                                            "QToolButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                            "QToolButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));
}

void OSyncWidget::setLightBrownTheme()
{
    themeColor = "Light brown";
    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"));
    widgetForm->setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; padding: 0px; border: none;}"));
    userUI->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #e7956c; border: 1px solid #e7956c; border-radius: 60px;"
                                        "font-family: gadugi; font-size: 50px; font-weight: bold; padding-bottom: 2px;}"
                                        "QToolButton:hover {color: #ffffff; background-color: #e7956c; border: 1px solid #e7956c;}"
                                        "QToolButton:pressed {color: #ffffff; background-color: #e7956c; border: 1px solid #e7956c;}"));

    username->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #656565;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    labelInfo->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #656565;"
                                           "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    syncButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #ffa477; border: none; padding: 0px; border-radius: 3.5%;"
                                            "font-family: gadugi; font-size: 14px; font-weight: bold; padding-bottom: 2px;}"
                                            "QToolButton:hover {background-color: #e5926b; border: 1px solid #e5926b;}"
                                            "QToolButton:pressed {background-color: #d68864; border: 1px solid #d68864;}"));
}

void OSyncWidget::setDarkGrayTheme()
{
    themeColor = "Dark gray";
    setStyleSheet(QLatin1String("QMenu {background-color: #434344; padding: 0px; border: 1px solid #434344;}"));
    widgetForm->setStyleSheet(QLatin1String("QWidget {background-color: #434344; padding: 0px; border: none;}"));
    userUI->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #6b6b6b; border: 1px solid #6b6b6b; border-radius: 60px;"
                                        "font-family: gadugi; font-size: 50px; font-weight: bold; padding-bottom: 2px;}"
                                        "QToolButton:hover {color: #ffffff; background-color: #6b6b6b; border: 1px solid #6b6b6b;}"
                                        "QToolButton:pressed {color: #ffffff; background-color: #6b6b6b; border: 1px solid #6b6b6b;}"));

    username->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    labelInfo->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                           "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    syncButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;"
                                            "font-family: gadugi; font-size: 14px; font-weight: bold; padding-bottom: 2px;}"
                                            "QToolButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                            "QToolButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));
}

void OSyncWidget::setDarkTurquoiseTheme()
{
    themeColor = "Dark turquoise";
    setStyleSheet(QLatin1String("QMenu {background-color: #1e4446; padding: 0px; border: 1px solid #1e4446;}"));
    widgetForm->setStyleSheet(QLatin1String("QWidget {background-color: #1e4446; padding: 0px; border: none;}"));
    userUI->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #2e686b; border: 1px solid #2e686b; border-radius: 60px;"
                                        "font-family: gadugi; font-size: 50px; font-weight: bold; padding-bottom: 2px;}"
                                        "QToolButton:hover {color: #ffffff; background-color: #2e686b; border: 1px solid #2e686b;}"
                                        "QToolButton:pressed {color: #ffffff; background-color: #2e686b; border: 1px solid #2e686b;}"));

    username->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    labelInfo->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                           "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    syncButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;"
                                            "font-family: gadugi; font-size: 14px; font-weight: bold; padding-bottom: 2px;}"
                                            "QToolButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                            "QToolButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));
}

void OSyncWidget::setDarkBrownTheme()
{
    themeColor = "Dark brown";
    setStyleSheet(QLatin1String("QMenu {background-color: #462c21; padding: 0px; border: 1px solid #462c21;}"));
    widgetForm->setStyleSheet(QLatin1String("QWidget {background-color: #462c21; padding: 0px; border: none;}"));
    userUI->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #6b4232; border: 1px solid #6b4232; border-radius: 60px;"
                                        "font-family: gadugi; font-size: 50px; font-weight: bold; padding-bottom: 2px;}"
                                        "QToolButton:hover {color: #ffffff; background-color: #6b4232; border: 1px solid #6b4232;}"
                                        "QToolButton:pressed {color: #ffffff; background-color: #6b4232; border: 1px solid #6b4232;}"));

    username->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    labelInfo->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                           "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    syncButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #ffa477; border: none; padding: 0px; border-radius: 3.5%;"
                                            "font-family: gadugi; font-size: 14px; font-weight: bold; padding-bottom: 2px;}"
                                            "QToolButton:hover {background-color: #e5926b; border: 1px solid #e5926b;}"
                                            "QToolButton:pressed {background-color: #d68864; border: 1px solid #d68864;}"));
}

void OSyncWidget::setPrivateTheme()
{
    themeColor = "Private";
    setStyleSheet(QLatin1String("QMenu {background-color: #3c3244; padding: 0px; border: 1px solid #3c3244;}"));
    widgetForm->setStyleSheet(QLatin1String("QWidget {background-color: #3c3244; padding: 0px; border: none;}"));
    userUI->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #5e4e6b; border: 1px solid #5e4e6b; border-radius: 60px;"
                                        "font-family: gadugi; font-size: 50px; font-weight: bold; padding-bottom: 2px;}"
                                        "QToolButton:hover {color: #ffffff; background-color: #5e4e6b; border: 1px solid #5e4e6b;}"
                                        "QToolButton:pressed {color: #ffffff; background-color: #5e4e6b; border: 1px solid #5e4e6b;}"));

    username->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                          "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    labelInfo->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                           "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    syncButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #c2a1dd; border: none; padding: 0px; border-radius: 3.5%;"
                                            "font-family: gadugi; font-size: 14px; font-weight: bold; padding-bottom: 2px;}"
                                            "QToolButton:hover {background-color: #b595ce; border: 1px solid #b898d1;}"
                                            "QToolButton:pressed {background-color: #a68bbe; border: 2px solid #ad90c5;}"));
}

void OSyncWidget::setTabPage(OTabPage *tPage)
{
    tabPage = tPage;
}

void OSyncWidget::updateProfile(const QString accountName, bool isLoggedIn)
{
    if (isLoggedIn)
    {
        QStringList idList = accountName.split(" ");
        QString firstname = idList.first();
        firstname = firstname.replace(firstname.at(0), firstname.at(0).toUpper());

        QString lastname = idList.last();
        lastname = lastname.replace(lastname.at(0), lastname.at(0).toUpper());

        QToolButton *button = qobject_cast<QToolButton*>(parent());
        button->setIcon(QIcon());
        button->setText(QString(firstname.at(0)) + QString(lastname.at(0)));

        userUI->setText(QString(firstname.at(0)) + QString(lastname.at(0)));
        username->setText(firstname + " " + lastname);
        syncButton->setText("Log out");
        labelInfo->setText("All Ololan services are enabled");
        qDebug() << " OSyncWidget::updateSyncProfile : " << username;
    }
    else
    {
        QToolButton *button = qobject_cast<QToolButton*>(parent());
        button->setIcon(QIcon());
        if ((themeColor.compare("Light gray") == 0) || (themeColor.compare("Light turquoise") == 0) || (themeColor.compare("Light brown") == 0))
            button->setIcon(QIcon(":/images/account.png"));

        else if ((themeColor.compare("Dark gray") == 0) || (themeColor.compare("Dark turquoise") == 0) || (themeColor.compare("Dark brown") == 0) || (themeColor.compare("Private") == 0))
            button->setIcon(QIcon(":/private mode images/accountPrivateMode.png"));

        userUI->setText("GU");
        username->setText("Guest user");
        syncButton->setText("Sign In");
        labelInfo->setText("Sign in to enable all Ololan services");
    }
}

void OSyncWidget::setOlolanTheme(QString theme)
{
    themeColor = theme;
}
