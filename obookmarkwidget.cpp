#include "obookmarkwidget.h"
#include "obookmarkscore.h"
#include "otabpage.h"
#include "owebview.h"
#include "oinputcompleter.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QPropertyAnimation>
#include <QHideEvent>

OBookmarkWidget::OBookmarkWidget(QWidget *parent)
    :QMenu(parent)
{
    removed = false;
    done = false;
    setFixedSize(350, 200);
    setupControl();
    connect(doneButton, &QToolButton::clicked, this, &OBookmarkWidget::addBookmark);
    connect(removeButton, &QToolButton::clicked, this, &OBookmarkWidget::removeBookmark);
}

void OBookmarkWidget::setupControl()
{
    widgetForm = new QWidget(this);
    widgetForm->setFixedSize(348, 198);

    widgetLabel = new QLabel(widgetForm);
    widgetLabel->setText("Bookmark added");

    titleField = new QLineEdit(widgetForm);
    titleField->setFixedHeight(35);
    titleField->setPlaceholderText("Bookmark name");

    folderField = new QComboBox(widgetForm);
    folderField->setFixedHeight(35);

    folderFieldView = new OInputCompleter();
    folderField->setView(folderFieldView);

    removeButton = new QToolButton(this);
    removeButton->setText("Remove");
    removeButton->setFixedSize(90, 35);

    doneButton = new QToolButton(this);
    doneButton->setText("Done");
    doneButton->setFixedSize(90, 35);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    hLayout->addWidget(doneButton);
    hLayout->addSpacerItem(new QSpacerItem(100, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));
    hLayout->addWidget(removeButton);

    QVBoxLayout *vLayout = new QVBoxLayout(widgetForm);
    vLayout->setContentsMargins(7, 7, 7, 7);
    vLayout->setSpacing(10);
    vLayout->addWidget(widgetLabel);
    vLayout->addWidget(titleField);
    vLayout->addWidget(folderField);
    vLayout->addLayout(hLayout);

    QWidgetAction *widgetAction = new QWidgetAction(this);
    widgetAction->setDefaultWidget(widgetForm);
    addAction(widgetAction);
}

void OBookmarkWidget::showEvent(QShowEvent *event)
{
    removed = false;
    done = false;

    QMenu::showEvent(event);
    QRect startGeometry = geometry();
    QRect endGeometry = geometry();
    QPoint globalPosition = parentWidget()->mapToGlobal(parentWidget()->pos());
    const int distance = parentWidget()->width() - width() - 122;
    startGeometry.setCoords(globalPosition.x()+distance, globalPosition.y()+16, globalPosition.x()+distance+width(), globalPosition.y()+height()-10);
    endGeometry.setCoords(globalPosition.x()+distance, globalPosition.y()+31, globalPosition.x()+distance+width(), globalPosition.y()+height());

    setGeometry(startGeometry);
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry", this);
    animation->setDuration(100);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);
    animation->start();

    folderField->clear();
    QStringList folderList;
    for (int i = 0; i < bookmarkManager->getFolderList().count(); i++)
    {
        if (!folderList.contains(bookmarkManager->getFolderList().at(i)))
        {
            folderList.append(bookmarkManager->getFolderList().at(i));
            folderField->addItem(QIcon(":/web_applications/images/bookmarkFolder.png"), folderList.last());
        }
    }

    const QString parsedUrl = parseUrl(tabPage->getStackPage()->currentWidget()->findChild<OWebView*>()->url().toEncoded());
    if (!bookmarkManager->itemExist(parsedUrl))
    {
        bookmarkManager->addBookmarkItem(tabPage->getStackPage()->currentWidget()->findChild<OWebView*>()->title(), parsedUrl,
                                         tabPage->getStackPage()->currentWidget()->findChild<OWebView*>()->icon(),
                                         folderField->itemText(0));
    }

    parentButton->setChecked(true);
    const QString webTitle = tabPage->getStackPage()->currentWidget()->findChild<OWebView*>()->title();
    const QString bookmarkTitle = bookmarkManager->lastAddedTitle(parsedUrl);
    folderField->setCurrentText(bookmarkManager->lastAddedFolder(parsedUrl));
    titleField->setText(!bookmarkTitle.isEmpty() ? bookmarkTitle : webTitle);
    titleField->setFocus();
    titleField->selectAll();
}

void OBookmarkWidget::closeEvent(QCloseEvent *event)
{
    QMenu::closeEvent(event);
    QPoint mousePos = mapFromGlobal(cursor().pos());

    if (removed || done || !rect().contains(mousePos))
        event->accept();
    else
        event->ignore();
}

void OBookmarkWidget::setLightGrayTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"));
    widgetForm->setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; padding: 0px; border: none;}"));
    widgetLabel->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #68e7f0;"
                                             "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    titleField->setStyleSheet(QLatin1String("QLineEdit {border: 1px solid #efeff1; border-radius: 4%; color: #909090; font-size: 14px;"
                                            "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;"
                                            "background-color: #f9f9fb;}"
                                            "QLineEdit:hover {border: 1px solid #68e7f0;}"
                                            "QLineEdit:focus {border: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;}"
                                            "QLineEdit:focus:hover {border: 2px solid #68e7f0; padding-left: 0px;}"));

    QColor placeholderColor(192, 192, 192);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    titleField->setPalette(inputPalette);

    folderField->setStyleSheet(QLatin1String("QComboBox {border: 1px solid #efeff1; border-radius: 4%; color: #909090; font-size: 14px;"
                                             "font-family:'Segoe UI'; padding-left: 2px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;"
                                             "background-color: #f9f9fb;}"
                                             "QComboBox:hover {border: 1px solid #68e7f0;}"
                                             "QComboBox:focus {border: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;}"
                                             "QComboBox:on {border-left: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;"
                                             "border-right: 2px solid #68e7f0; border-bottom: none; border-top: 2px solid #68e7f0;"
                                             "border-bottom-left-radius: 0px; border-bottom-right-radius: 0px;}"
                                             "QComboBox:focus:hover {border: 2px solid #68e7f0; padding-left: 0px;}"
                                             "QComboBox::drop-down {subcontrol-origin: padding; subcontrol-position: top right; width: 20px;"
                                             "border-left: 1px solid #efeff1; border-top-right-radius: 1%; border-bottom-right-radius: 1%;}"
                                             "QComboBox::drop-down:focus {border-left: 2px solid #68e7f0; width: 20px; background-color: #68e7f0;}"
                                             "QComboBox::down-arrow {image: url(:/images/down.png); height: 12px; width: 12px;}"));

    removeButton->setStyleSheet(QLatin1String("QToolButton {color: #909090; background-color: #f9f9fb; border: none; padding: 0px; border-radius: 3.5%;}"
                                              "QToolButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                              "QToolButton:pressed {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"));

    doneButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;}"
                                            "QToolButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                            "QToolButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));

    /*folderFieldView->setStyleSheet(QLatin1String("QListView {background-color: #f9f9fb; padding: 0px; border-left: 2px solid #68e7f0; font-size: 14px;"
                                                 "border-right: 2px solid #68e7f0; border-bottom: 2px solid #68e7f0; border-top: none;"
                                                 "border-bottom-left-radius: 4%; border-bottom-right-radius: 4%; outline: none;}"
                                                 "QListView::item {color: #888888; height: 29px;}"
                                                 "QListView::item:hover {background-color: #efeff1; border-top: 1px solid #efeff1; border-bottom: 1px solid #efeff1;}"
                                                 "QListView::item:selected {background-color: #efeff1; border-top: 1px solid #efeff1; border-bottom: 1px solid #efeff1;}"));*/
}

void OBookmarkWidget::setLightTurquoiseTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"));
    widgetForm->setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; padding: 0px; border: none;}"));
    widgetLabel->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #68e7f0;"
                                             "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    titleField->setStyleSheet(QLatin1String("QLineEdit {border: 1px solid #efeff1; border-radius: 4%; color: #909090; font-size: 14px;"
                                            "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;"
                                            "background-color: #f9f9fb;}"
                                            "QLineEdit:hover {border: 1px solid #68e7f0;}"
                                            "QLineEdit:focus {border: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;}"
                                            "QLineEdit:focus:hover {border: 2px solid #68e7f0; padding-left: 0px;}"));

    QColor placeholderColor(192, 192, 192);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    titleField->setPalette(inputPalette);

    folderField->setStyleSheet(QLatin1String("QComboBox {border: 1px solid #efeff1; border-radius: 4%; color: #909090; font-size: 14px;"
                                             "font-family:'Segoe UI'; padding-left: 2px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;"
                                             "background-color: #f9f9fb;}"
                                             "QComboBox:hover {border: 1px solid #68e7f0;}"
                                             "QComboBox:focus {border: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;}"
                                             "QComboBox:on {border-left: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;"
                                             "border-right: 2px solid #68e7f0; border-bottom: none; border-top: 2px solid #68e7f0;"
                                             "border-bottom-left-radius: 0px; border-bottom-right-radius: 0px;}"
                                             "QComboBox:focus:hover {border: 2px solid #68e7f0; padding-left: 0px;}"
                                             "QComboBox::drop-down {subcontrol-origin: padding; subcontrol-position: top right; width: 20px;"
                                             "border-left: 1px solid #efeff1; border-top-right-radius: 1%; border-bottom-right-radius: 1%;}"
                                             "QComboBox::drop-down:focus {border-left: 2px solid #68e7f0; width: 20px; background-color: #68e7f0;}"
                                             "QComboBox::down-arrow {image: url(:/images/down.png); height: 12px; width: 12px;}"));

    removeButton->setStyleSheet(QLatin1String("QToolButton {color: #909090; background-color: #f9f9fb; border: none; padding: 0px; border-radius: 3.5%;}"
                                              "QToolButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                              "QToolButton:pressed {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"));

    doneButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;}"
                                            "QToolButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                            "QToolButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));

    /*folderFieldView->setStyleSheet(QLatin1String("QListView {background-color: #f9f9fb; padding: 0px; border-left: 2px solid #68e7f0; font-size: 14px;"
                                                 "border-right: 2px solid #68e7f0; border-bottom: 2px solid #68e7f0; border-top: none;"
                                                 "border-bottom-left-radius: 4%; border-bottom-right-radius: 4%; outline: none;}"
                                                 "QListView::item {color: #888888; height: 29px;}"
                                                 "QListView::item:hover {background-color: #69e8f1; border-top: 1px solid #69e8f1; border-bottom: 1px solid #69e8f1;}"
                                                 "QListView::item:selected {background-color: #69e8f1; border-top: 1px solid #69e8f1; border-bottom: 1px solid #69e8f1;}"));*/
}

void OBookmarkWidget::setLightBrownTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #ffffff; padding: 0px; border: 1px solid #efeff1;}"));
    widgetForm->setStyleSheet(QLatin1String("QWidget {background-color: #ffffff; padding: 0px; border: none;}"));
    widgetLabel->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #ffa477;"
                                             "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    titleField->setStyleSheet(QLatin1String("QLineEdit {border: 1px solid #efeff1; border-radius: 4%; color: #909090; font-size: 14px;"
                                            "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;"
                                            "background-color: #f9f9fb;}"
                                            "QLineEdit:hover {border: 1px solid #ffa477;}"
                                            "QLineEdit:focus {border: 2px solid #ffa477; padding-left: 0px; selection-background-color: #ffa477;}"
                                            "QLineEdit:focus:hover {border: 2px solid #ffa477; padding-left: 0px;}"));

    QColor placeholderColor(192, 192, 192);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    titleField->setPalette(inputPalette);

    folderField->setStyleSheet(QLatin1String("QComboBox {border: 1px solid #efeff1; border-radius: 4%; color: #909090; font-size: 14px;"
                                             "font-family:'Segoe UI'; padding-left: 2px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;"
                                             "background-color: #f9f9fb;}"
                                             "QComboBox:hover {border: 1px solid #ffa477;}"
                                             "QComboBox:focus {border: 2px solid #ffa477; padding-left: 0px; selection-background-color: #ffa477;}"
                                             "QComboBox:on {border-left: 2px solid #ffa477; padding-left: 0px; selection-background-color: #ffa477;"
                                             "border-right: 2px solid #ffa477; border-bottom: none; border-top: 2px solid #ffa477;"
                                             "border-bottom-left-radius: 0px; border-bottom-right-radius: 0px;}"
                                             "QComboBox:focus:hover {border: 2px solid #ffa477; padding-left: 0px;}"
                                             "QComboBox::drop-down {subcontrol-origin: padding; subcontrol-position: top right; width: 20px;"
                                             "border-left: 1px solid #efeff1; border-top-right-radius: 1%; border-bottom-right-radius: 1%;}"
                                             "QComboBox::drop-down:focus {border-left: 2px solid #ffa477; width: 20px; background-color: #ffa477;}"
                                             "QComboBox::down-arrow {image: url(:/images/down.png); height: 12px; width: 12px;}"));

    removeButton->setStyleSheet(QLatin1String("QToolButton {color: #909090; background-color: #f9f9fb; border: none; padding: 0px; border-radius: 3.5%;}"
                                              "QToolButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                              "QToolButton:pressed {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"));

    doneButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #ffa477; border: none; padding: 0px; border-radius: 3.5%;}"
                                            "QToolButton:hover {background-color: #e5926b; border: 1px solid #e5926b;}"
                                            "QToolButton:pressed {background-color: #d68864; border: 1px solid #d68864;}"));

    folderFieldView->setStyleSheet(QLatin1String("QListView {background-color: #f9f9fb; padding: 0px; border-left: 2px solid #ffa477; font-size: 14px;"
                                                 "border-right: 2px solid #ffa477; border-bottom: 2px solid #ffa477; border-top: none;"
                                                 "border-bottom-left-radius: 4%; border-bottom-right-radius: 4%; outline: none;}"
                                                 "QListView::item {color: #888888; height: 29px;}"
                                                 "QListView::item:hover {background-color: #ffa477; border-top: 1px solid #ffa477; border-bottom: 1px solid #ffa477;}"
                                                 "QListView::item:selected {background-color: #ffa477; border-top: 1px solid #ffa477; border-bottom: 1px solid #ffa477;}"));
}

void OBookmarkWidget::setDarkGrayTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #434344; padding: 0px; border: 1px solid #434344;}"));
    widgetForm->setStyleSheet(QLatin1String("QWidget {background-color: #434344; padding: 0px; border: none;}"));
    widgetLabel->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                             "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    titleField->setStyleSheet(QLatin1String("QLineEdit {background-color: #6b6b6b; border: 1px solid #6b6b6b; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                            "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                            "QLineEdit:hover {border: 1px solid #68e7f0;}"
                                            "QLineEdit:focus {border: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;}"
                                            "QLineEdit:focus:hover {border: 2px solid #68e7f0; padding-left: 0px;}"));

    QColor placeholderColor(192, 192, 192);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    titleField->setPalette(inputPalette);

    folderField->setStyleSheet(QLatin1String("QComboBox {border: 1px solid #6b6b6b; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                             "font-family:'Segoe UI'; padding-left: 2px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;"
                                             "background-color: #6b6b6b;}"
                                             "QComboBox:hover {border: 1px solid #68e7f0;}"
                                             "QComboBox:focus {border: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;}"
                                             "QComboBox:on {border-left: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;"
                                             "border-right: 2px solid #68e7f0; border-bottom: none; border-top: 2px solid #68e7f0;"
                                             "border-bottom-left-radius: 0px; border-bottom-right-radius: 0px;}"
                                             "QComboBox:focus:hover {border: 2px solid #68e7f0; padding-left: 0px;}"
                                             "QComboBox::drop-down {subcontrol-origin: padding; subcontrol-position: top right; width: 20px;"
                                             "border-left: 1px solid #bbbbbb; border-top-right-radius: 1%; border-bottom-right-radius: 1%;}"
                                             "QComboBox::drop-down:focus {border-left: 2px solid #68e7f0; width: 20px; background-color: #68e7f0;}"
                                             "QComboBox::down-arrow {image: url(:/private mode images/downPrivateMode.png); height: 12px; width: 12px;}"
                                             "QComboBox::down-arrow:focus {image: url(:/images/down.png); height: 12px; width: 12px;}"));

    removeButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #6b6b6b; border: none; padding: 0px; border-radius: 3.5%;}"
                                              "QToolButton:hover {background-color: #777778; border: 1px solid #777778;}"
                                              "QToolButton:pressed {background-color: #888889; border: 2px solid #888889;}"));

    doneButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;}"
                                            "QToolButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                            "QToolButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));

    folderFieldView->setStyleSheet(QLatin1String("QListView {background-color: #6b6b6b; padding: 0px; border-left: 2px solid #68e7f0; font-size: 14px;"
                                                 "border-right: 2px solid #68e7f0; border-bottom: 2px solid #68e7f0; border-top: none;"
                                                 "border-bottom-left-radius: 4%; border-bottom-right-radius: 4%; outline: none;}"
                                                 "QListView::item {color: #dddddd; height: 29px;}"
                                                 "QListView::item:hover {background-color: #434344; border-top: 1px solid #434344; border-bottom: 1px solid #434344;}"
                                                 "QListView::item:selected {background-color: #434344; border-top: 1px solid #434344; border-bottom: 1px solid #434344;}"));
}

void OBookmarkWidget::setDarkTurquoiseTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #1e4446; padding: 0px; border: 1px solid #1e4446;}"));
    widgetForm->setStyleSheet(QLatin1String("QWidget {background-color: #1e4446; padding: 0px; border: none;}"));
    widgetLabel->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                             "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    titleField->setStyleSheet(QLatin1String("QLineEdit {background-color: #2e686b; border: 1px solid #2e686b; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                            "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                            "QLineEdit:hover {border: 1px solid #68e7f0;}"
                                            "QLineEdit:focus {border: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;}"
                                            "QLineEdit:focus:hover {border: 2px solid #68e7f0; padding-left: 0px;}"));

    QColor placeholderColor(192, 192, 192);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    titleField->setPalette(inputPalette);

    folderField->setStyleSheet(QLatin1String("QComboBox {border: 1px solid #2e686b; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                             "font-family:'Segoe UI'; padding-left: 2px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;"
                                             "background-color: #2e686b;}"
                                             "QComboBox:hover {border: 1px solid #68e7f0;}"
                                             "QComboBox:focus {border: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;}"
                                             "QComboBox:on {border-left: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;"
                                             "border-right: 2px solid #68e7f0; border-bottom: none; border-top: 2px solid #68e7f0;"
                                             "border-bottom-left-radius: 0px; border-bottom-right-radius: 0px;}"
                                             "QComboBox:focus:hover {border: 2px solid #68e7f0; padding-left: 0px;}"
                                             "QComboBox::drop-down {subcontrol-origin: padding; subcontrol-position: top right; width: 20px;"
                                             "border-left: 1px solid #bbbbbb; border-top-right-radius: 1%; border-bottom-right-radius: 1%;}"
                                             "QComboBox::drop-down:focus {border-left: 2px solid #68e7f0; width: 20px; background-color: #68e7f0;}"
                                             "QComboBox::down-arrow {image: url(:/private mode images/downPrivateMode.png); height: 12px; width: 12px;}"
                                             "QComboBox::down-arrow:focus {image: url(:/images/down.png); height: 12px; width: 12px;}"));

    removeButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #2e686b; border: none; padding: 0px; border-radius: 3.5%;}"
                                              "QToolButton:hover {background-color: #347779; border: 1px solid #347779;}"
                                              "QToolButton:pressed {background-color: #448890; border: 1px solid #448890;}"));

    doneButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #68e7f0; border: none; padding: 0px; border-radius: 3.5%;}"
                                            "QToolButton:hover {background-color: #63dde3; border: 1px solid #63dde3;}"
                                            "QToolButton:pressed {background-color: #5dd0d6; border: 1px solid #5dd0d6;}"));

    folderFieldView->setStyleSheet(QLatin1String("QListView {background-color: #2e686b; padding: 0px; border-left: 2px solid #68e7f0; font-size: 14px;"
                                                 "border-right: 2px solid #68e7f0; border-bottom: 2px solid #68e7f0; border-top: none;"
                                                 "border-bottom-left-radius: 4%; border-bottom-right-radius: 4%; outline: none;}"
                                                 "QListView::item {color: #dddddd; height: 29px;}"
                                                 "QListView::item:hover {background-color: #1e4446; border-top: 1px solid #1e4446; border-bottom: 1px solid #1e4446;}"
                                                 "QListView::item:selected {background-color: #1e4446; border-top: 1px solid #1e4446; border-bottom: 1px solid #1e4446;}"));
}

void OBookmarkWidget::setDarkBrownTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #462c21; padding: 0px; border: 1px solid #462c21;}"));
    widgetForm->setStyleSheet(QLatin1String("QWidget {background-color: #462c21; padding: 0px; border: none;}"));
    widgetLabel->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                             "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    titleField->setStyleSheet(QLatin1String("QLineEdit {background-color: #6b4232; border: 1px solid #6b4232; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                            "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                            "QLineEdit:hover {border: 1px solid #ffa477;}"
                                            "QLineEdit:focus {border: 2px solid #ffa477; padding-left: 0px; selection-background-color: #68e7f0;}"
                                            "QLineEdit:focus:hover {border: 2px solid #ffa477; padding-left: 0px;}"));

    QColor placeholderColor(192, 192, 192);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    titleField->setPalette(inputPalette);

    folderField->setStyleSheet(QLatin1String("QComboBox {border: 1px solid #6b4232; border-radius: 4%; color: #909090; font-size: 14px;"
                                             "font-family:'Segoe UI'; padding-left: 2px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;"
                                             "background-color: #6b4232;}"
                                             "QComboBox:hover {border: 1px solid #ffa477;}"
                                             "QComboBox:focus {border: 2px solid #ffa477; padding-left: 0px; selection-background-color: #68e7f0;}"
                                             "QComboBox:on {border-left: 2px solid #ffa477; padding-left: 0px; selection-background-color: #ffa477;"
                                             "border-right: 2px solid #ffa477; border-bottom: none; border-top: 2px solid #ffa477;"
                                             "border-bottom-left-radius: 0px; border-bottom-right-radius: 0px;}"
                                             "QComboBox:focus:hover {border: 2px solid #ffa477; padding-left: 0px;}"
                                             "QComboBox::drop-down {subcontrol-origin: padding; subcontrol-position: top right; width: 20px;"
                                             "border-left: 1px solid #bbbbbb; border-top-right-radius: 1%; border-bottom-right-radius: 1%;}"
                                             "QComboBox::drop-down:focus {border-left: 2px solid #ffa477; width: 20px; background-color: #ffa477;}"
                                             "QComboBox::down-arrow {image: url(:/private mode images/downPrivateMode.png); height: 12px; width: 12px;}"));

    removeButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #6b4232; border: none; padding: 0px; border-radius: 3.5%;}"
                                              "QToolButton:hover {background-color: #794a38; border: 1px solid #794a38;}"
                                              "QToolButton:pressed {background-color: #905743; border: 1px solid #905743;}"));

    doneButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #ffa477; border: none; padding: 0px; border-radius: 3.5%;}"
                                            "QToolButton:hover {background-color: #e5926b; border: 1px solid #e5926b;}"
                                            "QToolButton:pressed {background-color: #d68864; border: 1px solid #d68864;}"));

    folderFieldView->setStyleSheet(QLatin1String("QListView {background-color: #6b4232; padding: 0px; border-left: 2px solid #ffa477; font-size: 14px;"
                                                 "border-right: 2px solid #ffa477; border-bottom: 2px solid #ffa477; border-top: none;"
                                                 "border-bottom-left-radius: 4%; border-bottom-right-radius: 4%; outline: none;}"
                                                 "QListView::item {color: #dddddd; height: 29px;}"
                                                 "QListView::item:hover {background-color: #462c21; border-top: 1px solid #462c21; border-bottom: 1px solid #462c21;}"
                                                 "QListView::item:selected {background-color: #462c21; border-top: 1px solid #462c21; border-bottom: 1px solid #462c21;}"));
}

void OBookmarkWidget::setPrivateTheme()
{
    setStyleSheet(QLatin1String("QMenu {background-color: #3c3244; padding: 0px; border: 1px solid #3c3244;}"));
    widgetForm->setStyleSheet(QLatin1String("QWidget {background-color: #3c3244; padding: 0px; border: none;}"));
    widgetLabel->setStyleSheet(QLatin1String("QLabel {border: none; background-color: transparent; color: #dddddd;"
                                             "font-family: gadugi; font-size: 14px; padding-bottom: 1px;}"));

    titleField->setStyleSheet(QLatin1String("QLineEdit {background-color: #5e4e6b; border: 1px solid #5e4e6b; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                            "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                            "QLineEdit:hover {border: 1px solid #c2a1dd;}"
                                            "QLineEdit:focus {border: 2px solid #c2a1dd; padding-left: 0px;}"
                                            "QLineEdit:focus:hover {border: 2px solid #c2a1dd; padding-left: 0px;}"));

    QColor placeholderColor(192, 192, 192);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    titleField->setPalette(inputPalette);

    folderField->setStyleSheet(QLatin1String("QComboBox {border: 1px solid #5e4e6b; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                             "font-family:'Segoe UI'; padding-left: 2px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;"
                                             "background-color: #5e4e6b;}"
                                             "QComboBox:hover {border: 1px solid #c2a1dd;}"
                                             "QComboBox:focus {border: 2px solid #c2a1dd; padding-left: 0px; selection-background-color: #c2a1dd;}"
                                             "QComboBox:on {border-left: 2px solid #c2a1dd; padding-left: 0px; selection-background-color: #c2a1dd;"
                                             "border-right: 2px solid #c2a1dd; border-bottom: none; border-top: 2px solid #c2a1dd;"
                                             "border-bottom-left-radius: 0px; border-bottom-right-radius: 0px;}"
                                             "QComboBox:focus:hover {border: 2px solid #c2a1dd; padding-left: 0px;}"
                                             "QComboBox::drop-down {subcontrol-origin: padding; subcontrol-position: top right; width: 20px;"
                                             "border-left: 1px solid #bbbbbb; border-top-right-radius: 1%; border-bottom-right-radius: 1%;}"
                                             "QComboBox::drop-down:focus {border-left: 2px solid #c2a1dd; width: 20px; background-color: #c2a1dd;}"
                                             "QComboBox::down-arrow {image: url(:/private mode images/downPrivateMode.png); height: 12px; width: 12px;}"));

    removeButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #5e4e6b; border: none; padding: 0px; border-radius: 3.5%;}"
                                              "QToolButton:hover {background-color: #695878; border: 1px solid #695878;}"
                                              "QToolButton:pressed {background-color: #79668b; border: 2px solid #79668b;}"));

    doneButton->setStyleSheet(QLatin1String("QToolButton {color: #ffffff; background-color: #c2a1dd; border: none; padding: 0px; border-radius: 3.5%;}"
                                            "QToolButton:hover {background-color: #b595ce; border: 1px solid #b898d1;}"
                                            "QToolButton:pressed {background-color: #a68bbe; border: 2px solid #ad90c5;}"));

    folderFieldView->setStyleSheet(QLatin1String("QListView {background-color: #5e4e6b; padding: 0px; border-left: 2px solid #c2a1dd; font-size: 14px;"
                                                 "border-right: 2px solid #c2a1dd; border-bottom: 2px solid #c2a1dd; border-top: none;"
                                                 "border-bottom-left-radius: 4%; border-bottom-right-radius: 4%; outline: none;}"
                                                 "QListView::item {color: #dddddd; height: 29px;}"
                                                 "QListView::item:hover {background-color: #3c3244; border-top: 1px solid #3c3244; border-bottom: 1px solid #3c3244;}"
                                                 "QListView::item:selected {background-color: #3c3244; border-top: 1px solid #3c3244; border-bottom: 1px solid #3c3244;}"));
}

void OBookmarkWidget::setBookmarkManager(OBookmarksCore *manager)
{
    bookmarkManager = manager;
}

void OBookmarkWidget::setTabPage(OTabPage *tabWidget)
{
    tabPage = tabWidget;
}

void OBookmarkWidget::addBookmark()
{
    done = true;
    hide();
    const QString url = parseUrl(tabPage->getStackPage()->currentWidget()->findChild<OWebView*>()->url().toEncoded());
    bookmarkManager->updateBookmarkItem(titleField->text(), url, folderField->currentText());
}

void OBookmarkWidget::removeBookmark()
{
    removed = true;
    hide();
    const QString encodedUrl = parseUrl(tabPage->getStackPage()->currentWidget()->findChild<OWebView*>()->url().toEncoded());
    const QUrl url = QUrl::fromEncoded(encodedUrl.toLatin1());
    bookmarkManager->deleteBookmarkItem(titleField->text(), url, folderField->currentText());
    if (parentButton->isChecked())
        parentButton->setChecked(false);
}

void OBookmarkWidget::setParentButton(QToolButton *pButton)
{
    parentButton = pButton;
}

bool OBookmarkWidget::isBookmarked(const QString &url)
{
    return bookmarkManager->itemExist(url);
}

const QString OBookmarkWidget::parseUrl(const QString &url)
{
    QString historyUrl = QLatin1String("qrc:/web%20applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web%20applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web%20applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web%20applications/html/settingsView.html");
    QString newTabUrl = QLatin1String("qrc:/web%20applications/html/newtabView.html");
    QString privateTabUrl = QLatin1String("qrc:/web%20applications/html/privatetabView.html");
    QString newUrl = url;

    if (url.compare(historyUrl) == 0)
        newUrl = "ololan://history";

    if (url.compare(bookmarksUrl) == 0)
        newUrl = "ololan://bookmarks";

    if (url.compare(downloadsUrl) == 0)
        newUrl = "ololan://downloads";

    if (url.compare(settingsUrl) == 0)
        newUrl = "ololan://settings";

    if ((newUrl.compare(newTabUrl) == 0) || (newUrl.compare(privateTabUrl) == 0))
        newUrl = "ololan://home";

    return newUrl;
}
