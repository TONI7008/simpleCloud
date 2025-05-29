#ifndef TCOMPRESSER_H
#define TCOMPRESSER_H

#include <QObject>
#include <QByteArray>
#include <QDebug>
#include <QThread>

class TCompresser : public QObject
{
    Q_OBJECT
public:
    explicit TCompresser(QObject *parent = nullptr);

    QByteArray compressData(const QByteArray &data);
    QByteArray decompressData(const QByteArray &compressedData);
    bool isWorking();
    void stop();
    void run();

signals:
    void unCompressingDone();
private:
    bool working;
    bool m_stop;
};

#endif // TCOMPRESSER_H
