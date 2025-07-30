#include "owebpage.h"
#include "oappcore.h"
#include "customwindow.h"
#include "otabpage.h"
#include "osettingscore.h"
#include <QBuffer>
#include <QIcon>
#include <QAuthenticator>

OWebPage::OWebPage(QWebEngineProfile *profile, QObject *parent)
    :QWebEnginePage(profile, parent)
{
    geolocationEnable = false;
    savePassword = true;
    websiteAutoSignIn = true;
    connect(this, &OWebPage::authenticationRequired, this, &OWebPage::credentialsAutoComplete);
    connect(this, &OWebPage::featurePermissionRequested, this, &OWebPage::processFeatureRequest);
}

void OWebPage::enableGeolaction(bool value)
{
    geolocationEnable = value;
}

void OWebPage::processFeatureRequest(const QUrl &securityOrigin, QWebEnginePage::Feature feature)
{
    if (feature == QWebEnginePage::Geolocation)
    {
        if (geolocationEnable)
            emit askFeaturePermission(securityOrigin.host(), "location", securityOrigin);
        else
            setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionDeniedByUser);
    }
    else if (feature == QWebEnginePage::MediaVideoCapture)
    {
        if (enableCamera)
            emit askFeaturePermission(securityOrigin.host(), "camera", securityOrigin);
        else
            setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionDeniedByUser);
    }
    else if (feature == QWebEnginePage::MediaAudioCapture)
    {
        if (enableMicrophone)
            emit askFeaturePermission(securityOrigin.host(), "microphone", securityOrigin);
        else
            setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionDeniedByUser);
    }
    else if (feature == QWebEnginePage::MediaAudioVideoCapture)
    {
        if (enableMicrophone && enableCamera)
            emit askFeaturePermission(securityOrigin.host(), "webcam", securityOrigin);
        else
            setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionDeniedByUser);
    }
}

void OWebPage::setPasswordSaving(bool value)
{
    savePassword = value;
}

void OWebPage::credentialsAutoComplete(const QUrl &requestUrl, QAuthenticator *authenticator)
{
    OTabPage *pTabPage = static_cast<OTabPage*>(parent());
    OAppCore *parentPtr = static_cast<OAppCore*>(pTabPage->getParentWindow()->getParentApplication());
    QList<QStringList> credentialList = parentPtr->getSettingsManager()->getCredentialList();

    if (websiteAutoSignIn)
    {
        for (int i = 0; i < credentialList.count(); i++)
            if (credentialList.at(i).at(2).compare(requestUrl.toEncoded()) == 0)
            {
                authenticator->realm();
                authenticator->setUser(credentialList.at(i).at(3));
                authenticator->setPassword(credentialList.at(i).at(4));
                break;
            }
    }
}

void OWebPage::setSiteAutoSignIn(bool value)
{
    websiteAutoSignIn = value;
}

void OWebPage::saveCredentials(const QUrl &webUrl)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);

    if (buffer.open(QIODevice::WriteOnly))
    {
        if(!icon().pixmap(16, 16).save(&buffer, "PNG", 0))
            qDebug("error saving icon filemlo");
    }
    else
        qDebug("buffer failed to open");

    if (buffer.isOpen())
        buffer.close();

    if (!webUrl.userName().isEmpty() && !webUrl.password().isEmpty())
    {
        OTabPage *pTabPage = static_cast<OTabPage*>(parent());
        OAppCore *parentPtr = static_cast<OAppCore*>(pTabPage->getParentWindow()->getParentApplication());
        parentPtr->getSettingsManager()->saveCredentials(bytes.toBase64(), webUrl.toEncoded(), webUrl.userName(), webUrl.password());
    }
}

void OWebPage::setEnableMicrophone(bool state)
{
    enableMicrophone = state;
}

void OWebPage::setEnableCamera(bool state)
{
    enableCamera = state;
}
