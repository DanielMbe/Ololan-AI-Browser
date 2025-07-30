#ifndef OBOOKMARKWIDGET_H
#define OBOOKMARKWIDGET_H

#include <QMenu>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QToolButton>

class OBookmarksCore;
class OTabPage;
class OInputCompleter;
class OBookmarkWidget : public QMenu
{
    Q_OBJECT

public:
    OBookmarkWidget(QWidget *parent = nullptr);
    void setupControl();
    void setLightGrayTheme();
    void setLightTurquoiseTheme();
    void setLightBrownTheme();
    void setDarkGrayTheme();
    void setDarkTurquoiseTheme();
    void setDarkBrownTheme();
    void setPrivateTheme();
    void setBookmarkManager(OBookmarksCore *manager);
    void setTabPage(OTabPage *tabWidget);
    void setParentButton(QToolButton *pButton);
    bool isBookmarked(const QString &url);
    const QString parseUrl(const QString &url);

public slots:
    void addBookmark();
    void removeBookmark();

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    QLabel *widgetLabel;
    QLineEdit *titleField;
    QComboBox *folderField;
    QWidget *widgetForm;
    QToolButton *removeButton;
    QToolButton *doneButton;
    QToolButton *parentButton;
    OBookmarksCore *bookmarkManager;
    OTabPage *tabPage;
    OInputCompleter *folderFieldView;
    bool removed;
    bool done;
};

#endif // OBOOKMARKWIDGET_H
