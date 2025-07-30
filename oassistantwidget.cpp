#include "oassistantwidget.h"
#include <QWidgetAction>
#include <QShowEvent>
#include <QHideEvent>
#include <QPropertyAnimation>

OAssistantWidget::OAssistantWidget(QWidget *parent)
    :QMenu(parent)
{
    setFixedHeight(550);
}
