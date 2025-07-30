#ifndef OSETTINGSCORE_H
#define OSETTINGSCORE_H

#include <QObject>
#include <QWebChannel>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include "owebview.h"

class OSettingsCore : public QObject
{
    Q_OBJECT
public:
    explicit OSettingsCore(QObject *parent = nullptr);
    //web app functions
    void setCustomFontSizeValue();
    void setMinimumFontSizeValue();
    void setSyncState(QString state, QString username);
    void setSelectedThemeColor(QString themeColor);
    void setBookmarkBarState(bool state);
    void setPageZoom(QString zoomValue);
    void setSelectedFontSize(int fontValue);
    void setSelectedStandardFont();
    void setSelectedSerifFont();
    void setSelectedSansSerifFont();
    void setSelectedFixedFont();
    void setSelectedStartPageOption(QString startOption);
    void setAllowJavascript();
    void setAllowJSCopyPaste();
    void setAllowJSPopUps();
    void setEnablePlugins();
    void setAllowInsecureContents();
    void setEnableInsecureGeolocation();
    void setEnableGeolocation(bool state);
    void setEnableDnsPrefetch();
    void setEnableInternalPDFViewer();
    void setSelectedWebProtection(QString protection);
    void setAskToSavePassword(bool state);
    void setWebsiteAutoSignIn(bool state);
    void setAutoSavePaymentMethods(bool state);
    void setDeleteCookiesOnClose(bool state);
    void setEnableDoNotTrack(bool state);
    void setSelectedDefaultSearchEngine(QString defaultSearchEngine);
    void setSelectedUILanguage(QString uiLanguageValue);
    void setSelectedSpellCheckLanguage(QString spellCheckLanguageValue);
    void setDownloadLocation();
    void setAskDownloadLocation(bool state);
    void setEnableOlolanAssistant(bool state);
    void setDefaultBrowserState(bool state);
    void setUseCanvasAcceleration();
    void setUseWebGLAcceleration();
    void setBrowsingDataSize(int cookieAndData, int cachedData, int browsingHistory, int passwordData, int paymentData);
    void setSelectedCookiesPolicy(QString policy);
    void loadSpecificPage(int index, QString title, QString url);
    void loadCredentials(int index, QByteArray icon, QString webAddress, QString loginAddress, QString password);
    void loadPaymentMethods(int index, QString cardName, QString cardNumber, QString cardExpiration);
    void setSendUsageData(bool state);
    void setEnableMicrophone(bool state);
    void setEnableCamera(bool state);
    void loadCustomSearchEngine(QString title, QString shortcut, QString url);
    void checkForUpdates();
    QWebEngineProfile *getWebProfile();
    QWebEngineProfile *getPrivateWebProfile();

    //core functions
    void setupAccountSettings();
    void setupAppearanceSettings();
    void setupStartupSettings();
    void setupPrivacySecuritySettings();
    void setupPersonalDataSettings();
    void setupBrowsingDataSettings();
    void setupSearchEngineSettings();
    void setupLanguageSettings();
    void setupDownloadSettings();
    void setupAssistantSettings();
    void setupDefaultBrowserSettings();
    void setupSystemSettings();
    void enableStrongProtection();
    void enableStandardProtection();
    void disableWebProtection();
    void updateSyncState(bool state, QString username, QString password, QString login);
    void setupIndexList();
    void addCookie(QNetworkCookie cookie);
    void selectAboutOlolan();
    void setAboutRequested(bool value);
    void sendStatisticsToOlolan();
    void updateBrowsingData();
    void saveDatabase();
    bool isSynchronized();
    bool isBookmarkBarVisible();
    void checkDefaultBrowserState();
    void clearCacheFolders(const QString &folderPath);
    const bool getEnableGeolocation();
    const bool getPasswordSaving();
    const bool getSiteAutoSignIn();
    const bool getPaymentSaveAndFill();
    const bool getDoNotTrack();
    const bool getEnableAssistant();
    const bool getAskDownloadPath();
    const bool getBookmarkBarVisibile();
    const bool getSendStatisticState();
    const bool getEnableCamera();
    const bool getEnableMicrophone();
    const bool getLoggedStatus();
    const int generateSpecificPageIndex();
    const int generateCredentialIndex();
    const int generatePaymentMethodIndex();
    const QStringList getBrowsingDataSize();
    const double getWebPageZoom();
    const QString getOlolanTheme();
    const QString getSearchEngine();
    const QString getWebProtection();
    const QString getDataSize(const int size);
    const QString getStartupOption();
    const QString getUsername();
    const QString getEmail();
    const QString getAccountPassword();
    QList<QString> getSpecificPageList();
    QList<QStringList> getCredentialList();
    QList<QStringList> getPaymentMethodList();
    qint64 calculateSize(const QString &path);

public slots:
    //web app functions
    void setCustomFontSize(int size);
    void setMinimumFontSize(int size);
    void synchronize(QString syncState);
    void selectTheme(QString themeColor);
    void showBookmarkBar(QString state);
    void selectPageZoom(QString zoomValue);
    void selectFontSize(QString fontSize);
    void selectStandardFont(QString standardFont);
    void selectSerifFont(QString serifFont);
    void selectSansSerifFont(QString sansSerifFont);
    void selectFixedFont(QString fixedFont);
    void selectStartPageOption(QString startOption);
    void addSpecificPage(QString title, QString url);
    void deleteSpecificPage(QString title, QString url, QString index);
    void saveSpecificPageEditing(QString index, QString title, QString url);
    void selectSpellCheckLanguage(QString language);
    void allowJavascript(QString state);
    void allowJSCopyPaste(QString state);
    void allowJSPopUps(QString state);
    void enablePlugins(QString state);
    void allowInsecureContents(QString state);
    void enableInsecureGeolocation(QString state);
    void enableGeolocation(QString state);
    void enableDnsPrefetch(QString state);
    void manageCertificates();
    void enableInternalPDFViewer(QString state);
    void selectWebProtection(QString protection);
    void askToSavePassword(QString state);
    void websiteAutoSignIn(QString state);
    void saveCredentials(QByteArray icon, QString webAddress, QString loginAddress, QString password);
    void deleteCredential(QString index);
    void saveCredentialPassword(QString index, QString password);
    void autoSavePaymentMethods(QString state);
    void savePaymentMethod(QString cardName, QString cardNumber, QString cardExpiration);
    void deletePaymentMethod(QString index);
    void savePaymentEditing(QString index, QString cardName, QString cardNumber, QString month, QString year);
    void selectCookiesPolicy(QString policy);
    void deleteCookiesOnClose(QString state);
    void enableDoNotTrack(QString state);
    void clearBrowsingData(QString cookieData, QString cachedData, QString browsingHistory, QString passwordData, QString paymentData);
    void loadCookieData();
    void deleteCookie(QString value, QString name);
    void clearAllCookies();
    void selectDefaultSearchEngine(QString searchEngine);
    void selectUILanguage(QString language);
    void selectDownloadLocation();
    void askDownloadLocation(QString state);
    void enableOlolanAssistant(QString state);
    void clearAssistantData();
    void setDefaultBrowser();
    void useCanvasAcceleration(QString state);
    void useWebGLAcceleration(QString state);
    void manageProxy();
    void restoreSettings();
    void updateOlolan();
    void removeInPrivateCookie(const QNetworkCookie &cookie);
    void removeGeneralCookie(const QNetworkCookie &cookie);
    void fillCookiesList(const QNetworkCookie &cookie);
    void sendUsageData(QString state);
    void enableCamera(QString state);
    void enableMicrophone(QString state);
    void saveSearchEngine(QString name, QString shortCut, QString url);
    void editSearchEngine(QString oldName, QString newName, QString shortCut, QString url);
    void deleteSearchEngine(QString name);
    void processServerResponse(QNetworkReply *serverResponse);
    void downloadAvailableUpdates();
    void deleteAccount();

    //core functions
    void loadSettings(bool status);
    void manageBookmarkBar();

private:
    OWebView *settingsWebView;
    QWebChannel *webChannel;
    QWebEngineProfile *normalWebProfile;
    QWebEngineProfile *privateWebProfile;
    QWebEngineSettings *normalWebSettings;
    QNetworkAccessManager *serverManager;
    QSqlQuery query;
    QSqlDatabase settingsDatabase;
    bool isOpened;
    bool synchronized;
    bool bookmarkBarVisibile;
    bool blockAllTrackers;
    bool blockInPrivateBrowsing;
    bool blockDangerousTrackers;
    bool aboutResquested;
    bool checkedForUpdates;
    bool updatesAvailable;
    QList<QStringList> specificPageList;
    QList<QStringList> credentialList;
    QList<QStringList> paymentMethodList;
    QList<QNetworkCookie> cookieList;
    QList<int> specificPageUsedIndexList;
    QList<int> credentialUsedIndexList;
    QList<int> paymentMethodUsedIndexList;
};

#endif // OSETTINGSCORE_H
