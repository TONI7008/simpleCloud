#include "elidedlabel.h"
#include <QPainter>
#include <QSizePolicy>
#include <QTextLayout>

ElidedLabel::ElidedLabel(QWidget *parent)
    : QFrame(parent), elided(false), m_padding(2)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

ElidedLabel::ElidedLabel(const QString &text, QWidget *parent)
    : QFrame(parent), elided(false), content(text), m_padding(2)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

QString ElidedLabel::text() const
{
    return content;
}

void ElidedLabel::setText(const QString &newText)
{
    if (content != newText) {
        content = newText;
        update();
        emit textChanged(content);
    }
}

QFont ElidedLabel::font() const
{
    return m_useCustomFont ? m_customFont : QFrame::font();
}

void ElidedLabel::setFont(const QFont &font)
{
    if (m_customFont != font) {
        m_customFont = font;
        m_useCustomFont = true;
        update();
        emit fontChanged(m_customFont);
    }
}

void ElidedLabel::setFontSize(int pointSize)
{
    QFont newFont = font();
    newFont.setPointSize(pointSize);
    setFont(newFont);
}

void ElidedLabel::setFontFamily(const QString &family)
{
    QFont newFont = font();
    newFont.setFamily(family);
    setFont(newFont);
}

Qt::Alignment ElidedLabel::alignment() const
{
    return m_alignment;
}

void ElidedLabel::setAlignment(Qt::Alignment alignment)
{
    if (m_alignment != alignment) {
        m_alignment = alignment;
        update();
    }
}

bool ElidedLabel::isElided() const
{
    return elided;
}

int ElidedLabel::padding() const
{
    return m_padding;
}

void ElidedLabel::setPadding(int padding)
{
    if (m_padding != padding) {
        m_padding = padding;
        update();
    }
}

void ElidedLabel::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::TextAntialiasing);
    if (m_useCustomFont)
        painter.setFont(m_customFont);

    QFontMetrics fontMetrics(painter.font());
    int lineSpacing = fontMetrics.lineSpacing();
    bool didElide = false;

    QTextLayout textLayout(content, painter.font());
    QTextOption opt;
    opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    textLayout.setTextOption(opt);
    textLayout.beginLayout();

    QVector<QTextLine> lines;
    int y = 0;
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid()) break;
        line.setLineWidth(width() - 2 * m_padding);
        if (y + lineSpacing <= height() || lines.isEmpty()) {
            line.setPosition(QPointF(0, y));
            lines.append(line);
            y += lineSpacing;
        } else {
            didElide = true;
            break;
        }
    }
    textLayout.endLayout();

    int totalHeight = y;
    int yOffset = 0;
    if (m_alignment & Qt::AlignVCenter)
        yOffset = (height() - totalHeight) / 2;
    else if (m_alignment & Qt::AlignBottom)
        yOffset = height() - totalHeight;
    yOffset = qMax(yOffset, 0);

    for (int i = 0; i < lines.size(); ++i) {
        QTextLine line = lines[i];
        QString lineText = content.mid(line.textStart(), line.textLength());

        if (i == lines.size() - 1) {
            bool needsElision = false;
            bool isTruncated = (line.textStart() + line.textLength()) < content.length();
            bool isTooWide = fontMetrics.horizontalAdvance(lineText) > (width() - 2 * m_padding);
            bool isSingleLineElided = (lines.size() == 1 && didElide);
            if (isTruncated || isTooWide || isSingleLineElided)
                needsElision = true;

            if (needsElision) {
                QString ellipsis = "...";
                int maxWidth = width() - 2 * m_padding;
                QString elidedText;
                for (int len = lineText.length(); len >= 0; --len) {
                    QString candidate = lineText.left(len).trimmed() + ellipsis;
                    if (fontMetrics.horizontalAdvance(candidate) <= maxWidth) {
                        elidedText = candidate;
                        break;
                    }
                }
                if (elidedText.isEmpty()) {
                    elidedText = ellipsis;
                }
                lineText = elidedText;
                didElide = true;

            }
        }

        QRect lineRect(m_padding, yOffset + static_cast<int>(line.position().y()), width() - 2 * m_padding, lineSpacing);
        painter.drawText(lineRect, m_alignment & ~Qt::AlignVertical_Mask, lineText);
    }

    if (didElide != elided) {
        elided = didElide;
        emit elisionChanged(didElide);
    }

    // Tooltip logic
    setToolTip(didElide ? content : QString());
}
