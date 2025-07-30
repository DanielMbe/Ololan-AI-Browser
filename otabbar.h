#ifndef OTABBAR_H
#define OTABBAR_H

#include "olibrary.h"
#include "customtoolbutton.h"
#include <QLabel>
#include <QWidget>
#include <QHBoxLayout>
#include <QSequentialAnimationGroup>
#include <QVariant>

//tabs class
class OTab : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int width READ width WRITE setFixedWidth NOTIFY widthChanged)

public:
    explicit OTab(QWidget *parent = nullptr);
    void setTabIndex(int index);
    void hideContent();
    void showContent();
    void setLightGrayTheme(bool isSelected);
    void setLightTurquoiseTheme(bool isSelected);
    void setLightBrownTheme(bool isSelected);
    void setDarkGrayTheme(bool isSelected);
    void setDarkTurquoiseTheme(bool isSelected);
    void setDarkBrownTheme(bool isSelected);
    void setPrivateTheme(bool isSelected);
    QIcon getTabIcon();
    QString getTabText();
    QToolButton *getTabCloseButton();
    int getTabIndex();

signals:
    void tabIndexSignal(int &index);
    void tabClicked(int &index);
    void widthChanged(int value);

public slots:
    void closeTab();
    void setSelectedStyle(QString themeColor);
    void setDeselectedStyle(QString themeColor);
    void setTabIcon(const QIcon &icon);
    void setTabText(const QString &tabTextString);
    void tabIconClick();
    void startLoadingAnimation();
    void stopLoadingAnimation(bool state);
    void restartLoadingAnimation(int value);
    void rotateLoadingIcon(QVariant value);
    void restartLoading();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QLabel *tabText;
    QToolButton *tabCloseButton;
    QToolButton *tabIconButton;
    QSequentialAnimationGroup *animationGroup;
    QString tabThemeColor;
    int tabIndex;
    bool isLoading;
    bool isSelectedTab;
};

//tab bar class
class OTabBar : public QWidget
{
    Q_OBJECT

public:
    OTabBar(QWidget *parent = nullptr);
    void updateSize(QResizeEvent *event);
    void mouseTabMoveEvent(QMouseEvent *event);
    void addTabAnimation(OTab *tab);
    void removeTabAnimation(OTab *tab);
    void setTabsWidth(int &width);
    void setTabToMove(OTab *tab);
    void setDistanceWidth(const int distance);
    void setOldPosition(const QPoint position);
    void setWindowMoved(bool isMoved);
    void setupWidget();
    void setWinType(WindowType tabType);
    void setIsGrabbed(bool state);
    void hideTab();
    void showTab();
    void moveToLastTab();
    void switchToTab(const int index);
    void grabTab(QIcon icon, QString tabTitle);
    void grabTabWindow();
    void hideTab(bool onGrab);
    void moveToLastTab(bool onGrab);
    void swipeTabLeft(bool animate);
    void swipeTabRight(bool animate);
    void setLightGrayTheme();
    void setLightTurquoiseTheme();
    void setLightBrownTheme();
    void setDarkGrayTheme();
    void setDarkTurquoiseTheme();
    void setDarkBrownTheme();
    void setPrivateTheme();
    void bruteRemoveTab(int &index);
    int currentTab();
    int count();
    int getTabsWidth();
    bool resizeTabs();
    bool resizeOnAdding();
    bool resizeOnRemoving();
    bool resizeOnGrabbing();
    bool getWindowMoved();
    bool getIsGrabbed();
    OTab *getTabAt(int index);
    OTab *getTabToMove();

public slots:
    void addTab(QIcon icon, QString tabTitle);
    void removeTab(int &index);
    void setCurrentTab(const int index);
    void removingEnded();
    void tabReindexing();
    void selectNextTab();
    void selectPreviousTab();
    void ajustTabSize();
    void handleCompleter();

signals:
    void currentChanged(const int index);
    void tabCloseRequested(int &index);
    void tabMoved(const int from, const int to);
    void addButtonClicked();
    void assistantButtonClicked();
    void maxMinButtonClicked();
    void minimizeButtonClicked();
    void closeButtonClicked();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void tabMoveEvent();

private:
    QList <OTab*> tabsList;
    QHBoxLayout *tabsLayout;
    OTab *tabToRemove;
    OTab *tabToMove;
    QPoint clickPosition;
    QPoint oldPosition;
    QString tabBarThemeColor;
    CustomToolButton *addButton;
    CustomToolButton *maxMinButton;
    CustomToolButton *minimizeButton;
    CustomToolButton *closeButton;
    CustomToolButton *nextTabButton;
    CustomToolButton *previousTabButton;
    TabMoveDirection moveDirection;
    WindowType winType;
    int activeTab;
    int tabsWidth;
    int distanceWidth;
    bool windowMoved;
    bool isGrabbed;
};

#endif // OTABBAR_H
