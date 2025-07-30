#ifndef OCERTIFICATEMANAGER_H
#define OCERTIFICATEMANAGER_H

#include <QObject>
#include <QDialog>
#include <QTableWidget>
#include <QWebEngineProfile>

class OCertificateManager : public QDialog
{
    Q_OBJECT

public:
    OCertificateManager(QWidget *parent = nullptr);
    void setupView();
    QTableWidget *createPage(const QString section);
    void setupViewList(QTableWidget *widget, const QString section);
    void setupDetailsView();
    void setWebProfile(QWebEngineProfile *profile);
    void setupDetailsList();

public slots:
    void removeCertificate();
    void exportCertificate();
    void importCertificate();
    void viewCertificate();
    void selectCertificateIndex(int row, int column);
    void selectCertificateType(int type);

private:
    QWebEngineProfile *webProfile;
    QDialog *certificateDetails;
    QPushButton *exportCert;
    QPushButton *removeCert;
    QPushButton *viewCert;
    int certificateIndex;
    int certificateType;
};

#endif // OCERTIFICATEMANAGER_H
