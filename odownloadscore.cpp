#include "odownloadscore.h"
#include "oappcore.h"
#include <QWebChannel>
#include <QSqlError>
#include <QDir>
#include <QProcess>
#include <cmath>
#include "odownloadwidget.h"
#include <QTimer>
#include <QFile>

ODownloadItem::ODownloadItem(QObject *parent)
    :QObject(parent)
{
    download = nullptr;
    itemTitle = "olodownload";
    itemUrl = "olourl";
    itemSize = "0 bytes";
    itemDownloadedSize = "0 bytes";
    itemIcon = "EmPtY64";
    itemState = "unknow";
}

const QString ODownloadItem::getUrl()
{
    return itemUrl;
}

const QString ODownloadItem::getTitle()
{
    return itemTitle;
}

const QString ODownloadItem::getSize()
{
    return itemSize;
}

const QString ODownloadItem::getDownloadedSize()
{
    return itemDownloadedSize;
}

const QByteArray ODownloadItem::getBytesSize()
{
    return QByteArray::number(downloadFile->size());
}

const QString ODownloadItem::getState()
{
    return itemState;
}

const QString ODownloadItem::getDownloadDirectory()
{
    return itemDir;
}

const QByteArray ODownloadItem::getIcon()
{
    return itemIcon;
}

const int ODownloadItem::getIndex()
{
    return itemIndex;
}

const QDate ODownloadItem::getDownloadDate()
{
    return itemDate;
}

const quint64 ODownloadItem::getElapsedTime()
{
    return elapsedTime.elapsed();
}

QNetworkReply *ODownloadItem::getDownloadItem()
{
    return download;
}

void ODownloadItem::saveToFile()
{
    if (downloadFile->isOpen())
        downloadFile->write(download->readAll());
    else
    {
        if (downloadFile->open(QIODevice::WriteOnly | QIODeviceBase::Append))
            downloadFile->write(download->readAll());
        else
            qDebug("Can't open download file");
    }
}

void ODownloadItem::createDownloadFile()
{
    downloadFile = new QFile(itemDir + "/" + itemTitle);
}

void ODownloadItem::setUrl(const QString url)
{
    itemUrl = url;
}

void ODownloadItem::setTitle(const QString title)
{
    itemTitle = title;
}

void ODownloadItem::setIcon(const QByteArray icon)
{
    itemIcon = icon;
}

void ODownloadItem::setSize(const QString size)
{
    itemSize = size;
}

void ODownloadItem::setState(const QString state)
{
    itemState = state;
}

void ODownloadItem::setDownloadDirectory(const QString directory)
{
    itemDir = directory;
}

void ODownloadItem::setDownloadedSize(const QString downloadedSize)
{
    itemDownloadedSize = downloadedSize;
}

void ODownloadItem::setIndex(const int index)
{
    itemIndex = index;
}

void ODownloadItem::setDownloadDate(const QDate downloadDate)
{
    itemDate = downloadDate;
}

void ODownloadItem::setDownloadItem(QNetworkReply *reply)
{
    download = reply;
    connect(reply, &QNetworkReply::readyRead, this, &ODownloadItem::saveToFile);
}

void ODownloadItem::closeFile()
{
    downloadFile->flush();
    downloadFile->close();
}

void ODownloadItem::start()
{
    itemState = "Downloading";
    elapsedTime.start();
}

void ODownloadItem::pause()
{
    downloadFile->flush();
    downloadFile->close();
    itemState = "Paused";
    elapsedTime = QElapsedTimer();
}

void ODownloadItem::stop()
{
    downloadFile->flush();
    downloadFile->close();
    itemState = "Failed";
    elapsedTime = QElapsedTimer();
}

void ODownloadItem::resume()
{
    itemState = "Downloading";
    elapsedTime.restart();
}

ODownloadsCore::ODownloadsCore(QObject *parent)
    :QObject(parent)
{
    setupIndexList();
    isReady = false;
    webChannel = new QWebChannel(this);
    webChannel->registerObject("downloadsManager", this);
    downloadWebView = nullptr;
    webManager = new QNetworkAccessManager(this);

    QString databaseName = OAppCore::applicationDirPath().append("/browserDBDir/ololandownloadsdatabase.gdb");
    downloadDatabase = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), QLatin1String("ololanDownloadsDatabase"));
    downloadDatabase.setDatabaseName(databaseName);
    downloadDatabase.setHostName(QLatin1String("ololanhost"));
    downloadDatabase.setUserName(QLatin1String("ololanRoot"));
    downloadDatabase.setPassword(QLatin1String("OlolanBrowserDB2022"));

    if (downloadDatabase.open(QLatin1String("ololanRoot"), QLatin1String("OlolanBrowserDB2022")))
    {
        query = QSqlQuery(downloadDatabase);

        QString statementA1 = QLatin1String("SELECT downid, downurl, downtitle, downicon, downsize, downdsize, downdate, downstate, downdir FROM ololandownload");
        if (query.exec(statementA1))
        {
            while (query.next())
            {
                ODownloadItem *item = new ODownloadItem;
                item->setIndex(query.value(0).toInt());
                item->setUrl(query.value(1).toByteArray());
                item->setTitle(query.value(2).toByteArray());
                item->setIcon(query.value(3).toByteArray());
                item->setSize(query.value(4).toByteArray());
                item->setDownloadedSize(query.value(5).toByteArray());
                item->setDownloadDate(QDate::fromString(query.value(6).toByteArray()));
                item->setState(query.value(7).toByteArray());
                item->setDownloadDirectory(query.value(8).toByteArray());
                item->createDownloadFile();
                downloadItemsList.prepend(item);
                downloadIndexList.removeOne(item->getIndex());
            }
        }
        else
        {
            qDebug(query.lastError().text().toLatin1());
            QString statementA2 = QLatin1String("CREATE TABLE ololandownload(downid SMALLINT UNSIGNED NOT NULL, downurl VARCHAR(2048) NOT NULL, downtitle VARCHAR(256) NOT NULL,"
                                                "downicon VARCHAR(8192) NOT NULL, downsize VARCHAR(256) NOT NULL, downdsize VARCHAR(256) NOT NULL, downdate VARCHAR(128) NOT NULL,"
                                                "downstate VARCHAR(128) NOT NULL, downdir VARCHAR(2048) NOT NULL, PRIMARY KEY (downid))");
            if (!query.exec(statementA2))
                qDebug(query.lastError().text().toLatin1());
        }
    }
    else
        qDebug(downloadDatabase.lastError().text().toLatin1());
}

void ODownloadsCore::acceptDownloadItem(QWebEngineDownloadRequest *download)
{
    if (download->state() == download->DownloadRequested)
    {
        ODownloadItem *item = new ODownloadItem(this);
        item->setIndex(generateIndex());
        item->setTitle(download->downloadFileName());
        item->setUrl(download->url().toEncoded());
        item->setIcon("EmPtY64");
        item->setDownloadDate(QDate::currentDate());
        item->setState("Downloading");
        item->setDownloadDirectory(download->downloadDirectory());
        item->createDownloadFile();
        download->cancel();
        addItem(item);
    }
}

void ODownloadsCore::addItem(ODownloadItem *item)
{
    downloadItemsList.prepend(item);
    if (!downloadDatabase.isOpenError())
    {
        QString statement = QLatin1String("INSERT INTO ololandownload (downid, downurl, downtitle, downicon, downsize, downdsize, downdate,"
                                          "downstate, downdir) VALUES(:downid, :downurl, :downtitle, :downicon, :downsize, :downdsize, :downdate, :downstate, :downdir)");
        query.prepare(statement);
        query.bindValue(":downid", item->getIndex());
        query.bindValue(":downurl", item->getUrl());
        query.bindValue(":downtitle", item->getTitle().toLatin1());
        query.bindValue(":downicon", item->getIcon());
        query.bindValue(":downsize", item->getSize());
        query.bindValue(":downdsize", item->getDownloadedSize());
        query.bindValue(":downdate", item->getDownloadDate().toString());
        query.bindValue(":downstate", "Downloading");
        query.bindValue(":downdir", item->getDownloadDirectory());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
    }
    else
        qDebug(downloadDatabase.lastError().text().toLatin1());

    OAppCore *appCore = static_cast<OAppCore*>(parent());
    if (appCore->isDownloadsOpened())
        updateDownloadsList(item);

    startDownload(QString::number(item->getIndex()));
}

void ODownloadsCore::loadDownloadItem(bool status)
{
    isReady = false;
    if (!status)
        return;
    else
    {
        downloadWebView = qobject_cast<OWebView*>(sender());
        QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");

        if (downloadWebView->url().toString().compare(downloadsUrl) == 0)
        {
            downloadWebView->page()->setWebChannel(webChannel);
            QString initScript = QLatin1String("setupConnector();");
            downloadWebView->page()->runJavaScript(initScript);

            if (downloadItemsList.count() == 0)
                loadEmptyDownloadView();
            else if (!downloadDatabase.isOpenError())
            {
                loadEmptyDownloadView(false);
                QList<QDate> dateList;
                bool primaryDate = false;

                for (int i = 0; i < downloadItemsList.count(); i++)
                    if (!dateList.contains(downloadItemsList.at(i)->getDownloadDate()))
                        dateList.append(downloadItemsList.at(i)->getDownloadDate());

                for (int i = 0; i < dateList.count(); i++)
                {
                    if (i < 2)
                        primaryDate = true;
                    else
                        primaryDate = false;
                    //itemDate.at(0) = day, itemDate.at(1) = month, itemDate.at(2) = year
                    QStringList itemDate = getDateAsString(dateList.at(i), primaryDate);
                    QString dateIndex = QString::number(dateList.at(i).day()) + QString::number(dateList.at(i).month()) + QString::number(dateList.at(i).year());
                    QString dateScript = QLatin1String("loadDownloadedDate('%1', '%2', '%3', '%4', 'add');").arg(itemDate.at(0), itemDate.at(1), itemDate.at(2), dateIndex);
                    downloadWebView->page()->runJavaScript(dateScript);

                    for (int j = 0; j < downloadItemsList.count(); j++)
                        if (downloadItemsList.at(j)->getDownloadDate() == dateList.at(i))
                        {
                            ODownloadItem *item = downloadItemsList.at(j);
                            QString result = (QFile::exists(item->getDownloadDirectory() + "/" + item->getTitle()) ? "true" : "false");
                            if (result.compare("false") == 0)
                                item->setState("Failed");

                            QString iconPath = QLatin1String("qrc:/web_applications/images/ololanLogo.png");

                            if (item->getState().compare("Downloading") == 0)
                                iconPath = QLatin1String("qrc:/web_applications/images/activeDownload.png");
                            else if (item->getState().compare("Paused") == 0)
                                iconPath = QLatin1String("qrc:/web_applications/images/pausedDownload.png");
                            else if (item->getState().compare("Failed") == 0)
                                iconPath = QLatin1String("qrc:/web_applications/images/failedDownload.png");
                            else if (item->getState().compare("Complete") == 0)
                                iconPath = QLatin1String("qrc:/web_applications/images/completeDownload.png");

                            if (item->getIcon() != "EmPtY64")
                                iconPath = QLatin1String("data:image/png;base64,%1").arg(item->getIcon());

                            QString itemScript = QLatin1String("loadDownloadItem('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8');").arg(QString::number(item->getIndex()), item->getTitle(), item->getUrl(), item->getSize(), item->getDownloadedSize(), iconPath, item->getState(), result);
                            downloadWebView->page()->runJavaScript(itemScript);
                        }
                }
            }
            isReady = true;
        }
        else
        {
            disconnect(downloadWebView, &OWebView::loadFinished, this, &ODownloadsCore::loadDownloadItem);
            OAppCore *parentApp = static_cast<OAppCore*>(parent());
            parentApp->downloadsClosed();
        }
    }
}

void ODownloadsCore::updateDownloadsList(ODownloadItem *item)
{
    if (!downloadDatabase.isOpenError())
    {
        loadEmptyDownloadView(false);
        if (downloadItemsList.count() > 1)
        {
            if ((item->getDownloadDate().year() != downloadItemsList.at(1)->getDownloadDate().year()) ||
                (item->getDownloadDate().month() != downloadItemsList.at(1)->getDownloadDate().month()) ||
                (item->getDownloadDate().day() != downloadItemsList.at(1)->getDownloadDate().day()))
            {
                QStringList itemDate = getDateAsString(item->getDownloadDate(), true);
                QString dateIndex = QString::number(item->getDownloadDate().day()) + QString::number(item->getDownloadDate().month()) + QString::number(item->getDownloadDate().year());
                QString dateScript = QLatin1String("loadDownloadedDate('%1', '%2', '%3', '%4', 'insert');").arg(itemDate.at(0), itemDate.at(1), itemDate.at(2), dateIndex);
                downloadWebView->page()->runJavaScript(dateScript);
            }

            QString iconPath = QLatin1String("qrc:/web_applications/images/ololanLogo.png");
            if (item->getState().compare("Downloading") == 0)
                iconPath = QLatin1String("qrc:/web_applications/images/activeDownload.png");
            else if (item->getState().compare("Paused") == 0)
                iconPath = QLatin1String("qrc:/web_applications/images/pausedDownload.png");
            else if (item->getState().compare("Failed") == 0)
                iconPath = QLatin1String("qrc:/web_applications/images/failedDownload.png");
            else if (item->getState().compare("Complete") == 0)
                iconPath = QLatin1String("qrc:/web_applications/images/completeDownload.png");

            if (item->getIcon() != "EmPtY64")
                iconPath = QLatin1String("data:image/png;base64,%1").arg(item->getIcon());

            QString itemScript = QLatin1String("updateDownloadsList('%1', '%2', '%3', '%4', '%5', '%6', '%7');").arg(QString::number(item->getIndex()),  item->getTitle(), item->getUrl(), item->getSize(), item->getDownloadedSize(), iconPath, item->getState());
            downloadWebView->page()->runJavaScript(itemScript);
        }
        else
        {
            QStringList itemDate = getDateAsString(downloadItemsList.first()->getDownloadDate(), true);
            QString dateIndex = QString::number(item->getDownloadDate().day()) + QString::number(item->getDownloadDate().month()) + QString::number(item->getDownloadDate().year());
            QString dateScript = QLatin1String("loadDownloadedDate('%1', '%2', '%3', '%4', 'add');").arg(itemDate.at(0), itemDate.at(1), itemDate.at(2), dateIndex);
            downloadWebView->page()->runJavaScript(dateScript);

            QString iconPath = QLatin1String("qrc:/web_applications/images/ololanLogo.png");
            if (item->getState().compare("Downloading") == 0)
                iconPath = QLatin1String("qrc:/web_applications/images/activeDownload.png");
            else if (item->getState().compare("Paused") == 0)
                iconPath = QLatin1String("qrc:/web_applications/images/pausedDownload.png");
            else if (item->getState().compare("Failed") == 0)
                iconPath = QLatin1String("qrc:/web_applications/images/failedDownload.png");
            else if (item->getState().compare("Complete") == 0)
                iconPath = QLatin1String("qrc:/web_applications/images/completeDownload.png");

            if (item->getIcon() != "EmPtY64")
                iconPath = QLatin1String("data:image/png;base64,%1").arg(item->getIcon());

            QString itemScript = QLatin1String("loadDownloadItem('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8');").arg(QString::number(item->getIndex()),  item->getTitle(), item->getUrl(), item->getSize(), item->getDownloadedSize(), iconPath, item->getState(), "true");
            downloadWebView->page()->runJavaScript(itemScript);
        }
    }
}

void ODownloadsCore::startDownload(const QString index)
{
    for (int i = 0; i < downloadItemsList.count(); i++)
        if (downloadItemsList.at(i)->getIndex() == index.toInt())
        {
            QNetworkRequest request(QUrl(downloadItemsList.at(i)->getUrl()));
            downloadItemsList.at(i)->setDownloadItem(webManager->get(request));
            downloadItemsList.at(i)->start();
            connect(downloadItemsList.at(i)->getDownloadItem(), &QNetworkReply::finished,
                    this, &ODownloadsCore::downloadFinished);
            connect(downloadItemsList.at(i)->getDownloadItem(), &QNetworkReply::downloadProgress,
                    this, &ODownloadsCore::updateTransferRate);
            break;
        }
}

void ODownloadsCore::restartDownload(const QString index)
{
    for (int i = 0; i < downloadItemsList.count(); i++)
        if (downloadItemsList.at(i)->getIndex() == index.toInt())
        {
            OAppCore *parentApp = static_cast<OAppCore*>(parent());
            parentApp->setDownloadState(true);
            startDownload(index);

            if (!downloadDatabase.isOpenError())
            {
                QString statement = QLatin1String("UPDATE ololandownload SET downstate = :downstate WHERE downid = :downid");
                query.prepare(statement);
                query.bindValue(":downstate", "Downloading");
                query.bindValue(":downid", downloadItemsList.at(i)->getIndex());

                if (!query.exec())
                    qDebug(query.lastError().text().toLatin1());
                query.finish();
            }
        }
}

void ODownloadsCore::deleteDownloadItem(const QString index)
{
    QDate itemDate = QDate();
    for (int i = 0; i < downloadItemsList.count(); i++)
        if (downloadItemsList.at(i)->getIndex() == index.toInt())
        {
            if (!downloadDatabase.isOpenError())
            {
                QString statement = QLatin1String("DELETE FROM ololandownload WHERE downid = :downid");
                query.prepare(statement);
                query.bindValue(":downid", downloadItemsList.at(i)->getIndex());

                if (!query.exec())
                    qDebug(query.lastError().text().toLatin1());

                itemDate = downloadItemsList.at(i)->getDownloadDate();
                downloadIndexList.append(downloadItemsList.at(i)->getIndex());
                downloadItemsList.at(i)->deleteLater();
                downloadItemsList.removeAt(i);
                break;
            }
            else
                qDebug(downloadDatabase.lastError().text().toLatin1());
        }

    bool exist = false;
    for (int i = 0; i < downloadItemsList.count(); i++)
        if (itemDate == downloadItemsList.at(i)->getDownloadDate())
            exist = true;

    if (!exist)
    {
        QString dateIndex = QString::number(itemDate.day()) + QString::number(itemDate.month()) + QString::number(itemDate.year());
        downloadWebView->page()->runJavaScript(QLatin1String("deleteDate('%1');").arg(dateIndex));
    }

    if (downloadItemsList.count() == 0)
        loadEmptyDownloadView();
}

void ODownloadsCore::pauseDownload(const QString index)
{
    for (int i = 0; i < downloadItemsList.count(); i++)
        if (downloadItemsList.at(i)->getIndex() == index.toInt())
        {

            if (!downloadDatabase.isOpenError())
            {
                QString statement = QLatin1String("UPDATE ololandownload SET downsize = :downsize, downdsize = :downdsize, downstate = :downstate WHERE downid = :downid");
                query.prepare(statement);
                query.bindValue(":downsize", downloadItemsList.at(i)->getSize());
                query.bindValue(":downdsize", downloadItemsList.at(i)->getDownloadedSize());
                query.bindValue(":downstate", "Paused");
                query.bindValue(":downid", downloadItemsList.at(i)->getIndex());

                if (!query.exec())
                    qDebug(query.lastError().text().toLatin1());
                query.finish();
            }

            disconnect(downloadItemsList.at(i)->getDownloadItem(), &QNetworkReply::downloadProgress,
                    this, &ODownloadsCore::updateTransferRate);
            disconnect(downloadItemsList.at(i)->getDownloadItem(), &QNetworkReply::finished,
                    this, &ODownloadsCore::downloadFinished);
            downloadItemsList.at(i)->getDownloadItem()->abort();
            downloadItemsList.at(i)->pause();

            OAppCore *parentApp = static_cast<OAppCore*>(parent());
            if (!hasActiveDownload())
                parentApp->setDownloadState(false);
            break;
        }
}

void ODownloadsCore::stopDownload(const QString index)
{
    for (int i = 0; i < downloadItemsList.count(); i++)
        if (downloadItemsList.at(i)->getIndex() == index.toInt())
        {
            if (!downloadDatabase.isOpenError())
            {
                QString statement = QLatin1String("UPDATE ololandownload SET downsize = :downsize, downdsize = :downdsize, downstate = :downstate WHERE downid = :downid");
                query.prepare(statement);
                query.bindValue(":downsize", downloadItemsList.at(i)->getSize());
                query.bindValue(":downdsize", downloadItemsList.at(i)->getDownloadedSize());
                query.bindValue(":downstate", "Failed");
                query.bindValue(":downid", downloadItemsList.at(i)->getIndex());

                if (!query.exec())
                    qDebug(query.lastError().text().toLatin1());
                query.finish();
            }

            if (downloadItemsList.at(i)->getDownloadItem() != nullptr)
            {
                disconnect(downloadItemsList.at(i)->getDownloadItem(), &QNetworkReply::downloadProgress,
                           this, &ODownloadsCore::updateTransferRate);
                disconnect(downloadItemsList.at(i)->getDownloadItem(), &QNetworkReply::finished,
                           this, &ODownloadsCore::downloadFinished);
                downloadItemsList.at(i)->getDownloadItem()->abort();
            }

            downloadItemsList.at(i)->stop();
            emit removeWidget(downloadItemsList.at(i)->getIndex());

            OAppCore *parentApp = static_cast<OAppCore*>(parent());
            if (!hasActiveDownload())
                parentApp->setDownloadState(false);
            break;
        }
}

void ODownloadsCore::openDownloadLocation(const QString index)
{
    for (int i = 0; i < downloadItemsList.count(); i++)
        if (downloadItemsList.at(i)->getIndex() == index.toInt())
        {
            QString path = downloadItemsList.at(i)->getDownloadDirectory() + QString("/") + downloadItemsList.at(i)->getTitle();
            QStringList args;
            args << "/select," << QDir::toNativeSeparators(path);
            QProcess process(this);
            process.startDetached("explorer.exe", args);
            break;
        }
}

void ODownloadsCore::resumeDownload(const QString index)
{
    for (int i = 0; i < downloadItemsList.count(); i++)
        if (downloadItemsList.at(i)->getIndex() == index.toInt())
        {
            OAppCore *parentApp = static_cast<OAppCore*>(parent());
            parentApp->setDownloadState(true);

            QByteArray range = QByteArray("bytes=" + downloadItemsList.at(i)->getBytesSize() + "-");
            QNetworkRequest request(QUrl(downloadItemsList.at(i)->getUrl()));
            request.setRawHeader(QByteArray("Range"), range);
            downloadItemsList.at(i)->setDownloadItem(webManager->get(request));
            downloadItemsList.at(i)->start();

            connect(downloadItemsList.at(i)->getDownloadItem(), &QNetworkReply::downloadProgress,
                    this, &ODownloadsCore::updateTransferRate);
            connect(downloadItemsList.at(i)->getDownloadItem(), &QNetworkReply::finished,
                    this, &ODownloadsCore::downloadFinished);

            if (!downloadDatabase.isOpenError())
            {
                QString statement = QLatin1String("UPDATE ololandownload SET downstate = :downstate WHERE downid = :downid");
                query.prepare(statement);
                query.bindValue(":downstate", downloadItemsList.at(i)->getState());
                query.bindValue(":downid", downloadItemsList.at(i)->getIndex());

                if (!query.exec())
                    qDebug(query.lastError().text().toLatin1());
                query.finish();
            }
            break;
        }
}

void ODownloadsCore::openDownloadsFolder()
{
    QString path = "C:/Downloads/";
    QStringList args;
    args << "/open," << QDir::toNativeSeparators(path);
    QProcess process(this);
    process.startDetached("explorer.exe", args);
}

void ODownloadsCore::clearDownloadsList()
{
    if (!downloadDatabase.isOpenError())
    {
        QString statement = QLatin1String("DELETE FROM ololandownload");
        query.prepare(statement);

        if (!query.exec())
        {
            qDebug(query.lastError().text().toLatin1());
            return;
        }

        downloadItemsList.clear();
    }
}

void ODownloadsCore::downloadFinished()
{
    QNetworkReply *item = qobject_cast<QNetworkReply*>(sender());
    for (int i = 0; i < downloadItemsList.count(); i++)
        if (downloadItemsList.at(i)->getDownloadItem() == item)
        {
            disconnect(downloadItemsList.at(i)->getDownloadItem(), &QNetworkReply::downloadProgress,
                    this, &ODownloadsCore::updateTransferRate);
            disconnect(downloadItemsList.at(i)->getDownloadItem(), &QNetworkReply::finished,
                    this, &ODownloadsCore::downloadFinished);

            if (item->isFinished() && (item->error() == QNetworkReply::NoError))
            {
                if (!downloadDatabase.isOpenError())
                {
                    QString statement = QLatin1String("UPDATE ololandownload SET downdsize = :downdsize, downstate = :downstate WHERE downid = :downid");
                    query.prepare(statement);
                    query.bindValue(":downdsize", downloadItemsList.at(i)->getSize());
                    query.bindValue(":downstate", "Complete");
                    query.bindValue(":downid", downloadItemsList.at(i)->getIndex());

                    if (!query.exec())
                        qDebug(query.lastError().text().toLatin1());
                    query.finish();
                }

                downloadItemsList.at(i)->closeFile();
                downloadItemsList.at(i)->setState("Complete");

                if (isReady)
                {
                    QString itemScript = QLatin1String("downloadFinished('%1', '%2');").arg(QString::number(downloadItemsList.at(i)->getIndex()), downloadItemsList.at(i)->getSize());
                    downloadWebView->page()->runJavaScript(itemScript);
                }
            }
            else
                stopDownload(QString::number(downloadItemsList.at(i)->getIndex()));
            emit removeWidget(downloadItemsList.at(i)->getIndex());

            QTimer *timer = new QTimer(this);
            timer->setInterval(500);
            connect(timer, &QTimer::timeout, this, [=]() {
                if (!hasActiveDownload())
                {
                    OAppCore *appCore = static_cast<OAppCore*>(parent());
                    appCore->setDownloadState(false);
                }
                timer->stop();
            });
            timer->start();

            break;
        }
}

void ODownloadsCore::updateTransferRate(quint64 receivedBytes, quint64 totalBytes)
{
    QNetworkReply *item = qobject_cast<QNetworkReply*>(sender());
    qint64 bytesReceived = receivedBytes;
    qint64 bytesTotal = totalBytes;

    for (int i = 0; i < downloadItemsList.count(); i++)
        if (downloadItemsList.at(i)->getDownloadItem() == item)
        {
            quint64 range = 1024;
            quint64 totalSize = (bytesTotal - bytesReceived) + downloadItemsList.at(i)->getBytesSize().toULongLong();
            QString sizeToString = "";
            if (totalSize < range)
                sizeToString = (QString::number(totalSize) + QString(" bytes"));
            else if (totalSize < range*range)
                sizeToString = (QString::number((double)(totalSize/range)) + QString(" KB"));
            else if (totalSize < range*range*range)
                    sizeToString = (QString::number((double)(totalSize/(range*range))) + QString(" MB"));
            else if (totalSize < range*range*range*range)
                    sizeToString = (QString::number((double)(totalSize/(range*range*range))) + QString(" GB"));
            else if (totalSize < range*range*range*range*range)
                    sizeToString = (QString::number((double)(totalSize/(range*range*range*range))) + QString(" TB"));

            quint64 downloadedSize = (quint64)downloadItemsList.at(i)->getBytesSize().toInt();
            QString downloadedSizeToString = "";
            if (downloadedSize < range)
                downloadedSizeToString = (QString::number(downloadedSize) + QString(" bytes"));
            else if (downloadedSize < range*range)
                downloadedSizeToString = (QString::number((double)(downloadedSize/range)) + QString(" KB"));
            else if (downloadedSize < range*range*range)
                    downloadedSizeToString = (QString::number((double)(downloadedSize/(range*range))) + QString(" MB"));
            else if (downloadedSize < range*range*range*range)
                    downloadedSizeToString = (QString::number((double)(downloadedSize/(range*range*range))) + QString(" GB"));
            else if (downloadedSize < range*range*range*range*range)
                    downloadedSizeToString = (QString::number((double)(downloadedSize/(range*range*range*range))) + QString(" TB"));
            downloadItemsList.at(i)->setDownloadedSize(downloadedSizeToString);

            const quint64 timeElapsed = downloadItemsList.at(i)->getElapsedTime();
            double speed = bytesReceived * 1000.0 / timeElapsed;
            double timeInSecond = (double)(bytesTotal) / speed;
            QString unit = "";

            if (speed < 1024)
                unit = "bytes/sec ";
            else if (speed < 1024*1024)
            {
                speed /= 1024;
                unit = "KB/s ";
            }
            else  if (speed < 1024*1024*1024)
            {
                speed /= 1024*1024;
                unit = "MB/s ";
            }
            else
            {
                speed /= 1024*1024*1024;
                unit = "GB/s ";
            }

            if (item->isRunning())
            {
                if (bytesTotal < 1)
                    bytesTotal = 1;

                const QString timeLeft = getTimeLeft((int)timeInSecond);
                const int percentage = (int)(bytesReceived * 100 / bytesTotal);
                const QString rate = QString("%1 %2 %3 left").arg(QString::number(speed, 'f', 2), unit, timeLeft);
                downloadItemsList.at(i)->setDownloadedSize(downloadedSizeToString);
                downloadItemsList.at(i)->setSize(sizeToString);
                emit updateWidgetTransfertRate(downloadItemsList.at(i)->getIndex(), QString("%1 %2").arg(QString::number(speed, 'f', 2), unit), (downloadedSizeToString + QString("/") + sizeToString), percentage);

                if (isReady)
                {
                    QString rateScript = QLatin1String("updateTransferRate('%1', '%2', '%3', '%4');").arg(QString::number(downloadItemsList.at(i)->getIndex()), rate, (downloadedSizeToString + QString("/") + sizeToString), QString::number(percentage));
                    downloadWebView->page()->runJavaScript(rateScript);
                }
            }

            break;
        }
}

const QString ODownloadsCore::getTimeLeft(const int seconds)
{
    QString timeLeft = "";
    QTime downloadTimer = QTime(0, 0, 0, 0);
    downloadTimer = downloadTimer.addSecs(seconds);
    if (downloadTimer.isValid())
    {
        if (downloadTimer.hour() > 0)
            timeLeft = QString("%1h %2min %3s").arg(QString::number(downloadTimer.hour()), QString::number(downloadTimer.minute()), QString::number(downloadTimer.second()));
        else if (downloadTimer.minute() > 0)
            timeLeft = QString("%1min %2s").arg(QString::number(downloadTimer.minute()), QString::number(downloadTimer.second()));
        else
            timeLeft = QString("%1s").arg(QString::number(downloadTimer.second()));
    }
    else
    {
        double valueT = seconds;

        if (seconds < 86400)
            valueT = 86453;

        double lostPart;
        double day = valueT / 86400.0;
        double hour = std::modf(day, &lostPart) * 24.0;
        double minute = std::modf(hour, &lostPart) * 60.0;
        double second = std::modf(minute, &lostPart) * 60.0;
        timeLeft = QString("%1day(s) %2h %3min %4s").arg(QString::number((int)day), QString::number((int)hour), QString::number((int)minute), QString::number((int)second));
        if (day > 6)
        {
            double week = day / 7.0;
            day = std::modf(week, &lostPart) * 7.0;
            hour = std::modf(day, &lostPart) * 24.0;
            minute = std::modf(hour, &lostPart) * 60.0;
            second = std::modf(minute, &lostPart) * 60.0;
            timeLeft = QString("%1week(s) %2day(s) %3h %4min %5s").arg(QString::number((int)week), QString::number((int)day), QString::number((int)hour), QString::number((int)minute), QString::number((int)second));
            if (week > 3)
            {
                double month = week / 4;
                week = std::modf(month, &lostPart) * 4.0;
                day = std::modf(week, &lostPart) * 7.0;
                hour = std::modf(day, &lostPart) * 24.0;
                minute = std::modf(hour, &lostPart) * 60.0;
                second = std::modf(minute, &lostPart) * 60.0;
                timeLeft = QString("%1month(s) %2week(s) %3day(s) %4h %5min %6s").arg(QString::number((int)month), QString::number((int)week), QString::number((int)day), QString::number((int)hour), QString::number((int)minute), QString::number((int)second));
            }
        }
    }
    return timeLeft;
}

const QStringList ODownloadsCore::getDateAsString(const QDate date, bool primaryDate)
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

const QString ODownloadsCore::getDayAsString(const int day)
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

const QString ODownloadsCore::getMonthAsString(const int month)
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

void ODownloadsCore::loadEmptyDownloadView(bool state)
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    if (appCore->isDownloadsOpened())
    {
        QString itemScript = QLatin1String("loadEmptyDownloadView(%1);").arg((state ? "true" : "false"));
        downloadWebView->page()->runJavaScript(itemScript);
    }
}

void ODownloadsCore::setDownloadsWebView(OWebView *webView)
{
    downloadWebView = webView;
}

void ODownloadsCore::saveDatabase()
{
    for (int i = 0; i < downloadItemsList.count(); i++)
    {
        if (downloadItemsList.at(i)->getState().compare("Downloading") == 0)
            pauseDownload(QString::number(downloadItemsList.at(i)->getIndex()));
    }
    downloadDatabase.close();
}

void ODownloadsCore::searchInDownloads(const QString &value)
{
    loadEmptyDownloadView(false);
    QList<QDate> dateList;
    bool primaryDate = false;
    bool noResult = false;

    for (int i = 0; i < downloadItemsList.count(); i++)
        if (downloadItemsList.at(i)->getUrl().contains(value, Qt::CaseInsensitive))
            if (!dateList.contains(downloadItemsList.at(i)->getDownloadDate()))
                dateList.append(downloadItemsList.at(i)->getDownloadDate());

    for (int i = 0; i < dateList.count(); i++)
    {
        if (i < 2)
            primaryDate = true;
        else
            primaryDate = false;
        //itemDate.at(0) = day, itemDate.at(1) = month, itemDate.at(2) = year
        QStringList itemDate = getDateAsString(dateList.at(i), primaryDate);
        QString dateScript = QLatin1String("loadDownloadedDate('%1', '%2', '%3', 'add');").arg(itemDate.at(0), itemDate.at(1), itemDate.at(2));
        downloadWebView->page()->runJavaScript(dateScript);

        for (int j = 0; j < downloadItemsList.count(); j++)
            if (downloadItemsList.at(j)->getUrl().contains(value, Qt::CaseInsensitive) && (downloadItemsList.at(j)->getDownloadDate() == dateList.at(i)))
            {
                ODownloadItem *item = downloadItemsList.at(j);
                QString iconPath = QLatin1String("qrc:/web_applications/images/ololanLogo.png");

                if (item->getIcon() != "EmPtY64")
                    iconPath = QLatin1String("data:image/png;base64,%1").arg(item->getIcon());

                QString itemScript = QLatin1String("loadDownloadItem('%1', '%2', '%3', '%4', '%5', '%6', '%7');").arg(QString::number(item->getIndex()),  item->getTitle(), item->getUrl(), item->getSize(), item->getDownloadedSize(), iconPath, item->getState());
                downloadWebView->page()->runJavaScript(itemScript);
                noResult = true;
            }
    }

    if (!noResult && !value.isEmpty())
        downloadWebView->page()->runJavaScript("noDownloadResult();");
    else if (!noResult && value.isEmpty() && downloadItemsList.isEmpty())
        loadEmptyDownloadView();
}

const int ODownloadsCore::generateIndex()
{
    const int index = downloadIndexList.first();
    downloadIndexList.removeFirst();
    return index;
}

void ODownloadsCore::setupIndexList()
{
    for (int i = 1; i < 102410; i++)
        downloadIndexList.append(i);
}

void ODownloadsCore::setDownloadWidget()
{
    ODownloadWidget *downloadsWidgetManager = qobject_cast<ODownloadWidget*>(sender());
    connect(downloadsWidgetManager, &ODownloadWidget::pause, this, &ODownloadsCore::pauseDownloadWeb);
    connect(downloadsWidgetManager, &ODownloadWidget::stop, this, &ODownloadsCore::stopDownloadWeb);
    connect(downloadsWidgetManager, &ODownloadWidget::resume, this, &ODownloadsCore::resumeDownloadWeb);
    connect(this, &ODownloadsCore::removeWidget, downloadsWidgetManager, &ODownloadWidget::removeInactiveItem);
    connect(this, &ODownloadsCore::updateWidgetTransfertRate, downloadsWidgetManager, &ODownloadWidget::updateTransfertRate);

    downloadsWidgetManager->restoreHeight();
    downloadsWidgetManager->setUpdatesEnabled(false);
    for (int i = 0; i < downloadItemsList.count(); i++)
        if (downloadItemsList.at(i)->getState().compare("Downloading") == 0)
            downloadsWidgetManager->setupWidget(downloadItemsList.at(i)->getIndex(), downloadItemsList.at(i)->getTitle(),
                                                downloadItemsList.at(i)->getIcon(), downloadItemsList.at(i)->getSize(), downloadItemsList.at(i)->getState());

    for (int i = 0; i < downloadItemsList.count(); i++)
        if (downloadItemsList.at(i)->getState().compare("Paused") == 0)
            downloadsWidgetManager->setupWidget(downloadItemsList.at(i)->getIndex(), downloadItemsList.at(i)->getTitle(),
                                                downloadItemsList.at(i)->getIcon(), downloadItemsList.at(i)->getDownloadedSize() + "/" + downloadItemsList.at(i)->getSize(), downloadItemsList.at(i)->getState());
    downloadsWidgetManager->setUpdatesEnabled(true);

    if (downloadItemsList.isEmpty())
    {downloadsWidgetManager->loadEmptyView(true);}
}

void ODownloadsCore::unsetDownloadWidget()
{
    ODownloadWidget *downloadsWidgetManager = qobject_cast<ODownloadWidget*>(sender());
    downloadsWidgetManager->clearWidgetItemList();
    disconnect(downloadsWidgetManager, &ODownloadWidget::pause, this, &ODownloadsCore::pauseDownloadWeb);
    disconnect(downloadsWidgetManager, &ODownloadWidget::stop, this, &ODownloadsCore::stopDownloadWeb);
    disconnect(downloadsWidgetManager, &ODownloadWidget::resume, this, &ODownloadsCore::resumeDownloadWeb);
    disconnect(this, &ODownloadsCore::removeWidget, downloadsWidgetManager, &ODownloadWidget::removeInactiveItem);
    disconnect(this, &ODownloadsCore::updateWidgetTransfertRate, downloadsWidgetManager, &ODownloadWidget::updateTransfertRate);
}

void ODownloadsCore::pauseDownloadWeb(const QString index)
{
    if (isReady)
    {
        QString itemScript = QLatin1String("pauseItem('%1');").arg(index);
        downloadWebView->page()->runJavaScript(itemScript);
    }
    pauseDownload(index);
}

void ODownloadsCore::stopDownloadWeb(const QString index)
{
    if (isReady)
    {
        QString itemScript = QLatin1String("stopItem('%1');").arg(index);
        downloadWebView->page()->runJavaScript(itemScript);
    }
    stopDownload(index);
}

void ODownloadsCore::resumeDownloadWeb(const QString index)
{
    if (isReady)
    {
        QString itemScript = QLatin1String("resumeItem('%1');").arg(index);
        downloadWebView->page()->runJavaScript(itemScript);
    }
    resumeDownload(index);
}

void ODownloadsCore::pauseAllDownloads()
{
    for (int i = 0; i < downloadItemsList.count(); i++)
    {
        if (downloadItemsList.at(i)->getState().compare("Downloading", Qt::CaseInsensitive) == 0)
            pauseDownloadWeb(QString::number(downloadItemsList.at(i)->getIndex()));
    }
}

void ODownloadsCore::pauseDownloadByDate(QString date)
{
    QDate stringDate = QDate::fromString(date, "MMMM dd, yyyy");
    for (int i = 0; i < downloadItemsList.count(); i++)
    {
        QDate itemDate = QDate::fromString(downloadItemsList.at(i)->getDownloadDate().toString("MMMM dd, yyyy"));
        if ((itemDate == stringDate) &&
            (downloadItemsList.at(i)->getState().compare("Downloading", Qt::CaseInsensitive) == 0))
            pauseDownloadWeb(QString::number(downloadItemsList.at(i)->getIndex()));
    }
}

void ODownloadsCore::resumeAllDownloads()
{
    for (int i = 0; i < downloadItemsList.count(); i++)
    {
        if (downloadItemsList.at(i)->getState().compare("Paused", Qt::CaseInsensitive) == 0)
            resumeDownloadWeb(QString::number(downloadItemsList.at(i)->getIndex()));
    }
}

void ODownloadsCore::resumeByDate(QString date)
{
    QDate stringDate = QDate::fromString(date, "MMMM dd, yyyy");
    for (int i = 0; i < downloadItemsList.count(); i++)
    {
        QDate itemDate = QDate::fromString(downloadItemsList.at(i)->getDownloadDate().toString("MMMM dd, yyyy"));
        if ((itemDate == stringDate) &&
            (downloadItemsList.at(i)->getState().compare("Paused", Qt::CaseInsensitive) == 0))
            resumeDownloadWeb(QString::number(downloadItemsList.at(i)->getIndex()));
    }
}

void ODownloadsCore::restartAllDownloads()
{
    for (int i = 0; i < downloadItemsList.count(); i++)
    {
        if (downloadItemsList.at(i)->getState().compare("Failed", Qt::CaseInsensitive) == 0)
        {
            if (isReady)
            {
                QString itemScript = QLatin1String("resumeItem('%1');").arg(QString::number(downloadItemsList.at(i)->getIndex()));
                downloadWebView->page()->runJavaScript(itemScript);
            }
            restartDownload(QString::number(downloadItemsList.at(i)->getIndex()));
        }
    }
}

void ODownloadsCore::restartByDate(QString date)
{
    QDate stringDate = QDate::fromString(date, "MMMM dd, yyyy");
    for (int i = 0; i < downloadItemsList.count(); i++)
    {
        QDate itemDate = QDate::fromString(downloadItemsList.at(i)->getDownloadDate().toString("MMMM dd, yyyy"));
        if ((itemDate == stringDate) &&
            (downloadItemsList.at(i)->getState().compare("Failed", Qt::CaseInsensitive) == 0))
        {
            if (isReady)
            {
                QString itemScript = QLatin1String("resumeItem('%1');").arg(QString::number(downloadItemsList.at(i)->getIndex()));
                downloadWebView->page()->runJavaScript(itemScript);
            }
            restartDownload(QString::number(downloadItemsList.at(i)->getIndex()));
        }
    }
}

void ODownloadsCore::stopByDate(QString date)
{
    QDate stringDate = QDate::fromString(date, "MMMM dd, yyyy");
    for (int i = 0; i < downloadItemsList.count(); i++)
    {
        QDate itemDate = QDate::fromString(downloadItemsList.at(i)->getDownloadDate().toString("MMMM dd, yyyy"));
        if (itemDate == stringDate)
        {
            if ((downloadItemsList.at(i)->getState().compare("Paused", Qt::CaseInsensitive) == 0) ||
                (downloadItemsList.at(i)->getState().compare("Downloading", Qt::CaseInsensitive) == 0))
                stopDownloadWeb(QString::number(downloadItemsList.at(i)->getIndex()));
        }
    }
}

void ODownloadsCore::stopAllDownloads()
{
    for (int i = 0; i < downloadItemsList.count(); i++)
    {
        if ((downloadItemsList.at(i)->getState().compare("Paused", Qt::CaseInsensitive) == 0) ||
            (downloadItemsList.at(i)->getState().compare("Downloading", Qt::CaseInsensitive) == 0))
            stopDownloadWeb(QString::number(downloadItemsList.at(i)->getIndex()));
    }
}

void ODownloadsCore::deleteByDate(QString date)
{
    QDate stringDate = QDate::fromString(date, "MMMM dd, yyyy");
    for (int i = 0; i < downloadItemsList.count(); i++)
    {
        QDate itemDate = QDate::fromString(downloadItemsList.at(i)->getDownloadDate().toString("MMMM dd, yyyy"));
        if (itemDate == stringDate)
        {
            if ((downloadItemsList.at(i)->getState().compare("Paused", Qt::CaseInsensitive) == 0) ||
                (downloadItemsList.at(i)->getState().compare("Downloading", Qt::CaseInsensitive) == 0))
                stopDownloadWeb(QString::number(downloadItemsList.at(i)->getIndex()));
            deleteDownloadItem(QString::number(downloadItemsList.at(i)->getIndex()));
        }
    }
}

void ODownloadsCore::deleteAllDownloads()
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    if (appCore->isDownloadsOpened())
    {
        QString itemScript = QLatin1String("clearDownloadsList()");
        downloadWebView->page()->runJavaScript(itemScript);
    }
    else
        clearDownloadsList();
}

QList<ODownloadItem*> ODownloadsCore::getDownloadItemList()
{
    return downloadItemsList;
}

bool ODownloadsCore::isDateExisting(QString date)
{
    QDate stringDate = QDate::fromString(date, "MMMM dd, yyyy");
    bool exist = false;

    for (int i = 0; i < downloadItemsList.count(); i++)
    {
        QDate itemDate = QDate::fromString(downloadItemsList.at(i)->getDownloadDate().toString("MMMM dd, yyyy"));
        if (itemDate == stringDate)
        {
            exist = true;
            break;
        }
    }

    return exist;
}

bool ODownloadsCore::hasActiveDownload()
{
    bool exist = false;
    for (int i = 0; i < downloadItemsList.count(); i++)
        if (downloadItemsList.at(i)->getState().compare("Downloading", Qt::CaseInsensitive) == 0)
        {
            exist = true;
            break;
        }

    return exist;
}

bool ODownloadsCore::hasPausedDownload()
{
    bool exist = false;
    for (int i = 0; i < downloadItemsList.count(); i++)
        if (downloadItemsList.at(i)->getState().compare("Paused", Qt::CaseInsensitive) == 0)
        {
            exist = true;
            break;
        }

    return exist;
}

bool ODownloadsCore::hasFailedDownload()
{
    bool exist = false;
    for (int i = 0; i < downloadItemsList.count(); i++)
        if (downloadItemsList.at(i)->getState().compare("Failed", Qt::CaseInsensitive) == 0)
        {
            exist = true;
            break;
        }

    return exist;
}
