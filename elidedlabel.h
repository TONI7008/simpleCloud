#ifndef ELIDEDLABEL_H
#define ELIDEDLABEL_H

#include <QFrame>
#include <QFont>
#include <QTextLayout>

class ElidedLabel : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool isElided READ isElided NOTIFY elisionChanged)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(int padding READ padding WRITE setPadding) // New property

public:
    explicit ElidedLabel(QWidget *parent = nullptr);
    explicit ElidedLabel(const QString &text, QWidget *parent = nullptr);

    // Text content
    QString text() const;
    void setText(const QString &text);

    // Font handling
    QFont font() const;
    void setFont(const QFont &font);
    void setFontSize(int pointSize);
    void setFontFamily(const QString &family);

    // Alignment
    Qt::Alignment alignment() const;
    void setAlignment(Qt::Alignment alignment);

    // Elision state
    bool isElided() const;

    // Padding
    int padding() const;
    void setPadding(int padding);

signals:
    void textChanged(const QString &text);
    void elisionChanged(bool elided);
    void fontChanged(const QFont &font);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool elided;
    QString content;
    Qt::Alignment m_alignment = Qt::AlignLeft | Qt::AlignVCenter;
    QFont m_customFont;
    bool m_useCustomFont = false;
    int m_padding = 4; // New member variable
};

#endif // ELIDEDLABEL_H
