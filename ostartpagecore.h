#ifndef OSTARTPAGECORE_H
#define OSTARTPAGECORE_H

#include <QObject>
#include <QAction>

class OWebView;
class QWebChannel;
class OHistoryItem;
class OStartPageCore : public QObject
{
    Q_OBJECT

public:
    OStartPageCore(QObject *parent = nullptr);
    void addLastVisitedItem();
    void setupLastItem(QList<OHistoryItem*> list);
    void setSearchEngineName(const QString name);

public slots:
    void processQuery(const QString query);
    void setupStartPage(bool state);

private:
    OWebView *webView;
    QList<QAction*> itemList;
    QWebChannel *webChannel;
    QString searchEngineName;
};

#endif // OSTARTPAGECORE_H
