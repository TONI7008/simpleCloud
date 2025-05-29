#ifndef TFILEINFO_H
#define TFILEINFO_H

#include <QString>
#include <QDateTime>
#include <QMimeType>
#include <QMimeDatabase>

class TFileInfo {
public:
    QString filepath;
    qint64 size = 0;
    QDateTime uploadDate;
    QDateTime lastEditDate;
    QString mimetype;
    bool deleted = false;

    // Add proper copy semantics
    TFileInfo() = default;
    TFileInfo(const TFileInfo&) = default;
    TFileInfo& operator=(const TFileInfo&) = default;

    void data() const {
        qDebug("////// START OF INFO //////");
        qDebug() << "Filename =" << filepath;
        qDebug() << "Size =" << size;
        qDebug() << "Upload date =" << uploadDate.toString(Qt::ISODate);
        qDebug() << "Last edit date =" << lastEditDate.toString(Qt::ISODate);
        qDebug() << "Mimetype =" << mimetype.toStdString();
        qDebug() << "Deleted =" << deleted;
        qDebug("/////// END OF INFO //////");
    }
};

#endif // TFILEINFO_H
