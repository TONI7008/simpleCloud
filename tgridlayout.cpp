#include "tgridlayout.h"
#include <QWidget>
#include <QPropertyAnimation>
#include <QDebug>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>

TGridLayout::TGridLayout(QWidget *parent)
    : QGridLayout(parent), currentRow(0), currentCol(0), columnCountVar(3), widgetCount(0),
    animationDurationMs(265), easing(QEasingCurve::OutQuad), animationGroup(new QParallelAnimationGroup), zoomedWidgetPtr(nullptr) {

        //setSizeConstraint(QLayout::SetMinAndMaxSize);
    m_baseWidth=150;

}

TGridLayout::~TGridLayout()
{
    delete animationGroup;
}

void TGridLayout::addWidget(QWidget *widget) {
    if (!widget) return;

    updateColumnCount();
    widgetList.append(new WidgetWrapper(currentRow,currentCol,widget, widget->sizePolicy()));

    QGridLayout::addWidget(widget, currentRow, currentCol);

    widgetCount++;

    currentCol++;
    if (currentCol >= columnCountVar) {
        currentCol = 0;
        currentRow++;
    }
    widget->show();
    if (isZoomed()) widget->hide();
    update();
}

void TGridLayout::addWidgetList(QList<QWidget *> widgetL)
{
    for(const auto& elt : widgetL){
        removeWidget(elt);
        addWidget(elt);
    }
    rearrangeWidgets();
}

void TGridLayout::removeWidgetAnimated(QWidget *widget) {
    if (!widget) return;

    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(opacityEffect);

    QPropertyAnimation *fadeAnim = new QPropertyAnimation(opacityEffect, "opacity");
    fadeAnim->setDuration(animationDurationMs/10);
    fadeAnim->setStartValue(1.0);
    fadeAnim->setEndValue(0.0);

    connect(fadeAnim, &QPropertyAnimation::finished,this, [this, widget]() {
        QGridLayout::removeWidget(widget);
        widgetCount--;
        originalGeometries.remove(widget);
        auto it = std::find_if(widgetList.begin(), widgetList.end(), [widget](WidgetWrapper* elt) {
            return elt->widget == widget;
        });

        if (it != widgetList.end()) {
            delete *it;  // Delete the allocated WidgetWrapper
            widgetList.erase(it);
        }
        widget->deleteLater();
        rearrangeWidgets();
    });

    fadeAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

void TGridLayout::removeWidget(QWidget *widget) {
    if (!widget) return;

    // Find the WidgetWrapper pointer corresponding to the widget
    auto it = std::find_if(widgetList.begin(), widgetList.end(), [widget](WidgetWrapper* elt) {
        return elt->widget == widget;
    });

    if (it != widgetList.end()) {
        delete *it;  // Delete the allocated WidgetWrapper
        widgetList.erase(it);
    }

    QGridLayout::removeWidget(widget);
    widget->hide();

}


void TGridLayout::removeAll() {
    while (!widgetList.isEmpty()) {
        QWidget* widget=widgetList.takeFirst()->widget;
        if (!widget) return;
        QGridLayout::removeWidget(widget);
        widget->hide();
        widgetCount--;
        originalGeometries.remove(widget);

        delete widget;
    }
    widgetCount = 0;
    zoomedWidgetPtr=nullptr;
    update();
}

void TGridLayout::updateColumnCount() {
    if (!parentWidget()) return;
    int width = parentWidget()->width();
    columnCountVar = qMax(1, width / m_baseWidth);
}

void TGridLayout::rearrangeWidgets() {
    if (isZoomed()) return;

    currentRow = 0;
    currentCol = 0;
    updateColumnCount();

    for (auto &wrapper : widgetList) {
        QWidget *widget = wrapper->widget;
        QGridLayout::addWidget(widget, currentRow, currentCol);
        wrapper->row=currentRow;
        wrapper->column=currentCol;

        originalGeometries[widget] = widget->geometry();

        currentCol++;
        if (currentCol >= columnCountVar) {
            currentCol = 0;
            currentRow++;
        }
    }
}

void TGridLayout::setBaseWidth(short nw)
{
    m_baseWidth=nw;
}

short TGridLayout::baseWidth() const
{
    return m_baseWidth;
}

/*void TGridLayout::zoomTo(QWidget *widget) {
    if (!widget || isZoomed()) return;

    rearrangeWidgets();

    zoomedWidgetPtr = widget;
    zoomedWidgetPtr->raise();

    QParallelAnimationGroup *animGroup = new QParallelAnimationGroup(this);

    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect fullSize = widget->parentWidget()->rect().adjusted(left, top, -right, -bottom);
    QEasingCurve leasing(QEasingCurve::OutQuad);
    easing.setAmplitude(0.7);
    easing.setPeriod(1.0);
    QPoint zoomedCenter = widget->geometry().center();

    for (auto &wrapper : widgetList) {
        QWidget *w = wrapper->widget;
        if (w == widget) continue;

        QRect widgetRect = w->geometry();
        QPoint widgetCenter = widgetRect.center();
        QPointF direction = QPointF(widgetCenter - zoomedCenter);

        if (qFuzzyIsNull(direction.manhattanLength())) {
            direction = QPointF(1.0, 1.0);
        }

        double length = std::hypot(direction.x(), direction.y());
        direction /= length;

        // Calculate minimal push distance until the entire widget is outside the zoom rect
        QPointF probe = QPointF(widgetCenter);
        int buffer = 20;
        QRect zoomedFinal = fullSize;

        while (zoomedFinal.intersects(widgetRect)) {
            probe += direction * 5.0;
            widgetRect.moveCenter(probe.toPoint());
        }

        // Adjust for vertical and horizontal spacing
        int horizontalSpacing = this->horizontalSpacing();
        int verticalSpacing = this->verticalSpacing();
        QPointF newCenterF = probe + direction * (buffer + horizontalSpacing + verticalSpacing);
        QPoint newCenter = newCenterF.toPoint();
        QRect newGeometry(newCenter - QPoint(w->width() / 2, w->height() / 2), w->size());

        QPropertyAnimation *anim = new QPropertyAnimation(w, "geometry");
        anim->setEndValue(newGeometry);
        anim->setDuration(animationDurationMs);
        anim->setEasingCurve(leasing);
        animGroup->addAnimation(anim);
    }

    QPropertyAnimation *zoomAnim = new QPropertyAnimation(widget, "geometry");
    zoomAnim->setEndValue(fullSize);
    zoomAnim->setDuration(animationDurationMs);
    zoomAnim->setEasingCurve(leasing);
    animGroup->addAnimation(zoomAnim);

    connect(animGroup, &QParallelAnimationGroup::finished, this, [this, fullSize] {
        zoomedWidgetPtr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        for (auto &wrapper : widgetList) {
            if (wrapper->widget != zoomedWidgetPtr) {
                wrapper->widget->hide();
            }
        }
        zoomedWidgetPtr->setGeometry(fullSize);
        zoomedWidgetPtr->updateGeometry();
        update();
    });

    animGroup->start(QAbstractAnimation::DeleteWhenStopped);
}*/

/*void TGridLayout::showAll() {
    if (!zoomedWidgetPtr) return;

    QParallelAnimationGroup *animGroup = new QParallelAnimationGroup(this);
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect fullSize = parentWidget()->rect().adjusted(left, top, -right, -bottom);
    QPoint zoomedCenter = fullSize.center();


    QPropertyAnimation *anim = new QPropertyAnimation(zoomedWidgetPtr, "geometry");
    anim->setStartValue(fullSize);
    anim->setEndValue(originalGeometries[zoomedWidgetPtr]);
    anim->setDuration(animationDurationMs);
    anim->setEasingCurve(QEasingCurve::InQuad);
    animGroup->addAnimation(anim);

    for (auto &wrapper : widgetList) {
        if(wrapper->widget == zoomedWidgetPtr) continue;

        wrapper->widget->show();
        QPoint widgetCenter = wrapper->widget->geometry().center();
        QPoint direction = widgetCenter - zoomedCenter;

        if (direction.manhattanLength() == 0) {
            direction = QPoint(1, 1);
        }
        direction = direction / std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

        int pushDistance = parentWidget()->width()/1.5 + horizontalSpacing()+left+right;
        QPoint newCenter = widgetCenter + direction * pushDistance;
        QRect newGeometry = QRect(newCenter - QPoint(wrapper->widget->width() / 2, wrapper->widget->height() / 2),
                                  wrapper->widget->size());

        QPropertyAnimation *anim = new QPropertyAnimation(wrapper->widget, "geometry");
        anim->setStartValue(newGeometry);
        anim->setEndValue(originalGeometries[wrapper->widget]);
        anim->setDuration(animationDurationMs);
        anim->setEasingCurve(easing);
        animGroup->addAnimation(anim);
    }

    connect(animGroup, &QParallelAnimationGroup::finished, this, [this] {
        zoomedWidgetPtr->setGeometry(originalGeometries[zoomedWidgetPtr]);
        auto zoomedWrapper = std::find_if(widgetList.begin(), widgetList.end(), [this](WidgetWrapper* elt) {
            return elt->widget == zoomedWidgetPtr;
        });
        if (zoomedWrapper != widgetList.end()) {
            zoomedWidgetPtr->setSizePolicy((*zoomedWrapper)->originalSizePolicy);
        }

        zoomedWidgetPtr = nullptr;
        rearrangeWidgets();
        // Restore original size policy of the zoomed widget
    });

    animGroup->start(QAbstractAnimation::DeleteWhenStopped);
}*/

void TGridLayout::showAll() {
    if (!zoomedWidgetPtr) return;

    QParallelAnimationGroup *animGroup = new QParallelAnimationGroup(this);
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect fullSize = parentWidget()->rect().adjusted(left, top, -right, -bottom);

    // Find the grid position of the zoomed widget
    int zoomRow = -1, zoomCol = -1;
    for (int i = 0; i < widgetList.size(); ++i) {
        if (widgetList[i]->widget == zoomedWidgetPtr) {
            zoomRow = widgetList[i]->row;
            zoomCol = widgetList[i]->column;
            break;
        }
    }
    QEasingCurve _easing(QEasingCurve::InBounce);
    _easing.setAmplitude(0.7);
    _easing.setPeriod(1.0);

    // Animate the zoomed widget back to its original position
    QPropertyAnimation *zoomAnim = new QPropertyAnimation(zoomedWidgetPtr, "geometry");
    zoomAnim->setStartValue(fullSize);
    zoomAnim->setEndValue(originalGeometries[zoomedWidgetPtr]);
    zoomAnim->setDuration(animationDurationMs);
    zoomAnim->setEasingCurve(easing);
    animGroup->addAnimation(zoomAnim);

    // Animate other widgets back from their pushed positions
    for (auto &wrapper : widgetList) {

        QWidget *w = wrapper->widget;
        if (w == zoomedWidgetPtr) continue;
        w->show();
        int widgetRow = wrapper->row;
        int widgetCol = wrapper->column;
        QRect widgetRect = originalGeometries[w];
        QSize widgetSize = widgetRect.size();

        QPoint newTopLeft;

        if (widgetRow < zoomRow) {
            // Push Up
            int distance = zoomRow - widgetRow;
            int offset = fullSize.top() - (widgetSize.height() + verticalSpacing()) * distance;
            newTopLeft = QPoint(widgetRect.left(), offset - top);

        } else if (widgetRow > zoomRow) {
            // Push Down (Fixed spacing)
            int distance = widgetRow - zoomRow;
            int offset = fullSize.bottom() + verticalSpacing() + (widgetSize.height() + verticalSpacing()) * (distance - 1);
            newTopLeft = QPoint(widgetRect.left(), offset+bottom);

        } else if (widgetCol < zoomCol) {
            // Push Left
            int distance = zoomCol - widgetCol;
            int offset = fullSize.left() - (widgetSize.width() + horizontalSpacing()) * distance;
            newTopLeft = QPoint(offset - left, widgetRect.top());

        } else if (widgetCol > zoomCol) {
            // Push Right (Fixed overlap)
            int distance = widgetCol - zoomCol;
            int offset = fullSize.right() + horizontalSpacing() + (widgetSize.width() + horizontalSpacing()) * (distance - 1);
            newTopLeft = QPoint(offset+right, widgetRect.top());
        }

        QRect startGeometry(newTopLeft, widgetSize);
        QPropertyAnimation *anim = new QPropertyAnimation(wrapper->widget, "geometry");
        anim->setStartValue(startGeometry);
        anim->setEndValue(originalGeometries[wrapper->widget]);
        anim->setDuration(animationDurationMs);
        anim->setEasingCurve(easing);
        animGroup->addAnimation(anim);
    }

    connect(animGroup, &QParallelAnimationGroup::finished, this, [this] {
        zoomedWidgetPtr->setGeometry(originalGeometries[zoomedWidgetPtr]);
        auto zoomedWrapper = std::find_if(widgetList.begin(), widgetList.end(), [this](WidgetWrapper* elt) {
            return elt->widget == zoomedWidgetPtr;
        });
        if (zoomedWrapper != widgetList.end()) {
            zoomedWidgetPtr->setSizePolicy((*zoomedWrapper)->originalSizePolicy);
        }

        zoomedWidgetPtr = nullptr;
        rearrangeWidgets();
    });

    animGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void TGridLayout::zoomTo(QWidget *widget) {
    if (!widget || isZoomed()) return;

    rearrangeWidgets();
    zoomedWidgetPtr = widget;
    zoomedWidgetPtr->raise();

    QParallelAnimationGroup *animGroup = new QParallelAnimationGroup(this);

    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect fullSize = widget->parentWidget()->rect().adjusted(left, top, -right, -bottom);

    QEasingCurve _easing(QEasingCurve::OutBounce);
    _easing.setAmplitude(0.7);
    _easing.setPeriod(1.0);

    int zoomRow = -1, zoomCol = -1;
    for (const auto &wrapper : std::as_const(widgetList)) {
        if (wrapper->widget == widget) {
            zoomRow = wrapper->row;
            zoomCol = wrapper->column;
            break;
        }
    }

    for (const auto &wrapper : std::as_const(widgetList)) {
        QWidget *w = wrapper->widget;
        if (w == widget) continue;

        int widgetRow = wrapper->row;
        int widgetCol = wrapper->column;
        QRect widgetRect = w->geometry();
        QSize widgetSize = widgetRect.size();

        QPoint newTopLeft;

        if (widgetRow < zoomRow) {
            // Push Up
            int distance = zoomRow - widgetRow;
            int offset = fullSize.top() - (widgetSize.height() + verticalSpacing()) * distance;
            newTopLeft = QPoint(widgetRect.left(), offset - top);

        } else if (widgetRow > zoomRow) {
            // Push Down (Fixed spacing)
            int distance = widgetRow - zoomRow;
            int offset = fullSize.bottom() + verticalSpacing() + (widgetSize.height() + verticalSpacing()) * (distance - 1);
            newTopLeft = QPoint(widgetRect.left(), offset+bottom);

        } else if (widgetCol < zoomCol) {
            // Push Left
            int distance = zoomCol - widgetCol;
            int offset = fullSize.left() - (widgetSize.width() + horizontalSpacing()) * distance;
            newTopLeft = QPoint(offset - left, widgetRect.top());

        } else if (widgetCol > zoomCol) {
            // Push Right (Fixed overlap)
            int distance = widgetCol - zoomCol;
            int offset = fullSize.right() + horizontalSpacing() + (widgetSize.width() + horizontalSpacing()) * (distance - 1);
            newTopLeft = QPoint(offset+right, widgetRect.top());
        }

        QRect newGeometry(newTopLeft, widgetSize);

        QPropertyAnimation *anim = new QPropertyAnimation(w, "geometry");
        anim->setEndValue(newGeometry);
        anim->setDuration(animationDurationMs);
        anim->setEasingCurve(easing);
        animGroup->addAnimation(anim);
    }

    QPropertyAnimation *zoomAnim = new QPropertyAnimation(widget, "geometry");
    zoomAnim->setEndValue(fullSize);
    zoomAnim->setDuration(animationDurationMs);
    zoomAnim->setEasingCurve(easing);
    animGroup->addAnimation(zoomAnim);

    connect(animGroup, &QParallelAnimationGroup::finished, this, [this, fullSize] {
        zoomedWidgetPtr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        for (auto wrapper : std::as_const(widgetList)) {
            if (wrapper->widget != zoomedWidgetPtr) {
                wrapper->widget->hide();
            }
        }
        zoomedWidgetPtr->setGeometry(fullSize);
        zoomedWidgetPtr->updateGeometry();
        update();
    });

    animGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void TGridLayout::setAnimationDuration(int duration) {
    animationDurationMs = duration;
}

int TGridLayout::animationDuration() const {
    return animationDurationMs;
}

void TGridLayout::setEasingCurve(const QEasingCurve &easingCurve) {
    easing = easingCurve;
}

QEasingCurve TGridLayout::easingCurve() const {
    return easing;
}

bool TGridLayout::isZoomed() const {
    return zoomedWidgetPtr != nullptr;
}

QWidget* TGridLayout::zoomedWidget() const {
    return zoomedWidgetPtr;
}
