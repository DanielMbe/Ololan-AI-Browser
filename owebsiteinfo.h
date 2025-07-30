#ifndef OWEBSITEINFO_H
#define OWEBSITEINFO_H

#include <QObject>
#include <QMenu>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>

class OWebsiteInfo : public QMenu
{
    Q_OBJECT
public:
    OWebsiteInfo(QWidget *parent = nullptr);
    void setupView();
    void setLightGrayTheme();
    void setLightTurquoiseTheme();
    void setLightBrownTheme();
    void setDarkGrayTheme();
    void setDarkTurquoiseTheme();
    void setDarkBrownTheme();
    void setPrivateTheme();
    void updateWebsiteInfo(const QString urlHost, bool isSecure);
    void updatePermissionInfo(const QString website, const QString permission, const QUrl security);
    void updateAuthentificationInfo(const QString user, const QUrl url);
    void setCredentialsUrl(const QUrl url);
    const QUrl getCredentialsUrl();

public slots:
    void saveCredentials();
    void allowPermission();
    void blockPermission();

protected:
    void hideEvent(QHideEvent *event) override;

private:
    QStackedWidget *panel;
    QWidget *widgetA;
    QWidget *widgetB;
    QWidget *widgetC;
    QLabel *website;
    QLabel *websiteIcon;
    QLabel *securityInfo;
    QLabel *securityInfoIcon;
    QLabel *webPermission;
    QLabel *username;
    QLabel *password;
    QPushButton *allow;
    QPushButton *block;
    QPushButton *savePwd;
    QPushButton *cancelPwd;
    QString ololanTheme;
    QString webRequest;
    QUrl credentialsUrl;
    QUrl securityOrigin;
};

#endif // OWEBSITEINFO_H
