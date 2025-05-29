#include "tpushbutton.h"
#include "tcloud.h"

TPushButton::TPushButton(QWidget* parent) : QPushButton(parent) {

    timer = new QTimer(this);
    timer->setSingleShot(true);

    connect(timer, &QTimer::timeout, this,[&] {
        if(!isDoubleClick){
            emit Clicked(m_clickPos);
        }
        isDoubleClick=false;
    });
}

TPushButton::~TPushButton()
{
    delete timer;
}

void TPushButton::Click(){
    emit Clicked(this->pos());
}

void TPushButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDoubleClick=true;
        timer->stop();
        emit doubleClicked(event->pos());
    }
    //QPushButton::mouseDoubleClickEvent(event);
}

void TPushButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) {
        emit rightClicked(event->pos());
    } else if (event->button() == Qt::LeftButton) {
        if (event->modifiers() & Qt::ControlModifier) {
            emit ctrlSelected();
        } else {
            m_clickPos=event->pos();
            if (!timer->isActive()) {
                timer->start(TCLOUD::DoubleClickTime);
            }
        }
    }
    //QPushButton::mousePressEvent(event); // Avoid re-emitting the signal
}
