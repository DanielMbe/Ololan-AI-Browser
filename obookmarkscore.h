#ifndef OBOOKMARKSCORE_H
#define OBOOKMARKSCORE_H

#include <QUrl>
#include <QIcon>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "owebview.h"
#include <QWebChannel>

class OTabPage;
class OBookmarksItem : public QObject
{
public:
    explicit OBookmarksItem(QObject *parent = nullptr, const QString url = QString(), const QString title = QString(), const QByteArray icon = QByteArray(), const QString folder = QString(), const int index = 1);
    void setUrl(const QString url);
    void setTitle(const QString title);
    void setDeviceName(const QString device);
    void setIcon(const QByteArray icon);
    void setFolder(const QString folder);
    void setIndex(const int index);
    const QString getUrl();
    const QString getTitle();
    const QString getFolder();
    const QByteArray getIcon();
    const int getIndex();

private:
    QString itemUrl;
    QString itemTitle;
    QString deviceName;
    QString itemFolder;
    QByteArray itemIcon;
    int itemIndex;
};

class OBookmarksCore : public QObject
{
    Q_OBJECT

public:
    explicit OBookmarksCore(QObject *parent = nullptr);
    void setBookmarksWebView(OWebView *webView);
    void loadEmptyBookmarkView(bool state = true);
    void completeLastDeleting();
    void setupIndexList();
    void saveDatabase();
    void setState(bool state);
    void deleteBookmarkItem(const QString title, const QUrl url, const QString folder);
    void updateBookmarkItem(const QString title, const QString url, const QString folder);
    void updateBookmarkBar();
    void updateBookmarkBar(const QString index, const QString newTitle, const QString newUrl);
    void deleteBookmarkBarItem(const QString bmkIndex);
    void setTabPage(OTabPage *tPage);
    void setEditingItem(const QString index, bool state);
    void openAllInNewTab();
    void openAllInNewWiwndow();
    void openAllInPrivateWindow();
    void openOccurenceInNewTab(QString bookmark);
    void openOccurenceInNewWindow(QString bookmark);
    void openOccurenceInPrivateWindow(QString bookmark);
    void openFolderInNewTab(QString folder);
    void openFolderInNewWindow(QString folder);
    void openFolderInPrivate(QString folder);
    void deleteBmkOccurence(QString bookmark);
    void clearBookmark();
    void clearBmkFolder(QString folder);
    bool itemExist(const QString title, const QString url);
    bool itemExist(const QString url);
    bool isItemExisting(QString item);
    bool isFolderExisting(QString folder);
    bool isFolderEmpty(QString folder);
    const int generateIndex();
    const QString lastAddedFolder(const QString url);
    const QString lastAddedTitle(const QString url);
    QStringList getFolderList();
    QList<OBookmarksItem*> getBookmarkList();
    QList<QAction *> getBookmarkBarItemsList();

public slots:
    void loadBookmarks(bool status);
    void addBookmarkItem(const QString title, const QString url, QIcon favicon, const QString folder);
    void addBookmarkItem(const QString title, const QString url, const QString folder);
    void addFolder(const QString folder);
    void deleteBookmarkItem(const QString url, const QString folder, const QString bmkIndex);
    void deleteBmkFromBar(const QString url, const QString folder, const QString bmkIndex);
    void deleteBookmarkItem(const QString folder);
    void deleteFolder(const QString folder);
    void updateBookmarkItem(const QString title, const QString url, const QString folder, const QString bmkIndex);
    void selectFolder(const QString folder);
    void changeFolderName(const QString oldName, const QString newName);
    void moveBookmarkItem(const QString url, const QString bmkIndex, const QString from, const QString to);
    void duplicateBookmarkItem(const QString title, const QString url, const QString destinationFolder);
    void saveWebIcon(const QIcon &icon);
    void searchInBookmark(const QString &value);
    void updateBookmarkView(const QByteArray icon, const QString title, const QString url, const QString folder, const QString index);
    void setDeletedBookmark(const QString title, const QString url, const QString folder, const QString index);
    void setDeletedFolder(const QString folder);
    void clearFolderDeleting();
    void clearBookmarkDeleting();
    void openInNewTab(const QString url);
    void openInNewWindow(const QString url);
    void openInPrivateWindow(const QString url);

signals:
    void bookmarkDeleted(const QString url);

private:
    OWebView *bookmarksWebView;
    QSqlQuery query;
    QList<OBookmarksItem*> bookmarksList;
    QList<OBookmarksItem*> deletedBookmarksList;
    QList<int> bookmarksUsedIndexList;
    QList<int> bookmarksUnusedIndexList;
    QStringList foldersList;
    QSqlDatabase bookmarksDatabase;
    QWebChannel *webChannel;
    OTabPage *tabPage;
    bool isOpened;
    bool isEditingItem;
    int editingIndex;
};

#endif // OBOOKMARKSCORE_H
