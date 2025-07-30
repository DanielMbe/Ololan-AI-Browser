#include "obookmarkscore.h"
#include "oappcore.h"
#include "otabpage.h"
#include "customwindow.h"
#include <QWebEngineScript>
#include <QSqlError>
#include <QFile>
#include <QBuffer>
#include <QTimer>

OBookmarksItem::OBookmarksItem(QObject *parent, const QString url, const QString title, const QByteArray icon, const QString folder, const int index)
    :QObject(parent)
{
    setUrl(url);
    setTitle(title);
    setIcon(icon);
    setFolder(folder);
    setIndex(index);
}

const QString OBookmarksItem::getUrl()
{
    return itemUrl;
}

const QString OBookmarksItem::getTitle()
{
    return itemTitle;
}

const QByteArray OBookmarksItem::getIcon()
{
    return itemIcon;
}

const QString OBookmarksItem::getFolder()
{
    return itemFolder;
}

const int OBookmarksItem::getIndex()
{
    return itemIndex;
}

void OBookmarksItem::setUrl(const QString url)
{
    itemUrl = url;
}

void OBookmarksItem::setTitle(const QString title)
{
    itemTitle = title;
}

void OBookmarksItem::setIcon(const QByteArray icon)
{
    itemIcon = icon;
}

void OBookmarksItem::setDeviceName(const QString device)
{
    deviceName = device;
}

void OBookmarksItem::setFolder(const QString folder)
{
    itemFolder = folder;
}

void OBookmarksItem::setIndex(const int index)
{
    itemIndex = index;
}

OBookmarksCore::OBookmarksCore(QObject *parent)
    :QObject(parent)
{
    setupIndexList();
    isOpened = false;
    isEditingItem = false;
    editingIndex = -1;
    webChannel = new QWebChannel(this);
    webChannel->registerObject("bookmarksManager", this);
    bookmarksWebView = nullptr;

    QString databaseName = OAppCore::applicationDirPath().append("/browserDBDir/ololanbookmarksdatabase.gdb");
    bookmarksDatabase = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), QLatin1String("ololanBookmarksDatabase"));
    bookmarksDatabase.setDatabaseName(databaseName);
    bookmarksDatabase.setHostName(QLatin1String("ololanhost"));
    bookmarksDatabase.setUserName(QLatin1String("ololanRoot"));
    bookmarksDatabase.setPassword(QLatin1String("OlolanBrowserDB2022"));

    if (bookmarksDatabase.open(QLatin1String("ololanRoot"), QLatin1String("OlolanBrowserDB2022")))
    {
        query = QSqlQuery(bookmarksDatabase);
        QString statementA1 = QLatin1String("SELECT bmkid, bmkurl, bmktitle, bmkicon, bmkfolder FROM ololanbookmarks");
        if (query.exec(statementA1))
        {
            while (query.next())
            {
                OBookmarksItem *item = new OBookmarksItem;
                item->setIndex(query.value(0).toInt());
                item->setUrl(QString(query.value(1).toByteArray()));
                item->setTitle(QString(query.value(2).toByteArray()));
                item->setIcon(query.value(3).toByteArray());
                item->setFolder(QString(query.value(4).toByteArray()));
                bookmarksList.append(item);
                bookmarksUnusedIndexList.removeOne(item->getIndex());
            }
        }
        else
        {
            qDebug(query.lastError().text().toLatin1());
            QString statementA2 = QLatin1String("CREATE TABLE ololanbookmarks(bmkid SMALLINT UNSIGNED NOT NULL, bmkurl VARCHAR(2048) NOT NULL, bmktitle VARCHAR(256) NOT NULL,"
                                                "bmkicon VARCHAR(8192) NOT NULL, bmkfolder VARCHAR(256) NOT NULL, PRIMARY KEY (bmkid))");
            if (!query.exec(statementA2))
                qDebug(query.lastError().text().toLatin1());
        }

        QString statementB1 = QLatin1String("SELECT bmkfolders FROM ololanbookmarksfolderlist");
        if (query.exec(statementB1))
        {
            while (query.next())
                foldersList.append(QString(query.value(0).toString()));
        }
        else
        {
            qDebug(query.lastError().text().toLatin1());
            QString statementB2 = QLatin1String("CREATE TABLE ololanbookmarksfolderlist(bmkfolders VARCHAR(256) NOT NULL, PRIMARY KEY (bmkfolders))");

            if (!query.exec(statementB2))
                qDebug(query.lastError().text().toLatin1());
            else
                addFolder(QString("Bookmark bar"));
        }

        QString statementC1 = QLatin1String("SELECT deletedbmkid, deletedbmkurl, deletedbmktitle, deletedbmkfolder FROM ololandeletedbookmarks");
        if (!query.exec(statementC1))
        {
            QString statementC2 = QLatin1String("CREATE TABLE ololandeletedbookmarks(deletedbmkid SMALLINT UNSIGNED NOT NULL, deletedbmkurl VARCHAR(2048) NOT NULL,"
                                                "deletedbmktitle VARCHAR(256) NOT NULL, deletedbmkfolder VARCHAR(256) NOT NULL, PRIMARY KEY (deletedbmkid))");
            if (!query.exec(statementC2))
                qDebug(query.lastError().text().toLatin1());
        }

        QString statementD1 = QLatin1String("SELECT deletedbmkfolders FROM ololandeletedfolder");
        if (!query.exec(statementD1))
        {
            QString statementD2 = QLatin1String("CREATE TABLE ololandeletedfolder(deletedbmkfolders VARCHAR(256) NOT NULL, PRIMARY KEY (deletedbmkfolders))");
            if (!query.exec(statementD2))
                qDebug(query.lastError().text().toLatin1());
        }

        query.finish();
    }
    else
        qDebug(bookmarksDatabase.lastError().text().toLatin1());
}

void OBookmarksCore::loadBookmarks(bool status)
{
    if (!status)
        return;
    else
    {
        bookmarksWebView = qobject_cast<OWebView*>(sender());
        QString bookmarksUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");
        if (bookmarksWebView->url().toString().compare(bookmarksUrl) == 0)
        {
            isOpened = true;
            bookmarksWebView->page()->setWebChannel(webChannel);
            QString initScript = QLatin1String("setupConnector();");
            bookmarksWebView->page()->runJavaScript(initScript);
            bookmarksWebView->page()->runJavaScript("setControlBarVisible();");
            completeLastDeleting();
            tabPage = static_cast<OTabPage*>(bookmarksWebView->parentWidget()->parentWidget()->parentWidget());

            for (int i = 0; i < foldersList.count(); i++)
            {
                if (foldersList.at(i).contains("'"))
                {
                    QString itemScript = "loadFolder(";
                    itemScript += '"';
                    itemScript += '%';
                    itemScript += '1';
                    itemScript += '"';
                    itemScript += ");";
                    itemScript = itemScript.arg(foldersList.at(i));
                    bookmarksWebView->page()->runJavaScript(itemScript);
                }
                else if (foldersList.at(i).contains('"'))
                {
                    QString itemScript = "loadFolder('%1');";
                    itemScript = itemScript.arg(foldersList.at(i));
                    bookmarksWebView->page()->runJavaScript(itemScript);
                }
                else
                {
                    QString folderScript = QLatin1String("loadFolder('%1', false);").arg(foldersList.at(i));
                    bookmarksWebView->page()->runJavaScript(folderScript);
                }
            }

            bool hasItem = false;
            for (int i = 0; (i < bookmarksList.count()) && !hasItem; i++)
                if (bookmarksList.at(i)->getFolder().compare("Bookmark bar") == 0)
                    hasItem = true;

            if (!hasItem)
                loadEmptyBookmarkView();
            else if (!bookmarksDatabase.isOpenError())
            {
                loadEmptyBookmarkView(false);

                for (int i = 0; i < bookmarksList.count(); i++)
                    if (bookmarksList.at(i)->getFolder().compare("Bookmark bar") == 0)
                    {
                        OBookmarksItem *item = bookmarksList.at(i);
                        QString iconPath = QLatin1String("qrc:/web_applications/images/webpage.png");

                        if (bookmarksList.at(i)->getIcon().compare("EmPtY64") != 0)
                            iconPath = QLatin1String("data:image/png;base64,%1").arg(bookmarksList.at(i)->getIcon());

                        if (item->getUrl().contains("'"))
                        {
                            QString itemScript = "loadBookmarkItem(";
                            for (int i = 1; i < 6; i++)
                            {
                                itemScript += '"';
                                itemScript += '%';
                                itemScript += QString::number(i);
                                itemScript += '"';

                                if (i < 5)
                                    itemScript += ',';

                                if (i == 5)
                                    itemScript += ");";
                            }

                            itemScript = itemScript.arg(item->getTitle(), item->getUrl(), iconPath, item->getFolder(), QString::number(item->getIndex()));
                            bookmarksWebView->page()->runJavaScript(itemScript);
                        }
                        else if (item->getUrl().contains('"'))
                        {
                            QString itemScript = "loadBookmarkItem(";
                            for (int i = 1; i < 6; i++)
                            {
                                itemScript += "'%";
                                itemScript += QString::number(i);
                                itemScript += "'";

                                if (i < 5)
                                    itemScript += ',';

                                if (i == 5)
                                    itemScript += ");";
                            }

                            itemScript = itemScript.arg(item->getTitle(), item->getUrl(), iconPath, item->getFolder(), QString::number(item->getIndex()));
                            bookmarksWebView->page()->runJavaScript(itemScript);
                        }
                        else
                        {
                            QString itemScript = QLatin1String("loadBookmarkItem('%1', '%2', '%3', '%4', '%5');").arg(item->getTitle(), item->getUrl(), iconPath, item->getFolder(), QString::number(item->getIndex()));
                            bookmarksWebView->page()->runJavaScript(itemScript);
                        }
                    }
            }
            else
                qDebug(bookmarksDatabase.lastError().text().toLatin1());

            QString setupScript = QLatin1String("setupStart();");
            bookmarksWebView->page()->runJavaScript(setupScript);

            if (isEditingItem && (editingIndex > -1))
            {
                bookmarksWebView->page()->runJavaScript(QLatin1String("setEditingItem('%1');").arg(QString::number(editingIndex)));
                isEditingItem = false;
                editingIndex = -1;
            }
        }
        else
        {
            disconnect(bookmarksWebView, &OWebView::loadFinished, this, &OBookmarksCore::loadBookmarks);
            OAppCore *parentApp = static_cast<OAppCore*>(parent());
            parentApp->bookmarksClosed();
            isOpened = false;
            tabPage = nullptr;
        }
    }
}

void OBookmarksCore::updateBookmarkView(const QByteArray icon, const QString title, const QString url, const QString folder,
                                        const QString index)
{
    loadEmptyBookmarkView(false);

    QString iconPath = QLatin1String("qrc:/web_applications/images/webpage.png");
    if (icon.compare("EmPtY64") != 0)
        iconPath = QLatin1String("data:image/png;base64,%1").arg(icon);

    if (url.contains("'"))
    {
        QString itemScript = "loadBookmarkItem(";
        for (int i = 1; i < 6; i++)
        {
            itemScript += '"';
            itemScript += '%';
            itemScript += QString::number(i);
            itemScript += '"';

            if (i < 5)
                itemScript += ',';

            if (i == 5)
                itemScript += ");";
        }

        itemScript = itemScript.arg(title, url, iconPath, folder, index);
        bookmarksWebView->page()->runJavaScript(itemScript);
    }
    else if (url.contains('"'))
    {
        QString itemScript = "loadBookmarkItem(";
        for (int i = 1; i < 6; i++)
        {
            itemScript += "'%";
            itemScript += QString::number(i);
            itemScript += "'";

            if (i < 5)
                itemScript += ',';

            if (i == 5)
                itemScript += ");";
        }

        itemScript = itemScript.arg(title, url, iconPath, folder, index);
        bookmarksWebView->page()->runJavaScript(itemScript);
    }
    else
    {
        QString itemScript = QLatin1String("loadBookmarkItem('%1', '%2', '%3', '%4', '%5');").arg(title, url, iconPath, folder, index);
        bookmarksWebView->page()->runJavaScript(itemScript);
    }
}

void OBookmarksCore::addBookmarkItem(const QString title, const QString url, QIcon favicon, const QString folder)
{
    QByteArray icon;
    if (!favicon.isNull())
    {
        QBuffer buffer(&icon);
        if (buffer.open(QIODevice::WriteOnly))
            if(!favicon.pixmap(16, 16).save(&buffer, "PNG", 0))
                qDebug("error saving icon file in addbookmarkitem");
    }
    else if (favicon.isNull())
        icon = "EmPtY64";

    const int index = generateIndex();
    OBookmarksItem *item = new OBookmarksItem(this, url, title, icon.toBase64(), folder, index);
    bookmarksList.append(item);
    addFolder(folder);

    if (folder.compare("Bookmark bar") == 0)
        updateBookmarkBar();

    if (!bookmarksDatabase.isOpenError())
    {
        QString statement = QLatin1String("INSERT INTO ololanbookmarks (bmkid, bmkurl, bmktitle, bmkicon, bmkfolder) VALUES(:bmkid, :bmkurl, :bmktitle, :bmkicon, :bmkfolder)");
        query.prepare(statement);
        query.bindValue(":bmkid", item->getIndex());
        query.bindValue(":bmkurl", item->getUrl());
        query.bindValue(":bmktitle", item->getTitle().toLatin1());
        query.bindValue(":bmkicon", icon.toBase64());
        query.bindValue(":bmkfolder", item->getFolder().toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        else
            bookmarksUnusedIndexList.removeOne(index);
        query.finish();
    }
    else
        qDebug(bookmarksDatabase.lastError().text().toLatin1());

    if (isOpened)
        updateBookmarkView(icon.toBase64(), title, url, folder, QString::number(index));
}

void OBookmarksCore::addBookmarkItem(const QString title, const QString url, const QString folder)
{
    const int index = generateIndex();
    OBookmarksItem *item = new OBookmarksItem(this, url, title, QByteArray("EmPtY64"), folder, index);
    bookmarksList.append(item);
    addFolder(folder);

    if (folder.compare("Bookmark bar") == 0)
        updateBookmarkBar();

    if (!bookmarksDatabase.isOpenError())
    {
        QString statement = QLatin1String("INSERT INTO ololanbookmarks (bmkid, bmkurl, bmktitle, bmkicon, bmkfolder) VALUES(:bmkid, :bmkurl, :bmktitle, :bmkicon, :bmkfolder)");
        query.prepare(statement);
        query.bindValue(":bmkid", item->getIndex());
        query.bindValue(":bmkurl", item->getUrl());
        query.bindValue(":bmktitle", item->getTitle().toLatin1());
        query.bindValue(":bmkicon", "EmPtY64");
        query.bindValue(":bmkfolder", item->getFolder().toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        else
            bookmarksUnusedIndexList.removeOne(index);
        query.finish();
    }
    else
        qDebug(bookmarksDatabase.lastError().text().toLatin1());

    if (isOpened)
    {
        QString webScript = QLatin1String("setLastAddedIndex('%1');").arg(QString::number(index));
        bookmarksWebView->page()->runJavaScript(webScript);
    }
}

void OBookmarksCore::updateBookmarkItem(const QString title, const QString url, const QString folder, const QString bmkIndex)
{
    for (int i = 0; i < bookmarksList.count(); i++)
        if ((bookmarksList.at(i)->getIndex() == bmkIndex.toInt()) && (bookmarksList.at(i)->getFolder().compare(folder) == 0))
        {
            bookmarksList.at(i)->setTitle(title);
            bookmarksList.at(i)->setUrl(url);

            if (folder.compare("Bookmark bar") == 0)
                updateBookmarkBar(QString::number(bookmarksList.at(i)->getIndex()), title, url);

            if (!bookmarksDatabase.isOpenError())
            {
                QString statement = QLatin1String("UPDATE ololanbookmarks SET bmkurl = :bmkurl, bmktitle = :bmktitle WHERE bmkid = :bmkid");
                query.prepare(statement);
                query.bindValue(":bmkurl", url.toLatin1());
                query.bindValue(":bmktitle", title.toLatin1());
                query.bindValue(":bmkid", bookmarksList.at(i)->getIndex());

                if (!query.exec())
                    qDebug(query.lastError().text().toLatin1());
                query.finish();
            }
            break;
        }
}

void OBookmarksCore::updateBookmarkItem(const QString title, const QString url, const QString folder)
{
    for (int i = bookmarksList.count() - 1; i > -1; i--)
        if (bookmarksList.at(i)->getUrl().compare(url) == 0)
        {
            moveBookmarkItem(url, QString::number(bookmarksList.at(i)->getIndex()), bookmarksList.at(i)->getFolder(), folder);
            updateBookmarkItem(title, url, folder, QString::number(bookmarksList.at(i)->getIndex()));
            break;
        }
}

void OBookmarksCore::duplicateBookmarkItem(const QString title, const QString url, const QString destinationFolder)
{
    for (int i = 0; i < bookmarksList.count(); i++)
        if (bookmarksList.at(i)->getUrl().compare(url) == 0)
        {
            if (bookmarksList.at(i)->getIcon().compare("EmPtY64") != 0)
            {
                QPixmap iconPixmap(16, 16);
                iconPixmap.loadFromData(QByteArray::fromBase64(bookmarksList.at(i)->getIcon()));
                addBookmarkItem(title, url, QIcon(iconPixmap), destinationFolder);
            }
            else
                addBookmarkItem(title, url, destinationFolder);
            break;
        }
}

void OBookmarksCore::moveBookmarkItem(const QString url, const QString bmkIndex, const QString from, const QString to)
{
    for (int i = 0; i < bookmarksList.count(); i++)
        if (bookmarksList.at(i)->getIndex() == bmkIndex.toInt())
        {
            if (!bookmarksDatabase.isOpenError())
            {
                QString statement = QLatin1String("DELETE FROM ololanbookmarks WHERE bmkid = :bmkid");
                query.prepare(statement);
                query.bindValue(":bmkid", bookmarksList.at(i)->getIndex());

                if (!query.exec())
                    qDebug(query.lastError().text().toLatin1());
                query.finish();
            }
            else
                qDebug(bookmarksDatabase.lastError().text().toLatin1());

            if (from.compare("Bookmark bar") == 0)
                deleteBookmarkBarItem(bmkIndex);

            OBookmarksItem *item = bookmarksList.at(i);
            bookmarksList.removeAt(i);
            bookmarksUnusedIndexList.append(item->getIndex());

            if (item->getIcon().compare("EmPtY64") != 0)
            {
                QPixmap iconPixmap(16, 16);
                iconPixmap.loadFromData(QByteArray::fromBase64(item->getIcon()));
                addBookmarkItem(item->getTitle(), item->getUrl(), QIcon(iconPixmap), to);
            }
            else
                addBookmarkItem(item->getTitle(), item->getUrl(), to);
            item->deleteLater();
            break;
        }
}

void OBookmarksCore::deleteBmkFromBar(const QString url, const QString folder, const QString bmkIndex)
{
    if (isOpened)
    {
        QString setupScript = QLatin1String("deleteBookmarkBar('%1');").arg(bmkIndex);
        bookmarksWebView->page()->runJavaScript(setupScript);
    }
    else
        deleteBookmarkItem(url, folder, bmkIndex);
}

void OBookmarksCore::deleteBookmarkItem(const QString url, const QString folder, const QString bmkIndex)
{
    for (int i = 0; i < bookmarksList.count(); i++)
        if ((bookmarksList.at(i)->getIndex() == bmkIndex.toInt()) && (bookmarksList.at(i)->getFolder().compare(folder) == 0))
        {
            if (bookmarksList.at(i)->getUrl().compare(url) == 0)
            {
                if (folder.compare("Bookmark bar") == 0)
                    deleteBookmarkBarItem(bmkIndex);

                if (!bookmarksDatabase.isOpenError())
                {
                    QString statement = QLatin1String("DELETE FROM ololanbookmarks WHERE bmkid = :bmkid");
                    query.prepare(statement);
                    query.bindValue(":bmkid", bookmarksList.at(i)->getIndex());

                    if (!query.exec())
                        qDebug(query.lastError().text().toLatin1());
                    query.finish();

                    emit bookmarkDeleted(bookmarksList.at(i)->getUrl());
                    bookmarksUnusedIndexList.append(bookmarksList.at(i)->getIndex());
                    bookmarksList.at(i)->deleteLater();
                    bookmarksList.removeAt(i);
                }
                else
                    qDebug(bookmarksDatabase.lastError().text().toLatin1());
            }
            else
                qDebug("Url doesn't matches");
            break;
        }
}

void OBookmarksCore::deleteBookmarkItem(const QString title, const QUrl url, const QString folder)
{
    for (int i = bookmarksList.count() - 1; i > -1; i--)
    {
        if ((bookmarksList.at(i)->getUrl().compare(url.toEncoded()) == 0) && (bookmarksList.at(i)->getFolder().compare(folder) == 0) &&
            (bookmarksList.at(i)->getTitle().compare(title) == 0))
        {
            deleteBmkFromBar(bookmarksList.at(i)->getUrl(), bookmarksList.at(i)->getFolder(),
                             QString::number(bookmarksList.at(i)->getIndex()));
            break;
        }
        else if (i == 0)
        {
            for (int j = bookmarksList.count() - 1; j > -1; j--)
                if (bookmarksList.at(j)->getUrl().compare(url.toEncoded()) == 0)
                {
                    deleteBmkFromBar(bookmarksList.at(j)->getUrl(), bookmarksList.at(j)->getFolder(),
                                     QString::number(bookmarksList.at(j)->getIndex()));
                    break;
                }
        }
    }
}

void OBookmarksCore::deleteBookmarkItem(const QString folder)
{
    for (int i = 0; i < bookmarksList.count(); i++)
    {
        if (bookmarksList.at(i)->getFolder().compare(folder) == 0)
        {
            OBookmarksItem *item = bookmarksList.at(i);
            deleteBookmarkItem(item->getUrl(), folder, QString::number(bookmarksList.at(i)->getIndex()));
            i--;
        }
    }
}

void OBookmarksCore::addFolder(const QString folder)
{
    if (!foldersList.contains(folder))
    {
        foldersList.append(folder);
        if (!bookmarksDatabase.isOpenError())
        {
            QString statement = QLatin1String("INSERT INTO ololanbookmarksfolderlist (bmkfolders) VALUES(:bmkfolders)");
            query.prepare(statement);
            query.bindValue(":bmkfolders", folder.toLatin1());

            if (!query.exec())
                qDebug(query.lastError().text().toLatin1());
            query.finish();
        }
        else
            qDebug(bookmarksDatabase.lastError().text().toLatin1());
    }
}

void OBookmarksCore::selectFolder(const QString folder)
{
    if (foldersList.contains(folder))
    {
        bool hasItem = false;
        for (int i = 0; (i < bookmarksList.count()) && !hasItem; i++)
            if (bookmarksList.at(i)->getFolder().compare(folder) == 0)
                hasItem = true;

        if (!hasItem)
            loadEmptyBookmarkView();
        else if (!bookmarksDatabase.isOpenError())
        {
            loadEmptyBookmarkView(false);
            int count = 0;

            for (int i = 0; i < bookmarksList.count(); i++)
                if (bookmarksList.at(i)->getFolder().compare(folder) == 0)
                {
                    count++;
                    OBookmarksItem *item = bookmarksList.at(i);
                    QString iconPath = QLatin1String("qrc:/web_applications/images/webpage.png");
                    bool isDeleted = false;

                    if (bookmarksList.at(i)->getIcon() != QByteArray("EmPtY64"))
                        iconPath = QLatin1String("data:image/png;base64,%1").arg(bookmarksList.at(i)->getIcon());

                    if (!deletedBookmarksList.isEmpty())
                        for (int j = 0; j < deletedBookmarksList.count(); j++)
                            if (deletedBookmarksList.at(j)->getIndex() == bookmarksList.at(i)->getIndex())
                            {
                                isDeleted = true;
                                break;
                            }

                    if (!isDeleted)
                    {
                        QString itemScript = QLatin1String("loadBookmarkItem('%1', '%2', '%3', '%4', '%5');").arg(item->getTitle(), item->getUrl(), iconPath, item->getFolder(), QString::number(item->getIndex()));
                        bookmarksWebView->page()->runJavaScript(itemScript);
                    }
                }

            if (!deletedBookmarksList.isEmpty())
                if ((count == deletedBookmarksList.count()) && (deletedBookmarksList.at(0)->getFolder().compare(folder) == 0))
                    loadEmptyBookmarkView();

            QString setupScript = QLatin1String("setControlDefaultState();");
            bookmarksWebView->page()->runJavaScript(setupScript);
        }
        else
            qDebug(bookmarksDatabase.lastError().text().toLatin1());
    }
}

void OBookmarksCore::changeFolderName(const QString oldName, const QString newName)
{
    if (foldersList.contains(oldName))
    {
        foldersList.replace(foldersList.indexOf(oldName), newName);
        if (!bookmarksDatabase.isOpenError())
        {
            QString statement = QLatin1String("UPDATE ololanbookmarksfolderlist SET bmkfolders = :bmkfolders WHERE bmkfolders = :oldfolder");
            query.prepare(statement);
            query.bindValue(":bmkfolders", newName.toLatin1());
            query.bindValue(":oldfolder", oldName.toLatin1());

            if (query.exec())
            {
                query.finish();
                for (int i = 0; i < bookmarksList.count(); i++)
                    if (bookmarksList.at(i)->getFolder().compare(oldName) == 0)
                        bookmarksList.at(i)->setFolder(newName);

                QString statement = QLatin1String("UPDATE ololanbookmarks SET bmkfolder = :bmkfolder WHERE bmkfolder = :oldfolder");
                query.prepare(statement);
                query.bindValue(":bmkfolder", newName.toLatin1());
                query.bindValue(":oldfolder", oldName.toLatin1());
                if (!query.exec())
                    qDebug(query.lastError().text().toLatin1());
            }
            else
                qDebug(query.lastError().text().toLatin1());
            query.finish();
        }
    }
}

void OBookmarksCore::deleteFolder(const QString folder)
{
    if (foldersList.contains(folder))
    {
        foldersList.removeOne(folder);
        if (!bookmarksDatabase.isOpenError())
        {
            QString statement = QLatin1String("DELETE FROM ololanbookmarksfolderlist WHERE bmkfolders = :bmkfolders");
            query.prepare(statement);
            query.bindValue(":bmkfolders", folder.toLatin1());

            if (query.exec())
                deleteBookmarkItem(folder);
            else
                qDebug(query.lastError().text().toLatin1());
            query.finish();
        }
        else
            qDebug(bookmarksDatabase.lastError().text().toLatin1());
    }
}

void OBookmarksCore::setBookmarksWebView(OWebView *webView)
{
    bookmarksWebView = webView;
}

void OBookmarksCore::loadEmptyBookmarkView(bool state)
{
    QString itemScript = QLatin1String("bookmarksViewEmpty(%1);").arg((state ? "true" : "false"));
    bookmarksWebView->page()->runJavaScript(itemScript);

    if (!state)
        bookmarksWebView->page()->runJavaScript(QLatin1String("clearBookmarksView();"));
}

void OBookmarksCore::saveWebIcon(const QIcon &icon)
{
    OWebView *webPage = qobject_cast<OWebView*>(sender());
    disconnect(webPage, &OWebView::iconChanged, this, &OBookmarksCore::saveWebIcon);

    for (int i = 0; bookmarksList.count() > i; i++)
    {
        if (bookmarksList.at(i)->getUrl().compare(webPage->url().toEncoded()) == 0)
        {
            if (!bookmarksDatabase.isOpenError())
            {
                QByteArray bytes;
                QBuffer buffer(&bytes);

                if (buffer.open(QIODevice::WriteOnly))
                {
                    if(!icon.pixmap(16, 16).save(&buffer, "PNG", 0))
                        qDebug("error saving icon file in savewebicon bmk");
                }
                else
                    qDebug("buffer failed to open");

                if (buffer.isOpen())
                    buffer.close();

                QString statement = QLatin1String("UPDATE lemonhistory SET bmkicon = :bmkicon WHERE bmkurl = :bmkurl");
                query.prepare(statement);
                query.bindValue(":bmkicon", bytes.toBase64());
                query.bindValue(":bmkurl", webPage->url().toEncoded());

                if (!query.exec())
                    qDebug(query.lastError().text().toLatin1());
                query.finish();

                bookmarksList.at(i)->setIcon(bytes.toBase64());
                break;
            }
            else
                qDebug(bookmarksDatabase.lastError().text().toLatin1());

            break;
        }
    }
}

void OBookmarksCore::searchInBookmark(const QString &value)
{
    loadEmptyBookmarkView(false);
    bool noResult = false;

    for (int i = 0; i < bookmarksList.count(); i++)
        if (bookmarksList.at(i)->getUrl().contains(value, Qt::CaseInsensitive) || bookmarksList.at(i)->getTitle().contains(value, Qt::CaseInsensitive))
        {
            OBookmarksItem *item = bookmarksList.at(i);
            QString iconPath = QLatin1String("qrc:/web_applications/images/webpage.png");

            if (item->getIcon() != "EmPtY64")
                iconPath = QLatin1String("data:image/png;base64,%1").arg(item->getIcon());

            QString itemScript = QLatin1String("loadSearchResults('%1', '%2', '%3', '%4', '%5');").arg(item->getTitle(), item->getUrl(), iconPath, item->getFolder(), QString::number(item->getIndex()));
            bookmarksWebView->page()->runJavaScript(itemScript);
            noResult = true;
        }

    if (!noResult && !value.isEmpty())
        bookmarksWebView->page()->runJavaScript("noBookmarkResult();");
    else if (!noResult && value.isEmpty() && bookmarksList.isEmpty())
        loadEmptyBookmarkView(true);
}

void OBookmarksCore::saveDatabase()
{
    bookmarksDatabase.close();
}

const int OBookmarksCore::generateIndex()
{
    return bookmarksUnusedIndexList.first();
}

void OBookmarksCore::setupIndexList()
{
    for (int i = 1; i < 102410; i++)
        bookmarksUnusedIndexList.append(i);
}

void OBookmarksCore::completeLastDeleting()
{
    if (!bookmarksDatabase.isOpenError())
    {
        if (deletedBookmarksList.isEmpty())
        {
            QString statementA1 = QLatin1String("SELECT deletedbmkid, deletedbmkurl, deletedbmktitle, deletedbmkfolder FROM ololandeletedbookmarks");
            if (query.exec(statementA1))
                while (query.next())
                {
                    OBookmarksItem *bookmark = new OBookmarksItem;
                    bookmark->setIndex(query.value(0).toInt());
                    bookmark->setUrl(query.value(1).toByteArray());
                    bookmark->setTitle(query.value(2).toByteArray());
                    bookmark->setFolder(query.value(3).toByteArray());
                    deletedBookmarksList.append(bookmark);
                }
        }

        for (int i = 0; i < deletedBookmarksList.count(); i++)
            deleteBookmarkItem(deletedBookmarksList.at(i)->getUrl(), deletedBookmarksList.at(i)->getFolder(), QString::number(deletedBookmarksList.at(i)->getIndex()));
        deletedBookmarksList.clear();

        QString statementA2 = QLatin1String("DELETE FROM ololandeletedbookmarks");
        if (!query.exec(statementA2))
            qDebug(query.lastError().text().toLatin1());
        query.finish();

        QString statementB1 = QLatin1String("SELECT deletedbmkfolders FROM ololandeletedfolder");
        if (query.exec(statementB1))
        {
            while (query.next())
                deleteFolder(query.value(0).toByteArray());

            QString statementB2 = QLatin1String("DELETE FROM ololandeletedfolder");
            if (!query.exec(statementB2))
                qDebug(query.lastError().text().toLatin1());
        }
        query.finish();
    }
}

void OBookmarksCore::setDeletedBookmark(const QString title, const QString url, const QString folder, const QString index)
{
    if (!bookmarksDatabase.isOpenError())
    {
        QString statement = QLatin1String("INSERT INTO ololandeletedbookmarks (deletedbmkid, deletedbmkurl, deletedbmktitle, deletedbmkfolder)"
                                          "VALUES(:deletedbmkid, :deletedbmkurl, :deletedbmktitle, :deletedbmkfolder)");
        query.prepare(statement);
        query.bindValue(":deletedbmkid", index.toInt());
        query.bindValue(":deletedbmkurl", url.toLatin1());
        query.bindValue(":deletedbmktitle", title.toLatin1());
        query.bindValue(":deletedbmkfolder", folder.toLatin1());

        if (query.exec())
        {
            OBookmarksItem *item = new OBookmarksItem();
            item->setIndex(index.toInt());
            item->setTitle(title);
            item->setUrl(url);
            item->setFolder(folder);
            deletedBookmarksList.append(item);
        }
        else
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OBookmarksCore::setDeletedFolder(const QString folder)
{
    if (!bookmarksDatabase.isOpenError())
    {
        QString statement = QLatin1String("INSERT INTO ololandeletedfolder (deletedbmkfolders) VALUES(:deletedbmkfolders)");
        query.prepare(statement);
        query.bindValue(":deletedbmkfolders", folder.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OBookmarksCore::clearBookmarkDeleting()
{
    if (!bookmarksDatabase.isOpenError())
    {
        QString statement = QLatin1String("DELETE FROM ololandeletedbookmarks");

        if (query.exec(statement))
            deletedBookmarksList.clear();
        else
            qDebug(query.lastError().text().toLatin1());

        query.finish();
    }
}

void OBookmarksCore::clearFolderDeleting()
{
    if (!bookmarksDatabase.isOpenError())
    {
        QString statement = QLatin1String("DELETE FROM ololandeletedfolder");

        if (!query.exec(statement))
            qDebug(query.lastError().text().toLatin1());

        query.finish();
    }
}

QStringList OBookmarksCore::getFolderList()
{
    return foldersList;
}

bool OBookmarksCore::itemExist(const QString title, const QString url)
{
    bool exist = false;

    for (int i = bookmarksList.count() - 1; i > -1; i--)
        if ((bookmarksList.at(i)->getTitle().compare(title) == 0) &&
            (bookmarksList.at(i)->getUrl().compare(url) == 0))
        {
            exist = true;
            break;
        }

    return exist;
}

bool OBookmarksCore::itemExist(const QString url)
{
    bool exist = false;

    for (int i = bookmarksList.count() - 1; i > -1; i--)
        if (bookmarksList.at(i)->getUrl().compare(url) == 0)
        {
            exist = true;
            break;
        }

    return exist;
}

QList<OBookmarksItem*> OBookmarksCore::getBookmarkList()
{
    return bookmarksList;
}

void OBookmarksCore::setState(bool state)
{
    isOpened = state;
}

const QString OBookmarksCore::lastAddedFolder(const QString url)
{
    QString bookmarkFolder = "Bookmark bar";

    for (int i = bookmarksList.count() - 1; i > -1; i--)
        if (bookmarksList.at(i)->getUrl().compare(url) == 0)
        {
            bookmarkFolder = bookmarksList.at(i)->getFolder();
            break;
        }

    return bookmarkFolder;
}

const QString OBookmarksCore::lastAddedTitle(const QString url)
{
    QString bookmarkTitle = "";

    for (int i = bookmarksList.count() - 1; i > -1; i--)
        if (bookmarksList.at(i)->getUrl().compare(url) == 0)
        {
            bookmarkTitle = bookmarksList.at(i)->getTitle();
            break;
        }

    return bookmarkTitle;
}

void OBookmarksCore::openInNewTab(const QString url)
{
    QString textUrl = url;
    QString historyUrl = QLatin1String("qrc:/web%20applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web%20applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web%20applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web%20applications/html/settingsView.html");
    QString newTabUrl = QLatin1String("qrc:/web%20applications/html/newtabView.html");

    if (textUrl.compare("ololan://history") == 0)
        textUrl = historyUrl;

    if (textUrl.compare("ololan://bookmarks") == 0)
        textUrl = bookmarksUrl;

    if (textUrl.compare("ololan://downloads") == 0)
        textUrl = downloadsUrl;

    if (textUrl.compare("ololan://settings") == 0)
        textUrl = settingsUrl;

    if (textUrl.compare("ololan://home") == 0)
        textUrl = newTabUrl;

    OAppCore *appCore = static_cast<OAppCore*>(parent());
    OTabPage *tabPagePtr = static_cast<OTabPage*>(appCore->allWindowsList().at(appCore->getCurrentWindowIndex())->getCentralWidget());
    tabPagePtr->openInTab(textUrl);
}

void OBookmarksCore::openInNewWindow(const QString url)
{
    QString textUrl = url;
    QString historyUrl = QLatin1String("qrc:/web%20applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web%20applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web%20applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web%20applications/html/settingsView.html");
    QString newTabUrl = QLatin1String("qrc:/web%20applications/html/newtabView.html");

    if (textUrl.compare("ololan://history") == 0)
        textUrl = historyUrl;

    if (textUrl.compare("ololan://bookmarks") == 0)
        textUrl = bookmarksUrl;

    if (textUrl.compare("ololan://downloads") == 0)
        textUrl = downloadsUrl;

    if (textUrl.compare("ololan://settings") == 0)
        textUrl = settingsUrl;

    if (textUrl.compare("ololan://home") == 0)
        textUrl = newTabUrl;

    OAppCore *appCore = static_cast<OAppCore*>(parent());
    appCore->openInWindow(textUrl);
}

void OBookmarksCore::openInPrivateWindow(const QString url)
{
    QString textUrl = url;
    QString historyUrl = QLatin1String("qrc:/web%20applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web%20applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web%20applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web%20applications/html/settingsView.html");
    QString newTabUrl = QLatin1String("qrc:/web%20applications/html/newtabView.html");

    if (textUrl.compare("ololan://history") == 0)
        textUrl = historyUrl;

    if (textUrl.compare("ololan://bookmarks") == 0)
        textUrl = bookmarksUrl;

    if (textUrl.compare("ololan://downloads") == 0)
        textUrl = downloadsUrl;

    if (textUrl.compare("ololan://settings") == 0)
        textUrl = settingsUrl;

    if (textUrl.compare("ololan://home") == 0)
        textUrl = newTabUrl;

    OAppCore *appCore = static_cast<OAppCore*>(parent());
    appCore->openInPrivateWindow(textUrl);
}

void OBookmarksCore::setTabPage(OTabPage *tPage)
{
    tabPage = tPage;
}

void OBookmarksCore::updateBookmarkBar()
{
    QPixmap icon = QPixmap(13, 13);
    icon.loadFromData(QByteArray::fromBase64(bookmarksList.last()->getIcon()), "PNG");
    QAction *bookmarkItem = new QAction();

    if (bookmarksList.last()->getIcon().compare("EmPtY64") != 0)
        bookmarkItem->setIcon(icon);
    else
        bookmarkItem->setIcon(QIcon(":/web_applications/images/webpage.png"));

    bookmarkItem->setText(bookmarksList.last()->getTitle());
    bookmarkItem->setToolTip(bookmarksList.last()->getTitle() +"*x-z-x*"+ bookmarksList.last()->getUrl());
    bookmarkItem->setData(QString::number(bookmarksList.last()->getIndex()));

    OAppCore *parentPtr = static_cast<OAppCore*>(parent());
    parentPtr->updateBookmarkBar(bookmarkItem);
}

void OBookmarksCore::updateBookmarkBar(const QString index, const QString newTitle, const QString newUrl)
{
    OAppCore *parentPtr = static_cast<OAppCore*>(parent());
    parentPtr->updateBookmarkBar(index, newTitle, newUrl);
}

QList<QAction*> OBookmarksCore::getBookmarkBarItemsList()
{
    QList<QAction*> itemsList;
    for (int i = 0; i < bookmarksList.count(); i++)
        if (bookmarksList.at(i)->getFolder().compare("Bookmark bar") == 0)
        {
            QPixmap icon = QPixmap(13, 13);
            icon.loadFromData(QByteArray::fromBase64(bookmarksList.at(i)->getIcon()), "PNG");
            QAction *bookmarkItem = new QAction();

            if (bookmarksList.at(i)->getIcon().compare("EmPtY64") != 0)
                bookmarkItem->setIcon(icon);
            else
                bookmarkItem->setIcon(QIcon(":/web_applications/images/webpage.png"));

            bookmarkItem->setText(bookmarksList.at(i)->getTitle());
            bookmarkItem->setToolTip(bookmarksList.at(i)->getTitle() +"*x-z-x*"+ bookmarksList.at(i)->getUrl());
            bookmarkItem->setData(QString::number(bookmarksList.at(i)->getIndex()));

            itemsList.append(bookmarkItem);
        }

    return itemsList;
}

void OBookmarksCore::deleteBookmarkBarItem(const QString bmkIndex)
{
    OAppCore *parentPtr = static_cast<OAppCore*>(parent());
    parentPtr->deleteBookmarkBarItem(bmkIndex);
}

void OBookmarksCore::setEditingItem(const QString index, bool state)
{
    isEditingItem = state;
    editingIndex = index.toInt();
}

void OBookmarksCore::openAllInNewTab()
{
    int openTime = 0;
    for (int i = 0; i < bookmarksList.count(); i++)
    {
        openTime++;
        const QString url = bookmarksList.at(i)->getUrl();
        QTimer *timer = new QTimer(this);
        timer->setInterval(openTime*100);
        connect(timer, &QTimer::timeout, this, [=]() { openInNewTab(url); timer->stop(); });
        timer->start();
    }
}

void OBookmarksCore::openAllInNewWiwndow()
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    int count = 0;
    int openTime = 1;

    for (int i = 0; i < bookmarksList.count(); i++)
    {
        if (count == 0)
            openInNewWindow(bookmarksList.at(i)->getUrl());
        else
        {
            openTime++;
            OTabPage *tabPage = static_cast<OTabPage*>(appCore->allWindowsList().last()->getCentralWidget());
            const QString url = bookmarksList.at(i)->getUrl();
            QTimer *timer = new QTimer(this);
            timer->setInterval(openTime*100);
            connect(timer, &QTimer::timeout, this, [=]() { tabPage->openInTab(url); timer->stop(); });
            timer->start();
        }
        count++;
    }
}

void OBookmarksCore::openAllInPrivateWindow()
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    int count = 0;
    int openTime = 1;

    for (int i = 0; i < bookmarksList.count(); i++)
    {
        if (count == 0)
            openInPrivateWindow(bookmarksList.at(i)->getUrl());
        else
        {
            openTime++;
            OTabPage *tabPage = static_cast<OTabPage*>(appCore->allWindowsList().last()->getCentralWidget());
            const QString url = bookmarksList.at(i)->getUrl();
            QTimer *timer = new QTimer(this);
            timer->setInterval(openTime*100);
            connect(timer, &QTimer::timeout, this, [=]() { tabPage->openInTab(url); timer->stop(); });
            timer->start();
        }
        count++;
    }
}

void OBookmarksCore::openOccurenceInNewTab(QString bookmark)
{
    int openTime = 0;
    for (int i = 0; i < bookmarksList.count(); i++)
    {
        if (bookmarksList.at(i)->getTitle().startsWith(bookmark, Qt::CaseInsensitive) ||
            bookmarksList.at(i)->getTitle().contains(bookmark, Qt::CaseInsensitive))
        {
            openTime++;
            const QString url = bookmarksList.at(i)->getUrl();
            QTimer *timer = new QTimer(this);
            timer->setInterval(openTime*100);
            connect(timer, &QTimer::timeout, this, [=]() { openInNewTab(url); timer->stop(); });
            timer->start();
        }
    }
}

void OBookmarksCore::openOccurenceInNewWindow(QString bookmark)
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    int count = 0;
    int openTime = 1;

    for (int i = 0; i < bookmarksList.count(); i++)
    {
        if (bookmarksList.at(i)->getTitle().startsWith(bookmark, Qt::CaseInsensitive) ||
            bookmarksList.at(i)->getTitle().contains(bookmark, Qt::CaseInsensitive))
        {
            if (count == 0)
                openInNewWindow(bookmarksList.at(i)->getUrl());
            else
            {
                openTime++;
                OTabPage *tabPage = static_cast<OTabPage*>(appCore->allWindowsList().last()->getCentralWidget());
                const QString url = bookmarksList.at(i)->getUrl();
                QTimer *timer = new QTimer(this);
                timer->setInterval(openTime*100);
                connect(timer, &QTimer::timeout, this, [=]() { tabPage->openInTab(url); timer->stop(); });
                timer->start();
            }
            count++;
        }
    }
}

void OBookmarksCore::openOccurenceInPrivateWindow(QString bookmark)
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    int count = 0;
    int openTime = 1;

    for (int i = 0; i < bookmarksList.count(); i++)
    {
        if (bookmarksList.at(i)->getTitle().startsWith(bookmark, Qt::CaseInsensitive) ||
            bookmarksList.at(i)->getTitle().contains(bookmark, Qt::CaseInsensitive))
        {
            if (count == 0)
                openInPrivateWindow(bookmarksList.at(i)->getUrl());
            else
            {
                openTime++;
                OTabPage *tabPage = static_cast<OTabPage*>(appCore->allWindowsList().last()->getCentralWidget());
                const QString url = bookmarksList.at(i)->getUrl();
                QTimer *timer = new QTimer(this);
                timer->setInterval(openTime*100);
                connect(timer, &QTimer::timeout, this, [=]() { tabPage->openInTab(url); timer->stop(); });
                timer->start();
            }
            count++;
        }
    }
}

void OBookmarksCore::openFolderInNewTab(QString folder)
{
    int openTime = 0;
    for (int i = 0; i < bookmarksList.count(); i++)
    {
        if (bookmarksList.at(i)->getFolder().compare(folder, Qt::CaseSensitive) == 0)
        {
            openTime++;
            const QString url = bookmarksList.at(i)->getUrl();
            QTimer *timer = new QTimer(this);
            timer->setInterval(openTime*100);
            connect(timer, &QTimer::timeout, this, [=]() { openInNewTab(url); timer->stop(); });
            timer->start();
        }
    }
}

void OBookmarksCore::openFolderInNewWindow(QString folder)
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    int count = 0;
    int openTime = 1;

    for (int i = 0; i < bookmarksList.count(); i++)
    {
        if (bookmarksList.at(i)->getFolder().compare(folder, Qt::CaseSensitive) == 0)
        {
            if (count == 0)
                openInNewWindow(bookmarksList.at(i)->getUrl());
            else
            {
                openTime++;
                OTabPage *tabPage = static_cast<OTabPage*>(appCore->allWindowsList().last()->getCentralWidget());
                const QString url = bookmarksList.at(i)->getUrl();
                QTimer *timer = new QTimer(this);
                timer->setInterval(openTime*100);
                connect(timer, &QTimer::timeout, this, [=]() { tabPage->openInTab(url); timer->stop(); });
                timer->start();
            }
            count++;
        }
    }
}

void OBookmarksCore::openFolderInPrivate(QString folder)
{
    OAppCore *appCore = static_cast<OAppCore*>(parent());
    int count = 0;
    int openTime = 1;

    for (int i = 0; i < bookmarksList.count(); i++)
    {
        if (bookmarksList.at(i)->getFolder().compare(folder, Qt::CaseSensitive) == 0)
        {
            if (count == 0)
                openInPrivateWindow(bookmarksList.at(i)->getUrl());
            else
            {
                openTime++;
                OTabPage *tabPage = static_cast<OTabPage*>(appCore->allWindowsList().last()->getCentralWidget());
                const QString url = bookmarksList.at(i)->getUrl();
                QTimer *timer = new QTimer(this);
                timer->setInterval(openTime*100);
                connect(timer, &QTimer::timeout, this, [=]() { tabPage->openInTab(url); timer->stop(); });
                timer->start();
            }
            count++;
        }
    }
}

void OBookmarksCore::deleteBmkOccurence(QString bookmark)
{
    for (int i = 0; i < bookmarksList.count(); i++)
    {
        if (bookmarksList.at(i)->getTitle().startsWith(bookmark, Qt::CaseInsensitive) ||
            bookmarksList.at(i)->getTitle().contains(bookmark, Qt::CaseInsensitive))
        {
            deleteBookmarkItem(bookmarksList.at(i)->getUrl(), bookmarksList.at(i)->getFolder(),
                               QString::number(bookmarksList.at(i)->getIndex()));
            i--;
        }
    }

    bookmarksWebView->page()->runJavaScript(QLatin1String("reloadCurrentFolder();"));
}

void OBookmarksCore::clearBookmark()
{
    for (int i = 0; i < bookmarksList.count(); i++)
    {
        deleteBookmarkItem(bookmarksList.at(i)->getUrl(), bookmarksList.at(i)->getFolder(),
                           QString::number(bookmarksList.at(i)->getIndex()));
        i--;
    }

    while (foldersList.count() > 1)
    {
        deleteFolder(foldersList.at(1));
    }

    bookmarksWebView->page()->runJavaScript(QLatin1String("removeFolder();"));
    bookmarksWebView->page()->runJavaScript(QLatin1String("clearBookmarksView();"));
    selectFolder("Bookmark bar");
    loadEmptyBookmarkView();
}

void OBookmarksCore::clearBmkFolder(QString folder)
{
    for (int i = 0; i < bookmarksList.count(); i++)
    {
        if (bookmarksList.at(i)->getFolder().compare(folder, Qt::CaseSensitive) == 0)
        {
            deleteBookmarkItem(bookmarksList.at(i)->getUrl(), bookmarksList.at(i)->getFolder(),
                               QString::number(bookmarksList.at(i)->getIndex()));
            i--;
        }
    }

    bookmarksWebView->page()->runJavaScript(QLatin1String("reloadCurrentFolder();"));
}

bool OBookmarksCore::isItemExisting(QString item)
{
    bool exist = false;
    for (int i = 0; i < bookmarksList.count(); i++)
        if (bookmarksList.at(i)->getTitle().startsWith(item, Qt::CaseInsensitive) ||
            bookmarksList.at(i)->getTitle().contains(item, Qt::CaseInsensitive))
        {
            exist = true;
            break;
        }

    return exist;
}

bool OBookmarksCore::isFolderExisting(QString folder)
{
    bool exist = false;
    for (int i = 0; i < bookmarksList.count(); i++)
        if (bookmarksList.at(i)->getFolder().compare(folder, Qt::CaseSensitive) == 0)
        {
            exist = true;
            break;
        }

    return exist;
}

bool OBookmarksCore::isFolderEmpty(QString folder)
{
    bool exist = true;
    for (int i = 0; i < bookmarksList.count(); i++)
        if (bookmarksList.at(i)->getFolder().compare(folder, Qt::CaseSensitive) == 0)
        {
            exist = false;
            break;
        }

    return exist;
}
