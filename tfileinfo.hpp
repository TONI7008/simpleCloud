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

    friend QDebug operator<<(QDebug debug, const TFileInfo& info) {
        QDebugStateSaver saver(debug); // Saves and restores QDebug's state (e.g., whether compact or not)
        debug.nospace() << "TFileInfo(" << info.filepath
                        << ", Size: " << info.size
                        << ", Modified: " << info.lastEditDate.toString(Qt::ISODate) << ")"
                        << ", Upload Date: " << info.uploadDate.toString(Qt::ISODate) << ")";
        return debug;
    }
};

#endif // TFILEINFO_H
