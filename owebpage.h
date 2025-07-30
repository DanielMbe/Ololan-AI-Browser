#ifndef OWEBPAGE_H
#define OWEBPAGE_H

#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QByteArray>
#include <QObject>

class OWebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    explicit OWebPage(QWebEngineProfile *profile, QObject *parent = nullptr);
    void enableGeolaction(bool value);
    void setPasswordSaving(bool value);
    void setupCredentialList(QList<QStringList> credential);
    void setSiteAutoSignIn(bool value);
    void setEnableMicrophone(bool state);
    void setEnableCamera(bool state);

signals:
    void askFeaturePermission(const QString website, const QString permission, const QUrl securityOrigin);

public slots:
    void processFeatureRequest(const QUrl &securityOrigin, QWebEnginePage::Feature feature);
    void credentialsAutoComplete(const QUrl &requestUrl, QAuthenticator *authenticator);
    void saveCredentials(const QUrl &webUrl);

private:
    bool geolocationEnable;
    bool savePassword;
    bool websiteAutoSignIn;
    bool enableMicrophone;
    bool enableCamera;
};

#endif // OWEBPAGE_H
