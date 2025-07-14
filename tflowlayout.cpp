#include "tflowlayout.h"
#include <QPropertyAnimation>
#include <QTimer>
#include <QApplication>


#include "tflowlayout.h"
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

TFlowLayout::TFlowLayout(QWidget *parent) :
    QGridLayout(parent),
    m_easing(QEasingCurve::OutQuad),
    zoomedWidgetPtr(nullptr),
    m_duration(300)

{

}

TFlowLayout::~TFlowLayout()
{

}

void TFlowLayout::addWidget(QWidget *widget, short _row, short _column, short _rowSpan, short columnSpan, Qt::Alignment align)
{
    if (!widget) return;

    //updateColumnCount();
    WidgetWrapper wrapper=WidgetWrapper(cellWrapper(_row,_column,_rowSpan,columnSpan),widget, widget->sizePolicy());
    widgetList.append(wrapper);

    if(wrapper.isValid()){
        QGridLayout::addWidget(widget,_row,_column,_rowSpan,columnSpan,align);
    }else{
        QGridLayout::addWidget(widget);
    }


    widget->show();
    if (isZoomed()) widget->hide();
    update();


    originalGeometries[widget] = widget->rect();
}

bool TFlowLayout::isZoomed() const
{
    return zoomedWidgetPtr!=nullptr;
}

void TFlowLayout::removeWidget(QWidget *widget)
{
    if(!widget) return;
    QGridLayout::removeWidget(widget);
    originalGeometries.remove(widget);
    auto it = std::find_if(widgetList.begin(), widgetList.end(), [widget](WidgetWrapper elt) {
        return elt.widget == widget;
    });

    if (it != widgetList.end()) {
        widgetList.erase(it);
    }
    //widget->deleteLater();
}

void TFlowLayout::showAll() {
    if (!zoomedWidgetPtr) return;
    //repopulate();
    m_isAnimating=true;

    updateOriginalGeometriesForResize();

    QParallelAnimationGroup *animationGroup = new QParallelAnimationGroup(this);
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect fullSize = parentWidget()->rect().adjusted(left, top, -right, -bottom);

    // Find the grid position of the zoomed widget
    int zoomRow = -1, zoomCol = -1;
    for (int i = 0; i < widgetList.size(); ++i) {
        if (widgetList[i].widget == zoomedWidgetPtr) {
            if(!widgetList[i].isValid()){
                widgetList[i].cell=getWidgetPosition(zoomedWidgetPtr);
            }
            zoomRow = widgetList[i].cell.row;
            zoomCol = widgetList[i].cell.column;
            break;
        }
    }
    //QEasingCurve _easing(QEasingCurve::InBounce);
    //_easing.setAmplitude(0.7);
    //_easing.setPeriod(1.0);

    // Animate the zoomed widget back to its original position
    QPropertyAnimation *zoomAnim = new QPropertyAnimation(zoomedWidgetPtr, "geometry");
    zoomAnim->setStartValue(fullSize);
    zoomAnim->setEndValue(originalGeometries[zoomedWidgetPtr]);
    zoomAnim->setDuration(m_duration);
    zoomAnim->setEasingCurve(m_easing);
    animationGroup->addAnimation(zoomAnim);

    // Animate other widgets back from their pushed positions
    for (auto &wrapper : widgetList) {

        QWidget *w = wrapper.widget;
        if (w == zoomedWidgetPtr) continue;
        w->show();
        if(!wrapper.isValid()){
            wrapper.cell=getWidgetPosition(w);
        }
        int widgetRow = wrapper.cell.row;
        int widgetCol = wrapper.cell.column;
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
        QPropertyAnimation *anim = new QPropertyAnimation(wrapper.widget, "geometry");
        anim->setStartValue(startGeometry);
        anim->setEndValue(originalGeometries[wrapper.widget]);
        anim->setDuration(m_duration);
        anim->setEasingCurve(m_easing);
        animationGroup->addAnimation(anim);
    }

    connect(animationGroup, &QParallelAnimationGroup::finished, this, [this] {
        zoomedWidgetPtr->setGeometry(originalGeometries[zoomedWidgetPtr]);
        auto zoomedWrapper = std::find_if(widgetList.begin(), widgetList.end(), [this](WidgetWrapper elt) {
            return elt.widget == zoomedWidgetPtr;
        });
        if (zoomedWrapper != widgetList.end()) {
            zoomedWidgetPtr->setSizePolicy((*zoomedWrapper).originalSizePolicy);
        }

        zoomedWidgetPtr = nullptr;
        m_isAnimating=false;
        activate();
        update();
    });

    animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void TFlowLayout::zoomTo(QWidget *widget) {
    if (!widget || isZoomed()) return;
    repopulate();

    zoomedWidgetPtr = widget;
    zoomedWidgetPtr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    zoomedWidgetPtr->raise();
    m_isAnimating=true;

    QParallelAnimationGroup* animationGroup = new QParallelAnimationGroup(this);

    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect fullSize = widget->parentWidget()->rect().adjusted(left, top, -right, -bottom);

    //QEasingCurve _easing(QEasingCurve::OutBounce);
    //_easing.setAmplitude(0.7);
    //_easing.setPeriod(1.0);

    int zoomRow = -1, zoomCol = -1;
    for (auto &wrapper : widgetList) {
        if (wrapper.widget == widget) {
            if(!wrapper.isValid()){
                wrapper.cell=getWidgetPosition(wrapper.widget);
            }
            zoomRow = wrapper.cell.row;
            zoomCol = wrapper.cell.column;
            break;
        }
    }

    //QGridLayout::addWidget(zoomedWidgetPtr,zoomRow,zoomCol);

    for (auto &wrapper : widgetList) {
        QWidget *w = wrapper.widget;
        if (w == widget) continue;
        if(!wrapper.isValid()){
            wrapper.cell=getWidgetPosition(w);
        }
        int widgetRow = wrapper.cell.row;
        int widgetCol = wrapper.cell.column;
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
        anim->setDuration(m_duration);
        anim->setEasingCurve(m_easing);
        animationGroup->addAnimation(anim);
    }

    QPropertyAnimation *zoomAnim = new QPropertyAnimation(widget, "geometry");
    zoomAnim->setEndValue(fullSize);
    zoomAnim->setDuration(m_duration);
    zoomAnim->setEasingCurve(m_easing);
    animationGroup->addAnimation(zoomAnim);

    connect(animationGroup, &QParallelAnimationGroup::finished, this, [this, fullSize] {
        for (auto wrapper : std::as_const(widgetList)) {
            if (wrapper.widget != zoomedWidgetPtr) {
                wrapper.widget->hide();
            }
        }
        zoomedWidgetPtr->setGeometry(fullSize);
        zoomedWidgetPtr->updateGeometry();
        m_isAnimating=false;
        update();
    });

    animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
}


QWidget *TFlowLayout::zoomedWidget()
{
    return zoomedWidgetPtr;
}


bool TFlowLayout::isAnimating() const
{
    return m_isAnimating;
}

TFlowLayout::cellWrapper TFlowLayout::getWidgetPosition(QWidget *widget) const
{
    int row,column,rowSpan,columnSpan;
    int index = indexOf(widget);
    if (index != -1) {
        getItemPosition(index, &row, &column, &rowSpan, &columnSpan);
    } else {
        row = column = rowSpan = columnSpan = -1; // Widget not found
    }
    return cellWrapper(row,column,rowSpan,columnSpan);
}

void TFlowLayout::setAnimationDuration(int duration) {
    m_duration = duration;
}

int TFlowLayout::animationDuration() const {
    return m_duration;
}

void TFlowLayout::setEasingCurve(const QEasingCurve &easingCurve) {
    m_easing = easingCurve;
}

QEasingCurve TFlowLayout::easingCurve() const {
    return m_easing;
}

void TFlowLayout::repopulate()
{
    if (m_isAnimating) return;

    originalGeometries.clear();

    if (parentWidget()) {
        m_parentRectAtRepopulate = this->geometry();  // Use layout area, not widget area
    }

    for (auto &wrapper : widgetList) {
        if (isZoomed() && wrapper.widget == zoomedWidgetPtr) continue;
        QRect geo = wrapper.widget->geometry();
        originalGeometries[wrapper.widget] = geo;
    }
}

void TFlowLayout::updateOriginalGeometriesForResize()
{
    if (originalGeometries.isEmpty() || !parentWidget()) return;
    QRect currentParentRect = parentWidget()->rect();

    // Check if parent widget was actually resized
    if (currentParentRect == m_parentRectAtRepopulate) {
        return; // No resize occurred
    }

    // Calculate scale factors
    double scaleX = currentParentRect.width() > 0 ?
                        static_cast<double>(currentParentRect.width()) / m_parentRectAtRepopulate.width() : 1.0;
    double scaleY = currentParentRect.height() > 0 ?
                        static_cast<double>(currentParentRect.height()) / m_parentRectAtRepopulate.height() : 1.0;

    // Calculate offset differences
    int offsetX = currentParentRect.x() - m_parentRectAtRepopulate.x();
    int offsetY = currentParentRect.y() - m_parentRectAtRepopulate.y();



    // Update all originalGeometries based on the scale and offset
    for (auto it = originalGeometries.begin(); it != originalGeometries.end(); ++it) {
        QRect oldRect = it.value();

        // Scale the geometry
        int newX = static_cast<int>(oldRect.x() * scaleX) + offsetX;
        int newY = static_cast<int>(oldRect.y() * scaleY) + offsetY;
        int newWidth = static_cast<int>(oldRect.width() * scaleX);
        int newHeight = static_cast<int>(oldRect.height() * scaleY);

        QRect newRect(newX, newY, newWidth, newHeight);
        it.value() = newRect;
    }

    // Update the stored parent rect
    m_parentRectAtRepopulate = currentParentRect;
}
