#ifndef OSMARTINPUT_H
#define OSMARTINPUT_H

#include "olibrary.h"
#include <QLineEdit>
#include "customtoolbutton.h"
#include "obookmarkwidget.h"
#include <QFocusEvent>
#include <QNetworkAccessManager>
#include <QWebEngineCertificateError>

class OInputCompleter;
class OHistoryCore;
class OWebsiteInfo;
class OWebPage;
class OAssistantWidget;
class OSmartInput : public QLineEdit
{
    Q_OBJECT

public:
    explicit OSmartInput(QWidget *parent = nullptr);
    void setSearchEngine(const QString engine);
    void setLightGrayTheme();
    void setLightTurquoiseTheme();
    void setLightBrownTheme();
    void setDarkGrayTheme();
    void setDarkTurquoiseTheme();
    void setDarkBrownTheme();
    void setPrivateTheme();
    void setFocusInLightGrayTheme();
    void setFocusInLightTurquoiseTheme();
    void setFocusInLightBrownTheme();
    void setFocusInDarkGrayTheme();
    void setFocusInDarkTurquoiseTheme();
    void setFocusInDarkBrownTheme();
    void setFocusInPrivateTheme();
    void setWebIconLightGrayTheme(UrlType urlType, bool focused);
    void setWebIconLightTurquoiseTheme(UrlType urlType, bool focused);
    void setWebIconLightBrownTheme(UrlType urlType, bool focused);
    void setWebIconDarkGrayTheme(UrlType urlType, bool focused);
    void setWebIconDarkTurquoiseTheme(UrlType urlType, bool focused);
    void setWebIconDarkBrownTheme(UrlType urlType, bool focused);
    void setWebIconPrivateTheme(UrlType urlType, bool focused);
    void setFocusOutLightGrayTheme();
    void setFocusOutLightTurquoiseTheme();
    void setFocusOutLightBrownTheme();
    void setFocusOutDarkGrayTheme();
    void setFocusOutDarkTurquoiseTheme();
    void setFocusOutDarkBrownTheme();
    void setFocusOutPrivateTheme();
    void setHistory(OHistoryCore *history);
    void updateCompleter(const QString &query, UrlType queryType);
    void showCompleter();
    bool isSecureUrl(const QString url);
    void setIconSecureStyle();
    void setIconUnsecureStyle();
    void saveUserCredentials();
    void setWebViewPage(OWebPage *page);
    void setPermission(const QString permission, bool accepted, const QUrl securityOrigin);
    void setSecurityTheme();
    void setUrlColor(const QString webUrl);
    void autoSuggest(const QString query);
    UrlType aboutTyping(const QString typing);
    OBookmarkWidget *getBookmarkWidget();
    OInputCompleter *getCompleter();

signals:
    void validated(const QString inputUrl);

public slots:
    void validateInput();
    void detectTyping(const QString &text);
    void completeHighlightedMatch(const QModelIndex &index);
    void processCompletion(const QModelIndex &index);
    void updateUrl(const QUrl &url);
    void uncheckDeletedBookmark(const QString url);
    void setPermissionInfo(const QString website, const QString permission, const QUrl securityOrigin);
    void autoCompleteSuggestion(QNetworkReply *reply);
    void certificateSecurity(const QWebEngineCertificateError &certificateError);
    void resetHasCertificateError(bool value);

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    CustomToolButton *searchIcon;
    CustomToolButton *securityButton;
    CustomToolButton *bookmarkButton;
    OInputCompleter *completerView;
    OHistoryCore *browserHistory;
    OBookmarkWidget *bookmarkMenu;
    OWebsiteInfo *websiteInfo;
    OWebPage *webViewPage;
    WindowType winType;
    QString searchEngine;
    QString searchEngineName;
    QString ololanTheme;
    QString newUrl;
    QString autoSuggestUrl;
    QNetworkAccessManager *networkManager;
    bool hasCertificateError;
};

#endif // OSMARTINPUT_H
