#ifndef OSYNCWIDGET_H
#define OSYNCWIDGET_H

#include <QMenu>
#include <QToolButton>
#include <QObject>
#include <QLabel>

class OTabPage;
class OSyncWidget : public QMenu
{
    Q_OBJECT
    
public:
    OSyncWidget(QWidget *parent = nullptr);
    void setupControl();
    void setLightGrayTheme();
    void setLightTurquoiseTheme();
    void setLightBrownTheme();
    void setDarkGrayTheme();
    void setDarkTurquoiseTheme();
    void setDarkBrownTheme();
    void setPrivateTheme();
    void setTabPage(OTabPage *tPage);
    void updateProfile(const QString accountName, bool isLoggedIn);
    void setOlolanTheme(QString theme);

public slots:
    void synchronize();

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    OTabPage *tabPage;
    QWidget *widgetForm;
    QToolButton *userUI;
    QLabel *username;
    QToolButton *syncButton;
    QLabel *labelInfo;
    QString themeColor;
    bool signIn;
};

#endif // OSYNCWIDGET_H
