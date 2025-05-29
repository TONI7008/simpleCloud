#ifndef TFILECHECKER_H
#define TFILECHECKER_H

#include <QObject>
#include <QTimer>

class TFolder;

class TFileChecker : public QObject
{
public:

    bool check(QString filename);
    void findIdenticals();
    void findSamenameFiles();
    bool isOk(QString filename,TFolder* folder);

    static void cleanUp();
    static void Init();
    static TFileChecker* instance();
    static TFileChecker* m_checker;


private:
    explicit TFileChecker(QObject *parent = nullptr);
    QStringList identicalFiles;
    QStringList SameNameFiles;

};

#endif // TFILECHECKER_H
