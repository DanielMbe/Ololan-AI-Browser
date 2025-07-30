#include "oinputcompleter.h"
#include <QStandardItem>
#include<QScrollBar>

OInputCompleter::OInputCompleter(QWidget *parent)
    :QListView(parent)
{
    modelList = new QStandardItemModel(0, 1);
    setModel(modelList);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWindowFlags(Qt::ToolTip | Qt::WindowSystemMenuHint);
}

QStandardItemModel *OInputCompleter::getModel()
{
    return modelList;
}

void OInputCompleter::setLightGrayTheme()
{
    setStyleSheet(QLatin1String("QListView {background-color: #ffffff; padding: 0px; border-left: 2px solid #68e7f0; font-size: 14px;"
                                "border-right: 2px solid #68e7f0; border-bottom: 2px solid #68e7f0; border-top: none;"
                                "border-bottom-left-radius: 4%; border-bottom-right-radius: 4%; outline: none;}"
                                "QListView::item {color: #888888; height: 37px;}"
                                "QListView::item:hover {background-color: #efeff1; border-top: 1px solid #efeff1; border-bottom: 1px solid #efeff1;}"
                                "QListView::item:selected {background-color: #efeff1; border-top: 1px solid #efeff1; border-bottom: 1px solid #efeff1;}"));
}

void OInputCompleter::setLightTurquoiseTheme()
{
    setStyleSheet(QLatin1String("QListView {background-color: #ffffff; padding: 0px; border-left: 2px solid #68e7f0; font-size: 14px;"
                                "border-right: 2px solid #68e7f0; border-bottom: 2px solid #68e7f0; border-top: none;"
                                "border-bottom-left-radius: 4%; border-bottom-right-radius: 4%; outline: none;}"
                                "QListView::item {color: #888888; height: 37px;}"
                                "QListView::item:hover {background-color: #69e8f1; border-top: 1px solid #69e8f1; border-bottom: 1px solid #69e8f1;}"
                                "QListView::item:selected {background-color: #69e8f1; border-top: 1px solid #69e8f1; border-bottom: 1px solid #69e8f1;}"));
}

void OInputCompleter::setLightBrownTheme()
{
    setStyleSheet(QLatin1String("QListView {background-color: #ffffff; padding: 0px; border-left: 2px solid #ffa477; font-size: 14px;"
                                "border-right: 2px solid #ffa477; border-bottom: 2px solid #ffa477; border-top: none;"
                                "border-bottom-left-radius: 4%; border-bottom-right-radius: 4%; outline: none;}"
                                "QListView::item {color: #888888; height: 37px;}"
                                "QListView::item:hover  {background-color: #ffa477; border-top: 1px solid #ffa477; border-bottom: 1px solid #ffa477;}"
                                "QListView::item:selected {background-color: #ffa477; border-top: 1px solid #ffa477; border-bottom: 1px solid #ffa477;}"));
}

void OInputCompleter::setDarkGrayTheme()
{
    setStyleSheet(QLatin1String("QListView {background-color: #434344; padding: 0px; border-left: 2px solid #68e7f0; font-size: 14px;"
                                "border-right: 2px solid #68e7f0; border-bottom: 2px solid #68e7f0; border-top: none;"
                                "border-bottom-left-radius: 4%; border-bottom-right-radius: 4%; outline: none;}"
                                "QListView::item {color: #dddddd; height: 37px;}"
                                "QListView::item:hover {background-color: #6b6b6b; border-top: 1px solid #6b6b6b; border-bottom: 1px solid #6b6b6b;}"
                                "QListView::item:selected {background-color: #6b6b6b; border-top: 1px solid #6b6b6b; border-bottom: 1px solid #6b6b6b;}"));
}

void OInputCompleter::setDarkTurquoiseTheme()
{
    setStyleSheet(QLatin1String("QListView {background-color: #1e4446; padding: 0px; border-left: 2px solid #68e7f0; font-size: 14px;"
                                "border-right: 2px solid #68e7f0; border-bottom: 2px solid #68e7f0; border-top: none;"
                                "border-bottom-left-radius: 4%; border-bottom-right-radius: 4%; outline: none;}"
                                "QListView::item {color: #dddddd; height: 37px;}"
                                "QListView::item:hover {background-color: #2e686b; border-top: 1px solid #2e686b; border-bottom: 1px solid #2e686b;}"
                                "QListView::item:selected {background-color: #2e686b; border-top: 1px solid #2e686b; border-bottom: 1px solid #2e686b;}"));
}

void OInputCompleter::setDarkBrownTheme()
{
    setStyleSheet(QLatin1String("QListView {background-color: #462c21; padding: 0px; border-left: 2px solid #ffa477; font-size: 14px;"
                                "border-right: 2px solid #ffa477; border-bottom: 2px solid #ffa477; border-top: none;"
                                "border-bottom-left-radius: 4%; border-bottom-right-radius: 4%; outline: none;}"
                                "QListView::item {color: #dddddd; height: 37px;}"
                                "QListView::item:hover {background-color: #6b4132; border-top: 1px solid #6b4132; border-bottom: 1px solid #6b4132;}"
                                "QListView::item:selected {background-color: #6b4132; border-top: 1px solid #6b4132; border-bottom: 1px solid #6b4132;}"));
}

void OInputCompleter::setPrivateTheme()
{
    setStyleSheet(QLatin1String("QListView {background-color: #3c3244; padding: 0px; border-left: 2px solid #c2a1dd; font-size: 14px;"
                                "border-right: 2px solid #c2a1dd; border-bottom: 2px solid #c2a1dd; border-top: none;"
                                "border-bottom-left-radius: 4%; border-bottom-right-radius: 4%; outline: none;}"
                                "QListView::item {color: #dddddd; height: 37px;}"
                                "QListView::item:hover {background-color: #5e4e6b; border-top: 1px solid #5e4e6b; border-bottom: 1px solid #5e4e6b;}"
                                "QListView::item:selected {background-color: #5e4e6b; border-top: 1px solid #5e4e6b; border-bottom: 1px solid #5e4e6b;}"));
}
