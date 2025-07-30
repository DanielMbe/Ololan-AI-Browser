#ifndef OHISTORYCORE_H
#define OHISTORYCORE_H

#include <QUrl>
#include <QIcon>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "owebview.h"
#include <QWebChannel>

class OHistoryItem : public QObject
{
public:
    OHistoryItem(const QUrl url = QUrl(), const QString title = QString(), const QByteArray icon = QByteArray(), const QDateTime lastVisited = QDateTime(), const int index = -1);
    void setUrl(const QUrl url);
    void setTitle(const QString title);
    void setLastVisited(const QDateTime lastVisited);
    void setDeviceName(const QString device);
    void setIcon(const QByteArray icon);
    void setIndex(const int index);
    const QUrl getUrl();
    const QString getTitle();
    const QString getDeviceName();
    const QDateTime getLastVisited();
    const QByteArray getIcon();
    const int getIndex();

private:
    QUrl itemUrl;
    QString itemTitle;
    QString deviceName;
    QDateTime itemDateTime;
    QByteArray itemIcon;
    int itemIndex;
};

class OHistoryCore : public QObject
{
    Q_OBJECT

public:
    explicit OHistoryCore(QObject *parent = nullptr);
    void updateHistoryView();
    void addItem(OHistoryItem *item);
    void loadEmptyHistoryView(bool state = true);
    void setHistoryWebView(OWebView *webView);
    void setupIndexList();
    void saveDatabase();
    void openDateInNewTab(QString date);
    void openDateInNewWindow(QString date);
    void openDateInPrivateWindow(QString date);
    void openHistoryInNewTab();
    void openHistoryInNewWindow();
    void openHistoryInPrivateWindow();
    void openOccurenceInNewTab(QString page);
    void openOccurenceInNewWindow(QString page);
    void openOccurenceInPrivateWindow(QString page);
    void deleteByOccurence(QString page);
    void deleteByDate(QString date);
    bool isDateExisting(QString date);
    bool isItemExisting(QString item);
    const int generateIndex();
    const int getHistoryDataSize();
    const QStringList getDateAsString(const QDate date, bool primaryDate = false);
    const QString getDayAsString(const int day);
    const QString getMonthAsString(const int month);
    const QString getTimeAsString(const QTime time);
    OWebView *getHistoryWebView();
    QList<OHistoryItem*> getHistoryItemsList();

public slots:    
    void pageLoadFinished(bool status);
    void clearHistory();
    void saveWebIcon(const QIcon &icon);
    void removeHistoryItem(const QString index);
    void loadHistoryItems();
    void searchInHistory(const QString &value);
    void clearBrowsingData();

private:
    QSqlDatabase historyDatabase;
    QList<OHistoryItem*> historyItemsList;
    QString errorType;
    OWebView *historyWebView;
    QSqlQuery query;
    QWebChannel *webChannel;
    QList<int> historyIndexList;
};

#endif // OHISTORYCORE_H
