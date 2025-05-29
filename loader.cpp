#include "loader.h"
#include "tframe.h"

Loader::Loader(TFrame* tf,QObject *parent)
    : AnimationHandler(parent), m_frame(tf),started(false),paused(false) {
    init();
}

Loader::~Loader(){

}

void Loader::init() {
    if (m_frame) {
        // Keep the frame with fixed size as required
        m_frame->hide();
        m_frame->setBorderSize(4);
        m_frame->setEnabledBorder(true);
        m_frame->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        m_frame->setAttribute(Qt::WA_TranslucentBackground);
        m_frame->raise();


        m_speedLabel = new QLabel(m_frame);
        m_speedLabel->setMinimumHeight(30);
        m_speedLabel->setText("Speed: 0 KB/s");
        m_speedLabel->setAlignment(Qt::AlignCenter);
        m_speedLabel->setStyleSheet("font-size: 13px; color: #ffffff;");  // Smaller font to fit

        m_progressLabel = new QLabel(m_frame);
        m_progressLabel->setMinimumHeight(30);
        m_progressLabel->setText("/");
        m_progressLabel->setAlignment(Qt::AlignCenter);
        m_progressLabel->setStyleSheet("font-size: 13px; color: #ffffff;");  // Smaller font to fit

        m_bar = new CircularProgressBar(m_frame);
        m_bar->setEnableText(false);
        m_bar->setProgressWidth(7);
        //m_bar->setFixedSize(120, 120);  // Smaller size to fit within the limited space
        m_bar->setChunkColor(QColor(255, 237, 234));  // Soft color for aesthetics

        initializeEffect(m_frame);

        QVBoxLayout *layout = new QVBoxLayout(m_frame);
        layout->setContentsMargins(5, 2, 5, 2);  // Small margins to maximize usable space
        layout->setSpacing(1);  // Reduced spacing to fit elements

        layout->addWidget(m_speedLabel, 0, Qt::AlignCenter);  // Speed label on top
        layout->addWidget(m_bar, 0, Qt::AlignCenter);         // Progress bar in the middle
        layout->addWidget(m_progressLabel, 0, Qt::AlignCenter);  // Progress label below the bar

        m_frame->setLayout(layout);

        connect(opacityAnimation, &QPropertyAnimation::finished, this, [this]() {
            if (show) {
                m_frame->hide();
                show = false;
                m_effect->setOpacity(0);
                if (terminated) {
                    emit done();
                }
            } else {
                show = true;
                m_effect->setOpacity(1);
                m_frame->show();
                m_frame->raise();
            }
        });

        connect(m_bar,&CircularProgressBar::stopEmitted,this,[this]{
            if(started){
                if(!paused){
                    paused=true;
                    startOpacityAnimation(1.0, 0.0);
                }else{
                    startOpacityAnimation(0.0, 1.0);
                    paused=false;
                }

            }
        });
    }
}


void Loader::start() {
    if(started) return;
    started=true;
    startOpacityAnimation(0.0, 1.0);
}
void Loader::resume() {
    if(!paused) return;
    m_bar->click();
}

void Loader::terminate() {
    if(!started) return;
    started=false;
    terminated = true;
    startOpacityAnimation(1.0, 0.0);
}

void Loader::setRange(int min, int max) {
    m_bar->setRange(min, max);
}

void Loader::setValue(int value) {
    m_bar->setValue(value);
    if (value >= m_bar->maximum()) {
        terminate();
    }
}

void Loader::setLoop(bool set) {
    m_bar->setInfiniteLoop(set);
}

void Loader::setSpeed(QString speed)
{
    m_speedLabel->setText(speed);
}

void Loader::setProgress(QString progress)
{
    m_progressLabel->setText(progress);
}
