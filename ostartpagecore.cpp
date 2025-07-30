#include "ostartpagecore.h"
#include "owebview.h"
#include "oappcore.h"
#include "ohistorycore.h"
#include <QWebChannel>

OStartPageCore::OStartPageCore(QObject *parent)
    : QObject(parent)
{
    webChannel = new QWebChannel(this);
    webChannel->registerObject("startPageManager", this);
    webView = nullptr;
}

void OStartPageCore::addLastVisitedItem()
{
    for (int i = 0;i < itemList.count(); i++)
    {
        QAction *item = itemList.at(i);
        QString iconPath = QLatin1String("qrc:/images/webpage.png");
        if (item->data().toByteArray() != "EmPtY64")
            iconPath = QLatin1String("data:image/png;base64,%1").arg(item->data().toByteArray());

        if (item->text().contains("'"))
        {
            QString itemScript = "addItem(";
            for (int i = 1; i < 4; i++)
            {
                itemScript += '"';
                itemScript += '%';
                itemScript += QString::number(i);
                itemScript += '"';

                if (i < 3)
                    itemScript += ',';

                if (i == 3)
                    itemScript += ");";
            }

            itemScript = itemScript.arg(item->toolTip(), item->text(), iconPath);
            webView->page()->runJavaScript(itemScript);
        }
        else if (item->text().contains('"'))
        {
            QString itemScript = "addItem(";
            for (int i = 1; i < 4; i++)
            {
                itemScript += "'%";
                itemScript += QString::number(i);
                itemScript += "'";

                if (i < 3)
                    itemScript += ',';

                if (i == 3)
                    itemScript += ");";
            }

            itemScript = itemScript.arg(item->toolTip(), item->text(), iconPath);
            webView->page()->runJavaScript(itemScript);
        }
        else
        {
            QString script = QLatin1String("addItem('%1', '%2', '%3');").arg(item->toolTip(), item->text(), iconPath);
            webView->page()->runJavaScript(script);
        }
    }
}

void OStartPageCore::setupLastItem(QList<OHistoryItem*> list)
{
    for (int i = 0; (i < 10) && (list.count() > i); i++)
    {
        QAction *item = new QAction(this);
        item->setToolTip(list.at(i)->getTitle());
        item->setText(list.at(i)->getUrl().toEncoded());
        item->setData(list.at(i)->getIcon());
        itemList.append(item);
    }
}

void OStartPageCore::processQuery(const QString query)
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    appCore->processWebQuery(query);
}

void OStartPageCore::setSearchEngineName(const QString name)
{
    searchEngineName = (name.contains("#*#") ? name.split("#*#").first() : name);
}

void OStartPageCore::setupStartPage(bool state)
{
    if (state)
    {
        QString newTabUrl = QLatin1String("qrc:/web_applications/html/newtabView.html");
        webView = qobject_cast<OWebView*>(sender());

        if (webView->url().toString().compare(newTabUrl) == 0)
        {
            webView->page()->setWebChannel(webChannel);
            QString itemScript = QLatin1String("setupConnector();");
            webView->page()->runJavaScript(itemScript);

            QString script = QLatin1String("setCurrentPlaceholder('%1')").arg(searchEngineName);
            webView->page()->runJavaScript(script);

            addLastVisitedItem();
        }
    }
}
