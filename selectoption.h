#ifndef SELECTOPTION_H
#define SELECTOPTION_H

#include <QWidget>
#include <QObject>
#include <QPropertyAnimation>
#include <QPainter>

class SelectOption : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int chunkPos READ chunkPos WRITE setChunkPos)
    Q_PROPERTY(int chunkWidth READ chunkWidth WRITE setChunkWidth)
    Q_PROPERTY(int chunkHeight READ chunkHeight WRITE setChunkHeight)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)

public:
    explicit SelectOption(QWidget *parent = nullptr);

    int chunkPos() const;
    void setChunkPos(int pos);

    int chunkWidth() const;
    void setChunkWidth(int width);

    int chunkHeight() const;
    void setChunkHeight(int height);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);

public slots:
    void moveLeft(int pixels);
    void moveRight(int pixels);
    void moveUp(int pixels);
    void moveDown(int pixels);
    void setChunkColor(QColor color);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_chunkPos;
    int m_chunkWidth;
    int m_chunkHeight;
    QColor m_color;
    Qt::Orientation m_orientation;
    QPropertyAnimation *m_animation;
    short m_duration;
};

#endif // SELECTOPTION_H
