#ifndef OWEBINSPECTOR_H
#define OWEBINSPECTOR_H

#include <QWebEngineView>
#include <QWebEnginePage>

class OWebInspector : public QWebEngineView
{
    Q_OBJECT

public:
    explicit OWebInspector(QWidget *parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif // OWEBINSPECTOR_H
