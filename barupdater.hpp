#ifndef BARUPDATER_H
#define BARUPDATER_H

#include <QObject>
#include <QList>
#include <QPair>
#include <QDebug>
#include <QThread>

class BarUpdater : public QObject {
    Q_OBJECT
public:
    explicit BarUpdater(QObject* parent = nullptr);
    ~BarUpdater() override;

    inline qint64 getValue() const;

public slots:
    inline void setValue(qint64 size);
    inline void reset();
    inline void stop();
    inline void run();

    // Static/global progress management
    static inline void addSize(qint64 size);
    static inline int transactionCount();
    static inline quint64 globalValue();
    static inline quint64 globalSize();
    static inline BarUpdater* instance();
    static inline void cleanUp();

signals:
    void increased();

private:
    qint64 m_int;
    int m_id;
    bool stopped;

    inline void unregisterSelf();

    // Static members
    static QList<QPair<int, qint64>> s_progressList; // (id, value)
    static int s_nextId;
    static quint64 s_totalSize;
    static BarUpdater* s_instance;
};

// Static member definitions
inline QList<QPair<int, qint64>> BarUpdater::s_progressList;
inline int BarUpdater::s_nextId = 0;
inline quint64 BarUpdater::s_totalSize = 0;
inline BarUpdater* BarUpdater::s_instance = nullptr;

// Constructor implementation
inline BarUpdater::BarUpdater(QObject* parent)
    : QObject(parent), m_int(0), m_id(++s_nextId), stopped(false)
{
    s_progressList.append(qMakePair(m_id, m_int));
}

// Destructor implementation
inline BarUpdater::~BarUpdater() {
    unregisterSelf();
}

// Member function implementations
inline qint64 BarUpdater::getValue() const {
    return m_int;
}

inline void BarUpdater::setValue(qint64 size) {
    m_int = size;
    // Update value in progress list
    for (auto& pair : s_progressList) {
        if (pair.first == m_id) {
            pair.second = m_int;
            break;
        }
    }
    emit increased();
    emit instance()->increased();
}

inline void BarUpdater::reset() {
    setValue(0);
}

inline void BarUpdater::stop() {
    unregisterSelf();
    stopped = true;
}

inline void BarUpdater::run() {
    // Dummy run loop, can be extended as needed
    while (!stopped) {
        // Do nothing
    }
    thread()->quit();
}

// Static function implementations
inline void BarUpdater::addSize(qint64 size) {
    s_totalSize += size;
}

inline int BarUpdater::transactionCount() {
    return s_progressList.size();
}

inline quint64 BarUpdater::globalValue() {
    quint64 result = 0;
    for (const auto& pair : std::as_const(s_progressList)) {
        result += pair.second;
    }
    return result;
}

inline quint64 BarUpdater::globalSize() {
    return s_totalSize;
}

inline BarUpdater* BarUpdater::instance() {
    if (!s_instance) {
        s_instance = new BarUpdater;
        s_instance->unregisterSelf(); // Don't count singleton in progress
    }
    return s_instance;
}

inline void BarUpdater::cleanUp() {
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}

// Private function implementation
inline void BarUpdater::unregisterSelf() {
    for (int i = 0; i < s_progressList.size(); ++i) {
        if (s_progressList[i].first == m_id) {
            s_totalSize-=s_progressList[i].second;
            s_progressList.removeAt(i);
            break;
        }
    }
}

#endif // BARUPDATER_H
