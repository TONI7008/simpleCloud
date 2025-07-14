#include "twidget.h"
#include <QPainter>
#include <QFile>
#include "tcloud.hpp"

TWidget::TWidget(QWidget *parent)
    : MarginDetectingWidget(parent), isDoubleClick(false), enableBackground(false), enableBorder(false),
    borderSize(0), borderColorValue(QColor(71, 158, 245)), m_bRadius(0) {


    //m_backgroundImage=QPixmap::fromImage(TCLOUD::defaultWidgetImage());

    //setAttribute(Qt::WA_NoMousePropagation, false);
    //setAttribute(Qt::WA_MouseNoMask, true);
    timer = new QTimer(this);
    timer->setSingleShot(true);

    connect(timer, &QTimer::timeout, this, [this] {
        if (!isDoubleClick) {
            emit Clicked(m_clickPos);
        }
        isDoubleClick = false;
    });


}

TWidget::~TWidget() {
    timer->stop();
    delete timer;
}

void TWidget::setEnableBackground(bool enable) {
    if(enable==enableBackground && !enableBackground){
        m_backgroundImage=QPixmap();
    }
    enableBackground = enable;
    repaint();
}

void TWidget::setBackgroundImage(const QString& image) {
    m_backgroundImage = QPixmap(image);
    repaint();
}

void TWidget::setBackgroundImage(const QPixmap &pix)
{
    m_backgroundImage=pix;
    repaint();
}

void TWidget::setBackgroundColor(const QColor &color)
{

    QSize frameSize = size();

    if (frameSize.isEmpty()) {
        frameSize = QSize(1, 1);
    }
    QPixmap colorPixmap(frameSize);

    colorPixmap.fill(color);
    setBackgroundImage(colorPixmap);

}

void TWidget::setBorder(bool enable) {
    if (enableBorder != enable) {
        enableBorder = enable;
        repaint();
    }
}

void TWidget::setBorderSize(short size) {
    if (borderSize != size) {
        borderSize = size;
        update();
    }
}

void TWidget::setBorderColor(const QColor& color) {
    if (borderColorValue != color) {
        borderColorValue = color;
        repaint();
    }
}

void TWidget::setCornerStyle(CornerStyle style) {
    if (m_cornerStyle != style) {
        m_cornerStyle = style;
        repaint();
    }
}

void TWidget::setBorderRadius(short radius) {
    if (m_bRadius != radius) {
        m_bRadius = radius;
        repaint();
    }
}

short TWidget::borderRadius() const {
    return m_bRadius;
}

bool TWidget::borderEnabled() const {
    return enableBorder;
}

QColor TWidget::borderColor() const {
    return borderColorValue;
}

TWidget::CornerStyle TWidget::cornerStyle() const {
    return m_cornerStyle;
}

void TWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    emit resizing();
}

void TWidget::mousePressEvent(QMouseEvent *event) {

    isDoubleClick = false;

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

    if (event->button() == Qt::XButton1) {
        emit previousPressed();
    } else if (event->button() == Qt::XButton2) {
        emit nextPressed();
    }
    //MarginDetectingWidget::mousePressEvent(event);
}

void TWidget::mouseDoubleClickEvent(QMouseEvent *event) {

    if (event->button() == Qt::LeftButton) {
        isDoubleClick = true;
        timer->stop();
        emit doubleClicked(event->pos());
    }
    MarginDetectingWidget::mouseDoubleClickEvent(event);
}


void TWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !isDoubleClick){

    }

}



void TWidget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    if (windowFlags() & Qt::Dialog) {
        QPainter painter(this);
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.fillRect(rect(), Qt::transparent);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath path;
    QRect r = rect();
    int rSize = m_bRadius;


    switch (m_cornerStyle) {
    case CornerStyle::TopOnly:
        path.moveTo(r.bottomLeft());
        path.lineTo(r.topLeft() + QPoint(0, rSize));
        path.arcTo(QRect(r.topLeft(), QSize(2 * rSize, 2 * rSize)), 180, -90);
        path.lineTo(r.topRight() - QPoint(rSize, 0));
        path.arcTo(QRect(r.topRight() - QPoint(2 * rSize, 0), QSize(2 * rSize, 2 * rSize)), 90, -90);
        path.lineTo(r.bottomRight());
        path.lineTo(r.bottomLeft());
        break;

    case CornerStyle::BottomOnly:
        path.moveTo(r.topLeft());
        path.lineTo(r.bottomLeft() - QPoint(0, rSize));
        path.arcTo(QRect(r.bottomLeft() - QPoint(0, 2 * rSize), QSize(2 * rSize, 2 * rSize)), 180, 90);
        path.lineTo(r.bottomRight() - QPoint(rSize, 0));
        path.arcTo(QRect(r.bottomRight() - QPoint(2 * rSize, 2 * rSize), QSize(2 * rSize, 2 * rSize)), 270, 90);
        path.lineTo(r.topRight());
        path.lineTo(r.topLeft());
        break;

    case CornerStyle::LeftOnly:
        path.moveTo(r.topRight());
        path.lineTo(r.topLeft() + QPoint(rSize, 0));
        path.arcTo(QRect(r.topLeft(), QSize(2 * rSize, 2 * rSize)), 90, 90);
        path.lineTo(r.bottomLeft() + QPoint(0, -rSize));
        path.arcTo(QRect(r.bottomLeft() - QPoint(0, 2 * rSize), QSize(2 * rSize, 2 * rSize)), 180, 90);
        path.lineTo(r.bottomRight());
        path.lineTo(r.topRight());
        break;

    case CornerStyle::RightOnly:
        path.moveTo(r.topLeft());
        path.lineTo(r.topRight() - QPoint(rSize, 0));
        path.arcTo(QRect(r.topRight() - QPoint(2 * rSize, 0), QSize(2 * rSize, 2 * rSize)), 90, -90);
        path.lineTo(r.bottomRight() - QPoint(0, rSize));
        path.arcTo(QRect(r.bottomRight() - QPoint(2 * rSize, 2 * rSize), QSize(2 * rSize, 2 * rSize)), 0, -90);
        path.lineTo(r.bottomLeft());
        path.lineTo(r.topLeft());
        break;

    case CornerStyle::BottomLeft:
        // Rounded corner on the bottom-left only
        path.moveTo(r.topRight());
        path.lineTo(r.topLeft());
        path.lineTo(r.bottomLeft() - QPoint(0, rSize));
        path.arcTo(QRect(r.bottomLeft() - QPoint(0, 2 * rSize), QSize(2 * rSize, 2 * rSize)), 180, 90);
        path.lineTo(r.bottomRight());
        path.lineTo(r.topRight());
        break;

    case CornerStyle::BottomRight:
        // Rounded corner on the bottom-right only
        path.moveTo(r.topLeft());
        path.lineTo(r.topRight());
        path.lineTo(r.bottomRight() - QPoint(0, rSize));
        path.arcTo(QRect(path.currentPosition().toPoint() - QPoint(2 * rSize, rSize), QSize(2 * rSize, 2 * rSize)), 0, -90);
        path.lineTo(r.bottomLeft());
        path.lineTo(r.topLeft());
        break;

    case CornerStyle::None:
        path.addRect(r);
        break;

    case CornerStyle::Default:
    default:
        path.addRoundedRect(r, rSize, rSize);
        break;
    }

    painter.setClipPath(path);

    // Draw background if enabled
    if (enableBackground && !m_backgroundImage.isNull()) {
        painter.drawPixmap(0, 0, width(), height(), m_backgroundImage);
    }

    // Draw border if enabled
    if (enableBorder && borderSize > 0) {
        QPen pen(borderColorValue, borderSize);
        painter.setPen(pen);
        pen.setJoinStyle(Qt::MiterJoin);
        pen.setCapStyle(Qt::RoundCap);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }
}

void TWidget::moveEvent(QMoveEvent *event) {
    MarginDetectingWidget::moveEvent(event);
    emit isMoving();
}

void TWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    emit isShown();
}

void TWidget::hideEvent(QHideEvent *event) {
    QWidget::hideEvent(event);
    emit isHiden();
}

void TWidget::focusInEvent(QFocusEvent *event) {
    QWidget::focusInEvent(event);
    emit focusIn();
}

void TWidget::focusOutEvent(QFocusEvent *event) {
    QWidget::focusOutEvent(event);
    emit focusOut();
}

void TWidget::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() & Qt::ControlModifier && event->modifiers() & Qt::ShiftModifier) {
        if (event->key() == Qt::Key_T) {
            emit debugPressed();
        }
    }
    QWidget::keyPressEvent(event);
}

void TWidget::pressPrev() {
    emit previousPressed();
}

void TWidget::pressNext() {
    emit nextPressed();
}
