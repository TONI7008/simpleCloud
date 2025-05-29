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
}

QPushButton:hover {
    background: #479EF5;
    color: black;
}

QPushButton:pressed {
    background-color: rgba(70, 158, 245, 0.6);
}
)";

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

/*void HistoryDisplay::createModernView()
{
    if (m_path.isEmpty()) return;

    QStringList parts = m_path.split("/", Qt::SkipEmptyParts);
    if (parts.isEmpty()) return;

    // Add home button if path starts with special marker
    if (m_path.startsWith(":")) {
        m_layout->addWidget(m_homeButton);
        m_homeButton->setVisible(true);
        m_homeButton->setIcon(m_homeIcon);
    }

    for (int i = 1; i < parts.size(); ++i) {
        QPushButton* pathButton = new QPushButton(parts[i], this);
        pathButton->setStyleSheet(m_buttonStyle);
        pathButton->setMinimumHeight(32);
        pathButton->setProperty("fullPath", buildPartialPath(i));
        connect(pathButton, &QPushButton::clicked,this, [this, pathButton]() {
            emit pathClicked(pathButton->property("fullPath").toString());
        });

        m_layout->addWidget(pathButton);

        // Add separator except after last item
        if (i < parts.size() - 1) {
            QPushButton* separator = new QPushButton(this);
            separator->setStyleSheet("background: transparent; border: none;");
            separator->setIcon(m_separatorIcon);
            separator->setFixedSize(24, 24);
            separator->setEnabled(false);
            m_layout->addWidget(separator);
        }
    }
}*/
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

void HistoryDisplay::createMinimalView()
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
    setBorder(false);

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


MessageWidget::MessageWidget(QWidget *parent)
    : QWidget{parent}
{
    m_layout= new QHBoxLayout(this);
    m_label=new QLabel(this);

    m_label->setWordWrap(true);
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
