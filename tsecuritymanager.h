#ifndef TSECURITYMANAGER_H
#define TSECURITYMANAGER_H

#include <QByteArray>
#include <QDebug>
#include <QSslConfiguration>
#include <QObject>

class TSecurityManager : public QObject
{
    Q_OBJECT
public:
    TSecurityManager(QObject* parent=nullptr);
    ~TSecurityManager();

    bool loadCertificate(const QString& clientKey, const QString& clientCertif, const QString& caCert);
    QSslConfiguration config() const;
    bool loaded() const;
private:
    // Helper function to handle OpenSSL errors
    static void handleOpenSSLErrors();
    QSslConfiguration m_config;
    QByteArray m_key;
    bool m_loaded=false;

};

#endif // TSECURITYMANAGER_H
