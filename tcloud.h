#ifndef TCLOUD_H
#define TCLOUD_H

#include <QTimer>
#include <QEventLoop>

namespace TCLOUD {
enum Theme {
    Dark,
    Light
};

enum View{
    Linear,
    Grid
};
inline const short int DoubleClickTime=250;
inline void Wait(int ms) {
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();
}
inline QString formatSize(quint64 size){
    double convertedSize = static_cast<double>(size);
    QString unit = "bytes";

    if (size >= 1024 && size < 1024 * 1024) {
        convertedSize = size / 1024.0;
        unit = "KB";
    } else if (size >= 1024 * 1024 && size < 1024 * 1024 * 1024) {
        convertedSize = size / (1024.0 * 1024);
        unit = "MB";
    } else if (size >= 1024 * 1024 * 1024) {
        convertedSize = size / (1024.0 * 1024 * 1024);
        unit = "GB";
    }

    return QString::number(convertedSize, 'f', 2) + "" + unit;
}

inline QString capitalize(const QString &str){
    QString tmp = str;
    tmp = tmp.toLower();
    tmp[0] = str[0].toUpper();
    return tmp;
}

inline QString serverAdress="104.159.90.176";
inline quint16 port=1244;
inline quint16 sendFilePort=1245;
inline quint16 receiveFilePort=1246;
}



#endif // TCLOUD_H
