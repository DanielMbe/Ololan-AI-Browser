#ifndef OINPUTCOMPLETER_H
#define OINPUTCOMPLETER_H

#include <QListView>
#include <QObject>
#include <QStandardItemModel>

class OInputCompleter : public QListView
{
    Q_OBJECT

public:
    explicit OInputCompleter(QWidget *parent = nullptr);
    void setLightGrayTheme();
    void setLightTurquoiseTheme();
    void setLightBrownTheme();
    void setDarkGrayTheme();
    void setDarkTurquoiseTheme();
    void setDarkBrownTheme();
    void setPrivateTheme();
    QStandardItemModel *getModel();

private:
    QStandardItemModel *modelList;
};

#endif // OINPUTCOMPLETER_H
