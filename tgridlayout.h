#ifndef TGRIDLAYOUT_H
#define TGRIDLAYOUT_H

#include <QGridLayout>
#include <QObject>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <QMap>
#include <QPropertyAnimation>
#include <QList>

class TGridLayout : public QGridLayout {
    Q_OBJECT

public:
    explicit TGridLayout(QWidget *parent = nullptr);
    ~TGridLayout();

    void addWidget(QWidget *widget);
    void addWidgetList(QList<QWidget*> widgetL);
    void removeWidget(QWidget *widget);
    void removeAll();
    void zoomTo(QWidget *widget);
    void showAll();
    void setAnimationDuration(int duration);
    int animationDuration() const;
    void setEasingCurve(const QEasingCurve &easing);
    QEasingCurve easingCurve() const;
    bool isZoomed() const;
    QWidget* zoomedWidget() const;
    void updateColumnCount();
    void rearrangeWidgets();
    void setBaseWidth(short);
    short baseWidth() const;


    void removeWidgetAnimated(QWidget *widget);
private:
    struct WidgetWrapper {
        short row, column;
        QWidget *widget;
        QSizePolicy originalSizePolicy;
        WidgetWrapper(short _row, short _column, QWidget *w, QSizePolicy policy)
            : row(_row), column(_column), widget(w), originalSizePolicy(policy) {}

        bool operator==(const WidgetWrapper& other) const {
            return (row == other.row &&
                    column == other.column &&
                    widget == other.widget);
        }
    };

    QList<WidgetWrapper*> widgetList;
    int currentRow;
    int currentCol;
    int columnCountVar;
    int widgetCount;
    int animationDurationMs;
    int m_baseWidth;

    QEasingCurve easing;
    QParallelAnimationGroup *animationGroup;
    QWidget *zoomedWidgetPtr;
    QMap<QWidget*, QRect> originalGeometries;
    QMap<QWidget*, QRect> targetGeometries;

};

#endif // TGRIDLAYOUT_H
