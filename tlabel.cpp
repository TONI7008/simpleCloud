#include "tlabel.h"
#include "tcloud.h"

TLabel::TLabel(QWidget* parent) : ElidedLabel(parent) {
    setAcceptDrops(true);
    timer = new QTimer(this);
    timer->setSingleShot(true);
    setAttribute(Qt::WA_StyledBackground, true);

    connect(timer, &QTimer::timeout, this, [&] {
        if (!isDoubleClick) {
            emit Clicked(m_clickPos);
        }
        isDoubleClick = false;
    });

    QString labelStyle = R"(
    background:rgba(150,120,120,0.7);background:rgba(50,50,50,0.7);
    background:transparent;border-radius:1px;
    font: 12pt "Noto Sans";
)";
    setStyleSheet(labelStyle);

}

void TLabel::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) {
        emit rightClicked(event->pos());
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_clickPos = event->pos();
        isDoubleClick = false;

        if (event->modifiers() & Qt::ControlModifier) {
            emit ctrlSelected();
        }
        timer->start(TCLOUD::DoubleClickTime); // Wait to see if a double-click follows
    }
}

void TLabel::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDoubleClick = true;
        timer->stop();
        emit doubleClicked(event->pos());
    }
}

void TLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !isDoubleClick){

    }

}

TLabel::TLabel(QString content, QWidget *parent) : TLabel(parent) {
    setText(content);
}

TLabel::~TLabel() {
    timer->deleteLater();
}

void TLabel::click() {
    emit Clicked(this->pos());
}
