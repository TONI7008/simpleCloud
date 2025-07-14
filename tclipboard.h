// tclipboard.h
#ifndef TCLIPBOARD_H
#define TCLIPBOARD_H

#include <QObject>
#include <QStringList>
#include "tcloudelt.h"

class TFolder;
class TFile;
class NetworkAgent;

class TClipBoard : public QObject {
    Q_OBJECT
public:
    enum TransactionType { None, Copy, Cut, SoftDelete };

    static TClipBoard* instance();
    static void Init();
    static void cleanUp();

    void set(TransactionType type, const QList<eltCore>& items);
    void set(TransactionType type, const eltCore& item);
    void preparePaste();
    void pasteTo(TFolder* targetFolder, NetworkAgent* agent);
    void clear();
    TransactionType type() const { return m_type; }
    const QList<eltCore>& items() const { return m_items; }

signals:
    void operationDone(bool success, QString message);

private slots:
    void handleCopySuccess();
    void handleCopyFailed(QString);
    void handleCutSuccess();
    void handleCutFailed(QString);
    void handleDeleteSuccess();
    void handleDeleteFailed(QString);

private:
    TClipBoard();
    ~TClipBoard();
    static TClipBoard* s_instance;

    TransactionType m_type = None;
    QList<eltCore> m_items;
    QList<eltCore> m_rootOps;
    QString m_targetPath;
    TFolder* m_targetFolder = nullptr;
    NetworkAgent* m_agent = nullptr;

    int m_pendingCount = 0;
    void connectSignals();
};

#endif // TCLIPBOARD_H
