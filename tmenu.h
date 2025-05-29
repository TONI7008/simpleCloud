#ifndef TMENU_H
#define TMENU_H

#include <QMenu>
#include <QObject>
#include <QTimer>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QMouseEvent>


class TMenu : public QMenu
{
    Q_OBJECT
public:

    TMenu(QWidget *parent = nullptr);
    TMenu(QString title,QWidget *parent = nullptr);
    TMenu(QIcon icon,QString title,QWidget *parent = nullptr);
    ~TMenu();

    TMenu* addMenu(const QString &title);
    TMenu* addMenu(const QIcon &icon, const QString &title);

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

    void setBorder(bool enable);
    void setBorderSize(short size);
    void setBorderColor(const QColor& color);
    void setCornerStyle(CornerStyle style);

    void setBorderRadius(short radius);
    short borderRadius() const;
    bool borderEnabled() const;
    QColor borderColor() const;
    CornerStyle cornerStyle() const;

    const static QString menuStyle;

    TMenu *addMenu(TMenu *menu);
protected:
    void paintEvent(QPaintEvent* event) override;




private:
    bool isDoubleClick = false;
    bool enableBackground = false;
    bool enableBorder = false;
    short borderSize = 0;
    QColor borderColorValue = QColor(71, 158, 245); // Default border color
    QPoint m_clickPos;
    short m_bRadius = 0;
    CornerStyle m_cornerStyle = CornerStyle::Default;
    TMenu *createMenu(const QString &title, const QIcon &icon);
};


#endif // TMENU_H
