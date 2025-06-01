#include "backgroundframe.h"
#include <QGridLayout>

QList<BackgroundFrame*> BackgroundFrame::m_list;

BackgroundFrame::BackgroundFrame(QWidget *parent)
    : TFrame{parent}
{
    deleteButton = new QToolButton(this);
    setupDeleteButton();

    connect(this, &BackgroundFrame::Clicked, this, [this] {
        if (!m_simpleType) {
            manage();
        }
    });

    setBorderSize(4);
    setRoundness(15);

    QGridLayout* layout = new QGridLayout(this);
    layout->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    layout->addWidget(deleteButton);
    layout->setContentsMargins(1, 1, 1, 1);  // Add some margins
    setLayout(layout);

    connect(deleteButton, &QToolButton::clicked, this, [this] {
        m_list.removeOne(this);
        emit kill();
    });

    setClickInterval(60);
    updateAppearance();
}

BackgroundFrame::~BackgroundFrame()
{
    m_list.removeOne(this);
}

void BackgroundFrame::setupDeleteButton()
{
    deleteButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    deleteButton->setIcon(QIcon(":/icons/deletewhite.svg"));
    deleteButton->setIconSize(QSize(28, 28));
    deleteButton->setFixedSize(32, 32);
    deleteButton->setStyleSheet(defaultStyle);
}

void BackgroundFrame::updateAppearance()
{
    if (m_simpleType) {
        setEnableBackground(false);
        setEnableBackground(false);
        deleteButton->setStyleSheet("background: transparent; border: none;");
        deleteButton->setVisible(false);
    } else {
        setEnableBackground(true);
        deleteButton->setStyleSheet(defaultStyle);
        deleteButton->setVisible(true);
        if (!m_list.contains(this)) {
            m_list.append(this);
        }
    }
}

void BackgroundFrame::setSelected(bool s)
{
    selected = s;
    setEnabledBorder(selected);
}

void BackgroundFrame::manage()
{
    if (m_list.isEmpty()) return;

    setSelected(true);
    for (auto& frame : m_list) {
        if (frame != this) {
            frame->setSelected(false);
        }
    }
}

void BackgroundFrame::setText(const QString& text)
{
    m_text = text;
}

void BackgroundFrame::setSimpleType(bool simple)
{
    if (m_simpleType == simple) return;

    m_simpleType = simple;
    updateAppearance();
}

void BackgroundFrame::setPixmap(const QPixmap& pixmap)
{
    QPixmap scaledPixmap = pixmap.scaled(
        this->width(),
        this->height(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );
    m_backgroundImage = scaledPixmap;
    setBackgroundImage(pixmap);
}

QString BackgroundFrame::text() const
{
    return m_text;
}

QList<BackgroundFrame*> BackgroundFrame::instances()
{
    return m_list;
}
