#ifndef ODOWNLOADWIDGET_H
#define ODOWNLOADWIDGET_H

#include "olibrary.h"
#include <QMenu>
#include <QShowEvent>
#include <QObject>

class OWebWidget;
class ODownloadWidgetCore : public QObject
{
    Q_OBJECT

friend class ODownloadWidget;
public:
    explicit ODownloadWidgetCore(QObject *parent = nullptr);
    void setParentMenu(ODownloadWidget *parentMenu);

public slots:
    void pauseDownload(const QString index);
    void stopDownload(const QString index);
    void resumeDownload(const QString index);
    void openManager();

private:
    ODownloadWidget *parentWidget;
};

class ODownloadWidget : public QMenu
{
    Q_OBJECT

public:
    explicit ODownloadWidget(QWidget *parent = nullptr, WindowType type = Normal);
    void setupWidget(const int index, const QString title, const QByteArray icon, const QString size, const QString state);
    void clearWidgetItemList();
    void loadEmptyView(bool state);
    void setLightGrayTheme();
    void setLightTurquoiseTheme();
    void setLightBrownTheme();
    void setDarkGrayTheme();
    void setDarkTurquoiseTheme();
    void setDarkBrownTheme();
    void setPrivateTheme();
    void setOlolanTheme(QString themeColor);
    void restoreHeight();

signals:
    void pause(const QString index);
    void stop(const QString index);
    void resume(const QString index);
    void openDownloadsManager();

public slots:
    void widgetsReady(bool status);
    void removeInactiveItem(const int index);
    void updateTransfertRate(const int index, const QString rate, const QString size, const int percentage);

protected:
    void showEvent(QShowEvent *event) override;

private:
    WindowType windType;
    ODownloadWidgetCore *widgetCore;
    OWebWidget *widgetWebView;
    QString ololanTheme;
    bool isReady;
};

#endif // ODOWNLOADWIDGET_H
