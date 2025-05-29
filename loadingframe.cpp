#include "loadingframe.h"
#include "circularprogressbar.h"
#include "tcloud.h"

LoadingFrame::LoadingFrame(QWidget *parent) : TFrame(parent)
{
    hide();
    setCornerStyle(TFrame::CornerStyle::None);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
    setBackgroundImage(":/pictures/image2.png");

    // Main layout using QVBoxLayout for better vertical centering
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);  // Remove default margins
    mainLayout->setSpacing(5);  // Space between spinner and label

    // Container widget for centered content
    QWidget *contentWidget = new QWidget(this);
    contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Layout for the centered content
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(40);  // Consistent spacing

    m_handler = new AnimationHandler(nullptr);
    m_handler->initializeEffect(this);

    // Spinner setup
    m_spinner = new CircularProgressBar(this);
    m_spinner->setMinimumSize(120, 120);
    m_spinner->setProgressWidth(8);
    m_spinner->setChunkColor(Qt::white);
    m_spinner->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);  // Fixed size for proper centering

    // Label setup
    m_textLabel = new QLabel("Please wait...", this);
    m_textLabel->setMinimumHeight(40);  // Reduced height for better proportions
    m_textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_textLabel->setAlignment(Qt::AlignCenter);

    // Style sheet
    QString style = R"(
        QFrame {
            background: transparent;
            border: none;
            font: 700 16pt "Noto Sans";
            color: rgba(189, 189, 189,0.5);
        }
        QLabel {
            background: transparent; // Added for better visibility
            font: 700 16pt "Noto Sans";
            color: rgba(189, 189, 189,0.5);

        }
    )";
    setStyleSheet(style);

    // Add widgets to content layout with stretch factors
    contentLayout->addStretch(1);  // Push content to vertical center
    contentLayout->addWidget(m_spinner, 0, Qt::AlignHCenter);
    contentLayout->addWidget(m_textLabel, 0, Qt::AlignHCenter);
    contentLayout->addStretch(1);  // Push content to vertical center

    // Add content widget to main layout
    mainLayout->addWidget(contentWidget);
    setLayout(mainLayout);

    // Animation connection
    connect(m_handler->getAnimation(), &QPropertyAnimation::finished, this, [this]() {
        if (m_handler->effect()->opacity() == 0) {
            m_handler->effect()->setOpacity(0);
            m_spinner->setInfiniteLoop(false);
            hide();
            loading = false;
        } else {
            m_spinner->setInfiniteLoop(true);
            show();
            raise();
            m_handler->effect()->setOpacity(1);
            loading = true;
        }
    });
}

LoadingFrame::~LoadingFrame()
{
    m_spinner->deleteLater();
    m_handler->deleteLater();
}

void LoadingFrame::start(QRect geometry, const QString &initialText)
{
    setMinimumSize(geometry.size());
    raise();
    m_handler->startOpacityAnimation(0.0, 1.0);
    if(initialText.isEmpty()){
        m_textLabel->hide();
    }else{
        m_textLabel->setText(initialText);
        m_textLabel->show();
    }
}

void LoadingFrame::stop()
{
    if(m_handler->getAnimation()->state()==QAbstractAnimation::Running) m_handler->getAnimation()->stop();
    m_handler->startOpacityAnimation(1.0, 0.0);
}

bool LoadingFrame::isLoading()
{
    return loading;
}

void LoadingFrame::setMessage(const QString &t) const
{
    m_textLabel->setText(t);
    m_textLabel->update();
}
