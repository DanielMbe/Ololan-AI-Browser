#ifndef OWEBWIDGET_H
#define OWEBWIDGET_H

#include <QWebEngineView>
#include <QWebEnginePage>

class OWebWidget;
class OAssistantCore;
class OTabPage;
class OWebWidget : public QWebEngineView
{
    Q_OBJECT
public:
    explicit OWebWidget(QWidget *parent = nullptr);
    void setupAssistant(OAssistantCore *assistant);
    void setTabPage(OTabPage *tPage);
    void setLightGrayTheme();
    void setLightTurquoiseTheme();
    void setLightBrownTheme();
    void setDarkGrayTheme();
    void setDarkTurquoiseTheme();
    void setDarkBrownTheme();
    void setPrivateTheme();

public slots:
    void widgetsReady(bool status);
    void showSuggestions(const QStringList commands);
    void addAssistantSpeech(const QString speech);
    void beforeHiding();
    void setActiveProcessor();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    OAssistantCore *assistantManager;
    OTabPage *tabPage;
    QString ololanTheme;
    bool pageReady;
};

#endif // OWEBWIDGET_H
