#include "margindetectingwidget.h"
#include <QMouseEvent>
#include <QChildEvent>
#include <QDebug>

MarginDetectingWidget::MarginDetectingWidget(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);
    refreshEventFilters();
}

MarginDetectingWidget::~MarginDetectingWidget()
{
    // Disconnect all destroyed signals first
    for (const QPointer<QObject> &obj : m_trackedObjects) {
        if (!obj.isNull()) {
            disconnect(obj, &QObject::destroyed, this, nullptr);
        }
    }
    m_trackedObjects.clear();
}

void MarginDetectingWidget::teardownChild(QObject *child)
{
    if (!child) return;

    child->removeEventFilter(this);
    disconnect(child, &QObject::destroyed, this, nullptr);
    m_trackedObjects.remove(child);

    if (m_recursiveDetection) {
        // Teardown children recursively
        for (QObject *obj : child->children()) {
            if (obj->isWidgetType()) {
                teardownChild(obj);
            }
        }
    }
}


void MarginDetectingWidget::setupChild(QObject *child)
{
    if (!child || m_trackedObjects.contains(child)) {
        return;
    }

    child->installEventFilter(this);
    if (child->isWidgetType()) {
        QWidget *widget = qobject_cast<QWidget*>(child);
        if (widget) {
            widget->setMouseTracking(true);
            widget->setAttribute(Qt::WA_Hover);
        }
    }

    m_trackedObjects.insert(child);

    if (m_recursiveDetection) {
        // Setup existing children recursively
        for (QObject *obj : child->children()) {
            if (obj->isWidgetType()) {
                setupChild(obj);
            }
        }

        // Safe connection using QPointer
        QPointer<MarginDetectingWidget> guard(this);
        connect(child, &QObject::destroyed, this, [guard, this, child]() {
            if (guard && !m_trackedObjects.isEmpty()) {
                m_trackedObjects.remove(child);
            }
        });
    }
}

void MarginDetectingWidget::setMarginWidth(int width)
{
    if (m_marginWidth != width && width >= 0) {
        m_marginWidth = width;
        update();
    }
}

void MarginDetectingWidget::setMarginDirection(MarginDirection direction)
{
    if (m_direction != direction) {
        m_direction = direction;
        update();
    }
}

void MarginDetectingWidget::setDetectionEnabled(bool enabled)
{
    if (m_detectionEnabled != enabled) {
        m_detectionEnabled = enabled;
        if (!enabled && m_mouseInMargin) {
            m_mouseInMargin = false;
            emit mouseLeftMarginArea();
        }
    }
}

void MarginDetectingWidget::setRecursiveDetection(bool recursive)
{
    if (m_recursiveDetection != recursive) {
        m_recursiveDetection = recursive;
        refreshEventFilters();
    }
}

bool MarginDetectingWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ChildAdded || event->type() == QEvent::ChildRemoved) {
        QChildEvent *childEvent = static_cast<QChildEvent*>(event);
        if (childEvent->child()->isWidgetType()) {
            if (event->type() == QEvent::ChildAdded) {
                setupChild(childEvent->child());
            } else {
                teardownChild(childEvent->child());
            }
        }
    }
    return QWidget::event(event);
}

bool MarginDetectingWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (!m_detectionEnabled) {
        return false;
    }

    switch (event->type()) {
    case QEvent::MouseMove:
    case QEvent::HoverMove: {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint pos = mapToThis(watched, mouseEvent->pos());
        checkMarginArea(pos);
        break;
    }
    case QEvent::Leave: {
        if (m_mouseInMargin) {
            m_mouseInMargin = false;
            emit mouseLeftMarginArea();
        }
        break;
    }
    default:
        break;
    }
    return false;
}

void MarginDetectingWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_detectionEnabled) {
        checkMarginArea(event->pos());
    }
    QWidget::mouseMoveEvent(event);
}

void MarginDetectingWidget::leaveEvent(QEvent *event)
{
    if (m_mouseInMargin) {
        m_mouseInMargin = false;
        emit mouseLeftMarginArea();
    }
    QWidget::leaveEvent(event);
}

void MarginDetectingWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void MarginDetectingWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    refreshEventFilters();
}

void MarginDetectingWidget::refreshEventFilters()
{
    // Clear existing tracking
    for (QObject *obj : std::as_const(m_trackedObjects)) {
        obj->removeEventFilter(this);
    }
    m_trackedObjects.clear();

    // Rebuild tracking
    for (QObject *obj : children()) {
        if (obj->isWidgetType()) {
            setupChild(obj);
        }
    }
}

bool MarginDetectingWidget::isInMarginArea(const QPoint& pos) const
{
    const QRect rect = this->rect();
    const int margin = m_marginWidth;

    switch (m_direction) {
    case Left:
        return pos.x() <= margin;
    case Top:
        return pos.y() <= margin;
    case Right:
        return pos.x() >= (rect.width() - margin);
    case Bottom:
        return pos.y() >= (rect.height() - margin);
    case AllSides:
        return pos.x() <= margin || pos.y() <= margin ||
               pos.x() >= (rect.width() - margin) ||
               pos.y() >= (rect.height() - margin);
    default:
        return false;
    }
}

QPoint MarginDetectingWidget::mapToThis(const QObject *watched, const QPoint &pos) const
{
    const QWidget *widget = qobject_cast<const QWidget*>(watched);
    if (!widget) {
        return pos;
    }

    // Map through the widget hierarchy
    QPoint result = pos;
    while (widget && widget != this) {
        result = widget->mapToParent(result);
        widget = widget->parentWidget();
    }
    return result;
}

void MarginDetectingWidget::checkMarginArea(const QPoint& pos)
{
    if (!m_detectionEnabled) return;

    bool currentlyInMargin = isInMarginArea(pos);

    if (currentlyInMargin && !m_mouseInMargin) {
        m_mouseInMargin = true;
        emit mouseEnteredMarginArea(pos);
    } else if (currentlyInMargin && m_mouseInMargin) {
        emit mouseMovedInMarginArea(pos);
    } else if (!currentlyInMargin && m_mouseInMargin) {
        m_mouseInMargin = false;
        emit mouseLeftMarginArea();
    }
}
