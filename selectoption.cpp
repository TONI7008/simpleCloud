#include "selectoption.h"

SelectOption::SelectOption(QWidget *parent)
    : QWidget(parent), m_chunkPos(0), m_chunkWidth(50), m_chunkHeight(8), m_color(QColor(71, 158, 245, 255)),
    m_orientation(Qt::Horizontal),
    m_duration(150)
{
    setFixedHeight(8);
    setMinimumWidth(50);
    m_animation = new QPropertyAnimation(this, "chunkPos");
    m_animation->setDuration(m_duration);
    m_animation->setEasingCurve(QEasingCurve::OutQuad);
}

int SelectOption::chunkPos() const {
    return m_chunkPos;
}

void SelectOption::setChunkPos(int pos) {
    m_chunkPos = pos;
    update();
}

int SelectOption::chunkWidth() const {
    return m_chunkWidth;
}

void SelectOption::setChunkWidth(int width) {
    m_chunkWidth = width;
    update();
}

int SelectOption::chunkHeight() const {
    return m_chunkHeight;
}

void SelectOption::setChunkHeight(int height) {
    m_chunkHeight = height;
    if(m_orientation==Qt::Horizontal){
        setFixedHeight(height);
    }else{
        setMaximumHeight(16777215);
        setMinimumHeight(height);
    }
    update();
}

Qt::Orientation SelectOption::orientation() const {
    return m_orientation;
}

void SelectOption::setOrientation(Qt::Orientation orientation) {
    m_orientation = orientation;
    if (orientation == Qt::Vertical) {
        setFixedWidth(8);
        setChunkHeight(45);
        setChunkWidth(5);
        m_chunkPos = 0;
    } else {
        setFixedHeight(8);
        setMinimumWidth(50);
        m_chunkPos = 0;
    }
    update();
}

void SelectOption::setChunkColor(QColor color) {
    m_color = color;
    update();
}

void SelectOption::moveLeft(int pixels) {
    if (m_orientation == Qt::Horizontal) {
        int targetPos = qMax(0, m_chunkPos - pixels);
        m_animation->setStartValue(m_chunkPos);
        m_animation->setEndValue(targetPos);
        m_animation->start();
    }
}

void SelectOption::moveRight(int pixels) {
    if (m_orientation == Qt::Horizontal) {
        int targetPos = qMin(width() - m_chunkWidth, m_chunkPos + pixels);
        m_animation->setStartValue(m_chunkPos);
        m_animation->setEndValue(targetPos);
        m_animation->start();
    }
}

void SelectOption::moveUp(int pixels) {
    if (m_orientation == Qt::Vertical) {
        int targetPos = qMax(0, m_chunkPos - pixels);
        m_animation->setStartValue(m_chunkPos);
        m_animation->setEndValue(targetPos);
        m_animation->start();
    }
}

void SelectOption::moveDown(int pixels) {
    if (m_orientation == Qt::Vertical) {
        int targetPos = qMin(height() - m_chunkHeight, m_chunkPos + pixels);
        m_animation->setStartValue(m_chunkPos);
        m_animation->setEndValue(targetPos);
        m_animation->start();
    }
}

void SelectOption::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(m_color);
    painter.setPen(Qt::NoPen);

    if (m_orientation == Qt::Horizontal) {
        painter.drawRoundedRect(m_chunkPos, 1, m_chunkWidth, m_chunkHeight, 5, 5);
    } else {
        painter.drawRoundedRect(1, m_chunkPos, m_chunkWidth, m_chunkHeight, 5, 5);
    }
}
