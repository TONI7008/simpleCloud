#ifndef LOADER_H
#define LOADER_H

#include "circularprogressbar.h"
#include <QLabel>
#include "animhandler.h"

class TFrame;

class Loader : public AnimationHandler{
    Q_OBJECT
public:
    explicit Loader(TFrame*,QObject *parent = nullptr);
    ~Loader();
    void start();
    void terminate();

    void setRange(int min, int max);
    void setValue(int value);
    void setLoop(bool);
    void setSpeed(QString);
    void setProgress(QString);
    CircularProgressBar* progressBar(){
        return m_bar;
    }

    void resume();
signals:
    void done();

private:
    TFrame* m_frame;
    QLabel* m_speedLabel;
    QLabel* m_progressLabel;
    bool show=false;
    CircularProgressBar* m_bar;
    bool terminated=false;
    void init();
    bool started;
    bool paused;
};

#endif // LOADER_H
