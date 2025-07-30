#include "ofindinput.h"
#include <QPropertyAnimation>
#include <QWidgetAction>
#include <QPalette>

OFindInput::OFindInput(QWidget *parent)
    :QLineEdit(parent)
{
    winType = WindowType::Normal;
    closeFTextButton = new CustomToolButton(this);
    closeFTextButton->setObjectName(QLatin1String("closeFText"));
    closeFTextButton->setFixedSize(22, 20);
    closeFTextButton->setToolTip(QLatin1String("Close find text"));

    QWidgetAction *closeWidget = new QWidgetAction(this);
    closeWidget->setDefaultWidget(closeFTextButton);

    addAction(closeWidget, QLineEdit::TrailingPosition);
    setPlaceholderText(tr("Find text here"));
}

void OFindInput::setLightGrayTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {border: 1px solid #e1e1e3; border-radius: 4%; color: #353535; font-size: 14px; background-color: #e1e1e3;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #e1e1e3;}"
                                "QLineEdit:focus {border: 2px solid #56bcc5; padding-left: 0px; selection-background-color: #56bcc5;}"
                                "QLineEdit:focus:hover {border: 2px solid #56bcc5; padding-left: 0px;}"));

    closeFTextButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                  "background-image: url(:/images/closeOne.png); background-position: center;}"
                                                  "QToolButton:hover {background-image: url(:/images/closeTwo.png);}"));

    QColor placeholderColor(192, 192, 192);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    setPalette(inputPalette);

    ololanTheme = "Light gray";
    winType = WindowType::Normal;
}

void OFindInput::setLightTurquoiseTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {border: 1px solid #c0ebee; border-radius: 4%; color: #353535; font-size: 14px; background-color: #c0ebee;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #c0ebee;}"
                                "QLineEdit:focus {border: 2px solid #56bcc5; padding-left: 0px; selection-background-color: #56bcc5;}"
                                "QLineEdit:focus:hover {border: 2px solid #56bcc5; padding-left: 0px;}"));

    closeFTextButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                  "background-image: url(:/images/closeOne.png); background-position: center;}"
                                                  "QToolButton:hover {background-image: url(:/images/closeTwo.png);}"));
    QColor placeholderColor(192, 192, 192);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    setPalette(inputPalette);

    ololanTheme = "Light turquoise";
    winType = WindowType::Normal;
}

void OFindInput::setLightBrownTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {border: 1px solid #efeff1; border-radius: 4%; color: #909090; font-size: 14px; background-color: #ffd0ae;"
                                "font-family:'Segoe UI'; padding-left: 3px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #ffa477;}"
                                "QLineEdit:focus {border: 2px solid #ffa477; padding-left: 2px; selection-background-color: #68e7f0;}"
                                "QLineEdit:focus:hover {border: 2px solid #ffa477; padding-left: 2px;}"));

    closeFTextButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                  "background-image: url(:/images/closeOne.png); background-position: center;}"
                                                  "QToolButton:hover {background-image: url(:/images/closeTwo.png);}"));

    QColor placeholderColor(192, 192, 192);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    setPalette(inputPalette);

    ololanTheme = "Light brown";
    winType = WindowType::Normal;
}

void OFindInput::setDarkGrayTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {background-color: #434344; border: 1px solid #434344; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                "font-family:'Segoe UI'; padding-left: 3px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #68e7f0;}"
                                "QLineEdit:focus {border: 2px solid #68e7f0; padding-left: 2px; selection-background-color: #68e7f0;}"
                                "QLineEdit:focus:hover {border: 2px solid #68e7f0; padding-left: 2px;}"));

    QColor placeholderColor(160, 160, 160);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    setPalette(inputPalette);

    closeFTextButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                  "background-image: url(:/private mode images/closeOnePrivateMode.png); background-position: center;}"
                                                  "QToolButton:hover {background-image: url(:/private mode images/closeTwoPrivateMode.png);}"));

    ololanTheme = "Dark gray";
    winType = WindowType::Normal;
}

void OFindInput::setDarkTurquoiseTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {background-color: #1e4446; border: 1px solid #1e4446; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                "font-family:'Segoe UI'; padding-left: 3px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #68e7f0;}"
                                "QLineEdit:focus {border: 2px solid #68e7f0; padding-left: 2px; selection-background-color: #68e7f0;}"
                                "QLineEdit:focus:hover {border: 2px solid #68e7f0; padding-left: 2px;}"));

    QColor placeholderColor(160, 160, 160);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    setPalette(inputPalette);

    closeFTextButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                  "background-image: url(:/private mode images/closeOnePrivateMode.png); background-position: center;}"
                                                  "QToolButton:hover {background-image: url(:/private mode images/closeTwoPrivateMode.png);}"));

    ololanTheme = "Dark turquoise";
    winType = WindowType::Normal;
}

void OFindInput::setDarkBrownTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {background-color: #462c21; border: 1px solid #462c21; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                "font-family:'Segoe UI'; padding-left: 3px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #ffa477;}"
                                "QLineEdit:focus {border: 2px solid #ffa477; padding-left: 2px; selection-background-color: #68e7f0;}"
                                "QLineEdit:focus:hover {border: 2px solid #ffa477; padding-left: 2px;}"));

    QColor placeholderColor(160, 160, 160);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    setPalette(inputPalette);

    closeFTextButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                  "background-image: url(:/private mode images/closeOnePrivateMode.png); background-position: center;}"
                                                  "QToolButton:hover {background-image: url(:/private mode images/closeTwoPrivateMode.png);}"));

    ololanTheme = "Dark brown";
    winType = WindowType::Normal;
}

void OFindInput::setPrivateTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {background-color: #3c3244; border: 1px solid #3c3244; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                "font-family:'Segoe UI'; padding-left: 3px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #c2a1dd;}"
                                "QLineEdit:focus {border: 2px solid #c2a1dd; padding-left: 2px;}"
                                "QLineEdit:focus:hover {border: 2px solid #c2a1dd; padding-left: 2px;}"));

    QColor placeholderColor(160, 160, 160);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    setPalette(inputPalette);

    closeFTextButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                  "background-image: url(:/private mode images/closeOnePrivateMode.png); background-position: center;}"
                                                  "QToolButton:hover {background-image: url(:/private mode images/closeTwoPrivateMode.png);}"));

    ololanTheme = "Private";
    winType = WindowType::Private;
}

void OFindInput::focusInEvent(QFocusEvent *event)
{
    if (winType == WindowType::Normal)
    {
        if (ololanTheme.compare("Light gray") == 0)
        {
            setStyleSheet(QLatin1String("QLineEdit {border: 1px solid #e1e1e3; border-radius: 4%; color: #353535; font-size: 14px; background-color: #e1e1e3;"
                                        "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                        "QLineEdit:hover {border: 1px solid #e1e1e3;}"
                                        "QLineEdit:focus {border: 2px solid #56bcc5; padding-left: 0px; selection-background-color: #56bcc5;}"
                                        "QLineEdit:focus:hover {border: 2px solid #56bcc5; padding-left: 0px;}"));

            QColor placeholderColor(192, 192, 192);
            QPalette inputPalette = palette();
            inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
            setPalette(inputPalette);
        }
        else if (ololanTheme.compare("Light turquoise") == 0)
        {
            setStyleSheet(QLatin1String("QLineEdit {border: 1px solid #c0ebee; border-radius: 4%; color: #353535; font-size: 14px; background-color: #c0ebee;"
                                        "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                        "QLineEdit:hover {border: 1px solid #c0ebee;}"
                                        "QLineEdit:focus {border: 2px solid #56bcc5; padding-left: 0px; selection-background-color: #56bcc5;}"
                                        "QLineEdit:focus:hover {border: 2px solid #56bcc5; padding-left: 0px;}"));

            QColor placeholderColor(192, 192, 192);
            QPalette inputPalette = palette();
            inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
            setPalette(inputPalette);
        }
        else if (ololanTheme.compare("Light brown") == 0)
        {
            setStyleSheet(QLatin1String("QLineEdit {border: 1px solid #efeff1; border-radius: 4%; color: #909090; font-size: 14px; background-color: #ffd0ae;"
                                        "font-family:'Segoe UI'; padding-left: 3px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                        "QLineEdit:hover {border: 1px solid #ffa477;}"
                                        "QLineEdit:focus {border: 2px solid #ffa477; padding-left: 2px; selection-background-color: #68e7f0;}"
                                        "QLineEdit:focus:hover {border: 2px solid #ffa477; padding-left: 2px;}"));

            QColor placeholderColor(192, 192, 192);
            QPalette inputPalette = palette();
            inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
            setPalette(inputPalette);
        }
        else if (ololanTheme.compare("Dark gray") == 0)
        {
            setStyleSheet(QLatin1String("QLineEdit {background-color: #434344; border: 1px solid #434344; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                        "font-family:'Segoe UI'; padding-left: 3px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                        "QLineEdit:hover {border: 1px solid #68e7f0;}"
                                        "QLineEdit:focus {border: 2px solid #68e7f0; padding-left: 2px; selection-background-color: #68e7f0;}"
                                        "QLineEdit:focus:hover {border: 2px solid #68e7f0; padding-left: 2px;}"));

            QColor placeholderColor(160, 160, 160);
            QPalette inputPalette = palette();
            inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
            setPalette(inputPalette);
        }
        else if (ololanTheme.compare("Dark turquoise") == 0)
        {
            setStyleSheet(QLatin1String("QLineEdit {background-color: #1e4446; border: 1px solid #1e4446; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                        "font-family:'Segoe UI'; padding-left: 3px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                        "QLineEdit:hover {border: 1px solid #68e7f0;}"
                                        "QLineEdit:focus {border: 2px solid #68e7f0; padding-left: 2px; selection-background-color: #68e7f0;}"
                                        "QLineEdit:focus:hover {border: 2px solid #68e7f0; padding-left: 2px;}"));

            QColor placeholderColor(160, 160, 160);
            QPalette inputPalette = palette();
            inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
            setPalette(inputPalette);
        }
        else if (ololanTheme.compare("Dark brown") == 0)
        {
            setStyleSheet(QLatin1String("QLineEdit {background-color: #462c21; border: 1px solid #462c21; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                        "font-family:'Segoe UI'; padding-left: 3px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                        "QLineEdit:hover {border: 1px solid #ffa477;}"
                                        "QLineEdit:focus {border: 2px solid #ffa477; padding-left: 2px; selection-background-color: #68e7f0;}"
                                        "QLineEdit:focus:hover {border: 2px solid #ffa477; padding-left: 2px;}"));

            QColor placeholderColor(160, 160, 160);
            QPalette inputPalette = palette();
            inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
            setPalette(inputPalette);
        }
    }
    else if (winType == WindowType::Private)
    {
        setStyleSheet(QLatin1String("QLineEdit {background-color: #3c3244; border: 1px solid #393041; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                    "font-family:'Segoe UI'; padding-left: 3px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                    "QLineEdit:hover {border: 1px solid #c2a1dd;}"
                                    "QLineEdit:focus {border: 2px solid #c2a1dd; padding-left: 2px;}"
                                    "QLineEdit:focus:hover {border: 2px solid #c2a1dd; padding-left: 2px;}"));

        QColor placeholderColor(160, 160, 160);
        QPalette inputPalette = palette();
        inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
        setPalette(inputPalette);
    }

    QLineEdit::focusInEvent(event);
}

void OFindInput::focusOutEvent(QFocusEvent *event)
{
    if (winType == WindowType::Normal)
    {
        if (ololanTheme.compare("Light gray") == 0)
            setLightGrayTheme();
        else if (ololanTheme.compare("Light turquoise") == 0)
            setLightTurquoiseTheme();
        else if (ololanTheme.compare("Light brown") == 0)
            setLightBrownTheme();
        else if (ololanTheme.compare("Dark gray") == 0)
            setDarkGrayTheme();
        else if (ololanTheme.compare("Dark turquoise") == 0)
            setDarkTurquoiseTheme();
        else if (ololanTheme.compare("Dark brown") == 0)
            setDarkBrownTheme();
    }
    else if (winType == WindowType::Private)
        setPrivateTheme();

    QLineEdit::focusOutEvent(event);
}

void OFindInput::showFindInput()
{
    classPtr->setVisible(true);
    setFocus();
    QPropertyAnimation *animation = new QPropertyAnimation(this, "width", this);
    animation->setDuration(150);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setStartValue(1);
    animation->setEndValue(300);
    animation->start();
}

void OFindInput::hideFindInput()
{
    emit textEdited("");
    clear();
    QPropertyAnimation *animation = new QPropertyAnimation(this, "width", this);
    connect(animation, &QPropertyAnimation::finished, this, &OFindInput::hideByAction);
    animation->setDuration(150);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setStartValue(300);
    animation->setEndValue(1);
    animation->start();
}

void OFindInput::hideByAction()
{
    classPtr->setVisible(false);
}

void OFindInput::setClassPtr(QAction *ptr)
{
    classPtr = ptr;
}

CustomToolButton *OFindInput::getCloseButton()
{
    return closeFTextButton;
}
