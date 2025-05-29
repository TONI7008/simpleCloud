#ifndef TLABEL_H
#define TLABEL_H

#include <QLabel>
#include <QTimer>
#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include "elidedlabel.h"

class TLabel : public ElidedLabel {
    Q_OBJECT
public:
    explicit TLabel(QWidget* parent = nullptr);
    explicit TLabel(QString content,QWidget* parent = nullptr);
    ~TLabel();

    void click();

signals:
    void Clicked(const QPoint& pos);
    void rightClicked(const QPoint& pos);
    void doubleClicked(const QPoint& pos);
    void ctrlSelected();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    QTimer *timer=nullptr;
    bool isDoubleClick=false;
    QPoint m_clickPos;
};

#endif // TLABEL_H
