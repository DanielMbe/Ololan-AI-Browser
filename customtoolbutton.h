#ifndef CUSTOMTOOLBUTTON_H
#define CUSTOMTOOLBUTTON_H

#include <QToolButton>
#include <QObject>

class CustomToolButton : public QToolButton
{
    Q_OBJECT

public:
    explicit CustomToolButton(QWidget *parent = nullptr);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // CUSTOMTOOLBUTTON_H
