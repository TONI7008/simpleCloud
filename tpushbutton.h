#ifndef TPUSHBUTTON_H
#define TPUSHBUTTON_H

#include <QPushButton>
#include <QTimer>
#include <QMouseEvent>

class TPushButton : public QPushButton
{
    Q_OBJECT
public:
    TPushButton(QWidget* parent=nullptr);
    ~TPushButton();
    void Click();;

signals:
    void doubleClicked(const QPoint& pos);
    void rightClicked(const QPoint& pos);
    void ctrlSelected();
    void Clicked(const QPoint& pos);

protected:
    void mousePressEvent(QMouseEvent* event) override ;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    bool isDoubleClick=false;
    QTimer *timer;
    QPoint m_clickPos;
};

#endif // TPUSHBUTTON_H
