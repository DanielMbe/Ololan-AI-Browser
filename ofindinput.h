#ifndef OFINDINPUT_H
#define OFINDINPUT_H

#include <QLineEdit>
#include <QLabel>
#include "customtoolbutton.h"
#include "olibrary.h"

class OFindInput : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(int width READ width WRITE setFixedWidth NOTIFY widthChanged)

public:
    OFindInput(QWidget *parent = nullptr);
    void setLightGrayTheme();
    void setLightTurquoiseTheme();
    void setLightBrownTheme();
    void setDarkGrayTheme();
    void setDarkTurquoiseTheme();
    void setDarkBrownTheme();
    void setPrivateTheme();
    void showFindInput();
    void setClassPtr(QAction *ptr);
    CustomToolButton *getCloseButton();

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

signals:
    void widthChanged(int value);

public slots:
    void hideByAction();
    void hideFindInput();

private:
    WindowType winType;
    QString ololanTheme;
    QAction *classPtr;
    CustomToolButton *closeFTextButton;
};

#endif // OFINDINPUT_H
