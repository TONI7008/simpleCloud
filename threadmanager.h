#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include "tfileinfo.h"
#include <QQueue>
#include <QDataStream>
#include <QDesktopServices>
#include <QThread>
#include <QList>

// Forward declarations
class Loader;
class UClient;
class DClient;
class SettingsManager;
class mainThread;

// Struct to hold file pairs (filename and filepath)
struct filePair {
    filePair(QString name = "", QString path = "")
        : filename(name), filepath(path) {}

    QString filename;
    QString filepath;

    // Overload the << operator for QDataStream
    friend QDataStream& operator<<(QDataStream& out, const filePair& pair) {
        out << pair.filename << pair.filepath;
        return out;
    }

    // Overload the >> operator for QDataStream
    friend QDataStream& operator>>(QDataStream& in, filePair& pair) {
        in >> pair.filename >> pair.filepath;
        return in;
    }

    // Overload the == operator for comparison
    bool operator==(const filePair& other) const {
        return filename == other.filename && filepath == other.filepath;
    }
};

// Struct to hold queue items (filename, username, and Loader pointer)
struct queueItem {
    queueItem(QString filename = "", QString username = "", Loader* loader = nullptr)
        : m_filename(filename), m_username(username), m_loader(loader) {}

    QString first() const { return m_filename; }
    QString second() const { return m_username; }
    Loader* third() const { return m_loader; }

    QString m_filename;
    QString m_username;
    Loader* m_loader;

    // Overload the << operator for QDataStream
    friend QDataStream& operator<<(QDataStream& out, const queueItem& item) {
        out << item.m_filename << item.m_username << reinterpret_cast<quintptr>(item.m_loader);
        return out;
    }

    // Overload the >> operator for QDataStream
    friend QDataStream& operator>>(QDataStream& in, queueItem& item) {
        QString filename, username;
        quintptr loaderPtr;
        in >> filename >> username >> loaderPtr;
        item.m_filename = filename;
        item.m_username = username;
        item.m_loader = reinterpret_cast<Loader*>(loaderPtr);
        return in;
    }

    // Overload the == operator for comparison
    bool operator==(const queueItem& other) const {
        return m_filename == other.m_filename &&
               m_username == other.m_username &&
               m_loader == other.m_loader; // Note: This checks pointer equality
    }
};

// Custom queue class based on QList
template <class T>
class TQueue : public QList<T> {
public:
    inline void swap(TQueue<T>& other) noexcept { QList<T>::swap(other); } // Prevent QList<->QQueue swaps
    inline void enqueue(const T& t) { QList<T>::append(t); }
    inline T dequeue() { return QList<T>::takeFirst(); }
    inline T& head() { return QList<T>::first(); }
    inline const T& head() const { return QList<T>::first(); }
};

// ThreadManager class definition
class ThreadManager : public QThread {
    Q_OBJECT

public:
    explicit ThreadManager(mainThread* th, QObject* parent = nullptr);
    ~ThreadManager();

    void upload(QString filename, QString username, Loader* loader);
    void download(QString filename, QString username, Loader* loader);
    void downloadII(QString filename, QString username, Loader* loader);

    void setPath(QString path);
    void openFile(QString filename);

    void setUploadLimit(short limit);
    void setDownloadLimit(short limit);

    void Stop() { stop = true; }
    void setManager(SettingsManager* manager);
    void Start();

signals:
    void uploadDone(TFileInfo);

protected:
    void run() override; // Override QThread's run() method

private:
    void startUpload(QString filename, QString username, Loader* loader);
    void startDownload(QString filename, QString username, Loader* loader);
    void startDownloadII(QString filename, QString username, Loader* loader);

    QList<DClient*> Dlist;
    QList<UClient*> Ulist;
    QList<filePair> downloadedFile;

    TQueue<queueItem> uploadQueue;
    TQueue<queueItem> downloadQueue;

    SettingsManager* m_manager;
    mainThread* m_thread;

    short limit_download;
    short limit_upload;

    bool stop = false;
    QString m_path = ":/";

    friend class ThreadManagerHelper;
};

#endif // THREADMANAGER_H
