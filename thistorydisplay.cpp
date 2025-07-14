#include "thistorydisplay.h"
#include <QApplication>

const QString THistoryDisplay::DEFAULT_STYLE = R"(
QPushButton {
    border-radius: 10px;
    background: rgba(120,120,120,0.5);
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


THistoryDisplay::THistoryDisplay(QWidget* parent) : QWidget(parent)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(3, 3,3,3);
    //m_layout->setAlignment();
    //m_layout->setStretchFactor(2);
    m_layout->setSpacing(2);

    m_homeButton = new QPushButton(this);
    m_homeButton->setVisible(false);
    m_homeButton->setIcon(QIcon(":/icons/folder.svg"));
    m_homeButton->setStyleSheet("background: transparent; border: none;");
    m_homeButton->setIconSize(QSize(28, 28));
    m_homeButton->setFixedSize(QSize(28, 28));
    connect(m_homeButton, &QPushButton::clicked, this, &THistoryDisplay::homeClicked);

    m_homeIcon = QIcon(":/icons/folder.svg");
    m_separatorIcon = QIcon(":/icons/rightwhite.svg");
    m_buttonStyle = DEFAULT_STYLE;
}

THistoryDisplay::~THistoryDisplay()
{
    clearLayout();
}

void THistoryDisplay::setPath(const QString& newPath)
{
    if (m_path != newPath) {
        m_path = newPath;
        updateView();
    }
}

QString THistoryDisplay::path() const
{
    return m_path;
}

THistoryDisplay::View THistoryDisplay::view() const
{
    return m_view;
}

void THistoryDisplay::setView(View newView)
{
    if (m_view != newView) {
        m_view = newView;
        updateView();
    }
}

void THistoryDisplay::setHomeIcon(const QIcon& icon)
{
    m_homeIcon = icon;
    if (m_homeButton) {
        m_homeButton->setIcon(icon);
    }
}

void THistoryDisplay::setSeparatorIcon(const QIcon& icon)
{
    m_separatorIcon = icon;
    updateView();
}

void THistoryDisplay::setButtonStyle(const QString& style)
{
    m_buttonStyle = style;
    updateView();
}

void THistoryDisplay::updateView()
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

/*void THistoryDisplay::createModernView()
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

        pathButton->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
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
}*/

void THistoryDisplay::createNormalView()
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

/*void THistoryDisplay::createBreadcrumbView()
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
}*/

void THistoryDisplay::createMinimalView()
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

QPushButton* THistoryDisplay::createSeparator()
{
    QPushButton* separator = new QPushButton(this);
    separator->setObjectName("separator");
    separator->setStyleSheet("background: transparent; border: none;");
    separator->setIcon(m_separatorIcon);
    separator->setFixedSize(24, 24);
    separator->setEnabled(false);
    return separator;
}


void THistoryDisplay::clearLayout()
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

QString THistoryDisplay::buildPartialPath(int index) const
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

bool THistoryDisplay::needsEllipsis(const QStringList& parts)
{
    // Calculate total width needed
    int totalWidth = 0;
    QFontMetrics fm(QApplication::font());

    // Account for home button if present
    if (m_path.startsWith(":")) {
        totalWidth += m_homeButton->sizeHint().width() + 5; // +5 for spacing
    }

    // Account for all buttons and separators
    for (int i = 0; i < parts.size(); ++i) {
        totalWidth += fm.horizontalAdvance(parts[i]) + 40; // Button padding
        if (i < parts.size() - 1) {
            totalWidth += 24; // Separator width
        }
    }

    return totalWidth > this->width();
}

void THistoryDisplay::createModernView()
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
    bool useEllipsis = needsEllipsis(parts);

    if (useEllipsis) {
        // Show first 2 parts
        for (int i = 1; i < 3 && i < parts.size(); ++i) {
            addPathButton(parts[i], buildPartialPath(i));
            if (i < parts.size() - 1) {
                m_layout->addWidget(createSeparator());
            }
        }

        // Add middle ellipsis button
        QPushButton* ellipsis = new QPushButton("...", this);
        ellipsis->setStyleSheet(m_buttonStyle);
        ellipsis->setMinimumHeight(32);
        QString middlePath = buildPartialPath(parts.size() / 2);
        ellipsis->setProperty("fullPath", middlePath);
        ellipsis->setToolTip(middlePath);

        connect(ellipsis, &QPushButton::clicked, this, [this, middlePath]() {
            emit middleClicked(middlePath);
        });

        m_layout->addWidget(ellipsis);
        m_layout->addWidget(createSeparator());

        // Show last 2 parts
        for (int i = parts.size() - 3; i < parts.size(); ++i) {
            if (i >= 3) { // Avoid overlap with first parts
                addPathButton(parts[i], buildPartialPath(i));
                if (i < parts.size() - 1) {
                    m_layout->addWidget(createSeparator());
                }
            }
        }
    } else {
        // Original logic when no ellipsis needed
        int startIdx = parts.size() > maxButtons ? parts.size() - maxButtons : 1;

        if (startIdx > 1) {
            QPushButton* ellipsis = new QPushButton("...", this);
            ellipsis->setStyleSheet(m_buttonStyle);
            ellipsis->setEnabled(false);
            m_layout->addWidget(ellipsis);
        }

        for (int i = startIdx; i < parts.size(); ++i) {
            addPathButton(parts[i], buildPartialPath(i));
            if (i < parts.size() - 1) {
                m_layout->addWidget(createSeparator());
            }
        }
    }
}

// Helper function to add a path button
void THistoryDisplay::addPathButton(const QString& text, const QString& fullPath)
{
    QPushButton* pathButton = new QPushButton(text, this);
    pathButton->setStyleSheet(m_buttonStyle);
    pathButton->setMinimumHeight(32);
    pathButton->setProperty("fullPath", fullPath);

    connect(pathButton, &QPushButton::clicked, this, [this, fullPath]() {
        emit pathClicked(fullPath);
    });

    pathButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_layout->addWidget(pathButton);
}

// Update createBreadcrumbView similarly
void THistoryDisplay::createBreadcrumbView()
{
    if (m_path.isEmpty()) return;

    QStringList parts = m_path.split("/", Qt::SkipEmptyParts);
    if (parts.isEmpty()) return;

    if (m_path.startsWith(":")) {
        m_layout->addWidget(m_homeButton);
        m_homeButton->setVisible(true);
        m_homeButton->setIcon(m_homeIcon);
    }

    const int maxVisibleParts = 4;
    bool useEllipsis = needsEllipsis(parts);

    if (useEllipsis) {
        // Show first part
        addPathButton(parts[0], buildPartialPath(0));
        m_layout->addWidget(createSeparator());

        // Add middle ellipsis button
        QPushButton* ellipsis = new QPushButton("...", this);
        ellipsis->setObjectName("breadcrumbButton");
        ellipsis->setStyleSheet(m_buttonStyle);
        QString middlePath = buildPartialPath(parts.size() / 2);
        ellipsis->setProperty("fullPath", middlePath);
        ellipsis->setToolTip(middlePath);

        connect(ellipsis, &QPushButton::clicked, this, [this, middlePath]() {
            emit middleClicked(middlePath);
        });

        m_layout->addWidget(ellipsis);
        m_layout->addWidget(createSeparator());

        // Show last 2 parts
        for (int i = parts.size() - 2; i < parts.size(); ++i) {
            if (i > 0) { // Avoid overlap with first part
                addPathButton(parts[i], buildPartialPath(i));
                if (i < parts.size() - 1) {
                    m_layout->addWidget(createSeparator());
                }
            }
        }
    } else {
        // Original logic when no ellipsis needed
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
            addPathButton(parts[i], buildPartialPath(i));
            if (i < parts.size() - 1) {
                m_layout->addWidget(createSeparator());
            }
        }
    }
}
