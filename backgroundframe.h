#ifndef BACKGROUNDFRAME_H
#define BACKGROUNDFRAME_H

#include "tframe.h"
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainterPath>
#include <QPainter>
#include <QPixmap>
#include <QToolButton>

class BackgroundFrame : public TFrame {
    Q_OBJECT
public:
    explicit BackgroundFrame(QWidget* parent = nullptr);
    ~BackgroundFrame() override;

    void setSelected(bool s);
    bool isSelected() const { return selected; }
    void manage();
    void setText(const QString& text);
    void setSimpleType(bool simple);  // Renamed for consistency
    void setPixmap(const QPixmap& pixmap);
    QString text() const;

    static QList<BackgroundFrame*> instances();
    static QList<BackgroundFrame*> m_list;

signals:
    void kill();

private:
    bool selected = false;
    QString m_text;
    bool m_simpleType = false;
    QToolButton* deleteButton;
    QPixmap m_backgroundImage;

    void updateAppearance();
    void setupDeleteButton();

    const QString defaultStyle = R"(
        QToolButton {
            border-radius: 5px;
            background: rgba(30,30,30,0.4);
        }
        QToolButton:hover {
            border-radius: 5px;
            background: rgba(30,30,30,0.7);
        }
    )";
};

#endif // BACKGROUNDFRAME_H
