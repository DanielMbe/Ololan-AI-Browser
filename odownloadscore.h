#ifndef ODOWNLOADSCORE_H
#define ODOWNLOADSCORE_H

#include <QObject>
#include <QDate>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QElapsedTimer>
#include "owebview.h"
#include <QWebChannel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

class ODownloadItem : public QObject
{
    Q_OBJECT

public:
    ODownloadItem(QObject *parent = nullptr);
    void setTitle(const QString title);
    void setUrl(const QString url);
    void setSize(const QString size);
    void setDownloadedSize(const QString downloadedSize);
    void setIcon(const QByteArray icon);
    void setIndex(const int index);
    void setDownloadDate(const QDate downloadDate);
    void setDownloadItem(QNetworkReply *reply);
    void setState(const QString state);
    void setDownloadDirectory(const QString directory);
    void start();
    void pause();
    void stop();
    void resume();
    void createDownloadFile();
    void closeFile();
    const QString getTitle();
    const QString getUrl();
    const QString getSize();
    const QString getDownloadedSize();
    const QByteArray getBytesSize();
    const QString getState();
    const QString getDownloadDirectory();
    const QByteArray getIcon();
    const QDate getDownloadDate();
    const int getIndex();
    const quint64 getElapsedTime();
    QNetworkReply *getDownloadItem();
    QElapsedTimer elapsedTime;

public slots:
    void saveToFile();

private:
    QString itemTitle;
    QString itemUrl;
    QString itemSize;
    QString itemDownloadedSize;
    QString itemState;
    QString itemDir;
    QByteArray itemIcon;
    QDate itemDate;
    QFile *downloadFile;
    QNetworkReply *download;
    int itemIndex;
};

class ODownloadsCore : public QObject
{
    Q_OBJECT

public:
    explicit ODownloadsCore(QObject *parent = nullptr);
    void setDownloadsWebView(OWebView *webView);
    void loadEmptyDownloadView(bool state = true);
    void setupIndexList();
    void saveDatabase();
    void addItem(ODownloadItem *item);
    void updateDownloadsList(ODownloadItem *item);
    void startDownload(const QString index);
    void pauseAllDownloads();
    void pauseDownloadByDate(QString date);
    void resumeAllDownloads();
    void resumeByDate(QString date);
    void restartAllDownloads();
    void restartByDate(QString date);
    void stopByDate(QString date);
    void stopAllDownloads();
    void deleteByDate(QString date);
    void deleteAllDownloads();
    bool isDateExisting(QString date);
    bool hasActiveDownload();
    bool hasPausedDownload();
    bool hasFailedDownload();
    const int generateIndex();
    const QStringList getDateAsString(const QDate date, bool primaryDate = false);
    const QString getDayAsString(const int day);
    const QString getMonthAsString(const int month);
    const QString getTimeLeft(const int seconds);
    QList<ODownloadItem*> getDownloadItemList();

public slots:
    void acceptDownloadItem(QWebEngineDownloadRequest *download);
    void loadDownloadItem(bool status);
    void restartDownload(const QString index);
    void deleteDownloadItem(const QString index);
    void pauseDownload(const QString index);
    void stopDownload(const QString index);
    void openDownloadLocation(const QString index);
    void resumeDownload(const QString index);
    void searchInDownloads(const QString &value);
    void openDownloadsFolder();
    void clearDownloadsList();
    void downloadFinished();
    void updateTransferRate(quint64 receivedBytes, quint64 totalBytes);
    void setDownloadWidget();
    void unsetDownloadWidget();
    void pauseDownloadWeb(const QString index);
    void stopDownloadWeb(const QString index);
    void resumeDownloadWeb(const QString index);

signals:
    void removeWidget(const int index);
    void updateWidgetTransfertRate(const int index, const QString rate, const QString size, const int percentage);

private:
    QSqlDatabase downloadDatabase;
    QList<ODownloadItem*> downloadItemsList;
    OWebView *downloadWebView;
    QNetworkAccessManager *webManager;
    QSqlQuery query;
    QWebChannel *webChannel;
    QList<int> downloadIndexList;
    bool isReady;
};

#endif // ODOWNLOADSCORE_H
