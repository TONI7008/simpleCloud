#include "tcompresser.h"

TCompresser::TCompresser(QObject *parent)
    : QObject{parent},working(false),m_stop(false)
{
    qDebug("tcompresser started");
}

QByteArray TCompresser::compressData(const QByteArray &data) {
    working=true;
    QByteArray compressedData = qCompress(data);
    if (compressedData.isEmpty()) {
        qWarning() << "Compression failed.";
    }
    return compressedData;
}

QByteArray TCompresser::decompressData(const QByteArray &compressedData) {
    QByteArray decompressedData = qUncompress(compressedData);
    if (decompressedData.isEmpty()) {
        qWarning() << "Decompression failed or data is corrupt.";
    }
    working=false;
    return decompressedData;
}

bool TCompresser::isWorking()
{
    return working;
}

void TCompresser::stop()
{
    m_stop=true;
}

void TCompresser::run()
{
    /*if(m_stop){
        quit();
    }
    exec();*/
}
