#ifndef HOVERBUTTON_H
#define HOVERBUTTON_H

#include <QPushButton>
#include <QHoverEvent>
#include <QIcon>
#include <QMap>

class HoverButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(QColor color READ color)

public:
    HoverButton(QWidget *parent = nullptr);
    QColor color() const { return m_color; }
    void setObjectName(const QString&);

    static QColor defaultColor();
    static void setDefaultColor(const QColor &color);

protected:
    bool event(QEvent *event) override {
        if (event->type() == QEvent::HoverEnter || event->type() == QEvent::HoverLeave) {
            QHoverEvent* hoverEvent = static_cast<QHoverEvent*>(event);
            handleHoverEvent(hoverEvent);
        }
        return QPushButton::event(event);
    }

signals:
    void enterHover();
    void leaveHover();

private:
    QColor m_color;
    QStringList m_exceptions = {"profile", "accueil", "parametre", "securite", "performance", "contact"};

    static QMap<QString, QPair<QIcon, QIcon>> m_iconMap;
    static QColor m_defaultColor;

    void handleClicked();
    void handleHoverEvent(QHoverEvent* event);
    void hoverState();
    void leaveState();
    void animateColor(const QColor &startValue, const QColor &endValue);

    static void updateIconMap();
};

#endif
