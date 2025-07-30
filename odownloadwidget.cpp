#include "odownloadwidget.h"
#include "owebwidget.h"
#include <QWidgetAction>
#include <QShowEvent>
#include <QPropertyAnimation>
#include <QGuiApplication>
#include <QScreen>
#include <QWebChannel>

ODownloadWidgetCore::ODownloadWidgetCore(QObject *parent)
    :QObject(parent)
{
}

void ODownloadWidgetCore::pauseDownload(const QString index)
{
    emit parentWidget->pause(index);
}

void ODownloadWidgetCore::stopDownload(const QString index)
{
    emit parentWidget->stop(index);
}

void ODownloadWidgetCore::resumeDownload(const QString index)
{
    emit parentWidget->resume(index);
}

void ODownloadWidgetCore::openManager()
{
    emit parentWidget->openDownloadsManager();
    parentWidget->hide();
}

void ODownloadWidgetCore::setParentMenu(ODownloadWidget *parentMenu)
{
    parentWidget = parentMenu;
}

ODownloadWidget::ODownloadWidget(QWidget *parent, WindowType type)
    :QMenu(parent)
{
    windType = type;
    isReady = false;
    setFixedSize(470, 165);

    widgetCore = new ODownloadWidgetCore(this);
    widgetCore->setParentMenu(this);

    widgetWebView = new OWebWidget(this);
    widgetWebView->setFixedSize(468, 163);

    connect(widgetWebView, &OWebWidget::loadFinished, this, &ODownloadWidget::widgetsReady);
    widgetWebView->load(QUrl("qrc:/web_applications/html/downloadsWidgetView.html"));

    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #dfdfe1;}"));

    QWidgetAction *widgetAction = new QWidgetAction(this);
    widgetAction->setDefaultWidget(widgetWebView);
    addAction(widgetAction);
}

void ODownloadWidget::widgetsReady(bool status)
{
    if (status)
    {
        isReady = status;
        QWebChannel *widgetWebChannel = new QWebChannel(widgetCore);
        widgetWebChannel->registerObject("widgetCore", widgetCore);
        widgetWebView->page()->setWebChannel(widgetWebChannel);
        widgetWebView->page()->runJavaScript(QLatin1String("setupConnector();"));

        if (ololanTheme.compare("Light gray") == 0)
            setLightGrayTheme();
        else if (ololanTheme.compare("Light turquoise") == 0)
            setLightTurquoiseTheme();
        else if (ololanTheme.compare("Light brown") == 0)
            setLightBrownTheme();
        else if (ololanTheme.compare("Dark gray") == 0)
            setDarkGrayTheme();
        else if (ololanTheme.compare("Dark turquoise") == 0)
            setDarkTurquoiseTheme();
        else if (ololanTheme.compare("Dark brown") == 0)
            setDarkBrownTheme();
    }
}

void ODownloadWidget::setupWidget(const int index, const QString title, const QByteArray icon, const QString size, const QString state)
{
    QString iconPath = QLatin1String("qrc:/web_applications/images/ololanLogo.png");
    if (state.compare("Downloading") == 0)
        iconPath = QLatin1String("qrc:/web_applications/images/activeDownload.png");
    else if (state.compare("Paused") == 0)
        iconPath = QLatin1String("qrc:/web_applications/images/pausedDownload.png");
    else if (state.compare("Failed") == 0)
        iconPath = QLatin1String("qrc:/web_applications/images/failedDownload.png");
    else if (state.compare("Complete") == 0)
        iconPath = QLatin1String("qrc:/web_applications/images/completeDownload.png");
    else if (icon != "EmPtY64")
        iconPath = QLatin1String("data:image/png;base64,%1").arg(icon);

    loadEmptyView(false);
    QString script = QLatin1String("loadDownloadItem('%1', '%2', '%3', '%4', '%5');").arg(QString::number(index), title, iconPath, size, state);
    widgetWebView->page()->runJavaScript(script);

    if (height() < 700)
    {
        setFixedHeight(height()+74);
        widgetWebView->setFixedHeight(widgetWebView->height()+74);
    }

    if (ololanTheme.compare("Light gray") == 0)
        setLightGrayTheme();
    else if (ololanTheme.compare("Light turquoise") == 0)
        setLightTurquoiseTheme();
    else if (ololanTheme.compare("Light brown") == 0)
        setLightBrownTheme();
    else if (ololanTheme.compare("Dark gray") == 0)
        setDarkGrayTheme();
    else if (ololanTheme.compare("Dark turquoise") == 0)
        setDarkTurquoiseTheme();
    else if (ololanTheme.compare("Dark brown") == 0)
        setDarkBrownTheme();
    else if (ololanTheme.compare("Private") == 0)
        setPrivateTheme();
}

void ODownloadWidget::updateTransfertRate(const int index, const QString rate, const QString size, const int percentage)
{
    QString script = QLatin1String("updateTransferRate('%1', '%2', '%3', '%4');").arg(QString::number(index), rate, size, QString::number(percentage));
    widgetWebView->page()->runJavaScript(script);
}

void ODownloadWidget::removeInactiveItem(const int index)
{
    widgetWebView->page()->runJavaScript(QLatin1String("deleteDownloadItem('%1');").arg(QString::number(index)));
    if (height() > 165)
    {
        setFixedHeight(height()-74);
        widgetWebView->setFixedHeight(widgetWebView->height()-74);
    }

    if (height() < 165)
    {
        setFixedHeight(165);
        widgetWebView->setFixedHeight(163);
    }
}

void ODownloadWidget::clearWidgetItemList()
{
    widgetWebView->page()->runJavaScript(QLatin1String("clearDownloadsList();"));
}

void ODownloadWidget::loadEmptyView(bool state)
{
    QString itemScript = QLatin1String("loadEmptyWidgetView(%1);").arg((state ? "true" : "false"));
    widgetWebView->page()->runJavaScript(itemScript);
}

void ODownloadWidget::showEvent(QShowEvent *event)
{
    if (height() > 700)
    {
        setFixedHeight(700);
        widgetWebView->setFixedHeight(698);
    }

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

void ODownloadWidget::setLightGrayTheme()
{
    if (isReady)
        widgetWebView->page()->runJavaScript(QLatin1String("setLightGrayTheme();"));
    ololanTheme = "Light gray";
    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #dfdfe1;}"));
}

void ODownloadWidget::setLightTurquoiseTheme()
{
    if (isReady)
        widgetWebView->page()->runJavaScript(QLatin1String("setLightTurquoiseTheme();"));
    ololanTheme = "Light turquoise";
    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #dfdfe1;}"));
}

void ODownloadWidget::setLightBrownTheme()
{
    if (isReady)
        widgetWebView->page()->runJavaScript(QLatin1String("setLightBrownTheme();"));
    ololanTheme = "Light brown";
    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #dfdfe1;}"));
}

void ODownloadWidget::setDarkGrayTheme()
{
    if (isReady)
        widgetWebView->page()->runJavaScript(QLatin1String("setDarkGrayTheme();"));
    ololanTheme = "Dark gray";
    setStyleSheet(QLatin1String("QMenu {background-color: #434344; padding: 0px; border: 1px solid #434344;}"));
}

void ODownloadWidget::setDarkTurquoiseTheme()
{
    if (isReady)
        widgetWebView->page()->runJavaScript(QLatin1String("setDarkTurquoiseTheme();"));
    ololanTheme = "Dark turquoise";
    setStyleSheet(QLatin1String("QMenu {background-color: #1e4446; padding: 0px; border: 1px solid #1e4446;}"));
}

void ODownloadWidget::setDarkBrownTheme()
{
    if (isReady)
        widgetWebView->page()->runJavaScript(QLatin1String("setDarkBrownTheme();"));
    ololanTheme = "Dark brown";
    setStyleSheet(QLatin1String("QMenu {background-color: #462c21; padding: 0px; border: 1px solid #462c21;}"));
}

void ODownloadWidget::setPrivateTheme()
{
    if (isReady)
        widgetWebView->page()->runJavaScript(QLatin1String("setPrivateTheme();"));
    ololanTheme = "Private";
    setStyleSheet(QLatin1String("QMenu {background-color: #3c3244; padding: 0px; border: 1px solid #3c3244;}"));
}

void ODownloadWidget::setOlolanTheme(QString themeColor)
{
    ololanTheme = themeColor;
}

void ODownloadWidget::restoreHeight()
{
    setFixedHeight(165);
    widgetWebView->setFixedHeight(163);
}
