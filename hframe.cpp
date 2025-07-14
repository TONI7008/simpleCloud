#include "hframe.h"


HFrame::HFrame(QWidget *parent) : TFrame(parent) {
    disableBackground();
    setClickInterval(5000);
}

void HFrame::enterEvent(QEnterEvent *event) {
    QFrame::enterEvent(event);
    emit hoverStarted();
}

void HFrame::leaveEvent(QEvent *event) {
    QFrame::leaveEvent(event);
    emit hoverEnded();
}
