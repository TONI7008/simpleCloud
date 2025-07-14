#ifndef MARGINDETECTINGWIDGET_H
#define MARGINDETECTINGWIDGET_H

#include <QWidget>
#include <QSet>
#include <QPointer>

class MarginDetectingWidget : public QWidget
{
    Q_OBJECT
public:
    enum MarginDirection {
        Left,
        Top,
        Right,
        Bottom,
        AllSides
    };
    Q_ENUM(MarginDirection)

    explicit MarginDetectingWidget(QWidget *parent = nullptr);
    ~MarginDetectingWidget();

    void setMarginWidth(int width);
    void setMarginDirection(MarginDirection direction);
    void setDetectionEnabled(bool enabled);
    void setRecursiveDetection(bool recursive);

signals:
    void mouseEnteredMarginArea(const QPoint& pos);
    void mouseMovedInMarginArea(const QPoint& pos);
    void mouseLeftMarginArea();

protected:
    bool event(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    int m_marginWidth = 10;
    MarginDirection m_direction = AllSides;
    bool m_detectionEnabled = true;
    bool m_recursiveDetection = true;
    bool m_mouseInMargin = false;
    QSet<QPointer<QObject>> m_trackedObjects;

    void setupChild(QObject *child);
    void teardownChild(QObject *child);
    void refreshEventFilters();
    bool isInMarginArea(const QPoint& pos) const;
    void checkMarginArea(const QPoint &pos);
    QPoint mapToThis(const QObject *watched, const QPoint &pos) const;
};

// Proper qHash declaration for QPointer<QObject>
namespace std {
template<> struct hash<QPointer<QObject>> {
    size_t operator()(const QPointer<QObject> &p) const noexcept {
        return qHash(p.data());
    }
};
}

#endif // MARGINDETECTINGWIDGET_H
