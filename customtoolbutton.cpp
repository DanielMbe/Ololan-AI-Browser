#include "customtoolbutton.h"
#include <QEvent>
#include <QMouseEvent>

CustomToolButton::CustomToolButton(QWidget *parent)
    :QToolButton(parent)
{

}

void CustomToolButton::enterEvent(QEnterEvent *event)
{
    setCursor(QCursor(Qt::ArrowCursor));
    event->accept();
}

void CustomToolButton::leaveEvent(QEvent *event)
{
    unsetCursor();
    event->accept();
}

void CustomToolButton::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
}
