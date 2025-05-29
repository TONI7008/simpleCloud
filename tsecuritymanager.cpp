#include "tsecuritymanager.h"
#include <QFile>
#include <QSslKey>


// Initialize OpenSSL in the constructor
TSecurityManager::TSecurityManager(QObject* parent) : QObject(parent) {

}

// Clean up OpenSSL in the destructor
TSecurityManager::~TSecurityManager() {

}

bool TSecurityManager::loadCertificate(const QString& clientKey, const QString& clientCertif, const QString& caCert) {
    m_config = QSslConfiguration::defaultConfiguration();

    // Load CA certificate
    QFile caFile(caCert);
    if (!caFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open CA certificate file!";
        return false;
    }
    QList<QSslCertificate> caCerts = QSslCertificate::fromData(caFile.readAll(), QSsl::Pem);
    if (caCerts.isEmpty()) {
        qDebug() << "Failed to load CA certificates!";
        return false;
    }
    m_config.setCaCertificates(caCerts);

    // Load private key
    QFile keyFile(clientKey);
    if (!keyFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open private key file!";
        return false;
    }
    QSslKey privateKey(&keyFile, QSsl::Rsa, QSsl::Pem);
    if (privateKey.isNull()) {
        qDebug() << "Failed to load private key!";
        return false;
    }
    m_key = privateKey.toPem(); // Convert QSslKey to QByteArray
    m_config.setPrivateKey(privateKey);

    // Load client certificate
    QFile certFile(clientCertif);
    if (!certFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open certificate file!";
        return false;
    }
    QSslCertificate localCert(&certFile, QSsl::Pem);
    if (localCert.isNull()) {
        qDebug() << "Failed to load local certificate!";
        return false;
    }
    m_config.setLocalCertificate(localCert);
    m_config.setProtocol(QSsl::TlsV1_2OrLater);

    m_config.setPeerVerifyMode(QSslSocket::VerifyNone);
    m_loaded=true;
    return true;
}

QSslConfiguration TSecurityManager::config() const {
    return m_config;
}

bool TSecurityManager::loaded() const
{
    return m_loaded;
}

