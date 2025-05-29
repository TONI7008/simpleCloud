#ifndef HOVERBUTTON_H
#define HOVERBUTTON_H

#include <QPushButton>
#include <QHoverEvent>
#include <QToolButton>
#include <QIcon>
#include <QMap>

class State {
public:
    bool isNormal() const { return normal; }
    bool isSelected() const { return !normal; }
    void setNormal() { normal = true; }
    void setSelected() { normal = false; }
    State() : normal(true) {}

private:
    bool normal;
};

class HoverButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(QColor color READ color)

public:
    HoverButton(QWidget *parent = nullptr);
    QColor color() const { return m_color; }

    // Set a fixed icon size for all buttons
    void setGlobalIconSize(const QSize &size) { m_globalIconSize = size; }

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
    QSize m_globalIconSize = QSize(32, 32); // Default size
    QStringList m_exceptions = {"profile", "accueil", "parametre", "securite", "performance", "contact"};
    QMap<QString, QPair<QIcon, QIcon>> m_iconMap; // Simplified map
    State m_state;

    void handleClicked();
    void handleHoverEvent(QHoverEvent* event);
    void hoverState();
    void leaveState();
    void animateColor(const QColor &startValue, const QColor &endValue);
};

#endif
