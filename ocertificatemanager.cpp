#include "ocertificatemanager.h"
#include <QTabWidget>
#include <QGridLayout>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QScrollBar>
#include <QMessageBox>
#include <QWebEngineClientCertificateStore>
#include <QFileDialog>
#include <QDir>
#include <QSslCertificate>

OCertificateManager::OCertificateManager(QWidget *parent)
    :QDialog(parent, Qt::Dialog)
{
    setFixedSize(700, 450);
    setWindowTitle("Certificates");
    setModal(true);
    setStyleSheet(QLatin1String("QDialog {background-color: #f9f9fb;}"));
    setupView();
    certificateIndex = -1;
    certificateType = -1;
    setupDetailsView();
}

void OCertificateManager::setupView()
{
    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->setDocumentMode(true);
    tabWidget->setMovable(false);
    tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tabWidget->tabBar()->setExpanding(true);
    tabWidget->tabBar()->setDrawBase(false);
    tabWidget->tabBar()->setStyleSheet(QLatin1String("QTabBar::tab {background-color: #ffffff; border-left: 1px solid #ffffff;"
                                                     "border-right: 1px solid #ffffff; border-top: 1px solid #ffffff;"
                                                     "border-bottom: 2px solid #ffffff; height: 30px; color: #888888; padding: 0px;}"
                                                     "QTabBar::tab:selected {border-bottom: 2px solid #68e7f0; padding: 0px;}"));

    tabWidget->addTab(createPage("Personal"), "Personal");
    tabWidget->addTab(createPage("Local Certificates"), "Local Certificates");
    tabWidget->addTab(createPage("Peer Certificates"), "Peer Certificates");
    tabWidget->addTab(createPage("System CA Certificates"), "System CA Certificates");

    removeCert = new QPushButton("Remove", this);
    removeCert->setDisabled(true);
    removeCert->setFixedSize(90, 30);
    removeCert->setStyleSheet(QLatin1String("QPushButton {background-color: #efeff1; border: 1px solid #efeff1; padding: 0px; border-radius: 3.5%; color: #888888;}"
                                            "QPushButton:hover {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"
                                            "QPushButton:pressed {background-color: #d4d4d6; border: 1px solid #d4d4d6;}"
                                            "QPushButton:!enabled {background-color: #f5f5f7; border: 1px solid #f5f5f7; color: #a9a9a9;}"));

    exportCert = new QPushButton("Export", this);
    exportCert->setDisabled(true);
    exportCert->setFixedSize(90, 30);
    exportCert->setStyleSheet(QLatin1String("QPushButton {background-color: #efeff1; border: 1px solid #efeff1; padding: 0px; border-radius: 3.5%; color: #888888;}"
                                            "QPushButton:hover {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"
                                            "QPushButton:pressed {background-color: #d4d4d6; border: 1px solid #d4d4d6;}"
                                            "QPushButton:!enabled {background-color: #f5f5f7; border: 1px solid #f5f5f7; color: #a9a9a9;}"));

    viewCert = new QPushButton("View", this);
    viewCert->setDisabled(true);
    viewCert->setFixedSize(90, 30);
    viewCert->setStyleSheet(QLatin1String("QPushButton {background-color: #efeff1; border: 1px solid #efeff1; padding: 0px; border-radius: 3.5%; color: #888888;}"
                                            "QPushButton:hover {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"
                                            "QPushButton:pressed {background-color: #d4d4d6; border: 1px solid #d4d4d6;}"
                                            "QPushButton:!enabled {background-color: #f5f5f7; border: 1px solid #f5f5f7; color: #a9a9a9;}"));

    QPushButton *importCert = new QPushButton("Import", this);
    importCert->setFixedSize(90, 30);
    importCert->setStyleSheet(QLatin1String("QPushButton {background-color: #efeff1; border: 1px solid #efeff1; padding: 0px; border-radius: 3.5%; color: #888888;}"
                                            "QPushButton:hover {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"
                                            "QPushButton:pressed {background-color: #d4d4d6; border: 1px solid #d4d4d6;}"));

    QPushButton *closeDiag = new QPushButton("Close", this);
    closeDiag->setFixedSize(90, 30);
    closeDiag->setStyleSheet(QLatin1String("QPushButton {background-color: #efeff1; border: 1px solid #efeff1; padding: 0px; border-radius: 3.5%; color: #888888;}"
                                           "QPushButton:hover {background-color: #dfdfe1; border: 1px solid #dfdfe1;}"
                                           "QPushButton:pressed {background-color: #d4d4d6; border: 1px solid #d4d4d6;}"));

    connect(removeCert, &QPushButton::clicked, this, &OCertificateManager::removeCertificate);
    connect(exportCert, &QPushButton::clicked, this, &OCertificateManager::exportCertificate);
    connect(importCert, &QPushButton::clicked, this, &OCertificateManager::importCertificate);
    connect(viewCert, &QPushButton::clicked, this, &OCertificateManager::viewCertificate);
    connect(closeDiag, &QPushButton::clicked, this, &OCertificateManager::close);
    connect(tabWidget, &QTabWidget::currentChanged, this, &OCertificateManager::selectCertificateType);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    hLayout->addWidget(removeCert, 0, Qt::AlignLeft);
    hLayout->addWidget(exportCert, 0, Qt::AlignLeft);
    hLayout->addWidget(importCert, 0, Qt::AlignLeft);
    hLayout->addWidget(viewCert, 0, Qt::AlignLeft);
    hLayout->addSpacing(180);
    hLayout->addWidget(closeDiag, 0, Qt::AlignRight);

    QGridLayout *gLayout = new QGridLayout(this);
    gLayout->setSpacing(10);
    gLayout->setContentsMargins(10, 10, 10, 10);
    gLayout->addWidget(tabWidget, 0, 0);
    gLayout->addLayout(hLayout, 1, 0);
}

void OCertificateManager::setupDetailsView()
{
    certificateDetails = new QDialog(this);
    certificateDetails->setFixedSize(450, 600);
    certificateDetails->setWindowTitle("Details");
    certificateDetails->setModal(true);
    certificateDetails->setStyleSheet(QLatin1String("QDialog {background-color: #ffffff;}"));

    QTableWidget *tableWidget = new QTableWidget();
    tableWidget->setColumnCount(2);
    tableWidget->setStyleSheet(QLatin1String("QTableWidget {background-color: #ffffff; border: 1px solid #ffffff; outline: none;}"
                                             "QTableWidget::item {color: #888888; height: 30px;}"
                                             "QTableWidget::item:hover {background-color: #ffffff; border-top: 1px solid #ffffff; border-bottom: 1px solid #ffffff;}"
                                             "QTableWidget::item:selected {background-color: #efeff1; border-top: 1px solid #efeff1; border-bottom: 1px solid #efeff1;}"));

    tableWidget->verticalScrollBar()->setStyleSheet(QLatin1String("QScrollBar:vertical {border: none; background: #f2f5f5; width: 13px;"
                                                    "margin: 0px 0 0px 0; border-radius: 7px;}"
                                                    "QScrollBar::handle:vertical {background: #68e7f0;  min-height: 20px;}"
                                                    "QScrollBar::add-line:vertical {border: none; background: #f2f5f5;"
                                                    "height: 0px; subcontrol-position: bottom; subcontrol-origin: margin;}"
                                                    "QScrollBar::sub-line:vertical {border: none; background: #f2f5f5;"
                                                    "height: 0px; subcontrol-position: top; subcontrol-origin: margin;}"
                                                    "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {"
                                                    "border: none; width: 0px; height: 0px; background: #f2f5f5;}"
                                                    "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: #f2f5f5;}"));

    QHeaderView *columns = new QHeaderView(Qt::Horizontal, tableWidget);
    columns->setSectionsMovable(false);
    columns->setTextElideMode(Qt::ElideRight);
    columns->setSectionResizeMode(QHeaderView::Stretch);
    columns->setStyleSheet(QLatin1String("QHeaderView {background-color: #ffffff; border: 1px solid #efeff1;"
                                         "height: 30px; color: #888888;}"));
    tableWidget->setHorizontalHeader(columns);

    QStringList columnLabels;
    columnLabels << "Field" << "Value";
    tableWidget->setHorizontalHeaderLabels(columnLabels);
    tableWidget->verticalHeader()->hide();
    tableWidget->setFrameShape(QTableWidget::NoFrame);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setShowGrid(false);

    QHBoxLayout *hLayout = new QHBoxLayout(certificateDetails);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    hLayout->addWidget(tableWidget);
}

void OCertificateManager::setupDetailsList()
{
    QTableWidget *tableWidget = certificateDetails->findChild<QTableWidget*>();
    tableWidget->clearContents();
    tableWidget->setRowCount(9);
    QSslConfiguration sslConfig;
    QSslCertificate cert;

    if (certificateType == 0)
        cert = sslConfig.caCertificates().at(certificateIndex);
    else if (certificateType == 1)
        cert = sslConfig.localCertificateChain().at(certificateIndex);
    else if (certificateType == 2)
        cert = sslConfig.peerCertificateChain().at(certificateIndex);
    else if (certificateType == 3)
        cert = sslConfig.systemCaCertificates().at(certificateIndex);

    tableWidget->setItem(0, 0, new QTableWidgetItem("Organization", QTableWidgetItem::Type));
    tableWidget->setItem(1, 0, new QTableWidgetItem("CommonName", QTableWidgetItem::Type));
    tableWidget->setItem(2, 0, new QTableWidgetItem("LocalityName", QTableWidgetItem::Type));
    tableWidget->setItem(3, 0, new QTableWidgetItem("OrganizationalUnitName", QTableWidgetItem::Type));
    tableWidget->setItem(4, 0, new QTableWidgetItem("CountryName", QTableWidgetItem::Type));
    tableWidget->setItem(5, 0, new QTableWidgetItem("StateOrProvinceName", QTableWidgetItem::Type));
    tableWidget->setItem(6, 0, new QTableWidgetItem("DistinguishedNameQualifier", QTableWidgetItem::Type));
    tableWidget->setItem(7, 0, new QTableWidgetItem("SerialNumber", QTableWidgetItem::Type));
    tableWidget->setItem(8, 0, new QTableWidgetItem("EmailAddress", QTableWidgetItem::Type));
    tableWidget->setItem(0, 1, new QTableWidgetItem(cert.subjectInfo(QSslCertificate::Organization).join(" "), QTableWidgetItem::Type));
    tableWidget->setItem(1, 1, new QTableWidgetItem(cert.subjectInfo(QSslCertificate::CommonName).join(" "), QTableWidgetItem::Type));
    tableWidget->setItem(2, 1, new QTableWidgetItem(cert.subjectInfo(QSslCertificate::LocalityName).join(" "), QTableWidgetItem::Type));
    tableWidget->setItem(3, 1, new QTableWidgetItem(cert.subjectInfo(QSslCertificate::OrganizationalUnitName).join(" "), QTableWidgetItem::Type));
    tableWidget->setItem(4, 1, new QTableWidgetItem(cert.subjectInfo(QSslCertificate::CountryName).join(" "), QTableWidgetItem::Type));
    tableWidget->setItem(5, 1, new QTableWidgetItem(cert.subjectInfo(QSslCertificate::StateOrProvinceName).join(" "), QTableWidgetItem::Type));
    tableWidget->setItem(6, 1, new QTableWidgetItem(cert.subjectInfo(QSslCertificate::DistinguishedNameQualifier).join(" "), QTableWidgetItem::Type));
    tableWidget->setItem(7, 1, new QTableWidgetItem(cert.subjectInfo(QSslCertificate::SerialNumber).join(" "), QTableWidgetItem::Type));
    tableWidget->setItem(8, 1, new QTableWidgetItem(cert.subjectInfo(QSslCertificate::EmailAddress).join(" "), QTableWidgetItem::Type));
    tableWidget->clearSelection();
    tableWidget->clearFocus();
}

QTableWidget *OCertificateManager::createPage(const QString section)
{
    QTableWidget *tableWidget = new QTableWidget();
    tableWidget->setColumnCount(3);
    tableWidget->setStyleSheet(QLatin1String("QTableWidget {background-color: #ffffff; border: 1px solid #ffffff; outline: none;}"
                                             "QTableWidget::item {color: #888888; height: 30px;}"
                                             "QTableWidget::item:hover {background-color: #ffffff; border-top: 1px solid #ffffff; border-bottom: 1px solid #ffffff;}"
                                             "QTableWidget::item:selected {background-color: #efeff1; border-top: 1px solid #efeff1; border-bottom: 1px solid #efeff1;}"));

    tableWidget->verticalScrollBar()->setStyleSheet(QLatin1String("QScrollBar:vertical {border: none; background: #f2f5f5; width: 13px;"
                                                    "margin: 0px 0 0px 0; border-radius: 7px;}"
                                                    "QScrollBar::handle:vertical {background: #68e7f0;  min-height: 20px;}"
                                                    "QScrollBar::add-line:vertical {border: none; background: #f2f5f5;"
                                                    "height: 0px; subcontrol-position: bottom; subcontrol-origin: margin;}"
                                                    "QScrollBar::sub-line:vertical {border: none; background: #f2f5f5;"
                                                    "height: 0px; subcontrol-position: top; subcontrol-origin: margin;}"
                                                    "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {"
                                                    "border: none; width: 0px; height: 0px; background: #f2f5f5;}"
                                                    "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: #f2f5f5;}"));

    QHeaderView *columns = new QHeaderView(Qt::Horizontal, tableWidget);
    columns->setSectionsMovable(false);
    columns->setTextElideMode(Qt::ElideRight);
    columns->setSectionResizeMode(QHeaderView::Stretch);
    columns->setStyleSheet(QLatin1String("QHeaderView {background-color: #ffffff; border: 1px solid #efeff1;"
                                         "height: 30px; color: #888888;}"));
    tableWidget->setHorizontalHeader(columns);

    QStringList columnLabels;
    columnLabels << "Issuer" << "Start date" << "Expiry date";
    tableWidget->setHorizontalHeaderLabels(columnLabels);
    tableWidget->verticalHeader()->hide();
    tableWidget->setFrameShape(QTableWidget::NoFrame);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setShowGrid(false);

    setupViewList(tableWidget, section);
    connect(tableWidget, &QTableWidget::cellPressed, this, &OCertificateManager::selectCertificateIndex);

    return tableWidget;
}

void OCertificateManager::setupViewList(QTableWidget *widget, const QString section)
{
    if (section.compare("Personal") == 0)
    {
        QSslConfiguration *sslConfig = new QSslConfiguration();
        QList<QSslCertificate> list = sslConfig->caCertificates();
        widget->setRowCount(list.count());

        for (int i = 0; i < list.count(); i++)
        {
            widget->setItem(i, 0, new QTableWidgetItem(list.at(i).issuerDisplayName(), QTableWidgetItem::Type));
            widget->setItem(i, 1, new QTableWidgetItem(list.at(i).effectiveDate().toString(), QTableWidgetItem::Type));
            widget->setItem(i, 2, new QTableWidgetItem(list.at(i).expiryDate().toString(), QTableWidgetItem::Type));
        }
    }
    else if (section.compare("Local Certificates") == 0)
    {
        QSslConfiguration *sslConfig = new QSslConfiguration();
        QList<QSslCertificate> list = sslConfig->localCertificateChain();
        widget->setRowCount(list.count());

        for (int i = 0; i < list.count(); i++)
        {
            widget->setItem(i, 0, new QTableWidgetItem(list.at(i).issuerDisplayName(), QTableWidgetItem::Type));
            widget->setItem(i, 1, new QTableWidgetItem(list.at(i).effectiveDate().toString(), QTableWidgetItem::Type));
            widget->setItem(i, 2, new QTableWidgetItem(list.at(i).expiryDate().toString(), QTableWidgetItem::Type));
        }
    }
    else if (section.compare("Peer Certificates") == 0)
    {
        QSslConfiguration *sslConfig = new QSslConfiguration();
        QList<QSslCertificate> list = sslConfig->peerCertificateChain();
        widget->setRowCount(list.count());

        for (int i = 0; i < list.count(); i++)
        {
            widget->setItem(i, 0, new QTableWidgetItem(list.at(i).issuerDisplayName(), QTableWidgetItem::Type));
            widget->setItem(i, 1, new QTableWidgetItem(list.at(i).effectiveDate().toString(), QTableWidgetItem::Type));
            widget->setItem(i, 2, new QTableWidgetItem(list.at(i).expiryDate().toString(), QTableWidgetItem::Type));
        }
    }
    else if (section.compare("System CA Certificates") == 0)
    {
        QSslConfiguration sslConfig;
        QList<QSslCertificate> list = sslConfig.systemCaCertificates();
        widget->setRowCount(list.count());

        for (int i = 0; i < list.count(); i++)
        {
            widget->setItem(i, 0, new QTableWidgetItem(list.at(i).issuerDisplayName(), QTableWidgetItem::Type));
            widget->setItem(i, 1, new QTableWidgetItem(list.at(i).effectiveDate().toString(), QTableWidgetItem::Type));
            widget->setItem(i, 2, new QTableWidgetItem(list.at(i).expiryDate().toString(), QTableWidgetItem::Type));
        }
    }
}

void OCertificateManager::selectCertificateType(int type)
{
    certificateType = type;
    removeCert->setDisabled(true);
    exportCert->setDisabled(true);
    viewCert->setDisabled(true);
    qobject_cast<QTableWidget*>(static_cast<QTabWidget*>(sender())->currentWidget())->clearSelection();
    qobject_cast<QTableWidget*>(static_cast<QTabWidget*>(sender())->currentWidget())->clearFocus();
}

void OCertificateManager::selectCertificateIndex(int row = 0, int column = 0)
{
    certificateIndex = row;
    removeCert->setEnabled(true);
    exportCert->setEnabled(true);
    viewCert->setEnabled(true);
}

void OCertificateManager::removeCertificate()
{
    const int button = QMessageBox::warning(nullptr, "Remove certificate", "Certificates issued by the certification authorities or "
                                                     "any subordinate certification authorities will be distrust. "
                                                     "Do you want to remove this certificate?", QMessageBox::Yes, QMessageBox::No);
    if (button == QMessageBox::Yes)
    {
        QSslConfiguration sslConfig;
        if (certificateType == 0)
            webProfile->clientCertificateStore()->remove(sslConfig.caCertificates().at(certificateIndex));
        else if (certificateType == 1)
            webProfile->clientCertificateStore()->remove(sslConfig.localCertificateChain().at(certificateIndex));
        else if (certificateType == 2)
            webProfile->clientCertificateStore()->remove(sslConfig.peerCertificateChain().at(certificateIndex));
        else if (certificateType == 3)
            webProfile->clientCertificateStore()->remove(sslConfig.systemCaCertificates().at(certificateIndex));
    }
}

void OCertificateManager::exportCertificate()
{
    QSslConfiguration sslConfig;
    QSslCertificate cert;

    if (certificateType == 0)
        cert = sslConfig.caCertificates().at(certificateIndex);
    else if (certificateType == 1)
        cert = sslConfig.localCertificateChain().at(certificateIndex);
    else if (certificateType == 2)
        cert = sslConfig.peerCertificateChain().at(certificateIndex);
    else if (certificateType == 3)
        cert = sslConfig.systemCaCertificates().at(certificateIndex);

    QString path = QFileDialog::getSaveFileName(nullptr, QString("Save CA Certificate to disk"),
                                                QDir::currentPath() + QString("/") + cert.issuerDisplayName() + QString(".pem"),
                                                QLatin1String("X.509 Certificate (PEM) (*.pem;*.der)"));
    if (!path.isEmpty())
    {
        QFile certFile(path);
        if (certFile.open(QIODevice::WriteOnly))
        {
            if (path.endsWith(".der"))
                certFile.write(cert.toDer());
            else
                certFile.write(cert.toPem());
            certFile.close();
        }
    }
}

void OCertificateManager::importCertificate()
{
    QString path = QFileDialog::getOpenFileName(nullptr, QString("Select CA Certificate file to import"), QDir::currentPath(),
                                                QLatin1String("Certificate files (*.pem;*.der)"));
    if (!path.isEmpty())
    {
        QFile certFile(path);
        if (certFile.open(QIODevice::ReadOnly))
        {
            QSslCertificate cert(certFile.readAll());
            QSslConfiguration sslConfig;
            sslConfig.addCaCertificate(cert);
            webProfile->clientCertificateStore()->add(cert, cert.publicKey());
            certFile.close();
        }
    }
}

void OCertificateManager::setWebProfile(QWebEngineProfile *profile)
{
    webProfile = profile;
}

void OCertificateManager::viewCertificate()
{
    setupDetailsList();
    certificateDetails->exec();
}
