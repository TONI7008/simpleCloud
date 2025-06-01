#include "tdynamicframe.h"
#include "tstackedwidget.h"
#include <QPainter>

const QString HistoryDisplay::DEFAULT_STYLE = R"(
QPushButton {
    border-radius: 10px;
    background: rgba(30,30,30,0.5);
    padding-left: 10px;
    padding-right: 10px;
    color: white;
    font: 13pt "Segoe UI";
    border: none;
    min-height: 32px;
}

QPushButton:hover {
    background: #479EF5;
    color: black;
}

QPushButton:pressed {
    background-color: rgba(70, 158, 245, 0.6);
}

QPushButton#separator {
    background: transparent;
    border: none;
    padding: 0;
    min-width: 24px;
    min-height: 24px;
    border-radius: 0;
}

QPushButton#minimalButton {
    min-width: 80px;
    max-width: 200px;
}

QPushButton#breadcrumbButton {
    min-width: 120px;
    max-width: 300px;
}
)";

// File: utils/image_utils.cpp

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

HistoryDisplay::HistoryDisplay(QWidget* parent) : QWidget(parent)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(5, 2, 5, 2);
    m_layout->setSpacing(2);

    m_homeButton = new QPushButton(this);
    m_homeButton->setVisible(false);
    m_homeButton->setIcon(QIcon(":/icons/folder.svg"));
    m_homeButton->setStyleSheet("background: transparent; border: none;");
    m_homeButton->setIconSize(QSize(28, 28));
    m_homeButton->setFixedSize(QSize(28, 28));
    connect(m_homeButton, &QPushButton::clicked, this, &HistoryDisplay::homeClicked);

    m_homeIcon = QIcon(":/icons/folder.svg");
    m_separatorIcon = QIcon(":/icons/rightwhite.svg");
    m_buttonStyle = DEFAULT_STYLE;
}

HistoryDisplay::~HistoryDisplay()
{
    clearLayout();
}

void HistoryDisplay::setPath(const QString& newPath)
{
    if (m_path != newPath) {
        m_path = newPath;
        updateView();
    }
}

QString HistoryDisplay::path() const
{
    return m_path;
}

HistoryDisplay::View HistoryDisplay::view() const
{
    return m_view;
}

void HistoryDisplay::setView(View newView)
{
    if (m_view != newView) {
        m_view = newView;
        updateView();
    }
}

void HistoryDisplay::setHomeIcon(const QIcon& icon)
{
    m_homeIcon = icon;
    if (m_homeButton) {
        m_homeButton->setIcon(icon);
    }
}

void HistoryDisplay::setSeparatorIcon(const QIcon& icon)
{
    m_separatorIcon = icon;
    updateView();
}

void HistoryDisplay::setButtonStyle(const QString& style)
{
    m_buttonStyle = style;
    updateView();
}

void HistoryDisplay::updateView()
{
    clearLayout();

    switch (m_view) {
    case Modern:
        createModernView();
        break;
    case Normal:
        createNormalView();
        break;
    case Minimal:
        createMinimalView();
        break;
    case Breadcrumb:
        createBreadcrumbView();
        break;
    default:
        createModernView();
        break;
    }

    update();
}

void HistoryDisplay::createModernView()
{
    if (m_path.isEmpty()) return;

    QStringList parts = m_path.split("/", Qt::SkipEmptyParts);
    if (parts.isEmpty()) return;

    if (m_path.startsWith(":")) {
        m_layout->addWidget(m_homeButton);
        m_homeButton->setVisible(true);
        m_homeButton->setIcon(m_homeIcon);
    }

    const int maxButtons = 10;
    int startIdx = parts.size() > maxButtons ? parts.size() - maxButtons : 1;

    if (startIdx > 1) {
        QPushButton* ellipsis = new QPushButton("...", this);
        ellipsis->setStyleSheet(m_buttonStyle);
        ellipsis->setEnabled(false);
        m_layout->addWidget(ellipsis);
    }

    for (int i = startIdx; i < parts.size(); ++i) {
        QPushButton* pathButton = new QPushButton(parts[i], this);
        pathButton->setStyleSheet(m_buttonStyle);
        pathButton->setMinimumHeight(32);
        QString full = buildPartialPath(i);
        pathButton->setProperty("fullPath", full);

        connect(pathButton, &QPushButton::clicked, this, [this, full]() {
            emit pathClicked(full);
        });

        m_layout->addWidget(pathButton);

        if (i < parts.size() - 1) {
            QPushButton* separator = new QPushButton(this);
            separator->setStyleSheet("background: transparent; border: none;");
            separator->setIcon(m_separatorIcon);
            separator->setFixedSize(24, 24);
            separator->setEnabled(false);
            m_layout->addWidget(separator);
        }
    }
}

void HistoryDisplay::createNormalView()
{
    if (m_path.isEmpty()) return;

    QPushButton* pathButton = new QPushButton(m_path, this);
    pathButton->setStyleSheet(m_buttonStyle);
    pathButton->setProperty("fullPath", m_path);
    connect(pathButton, &QPushButton::clicked,this, [this, pathButton]() {
        emit pathClicked(pathButton->property("fullPath").toString());
    });

    m_layout->addWidget(pathButton);
}

/*void HistoryDisplay::createMinimalView()
{
    if (m_path.isEmpty()) return;

    QString displayText = m_path.split("/", Qt::SkipEmptyParts).last();
    if (displayText.isEmpty()) displayText = m_path;

    QPushButton* pathButton = new QPushButton(displayText, this);
    pathButton->setStyleSheet(m_buttonStyle);
    pathButton->setProperty("fullPath", m_path);
    connect(pathButton, &QPushButton::clicked,this, [this, pathButton]() {
        emit pathClicked(pathButton->property("fullPath").toString());
    });

    m_layout->addWidget(pathButton);
}

void HistoryDisplay::createBreadcrumbView()
{
    if (m_path.isEmpty()) return;

    QStringList parts = m_path.split("/", Qt::SkipEmptyParts);
    if (parts.size() <= 1) {
        createMinimalView();
        return;
    }

    QString displayText = "..." + parts.mid(parts.size() - 2).join("/");
    QPushButton* pathButton = new QPushButton(displayText, this);
    pathButton->setStyleSheet(m_buttonStyle);
    pathButton->setProperty("fullPath", m_path);
    connect(pathButton, &QPushButton::clicked,this, [this, pathButton]() {
        emit pathClicked(pathButton->property("fullPath").toString());
    });

    m_layout->addWidget(pathButton);
}*/
void HistoryDisplay::createBreadcrumbView()
{
    if (m_path.isEmpty()) return;

    QStringList parts = m_path.split("/", Qt::SkipEmptyParts);
    if (parts.isEmpty()) return;

    if (m_path.startsWith(":")) {
        m_layout->addWidget(m_homeButton);
        m_homeButton->setVisible(true);
        m_homeButton->setIcon(m_homeIcon);
    }

    const int maxVisibleParts = 4; // Show last N parts with separators
    int startIdx = parts.size() > maxVisibleParts ? parts.size() - maxVisibleParts : 1;

    if (startIdx > 1) {
        QPushButton* ellipsis = new QPushButton("...", this);
        ellipsis->setStyleSheet(m_buttonStyle);
        ellipsis->setEnabled(false);
        m_layout->addWidget(ellipsis);

        QPushButton* separator = createSeparator();
        m_layout->addWidget(separator);
    }

    for (int i = startIdx; i < parts.size(); ++i) {
        QPushButton* pathButton = new QPushButton(parts[i], this);
        pathButton->setObjectName("breadcrumbButton");
        pathButton->setStyleSheet(m_buttonStyle);
        pathButton->setProperty("fullPath", buildPartialPath(i));

        connect(pathButton, &QPushButton::clicked, this, [this, pathButton]() {
            emit pathClicked(pathButton->property("fullPath").toString());
        });

        m_layout->addWidget(pathButton);

        if (i < parts.size() - 1) {
            m_layout->addWidget(createSeparator());
        }
    }
}

void HistoryDisplay::createMinimalView()
{
    if (m_path.isEmpty()) return;

    QStringList parts = m_path.split("/", Qt::SkipEmptyParts);
    if (parts.isEmpty()) return;

    if (m_path.startsWith(":")) {
        m_layout->addWidget(m_homeButton);
        m_homeButton->setVisible(true);
        m_homeButton->setIcon(m_homeIcon);

        QPushButton* separator = createSeparator();
        m_layout->addWidget(separator);
    }

    QString displayText = parts.last();
    if (displayText.isEmpty()) displayText = m_path;

    QPushButton* pathButton = new QPushButton(displayText, this);
    pathButton->setObjectName("minimalButton");
    pathButton->setStyleSheet(m_buttonStyle);
    pathButton->setProperty("fullPath", m_path);
    pathButton->setToolTip(m_path);

    connect(pathButton, &QPushButton::clicked, this, [this]() {
        emit pathClicked(m_path);
    });

    m_layout->addWidget(pathButton);
}

QPushButton* HistoryDisplay::createSeparator()
{
    QPushButton* separator = new QPushButton(this);
    separator->setObjectName("separator");
    separator->setIcon(m_separatorIcon);
    separator->setEnabled(false);
    return separator;
}


void HistoryDisplay::clearLayout()
{
    QLayoutItem* item;
    while ((item = m_layout->takeAt(0)) != nullptr) {
        if (QWidget* widget = item->widget()) {
            if (widget != m_homeButton) {
                widget->hide();
                widget->deleteLater();
            }
        }
        delete item;
    }
    m_homeButton->setVisible(false);
}

QString HistoryDisplay::buildPartialPath(int index) const
{
    QStringList parts = m_path.split("/", Qt::SkipEmptyParts);
    if (index >= parts.size()) return m_path;

    QString result;
    for (int i = 0; i <= index; ++i) {
        if (!result.isEmpty()) result += "/";
        result += parts[i];
    }
    return result;
}



// TDynamicFrame implementation
TDynamicFrame::TDynamicFrame(QWidget* parent) : PopoutFrame(parent)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(1, 1, 1, 2);

    m_stack = new TStackedWidget(this);
    //m_stack->setAnimationDuration(500);
    m_stack->setAnimationType(TStackedWidget::VerticalSlide);

    m_hDisplay=new HistoryDisplay(this);
    m_stack->addWidget(m_hDisplay);

    m_mWidget=new MessageWidget(this);
    m_stack->addWidget(m_mWidget);


    connect(m_hDisplay,&HistoryDisplay::pathClicked,this,&TDynamicFrame::pathClicked);

    m_layout->addWidget(m_stack);
    setLayout(m_layout);

    setPopoutMaxWidth(600);
    setPopoutMaxHeight(50);

    setBackgroundImage(":/pictures/image.png");
    //setBorder(false);

    connect(m_stack,&TStackedWidget::animationFinished,this,[this]{
        popout(400,1.2);
    });


}

TDynamicFrame::~TDynamicFrame()
{

}

TStackedWidget* TDynamicFrame::stackedWidget() const
{
    return m_stack;
}

void TDynamicFrame::setAnimationDuration(int duration)
{
    m_stack->setAnimationDuration(duration);
}

void TDynamicFrame::setPath(const QString &path)
{
    m_hDisplay->setPath(path);
    m_stack->setCurrentWidget(m_hDisplay);
}

void TDynamicFrame::setMaxwidth(int width)
{
    setPopoutMaxWidth(width);
}

void TDynamicFrame::setMessage(const QString &text)
{
    if(m_mWidget){
        m_mWidget->setMessage(text);
        m_stack->setCurrentWidget(m_mWidget);
    }

}

QColor oppositeColor(const QColor &color) {
    int r = 255 - color.red();
    int g = 255 - color.green();
    int b = 255 - color.blue();
    int a = color.alpha();
    return QColor(r, g, b, a);
}

void TDynamicFrame::setBackgroundImage(const QString &imgPath)
{
    QList<QColor> topColors = getDominantColors(imgPath, 3);
    if (topColors.isEmpty()) {
        qWarning() << "TDynamicFrame::setBackgroundImage: No dominant colors found for image" << imgPath;
        return;
    }
    //QColor dominant=topColors.first();
    //setBorderColor(oppositeColor(dominant));

    QSize currentWidgetSize = size();
    if (!currentWidgetSize.isValid() || currentWidgetSize.isEmpty()) {
        qWarning() << "TDynamicFrame::setBackgroundImage: Invalid widget size ("
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
}

MessageWidget::MessageWidget(QWidget *parent)
    : QWidget{parent}
{
    m_layout= new QHBoxLayout(this);
    m_label=new QLabel(this);

    m_label->setWordWrap(true);
    //m_label->setSizePolicy();
    m_label->setAlignment(Qt::AlignHCenter);

    m_label->setText("Welcome to simpleCloud");

    m_layout->addWidget(m_label);
    setLayout(m_layout);

    setStyleSheet("background:transparent;");
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
