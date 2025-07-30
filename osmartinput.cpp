#include "osmartinput.h"
#include "ohistorycore.h"
#include "oinputcompleter.h"
#include "owebview.h"
#include "otabpage.h"
#include "owebsiteinfo.h"
#include "oappcore.h"
#include "customwindow.h"
#include <QWidgetAction>
#include <QUrl>
#include <QCompleter>
#include <QKeyEvent>
#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QTimer>

OSmartInput::OSmartInput(QWidget *parent)
    :QLineEdit(parent)
{
    hasCertificateError = false;
    searchEngineName = "";
    searchEngine = "";
    ololanTheme = "";
    newUrl = "";
    autoSuggestUrl = QLatin1String("https://www.google.com/complete/search?output=toolbar&q=%1");
    networkManager = new QNetworkAccessManager(this);

    completerView = new OInputCompleter();
    winType = WindowType::Normal;

    securityButton = new CustomToolButton(this);
    securityButton->setToolTip("Website informations");
    securityButton->setFixedSize(23, 19);

    websiteInfo = new OWebsiteInfo(securityButton);
    securityButton->setPopupMode(QToolButton::InstantPopup);

    searchIcon = new CustomToolButton(this);
    searchIcon->setFixedSize(19, 19);

    bookmarkButton = new CustomToolButton(this);
    bookmarkButton->setToolTip("Bookmark this tab");
    bookmarkButton->setCheckable(true);
    bookmarkButton->setFixedSize(19, 19);

    bookmarkMenu = new OBookmarkWidget(this);
    bookmarkButton->setMenu(bookmarkMenu);
    bookmarkButton->setPopupMode(QToolButton::InstantPopup);
    bookmarkMenu->setParentButton(bookmarkButton);

    QWidgetAction *securityWidget = new QWidgetAction(this);
    securityWidget->setDefaultWidget(securityButton);
    QWidgetAction *searchIconWidget = new QWidgetAction(this);
    searchIconWidget->setDefaultWidget(searchIcon);
    QWidgetAction *bookmarkButtonWidget = new QWidgetAction(this);
    bookmarkButtonWidget->setDefaultWidget(bookmarkButton);

    addAction(securityWidget, QLineEdit::LeadingPosition);
    addAction(searchIconWidget, QLineEdit::LeadingPosition);
    addAction(bookmarkButtonWidget, QLineEdit::TrailingPosition);
    setPlaceholderText(tr("Type here to browse the web"));

    connect(this, &OSmartInput::returnPressed, this, &OSmartInput::validateInput);
    connect(this, &OSmartInput::textEdited, this, &OSmartInput::detectTyping);
    connect(completerView, &OInputCompleter::clicked, this, &OSmartInput::processCompletion);
    connect(networkManager, &QNetworkAccessManager::finished, this, &OSmartInput::autoCompleteSuggestion);
}

void OSmartInput::validateInput()
{
    if (!text().isEmpty())
    {
        QString textUrl = text();
        OTabPage *tabPage = static_cast<OTabPage*>(parentWidget()->parentWidget()->parentWidget()->parentWidget());
        OAppCore *app = static_cast<OAppCore*>(tabPage->getParentWindow()->getParentApplication());

        if (textUrl.compare("ololan://history") == 0)
            app->openHistory();
        else if (textUrl.compare("ololan://bookmarks") == 0)
            app->openBookmarks();
        else if (textUrl.compare("ololan://downloads") == 0)
            app->openDownloads();
        else if (textUrl.compare("ololan://settings") == 0)
            app->openSettings();
        else if (textUrl.compare("ololan://home") == 0)
        {
            if (winType == WindowType::Normal)
                emit validated("qrc:/web_applications/html/newtabView.html");
            if (winType == WindowType::Private)
                emit validated("qrc:/web_applications/html/privatetabView.html");
        }
        else
        {
            UrlType typing = aboutTyping(textUrl);
            if (typing == SearchType)
            {
                QString searchQuery = searchEngine;
                textUrl = searchQuery.arg(textUrl);
            }

            setText(QUrl::fromUserInput(textUrl).toEncoded());
            setUrlColor(QUrl::fromUserInput(textUrl).toEncoded());
            emit validated(textUrl);
        }
        clearFocus();
    }
}

void OSmartInput::setUrlColor(const QString webUrl)
{
    const QUrl url = QUrl::fromEncoded(webUrl.toLatin1());
    QColor charColor;

    if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
        (ololanTheme.compare("Light brown") == 0))
        charColor = QColor(53, 53, 53);
    else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
             (ololanTheme.compare("Dark brown") == 0) || (ololanTheme.compare("Private") == 0))
        charColor = QColor(221, 221, 221);

    QBrush charBrush(charColor);
    QTextCharFormat charFormat;
    charFormat.setForeground(charBrush);

    if ((webUrl.compare("ololan://home") == 0) || (webUrl.compare("ololan://settings") == 0) ||
        (webUrl.compare("ololan://downloads") == 0) || (webUrl.compare("ololan://bookmarks") == 0) ||
        (webUrl.compare("ololan://history") == 0))
    {
        const int position = 9 - cursorPosition();
        const int length = webUrl.split("//").last().length();
        QList<QInputMethodEvent::Attribute> attributes;
        attributes.reserve(1);
        attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, position, length, charFormat));
        QLineEdit::inputMethodEvent(new QInputMethodEvent(QString(), attributes));
    }
    else if (aboutTyping(webUrl) == WebsiteType)
    {
        const int position = url.toString().indexOf(url.host()) - cursorPosition();
        QList<QInputMethodEvent::Attribute> attributes;
        attributes.reserve(1);
        attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, position, url.host().length(), charFormat));
        QLineEdit::inputMethodEvent(new QInputMethodEvent(QString(), attributes));
    }
    else
    {
        const int position = 0 - cursorPosition();
        QList<QInputMethodEvent::Attribute> attributes;
        attributes.reserve(1);
        attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, position, webUrl.length(), charFormat));
        QLineEdit::inputMethodEvent(new QInputMethodEvent(QString(), attributes));
    }
}

void OSmartInput::autoSuggest(const QString query)
{
    QString url = autoSuggestUrl.arg(query);
    networkManager->get(QNetworkRequest(url));
}

void OSmartInput::updateCompleter(const QString &query, UrlType queryType)
{
    QStandardItemModel *completerModel = completerView->getModel();
    completerModel->removeRows(0, completerModel->rowCount());

    if (!query.isEmpty())
    {
        QStandardItem *item = new QStandardItem();
        if (queryType == SearchType)
        {
            item->setText(query + QString(" -- ") + searchEngineName + QString(" search"));

            if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
                (ololanTheme.compare("Light brown") == 0))
                item->setIcon(QIcon(":/images/search.png"));
            else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
                     (ololanTheme.compare("Dark brown") == 0) || (ololanTheme.compare("Private") == 0))
                item->setIcon(QIcon(":/private_mode_images/searchPrivateMode.png"));
        }
        else if (queryType == WebsiteType)
        {
            item->setText(query);

            if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
                (ololanTheme.compare("Light brown") == 0))
                item->setIcon(QIcon(":/images/urlTyped.png"));
            else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
                     (ololanTheme.compare("Dark brown") == 0) || (ololanTheme.compare("Private") == 0))
                item->setIcon(QIcon(":/private_mode_images/urlTypedPrivateMode.png"));
        }

        completerModel->appendRow(item);
        completerView->setCurrentIndex(completerModel->index(0, 0));
    }

    showCompleter();
    autoSuggest(query);
}

void OSmartInput::autoCompleteSuggestion(QNetworkReply *reply)
{
    if (!hasFocus())
        return;

    QStandardItemModel *completerModel = completerView->getModel();
    QVector<QString> choices;
    if (reply->error() == QNetworkReply::NoError)
    {
        int count = 0;
        QByteArray response(reply->readAll());
        QXmlStreamReader xml(response);
        while (!xml.atEnd() || (count < 7))
        {
            count++;
            xml.readNext();
            if (xml.tokenType() == QXmlStreamReader::StartElement)
                if (xml.name().toString() == "suggestion")
                {
                    QStringView str = xml.attributes().value("data");
                    choices << str.toString();
                    QStandardItem *item = new QStandardItem();
                    item->setText(str.toString());

                    if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
                        (ololanTheme.compare("Light brown") == 0))
                        item->setIcon(QIcon(":/images/search.png"));
                    else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
                             (ololanTheme.compare("Dark brown") == 0) || (ololanTheme.compare("Private") == 0))
                        item->setIcon(QIcon(":/private_mode_images/searchPrivateMode.png"));

                    if (choices.count() < 7)
                        completerModel->appendRow(item);
                }
        }
    }

    if (choices.count() < 7)
    {
        for (int i = 0; i < browserHistory->getHistoryItemsList().count(); i++)
        {
            if (i == (7-choices.count()))
                break;

            if (browserHistory->getHistoryItemsList().at(i)->getTitle().startsWith(text(), Qt::CaseInsensitive) && !text().isEmpty())
            {
                QStandardItem *item = new QStandardItem();
                item->setText(browserHistory->getHistoryItemsList().at(i)->getTitle() + QString(" -- ") +
                              browserHistory->getHistoryItemsList().at(i)->getUrl().toString());

                if (browserHistory->getHistoryItemsList().at(i)->getIcon() != "EmPtY64")
                {
                    QPixmap icon = QPixmap(16, 16);
                    icon.loadFromData(QByteArray::fromBase64(browserHistory->getHistoryItemsList().at(i)->getIcon()), "PNG");
                    item->setIcon(QIcon(icon));
                }
                else
                    item->setIcon(QIcon(":/images/webpage.png"));
                completerModel->appendRow(item);
            }
        }

        if (completerModel->rowCount() < 8)
            for (int i = completerModel->rowCount(); i < browserHistory->getHistoryItemsList().count(); i++)
            {
                if (browserHistory->getHistoryItemsList().at(i)->getUrl().toString().contains(text(), Qt::CaseInsensitive)
                    && (completerModel->rowCount() < 8) && !text().isEmpty())
                {
                    QStandardItem *item = new QStandardItem();
                    item->setText(browserHistory->getHistoryItemsList().at(i)->getTitle() + QString(" -- ") +
                                  browserHistory->getHistoryItemsList().at(i)->getUrl().toString());

                    if (browserHistory->getHistoryItemsList().at(i)->getIcon() != "EmPtY64")
                    {
                        QPixmap icon = QPixmap(16, 16);
                        icon.loadFromData(QByteArray::fromBase64(browserHistory->getHistoryItemsList().at(i)->getIcon()), "PNG");
                        item->setIcon(QIcon(icon));
                    }
                    else
                        item->setIcon(QIcon(":/images/webpage.png"));
                    completerModel->appendRow(item);
                }

                if (completerModel->rowCount() == 8)
                    break;
            }
    }

    completerView->setCurrentIndex(completerModel->index(0, 0));
    showCompleter();
}

void OSmartInput::showCompleter()
{
    if (completerView->getModel()->rowCount() > 0)
    {
        const int completerHeight = (37 * completerView->getModel()->rowCount()) + 3;
        QPoint globalPos = mapToGlobal(pos());
        QRect popupGeometry = geometry();
        const int distanceWidth = 121;
        popupGeometry.setCoords(globalPos.x() - distanceWidth, globalPos.y() + 28, globalPos.x() - distanceWidth + width() - 1,
                                globalPos.y() + 28 + completerHeight);

        completerView->setGeometry(popupGeometry);
        if (completerView->isHidden())
            completerView->show();
    }
    else
    {
        if (completerView->isVisible())
            completerView->hide();
    }
}

void OSmartInput::completeHighlightedMatch(const QModelIndex &index)
{
    const QString textValue = index.data(Qt::DisplayRole).toString();
    QStringList activeMatches = textValue.split(" -- ");

    if (textValue.endsWith(searchEngineName + QString(" search")))
        setText(activeMatches.first());
    else
        setText(activeMatches.last());
}

void OSmartInput::processCompletion(const QModelIndex &index)
{
    completeHighlightedMatch(index);
    validateInput();
}

void OSmartInput::detectTyping(const QString &text)
{
    UrlType result = aboutTyping(text);

    if (winType == WindowType::Normal)
    {
        if (ololanTheme.compare("Light gray") == 0)
            setWebIconLightGrayTheme(result, true);
        else if (ololanTheme.compare("Light turquoise") == 0)
            setWebIconLightTurquoiseTheme(result, true);
        else if (ololanTheme.compare("Light brown") == 0)
            setWebIconLightBrownTheme(result, true);
        else if (ololanTheme.compare("Dark gray") == 0)
            setWebIconDarkGrayTheme(result, true);
        else if (ololanTheme.compare("Dark turquoise") == 0)
            setWebIconDarkTurquoiseTheme(result, true);
        else if (ololanTheme.compare("Dark brown") == 0)
            setWebIconDarkBrownTheme(result, true);
    }
    else if (winType == WindowType::Private)
        setWebIconPrivateTheme(result, true);

    updateCompleter(text, result);
}

UrlType OSmartInput::aboutTyping(const QString typing)
{
    if (typing.contains(" ") || typing.isEmpty())
        return SearchType;
    else if (typing.startsWith("https://") || typing.startsWith("http://") || typing.startsWith("ftp://") ||
             typing.startsWith("file://"))
    {
        QUrl typedUrl = QUrl::fromUserInput(typing);
        if (typedUrl.isValid())
            return WebsiteType;
        else
            return SearchType;
    }
    else if (typing.startsWith("ololan://"))
        return WebsiteType;
    else
    {
        QUrl typedUrl = QUrl::fromUserInput(typing);
        if (typedUrl.isValid() && typedUrl.host().contains("."))
            return WebsiteType;
        else
            return SearchType;
    }
}

void OSmartInput::setHistory(OHistoryCore *history)
{
    browserHistory = history;
}

void OSmartInput::setLightGrayTheme()
{
    ololanTheme = "Light gray";
    winType = WindowType::Normal;
    setFocusOutLightGrayTheme();
    setWebIconLightGrayTheme(aboutTyping(text()), false);

    QColor placeholderColor(147, 147, 147);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    setPalette(inputPalette);

    completerView->setLightGrayTheme();
    bookmarkMenu->setLightGrayTheme();
    websiteInfo->setLightGrayTheme();
}

void OSmartInput::setLightTurquoiseTheme()
{
    ololanTheme = "Light turquoise";
    winType = WindowType::Normal;
    setFocusOutLightTurquoiseTheme();
    setWebIconLightTurquoiseTheme(aboutTyping(text()), false);

    QColor placeholderColor(147, 147, 147);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    setPalette(inputPalette);

    completerView->setLightTurquoiseTheme();
    bookmarkMenu->setLightTurquoiseTheme();
    websiteInfo->setLightTurquoiseTheme();
}

void OSmartInput::setLightBrownTheme()
{
    ololanTheme = "Light brown";
    winType = WindowType::Normal;
    setFocusOutLightBrownTheme();
    setWebIconLightBrownTheme(aboutTyping(text()), false);

    QColor placeholderColor(147, 147, 147);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    setPalette(inputPalette);

    completerView->setLightBrownTheme();
    bookmarkMenu->setLightBrownTheme();
    websiteInfo->setLightBrownTheme();
}

void OSmartInput::setDarkGrayTheme()
{
    ololanTheme = "Dark gray";
    winType = WindowType::Normal;
    setFocusOutDarkGrayTheme();
    setWebIconDarkGrayTheme(aboutTyping(text()), false);

    QColor placeholderColor(160, 160, 160);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    setPalette(inputPalette);

    completerView->setDarkGrayTheme();
    bookmarkMenu->setDarkGrayTheme();
    websiteInfo->setDarkGrayTheme();
}

void OSmartInput::setDarkTurquoiseTheme()
{
    ololanTheme = "Dark turquoise";
    winType = WindowType::Normal;
    setFocusOutDarkTurquoiseTheme();
    setWebIconDarkTurquoiseTheme(aboutTyping(text()), false);

    QColor placeholderColor(160, 160, 160);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    setPalette(inputPalette);

    completerView->setDarkTurquoiseTheme();
    bookmarkMenu->setDarkTurquoiseTheme();
    websiteInfo->setDarkTurquoiseTheme();
}

void OSmartInput::setDarkBrownTheme()
{
    ololanTheme = "Dark brown";
    winType = WindowType::Normal;
    setFocusOutDarkBrownTheme();
    setWebIconDarkBrownTheme(aboutTyping(text()), false);

    QColor placeholderColor(160, 160, 160);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    setPalette(inputPalette);

    completerView->setDarkBrownTheme();
    bookmarkMenu->setDarkBrownTheme();
    websiteInfo->setDarkBrownTheme();
}

void OSmartInput::setPrivateTheme()
{
    ololanTheme = "Private";
    winType = WindowType::Private;
    setFocusOutPrivateTheme();
    setWebIconPrivateTheme(aboutTyping(text()), false);

    QColor placeholderColor(160, 160, 160);
    QPalette inputPalette = palette();
    inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
    setPalette(inputPalette);

    completerView->setPrivateTheme();
    bookmarkMenu->setPrivateTheme();
    websiteInfo->setPrivateTheme();
}

void OSmartInput::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    UrlType result = aboutTyping(text());

    if (winType == WindowType::Normal)
    {
        if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
            (ololanTheme.compare("Light brown") == 0))
        {
            if (ololanTheme.compare("Light gray") == 0)
            {
                setFocusInLightGrayTheme();
                setWebIconLightGrayTheme(result, true);
            }
            else if (ololanTheme.compare("Light turquoise") == 0)
            {
                setFocusInLightTurquoiseTheme();
                setWebIconLightTurquoiseTheme(result, true);
            }
            else if (ololanTheme.compare("Light brown") == 0)
            {
                setFocusInLightBrownTheme();
                setWebIconLightBrownTheme(result, true);
            }

            QColor placeholderColor(147, 147, 147);
            QPalette inputPalette = palette();
            inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
            setPalette(inputPalette);
        }
        else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
                 (ololanTheme.compare("Dark brown") == 0))
        {
            if (ololanTheme.compare("Dark gray") == 0)
            {
                setFocusInDarkGrayTheme();
                setWebIconDarkGrayTheme(result, true);
            }
            else if (ololanTheme.compare("Dark turquoise") == 0)
            {
                setFocusInDarkTurquoiseTheme();
                setWebIconDarkTurquoiseTheme(result, true);
            }
            else if (ololanTheme.compare("Dark brown") == 0)
            {
                setFocusInDarkBrownTheme();
                setWebIconDarkBrownTheme(result, true);
            }

            QColor placeholderColor(160, 160, 160);
            QPalette inputPalette = palette();
            inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
            setPalette(inputPalette);
        }
    }
    else if (winType == WindowType::Private)
    {
        setFocusInPrivateTheme();
        setWebIconPrivateTheme(result, true);

        QColor placeholderColor(160, 160, 160);
        QPalette inputPalette = palette();
        inputPalette.setColor(QPalette::PlaceholderText, placeholderColor);
        setPalette(inputPalette);
    }

    QTimer::singleShot(100, this, &OSmartInput::selectAll);
}

void OSmartInput::focusOutEvent(QFocusEvent *event)
{
    if (winType == WindowType::Normal)
    {
        if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
            (ololanTheme.compare("Light brown") == 0))
        {
            if (ololanTheme.compare("Light gray") == 0)
                setLightGrayTheme();
            else if (ololanTheme.compare("Light turquoise") == 0)
                setLightTurquoiseTheme();
            else if (ololanTheme.compare("Light brown") == 0)
                setLightBrownTheme();
        }
        else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
                 (ololanTheme.compare("Dark brown") == 0))
        {
            if (ololanTheme.compare("Dark gray") == 0)
                setDarkGrayTheme();
            else if (ololanTheme.compare("Dark turquoise") == 0)
                setDarkTurquoiseTheme();
            else if (ololanTheme.compare("Dark brown") == 0)
                setDarkBrownTheme();
        }
    }
    else if (winType == WindowType::Private)
        setPrivateTheme();

    if (completerView->isVisible())
        completerView->hide();

    if (!newUrl.isEmpty())
        updateUrl(webViewPage->url());

    if (!text().isEmpty())
        setUrlColor(text());

    QLineEdit::focusOutEvent(event);
}

void OSmartInput::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Down)
    {
        const int currentRow = completerView->currentIndex().row();
        if (currentRow != (completerView->getModel()->rowCount() - 1))
        {
            completerView->setCurrentIndex(completerView->getModel()->index(currentRow + 1, 0));
            completeHighlightedMatch(completerView->currentIndex());
        }
    }
    else if (event->key() == Qt::Key_Up)
    {
        const int currentRow = completerView->currentIndex().row();
        if (currentRow != 0)
        {
            completerView->setCurrentIndex(completerView->getModel()->index(currentRow - 1, 0));
            completeHighlightedMatch(completerView->currentIndex());
        }
    }

    QLineEdit::keyPressEvent(event);
}

void OSmartInput::setSearchEngine(const QString engine)
{
    searchEngineName = (engine.contains("#*#") ? engine.split("#*#").first() : engine);
    if (searchEngineName.compare("Google") == 0)
        searchEngine = QLatin1String("https://www.google.com/search?q=%1&oq=%1&client=ololan&sourceid=chrome&ie=UTF-8");
    else if (searchEngineName.compare("Bing") == 0)
        searchEngine = QLatin1String("https://www.bing.com/search?q=%1&FORM=OLOLANBR&PC=OLOLU");
    else if (searchEngineName.compare("Yahoo") == 0)
        searchEngine = QLatin1String("https://search.yahoo.com/search?ei=UTF-8&fr=crmas_sfp&p=%1");
    else if (searchEngineName.compare("DuckDuckGo") == 0)
        searchEngine = QLatin1String("https://duckduckgo.com/?q=%1");
    else
    {
        searchEngine = engine.split("#*#").last();
        if (!searchEngine.startsWith("https://") || !searchEngine.startsWith("http://") ||
            !searchEngine.startsWith("ftp://") || !searchEngine.startsWith("file://"))
            searchEngine = searchEngine.prepend("https://");
    }

    if (isVisible())
        if (webViewPage->url().toEncoded().compare("qrc:/web%20applications/html/newtabView.html") == 0)
            webViewPage->runJavaScript(QLatin1String("setCurrentPlaceholder('%1')").arg(searchEngineName));
}

OInputCompleter *OSmartInput::getCompleter()
{
    return completerView;
}

OBookmarkWidget *OSmartInput::getBookmarkWidget()
{
    return bookmarkMenu;
}

void OSmartInput::updateUrl(const QUrl &url)
{
    newUrl = url.toEncoded();
    QString historyUrl = QLatin1String("qrc:/web_applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web_applications/html/settingsView.html");
    QString newTabUrl = QLatin1String("qrc:/web_applications/html/newtabView.html");
    QString privateTabUrl = QLatin1String("qrc:/web_applications/html/privatetabView.html");

    if (newUrl.compare(historyUrl) == 0)
        newUrl = "ololan://history";

    if (newUrl.compare(bookmarksUrl) == 0)
        newUrl = "ololan://bookmarks";

    if (newUrl.compare(downloadsUrl) == 0)
        newUrl = "ololan://downloads";

    if (newUrl.compare(settingsUrl) == 0)
        newUrl = "ololan://settings";

    if ((newUrl.compare(newTabUrl) == 0) || (newUrl.compare(privateTabUrl) == 0))
        newUrl = "ololan://home";

    if (newUrl.startsWith("https://accounts.ololan.com/checkaccount", Qt::CaseInsensitive))
        newUrl = "https://accounts.ololan.com/checkaccount";

    if (bookmarkMenu->isBookmarked(newUrl))
    {
        if (!bookmarkButton->isChecked())
            bookmarkButton->setChecked(true);
    }
    else if (bookmarkButton->isChecked())
            bookmarkButton->setChecked(false);

    if (!url.userName().isEmpty() && !url.password().isEmpty())
    {
        websiteInfo->updateAuthentificationInfo(url.userName(), url);
        securityButton->click();
    }

    if (!hasFocus())
    {
        if (isSecureUrl(newUrl))
        {
            if ((newUrl.compare("ololan://home") == 0) || (newUrl.compare("ololan://settings") == 0) ||
                (newUrl.compare("ololan://downloads") == 0) || (newUrl.compare("ololan://bookmarks") == 0) ||
                (newUrl.compare("ololan://history") == 0))
            {
                websiteInfo->updateWebsiteInfo(newUrl, true);
                setIconSecureStyle();
            }
            else if (!hasCertificateError)
            {
                websiteInfo->updateWebsiteInfo(url.host(), true);
                setIconSecureStyle();
            }
        }
        else
        {
            setIconUnsecureStyle();
            websiteInfo->updateWebsiteInfo(url.host(), false);
        }

        securityButton->setMenu(websiteInfo);

        if (newUrl.compare("ololan://home") == 0)
        {
            newUrl = "";
            clear();
        }

        if (!newUrl.isEmpty())
        {
            clear();
            setText(newUrl);
            setCursorPosition(0);
            setUrlColor(newUrl);
            newUrl = "";
        }
    }
}

void OSmartInput::setPermissionInfo(const QString website, const QString permission, const QUrl securityOrigin)
{
    websiteInfo->updatePermissionInfo(website, permission, securityOrigin);
    securityButton->click();
}

bool OSmartInput::isSecureUrl(const QString url)
{
    if (url.startsWith("https://") || url.startsWith("ololan://"))
        return true;
    else
        return false;
}

void OSmartInput::certificateSecurity(const QWebEngineCertificateError &certificateError)
{
    hasCertificateError = true;
    OWebPage *webPage = qobject_cast<OWebPage*>(sender());
    QString historyUrl = QLatin1String("qrc:/web_applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web_applications/html/settingsView.html");
    QString newTabUrl = QLatin1String("qrc:/web_applications/html/newtabView.html");
    QString privateTabUrl = QLatin1String("qrc:/web_applications/html/privatetabView.html");
    QString pageUrl = webPage->url().toEncoded();

    if (pageUrl.compare(historyUrl) == 0)
        pageUrl = "ololan://history";

    if (pageUrl.compare(bookmarksUrl) == 0)
        pageUrl = "ololan://bookmarks";

    if (pageUrl.compare(downloadsUrl) == 0)
        pageUrl = "ololan://downloads";

    if (pageUrl.compare(settingsUrl) == 0)
        pageUrl = "ololan://settings";

    if ((pageUrl.compare(newTabUrl) == 0) || (pageUrl.compare(privateTabUrl) == 0))
        pageUrl = "ololan://home";

    if ((pageUrl.compare("ololan://home") == 0) || (pageUrl.compare("ololan://settings") == 0) ||
        (pageUrl.compare("ololan://downloads") == 0) || (pageUrl.compare("ololan://bookmarks") == 0) ||
        (pageUrl.compare("ololan://history") == 0))
        websiteInfo->updateWebsiteInfo(pageUrl, true);
    else
    {
        setIconUnsecureStyle();
        websiteInfo->updateWebsiteInfo(webPage->url().host(), false);
    }
}

void OSmartInput::resetHasCertificateError(bool value = false)
{
    hasCertificateError = false;
}

void OSmartInput::uncheckDeletedBookmark(const QString url)
{
    QString newUrl = url;
    QString historyUrl = QLatin1String("qrc:/web_applications/html/historyView.html");
    QString bookmarksUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");
    QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");
    QString settingsUrl = QLatin1String("qrc:/web_applications/html/settingsView.html");
    QString newTabUrl = QLatin1String("qrc:/web_applications/html/newtabView.html");
    OWebView *webView = parentWidget()->parentWidget()->findChild<OWebView*>("pageView");

    if (newUrl.compare("ololan://history") == 0)
        newUrl = historyUrl;

    if (newUrl.compare("ololan://bookmarks") == 0)
        newUrl = bookmarksUrl;

    if (newUrl.compare("ololan://downloads") == 0)
        newUrl = downloadsUrl;

    if (newUrl.compare("ololan://settings") == 0)
        newUrl = settingsUrl;

    if (newUrl.compare("ololan://home") == 0)
        newUrl = newTabUrl;

    if (webView->url().toEncoded().compare(newUrl.toLatin1()) == 0)
    {
        if (bookmarkButton->isChecked())
            bookmarkButton->setChecked(false);
    }
}

void OSmartInput::setIconSecureStyle()
{
    if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
        (ololanTheme.compare("Light brown") == 0))
        securityButton->setIcon(QIcon(":/images/secure.png"));
    else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
             (ololanTheme.compare("Dark brown") == 0) || (ololanTheme.compare("Private") == 0))
        securityButton->setIcon(QIcon(":/private_mode_images/securePrivateMode.png"));
}

void OSmartInput::setIconUnsecureStyle()
{
    securityButton->setIcon(QIcon(":/images/unsecure.png"));
}

void OSmartInput::setWebIconLightGrayTheme(UrlType urlType, bool focused)
{
    if (focused)
    {
        if (urlType == SearchType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px;"
                                                    "background-image: url(:/images/searchFocused.png); background-position: center; border-radius: 2px;}"));
        else if (urlType == WebsiteType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px;"
                                                    "background-image: url(:/images/urlTypedFocused.png); background-position: center; border-radius: 2px;}"));
    }
    else
    {
        if (urlType == SearchType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px;"
                                                    "background-image: url(:/images/search.png); background-position: center; border-radius: 2px;}"));
        else if (urlType == WebsiteType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px;"
                                                    "background-image: url(:/images/urlTyped.png); background-position: center; border-radius: 2px;}"));
    }
}

void OSmartInput::setWebIconLightTurquoiseTheme(UrlType urlType, bool focused)
{
    if (focused)
    {
        if (urlType == SearchType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px;"
                                                    "background-image: url(:/images/searchFocused.png); background-position: center; border-radius: 2px;}"));
        else if (urlType == WebsiteType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px;"
                                                    "background-image: url(:/images/urlTypedFocused.png); background-position: center; border-radius: 2px;}"));
    }
    else
    {
        if (urlType == SearchType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px;"
                                                    "background-image: url(:/images/search.png); background-position: center; border-radius: 2px;}"));
        else if (urlType == WebsiteType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px;"
                                                    "background-image: url(:/images/urlTyped.png); background-position: center; border-radius: 2px;}"));
    }
}

void OSmartInput::setWebIconLightBrownTheme(UrlType urlType, bool focused)
{
    if (focused)
    {
        if (urlType == SearchType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px;"
                                                    "background-image: url(:/images/searchFocusedBrown.png); background-position: center; border-radius: 2px;}"));
        else if (urlType == WebsiteType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px;"
                                                    "background-image: url(:/images/urlTypedFocusedBrown.png); background-position: center; border-radius: 2px;}"));
    }
    else
    {
        if (urlType == SearchType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px;"
                                                    "background-image: url(:/images/search.png); background-position: center; border-radius: 2px;}"));
        else if (urlType == WebsiteType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px;"
                                                    "background-image: url(:/images/urlTyped.png); background-position: center; border-radius: 2px;}"));
    }
}

void OSmartInput::setWebIconDarkGrayTheme(UrlType urlType, bool focused)
{
    if (focused)
    {
        if (urlType == SearchType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                    "background-image: url(:/images/searchFocused.png); background-position: center; padding-bottom: 1px; border-radius: 2px;}"));
        else if (urlType == WebsiteType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                    "background-image: url(:/images/urlTypedFocused.png); background-position: center; padding-bottom: 1px; border-radius: 2px;}"));
    }
    else
    {
        if (urlType == SearchType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                    "background-image: url(:/private_mode_images/searchPrivateMode.png); background-position: center; padding-bottom: 1px; border-radius: 2px;}"));
        else if (urlType == WebsiteType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                    "background-image: url(:/private_mode_images/urlTypedPrivateMode.png); background-position: center; padding-bottom: 1px; border-radius: 2px;}"));
    }
}

void OSmartInput::setWebIconDarkTurquoiseTheme(UrlType urlType, bool focused)
{
    if (focused)
    {
        if (urlType == SearchType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px; border-radius: 2px;"
                                                    "background-image: url(:/images/searchFocused.png); background-position: center;}"));
        else if (urlType == WebsiteType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px; border-radius: 2px;"
                                                    "background-image: url(:/images/urlTypedFocused.png); background-position: center;}"));
    }
    else
    {
        if (urlType == SearchType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px; border-radius: 2px;"
                                                    "background-image: url(:/private_mode_images/searchPrivateMode.png); background-position: center;}"));
        else if (urlType == WebsiteType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px; border-radius: 2px;"
                                                    "background-image: url(:/private_mode_images/urlTypedPrivateMode.png); background-position: center;}"));
    }
}

void OSmartInput::setWebIconDarkBrownTheme(UrlType urlType, bool focused)
{
    if (focused)
    {
        if (urlType == SearchType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px; border-radius: 2px;"
                                                    "background-image: url(:/images/searchFocusedBrown.png); background-position: center;}"));
        else if (urlType == WebsiteType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px; border-radius: 2px;"
                                                    "background-image: url(:/images/urlTypedFocusedBrown.png); background-position: center;}"));
    }
    else
    {
        if (urlType == SearchType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px; border-radius: 2px;"
                                                    "background-image: url(:/private_mode_images/searchPrivateMode.png); background-position: center;}"));
        else if (urlType == WebsiteType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px; border-radius: 2px;"
                                                    "background-image: url(:/private_mode_images/urlTypedPrivateMode.png); background-position: center;}"));
    }
}

void OSmartInput::setWebIconPrivateTheme(UrlType urlType, bool focused)
{
    if (focused)
    {
        if (urlType == SearchType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px; border-radius: 2px;"
                                                    "background-image: url(:/private_mode_images/searchFocusedPrivateMode.png); background-position: center;}"));
        else if (urlType == WebsiteType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px; border-radius: 2px;"
                                                    "background-image: url(:/private_mode_images/urlTypedFocusedPrivateMode.png); background-position: center;}"));
    }
    else
    {
        if (urlType == SearchType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px; border-radius: 2px;"
                                                    "background-image: url(:/private_mode_images/searchPrivateMode.png); background-position: center;}"));
        else if (urlType == WebsiteType)
            searchIcon->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat; padding-bottom: 1px; border-radius: 2px;"
                                                    "background-image: url(:/private_mode_images/urlTypedPrivateMode.png); background-position: center;}"));
    }
}

void OSmartInput::setFocusInLightGrayTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {border: 1px solid #e1e1e3; border-radius: 4%; color: #353535; font-size: 14px; background-color: #e1e1e3;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #e1e1e3;}"
                                "QLineEdit:focus {border: 2px solid #56bcc5; padding-left: 0px; selection-background-color: #56bcc5;}"
                                "QLineEdit:focus:hover {border: 2px solid #56bcc5; padding-left: 0px;}"));

    bookmarkButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/images/bookmarkFocused.png); background-position: center; margin-top: 0px;}"
                                                "QToolButton:hover {background-image: url(:/images/bookmarkFocused.png); border-radius: 2px;}"
                                                "QToolButton:checked {background-image: url(:/images/bookmarkOn.png);}"
                                                "QToolButton::menu-indicator {image: none;}"));
}

void OSmartInput::setFocusInLightTurquoiseTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {border: 1px solid #c0ebee; border-radius: 4%; color: #353535; font-size: 14px; background-color: #c0ebee;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #c0ebee;}"
                                "QLineEdit:focus {border: 2px solid #56bcc5; padding-left: 0px; selection-background-color: #56bcc5;}"
                                "QLineEdit:focus:hover {border: 2px solid #56bcc5; padding-left: 0px;}"));

    bookmarkButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/images/bookmarkFocused.png); background-position: center; margin-top: 0px;}"
                                                "QToolButton:hover {background-image: url(:/images/bookmarkFocused.png); border-radius: 2px;}"
                                                "QToolButton:checked {background-image: url(:/images/bookmarkOn.png);}"
                                                "QToolButton::menu-indicator {image: none;}"));
}

void OSmartInput::setFocusInLightBrownTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {border: 1px solid #ffd0ae; border-radius: 4%; color: #c0c0c0; font-size: 14px; background-color: #ffd0ae;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #ffd0ae;}"
                                "QLineEdit:focus {border: 2px solid #ffa477; padding-left: 0px; selection-background-color: #ffa477;}"
                                "QLineEdit:focus:hover {border: 2px solid #ffa477; padding-left: 0px;}"));

    bookmarkButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/images/bookmarkFocusedBrown.png); background-position: center; margin-top: 0px;}"
                                                "QToolButton:hover {background-image: url(:/images/bookmarkFocusedBrown.png); border-radius: 2px;}"
                                                "QToolButton:checked {background-image: url(:/images/bookmarkOnBrown.png);}"
                                                "QToolButton::menu-indicator {image: none;}"));
}

void OSmartInput::setFocusInDarkGrayTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {background-color: #434344; border: 1px solid #434344; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #434344;}"
                                "QLineEdit:focus {border: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;}"
                                "QLineEdit:focus:hover {border: 2px solid #68e7f0; padding-left: 0px;}"));

    bookmarkButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/images/bookmarkFocused.png); background-position: center; margin-top: -0.55px;}"
                                                "QToolButton:hover {background-image: url(:/images/bookmarkFocused.png);"
                                                "border-radius: 2px; background-color: #59595a; border: 1px solid #59595a;}"
                                                "QToolButton:checked {background-image: url(:/images/bookmarkOn.png);}"
                                                "QToolButton::menu-indicator {image: none;}"));
}

void OSmartInput::setFocusInDarkTurquoiseTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {background-color: #1e4446; border: 1px solid #1e4446; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #1e4446;}"
                                "QLineEdit:focus {border: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;}"
                                "QLineEdit:focus:hover {border: 2px solid #68e7f0; padding-left: 0px;}"));

    bookmarkButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/images/bookmarkFocused.png); background-position: center; margin-top: -0.55px;}"
                                                "QToolButton:hover {background-image: url(:/images/bookmarkFocused.png);"
                                                "border-radius: 2px; background-color: #27585b; border: 1px solid #27585b;}"
                                                "QToolButton:checked {background-image: url(:/images/bookmarkOn.png);}"
                                                "QToolButton::menu-indicator {image: none;}"));
}

void OSmartInput::setFocusInDarkBrownTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {background-color: #462c21; border: 1px solid #462c21; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #462c21;}"
                                "QLineEdit:focus {border: 2px solid #ffa477; padding-left: 0px; selection-background-color: #ffa477;}"
                                "QLineEdit:focus:hover {border: 2px solid #ffa477; padding-left: 0px;}"));

    bookmarkButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/images/bookmarkFocusedBrown.png); background-position: center; margin-top: 0px;}"
                                                "QToolButton:hover {background-image: url(:/images/bookmarkFocusedBrown.png);"
                                                "border-radius: 2px; background-color: #5b382b; border: 1px solid #5b382b;}"
                                                "QToolButton:checked {background-image: url(:/images/bookmarkOnBrown.png);}"
                                                "QToolButton::menu-indicator {image: none;}"));
}

void OSmartInput::setFocusInPrivateTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {background-color: #3c3244; border: 1px solid #393041; border-radius: 4%; color: #dddddd; font-size: 14px;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #3c3244;}"
                                "QLineEdit:focus {border: 2px solid #c2a1dd; padding-left: 0px; selection-background-color: #c2a1dd;}"
                                "QLineEdit:focus:hover {border: 2px solid #c2a1dd; padding-left: 0px;}"));

    bookmarkButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/private_mode_images/bookmarkFocusedPrivateMode.png); background-position: center; margin-top: -0.55px;}"
                                                "QToolButton:hover {background-image: url(:/private_mode_images/bookmarkFocusedPrivateMode.png);"
                                                "background-color: #50425b; border: 1px solid #50425b; border-radius: 2px;}"
                                                "QToolButton:checked {background-image: url(:/private_mode_images/bookmarkOnPrivateMode.png);}"
                                                "QToolButton::menu-indicator {image: none;}"));
}

void OSmartInput::setFocusOutLightGrayTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {border: 1px solid #e1e1e3; border-radius: 4%; color: #757575; font-size: 14px; background-color: #e1e1e3;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #e1e1e3;}"
                                "QLineEdit:focus {border: 2px solid #56bcc5; padding-left: 0px; selection-background-color: #56bcc5;}"
                                "QLineEdit:focus:hover {border: 2px solid #56bcc5; padding-left: 0px;}"));

    bookmarkButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/images/bookmarkOff.png); background-position: center; margin-top: 0px;}"
                                                "QToolButton:hover {background-image: url(:/images/bookmarkOff.png); background-color: #d1d1d3;"
                                                "border: 1px solid #d1d1d3; border-radius: 2px;}"
                                                "QToolButton:checked {background-image: url(:/images/bookmarkOn.png);}"
                                                "QToolButton::menu-indicator {image: none;}"));

    securityButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; padding-bottom: 1px; border-radius: 2px;}"
                                                "QToolButton:hover {background-color: #d1d1d3; border: 1px solid #d1d1d3;}"
                                                "QToolButton:pressed {background-color: #c4c4c6; border: 1px solid #c4c4c6;}"
                                                "QToolButton::menu-indicator {image: none;}"));
}

void OSmartInput::setFocusOutLightTurquoiseTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {border: 1px solid #c0ebee; border-radius: 4%; color: #353535; font-size: 14px; background-color: #c0ebee;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #c0ebee;}"
                                "QLineEdit:focus {border: 2px solid #56bcc5; padding-left: 0px; selection-background-color: #56bcc5;}"
                                "QLineEdit:focus:hover {border: 2px solid #56bcc5; padding-left: 0px;}"));

    bookmarkButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/images/bookmarkOff.png); background-position: center; margin-top: 0px;}"
                                                "QToolButton:hover {background-image: url(:/images/bookmarkOff.png); border-radius: 2px;}"
                                                "QToolButton:checked {background-image: url(:/images/bookmarkOn.png);}"
                                                "QToolButton::menu-indicator {image: none;}"));

    securityButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; padding-bottom: 1px; border-radius: 2px;}"
                                                "QToolButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                                "QToolButton:pressed {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"
                                                "QToolButton::menu-indicator {image: none;}"));
}

void OSmartInput::setFocusOutLightBrownTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {border: 1px solid #ffd0ae; border-radius: 4%; color: #c0c0c0; font-size: 14px; background-color: #ffd0ae;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #ffd0ae;}"
                                "QLineEdit:focus {border: 2px solid #ffa477; padding-left: 0px; selection-background-color: #ffa477;}"
                                "QLineEdit:focus:hover {border: 2px solid #ffa477; padding-left: 0px;}"));

    bookmarkButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/images/bookmarkOff.png); background-position: center; margin-top: 0px;}"
                                                "QToolButton:hover {background-image: url(:/images/bookmarkOff.png); border-radius: 2px;}"
                                                "QToolButton:checked {background-image: url(:/images/bookmarkOnBrown.png);}"
                                                "QToolButton::menu-indicator {image: none;}"));    

    securityButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; padding-bottom: 1px; border-radius: 2px;}"
                                                "QToolButton:hover {background-color: #efeff1; border: 1px solid #efeff1;}"
                                                "QToolButton:pressed {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"
                                                "QToolButton::menu-indicator {image: none;}"));
}

void OSmartInput::setFocusOutDarkGrayTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {background-color: #434344; border: 1px solid #434344; border-radius: 4%; color: #a0a0a0; font-size: 14px;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #434344;}"
                                "QLineEdit:focus {border: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;}"
                                "QLineEdit:focus:hover {border: 2px solid #68e7f0; padding-left: 0px;}"));

    bookmarkButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/private_mode_images/bookmarkOffPrivateMode.png); background-position: center; margin-top: -0.55px;}"
                                                "QToolButton:hover {background-image: url(:/private_mode_images/bookmarkOffPrivateMode.png);"
                                                "background-color: #59595a; border: 1px solid #59595a; border-radius: 2px;}"
                                                "QToolButton:checked {background-image: url(:/images/bookmarkOn.png);}"
                                                "QToolButton::menu-indicator {image: none;}"));

    securityButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; padding-bottom: 1px; border-radius: 2px;}"
                                                "QToolButton:hover {background-color: #59595a; border: 1px solid #59595a;}"
                                                "QToolButton:pressed {background-color: #505051; border: 1px solid #505051;}"
                                                "QToolButton::menu-indicator {image: none;}"));
}

void OSmartInput::setFocusOutDarkTurquoiseTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {background-color: #1e4446; border: 1px solid #1e4446; border-radius: 4%; color: #a0a0a0; font-size: 14px;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #1e4446;}"
                                "QLineEdit:focus {border: 2px solid #68e7f0; padding-left: 0px; selection-background-color: #68e7f0;}"
                                "QLineEdit:focus:hover {border: 2px solid #68e7f0; padding-left: 0px;}"));

    bookmarkButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/private_mode_images/bookmarkOffPrivateMode.png); background-position: center; margin-top: -0.55px;}"
                                                "QToolButton:hover {background-image: url(:/private_mode_images/bookmarkOffPrivateMode.png);"
                                                "background-color: #27585b; border: 1px solid #27585b; border-radius: 2px;}"
                                                "QToolButton:checked {background-image: url(:/images/bookmarkOn.png);}"
                                                "QToolButton::menu-indicator {image: none;}"));

    securityButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; padding-bottom: 1px; border-radius: 2px;}"
                                                "QToolButton:hover {background-color: #27585b; border: 1px solid #27585b;}"
                                                "QToolButton:pressed {background-color: #234f51; border: 1px solid #234f51;}"
                                                "QToolButton::menu-indicator {image: none;}"));
}

void OSmartInput::setFocusOutDarkBrownTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {background-color: #462c21; border: 1px solid #462c21; border-radius: 4%; color: #a0a0a0; font-size: 14px;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #462c21;}"
                                "QLineEdit:focus {border: 2px solid #ffa477; padding-left: 0px; selection-background-color: #ffa477;}"
                                "QLineEdit:focus:hover {border: 2px solid #ffa477; padding-left: 0px;}"));

    bookmarkButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/private_mode_images/bookmarkOffPrivateMode.png); background-position: center; margin-top: -0.55px;}"
                                                "QToolButton:hover {background-image: url(:/private_mode_images/bookmarkOffPrivateMode.png);"
                                                "background-color: #5b382b; border: 1px solid #5b382b; border-radius: 2px;}"
                                                "QToolButton:checked {background-image: url(:/images/bookmarkOnBrown.png);}"
                                                "QToolButton::menu-indicator {image: none;}"));

    securityButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; padding-bottom: 1px; border-radius: 2px;}"
                                                "QToolButton:hover {background-color: #5b382b; border: 1px solid #5b382b;}"
                                                "QToolButton:pressed {background-color: #513226; border: 1px solid #513226;}"
                                                "QToolButton::menu-indicator {image: none;}"));
}

void OSmartInput::setFocusOutPrivateTheme()
{
    setStyleSheet(QLatin1String("QLineEdit {background-color: #3c3244; border: 1px solid #3c3244; border-radius: 4%; color: #a0a0a0; font-size: 14px;"
                                "font-family:'Segoe UI'; padding-left: 1px; padding-right: 0px; padding-top: 0px; padding-bottom: 2px;}"
                                "QLineEdit:hover {border: 1px solid #3c3244;}"
                                "QLineEdit:focus {border: 2px solid #c2a1dd; padding-left: 0px; selection-background-color: #c2a1dd;}"
                                "QLineEdit:focus:hover {border: 2px solid #c2a1dd; padding-left: 0px;}"));

    bookmarkButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; background-repeat: no-repeat;"
                                                "background-image: url(:/private_mode_images/bookmarkOffPrivateMode.png); background-position: center; margin-top: -0.55px;}"
                                                "QToolButton:hover {background-image: url(:/private_mode_images/bookmarkOffPrivateMode.png);"
                                                "background-color: #50425b; border: 1px solid #50425b; border-radius: 2px;}"
                                                "QToolButton:checked {background-image: url(:/private_mode_images/bookmarkOnPrivateMode.png);}"
                                                "QToolButton::menu-indicator {image: none;}"));

    securityButton->setStyleSheet(QLatin1String("QToolButton {background-color: transparent; border: none; padding-bottom: 1px; border-radius: 2px;}"
                                                "QToolButton:hover {background-color: #50425b; border: 1px solid #50425b;}"
                                                "QToolButton:pressed {background-color: #473b51; border: 1px solid #473b51;}"
                                                "QToolButton::menu-indicator {image: none;}"));
}

void OSmartInput::setSecurityTheme()
{
    if (webViewPage->url().isEmpty())
    {
        if ((ololanTheme.compare("Light gray") == 0) || (ololanTheme.compare("Light turquoise") == 0) ||
            (ololanTheme.compare("Light brown") == 0))
            securityButton->setIcon(QIcon(":/images/urlTyped.png"));
        else if ((ololanTheme.compare("Dark gray") == 0) || (ololanTheme.compare("Dark turquoise") == 0) ||
                 (ololanTheme.compare("Dark brown") == 0) || (ololanTheme.compare("Private") == 0))
            securityButton->setIcon(QIcon(":/private_mode_images/urlTypedPrivateMode.png"));
    }
    else
    {
        QString url = webViewPage->url().toString();
        QString historyUrl = QLatin1String("qrc:/web_applications/html/historyView.html");
        QString bookmarksUrl = QLatin1String("qrc:/web_applications/html/bookmarksView.html");
        QString downloadsUrl = QLatin1String("qrc:/web_applications/html/downloadsView.html");
        QString settingsUrl = QLatin1String("qrc:/web_applications/html/settingsView.html");
        QString newTabUrl = QLatin1String("qrc:/web_applications/html/newtabView.html");
        QString privateTabUrl = QLatin1String("qrc:/web_applications/html/privatetabView.html");

        if (url.compare(historyUrl) == 0)
            url = "ololan://history";

        if (url.compare(bookmarksUrl) == 0)
            url = "ololan://bookmarks";

        if (url.compare(downloadsUrl) == 0)
            url = "ololan://downloads";

        if (url.compare(settingsUrl) == 0)
            url = "ololan://settings";

        if ((url.compare(newTabUrl) == 0) || (url.compare(privateTabUrl) == 0))
            url = "ololan://home";

        if (isSecureUrl(url))
            setIconSecureStyle();
        else
            setIconUnsecureStyle();
    }
}

void OSmartInput::saveUserCredentials()
{
    webViewPage->saveCredentials(websiteInfo->getCredentialsUrl());
}

void OSmartInput::setWebViewPage(OWebPage *page)
{
    webViewPage = page;
}

void OSmartInput::setPermission(const QString permission, bool accepted, const QUrl securityOrigin)
{
    if (permission.compare("location") == 0)
    {
        if (accepted)
            webViewPage->setFeaturePermission(securityOrigin, QWebEnginePage::Geolocation, QWebEnginePage::PermissionGrantedByUser);
        else
            webViewPage->setFeaturePermission(securityOrigin, QWebEnginePage::Geolocation, QWebEnginePage::PermissionDeniedByUser);
    }
    else if (permission.compare("camera") == 0)
    {
        if (accepted)
            webViewPage->setFeaturePermission(securityOrigin, QWebEnginePage::MediaVideoCapture, QWebEnginePage::PermissionGrantedByUser);
        else
            webViewPage->setFeaturePermission(securityOrigin, QWebEnginePage::MediaVideoCapture, QWebEnginePage::PermissionDeniedByUser);
    }
    else if (permission.compare("microphone") == 0)
    {
        if (accepted)
            webViewPage->setFeaturePermission(securityOrigin, QWebEnginePage::MediaAudioCapture, QWebEnginePage::PermissionGrantedByUser);
        else
            webViewPage->setFeaturePermission(securityOrigin, QWebEnginePage::MediaAudioCapture, QWebEnginePage::PermissionDeniedByUser);
    }
    else if (permission.compare("webcam") == 0)
    {
        if (accepted)
            webViewPage->setFeaturePermission(securityOrigin, QWebEnginePage::MediaAudioVideoCapture, QWebEnginePage::PermissionGrantedByUser);
        else
            webViewPage->setFeaturePermission(securityOrigin, QWebEnginePage::MediaAudioVideoCapture, QWebEnginePage::PermissionDeniedByUser);
    }
}
