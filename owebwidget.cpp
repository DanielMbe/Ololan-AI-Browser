#include "owebwidget.h"
#include "owebview.h"
#include "oassistantcore.h"
#include "otabpage.h"
#include <QContextMenuEvent>
#include <QWebChannel>

OWebWidget::OWebWidget(QWidget *parent)
    :QWebEngineView(parent)
{
    pageReady = false;
    ololanTheme = "";
}

void OWebWidget::contextMenuEvent(QContextMenuEvent *event)
{
    event->ignore();
}

void OWebWidget::setupAssistant(OAssistantCore *assistant)
{
    assistantManager = assistant;
    connect(this, &OWebWidget::loadFinished, this, &OWebWidget::widgetsReady);
    connect(assistant, &OAssistantCore::sendSuggestions, this, &OWebWidget::showSuggestions);
    connect(assistant, &OAssistantCore::sendAssistantSpeech, this, &OWebWidget::addAssistantSpeech);
    load(QUrl("qrc:/web_applications/html/assistantView.html"));
}

void OWebWidget::widgetsReady(bool status)
{
    pageReady = status;
    if (status)
    {
        QWebChannel *widgetWebChannel = new QWebChannel(assistantManager);
        widgetWebChannel->registerObject("assistantManager", assistantManager);
        page()->setWebChannel(widgetWebChannel);
        page()->runJavaScript(QLatin1String("setupConnector();"));

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
}

void OWebWidget::showSuggestions(const QStringList commands)
{
    QString scriptA = QLatin1String("emptySuggestionView();");
    page()->runJavaScript(scriptA);

    for (int i = 0; (i < commands.count()) && (i < 5); i++)
    {
        QString textValue = commands.at(i);
        if ((i == 0) && (textValue.length() > 6))
            textValue = QString(commands.at(i)).append(" - Press tab");

        if (textValue.length() > 6)
        {
            QString scriptB = QLatin1String("showSuggestions('%1');").arg(textValue);
            page()->runJavaScript(scriptB);
        }
        else
        {
            QString scriptC = QLatin1String("emptySuggestionView();");
            page()->runJavaScript(scriptC);
        }
    }

    if (commands.isEmpty())
    {
        QString scriptC = QLatin1String("emptySuggestionView();");
        page()->runJavaScript(scriptC);
    }
}

void OWebWidget::addAssistantSpeech(const QString speech)
{
    if (!speech.isEmpty())
    {
        QString script = QLatin1String("addAssistantSpeech('%1');").arg(speech);
        page()->runJavaScript(script);
    }
}

void OWebWidget::setTabPage(OTabPage *tPage)
{
    tabPage = tPage;
}

void OWebWidget::beforeHiding()
{
    page()->runJavaScript(QLatin1String("clearChatView();"));
    page()->runJavaScript(QLatin1String("loseFocus();"));
}

void OWebWidget::setActiveProcessor()
{
    QString historyUrl = QLatin1String("qrc:/web_applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");
    QString webUrl = tabPage->getStackPage()->currentWidget()->findChild<OWebView*>("pageView")->url().toEncoded();

    if (webUrl.compare(historyUrl) == 0)
        page()->runJavaScript(QLatin1String("setActiveProcessor('%1');").arg("historyProcessor"));
    else if (webUrl.compare(bookmarksUrl) == 0)
        page()->runJavaScript(QLatin1String("setActiveProcessor('%1');").arg("bookmarkProcessor"));
    else if (webUrl.compare(downloadsUrl) == 0)
        page()->runJavaScript(QLatin1String("setActiveProcessor('%1');").arg("downloadProcessor"));
    else
        page()->runJavaScript(QLatin1String("setActiveProcessor('%1');").arg("generalProcessor"));
}

void OWebWidget::setLightGrayTheme()
{
    if (pageReady)
        page()->runJavaScript(QLatin1String("setLightGrayTheme();"));
    ololanTheme = "Light gray";
}

void OWebWidget::setLightTurquoiseTheme()
{
    if (pageReady)
        page()->runJavaScript(QLatin1String("setLightTurquoiseTheme();"));
    ololanTheme = "Light turquoise";
}

void OWebWidget::setLightBrownTheme()
{
    if (pageReady)
        page()->runJavaScript(QLatin1String("setLightBrownTheme();"));
    ololanTheme = "Light brown";
}

void OWebWidget::setDarkGrayTheme()
{
    if (pageReady)
        page()->runJavaScript(QLatin1String("setDarkGrayTheme();"));
    ololanTheme = "Dark gray";
}

void OWebWidget::setDarkTurquoiseTheme()
{
    if (pageReady)
        page()->runJavaScript(QLatin1String("setDarkTurquoiseTheme();"));
    ololanTheme = "Dark turquoise";
}

void OWebWidget::setDarkBrownTheme()
{
    if (pageReady)
        page()->runJavaScript(QLatin1String("setDarkBrownTheme();"));
    ololanTheme = "Dark brown";
}

void OWebWidget::setPrivateTheme()
{
    if (pageReady)
        page()->runJavaScript(QLatin1String("setPrivateTheme();"));
    ololanTheme = "Private";
}
