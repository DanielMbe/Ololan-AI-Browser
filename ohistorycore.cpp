#include "ohistorycore.h"
#include "oappcore.h"
#include "customwindow.h"
#include "otabpage.h"
#include <QWebEngineHistory>
#include <QWebEngineScript>
#include <QSqlError>
#include <QFile>
#include <QBuffer>
#include <QTimer>

OHistoryItem::OHistoryItem(const QUrl url, const QString title, const QByteArray icon, const QDateTime lastVisited, const int index)
    :QObject()
{
    setUrl(url);
    setTitle(title);
    setIcon(icon);
    setLastVisited(lastVisited);
    setIndex(index);
}

const QUrl OHistoryItem::getUrl()
{
    return itemUrl;
}

const QString OHistoryItem::getTitle()
{
    return itemTitle;
}

const QByteArray OHistoryItem::getIcon()
{
    return itemIcon;
}

const QDateTime OHistoryItem::getLastVisited()
{
    return itemDateTime;
}

const QString OHistoryItem::getDeviceName()
{
    return deviceName;
}

const int OHistoryItem::getIndex()
{
    return itemIndex;
}

void OHistoryItem::setUrl(const QUrl url)
{
    itemUrl = url;
}

void OHistoryItem::setTitle(const QString title)
{
    itemTitle = title;
}

void OHistoryItem::setIcon(const QByteArray icon)
{
    itemIcon = icon;
}

void OHistoryItem::setLastVisited(const QDateTime lastVisited)
{
    itemDateTime = lastVisited;
}

void OHistoryItem::setDeviceName(const QString device)
{
    deviceName = device;
}

void OHistoryItem::setIndex(const int index)
{
    itemIndex = index;
}

//history manager

OHistoryCore::OHistoryCore(QObject *parent)
    : QObject(parent)
{
    setupIndexList();
    historyItemsList = QList<OHistoryItem*>();

    webChannel = new QWebChannel(this);
    webChannel->registerObject("historyManager", this);
    historyWebView = nullptr;

    QString databaseName = OAppCore::applicationDirPath().append("/browserDBDir/ololanhistorydatabase.gdb");
    historyDatabase = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), QLatin1String("ololanHistoryDatabase"));
    historyDatabase.setDatabaseName(databaseName);
    historyDatabase.setHostName(QLatin1String("ololanhost"));
    historyDatabase.setUserName(QLatin1String("ololanRoot"));
    historyDatabase.setPassword(QLatin1String("OlolanBrowserDB2022"));

    if (historyDatabase.open(QLatin1String("ololanRoot"), QLatin1String("OlolanBrowserDB2022")))
    {
        query = QSqlQuery(historyDatabase);

        QString statementA1 = QLatin1String("SELECT histid, histurl, histtitle, histicon, histdatetime FROM ololanhistory");
        if (query.exec(statementA1))
        {
            while (query.next())
            {
                OHistoryItem *item = new OHistoryItem;
                item->setIndex(query.value(0).toInt());
                item->setUrl(QUrl::fromEncoded(query.value(1).toByteArray()));
                item->setTitle(query.value(2).toByteArray());
                item->setIcon(query.value(3).toByteArray());
                item->setLastVisited(QDateTime::fromString(query.value(4).toByteArray()));
                historyItemsList.prepend(item);
                historyIndexList.removeOne(item->getIndex());
            }
        }
        else
        {
            qDebug(query.lastError().text().toLatin1());
            QString statementA2 = QLatin1String("CREATE TABLE ololanhistory(histid SMALLINT UNSIGNED NOT NULL, histurl VARCHAR(2048) NOT NULL, histtitle VARCHAR(256) NOT NULL,"
                                                "histicon VARCHAR(8192) NOT NULL, histdatetime VARCHAR(128) NOT NULL, PRIMARY KEY (histid))");
            if (!query.exec(statementA2))
                qDebug(query.lastError().text().toLatin1());
        }
    }
    else
        qDebug(historyDatabase.lastError().text().toLatin1());
}

void OHistoryCore::addItem(OHistoryItem *item)
{
    historyItemsList.prepend(item);

    if (!historyDatabase.isOpenError())
    {
        QString statement = QLatin1String("INSERT INTO ololanhistory (histid, histurl, histtitle, histicon, histdatetime) VALUES(:histid, :histurl, :histtitle, :histicon, :histdatetime)");
        query.prepare(statement);
        query.bindValue(":histid", item->getIndex());
        query.bindValue(":histurl", item->getUrl().toEncoded());
        query.bindValue(":histtitle", item->getTitle().toLatin1());
        query.bindValue(":histicon", item->getIcon());
        query.bindValue(":histdatetime", item->getLastVisited().toString().toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
    }
    else
        qDebug(historyDatabase.lastError().text().toLatin1());
}

void OHistoryCore::saveWebIcon(const QIcon &icon)
{
    OWebView *webPage = qobject_cast<OWebView*>(sender());
    disconnect(webPage, &OWebView::iconChanged, this, &OHistoryCore::saveWebIcon);

    for (int i = 0; historyItemsList.count() > i; i++)
    {
        if (historyItemsList.at(i)->getUrl() == webPage->url())
        {
            if (!historyDatabase.isOpenError())
            {
                QByteArray bytes;
                QBuffer buffer(&bytes);

                if (buffer.open(QIODevice::WriteOnly))
                {
                    if(!icon.pixmap(16, 16).save(&buffer, "PNG"))
                        qDebug("error saving icon file history savewebicon");
                }
                else
                    qDebug("buffer failed to open");

                if (buffer.isOpen())
                    buffer.close();

                QString statement = QLatin1String("UPDATE ololanhistory SET histicon = :histicon WHERE histurl = :histurl");
                query.prepare(statement);
                query.bindValue(":histicon", bytes.toBase64());
                query.bindValue(":histurl", webPage->url().toEncoded());

                if (!query.exec())
                    qDebug(query.lastError().text().toLatin1());

                historyItemsList.at(i)->setIcon(bytes.toBase64());
            }
            else
                qDebug(historyDatabase.lastError().text().toLatin1());

            break;
        }
    }
}

void OHistoryCore::removeHistoryItem(const QString index)
{
    for (int i = 0; i < historyItemsList.count(); i++)
        if (historyItemsList.at(i)->getIndex() == index.toInt())
        {
            if (!historyDatabase.isOpenError())
            {
                QString statement = QLatin1String("DELETE FROM ololanhistory WHERE histid = :histid");
                query.prepare(statement);
                query.bindValue(":histid", historyItemsList.at(i)->getIndex());

                if (!query.exec())
                    qDebug(query.lastError().text().toLatin1());

                historyIndexList.append(historyItemsList.at(i)->getIndex());
                historyItemsList.at(i)->deleteLater();
                historyItemsList.removeAt(i);
                break;
            }
            else
                qDebug(historyDatabase.lastError().text().toLatin1());
        }
    if (historyItemsList.count() == 0)
        loadEmptyHistoryView();
}

void OHistoryCore::pageLoadFinished(bool status)
{
    if(!status)
        return;

    OWebView *webPage = qobject_cast<OWebView*>(sender());
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    OHistoryItem *item = new OHistoryItem;
    QString historyUrl = QLatin1String("qrc:/web_applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web_applications/html/settingsView.html");
    QString newTabUrl = QLatin1String("qrc:/web_applications/html/newtabView.html");
    QString privateTabUrl = QLatin1String("qrc:/web_applications/html/privatetabView.html");
    QString aboutUrl = QLatin1String("about:blank");

    if (historyUrl.compare(webPage->url().toString()) == 0)
    {
        historyWebView = webPage;
        historyWebView->page()->setWebChannel(webChannel);
        QString itemScript = QLatin1String("setupConnector();");
        historyWebView->page()->runJavaScript(itemScript);
        loadHistoryItems();
    }
    else if ((webPage->url().toString().compare(newTabUrl) != 0) && (webPage->url().toString().compare(bookmarksUrl) != 0) &&
             (webPage->url().toString().compare(downloadsUrl) != 0) && (webPage->url().toString().compare(settingsUrl) != 0) &&
             (webPage->url().toString().compare(privateTabUrl) != 0) && (webPage->url().toString().compare(aboutUrl) != 0) &&
             !webPage->url().toString().startsWith("https://accounts.ololan.com/closeololanscript", Qt::CaseInsensitive) &&
             !webPage->url().toString().startsWith("https://accounts.ololan.com/checkaccount", Qt::CaseInsensitive) &&
             !webPage->url().toString().startsWith("https://accounts.ololan.com/loggedout", Qt::CaseInsensitive))
    {
        connect(webPage, &OWebView::iconChanged, this, &OHistoryCore::saveWebIcon);
        item->setUrl(webPage->url());
        item->setTitle(webPage->title());
        item->setIcon("EmPtY64");
        item->setLastVisited(webPage->page()->history()->currentItem().lastVisited());
        item->setIndex(generateIndex());
        addItem(item);

        if (appCore->isHistoryOpened())
            updateHistoryView();
    }
}

void OHistoryCore::clearHistory()
{
    if (!historyDatabase.isOpenError())
    {
        QString statement = QLatin1String("DELETE FROM ololanhistory");
        query.prepare(statement);

        if (!query.exec())
        {
            qDebug(query.lastError().text().toLatin1());
            return;
        }
        historyItemsList.clear();
        loadEmptyHistoryView();
    }
}

void OHistoryCore::loadHistoryItems()
{
    if (historyItemsList.count() == 0)
        loadEmptyHistoryView();
    else if (!historyDatabase.isOpenError())
    {
        loadEmptyHistoryView(false);
        QList<QDate> dateList;
        bool primaryDate = false;

        for (int i = 0; i < historyItemsList.count(); i++)
            if (!dateList.contains(historyItemsList.at(i)->getLastVisited().date()))
                dateList.append(historyItemsList.at(i)->getLastVisited().date());

        for (int i = 0; i < dateList.count(); i++)
        {
            if (i < 2)
                primaryDate = true;
            else
                primaryDate = false;
            //itemDate.at(0) = day, itemDate.at(1) = month, itemDate.at(2) = year
            QStringList itemDate = getDateAsString(dateList.at(i), primaryDate);
            QString dateScript = QLatin1String("loadVisitedDate('%1', '%2', '%3', 'add');").arg(itemDate.at(0), itemDate.at(1), itemDate.at(2));
            historyWebView->page()->runJavaScript(dateScript);

            for (int j = 0; j < historyItemsList.count(); j++)
                if (historyItemsList.at(j)->getLastVisited().date() == dateList.at(i))
                {
                    OHistoryItem *item = historyItemsList.at(j);
                    QString hours = getTimeAsString(item->getLastVisited().time());
                    QString iconPath = QLatin1String("qrc:/images/webpage.png");

                    if (item->getIcon() != "EmPtY64")
                        iconPath = QLatin1String("data:image/png;base64,%1").arg(item->getIcon());

                    if (item->getUrl().toEncoded().contains("'"))
                    {
                        QString itemScript = "loadHistoryItem(";
                        for (int i = 1; i < 7; i++)
                        {
                            itemScript += '"';
                            itemScript += '%';
                            itemScript += QString::number(i);
                            itemScript += '"';

                            if (i < 6)
                                itemScript += ',';

                            if (i == 6)
                                itemScript += ");";
                        }

                        itemScript = itemScript.arg(iconPath, item->getUrl().host(), item->getUrl().toEncoded(), item->getTitle(), hours, QString::number(item->getIndex()));
                        historyWebView->page()->runJavaScript(itemScript);
                    }
                    else if (item->getUrl().toEncoded().contains('"'))
                    {
                        QString itemScript = "loadHistoryItem(";
                        for (int i = 1; i < 7; i++)
                        {
                            itemScript += "'%";
                            itemScript += QString::number(i);
                            itemScript += "'";

                            if (i < 6)
                                itemScript += ',';

                            if (i == 6)
                                itemScript += ");";
                        }

                        itemScript = itemScript.arg(iconPath, item->getUrl().host(), item->getUrl().toEncoded(), item->getTitle(), hours, QString::number(item->getIndex()));
                        historyWebView->page()->runJavaScript(itemScript);
                    }
                    else
                    {
                        QString itemScript = QLatin1String("loadHistoryItem('%1', '%2', '%3', '%4', '%5', '%6');").arg(iconPath, item->getUrl().host(), item->getUrl().toEncoded(), item->getTitle(), hours, QString::number(item->getIndex()));
                        historyWebView->page()->runJavaScript(itemScript);
                    }
                }
        }
    }
}

void OHistoryCore::updateHistoryView()
{
    if (!historyDatabase.isOpenError())
    {
        if (historyItemsList.count() > 1)
        {
            if ((historyItemsList.first()->getLastVisited().date().year() != historyItemsList.at(1)->getLastVisited().date().year()) ||
                (historyItemsList.first()->getLastVisited().date().month() != historyItemsList.at(1)->getLastVisited().date().month()) ||
                (historyItemsList.first()->getLastVisited().date().day() != historyItemsList.at(1)->getLastVisited().date().day()))
            {
                QStringList itemDate = getDateAsString(historyItemsList.first()->getLastVisited().date(), true);
                QString dateScript = QLatin1String("loadVisitedDate('%1', '%2', '%3', 'insert');").arg(itemDate.at(0), itemDate.at(1), itemDate.at(2));
                historyWebView->page()->runJavaScript(dateScript);
            }

            OHistoryItem *item = historyItemsList.first();
            QString hours = getTimeAsString(item->getLastVisited().time());
            QString iconPath = QLatin1String("qrc:/images/webpage.png");

            if (item->getIcon() != "EmPtY64")
                iconPath = QLatin1String("data:image/png;base64,%1").arg(item->getIcon());

            if (item->getUrl().toEncoded().contains("'"))
            {
                QString itemScript = "insertHistoryItem(";
                for (int i = 1; i < 7; i++)
                {
                    itemScript += '"';
                    itemScript += '%';
                    itemScript += QString::number(i);
                    itemScript += '"';

                    if (i < 6)
                        itemScript += ',';

                    if (i == 6)
                        itemScript += ");";
                }

                itemScript = itemScript.arg(iconPath, item->getUrl().host(), item->getUrl().toEncoded(), item->getTitle(), hours, QString::number(item->getIndex()));
                historyWebView->page()->runJavaScript(itemScript);
            }
            else if (item->getUrl().toEncoded().contains('"'))
            {
                QString itemScript = "insertHistoryItem(";
                for (int i = 1; i < 7; i++)
                {
                    itemScript += "'%";
                    itemScript += QString::number(i);
                    itemScript += "'";

                    if (i < 6)
                        itemScript += ',';

                    if (i == 6)
                        itemScript += ");";
                }

                itemScript = itemScript.arg(iconPath, item->getUrl().host(), item->getUrl().toEncoded(), item->getTitle(), hours, QString::number(item->getIndex()));
                historyWebView->page()->runJavaScript(itemScript);
            }
            else
            {
                QString itemScript = QLatin1String("insertHistoryItem('%1', '%2', '%3', '%4', '%5', '%6');").arg(iconPath, item->getUrl().host(), item->getUrl().toEncoded(), item->getTitle(), hours, QString::number(item->getIndex()));
                historyWebView->page()->runJavaScript(itemScript);
            }
        }
        else
        {
            QStringList itemDate = getDateAsString(historyItemsList.first()->getLastVisited().date(), true);
            QString dateScript = QLatin1String("loadVisitedDate('%1', '%2', '%3', 'add');").arg(itemDate.at(0), itemDate.at(1), itemDate.at(2));
            historyWebView->page()->runJavaScript(dateScript);

            OHistoryItem *item = historyItemsList.first();
            QString hours = getTimeAsString(item->getLastVisited().time());
            QString iconPath = QLatin1String("qrc:/images/webpage.png");

            if (item->getIcon() != "EmPtY64")
                iconPath = QLatin1String("data:image/png;base64,%1").arg(item->getIcon());

            if (item->getUrl().toEncoded().contains("'"))
            {
                QString itemScript = "loadHistoryItem(";
                for (int i = 1; i < 7; i++)
                {
                    itemScript += '"';
                    itemScript += '%';
                    itemScript += QString::number(i);
                    itemScript += '"';

                    if (i < 6)
                        itemScript += ',';

                    if (i == 6)
                        itemScript += ");";
                }

                itemScript = itemScript.arg(iconPath, item->getUrl().host(), item->getUrl().toEncoded(), item->getTitle(), hours, QString::number(item->getIndex()));
                historyWebView->page()->runJavaScript(itemScript);
            }
            else if (item->getUrl().toEncoded().contains('"'))
            {
                QString itemScript = "loadHistoryItem(";
                for (int i = 1; i < 7; i++)
                {
                    itemScript += "'%";
                    itemScript += QString::number(i);
                    itemScript += "'";

                    if (i < 6)
                        itemScript += ',';

                    if (i == 6)
                        itemScript += ");";
                }

                itemScript = itemScript.arg(iconPath, item->getUrl().host(), item->getUrl().toEncoded(), item->getTitle(), hours, QString::number(item->getIndex()));
                historyWebView->page()->runJavaScript(itemScript);
            }
            else
            {
                QString itemScript = QLatin1String("loadHistoryItem('%1', '%2', '%3', '%4', '%5', '%6');").arg(iconPath, item->getUrl().host(), item->getUrl().toEncoded(), item->getTitle(), hours, QString::number(item->getIndex()));
                historyWebView->page()->runJavaScript(itemScript);
            }
        }
    }
}

const QStringList OHistoryCore::getDateAsString(const QDate date, bool primaryDate)
{
    QString day = "";
    QDate today = QDate::currentDate();

    if (primaryDate)
    {
        if (date == today)
            day = "Today - ";
        else if (today.year() == (date.year()+1))
        {
            if ((date.month() == 12) && (today.month() == 1))
                if ((date.day() == 31) && (today.day() == 1))
                    day = "Yesterday - ";
        }
        else if (today.year() == date.year())
        {
            if (today.month() == date.month())
            {
                if (today.day() == (date.day()+1))
                    day = "Yesterday - ";
            }
            else if (today.month() == (date.month()+1))
            {
                if (date.daysInMonth() == 28)
                {
                    if ((today.day() == 1) && (date.day() == 28))
                        day = "Yesterday - ";
                }
                else if (date.daysInMonth() == 29)
                {
                    if ((today.day() == 1) && (date.day() == 29))
                        day = "Yesterday - ";
                }
                else if (date.daysInMonth() == 30)
                {
                    if ((today.day() == 1) && (date.day() == 30))
                        day = "Yesterday - ";
                }
                else if (date.daysInMonth() == 31)
                {
                    if ((today.day() == 1) && (date.day() == 31))
                        day = "Yesterday - ";
                }
            }
        }
    }

    day += getDayAsString(date.dayOfWeek());
    QString month = getMonthAsString(date.month());
    month += " ";
    month += QString::number(date.day());
    QString year = QString::number(date.year());
    QStringList list = {day, month, year};

    return list;
}

const QString OHistoryCore::getDayAsString(const int day)
{
    QString dayOfWeek = "";

    if (day == 1)
        dayOfWeek = QLatin1String("Monday");
    else if (day == 2)
        dayOfWeek = QLatin1String("Tuesday");
    else if (day == 3)
        dayOfWeek = QLatin1String("Wednesday");
    else if (day == 4)
        dayOfWeek = QLatin1String("Thursday");
    else if (day == 5)
        dayOfWeek = QLatin1String("Friday");
    else if (day == 6)
        dayOfWeek = QLatin1String("Saturday");
    else if (day == 7)
        dayOfWeek = QLatin1String("Sunday");

    return dayOfWeek;
}

const QString OHistoryCore::getMonthAsString(const int month)
{
    QString monthOfTheYear = "";

    if (month == 1)
        monthOfTheYear = QLatin1String("January");
    else if (month == 2)
        monthOfTheYear = QLatin1String("February");
    else if (month == 3)
        monthOfTheYear = QLatin1String("March");
    else if (month == 4)
        monthOfTheYear = QLatin1String("April");
    else if (month == 5)
        monthOfTheYear = QLatin1String("May");
    else if (month == 6)
        monthOfTheYear = QLatin1String("June");
    else if (month == 7)
        monthOfTheYear = QLatin1String("July");
    else if (month == 8)
        monthOfTheYear = QLatin1String("August");
    else if (month == 9)
        monthOfTheYear = QLatin1String("September");
    else if (month == 10)
        monthOfTheYear = QLatin1String("October");
    else if (month == 11)
        monthOfTheYear = QLatin1String("November");
    else if (month == 12)
        monthOfTheYear = QLatin1String("December");

    return monthOfTheYear;
}

const QString OHistoryCore::getTimeAsString(const QTime time)
{
    QString hour;
    QString minutes = QString::number(time.minute());

    if (time.minute() < 10)
        minutes = minutes.prepend("0");

    if (time.hour() < 12)
    {
        if (time.hour() == 0)
            hour = "12";
        else
            hour = QString::number(time.hour());

        hour += ":";
        minutes += " AM";
    }
    else
    {
        if (time.hour() == 12)
            hour = QString::number(12);
        else if (time.hour() == 13)
            hour = QString::number(1);
        else if (time.hour() == 14)
            hour = QString::number(2);
        else if (time.hour() == 15)
            hour = QString::number(3);
        else if (time.hour() == 16)
            hour = QString::number(4);
        else if (time.hour() == 17)
            hour = QString::number(5);
        else if (time.hour() == 18)
            hour = QString::number(6);
        else if (time.hour() == 19)
            hour = QString::number(7);
        else if (time.hour() == 20)
            hour = QString::number(8);
        else if (time.hour() == 21)
            hour = QString::number(9);
        else if (time.hour() == 22)
            hour = QString::number(10);
        else if (time.hour() == 23)
            hour = QString::number(11);
        hour += ":";
        minutes += " PM";
    }

    return (hour+=minutes);
}

void OHistoryCore::loadEmptyHistoryView(bool state)
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    if (appCore->isHistoryOpened())
    {
        QString itemScript = QLatin1String("historyViewEmpty(%1);").arg((state ? "true" : "false"));
        historyWebView->page()->runJavaScript(itemScript);

        if (!state)
            historyWebView->page()->runJavaScript(QLatin1String("clearAllHistoryView();"));
    }
}

void OHistoryCore::setHistoryWebView(OWebView *webView)
{
    historyWebView = webView;
}

OWebView *OHistoryCore::getHistoryWebView()
{
    return historyWebView;
}

void OHistoryCore::saveDatabase()
{
    historyDatabase.close();
}

QList<OHistoryItem*> OHistoryCore::getHistoryItemsList()
{
    return historyItemsList;
}

void OHistoryCore::searchInHistory(const QString &value)
{
    loadEmptyHistoryView(false);
    QList<QDate> dateList;
    bool primaryDate = false;
    bool noResult = false;

    for (int i = 0; i < historyItemsList.count(); i++)
        if (historyItemsList.at(i)->getUrl().toString().contains(value, Qt::CaseInsensitive))
            if (!dateList.contains(historyItemsList.at(i)->getLastVisited().date()))
                dateList.append(historyItemsList.at(i)->getLastVisited().date());

    for (int i = 0; i < dateList.count(); i++)
    {
        if (i < 2)
            primaryDate = true;
        else
            primaryDate = false;
        //itemDate.at(0) = day, itemDate.at(1) = month, itemDate.at(2) = year
        QStringList itemDate = getDateAsString(dateList.at(i), primaryDate);
        QString dateScript = QLatin1String("loadVisitedDate('%1', '%2', '%3', 'add');").arg(itemDate.at(0), itemDate.at(1), itemDate.at(2));
        historyWebView->page()->runJavaScript(dateScript);

        for (int j = 0; j < historyItemsList.count(); j++)
            if (historyItemsList.at(j)->getUrl().toString().contains(value, Qt::CaseInsensitive) && (historyItemsList.at(j)->getLastVisited().date() == dateList.at(i)))
            {
                OHistoryItem *item = historyItemsList.at(j);
                QString hours = getTimeAsString(item->getLastVisited().time());
                QString iconPath = QLatin1String("qrc:/images/webpage.png");

                if (item->getIcon() != "EmPtY64")
                    iconPath = QLatin1String("data:image/png;base64,%1").arg(item->getIcon());

                QString itemScript = QLatin1String("loadHistoryItem('%1', '%2', '%3', '%4', '%5', '%6');").arg(iconPath, item->getUrl().host(), item->getUrl().toEncoded(), item->getTitle(), hours, QString::number(item->getIndex()));
                historyWebView->page()->runJavaScript(itemScript);
                noResult = true;
            }
    }

    if (!noResult && !value.isEmpty())
        historyWebView->page()->runJavaScript("noHistoryResult();");
    else if (!noResult && value.isEmpty() && historyItemsList.isEmpty())
        loadEmptyHistoryView();
}

const int OHistoryCore::generateIndex()
{
    const int index = historyIndexList.first();
    historyIndexList.removeFirst();
    return index;
}

void OHistoryCore::setupIndexList()
{
    for (int i = 1; i < 102410; i++)
        historyIndexList.append(i);
}

const int OHistoryCore::getHistoryDataSize()
{
    QByteArray space;
    QDataStream stream(&space, QIODevice::WriteOnly);
    for (OHistoryItem *item : historyItemsList)
        stream.writeRawData(QString(item->getDeviceName()+item->getTitle()+item->getUrl().toString()+item->getLastVisited().toString()).toStdString().data(), sizeof(item));
    return space.size();
}

void OHistoryCore::openDateInNewTab(QString date)
{
    QDate stringDate = QDate::fromString(date, "MMMM dd, yyyy");
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    OTabPage *tabPage = static_cast<OTabPage*>(appCore->allWindowsList().at(appCore->getCurrentWindowIndex())->getCentralWidget());
    int openTime = 0;

    for (int i = 0; i < historyItemsList.count(); i++)
    {
        if (historyItemsList.at(i)->getLastVisited().date() == stringDate)
        {
            openTime++;
            const QString url = historyItemsList.at(i)->getUrl().toString();
            QTimer *timer = new QTimer(this);
            timer->setInterval(openTime*100);
            connect(timer, &QTimer::timeout, this, [=]() { tabPage->openInTab(url); timer->stop(); });
            timer->start();
        }
    }
}

void OHistoryCore::openDateInNewWindow(QString date)
{
    QDate stringDate = QDate::fromString(date, "MMMM dd, yyyy");
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    int count = 0;
    int openTime = 1;

    for (int i = 0; i < historyItemsList.count(); i++)
    {
        if (historyItemsList.at(i)->getLastVisited().date() == stringDate)
        {
            if (count == 0)
                appCore->openInWindow(historyItemsList.at(i)->getUrl().toString());
            else
            {
                openTime++;
                OTabPage *tabPage = static_cast<OTabPage*>(appCore->allWindowsList().last()->getCentralWidget());
                const QString url = historyItemsList.at(i)->getUrl().toString();
                QTimer *timer = new QTimer(this);
                timer->setInterval(openTime*100);
                connect(timer, &QTimer::timeout, this, [=]() { tabPage->openInTab(url); timer->stop(); });
                timer->start();
            }
            count++;
        }
    }
}

void OHistoryCore::openDateInPrivateWindow(QString date)
{
    QDate stringDate = QDate::fromString(date, "MMMM dd, yyyy");
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    int count = 0;
    int openTime = 1;

    for (int i = 0; i < historyItemsList.count(); i++)
    {
        if (historyItemsList.at(i)->getLastVisited().date() == stringDate)
        {
            if (count == 0)
                appCore->openInPrivateWindow(historyItemsList.at(i)->getUrl().toString());
            else
            {
                openTime++;
                OTabPage *tabPage = static_cast<OTabPage*>(appCore->allWindowsList().last()->getCentralWidget());
                const QString url = historyItemsList.at(i)->getUrl().toString();
                QTimer *timer = new QTimer(this);
                timer->setInterval(openTime*100);
                connect(timer, &QTimer::timeout, this, [=]() { tabPage->openInTab(url); timer->stop(); });
                timer->start();
            }
            count++;
        }
    }
}

void OHistoryCore::openHistoryInNewTab()
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    OTabPage *tabPage = static_cast<OTabPage*>(appCore->allWindowsList().at(appCore->getCurrentWindowIndex())->getCentralWidget());
    int openTime = 0;

    for (int i = 0; i < historyItemsList.count(); i++)
    {
        openTime++;
        const QString url = historyItemsList.at(i)->getUrl().toString();
        QTimer *timer = new QTimer(this);
        timer->setInterval(openTime*100);
        connect(timer, &QTimer::timeout, this, [=]() { tabPage->openInTab(url); timer->stop(); });
        timer->start();
    }
}

void OHistoryCore::openHistoryInNewWindow()
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    int count = 0;
    int openTime = 1;

    for (int i = 0; i < historyItemsList.count(); i++)
    {
        if (count == 0)
            appCore->openInWindow(historyItemsList.at(i)->getUrl().toString());
        else
        {
            openTime++;
            OTabPage *tabPage = static_cast<OTabPage*>(appCore->allWindowsList().last()->getCentralWidget());
            const QString url = historyItemsList.at(i)->getUrl().toString();
            QTimer *timer = new QTimer(this);
            timer->setInterval(openTime*100);
            connect(timer, &QTimer::timeout, this, [=]() { tabPage->openInTab(url); timer->stop(); });
            timer->start();
        }
        count++;
    }
}

void OHistoryCore::openHistoryInPrivateWindow()
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    int count = 0;
    int openTime = 1;

    for (int i = 0; i < historyItemsList.count(); i++)
    {
        if (count == 0)
            appCore->openInPrivateWindow(historyItemsList.at(i)->getUrl().toString());
        else
        {
            openTime++;
            OTabPage *tabPage = static_cast<OTabPage*>(appCore->allWindowsList().last()->getCentralWidget());
            const QString url = historyItemsList.at(i)->getUrl().toString();
            QTimer *timer = new QTimer(this);
            timer->setInterval(openTime*100);
            connect(timer, &QTimer::timeout, this, [=]() { tabPage->openInTab(url); timer->stop(); });
            timer->start();
        }
        count++;
    }
}

void OHistoryCore::openOccurenceInNewTab(QString page)
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    OTabPage *tabPage = static_cast<OTabPage*>(appCore->allWindowsList().at(appCore->getCurrentWindowIndex())->getCentralWidget());
    int openTime = 0;

    for (int i = 0; i < historyItemsList.count(); i++)
    {
        if (historyItemsList.at(i)->getTitle().startsWith(page, Qt::CaseInsensitive) ||
                historyItemsList.at(i)->getTitle().contains(page, Qt::CaseInsensitive))
        {
            openTime++;
            const QString url = historyItemsList.at(i)->getUrl().toString();
            QTimer *timer = new QTimer(this);
            timer->setInterval(openTime*100);
            connect(timer, &QTimer::timeout, this, [=]() { tabPage->openInTab(url); timer->stop(); });
            timer->start();
        }
    }
}

void OHistoryCore::openOccurenceInNewWindow(QString page)
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    int count = 0;
    int openTime = 1;

    for (int i = 0; i < historyItemsList.count(); i++)
    {
        if (historyItemsList.at(i)->getTitle().startsWith(page, Qt::CaseInsensitive) ||
            historyItemsList.at(i)->getTitle().contains(page, Qt::CaseInsensitive))
        {
            if (count == 0)
                appCore->openInWindow(historyItemsList.at(i)->getUrl().toString());
            else
            {
                openTime++;
                OTabPage *tabPage = static_cast<OTabPage*>(appCore->allWindowsList().last()->getCentralWidget());
                const QString url = historyItemsList.at(i)->getUrl().toString();
                QTimer *timer = new QTimer(this);
                timer->setInterval(openTime*100);
                connect(timer, &QTimer::timeout, this, [=]() { tabPage->openInTab(url); timer->stop(); });
                timer->start();
            }
            count++;
        }
    }
}

void OHistoryCore::openOccurenceInPrivateWindow(QString page)
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    int count = 0;
    int openTime = 1;

    for (int i = 0; i < historyItemsList.count(); i++)
    {
        if (historyItemsList.at(i)->getTitle().startsWith(page, Qt::CaseInsensitive) ||
            historyItemsList.at(i)->getTitle().contains(page, Qt::CaseInsensitive))
        {
            if (count == 0)
                appCore->openInPrivateWindow(historyItemsList.at(i)->getUrl().toString());
            else
            {
                openTime++;
                OTabPage *tabPage = static_cast<OTabPage*>(appCore->allWindowsList().last()->getCentralWidget());
                const QString url = historyItemsList.at(i)->getUrl().toString();
                QTimer *timer = new QTimer(this);
                timer->setInterval(openTime*100);
                connect(timer, &QTimer::timeout, this, [=]() { tabPage->openInTab(url); timer->stop(); });
                timer->start();
            }
            count++;
        }
    }
}

void OHistoryCore::deleteByOccurence(QString page)
{
    for (int i = 0; i < historyItemsList.count(); i++)
    {
        if (historyItemsList.at(i)->getTitle().startsWith(page, Qt::CaseInsensitive) ||
            historyItemsList.at(i)->getTitle().contains(page, Qt::CaseInsensitive))
        {
            removeHistoryItem(QString::number(historyItemsList.at(i)->getIndex()));
            i--;
        }
    }

    loadEmptyHistoryView();
    QString script = QLatin1String("loadThisPCHistory();");
    historyWebView->page()->runJavaScript(script);
}

void OHistoryCore::deleteByDate(QString date)
{
    QDate stringDate = QDate::fromString(date, "MMMM dd, yyyy");
    for (int i = 0; i < historyItemsList.count(); i++)
    {
        if (historyItemsList.at(i)->getLastVisited().date() == stringDate)
        {
            removeHistoryItem(QString::number(historyItemsList.at(i)->getIndex()));
            i--;
        }
    }

    loadEmptyHistoryView();
    QString script = QLatin1String("loadThisPCHistory();");
    historyWebView->page()->runJavaScript(script);
}

bool OHistoryCore::isDateExisting(QString date)
{
    QDate stringDate = QDate::fromString(date, "MMMM dd, yyyy");
    bool exist = false;

    for (int i = 0; i < historyItemsList.count(); i++)
        if (historyItemsList.at(i)->getLastVisited().date() == stringDate)
        {
            exist = true;
            break;
        }

    return exist;
}

bool OHistoryCore::isItemExisting(QString item)
{
    bool exist = false;
    for (int i = 0; i < historyItemsList.count(); i++)
        if (historyItemsList.at(i)->getTitle().startsWith(item, Qt::CaseInsensitive) ||
            historyItemsList.at(i)->getTitle().contains(item, Qt::CaseInsensitive))
        {
            exist = true;
            break;
        }

    return exist;
}

void OHistoryCore::clearBrowsingData()
{
    clearHistory();
}
