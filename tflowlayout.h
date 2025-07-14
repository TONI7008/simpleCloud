#ifndef TFLOWLAYOUT_H
#define TFLOWLAYOUT_H

#include <QGridLayout>
#include <QWidget>
#include <QEasingCurve>
#include <QParallelAnimationGroup>


class TFlowLayout : public QGridLayout
{
    Q_OBJECT
public:
    struct cellWrapper{
        short row=-1;
        short column=-1;
        short rowSpan=1;
        short columnSpan=1;

        cellWrapper(short _row,short _column,short _rowSpan,short _columnSpan){
            row=_row;
            column=_column;
            rowSpan=_rowSpan;
            columnSpan=_columnSpan;
        }
    };
    struct WidgetWrapper {
        cellWrapper cell;
        QWidget *widget;
        QSizePolicy originalSizePolicy;
        bool isValid() const{
            return cell.row >= 0 && cell.column>=0 && cell.rowSpan>0 && cell.columnSpan > 0;
        }
        WidgetWrapper(cellWrapper _cell,QWidget *w, QSizePolicy policy)
            : cell(_cell),widget(w), originalSizePolicy(policy){}
    };

    TFlowLayout(QWidget* parent=nullptr);
    ~TFlowLayout();
    void addWidget(QWidget *widget,short _row=-1,short _column=-1,short _rowSpan=1,short columnSpan=1, Qt::Alignment align=Qt::Alignment());
    bool isZoomed() const;
    void removeWidget(QWidget*);

    void setEasingCurve(const QEasingCurve &easingCurve);
    QEasingCurve easingCurve() const;
    void setAnimationDuration(int duration);
    int animationDuration() const;
    void showAll();
    void zoomTo(QWidget *widget);

    QWidget* zoomedWidget();

    bool isAnimating() const;

private:

    QMap<QWidget*, QRect> originalGeometries;

    QList<WidgetWrapper> widgetList;

    QEasingCurve m_easing;
    QWidget* zoomedWidgetPtr;
    short m_duration;
    bool m_isAnimating=false;
    QRect m_parentRectAtRepopulate;  // Store parent rect when geometries were captured

    void updateOriginalGeometriesForResize();
    cellWrapper getWidgetPosition(QWidget *widget) const;
    void repopulate();
};



#endif // TFLOWLAYOUT_H
