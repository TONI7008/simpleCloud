#ifndef TFRAME_H
#define TFRAME_H

#include <QFrame>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QMouseEvent>

class TFrame : public QFrame
{
    Q_OBJECT
public:
    enum class CornerStyle {
        Default,// Rounded on all corners
        TopOnly,     // Rounded only on the top
        BottomOnly,  // Rounded only on the bottom
        LeftOnly,    // Rounded only on the left
        RightOnly,   // Rounded only on the right
        BottomLeft, // rounded only on the bottom left
        BottomRight,    // rounded only on the bottom right
        None         // No rounded corners
    };

    explicit TFrame(QWidget* parent=nullptr);
    ~TFrame();

    void setEnabledBorder(bool);
    void setBackgroundImage(const QString&);
    void setBackgroundImage(const QPixmap&);
    void setBackgroundColor(const QColor &color);

    void setRoundness(short int);
    void setEnableBackground(bool b);
    void setCornerStyle(CornerStyle style);
    void setBorder(bool);
    void setBorderSize(short size);
    void setBorderColor(const QColor& color); // New method to set border color
    QColor borderColor() const; // New method to get border color

    void setClickInterval(short newClickInterval);

    short getClickInterval() const;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void moveEvent(QMoveEvent *event) override;

signals:
    void Clicked(const QPoint& pos);
    void rightClicked(const QPoint& pos);
    void doubleClicked(const QPoint& pos);
    void ctrlSelected();
    void resizing();
    void isShown();
    void isHiden();
    void isMoving();

private:
    QPixmap m_backgroundImage = QPixmap(":/pictures/image.png");
    QTimer* timer;
    bool isDoubleClick = false;
    QPoint m_clickPos;
    short int roundness = 15;
    bool enable_background = true;
    bool enable_border = false;
    short int borderSize = 0;
    QColor borderColorValue = QColor(71, 158, 245); // Default border color
    CornerStyle cornerStyle = CornerStyle::Default;

    short clickInterval=250;
};

#endif // TFRAME_H
