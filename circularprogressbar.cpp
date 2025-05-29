#include "circularprogressbar.h"
#include <QPainterPath>
#include <QDebug>
#include <QResizeEvent>

CircularProgressBar::CircularProgressBar(QWidget *parent) : QProgressBar(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(50, 50);
    resize(width, height);

    paint = new QPainter();
    m_timer = new QTimer(this);
    animation = new QPropertyAnimation(this, "animationProgress");
    animation->setEasingCurve(m_curve);
    animation->setDuration(200);

    stopButton = new QPushButton(this);
    stopButton->setVisible(false);
    stopButton->setCheckable(true);
    stopButton->setChecked(false);
    stopButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //stopButton->setStyleSheet("background:rgba(193, 195, 196,0.6); border-radius: 6px; border: none;");
    stopButton->setStyleSheet(R"(
    QPushButton {
        background: transparent;
    }
    QPushButton:hover {
    background-image: url(":/icons/closeblue.svg");
    background-repeat: no-repeat;
    background-position: center;
    }
)");
    stopButton->setIcon(QIcon(":/icons/closewhite.svg"));
    stopButton->setIconSize(QSize(26, 26));

    connect(m_timer, &QTimer::timeout, this, &CircularProgressBar::updateChunkPosition);
    connect(animation, &QPropertyAnimation::valueChanged, this, [this](){ update(); });
    connect(this, &QProgressBar::valueChanged, this, [this](){
        if(!infiniteloop) updateProgressAnimation();
    });
    connect(stopButton, &QPushButton::clicked, this, &CircularProgressBar::stopEmitted);

    setupAnimations();
}

CircularProgressBar::~CircularProgressBar() {
    delete paint;
    delete animation;
    if (m_timer->isActive()) m_timer->stop();
    delete m_timer;
}

void CircularProgressBar::setupAnimations() {
    m_timer->start(m_duration); // ~60fps
    QTimer::singleShot(0, this, [this]() {
        if (maximum() == minimum()) {
            setInfiniteLoop(true);
        }
    });
}

void CircularProgressBar::setAnimationProgress(float progress) {
    if (m_animationProgress != progress) {
        m_animationProgress = progress;
        emit animationProgressChanged(progress);
    }
}

void CircularProgressBar::setAnimationThreshold(float threshold)
{
    m_threshold=threshold;
}

void CircularProgressBar::setAngle(int angle)
{
    startAngle=angle;
}



void CircularProgressBar::updateProgressAnimation() {
    float range = maximum() - minimum();
    float target = (range > 0) ? (value() - minimum()) / range : 0.0f;
    float delta = std::abs(target - m_animationProgress);

    if (value() >= maximum() || delta >= m_threshold) {
        animation->stop();
        animation->setStartValue(m_animationProgress);
        animation->setEndValue(target);
        animation->setDuration(qBound(100, static_cast<int>(delta * 1000), 1000));
        animation->start();
    }
}

/*void CircularProgressBar::setInfiniteLoop(bool loop) {
    if (infiniteloop == loop) return;

    infiniteloop = loop;
    if (infiniteloop) {
        animation->stop();
        m_timer->start(m_duration);
    } else {
        m_timer->stop();
        updateProgressAnimation();
        repaint();
    }
    emit modeChanged(infiniteloop);
}*/

void CircularProgressBar::setValue(int value) {
    value = qBound(minimum(), value, maximum());
    if (value == this->value()) return;
    QProgressBar::setValue(value);
}

void CircularProgressBar::setGradient(bool enable)
{
    gradient=enable;

    repaint();
}

void CircularProgressBar::setGradientValues(const QMap<qreal, QColor> &map)
{
    gradient_colors=map;

    repaint();
}

void CircularProgressBar::setWidth(int width)
{
    this->width=width;
    repaint();
}

void CircularProgressBar::setHeight(int height)
{
    this->height=height;
    repaint();
}

void CircularProgressBar::setProgressWidth(int width)
{
    progress_width=width;
    repaint();
}

void CircularProgressBar::setProgressRoundedCap(bool enable)
{
    progress_rounded_cap=enable;
    repaint();
}

void CircularProgressBar::setEnableBg(bool enable)
{
    enable_bg=enable;
    repaint();
}

void CircularProgressBar::setChunkColor(const QColor &color)
{
    chunk_color=color;
    repaint();
}

void CircularProgressBar::setEnableText(bool enable)
{
    enable_text=enable;
    repaint();
}

void CircularProgressBar::setRange(int minValue, int maxValue) {
    if (minValue >= maxValue) {
        qWarning() << "Invalid range";
        return;
    }

    //bool wasInfinite = !infiniteloop && (maximum() == minimum());
    QProgressBar::setRange(minValue, maxValue);

    if (infiniteloop && (maxValue > minValue)) {
        setInfiniteLoop(false);
    } else if (maxValue == minValue) {
        setInfiniteLoop(true);
    }
}

void CircularProgressBar::setChunkLength(double length)
{
    m_chunkLength=length;
    repaint();
}

void CircularProgressBar::setDuration(short duration)
{
    if(infiniteloop) {
        setInfiniteLoop(false);
        m_duration=duration;
        setInfiniteLoop(true);
    }

    m_duration=duration;
}

void CircularProgressBar::stop() {
    m_stop = true;
    setInfiniteLoop(false);
    animation->stop();
}

void CircularProgressBar::setTextBold(bool bold) {
    if (boldText != bold) {
        boldText = bold;
        textFont.setBold(boldText);
        repaint();
        //emit SI_textBoldChanged(boldText);
    }
}

void CircularProgressBar::setFontSizeRatio(float ratio) {
    if (ratio > 0 && ratio <= 0.5f && fontSizeRatio != ratio) { // Reasonable limits for ratio
        fontSizeRatio = ratio;
        repaint();
        //emit SI_fontSizeRatioChanged(fontSizeRatio);
    }
}

void CircularProgressBar::setCircularDegree(int value)
{
    circularDegree=value;
    repaint();

}


void CircularProgressBar::setClockwise(bool clockwise) {
    if (m_clockwise != clockwise) {
        m_clockwise = clockwise;

        // Reverse the animation direction if needed
        if (infiniteloop) {
            startAngle = m_clockwise ? 0 : 360;
        }

        update();
    }
}

void CircularProgressBar::click()
{
    stopButton->click();
}

void CircularProgressBar::resizeEvent(QResizeEvent *event) {
    QSize size = event->size();

    if (square) {
        width = height = qMin(size.width(), size.height());
        if (progressAlignment & Qt::AlignCenter) {
            marginX = (size.width() - width) / 2;
            marginY = (size.height() - height) / 2;
        } else {
            marginX = (progressAlignment & Qt::AlignRight) ? (size.width() - width) :
                          (progressAlignment & Qt::AlignHCenter) ? (size.width() - width) / 2 : 0;
            marginY = (progressAlignment & Qt::AlignBottom) ? (size.height() - height) :
                          (progressAlignment & Qt::AlignVCenter) ? (size.height() - height) / 2 : 0;
        }
    } else {
        width = size.width();
        height = size.height();
        marginX = marginY = 0;
    }

    if (stopButton->isVisible()) {
        int btnSize = qMin(width, height) / 2;
        stopButton->setGeometry(
            marginX + (width - btnSize) / 2,
            marginY + (height - btnSize) / 2,
            btnSize, btnSize
            );
    }

    update();
}

void CircularProgressBar::updateChunkPosition() {
    startAngle += m_clockwise ? 6 : -6; // Inverted the signs for proper direction
    if (startAngle >= 360) startAngle -= 360;
    else if (startAngle < 0) startAngle += 360;
    update();
}

void CircularProgressBar::setInfiniteLoop(bool loop) {
    if (infiniteloop == loop) return;

    infiniteloop = loop;
    if (infiniteloop) {
        animation->stop();
        startAngle = 0; // Always start from top
        m_timer->start(m_duration);
    } else {
        m_timer->stop();
        updateProgressAnimation();
        repaint();
    }
    emit modeChanged(infiniteloop);
}

void CircularProgressBar::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // Calculate drawing area
    int pnwidth = width - progress_width;
    int pnheight = height - progress_width;
    int margin = progress_width / 2;
    QRectF rect(marginX + margin, marginY + margin, pnwidth, pnheight);

    // Calculate adjusted background arc start angle
    int offset = (circularDegree > 180) ? (circularDegree - 180) / 2 : 0;
    int bgStartAngle = (180 + offset) * 16;
    int bgSpanAngle = m_clockwise ? -circularDegree * 16 : circularDegree * 16;

    // Draw background arc if enabled
    if (enable_bg) {
        QPen bgPen;
        bgPen.setColor(bg_color);
        bgPen.setWidth(progress_width);
        bgPen.setCosmetic(true);
        if (progress_rounded_cap) bgPen.setCapStyle(Qt::RoundCap);
        painter.setPen(bgPen);
        painter.drawArc(rect, bgStartAngle, bgSpanAngle);
    }

    // Calculate progress proportion (0.0 to 1.0)
    double progress = infiniteloop ? 0.0 : m_animationProgress;

    // Setup progress pen
    QPen pen;
    pen.setWidth(progress_width);
    pen.setCosmetic(true);
    if (progress_rounded_cap) pen.setCapStyle(Qt::RoundCap);

    // Apply gradient or solid color
    QLinearGradient linearGrad;
    if (progress > 0.85) {
        pen.setColor(QColor(71, 158, 245));
    } else if (gradient) {
        linearGrad = QLinearGradient(rect.topLeft(), rect.bottomRight());
        for (auto it = gradient_colors.begin(); it != gradient_colors.end(); ++it) {
            linearGrad.setColorAt(it.key(), it.value());
        }
        pen.setBrush(linearGrad);
    } else {
        pen.setColor(chunk_color);
    }

    // Calculate progress arc angles
    int spanAngle = infiniteloop ? m_chunkLength * 16 : progress * circularDegree * 16;
    if (m_clockwise) spanAngle = -spanAngle;

    int arcStartAngle = infiniteloop ? (180 * 16 + startAngle * 16) : bgStartAngle;

    // Draw the progress arc
    painter.setPen(pen);
    painter.drawArc(rect, arcStartAngle, spanAngle);

    // Draw text or button based on mode
    if (enable_text && !infiniteloop) {
        int fontSize = static_cast<int>(height * fontSizeRatio);
        textFont.setPixelSize(fontSize);
        textFont.setBold(boldText);
        painter.setFont(textFont);

        QPen textPen;
        if (gradient && progress <= 0.85) {
            textPen.setBrush(linearGrad);
        } else {
            textPen.setColor(text_color.isValid() ? text_color : chunk_color);
        }
        painter.setPen(textPen);

        QString text = QString::number(static_cast<int>(progress * 100)) + suffix;
        QFontMetrics metrics(textFont);
        QRect textRect = metrics.boundingRect(text);
        QPoint center = rect.center().toPoint();
        textRect.moveCenter(center);
        painter.drawText(textRect, Qt::AlignCenter, text);

        stopButton->setVisible(false);
    } else if (!enable_text && !infiniteloop) {
        int btnSize = qMin(width, height) / 2;
        stopButton->setGeometry(
            rect.center().x() - btnSize / 2,
            rect.center().y() - btnSize / 2,
            btnSize, btnSize
            );
        stopButton->setVisible(true);
    }
}
