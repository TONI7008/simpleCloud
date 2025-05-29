#include "popoutframe.h"
#include <QPainter>
#include <QDebug>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QTimer>

PopoutFrame::PopoutFrame(QWidget* parent) : TFrame(parent), animationGroup(new QSequentialAnimationGroup(this)) {
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setAttribute(Qt::WA_Hover);

    setWindowFlag(Qt::FramelessWindowHint);

    setMouseTracking(true);


    setBorder(true);
    setBorderSize(4);

    setEnableBackground(true);


}

PopoutFrame::~PopoutFrame() {
    if (shrinkAnim) shrinkAnim->deleteLater();
    if (expandAnim) expandAnim->deleteLater();
    animationGroup->deleteLater();
}

void PopoutFrame::popout() {
    popout(m_popoutDuration, m_popoutScale);
}


QRect PopoutFrame::updateExpandedGeometry() {
    int dw = width() * (m_popoutScale - 1.0) * m_widthExpansionFactor;
    int dh = height() * (m_popoutScale - 1.0);
    QRect expandedGeometry = originalGeometry.adjusted(-dw / 2, -dh / 2, dw / 2, dh / 2);

    // Apply max width constraint if set
    if (m_popoutMaxWidth > 0 && expandedGeometry.width() > m_popoutMaxWidth) {
        int newWidth = m_popoutMaxWidth;
        int newX = originalGeometry.x() + (originalGeometry.width() - newWidth) / 2;
        expandedGeometry.setX(newX);
        expandedGeometry.setWidth(newWidth);
    }

    // Apply max height constraint if set
    if (m_popoutMaxHeight > 0 && expandedGeometry.height() > m_popoutMaxHeight) {
        int newHeight = m_popoutMaxHeight;
        int newY = originalGeometry.y() + (originalGeometry.height() - newHeight) / 2;
        expandedGeometry.setY(newY);
        expandedGeometry.setHeight(newHeight);
    }

    return expandedGeometry;
}

void PopoutFrame::setPopoutDuration(int duration) {
    if (duration > 0) {
        m_popoutDuration = duration;
    }
}

void PopoutFrame::setPopoutScale(qreal scale) {
    if (scale > 1.0) {
        m_popoutScale = scale;
    }
}

void PopoutFrame::setEasingCurve(QEasingCurve::Type curve) {
    currentCurve = curve;
}

void PopoutFrame::setPopoutColor(const QColor& color) {
    borderColor = color;
    setBorderColor(color);
}

void PopoutFrame::setPopoutMaxWidth(int maxWidth) {
    m_popoutMaxWidth = maxWidth;
    setMaximumWidth(maxWidth);
}

void PopoutFrame::setPopoutMaxHeight(int maxHeight) {
    m_popoutMaxHeight = maxHeight;
    setMaximumHeight(m_popoutMaxHeight);
}

void PopoutFrame::moveEvent(QMoveEvent* event) {
    // Update original geometry if we move while not expanded
    if (geometry().size() == originalGeometry.size()) {
        originalGeometry.moveTo(event->pos());
    }
    TFrame::moveEvent(event);
}

void PopoutFrame::resizeEvent(QResizeEvent *event)
{
    TFrame::resizeEvent(event);
    setRoundness(height()/2);
}


void PopoutFrame::popout(int duration, qreal scale) {
    m_popoutScale=scale;
    originalGeometry = geometry();
    QRect expanded = calculateExpandedGeometry(originalGeometry);

    delete expandAnim;
    delete shrinkAnim;

    expandAnim = new QPropertyAnimation(this, "geometry");
    expandAnim->setDuration(duration);
    expandAnim->setStartValue(originalGeometry);
    expandAnim->setEndValue(expanded);
    expandAnim->setEasingCurve(currentCurve);

    shrinkAnim = new QPropertyAnimation(this, "geometry");
    shrinkAnim->setDuration(duration / 2);
    shrinkAnim->setStartValue(expanded);
    shrinkAnim->setEndValue(originalGeometry);
    shrinkAnim->setEasingCurve(currentCurve);

    animationGroup->clear();
    animationGroup->addAnimation(expandAnim);
    animationGroup->addAnimation(shrinkAnim);
    animationGroup->start();
}

QRect PopoutFrame::calculateExpandedGeometry(const QRect& baseGeometry) {
    int dw = baseGeometry.width() * (m_popoutScale - 1.0) * m_widthExpansionFactor;
    int dh = baseGeometry.height() * (m_popoutScale - 1.0);
    QRect expanded = baseGeometry.adjusted(-dw / 2, -dh / 2, dw / 2, dh / 2);

    QSize maxSize(m_popoutMaxWidth > 0 ? m_popoutMaxWidth : INT_MAX,
                  m_popoutMaxHeight > 0 ? m_popoutMaxHeight : INT_MAX);
    if (expanded.width() > maxSize.width())
        expanded.setWidth(maxSize.width());
    if (expanded.height() > maxSize.height())
        expanded.setHeight(maxSize.height());

    QPoint center = baseGeometry.center();
    expanded.moveCenter(center);
    return expanded;
}

void PopoutFrame::setMaximumSize(const QSize& size) {
    setPopoutMaxWidth(size.width());
    setPopoutMaxHeight(size.height());
    QFrame::setMaximumSize(size);
}

void PopoutFrame::enterEvent(QEnterEvent* event) {
    if(isHovered) return;
    isHovered = true;
    emit hoverStarted();

    if (animationGroup->state() == QAbstractAnimation::Running)
        animationGroup->stop();

    originalGeometry = geometry();
    startExpandAnimation();
    TFrame::enterEvent(event);
}

void PopoutFrame::leaveEvent(QEvent* event) {
    if(!isHovered) return;
    QTimer::singleShot(m_popoutDuration/2+5,this,[this]{
        isHovered=false;
    });

    emit hoverEnded();

    if (expandAnim && expandAnim->state() == QAbstractAnimation::Running)
        expandAnim->stop();

    startShrinkAnimation();

    TFrame::leaveEvent(event);
}

void PopoutFrame::startExpandAnimation() {
    expandAnim = new QPropertyAnimation(this, "geometry");
    expandAnim->setDuration(m_popoutDuration/2);
    expandAnim->setStartValue(geometry());
    expandAnim->setEndValue(calculateExpandedGeometry(geometry()));
    expandAnim->setEasingCurve(currentCurve);
    expandAnim->start();
}

void PopoutFrame::startShrinkAnimation() {
    shrinkAnim = new QPropertyAnimation(this, "geometry");
    shrinkAnim->setDuration(m_popoutDuration / 2);
    shrinkAnim->setStartValue(geometry());
    shrinkAnim->setEndValue(originalGeometry);
    shrinkAnim->setEasingCurve(currentCurve);
    shrinkAnim->start();
}


