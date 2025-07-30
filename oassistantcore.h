#ifndef OASSISTANTCORE_H
#define OASSISTANTCORE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QTimer>

class OTabPage;
class OHistoryCore;
class OBookmarksCore;
class ODownloadsCore;
class OAssistantCore : public QObject
{
    Q_OBJECT

public:
    explicit OAssistantCore(QObject *parent = nullptr);
    void setTabPage(OTabPage *currentTabPage);
    void setHistory(OHistoryCore *historyPtr);
    void setBookmark(OBookmarksCore *bookmarkPtr);
    void setDownload(ODownloadsCore *downloadPtr);
    void fillTable();
    void addAssistantSpeech(const QString speech);
    void executeGeneralCommand(const QString cmdCode, const QString keyword, const QString reply);
    void executeGeneralTabCommand(const QString tabCmdCode, const QString reply);
    void executeTabOpenCommand(const QString openCmdCode, const QString reply);
    void executeTabCloseCommand(const QString closeCmdCode, const QString reply);
    void executeTabSelectCommand(const QString selectCmdCode, const QString reply);
    void executeGeneralWindowCommand(const QString windowCmdCode, const QString reply);
    void executeWindowOpenCommand(const QString openCmdCode);
    void executeWindowCloseCommand(const QString closeCmdCode, const QString reply);
    void executeWindowSelectCommand(const QString selectCmdCode);
    void executeGeneralAppCommand(const QString appCmdCode, const QString reply);
    void executeGeneralBrowserCommand(const QString browserCmdCode, const QString reply);
    void executeHistoryCommand(const QString cmdCode, const QString keyword, const QString reply);
    void executeHistOpenCommand(const QString openCmdCode, const QString reply);
    void executeHistDeleteCommand(const QString deleteCmdCode, const QString reply);
    void executeBookmarkCommand(const QString cmdCode, const QString keyword, const QString reply);
    void executeBmkOpenCommand(const QString openCmdCode, const QString reply);
    void executeBmkDeleteCommand(const QString deleteCmdCode, const QString reply);
    void executeDownloadCommand(const QString cmdCode, const QString keyword, const QString reply);
    void executeDwnPauseCommand(const QString pauseCmdCode, const QString reply);
    void executeDwnResumeCommand(const QString resumeCmdCode, const QString reply);
    void executeDwnRestartCommand(const QString restartCmdCode, const QString reply);
    void executeDwnStopCommand(const QString stopCmdCode, const QString reply);
    void executeDwnOtherCommand(const QString otherCmdCode, const QString reply);

signals:
    void sendSuggestions(const QStringList commands);
    void sendAssistantSpeech(const QString speech);

public slots:
    void suggestCommands(const QString input, const QString processor);
    void sendCommand(const QString input, const QString processor);
    void showSuggestions(const QStringList commands);
    void processServerResponse(QNetworkReply *serverResponse);
    void processServerSuggestion(QNetworkReply *serverResponse);
    void stopAwaiting();

private:
    QList<QString> commandsList;
    QList<QString> keywordList;
    OTabPage *tabPage;
    OHistoryCore *histManager;
    OBookmarksCore *bmkManager;
    ODownloadsCore *dwnManager;
    QSqlDatabase assistantDatabase;
    QNetworkAccessManager *serverManager;
    QNetworkAccessManager *suggestionManager;
    QTimer *awaiting;
    QSqlQuery query;
    QString subject;
    QString question;
    QString cmdKeyword;
    QString questionCmdCode;
    QString currentProcessor;
    QString typingText;
    bool isAwaiting;
    bool isOpen;
    bool suggestFromGenP;
};

#endif // OASSISTANTCORE_H
