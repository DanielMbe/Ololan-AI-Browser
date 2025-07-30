#ifndef OWEBVIEW_H
#define OWEBVIEW_H

#include "olibrary.h"
#include <QWebEngineView>
#include "owebpage.h"
#include <QToolButton>
#include <QMenu>
#include <QWebEngineProfile>

class OTabPage;
class OWebView : public QWebEngineView
{
    Q_OBJECT

public:
    explicit OWebView(QWidget *parent = nullptr);

    void setNextPageAction(QToolButton *action);
    void setPreviousPageAction(QToolButton *action);
    void setReloadPageAction(QToolButton *action);
    void setWebViewType(WindowType webType);
    void setCurrentZoomFactor(qreal zoomFactor);
    void beforeClosing();
    void enableGeolaction(bool value);
    void setDoNotTrack(bool value);
    void setPaymentSaveAndFill(bool value);
    void setPasswordSaving(bool value);
    void setSiteAutoSignIn(bool value);
    void setEnableMicrophone(bool state);
    void setEnableCamera(bool state);
    void setTabPage(OTabPage *pTabPage);
    void setOlolanTheme(QString theme);
    void setupImageContextMenu(bool isPrivate);
    void setupLinkContextMenu();
    void setupTextSelectionContextMenu(bool isPrivate);
    void setupPageContextMenu();
    void setupHistoryContextMenu();
    void setupBoDoSeContextMenu();
    void setIsPrivateBrowsing(bool value);
    void setupWebPage(QWebEngineProfile *profile);
    qreal getCurrentZoomFactor();
    OTabPage *getTabPage();
    bool isPrivateBrowsing();

signals:
    void aboutToClose();

public slots:
    void loadPage(const QString url);
    void changeBrowsingButtonState();
    void reloadButtonOn();
    void reloadButtonOff();
    void reloadOrStop();
    void findTypedText(const QString text);
    void showPageSource();
    void inspectItem();
    void openContentInPrivateTab();
    void openImageInNewTab();
    void printContent();
    void searchTheWebFor();
    void savePage();
    void onLinkHovered(const QString &link);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;

private:
    QToolButton *nextPage;
    QToolButton *previousPage;
    QToolButton *reloadPage;
    QMenu *webViewMenu;
    OWebPage *webPage;
    OTabPage *parentTabPage;
    QString ololanTheme;
    QString hoveredLink;
    WindowType winType;
    bool isLoading;
    bool doNotTrack;
    qreal currentZoom;
    bool privateBrowsing;
};

#endif // OWEBVIEW_H
