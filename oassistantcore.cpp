#include "oassistantcore.h"
#include "oappcore.h"
#include "otabpage.h"
#include "customtoolbar.h"
#include "osmartinput.h"
#include "otabbar.h"
#include "oappcore.h"
#include "customwindow.h"
#include "owebview.h"
#include "otabpage.h"
#include "ohistorycore.h"
#include "obookmarkscore.h"
#include "odownloadscore.h"
#include "osettingscore.h"
#include <QSqlError>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>

OAssistantCore::OAssistantCore(QObject *parent)
    :QObject(parent)
{
    isAwaiting = false;
    isOpen = false;
    suggestFromGenP = false;
    subject = "";
    question = "";
    cmdKeyword = "";
    currentProcessor = "";
    questionCmdCode = "";
    typingText = "";
    histManager = nullptr;
    bmkManager = nullptr;
    dwnManager = nullptr;
    serverManager = new QNetworkAccessManager(this);
    connect(serverManager, &QNetworkAccessManager::finished, this, &OAssistantCore::processServerResponse);
    suggestionManager = new QNetworkAccessManager(this);
    connect(suggestionManager, &QNetworkAccessManager::finished, this, &OAssistantCore::processServerSuggestion);
    awaiting = new QTimer(this);

    QString databaseName = OAppCore::applicationDirPath().append("/browserDBDir/ololanassistantdatabase.gdb");
    assistantDatabase = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), QLatin1String("ololanAssistantDatabase"));
    assistantDatabase.setDatabaseName(databaseName);
    assistantDatabase.setHostName(QLatin1String("ololanhost"));
    assistantDatabase.setUserName(QLatin1String("ololanRoot"));
    assistantDatabase.setPassword(QLatin1String("OlolanBrowserDB2022"));

    if (assistantDatabase.open(QLatin1String("ololanRoot"), QLatin1String("OlolanBrowserDB2022")))
    {
        query = QSqlQuery(assistantDatabase);
        QString statementA1 = QLatin1String("SELECT astkeyword, astcmdcode FROM ololanassistant");
        if (query.exec(statementA1))
        {
            while (query.next())
            {
                keywordList.append(query.value(0).toString());
                commandsList.append(query.value(1).toString());
            }
        }
        else
        {
            qDebug(query.lastError().text().toLatin1());
            QString statementA2 = QLatin1String("CREATE TABLE ololanassistant(astkeyword VARCHAR(128) NOT NULL,"
                                                "astcmdcode VARCHAR(128) NOT NULL, PRIMARY KEY (astcmdcode))");
            if (!query.exec(statementA2))
                qDebug(query.lastError().text().toLatin1());
            else
                fillTable();
        }
    }
    else
        qDebug(assistantDatabase.lastError().text().toLatin1());
}

void OAssistantCore::fillTable()
{
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('tab', 'oant')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('tab', 'oatf')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('tab', 'cnt')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('tab', 'cpt')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('tabs', 'cant')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('tabs', 'capt')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('tabs', 'cot')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('tabs', 'cthl')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('tabs', 'cthr')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('tab', 'snt')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('tab', 'spt')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('tab', 'slt')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('tab', 'sft')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('window', 'oanw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('window', 'oapw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('window', 'cfw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('window', 'clw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('windows', 'cow')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('window', 'snw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('window', 'spw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('window', 'sfw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('window', 'slw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Stop', 'saad')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('help', 'ohc')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Report','ria')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Find', 'ft')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'ob')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'oh')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'od')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'os')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('bar', 'sbb')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('bar', 'hbb')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('page', 'spifm')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('page', 'ptp')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('page', 'stp')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('page', 'spsc')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('page', 'dtp')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Visit', 'vst')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Search for', 'sch')"));

    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Delete', 'daoo')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Delete', 'dvpo')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Clear', 'ch')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'ovpoint')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'ovpoinw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'ovpoipw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'oahint')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'oahinw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'oahipw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'ooopint')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'ooopinw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'ooopipw')"));

    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Delete', 'daboo')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Clear', 'cb')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Clear', 'cf')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'oabint')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'oabinw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'oabipw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'obooint')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'obooinw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'obooipw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'ofint')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open ','ofinw')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'ofipw')"));

    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Delete', 'ddo')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Clear', 'cdl')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Pause', 'paad')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Pause', 'pado')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Resume', 'rapd')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Resume', 'rpdo')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Open', 'odf')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Restart', 'rafd')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Restart', 'rfdo')"));
    query.exec(QLatin1String("INSERT INTO ololanassistant (astkeyword, astcmdcode) VALUES('Stop', 'sado')"));

    QString statementA1 = QLatin1String("SELECT astkeyword, astcmdcode FROM ololanassistant");
    if (query.exec(statementA1))
        while (query.next())
        {
            keywordList.append(query.value(0).toString());
            commandsList.append(query.value(1).toString());
        }
    query.finish();
}

void OAssistantCore::suggestCommands(const QString input, const QString processor)
{
    OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
    if (!parentPtr->getSettingsManager()->getEnableAssistant())
        return;

    typingText = input;
    currentProcessor = processor;
    QNetworkRequest request;
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.2; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Ololan/3.1.673.33 Chrome/118.0.5993.220 Safari/537.36");
    QString url = QLatin1String("https://cloudai.ololan.com?");

    if (processor.compare("generalProcessor") == 0)
    {
        url = url.append("suggestion=true&processor=general&suggestionvalue=").append(input);
        request.setUrl(QUrl::fromUserInput(url));
        suggestionManager->get(request);
    }
    else if (processor.compare("historyProcessor") == 0)
    {
        url = url.append("suggestion=true&processor=history&suggestionvalue=").append(input);
        request.setUrl(QUrl::fromUserInput(url));
        suggestionManager->get(request);
    }
    else if (processor.compare("bookmarkProcessor") == 0)
    {
        url = url.append("suggestion=true&processor=bookmark&suggestionvalue=").append(input);
        request.setUrl(QUrl::fromUserInput(url));
        suggestionManager->get(request);
    }
    else if (processor.compare("downloadProcessor") == 0)
    {
        url = url.append("suggestion=true&processor=download&suggestionvalue=").append(input);
        request.setUrl(QUrl::fromUserInput(url));
        suggestionManager->get(request);
    }
}

void OAssistantCore::sendCommand(const QString input, const QString processor)
{
    OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
    if (!parentPtr->getSettingsManager()->getEnableAssistant())
    {
        addAssistantSpeech("Sorry, I'm disable, go to settings and enable Ololan Eina before chatting.");
        return;
    }

    currentProcessor = processor;
    QNetworkRequest request;
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.2; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Ololan/3.1.673.33 Chrome/118.0.5993.220 Safari/537.36");
    QString url = QLatin1String("https://cloudai.ololan.com?");

    if (processor.compare("generalProcessor") == 0)
    {
        if (isAwaiting)
        {
            stopAwaiting();
            url = url.append("question=true&processor=general&questionvalue=").append(input).append(":::" + questionCmdCode);
        }
        else
            url = url.append("command=true&processor=general&commandvalue=").append(input);

        request.setUrl(QUrl::fromUserInput(url));
        serverManager->get(request);
    }
    else if (processor.compare("historyProcessor") == 0)
    {
        if (isAwaiting)
        {
            stopAwaiting();
            url = url.append("question=true&processor=history&questionvalue=").append(input).append(":::" + questionCmdCode);
        }
        else
            url = url.append("command=true&processor=history&commandvalue=").append(input);

        request.setUrl(QUrl::fromUserInput(url));
        serverManager->get(request);
    }
    else if (processor.compare("bookmarkProcessor") == 0)
    {
        if (isAwaiting)
        {
            stopAwaiting();
            url = url.append("question=true&processor=bookmark&questionvalue=").append(input).append(":::" + questionCmdCode);
        }
        else
            url = url.append("command=true&processor=bookmark&commandvalue=").append(input);

        request.setUrl(QUrl::fromUserInput(url));
        serverManager->get(request);
    }
    else if (processor.compare("downloadProcessor") == 0)
    {
        if (isAwaiting)
        {
            stopAwaiting();
            url = url.append("question=true&processor=download&questionvalue=").append(input).append(":::" + questionCmdCode);
        }
        else
        url = url.append("command=true&processor=download&commandvalue=").append(input);

        request.setUrl(QUrl::fromUserInput(url));
        serverManager->get(request);
    }
}

void OAssistantCore::processServerResponse(QNetworkReply *serverResponse)
{
    if (serverResponse->error() != QNetworkReply::NoError)
        addAssistantSpeech(QLatin1String("Sorry, i can't help you right now, try again later."));
    else
    {
        bool found = false;
        QJsonObject response = QJsonDocument::fromJson(serverResponse->readAll()).object();
        QString cmdCode = response["commandCode"].toString();
        QString reply = response["reply"].toString();

        question = response["question"].toString();
        if (question.compare("unknow") == 0)
            question = "";

        subject = response["subject"].toString();
        if (subject.compare("unknow") == 0)
            subject = "";
        else if (reply.contains("#"))
            reply = reply.replace("#", subject);

        for (int i = 0; i < commandsList.count(); i++)
            if (commandsList.at(i).compare(cmdCode) == 0)
            {
                cmdKeyword = keywordList.at(i);
                found = true;
                break;
            }

        if (found)
        {
            if (!question.isEmpty())
            {
                questionCmdCode = cmdCode;
                isAwaiting = true;

                awaiting->setSingleShot(true);
                connect(awaiting, &QTimer::timeout, this, &OAssistantCore::stopAwaiting);
                awaiting->start(7200000);
            }

            if (currentProcessor.compare("generalProcessor") == 0)
                executeGeneralCommand(cmdCode, cmdKeyword, reply);
            else if (currentProcessor.compare("historyProcessor") == 0)
                executeHistoryCommand(cmdCode, cmdKeyword, reply);
            else if (currentProcessor.compare("bookmarkProcessor") == 0)
                executeBookmarkCommand(cmdCode, cmdKeyword, reply);
            else if (currentProcessor.compare("downloadProcessor") == 0)
                executeDownloadCommand(cmdCode, cmdKeyword, reply);
        }
        else
            addAssistantSpeech(reply);
    }

    serverResponse->deleteLater();
    serverManager->clearAccessCache();
}

void OAssistantCore::processServerSuggestion(QNetworkReply *serverResponse)
{
    QString response = serverResponse->readAll();
    QStringList dataList = response.replace("\"", "").replace("[", "").replace("]", "").split(",");
    typingText = "";
    showSuggestions(dataList);
    suggestionManager->clearAccessCache();
    serverResponse->deleteLater();
}

void OAssistantCore::showSuggestions(const QStringList commands)
{
    emit sendSuggestions(commands);
}

void OAssistantCore::addAssistantSpeech(const QString speech)
{
    emit sendAssistantSpeech(speech);
}

void OAssistantCore::executeGeneralCommand(const QString cmdCode, const QString keyword, const QString reply)
{
    if ((keyword.compare("tab") == 0) || (keyword.compare("tabs") == 0))
        executeGeneralTabCommand(cmdCode, reply);
    else if ((keyword.compare("window") == 0) || (keyword.compare("windows") == 0))
        executeGeneralWindowCommand(cmdCode, reply);
    else if (keyword.compare("Open") == 0)
        executeGeneralAppCommand(cmdCode, reply);
    else
        executeGeneralBrowserCommand(cmdCode, reply);
}

void OAssistantCore::executeGeneralTabCommand(const QString tabCmdCode, const QString reply)
{
    if (tabCmdCode.startsWith("o", Qt::CaseInsensitive))
        executeTabOpenCommand(tabCmdCode, reply);
    else if (tabCmdCode.startsWith("s", Qt::CaseInsensitive))
        executeTabSelectCommand(tabCmdCode, reply);
    else if (tabCmdCode.startsWith("c", Qt::CaseInsensitive))
        executeTabCloseCommand(tabCmdCode, reply);
}

void OAssistantCore::executeTabOpenCommand(const QString openCmdCode, const QString reply)
{
    if (openCmdCode.compare("oant") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            tabPage->openNewTab();
            addAssistantSpeech(reply);
        }
    }
    else if (openCmdCode.compare("oatf") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else if (!subject.isEmpty())
        {
           QUrl url = QUrl::fromUserInput(subject);
           if (!url.host().isEmpty() && url.host().contains("."))
                tabPage->openInTab(subject);
           else
           {
               tabPage->openInTab("");
               tabPage->getStackPage()->widget(tabPage->count()-1)->findChild<CustomToolBar*>("controlBar")->getSmartBox()->setText(subject);
               tabPage->getStackPage()->widget(tabPage->count()-1)->findChild<CustomToolBar*>("controlBar")->getSmartBox()->validateInput();
           }

           subject = "";
           addAssistantSpeech(reply);
        }
    }
}

void OAssistantCore::executeTabCloseCommand(const QString closeCmdCode, const QString reply)
{
    if (closeCmdCode.compare("cnt") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if ((tabPage->currentTabPage()+1) < tabPage->count())
            {
                tabPage->closeTabPage(tabPage->currentTabPage()+1);
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech(QLatin1String("Sorry, there is no next tab available for closing."));
        }
    }
    else if (closeCmdCode.compare("cpt") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if ((tabPage->currentTabPage()-1) > (-1))
            {
                tabPage->closeTabPage(tabPage->currentTabPage()-1);
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech(QLatin1String("Sorry, there is no previous tab available for closing."));
        }
    }
    else if (closeCmdCode.compare("cant") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            QList<int> indexes;
            for (int i = (tabPage->count()-1); tabPage->currentTabPage() < i; i--)
                indexes << i;

            bool closed = false;
            while (indexes.count() > 0)
            {
                tabPage->bruteRemoving(indexes.first());
                indexes.removeFirst();
                closed = true;
            }

            if (closed)
            {
                tabPage->getTabBar()->resizeOnRemoving();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech(QLatin1String("Sorry, there are no next tabs available for closing."));
        }
    }
    else if (closeCmdCode.compare("capt") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            QList<int> indexes;
            for (int i = 0; i < tabPage->currentTabPage(); i++)
                indexes << i;

            bool closed = false;
            while (indexes.count() > 0)
            {
                tabPage->bruteRemoving(0);
                indexes.removeFirst();
                closed = true;
            }

            if (closed)
            {
                tabPage->getTabBar()->resizeOnRemoving();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech(QLatin1String("Sorry, there are no previous tabs available for closing."));
        }
    }
    else if (closeCmdCode.compare("cot") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            QList<int> indexes;
            for (int i = (tabPage->count()-1); tabPage->currentTabPage() < i; i--)
                indexes << i;

            int countA = 0;
            while (indexes.count() > 0)
            {
                tabPage->bruteRemoving(indexes.first());
                indexes.removeFirst();
                countA++;
            }

            for (int i = 0; i < tabPage->currentTabPage(); i++)
                indexes << i;

            int countB = 0;
            while (indexes.count() > 0)
            {
                tabPage->bruteRemoving(0);
                indexes.removeFirst();
                countB++;
            }

            if ((countA > 0) || (countB > 0))
            {
                tabPage->getTabBar()->resizeOnRemoving();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech(QLatin1String("Sorry, there are no other tabs available for closing."));
        }
    }
    else if (closeCmdCode.compare("cthl") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            int countA = 0;
            while (!tabPage->getTabBar()->getTabAt(0)->isVisible())
            {
                tabPage->bruteRemoving(0);
                countA++;
            }
            if (countA > 0)
                addAssistantSpeech(reply);
            else
                addAssistantSpeech(QLatin1String("Sorry, there are no tabs hidden on the left available for closing."));
        }
    }
    else if (closeCmdCode.compare("cthr") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            int countA = 0;
            while (!tabPage->getTabBar()->getTabAt(tabPage->count()-1)->isVisible())
            {
                tabPage->bruteRemoving(tabPage->count()-1);
                countA++;
            }
            if (countA > 0)
                addAssistantSpeech(reply);
            else
                addAssistantSpeech(QLatin1String("Sorry, there are no tabs hidden on the right available for closing."));
        }
    }
}

void OAssistantCore::executeTabSelectCommand(const QString selectCmdCode, const QString reply)
{
    if (selectCmdCode.compare("snt") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if ((tabPage->currentTabPage()+1) < tabPage->count())
            {
                tabPage->getTabBar()->setCurrentTab(tabPage->currentTabPage()+1);
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech(QLatin1String("Sorry, there is no next tab available to select."));
        }
    }
    else if (selectCmdCode.compare("spt") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if ((tabPage->currentTabPage()-1) >= 0)
            {
                tabPage->getTabBar()->setCurrentTab(tabPage->currentTabPage()-1);
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech(QLatin1String("Sorry, there is no previous tab available to select."));
        }
    }
    else if (selectCmdCode.compare("slt") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (tabPage->currentTabPage() < (tabPage->count()-1))
            {
                tabPage->getTabBar()->switchToTab(tabPage->count()-1);
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech(QLatin1String("Sorry, you are already at the last tab."));
        }
    }
    else if (selectCmdCode.compare("sft") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (tabPage->currentTabPage() > 0)
            {
                tabPage->getTabBar()->switchToTab(0);
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech(QLatin1String("Sorry, you are already at the first tab."));
        }
    }
}

void OAssistantCore::executeGeneralWindowCommand(const QString windowCmdCode, const QString reply)
{
    if (windowCmdCode.startsWith("o", Qt::CaseInsensitive))
        executeWindowOpenCommand(windowCmdCode);
    else if (windowCmdCode.startsWith("s", Qt::CaseInsensitive))
        executeWindowSelectCommand(windowCmdCode);
    else if (windowCmdCode.startsWith("c", Qt::CaseInsensitive))
        executeWindowCloseCommand(windowCmdCode, reply);
}

void OAssistantCore::executeWindowOpenCommand(const QString openCmdCode)
{
    OAppCore *parentPtr = static_cast<OAppCore*>(parent());
    if (openCmdCode.compare("oanw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
            parentPtr->openBrowserWindow();

    }
    else if (openCmdCode.compare("oapw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
            parentPtr->openPrivateBrowserWindow();
    }
}

void OAssistantCore::executeWindowCloseCommand(const QString closeCmdCode, const QString reply)
{
    OAppCore *parentPtr = static_cast<OAppCore*>(parent());
    if (closeCmdCode.compare("cfw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            parentPtr->allWindowsList().at(0)->getCentralWidget()->close();
            addAssistantSpeech(reply);
        }
    }
    else if (closeCmdCode.compare("clw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            parentPtr->allWindowsList().at(parentPtr->allWindowsList().count()-1)->getCentralWidget()->close();
            addAssistantSpeech(reply);
        }
    }
    else if (closeCmdCode.compare("cow") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            int closed = 0;
            for (int i = 0; i < parentPtr->allWindowsList().count(); i++)
            {
                OTabPage *otherTabPage = static_cast<OTabPage*>(parentPtr->allWindowsList().at(i)->getCentralWidget());
                if (tabPage != otherTabPage)
                {
                    otherTabPage->close();
                    closed++;
                    i--;
                }
            }
            if (closed > 0)
                addAssistantSpeech(reply);
            else
                addAssistantSpeech(QLatin1String("Sorry, there are no other windows to close."));
        }
    }
}

void OAssistantCore::executeWindowSelectCommand(const QString selectCmdCode)
{
    OAppCore *parentPtr = static_cast<OAppCore*>(parent());
    if (selectCmdCode.compare("snw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            for (int i = 0; i < parentPtr->allWindowsList().count(); i++)
            {
                OTabPage *otherTabPage = static_cast<OTabPage*>(parentPtr->allWindowsList().at(i)->getCentralWidget());
                if (tabPage == otherTabPage)
                {
                    if ((i + 1) < parentPtr->allWindowsList().count())
                    {
                        OTabPage *nextTabPage = static_cast<OTabPage*>(parentPtr->allWindowsList().at(i+1)->getCentralWidget());
                        SendMessage((HWND)nextTabPage->winId(), WM_ACTIVATE, HTCLIENT, 0);
                        OAppCore::postEvent(nextTabPage, new QEvent(QEvent::WindowActivate));
                        break;
                    }
                    else
                        addAssistantSpeech(QLatin1String("Sorry, there is no next window available to select."));
                }
            }
        }

    }
    else if (selectCmdCode.compare("spw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            for (int i = parentPtr->allWindowsList().count()-1; i >= 0; i--)
            {
                OTabPage *otherTabPage = static_cast<OTabPage*>(parentPtr->allWindowsList().at(i)->getCentralWidget());
                if (tabPage == otherTabPage)
                {
                    if ((i - 1) >= 0)
                    {
                        OTabPage *previousTabPage = static_cast<OTabPage*>(parentPtr->allWindowsList().at(i-1)->getCentralWidget());
                        SendMessage((HWND)previousTabPage->winId(), WM_ACTIVATE, HTCLIENT, 0);
                        OAppCore::postEvent(previousTabPage, new QEvent(QEvent::WindowActivate));
                        break;
                    }
                    else
                        addAssistantSpeech(QLatin1String("Sorry, there is no previous window available to select."));
                }
            }
        }
    }
    else if (selectCmdCode.compare("sfw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            OTabPage *firstTabPage = static_cast<OTabPage*>(parentPtr->allWindowsList().at(0)->getCentralWidget());
            if (tabPage != firstTabPage)
            {
                SendMessage((HWND)firstTabPage->winId(), WM_ACTIVATE, HTCLIENT, 0);
                OAppCore::postEvent(firstTabPage, new QEvent(QEvent::WindowActivate));
            }
            else
                addAssistantSpeech(QLatin1String("Sorry, you are already at the first window."));
        }
    }
    else if (selectCmdCode.compare("slw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            OTabPage *lastTabPage = static_cast<OTabPage*>(parentPtr->allWindowsList().at(parentPtr->allWindowsList().count()-1)->getCentralWidget());
            if (tabPage != lastTabPage)
            {
                SendMessage((HWND)lastTabPage->winId(), WM_ACTIVATE, HTCLIENT, 0);
                OAppCore::postEvent(lastTabPage, new QEvent(QEvent::WindowActivate));
            }
            else
                addAssistantSpeech(QLatin1String("Sorry, you are already at the last window."));
        }
    }
}

void OAssistantCore::executeGeneralAppCommand(const QString appCmdCode, const QString reply)
{
    OAppCore *parentPtr = static_cast<OAppCore*>(parent());
    if (!question.isEmpty())
    {
        addAssistantSpeech(question);
        question = "";
    }
    else
    {
        if (appCmdCode.compare("os") == 0)
            parentPtr->openSettings();
        else if (appCmdCode.compare("ob") == 0)
            parentPtr->openBookmarks();
        else if (appCmdCode.compare("oh") == 0)
            parentPtr->openHistory();
        else if (appCmdCode.compare("od") == 0)
            parentPtr->openDownloads();

        addAssistantSpeech(reply);
    }
}

void OAssistantCore::executeGeneralBrowserCommand(const QString browserCmdCode, const QString reply)
{
    if (!question.isEmpty())
    {
        addAssistantSpeech(question);
        question = "";
    }
    else
    {
        if (browserCmdCode.compare("spifm") == 0)
        {
            tabPage->showFullscreenMode();
            addAssistantSpeech(reply);
        }
        else if (browserCmdCode.compare("ptp") == 0)
        {
            tabPage->print();
            addAssistantSpeech(reply);
        }
        else if (browserCmdCode.compare("ft") == 0)
        {
            if (!subject.isEmpty())
            {
               tabPage->findTextWith(subject);
               subject = "";
            }
            addAssistantSpeech(reply);
        }
        else if (browserCmdCode.compare("stp") == 0)
        {
            tabPage->savePage();
            addAssistantSpeech(reply);
        }
        else if (browserCmdCode.compare("spsc") == 0)
        {
            tabPage->getStackPage()->currentWidget()->findChild<OWebView*>(QLatin1String("pageView"))->showPageSource();
            addAssistantSpeech(reply);
        }
        else if (browserCmdCode.compare("dtp") == 0)
        {
            tabPage->webDev();
            addAssistantSpeech(reply);
        }
        else if (browserCmdCode.compare("ria") == 0)
        {
            tabPage->reportIssue();
            addAssistantSpeech(reply);
        }
        else if (browserCmdCode.compare("ohc") == 0)
        {
            tabPage->helpCenter();
            addAssistantSpeech(reply);
        }
        else if (browserCmdCode.compare("hbb") == 0)
        {
            OAppCore *parentPtr = static_cast<OAppCore*>(parent());
            if (tabPage->isBookmarkBarVisible())
            {
                parentPtr->getSettingsManager()->manageBookmarkBar();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, the Bookmarks bar is already hidden.");
        }
        else if (browserCmdCode.compare("sbb") == 0)
        {
            OAppCore *parentPtr = static_cast<OAppCore*>(parent());
            if (!tabPage->isBookmarkBarVisible())
            {
                parentPtr->getSettingsManager()->manageBookmarkBar();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, the Bookmarks bar is already visible.");
        }
        else if (browserCmdCode.compare("vst") == 0)
        {
            if (!subject.isEmpty())
            {
                tabPage->visitPage(subject);
                subject = "";
            }
            addAssistantSpeech(reply);
        }
        else if (browserCmdCode.compare("sch") == 0)
        {
            if (!subject.isEmpty())
            {
               tabPage->searchTheWebFor(subject);
               subject = "";
            }
            addAssistantSpeech(reply);
        }
        else if (browserCmdCode.compare("saad") == 0)
            executeDwnStopCommand(browserCmdCode, reply);
    }
}

void OAssistantCore::setTabPage(OTabPage *currentTabPage)
{
    tabPage = currentTabPage;
}

void OAssistantCore::setHistory(OHistoryCore *historyPtr)
{
    histManager = historyPtr;
}

void OAssistantCore::setBookmark(OBookmarksCore *bookmarkPtr)
{
    bmkManager = bookmarkPtr;
}

void OAssistantCore::setDownload(ODownloadsCore *downloadPtr)
{
    dwnManager = downloadPtr;
}

void OAssistantCore::executeHistoryCommand(const QString cmdCode, const QString keyword, const QString reply)
{
    if (keyword.compare("Open") == 0)
        executeHistOpenCommand(cmdCode, reply);
    else if ((keyword.compare("Delete") == 0) || (keyword.compare("Clear") == 0))
        executeHistDeleteCommand(cmdCode, reply);
}

void OAssistantCore::executeHistOpenCommand(const QString openCmdCode, const QString reply)
{
    if (openCmdCode.compare("ovpoint") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else if (!subject.isEmpty())
        {
            if (histManager->isDateExisting(subject))
            {
                histManager->openDateInNewTab(subject);
                subject = "";
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, there are no pages of date \"" + subject + "\" in the history.");
        }
    }
    else if (openCmdCode.compare("ovpoinw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else if (!subject.isEmpty())
        {
            if (histManager->isDateExisting(subject))
            {
                histManager->openDateInNewWindow(subject);
                subject = "";
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, there are no pages of date \"" + subject + "\" in the history.");
        }
    }
    else if (openCmdCode.compare("ovpoipw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else if (!subject.isEmpty())
        {
            if (histManager->isDateExisting(subject))
            {
                histManager->openDateInPrivateWindow(subject);
                subject = "";
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, there are no pages of date \"" + subject + "\" in the history.");
        }
    }
    else if (openCmdCode.compare("oahint") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (!histManager->getHistoryItemsList().empty())
            {
                histManager->openHistoryInNewTab();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, the history manager is empty.");
        }
    }
    else if (openCmdCode.compare("oahinw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (!histManager->getHistoryItemsList().empty())
            {
                histManager->openHistoryInNewWindow();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, the history manager is empty.");
        }
    }
    else if (openCmdCode.compare("oahipw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (!histManager->getHistoryItemsList().empty())
            {
                histManager->openHistoryInPrivateWindow();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, the history manager is empty.");
        }
    }
    else if (openCmdCode.compare("ooopint") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (histManager->isItemExisting(subject))
            {
                histManager->openOccurenceInNewTab(subject);
                subject = "";
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, there are no occurences of \"" + subject + "\" in the history.");
        }
    }
    else if (openCmdCode.compare("ooopinw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (histManager->isItemExisting(subject))
            {
                histManager->openOccurenceInNewWindow(subject);
                subject = "";
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, there are no occurences of \"" + subject + "\" in the history.");
        }
    }
    else if (openCmdCode.compare("ooopipw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (histManager->isItemExisting(subject))
            {
                histManager->openOccurenceInPrivateWindow(subject);
                subject = "";
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, there are no occurences of \"" + subject + "\" in the history.");
        }
    }
}

void OAssistantCore::executeHistDeleteCommand(const QString deleteCmdCode, const QString reply)
{
    if (deleteCmdCode.compare("daoo") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (histManager->isItemExisting(subject))
            {
                histManager->deleteByOccurence(subject);
                subject = "";
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, there are no occurences of \"" + subject + "\" in the history.");
        }
    }
    else if (deleteCmdCode.compare("dvpo") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (histManager->isDateExisting(subject))
            {
                histManager->deleteByDate(subject);
                subject = "";
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, there are no pages of date \"" + subject + "\" in the history.");
        }
    }
    else if (deleteCmdCode.compare("ch") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (!histManager->getHistoryItemsList().isEmpty())
            {
                histManager->clearHistory();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, the history manager is already empty.");
        }
    }
}

void OAssistantCore::executeBookmarkCommand(const QString cmdCode, const QString keyword, const QString reply)
{
    if (keyword.compare("Open") == 0)
        executeBmkOpenCommand(cmdCode, reply);
    else if ((keyword.compare("Delete") == 0) || (keyword.compare("Clear") == 0))
        executeBmkDeleteCommand(cmdCode, reply);
}

void OAssistantCore::executeBmkOpenCommand(const QString openCmdCode, const QString reply)
{
    if (openCmdCode.compare("oabint") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (!bmkManager->getBookmarkList().isEmpty())
            {
                bmkManager->openAllInNewTab();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, the bookmark manager is empty.");
        }
    }
    else if (openCmdCode.compare("oabinw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (!bmkManager->getBookmarkList().isEmpty())
            {
                bmkManager->openAllInNewWiwndow();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, the bookmark manager is empty.");
        }
    }
    else if (openCmdCode.compare("oabipw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (!bmkManager->getBookmarkList().isEmpty())
            {
                bmkManager->openAllInPrivateWindow();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, the bookmark manager is empty.");
        }
    }
    else if (openCmdCode.compare("obooint") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (bmkManager->isItemExisting(subject))
            {
                bmkManager->openOccurenceInNewTab(subject);
                subject = "";
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("There are no occurences of \"" + subject + "\" in the bookmarks.");
        }
    }
    else if (openCmdCode.compare("obooinw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (bmkManager->isItemExisting(subject))
            {
                bmkManager->openOccurenceInNewWindow(subject);
                subject = "";
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("There are no occurences of \"" + subject + "\" in the bookmarks.");
        }
    }
    else if (openCmdCode.compare("obooipw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (bmkManager->isItemExisting(subject))
            {
                bmkManager->openOccurenceInPrivateWindow(subject);
                subject = "";
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, there are no occurences of \"" + subject + "\" in the bookmarks.");
        }
    }
    else if (openCmdCode.compare("ofint") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (bmkManager->isFolderExisting(subject))
            {
                if (!bmkManager->isFolderEmpty(subject))
                {
                    bmkManager->openFolderInNewTab(subject);
                    subject = "";
                    addAssistantSpeech(reply);
                }
                else
                    addAssistantSpeech("Sorry the folder \"" + subject + "\" is empty.");
            }
            else
                addAssistantSpeech("Sorry there is no folder \"" + subject + "\" in the bookmarks manager.");
        }
    }
    else if (openCmdCode.compare("ofinw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (bmkManager->isFolderExisting(subject))
            {
                if (!bmkManager->isFolderEmpty(subject))
                {
                    bmkManager->openFolderInNewWindow(subject);
                    subject = "";
                    addAssistantSpeech(reply);
                }
                else
                    addAssistantSpeech("Sorry the folder \"" + subject + "\" is empty.");
            }
            else
                addAssistantSpeech("Sorry there is no folder \"" + subject + "\" in the bookmarks manager.");
        }
    }
    else if (openCmdCode.compare("ofipw") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (bmkManager->isFolderExisting(subject))
            {
                if (!bmkManager->isFolderEmpty(subject))
                {
                    bmkManager->openFolderInPrivate(subject);
                    subject = "";
                    addAssistantSpeech(reply);
                }
                else
                    addAssistantSpeech("Sorry the folder \"" + subject + "\" is empty.");
            }
            else
                addAssistantSpeech("Sorry there is no folder \"" + subject + "\" in the bookmarks manager.");
        }
    }
}

void OAssistantCore::executeBmkDeleteCommand(const QString deleteCmdCode, const QString reply)
{
    if (deleteCmdCode.compare("daboo") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (bmkManager->isItemExisting(subject))
            {
                bmkManager->deleteBmkOccurence(subject);
                subject = "";
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, there are no occurences of \"" + subject + "\" in the bookmarks.");
        }
    }
    else if (deleteCmdCode.compare("cb") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (!bmkManager->getBookmarkList().isEmpty())
            {
                bmkManager->clearBookmark();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, the bookmark manager is already empty.");
        }
    }
    else if (deleteCmdCode.compare("cf") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (bmkManager->isFolderExisting(subject))
            {
                if (!bmkManager->isFolderEmpty(subject))
                {
                    bmkManager->clearBmkFolder(subject);
                    subject = "";
                    addAssistantSpeech(reply);
                }
                else
                    addAssistantSpeech("Sorry the folder \"" + subject + "\" is empty.");
            }
            else
                addAssistantSpeech("Sorry there is no folder \"" + subject + "\" in the bookmarks manager.");
        }
    }
}

void OAssistantCore::executeDownloadCommand(const QString cmdCode, const QString keyword, const QString reply)
{
    if (keyword.compare("Pause") == 0)
        executeDwnPauseCommand(cmdCode, reply);
    else if (keyword.compare("Resume") == 0)
        executeDwnResumeCommand(cmdCode, reply);
    else if (keyword.compare("Restart") == 0)
        executeDwnRestartCommand(cmdCode, reply);
    else if (keyword.compare("Stop") == 0)
        executeDwnStopCommand(cmdCode, reply);
    else if ((keyword.compare("Delete") == 0) || (keyword.compare("Clear") == 0) || (keyword.compare("Open") == 0))
        executeDwnOtherCommand(cmdCode, reply);
}

void OAssistantCore::executeDwnPauseCommand(const QString pauseCmdCode, const QString reply)
{
    if (pauseCmdCode.compare("paad") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (dwnManager->hasActiveDownload())
            {
                dwnManager->pauseAllDownloads();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, there are no active downloads to be paused.");
        }
    }
    else if (pauseCmdCode.compare("pado") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (dwnManager->isDateExisting(subject))
            {
                if (dwnManager->hasActiveDownload())
                {
                    dwnManager->pauseDownloadByDate(subject);
                    subject = "";
                    addAssistantSpeech(reply);
                }
                else
                    addAssistantSpeech("Sorry, there are no active downloads from \""+ subject +"\"to be paused.");
            }
            else
                addAssistantSpeech("Sorry, there are no downloads from \"" + subject + "\" in the download manager.");
        }
    }
}

void OAssistantCore::executeDwnResumeCommand(const QString resumeCmdCode, const QString reply)
{
    if (resumeCmdCode.compare("rapd") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (dwnManager->hasPausedDownload())
            {
                dwnManager->resumeAllDownloads();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, there are no paused downloads to be resume.");
        }
    }
    else if (resumeCmdCode.compare("rpdo") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (dwnManager->isDateExisting(subject))
            {
                if (dwnManager->hasPausedDownload())
                {
                    dwnManager->resumeByDate(subject);
                    subject = "";
                    addAssistantSpeech(reply);
                }
                else
                    addAssistantSpeech("Sorry, there are no paused downloads from \""+ subject +"\"to be resume.");
            }
            else
                addAssistantSpeech("Sorry, there are no downloads from \"" + subject + "\" in the download manager.");
        }
    }
}

void OAssistantCore::executeDwnRestartCommand(const QString restartCmdCode, const QString reply)
{
    if (restartCmdCode.compare("rafd") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (dwnManager->hasFailedDownload())
            {
                dwnManager->restartAllDownloads();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, there are no failed downloads to be restarted.");
        }
    }
    else if (restartCmdCode.compare("rfdo") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (dwnManager->isDateExisting(subject))
            {
                if (dwnManager->hasFailedDownload())
                {
                    dwnManager->restartByDate(subject);
                    subject = "";
                    addAssistantSpeech(reply);
                }
                else
                    addAssistantSpeech("Sorry, there are no failed downloads to be restarted.");
            }
            else
                addAssistantSpeech("Sorry, there are no downloads from \"" + subject + "\" in the download manager.");
        }
    }
}

void OAssistantCore::executeDwnStopCommand(const QString stopCmdCode, const QString reply)
{
    if (stopCmdCode.compare("sado") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (dwnManager->isDateExisting(subject))
            {
                if (dwnManager->hasActiveDownload())
                {
                    dwnManager->stopByDate(subject);
                    subject = "";
                    addAssistantSpeech(reply);
                }
                else
                    addAssistantSpeech("Sorry, there are no active downloads to be stopped.");
            }
            else
                addAssistantSpeech("Sorry, there are no downloads from \"" + subject + "\" in the download manager.");
        }
    }
    else if (stopCmdCode.compare("saad") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (dwnManager->hasActiveDownload())
            {
                dwnManager->stopAllDownloads();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, there are no active downloads to be stopped.");
        }
    }
}

void OAssistantCore::executeDwnOtherCommand(const QString otherCmdCode, const QString reply)
{
    if (otherCmdCode.compare("ddo") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (dwnManager->isDateExisting(subject))
            {
                if (!dwnManager->getDownloadItemList().isEmpty())
                {
                    dwnManager->deleteByDate(subject);
                    subject = "";
                    addAssistantSpeech(reply);
                }
                else
                    addAssistantSpeech("Sorry, the downloads manager is empty.");
            }
            else
                addAssistantSpeech("Sorry, there are no downloads from \"" + subject + "\" in the download manager.");
        }
    }
    else if (otherCmdCode.compare("cdl") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            if (!dwnManager->getDownloadItemList().isEmpty())
            {
                dwnManager->deleteAllDownloads();
                addAssistantSpeech(reply);
            }
            else
                addAssistantSpeech("Sorry, the downloads manager is empty.");
        }
    }
    else if (otherCmdCode.compare("odf") == 0)
    {
        if (!question.isEmpty())
        {
            addAssistantSpeech(question);
            question = "";
        }
        else
        {
            dwnManager->openDownloadsFolder();
            addAssistantSpeech(reply);
        }
    }
}

void OAssistantCore::stopAwaiting()
{
    disconnect(awaiting, &QTimer::timeout, this, &OAssistantCore::stopAwaiting);
    awaiting->stop();
    awaiting->setSingleShot(false);
    isAwaiting = false;
}
