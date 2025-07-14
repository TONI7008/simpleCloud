#ifndef TWIDGET_H
#define TWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QFile>
#include <QShowEvent>

#include <QWidget>
#include <QMouseEvent>
#include <QApplication>
#include "margindetectingwidget.h"

class TWidget : public MarginDetectingWidget
{
    Q_OBJECT
public:
    enum class CornerStyle {
        Default,    // Rounded on all corners
        TopOnly,    // Rounded only on the top
        BottomOnly, // Rounded only on the bottom
        LeftOnly,   // Rounded only on the left
        RightOnly,  // Rounded only on the right
        BottomLeft, // Rounded only on the bottom left
        BottomRight,// Rounded only on the bottom right
        None        // No rounded corners
    };

    explicit TWidget(QWidget *parent = nullptr);
    ~TWidget();

    void setEnableBackground(bool enable);
    void setBackgroundImage(const QString& image);
    void setBackgroundImage(const QPixmap& pix);
    void setBackgroundColor(const QColor& color);
    void setBorder(bool enable);
    void setBorderSize(short size);
    void setBorderColor(const QColor& color);
    void setCornerStyle(CornerStyle style);

    void setBorderRadius(short radius);
    short borderRadius() const;
    bool borderEnabled() const;
    QColor borderColor() const;
    CornerStyle cornerStyle() const;

    void pressNext();
    void pressPrev();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent* event) override;
    void hideEvent(QHideEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void Clicked(const QPoint& pos);
    void rightClicked(const QPoint& pos);
    void doubleClicked(const QPoint& pos);
    void ctrlSelected();
    void resizing();
    void isHiden();
    void isMoving();
    void isShown();
    void previousPressed();
    void nextPressed();
    void focusIn();
    void focusOut();
    void debugPressed();

private:
    QTimer* timer;
    bool isDoubleClick = false;
    bool enableBackground = false;
    bool enableBorder = false;
    short borderSize = 0;
    QColor borderColorValue = QColor(71, 158, 245); // Default border color
    QPoint m_clickPos;
    short m_bRadius = 0;
    QPixmap m_backgroundImage;
    CornerStyle m_cornerStyle = CornerStyle::Default;
};

#endif // TWIDGET_H
