#include "osettingscore.h"
#include "oappcore.h"
#include "ohistorycore.h"
#include "ocertificatemanager.h"
#include "otabpage.h"
#include <QWebEngineCookieStore>
#include <QSqlError>
#include <QFileDialog>
#include <winrt/windows.foundation.h>//ignore warning about co_await
#include <winrt/windows.system.h>
#include <Windows.h>
#include <tchar.h>
#include <intrin.h>
#include <QSysInfo>
#include <QFileInfo>
#include <QDir>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QProcess>
#include <QTimer>
#include <QSettings>
#include <QEventLoop>
#include <QHostInfo>
#include <QNetworkInterface>
#include <climits>
#include <QStandardPaths>

OSettingsCore::OSettingsCore(QObject *parent)
    :QObject(parent)
{
    QDir appDirectory(OAppCore::applicationDirPath());
    if (!appDirectory.exists(QLatin1String("ololanPrivateWebStorage")))
        appDirectory.mkdir(QLatin1String("ololanPrivateWebStorage"));

    if (!appDirectory.exists(QLatin1String("ololanWebStorage")))
        appDirectory.mkdir(QLatin1String("ololanWebStorage"));

    if (appDirectory.cd("ololanWebStorage"))
        appDirectory.mkdir(QLatin1String("Cache"));

    if (appDirectory.cd("Cache"))
        appDirectory.mkdir(QLatin1String("Cache_Data"));

    appDirectory.cd("../../");
    if (appDirectory.exists("ololanCache"))
        appDirectory.rmdir("ololanCache");

    if (appDirectory.exists("updates") && appDirectory.cd("updates"))
        appDirectory.removeRecursively();

    isOpened = false;
    aboutResquested = false;
    checkedForUpdates = false;
    updatesAvailable = false;
    webChannel = new QWebChannel(this);
    webChannel->registerObject("settingsManager", this);

    normalWebProfile = new QWebEngineProfile("normalBrowsing", this);
    normalWebProfile->setHttpUserAgent("Mozilla/5.0 (Windows NT 6.2; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Ololan/3.1.673.33 Chrome/118.0.5993.220 Safari/537.36");
    normalWebProfile->setHttpAcceptLanguage("EN");
    normalWebProfile->setHttpCacheType(QWebEngineProfile::DiskHttpCache);
    normalWebProfile->setHttpCacheMaximumSize(INT_MAX);
    normalWebSettings = normalWebProfile->settings();
    normalWebSettings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    normalWebProfile->setPersistentStoragePath(OAppCore::applicationDirPath().append("/ololanWebStorage"));
    normalWebProfile->setCachePath(OAppCore::applicationDirPath().append("/ololanWebStorage/Cache"));

    privateWebProfile = new QWebEngineProfile("privateBrowsing", this);
    privateWebProfile->setHttpUserAgent("Mozilla/5.0 (Windows NT 6.2; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Ololan/3.1.673.33 Chrome/118.0.5993.220 Safari/537.36");
    privateWebProfile->setHttpAcceptLanguage("EN");
    privateWebProfile->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);
    privateWebProfile->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, false);
    privateWebProfile->settings()->setAttribute(QWebEngineSettings::AllowGeolocationOnInsecureOrigins, false);
    privateWebProfile->settings()->setAttribute(QWebEngineSettings::JavascriptCanPaste, false);
    privateWebProfile->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, false);
    privateWebProfile->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, false);
    privateWebProfile->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    privateWebProfile->setPersistentStoragePath(OAppCore::applicationDirPath().append("/ololanPrivateWebStorage"));

    setupIndexList();
    serverManager = new QNetworkAccessManager(this);
    connect(serverManager, &QNetworkAccessManager::finished, this, &OSettingsCore::processServerResponse);
    connect(normalWebProfile->cookieStore(), &QWebEngineCookieStore::cookieAdded, this, &OSettingsCore::fillCookiesList);
    normalWebProfile->cookieStore()->loadAllCookies();

    QString databaseName = OAppCore::applicationDirPath().append("/browserDBDir/ololansettingsdatabase.gdb");
    settingsDatabase = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), QLatin1String("ololanSettingsDatabase"));
    settingsDatabase.setDatabaseName(databaseName);
    settingsDatabase.setHostName(QLatin1String("ololanhost"));
    settingsDatabase.setUserName(QLatin1String("ololanRoot"));
    settingsDatabase.setPassword(QLatin1String("OlolanBrowserDB2022"));

    if (settingsDatabase.open(QLatin1String("ololanRoot"), QLatin1String("OlolanBrowserDB2022")))
    {
        query = QSqlQuery(settingsDatabase);
        setupAccountSettings();
        setupAppearanceSettings();
        setupStartupSettings();
        setupPrivacySecuritySettings();
        setupPersonalDataSettings();
        setupBrowsingDataSettings();
        setupSearchEngineSettings();
        setupLanguageSettings();
        setupDownloadSettings();
        setupAssistantSettings();
        setupDefaultBrowserSettings();
        setupSystemSettings();
        checkDefaultBrowserState();
    }
    else
        qDebug(settingsDatabase.lastError().text().toLatin1());
}

void OSettingsCore::processServerResponse(QNetworkReply *serverResponse)
{
    if (serverResponse->error() != QNetworkReply::NoError)
    {
        QString answer = "Can't check for updates right now. Try again later";
        QString responseScript = QLatin1String("showUpdateCheckingInfo('%1', '%2');").arg(answer, "false");
        settingsWebView->page()->runJavaScript(responseScript);
    }
    else
    {
        QJsonObject response = QJsonDocument::fromJson(serverResponse->readAll()).object();
        QString browserVersion = response["browser version"].toString();
        QString chatbotVersion = response["chatbot version"].toString();
        QString frameworkVersion = response["framework version"].toString();
        QString buildVersion = response["build version"].toString();

        if ((browserVersion.toInt() != 3) || (chatbotVersion.toInt() != 1) || (frameworkVersion.toInt() != 673) || (buildVersion.toInt() != 33))
        {
            QString answer = "New version available. Click to download";
            QString responseScript = QLatin1String("showUpdateCheckingInfo('%1', '%2');").arg(answer, "true");
            settingsWebView->page()->runJavaScript(responseScript);
        }
        else
        {
            QString answer = "Ololan is up to date";
            QString responseScript = QLatin1String("showUpdateCheckingInfo('%1', '%2');").arg(answer, "false");
            settingsWebView->page()->runJavaScript(responseScript);
        }
    }
}

void OSettingsCore::checkForUpdates()
{
    disconnect(normalWebProfile->cookieStore(), &QWebEngineCookieStore::cookieAdded, this, &OSettingsCore::fillCookiesList);
    if (!checkedForUpdates)
    {
        QNetworkAccessManager manager;
        QNetworkRequest request(QUrl(""));
        request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.2; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Ololan/3.1.673.33 Chrome/118.0.5993.220 Safari/537.36");
        QNetworkReply* reply = manager.get(request);
        QEventLoop eventLoop;
        QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        eventLoop.exec();

        if (reply->error() == QNetworkReply::NoError)
        {
            {
                QString userIDBrowser = (QSysInfo::prettyProductName() + " " + QSysInfo::machineUniqueId() + " ");
                MEMORYSTATUSEX pcRam;
                pcRam.dwLength = sizeof(pcRam);
                userIDBrowser += (GlobalMemoryStatusEx(&pcRam) ? (QString::number(pcRam.ullTotalPhys/(1024*1024*1024)) + "GB") : "PcRamUnknown");

                QNetworkRequest IDRequest(QUrl());
                IDRequest.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.2; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Ololan/3.1.673.33 Chrome/118.0.5993.220 Safari/537.36");
                QNetworkAccessManager managerUser;
                QNetworkReply* replyUser = managerUser.get(IDRequest);
                QEventLoop eventLoopUser;
                QObject::connect(replyUser, &QNetworkReply::finished, &eventLoopUser, &QEventLoop::quit);
                eventLoopUser.exec();
            }

            checkedForUpdates = true;
            QJsonObject response = QJsonDocument::fromJson(reply->readAll()).object();
            QString browserVersion = response["browser version"].toString();
            QString chatbotVersion = response["chatbot version"].toString();
            QString frameworkVersion = response["framework version"].toString();
            QString buildVersion = response["build version"].toString();

            if ((browserVersion.toInt() != 3) || (chatbotVersion.toInt() != 1) || (frameworkVersion.toInt() != 673) || (buildVersion.toInt() != 33))
            {
                updatesAvailable = true;
                OAppCore *parentApp = static_cast<OAppCore*>(parent());
                parentApp->warnForUpdates();
            }
        }
        reply->deleteLater();
    }
}

void OSettingsCore::downloadAvailableUpdates()
{
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl(""));
    QNetworkReply* reply = manager.get(request);
    QObject::connect(reply, &QNetworkReply::downloadProgress, this, [=, this](qint64 received, qint64 total) {
        if (isOpened)
        {
            const int progress = (int)(received*100/total);
            QString responseScript = QLatin1String("showUpdateProgress('%1');").arg("downloading " + QString::number(progress) + "%");
            settingsWebView->page()->runJavaScript(responseScript);
        }
    });
    QEventLoop eventLoop;
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    if (reply->error() == QNetworkReply::NoError)
    {
        QDir appDirectory(OAppCore::applicationDirPath());
        if (!appDirectory.exists(QLatin1String("updates")))
            appDirectory.mkdir(QLatin1String("updates"));

        QFile updateFile(OAppCore::applicationDirPath() + "/updates" + "/browserUpdates.exe");
        if (updateFile.open(QIODevice::WriteOnly))
        {
            updateFile.write(reply->readAll());
            updateFile.close();

            QProcess updateProcess;
            updateProcess.setWorkingDirectory(appDirectory.absolutePath() + "/updates");
            updateProcess.start(appDirectory.absolutePath() + "/updates" + "/browserUpdates.exe");
            updateProcess.waitForFinished(1000);

            QTimer::singleShot(1000, this, [=, this](){
                OAppCore *parentApp = static_cast<OAppCore*>(parent());
                parentApp->exitFromBrowser();
            });
        }
        else
        {
            QString answer = "Error while installing updates. Try again later";
            QString responseScript = QLatin1String("showUpdateCheckingInfo('%1', '%2');").arg(answer, "true");
            settingsWebView->page()->runJavaScript(responseScript);
        }
    }
    else
    {
        QString answer = "Error while downloading updates. Try again later";
        QString responseScript = QLatin1String("showUpdateCheckingInfo('%1', '%2');").arg(answer, "true");
        settingsWebView->page()->runJavaScript(responseScript);
    }

    reply->deleteLater();
}

void OSettingsCore::updateOlolan()
{
    QDir appDirectory(OAppCore::applicationDirPath());
    if (appDirectory.exists(QLatin1String("updates")) && QFile::exists(OAppCore::applicationDirPath() + "/updates" + "/browserUpdates.exe"))
    {
        QProcess updateProcess;
        updateProcess.setWorkingDirectory(appDirectory.absolutePath() + "/updates");
        updateProcess.start(appDirectory.absolutePath() + "/updates" + "/browserUpdates.exe");
        updateProcess.waitForFinished(1000);

        QTimer::singleShot(1000, this, [=, this]() {
            OAppCore *parentApp = static_cast<OAppCore*>(parent());
            parentApp->exitFromBrowser();
        });
    }
    else
    {
        QNetworkRequest request;
        request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.2; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Ololan/3.1.673.33 Chrome/118.0.5993.220 Safari/537.36");
        QString url = QLatin1String("");
        request.setUrl(QUrl(url));
        serverManager->get(request);
    }
}

void OSettingsCore::loadSettings(bool status)
{
    if (!status)
        return;
    else
    {
        settingsWebView = qobject_cast<OWebView*>(sender());
        QString settingsUrl = QLatin1String("qrc:/web_applications/html/settingsView.html");
        if (settingsWebView->url().toString().compare(settingsUrl) == 0)
        {
            isOpened = true;
            settingsWebView->page()->setWebChannel(webChannel);
            QString initScript = QLatin1String("setupConnector();");
            settingsWebView->page()->runJavaScript(initScript);

            if (settingsDatabase.open(QLatin1String("ololanRoot"), QLatin1String("OlolanBrowserDB2022")))
            {
                updateBrowsingData();
                setupAccountSettings();
                setupAppearanceSettings();
                setupStartupSettings();
                setupPrivacySecuritySettings();
                setupPersonalDataSettings();
                setupBrowsingDataSettings();
                setupSearchEngineSettings();
                setupLanguageSettings();
                setupDownloadSettings();
                setupAssistantSettings();
                setupDefaultBrowserSettings();
                setupSystemSettings();
            }

            if (aboutResquested)
                selectAboutOlolan();
            else
                settingsWebView->page()->runJavaScript(QLatin1String("setOptionStyle('Ololan account');"));
        }
        else
        {
            disconnect(settingsWebView, &OWebView::loadFinished, this, &OSettingsCore::loadSettings);
            OAppCore *parentApp = static_cast<OAppCore*>(parent());
            parentApp->settingsClosed();
            isOpened = false;
        }
    }
}

void OSettingsCore::setCustomFontSizeValue()
{
    if (isOpened)
    {
        int fontSize = normalWebSettings->fontSize(QWebEngineSettings::DefaultFontSize);
        QString script = QLatin1String("setCustomFontSizeValue('%1');").arg(QString::number(fontSize));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setMinimumFontSizeValue()
{
    if (isOpened)
    {
        int fontSize = normalWebSettings->fontSize(QWebEngineSettings::MinimumFontSize);
        QString script = QLatin1String("setMinimumFontSizeValue('%1');").arg(QString::number(fontSize));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setSyncState(QString state, QString username)
{
    if (isOpened)
    {
        QString script = QLatin1String("setSyncState('%1', '%2');").arg(state, username);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setSelectedThemeColor(QString themeColor)
{
    if (isOpened)
    {
        QString script = QLatin1String("setSelectedThemeColor('%1');").arg(themeColor);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setBookmarkBarState(bool state)
{
    if (isOpened)
    {
        QString script = QLatin1String("setBookmarkBarState(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setPageZoom(QString zoomValue)
{
    if (isOpened)
    {
        QString script = QLatin1String("setPageZoom('%1');").arg(zoomValue);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setSelectedFontSize(int fontValue)
{
    if (isOpened)
    {
        QString fontSize = "custom";
        if (fontValue == 24)
            fontSize = "Huge";
        else if (fontValue == 20)
            fontSize = "Large";
        else if (fontValue == 16)
            fontSize = "Medium (default size)";
        else if (fontValue == 12)
            fontSize = "Small";
        else if (fontValue == 9)
            fontSize = "Tiny";
        else
            fontSize = "Custom";

        QString script = QLatin1String("setSelectedFontSize('%1');").arg(fontSize);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setSelectedStandardFont()
{
    if (isOpened)
    {
        QString standardFontValue = normalWebSettings->fontFamily(QWebEngineSettings::StandardFont);
        QString script = QLatin1String("setSelectedStandardFont('%1');").arg(standardFontValue);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setSelectedSerifFont()
{
    if (isOpened)
    {
        QString serifFontValue = normalWebSettings->fontFamily(QWebEngineSettings::SerifFont);
        QString script = QLatin1String("setSelectedSerifFont('%1');").arg(serifFontValue);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setSelectedSansSerifFont()
{
    if (isOpened)
    {
        QString sansSerifFontValue = normalWebSettings->fontFamily(QWebEngineSettings::SansSerifFont);
        QString script = QLatin1String("setSelectedSansSerifFont('%1');").arg(sansSerifFontValue);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setSelectedFixedFont()
{
    if (isOpened)
    {
        QString fixedFontValue = normalWebSettings->fontFamily(QWebEngineSettings::FixedFont);
        QString script = QLatin1String("setSelectedFixedFont('%1');").arg(fixedFontValue);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setSelectedStartPageOption(QString startOption)
{
    if (isOpened)
    {
        QString script = QLatin1String("setSelectedStartPageOption('%1');").arg(startOption);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setAllowJavascript()
{
    if (isOpened)
    {
        bool state = normalWebSettings->testAttribute(QWebEngineSettings::JavascriptEnabled);
        QString script = QLatin1String("setAllowJavascript(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setAllowJSCopyPaste()
{
    if (isOpened)
    {
        bool state = normalWebSettings->testAttribute(QWebEngineSettings::JavascriptCanAccessClipboard);
        QString script = QLatin1String("setAllowJSCopyPaste(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setAllowJSPopUps()
{
    if (isOpened)
    {
        bool state = normalWebSettings->testAttribute(QWebEngineSettings::JavascriptCanOpenWindows);
        QString script = QLatin1String("setAllowJSPopUps(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setEnablePlugins()
{
    if (isOpened)
    {
        bool state = normalWebSettings->testAttribute(QWebEngineSettings::PluginsEnabled);
        QString script = QLatin1String("setEnablePlugins(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setAllowInsecureContents()
{
    if (isOpened)
    {
        bool state = normalWebSettings->testAttribute(QWebEngineSettings::AllowRunningInsecureContent);
        QString script = QLatin1String("setAllowInsecureContents(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setEnableInsecureGeolocation()
{
    if (isOpened)
    {
        bool state = normalWebSettings->testAttribute(QWebEngineSettings::AllowGeolocationOnInsecureOrigins);
        QString script = QLatin1String("setEnableInsecureGeolocation(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setEnableGeolocation(bool state)
{
    if (isOpened)
    {
        QString script = QLatin1String("setEnableGeolocation(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setEnableDnsPrefetch()
{
    if (isOpened)
    {
        bool state = normalWebSettings->testAttribute(QWebEngineSettings::DnsPrefetchEnabled);
        QString script = QLatin1String("setEnableDnsPrefetch(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setEnableInternalPDFViewer()
{
    if (isOpened)
    {
        bool state = normalWebSettings->testAttribute(QWebEngineSettings::PdfViewerEnabled);
        QString script = QLatin1String("setEnableInternalPDFViewer(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setSelectedWebProtection(QString protection)
{
    if (protection.compare("standardProtection") == 0)
        enableStandardProtection();
    else if (protection.compare("strongProtection") == 0)
        enableStrongProtection();
    else if (protection.compare("noneProtection") == 0)
        disableWebProtection();

    if (isOpened)
    {
        QString script = QLatin1String("setSelectedWebProtection('%1');").arg(protection);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setAskToSavePassword(bool state)
{
    if (isOpened)
    {
        QString script = QLatin1String("setAskToSavePassword(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setWebsiteAutoSignIn(bool state)
{
    if (isOpened)
    {
        QString script = QLatin1String("setWebsiteAutoSignIn(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setAutoSavePaymentMethods(bool state)
{
    if (isOpened)
    {
        QString script = QLatin1String("setAutoSavePaymentMethods(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setSelectedCookiesPolicy(QString policy = "enableCookies")
{
    if (policy.compare("enableCookies") == 0)
    {
        normalWebProfile->cookieStore()->setCookieFilter(
            [](const QWebEngineCookieStore::FilterRequest &request)
            { return true; }
            );
    }
    else if (policy.compare("privateBrowsingCookies") == 0)
    {
        normalWebProfile->cookieStore()->setCookieFilter(
            [](const QWebEngineCookieStore::FilterRequest &request)
            { return !request.thirdParty; }
        );
    }
    else if (policy.compare("noneCookies") == 0)
    {
        normalWebProfile->cookieStore()->setCookieFilter(
            [](const QWebEngineCookieStore::FilterRequest &request)
            { return false; }
        );
    }

    loadCookieData();
    if (isOpened)
    {
        QString script = QLatin1String("setSelectedCookiesPolicy('%1');").arg(policy);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setDeleteCookiesOnClose(bool state)
{
    if (state)
        normalWebProfile->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    else
        normalWebProfile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);

    if (isOpened)
    {
        QString script = QLatin1String("setDeleteCookiesOnClose(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setEnableDoNotTrack(bool state)
{
    if (isOpened)
    {
        QString script = QLatin1String("setEnableDoNotTrack(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setSelectedDefaultSearchEngine(QString defaultSearchEngine)
{
    if (isOpened)
    {
        QString script = QLatin1String("setSelectedDefaultSearchEngine('%1');").arg(defaultSearchEngine);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setSelectedUILanguage(QString uiLanguageValue)
{
    if (isOpened)
    {
        QString script = QLatin1String("setSelectedUILanguage('%1');").arg(uiLanguageValue);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setSelectedSpellCheckLanguage(QString spellCheckLanguageValue)
{
    if (isOpened)
    {
        QString script = QLatin1String("setSelectedSpellCheckLanguage('%1');").arg(spellCheckLanguageValue);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setDownloadLocation()
{
    if (isOpened)
    {
        QString path = normalWebProfile->downloadPath();
        QString script = QLatin1String("setDownloadLocation('%1');").arg(path);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setAskDownloadLocation(bool state)
{
    if (isOpened)
    {
        QString script = QLatin1String("setAskDownloadLocation(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setEnableOlolanAssistant(bool state)
{
    if (isOpened)
    {
        QString script = QLatin1String("setEnableOlolanAssistant(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setDefaultBrowserState(bool state)
{
    if (isOpened)
    {
        QString script = QLatin1String("setDefaultBrowserState(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setUseCanvasAcceleration()
{
    if (isOpened)
    {
        bool state = normalWebSettings->testAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled);
        QString script = QLatin1String("setUseCanvasAcceleration(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setUseWebGLAcceleration()
{
    if (isOpened)
    {
        bool state = normalWebSettings->testAttribute(QWebEngineSettings::WebGLEnabled);
        QString script = QLatin1String("setUseWebGLAcceleration(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setBrowsingDataSize(int cookieAndData, int cachedData, int browsingHistory, int passwordData, int paymentData)
{
    if (isOpened)
    {
        const QString cookie = getDataSize(cookieAndData);
        const QString cache = getDataSize(cachedData);
        const QString history = getDataSize(browsingHistory);
        const QString password = getDataSize(passwordData);
        const QString payment = getDataSize(paymentData);
        QString script = QLatin1String("setBrowsingDataSize('%1', '%2', '%3', '%4', '%5');").arg(cookie, cache, history, password, payment);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::loadSpecificPage(int index, QString title, QString url)
{
    if (isOpened)
    {
        QString script = QLatin1String("loadSpecificPage('%1', '%2', '%3', %4);").arg(QString::number(index), title, url, "false");
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::loadCredentials(int index, QByteArray icon, QString webAddress, QString loginAddress, QString password)
{
    if (isOpened)
    {
        QStringList credential;
        credential << QString::number(index) << QString(icon) << webAddress << loginAddress << password;
        credentialList.append(credential);
        credentialUsedIndexList.removeOne(index);
        QString script = QLatin1String("loadCredentials('%1', '%2', '%3', '%4', '%5');").arg(QString::number(index), icon, QUrl(webAddress).host(), loginAddress, password);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::loadPaymentMethods(int index, QString cardName, QString cardNumber, QString cardExpiration)
{
    if (isOpened)
    {
        QStringList paymentMethod;
        paymentMethod << QString::number(index) << cardName << cardNumber << cardExpiration;
        paymentMethodList.append(paymentMethod);
        paymentMethodUsedIndexList.removeOne(index);
        QString script = QLatin1String("loadPaymentMethods('%1', '%2', '%3', '%4');").arg(QString::number(index), cardName, cardNumber, cardExpiration);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::setCustomFontSize(int size)
{
    normalWebSettings->setFontSize(QWebEngineSettings::DefaultFontSize, size);
    normalWebSettings->setFontSize(QWebEngineSettings::DefaultFixedFontSize, size-3);
    privateWebProfile->settings()->setFontSize(QWebEngineSettings::DefaultFontSize, size);
    privateWebProfile->settings()->setFontSize(QWebEngineSettings::DefaultFixedFontSize, size-3);
    setSelectedFontSize(size);

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE appearancesettings SET customfontsize = :customfontsize");
        query.prepare(statement);
        query.bindValue(":customfontsize", size);

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::setMinimumFontSize(int size)
{
    normalWebSettings->setFontSize(QWebEngineSettings::MinimumFontSize, size);
    normalWebSettings->setFontSize(QWebEngineSettings::MinimumLogicalFontSize, size-6);
    privateWebProfile->settings()->setFontSize(QWebEngineSettings::MinimumFontSize, size);
    privateWebProfile->settings()->setFontSize(QWebEngineSettings::MinimumLogicalFontSize, size-6);

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE appearancesettings SET minimumfontsize = :minimumfontsize");
        query.prepare(statement);
        query.bindValue(":minimumfontsize", size);

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::deleteAccount()
{
    const QString email = getEmail();

    if (email.compare("guestuser@ololan.com") != 0)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->deleteAccount();
    }
}

void OSettingsCore::synchronize(QString syncState)
{
    if (syncState.compare("Sign in") == 0)
    {
        synchronized = true;
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->synchronizeOlolan();
    }
    else if (syncState.compare("Sign out") == 0)
    {
        synchronized = false;
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->disconnectOlolan();
    }
}

void OSettingsCore::selectTheme(QString themeColor)
{
    OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
    parentPtr->setOlolanTheme(themeColor);

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE appearancesettings SET themecolor = :themecolor");
        query.prepare(statement);
        query.bindValue(":themecolor", themeColor.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::showBookmarkBar(QString state)
{
    if (state.compare("true") == 0)
        bookmarkBarVisibile = true;
    else
        bookmarkBarVisibile = false;

    OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
    parentPtr->setBookmarkBarVisibile(bookmarkBarVisibile);

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE appearancesettings SET bookmarkbarstate = :bookmarkbarstate");
        query.prepare(statement);
        query.bindValue(":bookmarkbarstate", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::selectPageZoom(QString zoomValue)
{
    int value;
    if (zoomValue.length() < 4)
        zoomValue.truncate(2);
    else if (zoomValue.length() >= 4)
        zoomValue.truncate(3);
     value = zoomValue.toInt();

     OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
     parentPtr->setWebPageZoom(value);

     if (!settingsDatabase.isOpenError())
     {
         QString statement = QLatin1String("UPDATE appearancesettings SET pagezoom = :pagezoom");
         query.prepare(statement);
         query.bindValue(":pagezoom", QString::number(value).toLatin1());

         if (!query.exec())
             qDebug(query.lastError().text().toLatin1());
         query.finish();
     }
}

void OSettingsCore::selectFontSize(QString fontSize)
{
    int value = 16;
    if (fontSize.compare("Very small") == 0)
        value = 9;
    else if (fontSize.compare("Small") == 0)
        value = 12;
    else if (fontSize.compare("Medium (default size)") == 0)
        value = 16;
    else if (fontSize.compare("Large") == 0)
        value = 20;
    else if (fontSize.compare("Very Large") == 0)
        value = 24;

    normalWebSettings->setFontSize(QWebEngineSettings::DefaultFontSize, value);
    normalWebSettings->setFontSize(QWebEngineSettings::DefaultFixedFontSize, value-3);
    privateWebProfile->settings()->setFontSize(QWebEngineSettings::DefaultFontSize, value);
    privateWebProfile->settings()->setFontSize(QWebEngineSettings::DefaultFixedFontSize, value-3);
    setCustomFontSizeValue();

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE appearancesettings SET fontsize = :fontsize");
        query.prepare(statement);
        query.bindValue(":fontsize", value);

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::selectStandardFont(QString standardFont)
{
    normalWebSettings->setFontFamily(QWebEngineSettings::StandardFont, standardFont);
    privateWebProfile->settings()->setFontFamily(QWebEngineSettings::StandardFont, standardFont);
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE appearancesettings SET standardfont = :standardfont");
        query.prepare(statement);
        query.bindValue(":standardfont", standardFont.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}
void OSettingsCore::selectSerifFont(QString serifFont)
{
    normalWebSettings->setFontFamily(QWebEngineSettings::SerifFont, serifFont);
    privateWebProfile->settings()->setFontFamily(QWebEngineSettings::SerifFont, serifFont);
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE appearancesettings SET seriffont = :seriffont");
        query.prepare(statement);
        query.bindValue(":seriffont", serifFont.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}
void OSettingsCore::selectSansSerifFont(QString sansSerifFont)
{
    normalWebSettings->setFontFamily(QWebEngineSettings::SansSerifFont, sansSerifFont);
    privateWebProfile->settings()->setFontFamily(QWebEngineSettings::SansSerifFont, sansSerifFont);
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE appearancesettings SET sansseriffont = :sansseriffont");
        query.prepare(statement);
        query.bindValue(":sansseriffont", sansSerifFont.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::selectFixedFont(QString fixedFont)
{
    normalWebSettings->setFontFamily(QWebEngineSettings::FixedFont, fixedFont);
    privateWebProfile->settings()->setFontFamily(QWebEngineSettings::FixedFont, fixedFont);
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE appearancesettings SET fixedfont = :fixedfont");
        query.prepare(statement);
        query.bindValue(":fixedfont", fixedFont.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::selectStartPageOption(QString startOption)
{
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE startupsettings SET startpageoption = :startpageoption");
        query.prepare(statement);
        query.bindValue(":startpageoption", startOption.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::addSpecificPage(QString title, QString url)
{
    if (!settingsDatabase.isOpenError())
    {
        QStringList page;
        page << QString::number(generateSpecificPageIndex()) << title << url;
        specificPageList.append(page);

        QString statement = QLatin1String("INSERT INTO specificpage (pageindex, specificpagetitle, specificpageurl) VALUES(:pageindex, :specificpagetitle, :specificpageurl)");
        query.prepare(statement);
        query.bindValue(":pageindex", page.at(0).toInt());
        query.bindValue(":specificpagetitle", title.toLatin1());
        query.bindValue(":specificpageurl", url.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();

        QString script = QLatin1String("setLastAddedIndex('%1');").arg(page.first());
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::deleteSpecificPage(QString title, QString url, QString index)
{
    QStringList page;
    page << index << title << url;
    if (specificPageList.contains(page))
        specificPageList.removeOne(page);

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("DELETE FROM specificpage WHERE pageindex = :pageindex");
        query.prepare(statement);
        query.bindValue(":pageindex", index.toInt());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::saveSpecificPageEditing(QString index, QString title, QString url)
{
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE specificpage SET specificpagetitle = :specificpagetitle, specificpageurl = :specificpageurl WHERE pageindex = :pageindex");
        query.prepare(statement);
        query.bindValue(":specificpagetitle", title.toLatin1());
        query.bindValue(":specificpageurl", url.toLatin1());
        query.bindValue(":pageindex", index.toInt());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();

        QStringList page;
        page << index << title << url;
        for (int i = 0; i < specificPageList.count(); i++)
            if (specificPageList.at(i).at(0).compare(index) == 0)
            {
                specificPageList.replace(i, page);
                break;
            }
    }
}

void OSettingsCore::selectSpellCheckLanguage(QString language)
{//implement english dictionary later
    if (language.compare("English") == 0)
    {
        QStringList englishDictionary;//fill with english word
        normalWebProfile->setSpellCheckLanguages(englishDictionary);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE languagesettings SET spellchecklanguage = :spellchecklanguage");
        query.prepare(statement);
        query.bindValue(":spellchecklanguage", language.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::allowJavascript(QString state)
{
    if (state.compare("true") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    }
    else if (state.compare("false") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::JavascriptEnabled, false);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE privacysecuritysettings SET runjs = :runjs");
        query.prepare(statement);
        query.bindValue(":runjs", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::allowJSCopyPaste(QString state)
{
    if (state.compare("true") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
    }
    else if (state.compare("false") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, false);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE privacysecuritysettings SET jsclipboard = :jsclipboard");
        query.prepare(statement);
        query.bindValue(":jsclipboard", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::allowJSPopUps(QString state)
{
    if (state.compare("true") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    }
    else if (state.compare("false") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE privacysecuritysettings SET jspopup = :jspopup");
        query.prepare(statement);
        query.bindValue(":jspopup", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::enablePlugins(QString state)
{
    if (state.compare("true") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::PluginsEnabled, true);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    }
    else if (state.compare("false") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::PluginsEnabled, false);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE privacysecuritysettings SET plugins = :plugins");
        query.prepare(statement);
        query.bindValue(":plugins", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::allowInsecureContents(QString state)
{
    if (state.compare("true") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    }
    else if (state.compare("false") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, false);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE privacysecuritysettings SET insecurecontent = :insecurecontent");
        query.prepare(statement);
        query.bindValue(":insecurecontent", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::enableInsecureGeolocation(QString state)
{
    if (state.compare("true") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::AllowGeolocationOnInsecureOrigins, true);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::AllowGeolocationOnInsecureOrigins, true);
    }
    else if (state.compare("false") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::AllowGeolocationOnInsecureOrigins, false);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::AllowGeolocationOnInsecureOrigins, false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE privacysecuritysettings SET insecuregeolocation = :insecuregeolocation");
        query.prepare(statement);
        query.bindValue(":insecuregeolocation", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::enableGeolocation(QString state)
{
    OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());

    if (state.compare("true") == 0)
        parentPtr->enableGeolocation(true);
    else if (state.compare("false") == 0)
    {
        parentPtr->enableGeolocation(false);
        enableInsecureGeolocation("false");
        setEnableInsecureGeolocation();
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE privacysecuritysettings SET geolocation = :geolocation");
        query.prepare(statement);
        query.bindValue(":geolocation", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::enableDnsPrefetch(QString state)
{
    if (state.compare("true") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    }
    else if (state.compare("false") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, false);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE privacysecuritysettings SET dnsentries = :dnsentries");
        query.prepare(statement);
        query.bindValue(":dnsentries", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::manageCertificates()
{
    OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
    OCertificateManager *certManager = new OCertificateManager(parentPtr->getActiveTabPage());
    certManager->setWebProfile(normalWebProfile);
    certManager->exec();
}

void OSettingsCore::enableInternalPDFViewer(QString state)
{
    if (state.compare("true") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::PdfViewerEnabled, true);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::PdfViewerEnabled, true);
    }
    else if (state.compare("false") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::PdfViewerEnabled, false);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::PdfViewerEnabled, false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE privacysecuritysettings SET internalpdfviewer = :internalpdfviewer");
        query.prepare(statement);
        query.bindValue(":internalpdfviewer", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::selectWebProtection(QString protection)
{
    if (protection.compare("standardProtection") == 0)
        enableStandardProtection();
    else if (protection.compare("strongProtection") == 0)
        enableStrongProtection();
    else if (protection.compare("noneProtection") == 0)
        disableWebProtection();

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE privacysecuritysettings SET webprotection = :webprotection");
        query.prepare(statement);
        query.bindValue(":webprotection", protection.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::enableStrongProtection()
{
    blockAllTrackers = true;
    blockDangerousTrackers = true;
    blockInPrivateBrowsing = true;

    normalWebSettings->setUnknownUrlSchemePolicy(QWebEngineSettings::DisallowUnknownUrlSchemes);
    privateWebProfile->settings()->setUnknownUrlSchemePolicy(QWebEngineSettings::DisallowUnknownUrlSchemes);
    allowInsecureContents("false");
    setAllowInsecureContents();
    allowJSCopyPaste("false");
    setAllowJSCopyPaste();
    allowJSPopUps("false");
    setAllowJSPopUps();
    allowJavascript("false");
    setAllowJavascript();
    enableGeolocation("false");
    setEnableGeolocation(false);
    enableInsecureGeolocation("false");
    setEnableInsecureGeolocation();
    enableCamera("false");
    setEnableCamera(false);
    enableMicrophone("false");
    setEnableMicrophone(false);
    enableDoNotTrack("true");
    setEnableDoNotTrack(true);
}

void OSettingsCore::enableStandardProtection()
{
    blockAllTrackers = false;
    blockDangerousTrackers = true;
    blockInPrivateBrowsing = true;

    normalWebSettings->setUnknownUrlSchemePolicy(QWebEngineSettings::AllowUnknownUrlSchemesFromUserInteraction);
    privateWebProfile->settings()->setUnknownUrlSchemePolicy(QWebEngineSettings::AllowUnknownUrlSchemesFromUserInteraction);
    allowInsecureContents("false");
    setAllowInsecureContents();
    allowJSCopyPaste("true");
    setAllowJSCopyPaste();
    allowJSPopUps("true");
    setAllowJSPopUps();
    allowJavascript("true");
    setAllowJavascript();
    enableGeolocation("true");
    setEnableGeolocation(true);
    enableInsecureGeolocation("false");
    setEnableInsecureGeolocation();
    enableCamera("true");
    setEnableCamera(true);
    enableMicrophone("true");
    setEnableMicrophone(true);
    enableDoNotTrack("true");
    setEnableDoNotTrack(true);
}

void OSettingsCore::disableWebProtection()
{
    blockAllTrackers = false;
    blockDangerousTrackers = false;
    blockInPrivateBrowsing = false;

    normalWebSettings->setUnknownUrlSchemePolicy(QWebEngineSettings::AllowAllUnknownUrlSchemes);
    privateWebProfile->settings()->setUnknownUrlSchemePolicy(QWebEngineSettings::AllowAllUnknownUrlSchemes);
    allowInsecureContents("true");
    setAllowInsecureContents();
    allowJSCopyPaste("true");
    setAllowJSCopyPaste();
    allowJSPopUps("true");
    setAllowJSPopUps();
    allowJavascript("true");
    setAllowJavascript();
    enableGeolocation("true");
    setEnableGeolocation(true);
    enableInsecureGeolocation("true");
    setEnableInsecureGeolocation();
    enableCamera("true");
    setEnableCamera(true);
    enableMicrophone("true");
    setEnableMicrophone(true);
    enableDoNotTrack("false");
    setEnableDoNotTrack(false);
}

const QString OSettingsCore::getWebProtection()
{
    QString protection = "standardProtection";
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT webprotection FROM privacysecuritysettings");
        if (query.exec(statement))
            while (query.next())
                protection = QString(query.value(0).toByteArray());
    }
    return protection;
}

void OSettingsCore::askToSavePassword(QString state)
{
    if (state.compare("true") == 0)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->setPasswordSaving(true);
    }
    else if (state.compare("false") == 0)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->setPasswordSaving(false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE personaldatasettings SET savepassword = :savepassword");
        query.prepare(statement);
        query.bindValue(":savepassword", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::websiteAutoSignIn(QString state)
{
    if (state.compare("true") == 0)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->setSiteAutoSignIn(true);
    }
    else if (state.compare("false") == 0)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->setSiteAutoSignIn(false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE personaldatasettings SET autosignin = :autosignin");
        query.prepare(statement);
        query.bindValue(":autosignin", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::saveCredentials(QByteArray icon, QString webAddress, QString loginAddress, QString password)
{
    if (!settingsDatabase.isOpenError())
    {
        QStringList credential;
        credential << QString::number(generateCredentialIndex()) << QString(icon) << webAddress << loginAddress << password;
        credentialList.append(credential);

        QString statement = QLatin1String("INSERT INTO credentials (credentialindex, icon, webAddress, loginAddress, password) VALUES(:credentialindex, :icon, :webAddress, :loginAddress, :password)");
        query.prepare(statement);
        query.bindValue(":credentialindex", credential.at(0).toInt());
        query.bindValue(":icon", icon);
        query.bindValue(":webAddress", webAddress.toLatin1());
        query.bindValue(":loginAddress", loginAddress.toLatin1());
        query.bindValue(":password", password.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::deleteCredential(QString index)
{

    for (int i = 0; i < credentialList.count(); i++)
        if (credentialList.at(i).at(0).compare(index) == 0)
        {
            credentialList.removeAt(i);
            if (!settingsDatabase.isOpenError())
            {
                QString statement = QLatin1String("DELETE FROM credentials WHERE credentialindex = :credentialindex");
                query.prepare(statement);
                query.bindValue(":credentialindex", index.toInt());

                if (!query.exec())
                    qDebug(query.lastError().text().toLatin1());
                query.finish();
            }
            break;
        }
}

void OSettingsCore::saveCredentialPassword(QString index, QString password)
{
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE credentials SET password = :password WHERE credentialindex = :credentialindex");
        query.prepare(statement);
        query.bindValue(":password", password.toLatin1());
        query.bindValue(":credentialindex", index.toInt());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();

        for (int i = 0; i < credentialList.count(); i++)
            if (credentialList.at(i).at(0).compare(index) == 0)
            {
                QStringList credential;
                credential << index << credentialList.at(i).at(1) << credentialList.at(i).at(2) << credentialList.at(i).at(3) << credentialList.at(i).at(4);
                credentialList.replace(i, credential);
                break;
            }
    }
}

void OSettingsCore::autoSavePaymentMethods(QString state)
{
    if (state.compare("true") == 0)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->setPaymentSaveAndFill(true);
    }
    else if (state.compare("false") == 0)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->setPaymentSaveAndFill(false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE personaldatasettings SET autosavefill = :autosavefill");
        query.prepare(statement);
        query.bindValue(":autosavefill", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::savePaymentMethod(QString cardName, QString cardNumber, QString cardExpiration)
{
    if (!settingsDatabase.isOpenError())
    {
        QStringList paymentMethod;
        paymentMethod << QString::number(generatePaymentMethodIndex()) << cardName << cardNumber << cardExpiration;
        paymentMethodList.append(paymentMethod);

        QString statement = QLatin1String("INSERT INTO paymentmethods (pmethodindex, cardName, cardNumber, cardExpiration) VALUES(:pmethodindex, :cardName, :cardNumber, :cardExpiration)");
        query.prepare(statement);
        query.bindValue(":pmethodindex", cardName.toLatin1());
        query.bindValue(":cardName", cardName.toLatin1());
        query.bindValue(":cardNumber", cardNumber.toLatin1());
        query.bindValue(":cardExpiration", cardExpiration.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::deletePaymentMethod(QString index)
{
    for (int i = 0; i < paymentMethodList.count(); i++)
        if (paymentMethodList.at(i).at(0).compare(index) == 0)
        {
            paymentMethodList.removeAt(i);
            if (!settingsDatabase.isOpenError())
            {
                QString statement = QLatin1String("DELETE FROM paymentmethods WHERE pmethodindex = :pmethodindex");
                query.prepare(statement);
                query.bindValue(":pmethodindex", index.toInt());

                if (!query.exec())
                    qDebug(query.lastError().text().toLatin1());
                query.finish();
            }
            break;
        }
}

void OSettingsCore::savePaymentEditing(QString index, QString cardName, QString cardNumber, QString month, QString year)
{
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE credentials SET cardName = :cardName, cardNumber = :cardNumber, cardExpiration = :cardExpiration WHERE pmethodindex = :pmethodindex");
        query.prepare(statement);
        query.bindValue(":cardName", cardName.toLatin1());
        query.bindValue(":cardNumber", cardNumber.toLatin1());
        query.bindValue(":cardExpiration", QString(month + "/" + year).toLatin1());
        query.bindValue(":pmethodindex", index.toInt());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();

        for (int i = 0; i < paymentMethodList.count(); i++)
            if (paymentMethodList.at(i).at(0).compare(index) == 0)
            {
                QStringList paymentMethod;
                paymentMethod << index << paymentMethodList.at(i).at(1) << paymentMethodList.at(i).at(2) << paymentMethodList.at(i).at(3);
                paymentMethodList.replace(i, paymentMethod);
                break;
            }
    }
}

void OSettingsCore::selectCookiesPolicy(QString policy)
{
    if (policy.compare("enableCookies") == 0)
    {
        normalWebProfile->cookieStore()->setCookieFilter(
            [](const QWebEngineCookieStore::FilterRequest &request)
            { return true; }
            );
    }
    else if (policy.compare("privateBrowsingCookies") == 0)
    {
        normalWebProfile->cookieStore()->setCookieFilter(
            [](const QWebEngineCookieStore::FilterRequest &request)
            { return !request.thirdParty; }
            );
    }
    else if (policy.compare("noneCookies") == 0)
    {
        normalWebProfile->cookieStore()->setCookieFilter(
            [](const QWebEngineCookieStore::FilterRequest &request)
            { return false; }
            );
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE browsingdatasettings SET cookieoption = :cookieoption");
        query.prepare(statement);
        query.bindValue(":cookieoption", policy.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::deleteCookiesOnClose(QString state)
{
    if (state.compare("true") == 0)
        normalWebProfile->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    else if (state.compare("false") == 0)
        normalWebProfile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE browsingdatasettings SET deleteonclose = :deleteonclose");
        query.prepare(statement);
        query.bindValue(":deleteonclose", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::enableDoNotTrack(QString state)
{
    if (state.compare("true") == 0)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->setDoNotTrack(true);
    }
    else if (state.compare("false") == 0)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->setDoNotTrack(false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE browsingdatasettings SET donottrack = :donottrack");
        query.prepare(statement);
        query.bindValue(":donottrack", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::clearBrowsingData(QString cookieData, QString cachedData, QString browsingHistory, QString passwordData, QString paymentData)
{
    if (cookieData.compare("true") == 0)
    {
        normalWebProfile->clearAllVisitedLinks();
        normalWebProfile->cookieStore()->deleteAllCookies();
    }

    if (cachedData.compare("true") == 0)
    {
        normalWebProfile->clearHttpCache();
        clearCacheFolders(OAppCore::applicationDirPath().append("/ololanWebStorage/Cache"));
    }

    if (browsingHistory.compare("true") == 0)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->getHistoryManager()->clearHistory();
    }

    if (passwordData.compare("true") == 0)
    {
        credentialList.clear();
        credentialUsedIndexList.clear();
        setupIndexList();

        QString statement = QLatin1String("DELETE FROM credentials");
        query.prepare(statement);
        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
    }

    if (paymentData.compare("true") == 0)
    {
        paymentMethodList.clear();
        paymentMethodUsedIndexList.clear();
        setupIndexList();

        QString statement = QLatin1String("DELETE FROM paymentmethods");
        query.prepare(statement);
        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
    }
    updateBrowsingData();
}

void OSettingsCore::loadCookieData()
{
    if (isOpened)
        for (int i = 0; i < cookieList.count(); i++)
        {
            int count = 1;
            for (int j = i+1; j < cookieList.count(); j++)
                if ((cookieList.at(i).value() == cookieList.at(j).value()) || (cookieList.at(i).name() == cookieList.at(j).name()))
                    count++;

            QString script = QLatin1String("loadCookieData('%1', '%2', '%3');").arg(cookieList.at(i).name(), cookieList.at(i).value(), QString::number(count));
            settingsWebView->page()->runJavaScript(script);
        }
}

void OSettingsCore::deleteCookie(QString value, QString name)
{
    for (int i = 0; i < cookieList.count(); i++)
        if (((cookieList.at(i).value() == value.toLatin1()) && !cookieList.at(i).value().isEmpty()) || ((cookieList.at(i).name() == name.toLatin1()) && !cookieList.at(i).name().isEmpty()))
        {
            normalWebProfile->cookieStore()->deleteCookie(cookieList.at(i));
            cookieList.removeAt(i);
            break;
        }
}

void OSettingsCore::clearAllCookies()
{
    clearBrowsingData("true", "false", "false", "false", "false");
}

void OSettingsCore::selectDefaultSearchEngine(QString searchEngine)
{
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE searchenginesettings SET selectedsearchengine = :selectedsearchengine");
        query.prepare(statement);
        query.bindValue(":selectedsearchengine", searchEngine.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();

        if ((searchEngine.compare("Google") != 0) && (searchEngine.compare("Bing") != 0) &&
            (searchEngine.compare("Yahoo") != 0) && (searchEngine.compare("DuckDuckGo") != 0))
        {
            QString statementA = QLatin1String("SELECT engineurl FROM searchenginelist WHERE enginename = :enginename");
            query.prepare(statementA);
            query.bindValue(":enginename", searchEngine.toLatin1());
            searchEngine = searchEngine.append("#*#");

            if (query.exec())
                while (query.next())
                    searchEngine = searchEngine.append(QString(query.value(0).toByteArray()));
        }

        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->setSearchEngine(searchEngine);
    }
}

void OSettingsCore::selectUILanguage(QString language)
{
    //set in settings **to be implemented after adding another language**
    if (!settingsDatabase.isOpenError())
    {
        synchronized = false;
        QString statement = QLatin1String("UPDATE languagesettings SET uilanguage = :uilanguage");
        query.prepare(statement);
        query.bindValue(":uilanguage", language.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::selectDownloadLocation()
{
    QString path = QFileDialog::getExistingDirectory(nullptr, QString("Select location"), normalWebProfile->downloadPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE downloadsettings SET downloadlocation = :downloadlocation");
        query.prepare(statement);
        query.bindValue(":downloadlocation", path.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();

        normalWebProfile->setDownloadPath(path);
        privateWebProfile->setDownloadPath(path);
        setDownloadLocation();
    }
}

void OSettingsCore::askDownloadLocation(QString state)
{
    if (state.compare("true") == 0)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->setAskDownloadPath(true);
    }
    else if (state.compare("false") == 0)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->setAskDownloadPath(false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE downloadsettings SET askingoption = :askingoption");
        query.prepare(statement);
        query.bindValue(":askingoption", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::enableOlolanAssistant(QString state)
{
    if (state.compare("true") == 0)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->setEnableAssistant(true);
    }
    else if (state.compare("false") == 0)
    {
        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        parentPtr->setEnableAssistant(false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE assistantsettings SET assistantenablestate = :assistantenablestate");
        query.prepare(statement);
        query.bindValue(":assistantenablestate", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::clearAssistantData()
{
    //to be done after server setup
    //send info to the server to clean data
}

void OSettingsCore::setDefaultBrowser()
{
    QString executablePath = OAppCore::applicationFilePath();
    QStringList protocolsAndFileExtensions = { "http", "https", ".html", ".htm", ".shtml", ".xht", ".xhtml", ".pdf" };

    for (const QString &item : protocolsAndFileExtensions) {
        QString protocolRegistryPath = "HKEY_CLASSES_ROOT\\" + item + "\\shell\\open\\command";
        QSettings protocolRegistry(protocolRegistryPath, QSettings::NativeFormat);
        protocolRegistry.setValue("", "\"" + executablePath + "\" \"%1\"");

        QString extensionRegistryPath = "HKEY_CLASSES_ROOT\\" + item;
        QSettings extensionRegistry(extensionRegistryPath, QSettings::NativeFormat);
        extensionRegistry.setValue("Default", "Ololan Browser");
    }

    QString registryPathA = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Shell\\Associations\\URLAssociations\\http\\UserChoice";
    QString registryValueNameA = "Progid";
    QSettings registryA(registryPathA, QSettings::NativeFormat);
    registryA.setValue(registryValueNameA, "AB55AADE-C448-47B4-BAC7-D1D3B31703CD");

    QString registryPathB = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Shell\\Associations\\URLAssociations\\https\\UserChoice";
    QString registryValueNameB = "Progid";
    QSettings registryB(registryPathB, QSettings::NativeFormat);
    registryB.setValue(registryValueNameB, "AB55AADE-C448-47B4-BAC7-D1D3B31703CD");

    QString statement = QLatin1String("UPDATE defaultbrowsersettings SET defaultbrowserstate = :defaultbrowserstate");
    query.prepare(statement);
    query.bindValue(":defaultbrowserstate", QString("true").toLatin1());

    if (!query.exec())
        qDebug(query.lastError().text().toLatin1());
    query.finish();
    setDefaultBrowserState(true);
}

void OSettingsCore::checkDefaultBrowserState()
{
    bool httpProtocols = true;
    bool httpsProtocols = true;

    QString registryPathA = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Shell\\Associations\\URLAssociations\\http\\UserChoice";
    QString registryValueNameA = "Progid";
    QSettings registryA(registryPathA, QSettings::NativeFormat);
    QString currentProgIDA = registryA.value(registryValueNameA).toString();

    if (currentProgIDA != "AB55AADE-C448-47B4-BAC7-D1D3B31703CD")
        httpProtocols = false;

    QString registryPathB = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Shell\\Associations\\URLAssociations\\https\\UserChoice";
    QString registryValueNameB = "Progid";
    QSettings registryB(registryPathB, QSettings::NativeFormat);
    QString currentProgIDB = registryB.value(registryValueNameB).toString();

    if (currentProgIDB != "AB55AADE-C448-47B4-BAC7-D1D3B31703CD")
        httpsProtocols = false;

    if (!httpProtocols || !httpsProtocols)
    {
        QString statement = QLatin1String("UPDATE defaultbrowsersettings SET defaultbrowserstate = :defaultbrowserstate");
        query.prepare(statement);
        query.bindValue(":defaultbrowserstate", QString("false").toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::useCanvasAcceleration(QString state)
{
    if (state.compare("true") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
    }
    else if (state.compare("false") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, false);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE systemsettings SET canvas = :canvas");
        query.prepare(statement);
        query.bindValue(":canvas", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::useWebGLAcceleration(QString state)
{
    if (state.compare("true") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::WebGLEnabled, true);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    }
    else if (state.compare("false") == 0)
    {
        normalWebSettings->setAttribute(QWebEngineSettings::WebGLEnabled, false);
        privateWebProfile->settings()->setAttribute(QWebEngineSettings::WebGLEnabled, false);
    }

    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE systemsettings SET webgl = :webgl");
        query.prepare(statement);
        query.bindValue(":webgl", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::manageProxy()
{
    winrt::Windows::System::Launcher::LaunchUriAsync(winrt::Windows::Foundation::Uri(L"ms-settings:network-proxy"));
}

void OSettingsCore::restoreSettings()
{
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("DROP TABLE IF EXISTS appearancesettings, accountsettings, assistantsettings,"
                                          "browsingdatasettings, searchEnginesettings, languagesettings, downloadsettings,"
                                          "defaultbrowsersettings, systemsettings, startupsettings, privacysecuritysettings,"
                                          "personaldatasettings, specificpage, credentials, paymentmethods");
        query.prepare(statement);
        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();

        setupAccountSettings();
        setupAppearanceSettings();
        setupStartupSettings();
        setupPrivacySecuritySettings();
        setupPersonalDataSettings();
        setupBrowsingDataSettings();
        setupSearchEngineSettings();
        setupLanguageSettings();
        setupDownloadSettings();
        setupAssistantSettings();
        setupDefaultBrowserSettings();
        setupSystemSettings();
    }
}

void OSettingsCore::setupAccountSettings()
{
    QString statementA1 = QLatin1String("SELECT synchronizestate, sendstatisticstate, username, userpassword FROM accountsettings");
    if (query.exec(statementA1))
    {
        while (query.next())
        {
            setSyncState(QString(query.value(0).toByteArray()), QString(query.value(2).toByteArray()));
            setSendUsageData((QString(query.value(1).toByteArray()).compare("true") == 0) ? true : false);
        }
    }
    else
    {
        qDebug(query.lastError().text().toLatin1());
        QString statementA2 = QLatin1String("CREATE TABLE accountsettings(synchronizestate VARCHAR(64) NOT NULL, sendstatisticstate VARCHAR(64) NOT NULL,"
                                            "username VARCHAR(256) NOT NULL, userpassword VARCHAR(256) NOT NULL, login VARCHAR(256) NOT NULL)");
        if (!query.exec(statementA2))
            qDebug(query.lastError().text().toLatin1());
        else
        {
            QString statementA3 = QLatin1String("INSERT INTO accountsettings(synchronizestate, sendstatisticstate, username, userpassword, login)"
                                                "VALUES(:synchronizestate, :sendstatisticstate, :username, :userpassword, :login)");
            query.prepare(statementA3);
            query.bindValue(":synchronizestate", QString("Signed out").toLatin1());
            query.bindValue(":sendstatisticstate", QString("false").toLatin1());
            query.bindValue(":username", QString("Guest User").toLatin1());
            query.bindValue(":userpassword", QString("guestpassword").toLatin1());
            query.bindValue(":login", QString("guestuser@ololan.com").toLatin1());
            qDebug("state : false || user : Guest User");

            if (!query.exec())
                qDebug(query.lastError().text().toLatin1());
            setupAccountSettings();
        }
    }
    query.finish();
}

void OSettingsCore::setupAppearanceSettings()
{
    QString statementA1 = QLatin1String("SELECT themecolor, bookmarkbarstate, pagezoom, fontsize, customfontsize, minimumfontsize,"
                                        "standardfont, seriffont, sansseriffont, fixedfont FROM appearancesettings");
    if (query.exec(statementA1))
    {
        while (query.next())
        {
            setSelectedThemeColor(QString(query.value(0).toByteArray()));
            setBookmarkBarState((QString(query.value(1).toByteArray()).compare("true") == 0) ? true : false);
            bookmarkBarVisibile = (QString(query.value(1).toByteArray()).compare("true") == 0) ? true : false;
            setPageZoom(QString(query.value(2).toByteArray()));
            setSelectedFontSize(query.value(3).toInt());

            normalWebSettings->setFontSize(QWebEngineSettings::DefaultFontSize, query.value(4).toInt());
            normalWebSettings->setFontSize(QWebEngineSettings::DefaultFixedFontSize, query.value(4).toInt()-3);
            setCustomFontSizeValue();
            normalWebSettings->setFontSize(QWebEngineSettings::MinimumFontSize, query.value(5).toInt());
            normalWebSettings->setFontSize(QWebEngineSettings::MinimumLogicalFontSize, query.value(5).toInt());
            setMinimumFontSizeValue();

            normalWebSettings->setFontFamily(QWebEngineSettings::StandardFont, QString(query.value(6).toByteArray()));
            setSelectedStandardFont();
            normalWebSettings->setFontFamily(QWebEngineSettings::SerifFont, QString(query.value(7).toByteArray()));
            setSelectedSerifFont();
            normalWebSettings->setFontFamily(QWebEngineSettings::SansSerifFont, QString(query.value(8).toByteArray()));
            setSelectedSansSerifFont();
            normalWebSettings->setFontFamily(QWebEngineSettings::FixedFont, QString(query.value(9).toByteArray()));
            setSelectedFixedFont();
        }
    }
    else
    {
        qDebug(query.lastError().text().toLatin1());
        QString statementA2 = QLatin1String("CREATE TABLE appearancesettings(themecolor VARCHAR(256) NOT NULL, bookmarkbarstate VARCHAR(256) NOT NULL,"
                                            "pagezoom VARCHAR(256) NOT NULL, fontsize SMALLINT UNSIGNED NOT NULL, customfontsize SMALLINT UNSIGNED NOT NULL,"
                                            "minimumfontsize SMALLINT UNSIGNED NOT NULL, standardfont VARCHAR(256) NOT NULL, seriffont VARCHAR(256) NOT NULL,"
                                            "sansseriffont VARCHAR(256) NOT NULL, fixedfont VARCHAR(256) NOT NULL)");
        if (!query.exec(statementA2))
            qDebug(query.lastError().text().toLatin1());
        else
        {
            QString statementA3 = QLatin1String("INSERT INTO appearancesettings (themecolor, bookmarkbarstate, pagezoom, fontsize, customfontsize, minimumfontsize,"
                                                "standardfont, seriffont, sansseriffont, fixedfont)"
                                                "VALUES(:themecolor, :bookmarkbarstate, :pagezoom, :fontsize, :customfontsize, :minimumfontsize,"
                                                ":standardfont, :seriffont, :sansseriffont, :fixedfont)");
            query.prepare(statementA3);
            query.bindValue(":themecolor", QString("Light gray").toLatin1());
            query.bindValue(":bookmarkbarstate", QString("true").toLatin1());
            query.bindValue(":pagezoom", QString("100").toLatin1());
            query.bindValue(":fontsize", 16);
            query.bindValue(":customfontsize", 16);
            query.bindValue(":minimumfontsize", 9);
            query.bindValue(":standardfont", QString("Times New Roman").toLatin1());
            query.bindValue(":seriffont", QString("Times New Roman").toLatin1());
            query.bindValue(":sansseriffont", QString("Arial").toLatin1());
            query.bindValue(":fixedfont", QString("Consolas").toLatin1());
            if (!query.exec())
                qDebug(query.lastError().text().toLatin1());

            setupAppearanceSettings();
        }
    }
    query.finish();
}

void OSettingsCore::setupStartupSettings()
{
    QString statementA1 = QLatin1String("SELECT startpageoption FROM startupsettings");
    if (query.exec(statementA1))
    {
        while (query.next())
        {
            setSelectedStartPageOption(QString(query.value(0).toByteArray()));
            if (QString(query.value(0).toByteArray()).compare("specific page") == 0)
            {
                QString statementB1 = QLatin1String("SELECT pageindex, specificpagetitle, specificpageurl FROM specificpage");
                if (query.exec(statementB1))
                    while (query.next())
                    {
                        loadSpecificPage(query.value(0).toInt(), QString(query.value(1).toByteArray()), QString(query.value(2).toByteArray()));
                        if (specificPageUsedIndexList.contains(query.value(0).toInt()))
                            specificPageUsedIndexList.removeOne(query.value(0).toInt());

                        QStringList page;
                        page << QString::number(query.value(0).toInt()) << QString(query.value(1).toByteArray()) << QString(query.value(2).toByteArray());
                        if (!specificPageList.contains(page))
                        specificPageList.append(page);
                    }
            }
        }
    }
    else
    {
        qDebug(query.lastError().text().toLatin1());
        QString statementA2 = QLatin1String("CREATE TABLE startupsettings(startpageoption VARCHAR(128) NOT NULL)");
        if (!query.exec(statementA2))
            qDebug(query.lastError().text().toLatin1());
        else
        {
            QString statementA3 = QLatin1String("INSERT INTO startupsettings (startpageoption) VALUES(:startpageoption)");
            query.prepare(statementA3);
            query.bindValue(":startpageoption", QString("start page").toLatin1());
            if (!query.exec())
                qDebug(query.lastError().text().toLatin1());

            QString statementB2 = QLatin1String("CREATE TABLE specificpage(pageindex SMALLINT UNSIGNED NOT NULL, specificpagetitle VARCHAR(256) NOT NULL, specificpageurl VARCHAR(2048) NOT NULL)");
            if (!query.exec(statementB2))
                qDebug(query.lastError().text().toLatin1());

            setupStartupSettings();
        }
    }
    query.finish();
}

void OSettingsCore::setupPrivacySecuritySettings()
{
    QString statementA1 = QLatin1String("SELECT runjs, jsclipboard, jspopup, plugins, insecurecontent, insecuregeolocation,"
                                        "geolocation, dnsentries, internalpdfviewer, webprotection, camera, microphone FROM privacysecuritysettings");
    if (query.exec(statementA1))
    {
        while (query.next())
        {
            normalWebSettings->setAttribute(QWebEngineSettings::JavascriptEnabled, (QString(query.value(0).toByteArray()).compare("true") == 0) ? true : false);
            setAllowJavascript();
            normalWebSettings->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, (QString(query.value(1).toByteArray()).compare("true") == 0) ? true : false);
            setAllowJSCopyPaste();
            normalWebSettings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, (QString(query.value(2).toByteArray()).compare("true") == 0) ? true : false);
            setAllowJSPopUps();

            normalWebSettings->setAttribute(QWebEngineSettings::PluginsEnabled, (QString(query.value(3).toByteArray()).compare("true") == 0) ? true : false);
            setEnablePlugins();
            normalWebSettings->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, (QString(query.value(4).toByteArray()).compare("true") == 0) ? true : false);
            setAllowInsecureContents();
            normalWebSettings->setAttribute(QWebEngineSettings::AllowGeolocationOnInsecureOrigins, (QString(query.value(5).toByteArray()).compare("true") == 0) ? true : false);
            setEnableInsecureGeolocation();

            setEnableGeolocation((QString(query.value(6).toByteArray()).compare("true") == 0) ? true : false);
            normalWebSettings->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, (QString(query.value(7).toByteArray()).compare("true") == 0) ? true : false);
            setEnableDnsPrefetch();
            normalWebSettings->setAttribute(QWebEngineSettings::PdfViewerEnabled, (QString(query.value(8).toByteArray()).compare("true") == 0) ? true : false);
            setEnableInternalPDFViewer();
            setEnableCamera((QString(query.value(10).toByteArray()).compare("true") == 0) ? true : false);
            setEnableMicrophone((QString(query.value(11).toByteArray()).compare("true") == 0) ? true : false);
            setSelectedWebProtection(QString(query.value(9).toByteArray()));
        }
    }
    else
    {
        qDebug(query.lastError().text().toLatin1());
        QString statementA2 = QLatin1String("CREATE TABLE privacysecuritysettings(runjs VARCHAR(64) NOT NULL, jsclipboard VARCHAR(64) NOT NULL, jspopup VARCHAR(64) NOT NULL,"
                                            "plugins VARCHAR(64) NOT NULL, insecurecontent VARCHAR(64) NOT NULL, insecuregeolocation VARCHAR(64) NOT NULL,"
                                            "geolocation VARCHAR(64) NOT NULL, dnsentries VARCHAR(64) NOT NULL, internalpdfviewer VARCHAR(64) NOT NULL, webprotection VARCHAR(256) NOT NULL,"
                                            "camera VARCHAR(64) NOT NULL, microphone VARCHAR(64) NOT NULL)");
        if (!query.exec(statementA2))
            qDebug(query.lastError().text().toLatin1());
        else
        {
            QString statementA3 = QLatin1String("INSERT INTO privacysecuritysettings (runjs, jsclipboard, jspopup, plugins, insecurecontent, insecuregeolocation,"
                                                "geolocation, dnsentries, internalpdfviewer, webprotection, camera, microphone)"
                                                "VALUES(:runjs, :jsclipboard, :jspopup, :plugins, :insecurecontent, :insecuregeolocation,"
                                                ":geolocation, :dnsentries, :internalpdfviewer, :webprotection, :camera, :microphone)");
            query.prepare(statementA3);
            query.bindValue(":runjs", QString("true").toLatin1());
            query.bindValue(":jsclipboard", QString("true").toLatin1());
            query.bindValue(":jspopup", QString("true").toLatin1());
            query.bindValue(":plugins", QString("true").toLatin1());
            query.bindValue(":insecurecontent", QString("false").toLatin1());
            query.bindValue(":insecuregeolocation", QString("false").toLatin1());
            query.bindValue(":geolocation", QString("true").toLatin1());
            query.bindValue(":dnsentries", QString("false").toLatin1());
            query.bindValue(":internalpdfviewer", QString("true").toLatin1());
            query.bindValue(":webprotection", QString("standardProtection").toLatin1());
            query.bindValue(":camera", QString("true").toLatin1());
            query.bindValue(":microphone", QString("true").toLatin1());
            if (!query.exec())
                qDebug(query.lastError().text().toLatin1());

            setupPrivacySecuritySettings();
        }
    }
    query.finish();
}

void OSettingsCore::setupPersonalDataSettings()
{
    QString statementA1 = QLatin1String("SELECT savepassword, autosignin, autosavefill FROM personaldatasettings");
    if (query.exec(statementA1))
    {
        while (query.next())
        {
            setAskToSavePassword((QString(query.value(0).toByteArray()).compare("true") == 0) ? true : false);
            setWebsiteAutoSignIn((QString(query.value(1).toByteArray()).compare("true") == 0) ? true : false);
            setAutoSavePaymentMethods((QString(query.value(2).toByteArray()).compare("true") == 0) ? true : false);
        }

        QString statementB1 = QLatin1String("SELECT credentialindex, icon, webAddress, loginAddress, password FROM credentials");
        if (query.exec(statementB1))
            while (query.next())
            {
                loadCredentials(query.value(0).toInt(), query.value(1).toByteArray(), query.value(2).toString(), query.value(3).toString(), query.value(4).toString());
                if (credentialUsedIndexList.contains(query.value(0).toInt()))
                    credentialUsedIndexList.removeOne(query.value(0).toInt());

                QStringList credential;
                credential << QString::number(query.value(0).toInt()) << QString(query.value(1).toByteArray()) << query.value(2).toString() << query.value(3).toString() << query.value(4).toString();
                if (!credentialList.contains(credential))
                credentialList.append(credential);
            }

        /*QString statementC1 = QLatin1String("SELECT pmethodindex, cardName, cardNumber, cardExpiration FROM paymentmethods");
        if (query.exec(statementC1))
            while (query.next())
            {
                loadPaymentMethods(query.value(0).toInt(), query.value(1).toString(), query.value(2).toString(), query.value(3).toString());
                if (paymentMethodUsedIndexList.contains(query.value(0).toInt()))
                    paymentMethodUsedIndexList.removeOne(query.value(0).toInt());

                QStringList paymentMethod;
                paymentMethod << QString::number(query.value(0).toInt()) << query.value(1).toString() << query.value(2).toString() << query.value(3).toString();
                if (!paymentMethodList.contains(paymentMethod))
                paymentMethodList.append(paymentMethod);
            }*/
    }
    else
    {
        qDebug(query.lastError().text().toLatin1());
        QString statementA2 = QLatin1String("CREATE TABLE personaldatasettings(savepassword VARCHAR(64) NOT NULL, autosignin VARCHAR(64) NOT NULL, autosavefill VARCHAR(64) NOT NULL)");
        if (!query.exec(statementA2))
            qDebug(query.lastError().text().toLatin1());
        else
        {
            QString statementA3 = QLatin1String("INSERT INTO personaldatasettings (savepassword, autosignin, autosavefill) VALUES(:savepassword, :autosignin, :autosavefill)");
            query.prepare(statementA3);
            query.bindValue(":savepassword", QString("true").toLatin1());
            query.bindValue(":autosignin", QString("true").toLatin1());
            query.bindValue(":autosavefill", QString("false").toLatin1());
            if (!query.exec())
                qDebug(query.lastError().text().toLatin1());

            QString statementB2 = QLatin1String("CREATE TABLE credentials(credentialindex SMALLINT UNSIGNED NOT NULL, icon VARCHAR(8192) NOT NULL, webAddress VARCHAR(2048) NOT NULL,"
                                                "loginAddress VARCHAR(2048) NOT NULL, password VARCHAR(2048) NOT NULL)");
            if (!query.exec(statementB2))
                qDebug(query.lastError().text().toLatin1());

            /*QString statementC2 = QLatin1String("CREATE TABLE paymentmethods(pmethodindex SMALLINT UNSIGNED NOT NULL, cardName VARCHAR(256) NOT NULL, cardNumber VARCHAR(256) NOT NULL,"
                                                "cardExpiration VARCHAR(256) NOT NULL)");
            if (!query.exec(statementC2))
                qDebug(query.lastError().text().toLatin1());*/

            setupPersonalDataSettings();
        }
    }
    query.finish();
}

void OSettingsCore::setupBrowsingDataSettings()
{
    QString statementB2 = QLatin1String("SELECT * FROM cookiedatalist");
    if (!query.exec(statementB2))
        qDebug(query.lastError().text().toLatin1());
    else
    {
        statementB2 = QLatin1String("DROP TABLE cookiedatalist");
        query.exec(statementB2);
    }

    QString statementA1 = QLatin1String("SELECT cookieoption, deleteonclose, donottrack, cookieAndDatasize, cachedDatasize,"
                                        "browsingHistorysize, passwordDatasize, paymentDatasize FROM browsingdatasettings");
    if (query.exec(statementA1))
    {
        while (query.next())
        {
            setSelectedCookiesPolicy(query.value(0).toString());
            setDeleteCookiesOnClose((QString(query.value(1).toByteArray()).compare("true") == 0) ? true : false);
            setEnableDoNotTrack((QString(query.value(2).toByteArray()).compare("true") == 0) ? true : false);
            setBrowsingDataSize(query.value(3).toInt(), query.value(4).toInt(), query.value(5).toInt(), query.value(6).toInt(), query.value(7).toInt());
        }

        loadCookieData();
    }
    else
    {
        qDebug(query.lastError().text().toLatin1());
        QString statementA2 = QLatin1String("CREATE TABLE browsingdatasettings(cookieoption VARCHAR(256) NOT NULL, deleteonclose VARCHAR(64) NOT NULL, donottrack VARCHAR(64) NOT NULL, cookieAndDatasize SMALLINT UNSIGNED NOT NULL,"
                                            "cachedDatasize SMALLINT UNSIGNED NOT NULL, browsingHistorysize SMALLINT UNSIGNED NOT NULL, passwordDatasize SMALLINT UNSIGNED NOT NULL, paymentDatasize SMALLINT UNSIGNED NOT NULL)");
        if (!query.exec(statementA2))
            qDebug(query.lastError().text().toLatin1());
        else
        {
            QString statementA3 = QLatin1String("INSERT INTO browsingdatasettings(cookieoption, deleteonclose, donottrack, cookieAndDatasize, cachedDatasize,"
                                                "browsingHistorysize, passwordDatasize, paymentDatasize)"
                                                "VALUES(:cookieoption, :deleteonclose, :donottrack, :cookieAndDatasize, :cachedDatasize,"
                                                ":browsingHistorysize, :passwordDatasize, :paymentDatasize)");
            query.prepare(statementA3);
            query.bindValue(":cookieoption", QString("enableCookies").toLatin1());
            query.bindValue(":deleteonclose", QString("false").toLatin1());
            query.bindValue(":donottrack", QString("true").toLatin1());
            query.bindValue(":cookieAndDatasize", 0);
            query.bindValue(":cachedDatasize", 0);
            query.bindValue(":browsingHistorysize", 0);
            query.bindValue(":passwordDatasize", 0);
            query.bindValue(":paymentDatasize", 0);
            if (!query.exec())
                qDebug(query.lastError().text().toLatin1());

            setupBrowsingDataSettings();
        }
    }
    query.finish();
}

void OSettingsCore::setupSearchEngineSettings()
{
    QString statementA1 = QLatin1String("SELECT enginename, engineshortcut, engineurl FROM searchenginelist");
    if (query.exec(statementA1))
    {
        while (query.next())
            loadCustomSearchEngine(QString(query.value(0).toByteArray()), QString(query.value(1).toByteArray()), QString(query.value(2).toByteArray()));

        QString statementB1 = QLatin1String("SELECT selectedsearchengine FROM searchenginesettings");
        if (query.exec(statementB1))
        {
            while (query.next())
                setSelectedDefaultSearchEngine(QString(query.value(0).toByteArray()));
        }
        else
            qDebug(query.lastError().text().toLatin1());
    }
    else
    {
        qDebug(query.lastError().text().toLatin1());
        QString statementA2 = QLatin1String("CREATE TABLE searchenginesettings(selectedsearchengine VARCHAR(256) NOT NULL)");
        if (!query.exec(statementA2))
            qDebug(query.lastError().text().toLatin1());
        else
        {
            QString statementA3 = QLatin1String("INSERT INTO searchenginesettings (selectedsearchengine) VALUES(:selectedsearchengine)");
            query.prepare(statementA3);
            query.bindValue(":selectedsearchengine", QString("Google").toLatin1());
            if (!query.exec())
                qDebug(query.lastError().text().toLatin1());

            QString statementB2 = QLatin1String("CREATE TABLE searchenginelist(enginename VARCHAR(256) NOT NULL,"
                                                "engineshortcut VARCHAR(1024) NOT NULL, engineurl VARCHAR(2048) NOT NULL)");
            if (!query.exec(statementB2))
                qDebug(query.lastError().text().toLatin1());

            setupSearchEngineSettings();
        }
    }
    query.finish();
}

void OSettingsCore::setupLanguageSettings()
{
    QString statementA1 = QLatin1String("SELECT uilanguage, spellchecklanguage FROM languagesettings");
    if (query.exec(statementA1))
    {
        while (query.next())
        {
            setSelectedUILanguage(QString(query.value(0).toByteArray()));
            setSelectedSpellCheckLanguage(QString(query.value(1).toByteArray()));
        }
    }
    else
    {
        qDebug(query.lastError().text().toLatin1());
        QString statementA2 = QLatin1String("CREATE TABLE languagesettings(uilanguage VARCHAR(256) NOT NULL, spellchecklanguage VARCHAR(256) NOT NULL)");
        if (!query.exec(statementA2))
            qDebug(query.lastError().text().toLatin1());
        else
        {
            QString statementA3 = QLatin1String("INSERT INTO languagesettings (uilanguage, spellchecklanguage) VALUES(:uilanguage, :spellchecklanguage)");
            query.prepare(statementA3);
            query.bindValue(":uilanguage", QString("English").toLatin1());
            query.bindValue(":spellchecklanguage", QString("English").toLatin1());
            if (!query.exec())
                qDebug(query.lastError().text().toLatin1());

            setupLanguageSettings();
        }
    }
    query.finish();
}

void OSettingsCore::setupDownloadSettings()
{
    QString statementA1 = QLatin1String("SELECT downloadlocation, askingoption FROM downloadsettings");
    if (query.exec(statementA1))
    {
        while (query.next())
        {
            normalWebProfile->setDownloadPath(QString(query.value(0).toByteArray()));
            privateWebProfile->setDownloadPath(QString(query.value(0).toByteArray()));
            setDownloadLocation();
            setAskDownloadLocation((QString(query.value(1).toByteArray()).compare("true") == 0) ? true : false);
        }
    }
    else
    {
        qDebug(query.lastError().text().toLatin1());
        QString statementA2 = QLatin1String("CREATE TABLE downloadsettings(downloadlocation VARCHAR(2048) NOT NULL, askingoption VARCHAR(64) NOT NULL)");
        if (!query.exec(statementA2))
            qDebug(query.lastError().text().toLatin1());
        else
        {
            QString statementA3 = QLatin1String("INSERT INTO downloadsettings (downloadlocation, askingoption) VALUES(:downloadlocation, :askingoption)");
            query.prepare(statementA3);
            query.bindValue(":downloadlocation", QString(normalWebProfile->downloadPath()).toLatin1());
            query.bindValue(":askingoption", QString("false").toLatin1());
            if (!query.exec())
                qDebug(query.lastError().text().toLatin1());

            setupDownloadSettings();
        }
    }
    query.finish();
}

void OSettingsCore::setupAssistantSettings()
{
    QString statementA1 = QLatin1String("SELECT assistantenablestate FROM assistantsettings");
    if (query.exec(statementA1))
    {
        while (query.next())
            setEnableOlolanAssistant((QString(query.value(0).toByteArray()).compare("true") == 0) ? true : false);
    }
    else
    {
        qDebug(query.lastError().text().toLatin1());
        QString statementA2 = QLatin1String("CREATE TABLE assistantsettings(assistantenablestate VARCHAR(64) NOT NULL)");
        if (!query.exec(statementA2))
            qDebug(query.lastError().text().toLatin1());
        else
        {
            QString statementA3 = QLatin1String("INSERT INTO assistantsettings (assistantenablestate) VALUES(:assistantenablestate)");
            query.prepare(statementA3);
            query.bindValue(":assistantenablestate", QString("true").toLatin1());
            if (!query.exec())
                qDebug(query.lastError().text().toLatin1());

            setupAssistantSettings();
        }
    }
    query.finish();
}

void OSettingsCore::setupDefaultBrowserSettings()
{
    QString statementA1 = QLatin1String("SELECT defaultbrowserstate FROM defaultbrowsersettings");
    if (query.exec(statementA1))
    {
        while (query.next())
            setDefaultBrowserState((QString(query.value(0).toByteArray()).compare("true") == 0) ? true : false);
    }
    else
    {
        qDebug(query.lastError().text().toLatin1());
        QString statementA2 = QLatin1String("CREATE TABLE defaultbrowsersettings(defaultbrowserstate VARCHAR(64) NOT NULL)");
        if (!query.exec(statementA2))
            qDebug(query.lastError().text().toLatin1());
        else
        {
            QString statementA3 = QLatin1String("INSERT INTO defaultbrowsersettings (defaultbrowserstate) VALUES(:defaultbrowserstate)");
            query.prepare(statementA3);
            query.bindValue(":defaultbrowserstate", QString("false").toLatin1());
            if (!query.exec())
                qDebug(query.lastError().text().toLatin1());

            setupDefaultBrowserSettings();
        }
    }
    query.finish();
}

void OSettingsCore::setupSystemSettings()
{
    QString statementA1 = QLatin1String("SELECT canvas, webgl FROM systemsettings");
    if (query.exec(statementA1))
    {
        while (query.next())
        {
            normalWebSettings->setAttribute(QWebEngineSettings::WebGLEnabled, ((QString(query.value(0).toByteArray()).compare("true") == 0) ? true : false));
            normalWebSettings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, ((QString(query.value(1).toByteArray()).compare("true") == 0) ? true : false));
            setUseCanvasAcceleration();
            setUseWebGLAcceleration();
        }
    }
    else
    {
        qDebug(query.lastError().text().toLatin1());
        QString statementA2 = QLatin1String("CREATE TABLE systemsettings(canvas VARCHAR(64) NOT NULL, webgl VARCHAR(64) NOT NULL)");
        if (!query.exec(statementA2))
            qDebug(query.lastError().text().toLatin1());
        else
        {
            QString statementA3 = QLatin1String("INSERT INTO systemsettings (canvas, webgl) VALUES(:canvas, :webgl)");
            query.prepare(statementA3);
            query.bindValue(":canvas", QString("true").toLatin1());
            query.bindValue(":webgl", QString("true").toLatin1());
            if (!query.exec())
                qDebug(query.lastError().text().toLatin1());

            setupSystemSettings();
        }
    }
    query.finish();
}

bool OSettingsCore::isSynchronized()
{
    return synchronized;
}

void OSettingsCore::updateSyncState(bool state, QString username, QString password, QString login)
{
    if (!settingsDatabase.isOpenError())
    {
        synchronized = state;
        QString statement = QLatin1String("UPDATE accountsettings SET synchronizestate = :synchronizestate, username = :username, userpassword = :userpassword, login = :login");
        query.prepare(statement);

        if (state)
        {
            query.bindValue(":synchronizestate", QString("Signed in").toLatin1());
            query.bindValue(":username", username.toLatin1());
            query.bindValue(":userpassword", password.toLatin1());
            query.bindValue(":login", login.toLatin1());
            setSyncState(QString("Signed in"), username);
            qDebug() << " OSettingsCore::updateSyncState : " << username << " * " << password << " * " << login;
        }
        else
        {
            query.bindValue(":synchronizestate", QString("Signed out").toLatin1());
            query.bindValue(":username", QString("Guest User").toLatin1());
            query.bindValue(":userpassword", QString("guestpassword").toLatin1());
            query.bindValue(":login", QString("guestuser@ololan.com").toLatin1());
            setSyncState(QString("Signed out"), QString("Guest User"));
        }

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::setupIndexList()
{
    for (int i = 1; i < 102410; i++)
    {
        if (!specificPageUsedIndexList.contains(i))
            specificPageUsedIndexList.append(i);

        if (!credentialUsedIndexList.contains(i))
            credentialUsedIndexList.append(i);

        if (!paymentMethodUsedIndexList.contains(i))
            paymentMethodUsedIndexList.append(i);
    }
}

const int OSettingsCore::generateSpecificPageIndex()
{
    const int index = specificPageUsedIndexList.first();
    specificPageUsedIndexList.removeFirst();
    return index;
}

const int OSettingsCore::generateCredentialIndex()
{
    const int index = credentialUsedIndexList.first();
    credentialUsedIndexList.removeFirst();
    return index;
}

const int OSettingsCore::generatePaymentMethodIndex()
{
    const int index = paymentMethodUsedIndexList.first();
    paymentMethodUsedIndexList.removeFirst();
    return index;
}

void OSettingsCore::removeInPrivateCookie(const QNetworkCookie &cookie)
{
    if (blockAllTrackers)
        privateWebProfile->cookieStore()->deleteCookie(cookie);
    /*else if (blockInPrivateBrowsing && !cookie.isSecure())
        privateWebProfile->cookieStore()->deleteCookie(cookie);*/
}

void OSettingsCore::removeGeneralCookie(const QNetworkCookie &cookie)
{
    if (blockAllTrackers){qDebug() << "deleted all trackers";
        normalWebProfile->cookieStore()->deleteCookie(cookie);
    }
    /*else if (blockInPrivateBrowsing)
    {
        if (!cookie.isSecure()){qDebug() << "deleted insecure trackers";
            normalWebProfile->cookieStore()->deleteCookie(cookie);
        }
        else
            addCookie(cookie);
    }*/
    else
        addCookie(cookie);
}

void OSettingsCore::fillCookiesList(const QNetworkCookie &cookie)
{
    if (!cookieList.contains(cookie))
        cookieList.append(cookie);
}

void OSettingsCore::addCookie(QNetworkCookie cookie)
{
    if (!cookieList.contains(cookie))
        cookieList.append(cookie);

    if (isOpened)
    {
        QString script = QLatin1String("updateCookiesList();");
        settingsWebView->page()->runJavaScript(script);
        for (int i = 0; i < cookieList.count(); i++)
        {
            int count = 1;
            for (int j = i+1; j < cookieList.count(); j++)
                if ((cookieList.at(i).value() == cookieList.at(j).value()) || (cookieList.at(i).name() == cookieList.at(j).name()))
                    count++;

            QString script = QLatin1String("loadCookieData('%1', '%2');").arg((!cookieList.at(i).value().isEmpty() ? cookieList.at(i).value() : cookieList.at(i).name()), QString::number(count));
            settingsWebView->page()->runJavaScript(script);
        }
    }
}

const QStringList OSettingsCore::getBrowsingDataSize()
{
    QStringList result;
    QByteArray spaceA;
    QDataStream streamA(&spaceA, QIODevice::WriteOnly);
    for (int i = 0; i < cookieList.size(); i++)
        streamA.writeRawData((const char *)&cookieList.at(i), sizeof(cookieList.at(i)));
    result << QString::number(spaceA.size());

    qint64 cacheSize = calculateSize(normalWebProfile->cachePath().append("/Cache_Data"));
    result << QString::number(int(cacheSize));
    
    OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
    result << QString::number(parentPtr->getHistoryManager()->getHistoryDataSize());   

    QByteArray spaceB;
    QDataStream streamB(&spaceB, QIODevice::WriteOnly);
    streamB << credentialList;
    if (!credentialList.empty())
        result << QString::number(spaceB.size());
    else
        result << QString::number(0);

    result << QString::number(paymentMethodList.size());
    return result;
}

qint64 OSettingsCore::calculateSize(const QString &path) {
    qint64 totalSize = 0;
    QDir dir(path);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo &fileInfo : fileList)
        if (fileInfo.isDir())
            totalSize += calculateSize(fileInfo.filePath());
        else
            totalSize += fileInfo.size();

    return totalSize;
}

void OSettingsCore::updateBrowsingData()
{
    const QStringList dataSize = getBrowsingDataSize();
    const int cookie = dataSize.at(0).toInt();
    const int cache = dataSize.at(1).toInt();
    const int history = dataSize.at(2).toInt();
    const int password = dataSize.at(3).toInt();
    const int payment = dataSize.at(4).toInt();

    QString statement = QLatin1String("UPDATE browsingdatasettings SET cookieAndDatasize = :cookieAndDatasize , cachedDatasize = :cachedDatasize,"
                                      "browsingHistorysize = :browsingHistorysize, passwordDatasize = :passwordDatasize, paymentDatasize = :paymentDatasize");
    query.prepare(statement);
    query.bindValue(":cookieAndDatasize", cookie);
    query.bindValue(":cachedDatasize", cache);
    query.bindValue(":browsingHistorysize", history);
    query.bindValue(":passwordDatasize", password);
    query.bindValue(":paymentDatasize", payment);

    if (!query.exec())
        qDebug(query.lastError().text().toLatin1());
    query.finish();
}

QWebEngineProfile *OSettingsCore::getWebProfile()
{
    return normalWebProfile;
}

QWebEngineProfile *OSettingsCore::getPrivateWebProfile()
{
    return privateWebProfile;
}

const bool OSettingsCore::getEnableGeolocation()
{
    bool state = true;
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT geolocation FROM privacysecuritysettings");
        if (query.exec(statement))
            while (query.next())
                state = (QString(query.value(0).toByteArray()).compare("true") == 0) ? true : false;
    }
    return state;
}

const bool OSettingsCore::getPasswordSaving()
{
    bool state = true;
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT savepassword FROM personaldatasettings");
        if (query.exec(statement))
            while (query.next())
                state = (QString(query.value(0).toByteArray()).compare("true") == 0) ? true : false;
    }
    return state;
}

const bool OSettingsCore::getSiteAutoSignIn()
{
    bool state = true;
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT autosignin FROM personaldatasettings");
        if (query.exec(statement))
            while (query.next())
                state = (QString(query.value(0).toByteArray()).compare("true") == 0) ? true : false;
    }
    return state;
}

const bool OSettingsCore::getPaymentSaveAndFill()
{
    bool state = true;
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT autosavefill FROM personaldatasettings");
        if (query.exec(statement))
            while (query.next())
                state = (QString(query.value(0).toByteArray()).compare("true") == 0) ? true : false;
    }
    return state;
}

const bool OSettingsCore::getDoNotTrack()
{
    bool state = false;
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT donottrack FROM browsingdatasettings");
        if (query.exec(statement))
            while (query.next())
                state = (QString(query.value(0).toByteArray()).compare("true") == 0) ? true : false;
    }
    return state;
}

const bool OSettingsCore::getEnableAssistant()
{
    bool state = true;
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT assistantenablestate FROM assistantsettings");
        if (query.exec(statement))
            while (query.next())
                state = (QString(query.value(0).toByteArray()).compare("true") == 0) ? true : false;
    }
    return state;
}

const bool OSettingsCore::getAskDownloadPath()
{
    bool state = false;
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT askingoption FROM downloadsettings");
        if (query.exec(statement))
            while (query.next())
                state = (QString(query.value(0).toByteArray()).compare("true") == 0) ? true : false;
    }
    return state;
}

const bool OSettingsCore::getBookmarkBarVisibile()
{
    bool state = true;
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT bookmarkbarstate FROM appearancesettings");
        if (query.exec(statement))
            while (query.next())
                state = (QString(query.value(0).toByteArray()).compare("true") == 0) ? true : false;
    }
    return state;
}

const double OSettingsCore::getWebPageZoom()
{
    double zoomValue = 1.0;
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT pagezoom FROM appearancesettings");
        if (query.exec(statement))
            while (query.next())
                zoomValue = QString(query.value(0).toByteArray()).toDouble()/100.0;
    }
    return zoomValue;
}

const QString OSettingsCore::getOlolanTheme()
{
    QString theme = "Light gray";
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT themecolor FROM appearancesettings");
        if (query.exec(statement))
            while (query.next())
                theme = QString(query.value(0).toByteArray());
    }
    return theme;
}

const QString OSettingsCore::getUsername()
{
    QString accountName = "Guest User";
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT username FROM accountsettings");
        if (query.exec(statement))
            while (query.next())
                accountName = QString(query.value(0).toByteArray());
    }
    return accountName;
}

const QString OSettingsCore::getEmail()
{
    QString accountEmail = "guestuser@ololan.com";
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT login FROM accountsettings");
        if (query.exec(statement))
            while (query.next())
                accountEmail = QString(query.value(0).toByteArray());
    }
    return accountEmail;
}

const QString OSettingsCore::getAccountPassword()
{
    QString accountPassword = "guestpassword";
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT userpassword FROM accountsettings");
        if (query.exec(statement))
            while (query.next())
                accountPassword = QString(query.value(0).toByteArray());
    }
    return accountPassword;
}

const bool OSettingsCore::getLoggedStatus()
{
    QString value = "";
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT synchronizestate FROM accountsettings");
        if (query.exec(statement))
            while (query.next())
                value = QString(query.value(0).toByteArray());
    }

    if (value.compare(QString("Signed in")) == 0)
        return true;
    else
        return false;
}

const QString OSettingsCore::getSearchEngine()
{
    QString searchEngine = "Google";
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT selectedsearchengine FROM searchenginesettings");
        if (query.exec(statement))
            while (query.next())
                searchEngine = QString(query.value(0).toByteArray());

        if ((searchEngine.compare("Google") != 0) && (searchEngine.compare("Bing") != 0) &&
            (searchEngine.compare("Yahoo") != 0) && (searchEngine.compare("DuckDuckGo") != 0))
        {
            QString statementA = QLatin1String("SELECT engineurl FROM searchenginelist WHERE enginename = :enginename");
            query.prepare(statementA);
            query.bindValue(":enginename", searchEngine.toLatin1());
            searchEngine = searchEngine.append("#*#");

            if (query.exec())
                while (query.next())
                    searchEngine = searchEngine.append(QString(query.value(0).toByteArray()));
        }
    }
    return searchEngine;
}

const QString OSettingsCore::getStartupOption()
{
    QString option = "start page";
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT startpageoption FROM startupsettings");
        if (query.exec(statement))
            while (query.next())
                option = QString(query.value(0).toByteArray());
    }
    return option;
}

QList<QString> OSettingsCore::getSpecificPageList()
{
    QList<QString> pages;
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT pageindex, specificpagetitle, specificpageurl FROM specificpage");
        if (query.exec(statement))
            while (query.next())
                pages.append(QString(query.value(2).toByteArray()));
    }
    return pages;
}

const bool OSettingsCore::getEnableCamera()
{
    bool state = true;
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT camera FROM privacysecuritysettings");
        if (query.exec(statement))
            while (query.next())
                state = (QString(query.value(0).toByteArray()).compare("true") == 0) ? true : false;
    }
    return state;
}

const bool OSettingsCore::getEnableMicrophone()
{
    bool state = true;
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT microphone FROM privacysecuritysettings");
        if (query.exec(statement))
            while (query.next())
                state = (QString(query.value(0).toByteArray()).compare("true") == 0) ? true : false;
    }
    return state;
}

void OSettingsCore::manageBookmarkBar()
{
    if (bookmarkBarVisibile)
    {
        showBookmarkBar(QString("false"));
        setBookmarkBarState(false);
    }
    else
    {
        showBookmarkBar(QString("true"));
        setBookmarkBarState(true);
    }
}

QList<QStringList> OSettingsCore::getCredentialList()
{
    return credentialList;
}

QList<QStringList> OSettingsCore::getPaymentMethodList()
{
    return paymentMethodList;
}

void OSettingsCore::selectAboutOlolan()
{
    settingsWebView->page()->runJavaScript(QLatin1String("selectAboutOlolan();"));
    setAboutRequested(false);

    if (updatesAvailable)
    {
        QString answer = "New version available. Click to download";
        QString responseScript = QLatin1String("showUpdateCheckingInfo('%1', '%2');").arg(answer, "true");
        settingsWebView->page()->runJavaScript(responseScript);
    }
}

void OSettingsCore::setAboutRequested(bool value)
{
    aboutResquested = value;
}

void OSettingsCore::sendUsageData(QString state)
{
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE accountsettings SET sendstatisticstate = :sendstatisticstate");
        query.prepare(statement);
        query.bindValue(":sendstatisticstate", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::setSendUsageData(bool state)
{
    if (isOpened)
    {
        QString script = QLatin1String("setSendUsageData(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::enableCamera(QString state)
{
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE privacysecuritysettings SET camera = :camera");
        query.prepare(statement);
        query.bindValue(":camera", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();

        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        if (state.compare("true") == 0)
            parentPtr->setEnableCamera(true);
        else if (state.compare("false") == 0)
            parentPtr->setEnableCamera(false);
    }
}

void OSettingsCore::setEnableCamera(bool state)
{
    if (isOpened)
    {
        QString script = QLatin1String("setEnableCamera(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::enableMicrophone(QString state)
{
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE privacysecuritysettings SET microphone = :microphone");
        query.prepare(statement);
        query.bindValue(":microphone", state.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();

        OAppCore *parentPtr = qobject_cast<OAppCore*>(parent());
        if (state.compare("true") == 0)
            parentPtr->setEnableMicrophone(true);
        else if (state.compare("false") == 0)
            parentPtr->setEnableMicrophone(false);
    }
}

void OSettingsCore::setEnableMicrophone(bool state)
{
    if (isOpened)
    {
        QString script = QLatin1String("setEnableMicrophone(%1);").arg((state ? "true" : "false"));
        settingsWebView->page()->runJavaScript(script);
    }
}

const bool OSettingsCore::getSendStatisticState()
{
    bool state = true;
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("SELECT sendstatisticstate FROM accountsettings");
        if (query.exec(statement))
            while (query.next())
                state = query.value(0).toBool();
    }
    return state;
}

void OSettingsCore::saveSearchEngine(QString name, QString shortCut, QString url)
{
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("INSERT INTO searchenginelist (enginename, engineshortcut, engineurl)"
                                          "VALUES(:enginename, :engineshortcut, :engineurl)");
        query.prepare(statement);
        query.bindValue(":enginename", name.toLatin1());
        query.bindValue(":engineshortcut", shortCut.toLatin1());
        query.bindValue(":engineurl", url.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::editSearchEngine(QString oldName, QString newName, QString shortCut, QString url)
{
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("UPDATE searchenginelist SET enginename = :enginename, engineshortcut = :engineshortcut,"
                                          "engineurl = :engineurl WHERE enginename = :oldname");
        query.prepare(statement);
        query.bindValue(":oldname", oldName.toLatin1());
        query.bindValue(":enginename", newName.toLatin1());
        query.bindValue(":engineshortcut", shortCut.toLatin1());
        query.bindValue(":engineurl", url.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::deleteSearchEngine(QString name)
{
    if (!settingsDatabase.isOpenError())
    {
        QString statement = QLatin1String("DELETE FROM searchenginelist WHERE enginename = :enginename");
        query.prepare(statement);
        query.bindValue(":enginename", name.toLatin1());

        if (!query.exec())
            qDebug(query.lastError().text().toLatin1());
        query.finish();
    }
}

void OSettingsCore::loadCustomSearchEngine(QString title, QString shortcut, QString url)
{
    if (isOpened)
    {
        QString script = QLatin1String("loadCustomSearchEngine('%1', '%2', '%3');").arg(title, shortcut, url);
        settingsWebView->page()->runJavaScript(script);
    }
}

void OSettingsCore::sendStatisticsToOlolan()
{
    //to be done when server is setup
    //send some simple statistics and data like history, bookmarks, assistant speechs to Ololan server
}

const QString OSettingsCore::getDataSize(const int size)
{
    quint64 range = 1024;
    quint64 totalSize = (quint64)size;
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
    return sizeToString;
}

void OSettingsCore::saveDatabase()
{
    if (normalWebProfile->persistentCookiesPolicy() == QWebEngineProfile::NoPersistentCookies)
        normalWebProfile->cookieStore()->deleteAllCookies();

    privateWebProfile->cookieStore()->deleteAllCookies();
    settingsDatabase.close();
}

void OSettingsCore::clearCacheFolders(const QString &folderPath)
{
    QDir dir(folderPath);
    QStringList entryList = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &entry : entryList)
    {qDebug() << "items : " << entry;
        const QString entryPath = folderPath + QDir::separator() + entry;
        if (QFileInfo(entryPath).isDir())
        {
            clearCacheFolders(entryPath);
            if (!dir.rmdir(entry))
                qDebug() << "folder : " << entry << "not deleted";
            else
                qDebug() << "folder : " << entry << "deleted";
        }
        else
        {
            if (!dir.remove(entry))
                qDebug() << "file : " << entry << "not deleted";
            else
                qDebug() << "file : " << entry << "deleted";
        }
    }
}
