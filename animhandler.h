#ifndef ANIMHANDLER_H
#define ANIMHANDLER_H

#include <QGridLayout>
#include <QGraphicsOpacityEffect>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QTimer>
#include <QLabel>
#include <queue>

class AnimationHandler : public QObject {
    Q_OBJECT
public:
    explicit AnimationHandler(QObject *parent = nullptr)
        : QObject(parent), opacityAnimation(nullptr), m_effect(nullptr),
        m_duration(133), m_easingCurve(QEasingCurve::OutSine) {
    }

    void initializeEffect(QWidget *widget) {
        if (widget) {
            // Clean up previous effect if exists
            if (m_effect) {
                widget->setGraphicsEffect(nullptr);
                delete m_effect;
            }

            m_effect = new QGraphicsOpacityEffect(widget);
            widget->setGraphicsEffect(m_effect);

            if (opacityAnimation) {
                opacityAnimation->stop();
                delete opacityAnimation;
            }

            opacityAnimation = new QPropertyAnimation(m_effect, "opacity", this);
            opacityAnimation->setDuration(m_duration);
            opacityAnimation->setEasingCurve(m_easingCurve);

            connect(opacityAnimation, &QPropertyAnimation::finished,
                    this, &AnimationHandler::processNextAnimation);
            connect(opacityAnimation, &QPropertyAnimation::valueChanged,
                    this, &AnimationHandler::animationValueChanged);
        }
    }

    QGraphicsOpacityEffect* effect() const {
        return m_effect;
    }

    QPropertyAnimation* getAnimation() const {
        return opacityAnimation;
    }

    void startOpacityAnimation(qreal startValue, qreal endValue, int duration = -1) {
        if (!opacityAnimation || !m_effect) return;

        // Use custom duration if provided, otherwise use default
        int animDuration = duration > 0 ? duration : m_duration;
        opacityAnimation->setDuration(animDuration);

        // If no animation is running, start immediately
        if (opacityAnimation->state() != QAbstractAnimation::Running) {
            m_effect->setOpacity(startValue); // Set initial value immediately
            opacityAnimation->setStartValue(startValue);
            opacityAnimation->setEndValue(endValue);
            opacityAnimation->start();
        } else {
            // Otherwise add to queue with timing information
            AnimationParams params;
            params.startValue = startValue;
            params.endValue = endValue;
            params.duration = animDuration;
            animationQueue.push(params);
        }
    }

    void setDuration(int duration) {
        if (duration > 0) {
            m_duration = duration;
            if (opacityAnimation) {
                opacityAnimation->setDuration(duration);
            }
        }
    }

    void setEasingCurve(QEasingCurve::Type curve) {
        m_easingCurve = curve;
        if (opacityAnimation) {
            opacityAnimation->setEasingCurve(curve);
        }
    }

    void fadeIn(QWidget* widget, int duration = -1) {
        initializeEffect(widget);
        startOpacityAnimation(0.0, 1.0, duration);
    }

    void fadeOut(QWidget* widget, int duration = -1) {
        initializeEffect(widget);
        startOpacityAnimation(1.0, 0.0, duration);
    }

    ~AnimationHandler() {
        if (opacityAnimation) {
            opacityAnimation->stop();
            disconnect(opacityAnimation, nullptr, this, nullptr);
            delete opacityAnimation;
        }
        if (m_effect) {
            m_effect=nullptr;
        }
    }

signals:
    void animationValueChanged(const QVariant &value);
    void animationStarted();
    void animationFinished();

protected slots:
    void processNextAnimation() {
        emit animationFinished();

        if (!animationQueue.empty()) {
            auto nextAnim = animationQueue.front();
            animationQueue.pop();

            m_effect->setOpacity(nextAnim.startValue);
            opacityAnimation->setStartValue(nextAnim.startValue);
            opacityAnimation->setEndValue(nextAnim.endValue);
            opacityAnimation->setDuration(nextAnim.duration);

            emit animationStarted();
            opacityAnimation->start();
        }
    }

protected:
    struct AnimationParams {
        qreal startValue;
        qreal endValue;
        int duration;
    };

    QPropertyAnimation *opacityAnimation;
    QGraphicsOpacityEffect *m_effect;
    int m_duration;
    QEasingCurve::Type m_easingCurve;
    std::queue<AnimationParams> animationQueue;
};
#endif // ANIMHANDLER_H
