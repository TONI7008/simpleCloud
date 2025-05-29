#ifndef CIRCULARPROGRESSBAR_H
#define CIRCULARPROGRESSBAR_H

#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPainter>
#include <QPushButton>
#include <QEasingCurve>
#include <QGraphicsDropShadowEffect>
#include <QTimer>

class CircularProgressBar : public QProgressBar {
    Q_OBJECT
    Q_PROPERTY(int startAngle READ angle WRITE setAngle NOTIFY startAngleChanged)
    Q_PROPERTY(float animationProgress READ animationProgress WRITE setAnimationProgress NOTIFY animationProgressChanged)
    Q_PROPERTY(double chunkLength READ chunkLength WRITE setChunkLength NOTIFY chunkLengthChanged)

public:
    explicit CircularProgressBar(QWidget *parent = nullptr);
    ~CircularProgressBar();

    // Setters
    void setCircularDegree(int value = 270);
    void setValue(int value = 0);
    void setSquare(bool enable = false);
    void setGradient(bool enable = false);
    void setGradientValues(const QMap<qreal, QColor> &map);
    void setMargin(int x = 0, int y = 0);
    void setTextAlignment(Qt::Alignment alignment = Qt::AlignCenter);
    void setProgressAlignment(Qt::Alignment alignment = Qt::AlignCenter);
    void setShadow(bool enable = true);
    void setWidth(int width = 200);
    void setHeight(int height = 200);
    void setProgressWidth(int width = 10);
    void setProgressRoundedCap(bool enable = true);
    void setEnableBg(bool enable = true);
    void setBgColor(const QColor &color = QColor(68, 71, 90));
    void setChunkColor(const QColor &color = QColor(73, 139, 209));
    void setEnableText(bool enable = true);
    void setMaxValue(int value = 100);
    void setMinValue(int value = 0);
    void setSuffix(const QString &suffix = "%");
    void setInfiniteLoop(bool loop);
    void setTextColor(const QColor &color = QColor(73, 139, 209));
    void setRange(int minValue, int maxValue);
    void setEasingCurve(QEasingCurve::Type curve);
    void setChunkLength(double length);
    void setDuration(short duration);
    void setAnimationProgress(float progress);
    void setAnimationThreshold(float threshold);

    // Getters
    double chunkLength() const { return m_chunkLength; }
    int getCircularDegree() const { return circularDegree; }
    int getMarginX() const { return marginX; }
    int getMarginY() const { return marginY; }
    int getWidth() const { return width; }
    bool isSquared() const { return square; }
    bool hasGradient() const { return gradient; }
    QMap<qreal, QColor> getGradientValues() const { return gradient_colors; }
    int getHeight() const { return height; }
    int getProgressWidth() const { return progress_width; }
    Qt::Alignment getTextAlignment() const { return textAlignment; }
    Qt::Alignment getProgressAlignment() const { return progressAlignment; }
    bool hasShadow() const { return shadow; }
    bool hasRoundedCap() const { return progress_rounded_cap; }
    bool isBackgroundEnabled() const { return enable_bg; }
    QColor getBgColor() const { return bg_color; }
    QColor getChunkColor() const { return chunk_color; }
    bool isTextEnabled() const { return enable_text; }
    QString getSuffix() const { return suffix; }
    QColor getTextColor() const { return text_color; }
    float animationProgress() const { return m_animationProgress; }
    float animationThreshold() const { return m_threshold; }
    bool isInfiniteLoop() const { return infiniteloop; }
    void stop();
    bool isStopped() const { return m_stop; }

    void setTextBold(bool bold);
    void setFontSizeRatio(float ratio);
    // Add these to the public section of the class
    bool isClockwise() const { return m_clockwise; }
    void setClockwise(bool clockwise);


    void click();
signals:
    void modeChanged(bool isInfinite);
    void animationProgressChanged(float progress);
    void SI_circularDegreeChanged(int value);
    void SI_valueChanged(int value);
    void SI_shadowChanged(bool enable);
    void SI_squareChanged(bool enable);
    void SI_marginChanged(int x, int y);
    void SI_widthChanged(int width);
    void SI_gradientChanged(bool enable);
    void SI_gradientValuesChanged(QMap<qreal, QColor> map);
    void SI_heightChanged(int height);
    void SI_textAlignmentChanged(Qt::Alignment alignment);
    void SI_progressAlignmentChanged(Qt::Alignment alignment);
    void SI_progressWidthChanged(int width);
    void SI_progressCapChanged(bool enable);
    void SI_backgroundChanged(bool enable);
    void SI_backgroundColorChanged(QColor color);
    void SI_chunkColorChanged(QColor color);
    void SI_textEnableChanged(bool enable);
    void SI_suffixChanged(QString suffix);
    void SI_textColorChanged(QColor color);
    void chunkLengthChanged(double length);
    void startAngleChanged(int angle);
    void stopEmitted(bool stopped);


protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateChunkPosition();
    void updateProgressAnimation();
    void setupAnimations();
    int angle() const { return startAngle; }
    void setAngle(int angle);

    QPainter *paint = nullptr;
    QPropertyAnimation *animation = nullptr;
    QTimer *m_timer = nullptr;
    QPushButton *stopButton = nullptr;

    // Visual properties
    bool square = true;
    int circularDegree = 360;
    int width = 100;
    int height = 100;
    int marginX = 0;
    int marginY = 0;
    int progress_width = 10;
    bool shadow = false;
    bool progress_rounded_cap = true;
    bool enable_bg = true;
    bool gradient = false;
    bool enable_text = true;
    bool infiniteloop = false;
    bool m_stop = false;

    // Animation properties
    float m_animationProgress = 0.0f;
    float m_threshold = 0.02f;
    int startAngle = 0;
    short m_duration = 18;
    double m_chunkLength = 135;
    QFont textFont;
    float fontSizeRatio = 0.1f; // Ratio of height for font size
    bool boldText = true;

    QEasingCurve::Type m_curve = QEasingCurve::OutQuart;

    // Color properties
    QColor bg_color = QColor(20, 20, 20, 255);
    QColor chunk_color = QColor(73, 139, 209);
    QColor text_color = QColor(73, 139, 209);
    QMap<qreal, QColor> gradient_colors;

    // Text properties
    QString suffix = "%";
    Qt::Alignment textAlignment = Qt::AlignCenter;
    Qt::Alignment progressAlignment = Qt::AlignCenter;
    // Add this to the private section
    bool m_clockwise = true; // Default to clockwise
};

#endif // CIRCULARPROGRESSBAR_H
