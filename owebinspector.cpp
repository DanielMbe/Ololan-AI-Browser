#include "owebinspector.h"
#include <QContextMenuEvent>

OWebInspector::OWebInspector(QWidget *parent)
    :QWebEngineView(parent)
{
    load(QUrl("http://127.0.0.1:3000"));
}

void OWebInspector::contextMenuEvent(QContextMenuEvent *event)
{
    event->ignore();
}
