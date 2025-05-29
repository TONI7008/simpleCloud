#include "tframe.h"

TFrame::TFrame(QWidget* parent) : QFrame(parent){
    timer = new QTimer(this);
    timer->setSingleShot(true);

    connect(timer, &QTimer::timeout, this, [this] {
        if (!isDoubleClick) {
            emit Clicked(m_clickPos);
        }
        isDoubleClick = false;
    });

}

TFrame::~TFrame() {
    timer->stop();
    delete timer;
}

void TFrame::mousePressEvent(QMouseEvent* event) {
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
        timer->start(250); // Wait to see if a double-click follows
    }
}

void TFrame::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDoubleClick = true;
        timer->stop();
        emit doubleClicked(event->pos());
    }
}

void TFrame::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !isDoubleClick){

    }

}


void TFrame::setEnabledBorder(bool f)
{
    enable_border=f;
    repaint();
}

void TFrame::setEnableBackground(bool b){
    if(b==enable_background && !enable_background){
        m_backgroundImage=QPixmap();
    }
    enable_background = b;
    repaint();
}

void TFrame::setBackgroundImage(const QString& path) {
    m_backgroundImage = QPixmap(path);
    repaint();
}

void TFrame::setBackgroundImage(const QPixmap& pixmap)
{
    m_backgroundImage=pixmap;
    repaint();
}


void TFrame::setBackgroundColor(const QColor &color)
{
    QSize frameSize = size();

    if (frameSize.isEmpty()) {
        frameSize = QSize(1, 1);
    }

    // Create a QPixmap of the frame's size
    QPixmap colorPixmap(frameSize);
    // Fill the pixmap with the specified color
    colorPixmap.fill(color);

    // Set this newly created pixmap as the background image
    setBackgroundImage(colorPixmap);
}


void TFrame::setRoundness(short round) {
    roundness = round;
    repaint();
}

void TFrame::setCornerStyle(CornerStyle style) {
    cornerStyle = style;
    repaint();
}

void TFrame::setBorder(bool b) {
    enable_border = b;
    repaint();
}

void TFrame::setBorderSize(short size) {
    borderSize = size;
    repaint();
}

void TFrame::setBorderColor(const QColor& color) {
    borderColorValue = color;
    repaint();
}

QColor TFrame::borderColor() const {
    return borderColorValue;
}

void TFrame::paintEvent(QPaintEvent *event) {
    QFrame::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath path;
    QRect r = rect();
    int rSize = roundness;

    // Use exact coordinates instead of bottomLeft()/topRight() etc.
    int left = r.left();
    int top = r.top();
    int right = r.right();
    int bottom = r.bottom();
    int width = r.width();
    int height = r.height();

    switch (cornerStyle) {
    case CornerStyle::TopOnly:
        // Rounded corners on the top only
        path.moveTo(left, bottom);
        path.lineTo(left, top + rSize);
        path.arcTo(QRect(left, top, 2 * rSize, 2 * rSize), 180, -90);
        path.lineTo(right - rSize, top);
        path.arcTo(QRect(right - 2 * rSize, top, 2 * rSize, 2 * rSize), 90, -90);
        path.lineTo(right, bottom);
        path.lineTo(left, bottom);
        break;

    case CornerStyle::BottomOnly:
        // Rounded corners on the bottom only
        path.moveTo(left, top);
        path.lineTo(left, bottom - rSize);
        path.arcTo(QRect(left, bottom - 2 * rSize, 2 * rSize, 2 * rSize), 180, 90);
        path.lineTo(right - rSize, bottom);
        path.arcTo(QRect(right - 2 * rSize, bottom - 2 * rSize, 2 * rSize, 2 * rSize), 270, 90);
        path.lineTo(right, top);
        path.lineTo(left, top);
        break;

    case CornerStyle::LeftOnly:
        // Rounded corners on the left only
        path.moveTo(right, top);
        path.lineTo(left + rSize, top);
        path.arcTo(QRect(left, top, 2 * rSize, 2 * rSize), 90, 90);
        path.lineTo(left, bottom - rSize);
        path.arcTo(QRect(left, bottom - 2 * rSize, 2 * rSize, 2 * rSize), 180, 90);
        path.lineTo(right, bottom);
        path.lineTo(right, top);
        break;

    case CornerStyle::RightOnly:
        // Rounded corners on the right only
        path.moveTo(left, top);
        path.lineTo(right - rSize, top);
        path.arcTo(QRect(right - 2 * rSize, top, 2 * rSize, 2 * rSize), 90, -90);
        path.lineTo(right, bottom - rSize);
        path.arcTo(QRect(right - 2 * rSize, bottom - 2 * rSize, 2 * rSize, 2 * rSize), 0, -90);
        path.lineTo(left, bottom);
        path.lineTo(left, top);
        break;

    case CornerStyle::BottomLeft:
        // Rounded corner on the bottom-left only
        path.moveTo(right, top);
        path.lineTo(left, top);
        path.lineTo(left, bottom - rSize);
        path.arcTo(QRect(left, bottom - 2 * rSize, 2 * rSize, 2 * rSize), 180, 90);
        path.lineTo(right, bottom);
        path.lineTo(right, top);
        break;

    case CornerStyle::BottomRight:
        // Rounded corner on the bottom-right only
        path.moveTo(left, top);
        path.lineTo(right, top);
        path.lineTo(right, bottom - rSize);
        path.arcTo(QRect(right - 2 * rSize, bottom - 2 * rSize, 2 * rSize, 2 * rSize), 0, -90);
        path.lineTo(left, bottom);
        path.lineTo(left, top);
        break;

    case CornerStyle::None:
        // No rounded corners
        path.addRect(r);
        break;

    case CornerStyle::Default:
    default:
        // Rounded corners on all sides
        path.addRoundedRect(r, rSize, rSize);
        break;
    }

    painter.setClipPath(path);

    if(enable_background){
        painter.drawPixmap(0, 0, width, height, m_backgroundImage);
    }

    // Draw the border if enabled
    if (enable_border && borderSize > 0) {
        QPen pen(borderColorValue, borderSize);
        painter.setPen(pen);
        pen.setJoinStyle(Qt::MiterJoin);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }
}


void TFrame::resizeEvent(QResizeEvent *event) {
    QFrame::resizeEvent(event);
    emit resizing();
}

void TFrame::moveEvent(QMoveEvent *event) {
    QFrame::moveEvent(event);
    emit isMoving();
}

short TFrame::getClickInterval() const
{
    return clickInterval;
}

void TFrame::setClickInterval(short newClickInterval)
{
    clickInterval = newClickInterval;
}

void TFrame::showEvent(QShowEvent *event) {
    QFrame::showEvent(event);
    emit isShown();
}

void TFrame::hideEvent(QHideEvent *event) {
    QFrame::hideEvent(event);
    emit isHiden();
}
