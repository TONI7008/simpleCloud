#ifndef HFRAME_H
#define HFRAME_H

#include <QFrame>
#include <QEnterEvent>
#include <QEvent>

#include "tframe.h"

class HFrame : public TFrame
{
    Q_OBJECT

public:
    explicit HFrame(QWidget* parent = nullptr);

signals:
    void hoverStarted();
    void hoverEnded();

protected:
    void enterEvent(QEnterEvent* event) override;

    void leaveEvent(QEvent* event) override;
};

#endif // HFRAME_H
