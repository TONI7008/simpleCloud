#include "tnotification.h"
#include "tstackedwidget.h"
#include <QPainter>


QList<QColor> getDominantColors(const QString& imagePath, int colorCount = 2) {
    QList<QColor> dominantColors;
    QImage image(imagePath);

    if (image.isNull()) {
        qWarning() << "Failed to load image:" << imagePath;
        return dominantColors;
    }

    const int step = qMax(1, qMin(image.width(), image.height()) / 100); // Downsample step
    QHash<QRgb, int> colorFrequencies;

    for (int y = 0; y < image.height(); y += step) {
        for (int x = 0; x < image.width(); x += step) {
            QColor color = image.pixelColor(x, y);

            // Quantize to 4 bits per channel (16 levels per channel)
            int r = (color.red() & 0xF0);
            int g = (color.green() & 0xF0);
            int b = (color.blue() & 0xF0);

            QRgb quantized = qRgb(r, g, b);
            colorFrequencies[quantized]++;
        }
    }

    if (colorFrequencies.isEmpty()) {
        qWarning() << "No colors found or empty image:" << imagePath;
        return dominantColors;
    }

    QList<QPair<int, QRgb>> sortedFrequencies;
    for (auto it = colorFrequencies.constBegin(); it != colorFrequencies.constEnd(); ++it) {
        sortedFrequencies.append(qMakePair(it.value(), it.key()));
    }

    std::sort(sortedFrequencies.begin(), sortedFrequencies.end(), [](const QPair<int, QRgb>& a, const QPair<int, QRgb>& b) {
        return a.first > b.first;
    });

    for (int i = 0; i < qMin(colorCount, sortedFrequencies.size()); ++i) {
        dominantColors.append(QColor(sortedFrequencies[i].second));
    }

    return dominantColors;
}

// TNotifaction implementation

TNotifaction* TNotifaction::m_dynamicFrame=nullptr;


TNotifaction::TNotifaction(QWidget* parent) : PopoutFrame(parent)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(1, 1, 1, 2);
    m_layout->setSpacing(2);
    m_layout->setAlignment(Qt::AlignVCenter);

    m_stack = new TStackedWidget(this);
    //m_stack->setAnimationDuration(500);
    m_stack->setAnimationType(TStackedWidget::VerticalSlide);


    m_mWidget=new MessageWidget(this);
    m_stack->addWidget(m_mWidget);


    m_layout->addWidget(m_stack);
    setLayout(m_layout);

    setPopoutMaxWidth(900);
    setPopoutMaxHeight(50);

    setBackgroundImage(":/pictures/image0.png");
    //setBorder(false);

    connect(m_stack,&TStackedWidget::animationFinished,this,[this]{
        popout(600,1.6);
    });

    resize(400,50);


    connect(this,&PopoutFrame::hoverEnded,this,[this]{
        if(borderColor()==Qt::red){
            setBorderColor();
        }
    });

    m_stack->setCurrentWidget(m_mWidget);
    setEnableBackground(true);
    setBorderSize(3);
    setFollowBorder(true);
    setBorder(true);

    //setStyleSheet("QFrame{background:rgba(190,190,190,0.7);}");

}


TNotifaction::~TNotifaction()
{

}

TStackedWidget* TNotifaction::stackedWidget() const
{
    return m_stack;
}

void TNotifaction::setAnimationDuration(int duration)
{
    m_stack->setAnimationDuration(duration);
}


void TNotifaction::setMaxwidth(int width)
{
    setPopoutMaxWidth(width);
}

void TNotifaction::setMessage(const QString &text,bool error)
{
    emit incomingNotif();

    if(m_mWidget){
        if(error){
            setBorderColor(Qt::red);
        }else{
            setBorderColor();
        }
        m_stack->setCurrentWidget(m_mWidget);
        m_mWidget->setMessage(text);
    }
}

TNotifaction *TNotifaction::instance()
{
    if(!m_dynamicFrame){
        m_dynamicFrame=new TNotifaction;
    }
    return m_dynamicFrame;
}

void TNotifaction::cleanUp()
{
    if(!m_dynamicFrame){
        delete m_dynamicFrame;
    }
}

QColor oppositeColor(const QColor &color) {
    int r = 255 - color.red();
    int g = 255 - color.green();
    int b = 255 - color.blue();
    int a = color.alpha();
    return QColor(r, g, b, a);
}

/*void TNotifaction::setBackgroundImage(const QString &imgPath)
{
    QList<QColor> topColors = getDominantColors(imgPath, 3);
    if (topColors.isEmpty()) {
        qWarning() << "TNotifaction::setBackgroundImage: No dominant colors found for image" << imgPath;
        return;
    }
    //QColor dominant=topColors.first();
    //setBorderColor(oppositeColor(dominant));

    QSize currentWidgetSize = size();
    if (!currentWidgetSize.isValid() || currentWidgetSize.isEmpty()) {
        qWarning() << "TNotifaction::setBackgroundImage: Invalid widget size ("
                   << currentWidgetSize.width() << "x" << currentWidgetSize.height() << "). Using 1x1 fallback.";
        currentWidgetSize = QSize(1, 1);
    }

    QPixmap gradientPixmap(currentWidgetSize);
    gradientPixmap.fill(Qt::transparent);

    QLinearGradient gradient(QPointF(0, 0), QPointF(currentWidgetSize.width(), currentWidgetSize.height()));

    const int stopCount = topColors.size();
    for (int i = 0; i < stopCount; ++i) {
        QColor color = topColors[i];
        color.setAlphaF(0.5);
        qreal position = static_cast<qreal>(i) / (stopCount - 1);
        gradient.setColorAt(position, color);
    }

    QPainter painter(&gradientPixmap);
    painter.fillRect(gradientPixmap.rect(), gradient);
    painter.end();

    TFrame::setBackgroundImage(gradientPixmap);
}*/

#include "tlabel.h"

MessageWidget::MessageWidget(QWidget *parent)
    : QWidget{parent}
{
    m_layout= new QHBoxLayout(this);
    m_layout->setContentsMargins(2,2,2,2);
    m_label=new TLabel(this);
    m_label->setMinimumWidth(200);
    m_label->setMinimumHeight(40);

    //m_label->setWordWrap(true);
    m_label->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_label->setAlignment(Qt::AlignCenter);

    m_label->setText("Welcome to simpleCloud");

    m_layout->addWidget(m_label);
    setLayout(m_layout);

    setStyleSheet("QFrame{background:rgb(200,255,200);}");
}

MessageWidget::~MessageWidget() {

}

void MessageWidget::setMessage(const QString &whatToDisplay)
{
    if(m_label){
        m_label->setText(whatToDisplay);
        m_label->update();
    }
}
