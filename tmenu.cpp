#include "tmenu.h"
#include <QPainter>
#include <QPainterPath>

const QString TMenu::menuStyle = R"(
QMenu {
    background: rgb(36, 38, 39);
    border: none;
    border-radius: 10px;
    padding: 4px;  /* Global padding */
    color: white;

    icon-size:20px;
}

QMenu::item {
    background-color:transparent;
    min-height: 24px;          /* Minimum height */
    height: 24px;              /* Explicit height (optional) */
    padding: 2px 5px 2px 3px; /* Top, Right, Bottom, Left */
    margin: 1px 0;             /* Vertical spacing between items */
}

QMenu::item:hover {
    color: rgba(71, 158, 245, 1);
    background-color: rgba(71, 158, 245, 0.1); /* Optional hover background */
    padding: 2px 6px 2px 3px; /* Match default padding */
}

QMenu::item:selected {
    color: rgba(71, 158, 245, 1);
}

QMenu::indicator {
    width: 16px;
    height: 16px;
    border-radius: 4px;
    margin-right: 5px;
    border: 1px solid white;
}

QMenu::indicator:checked {
    background-color: rgb(71, 158, 245);
    border: 1px solid white;
    image: url(:/ressources/CheckBox.svg);
}

QMenu::indicator:checked:pressed {
    background-color: #53aad2;
    border: 1px solid #53aad2;
    image: url(:/ressources/CheckBoxPressed.svg);
}

QMenu::right-arrow {
    image: url(:/icons/rightwhite.svg);
    width: 16px;
    height: 16px;
}

QMenu::right-arrow:hover {
    image: url(:/icons/rightblue.svg);
    width: 16px;
    height: 16px;
}
)";

TMenu::TMenu(QWidget *parent) :
    QMenu{parent},isDoubleClick(false), enableBackground(false), enableBorder(false),
    borderSize(0), borderColorValue(QColor(71, 158, 245)), m_bRadius(0)
{
    setWindowFlags(windowFlags() | Qt::NoDropShadowWindowHint | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    //setAttribute(Qt::WA_NoSystemBackground);
    setStyleSheet(menuStyle);
}
TMenu::TMenu(QString title,QWidget *parent) :
    QMenu{title,parent},isDoubleClick(false), enableBackground(false), enableBorder(false),
    borderSize(0), borderColorValue(QColor(71, 158, 245)), m_bRadius(0)
{
    setWindowFlags(windowFlags() | Qt::NoDropShadowWindowHint | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    //setAttribute(Qt::WA_NoSystemBackground);
    setStyleSheet(menuStyle);
}

TMenu::TMenu(QIcon icon, QString title, QWidget *parent)  :  TMenu{title,parent}
{

    setIcon(icon);
}

TMenu::~TMenu()
{

}

void TMenu::setBorderRadius(short b)
{
    m_bRadius=b;
}

short TMenu::borderRadius() const
{
    return m_bRadius;
}

bool TMenu::borderEnabled() const
{
    return enableBackground;
}

QColor TMenu::borderColor() const
{
    return borderColorValue;
}


void TMenu::setBorder(bool enable) {
    if (enableBorder != enable) {
        enableBorder = enable;
        repaint();
    }
}

void TMenu::setBorderSize(short size) {
    if (borderSize != size) {
        borderSize = size;
        update();
    }
}

void TMenu::setBorderColor(const QColor &color)
{
    if (borderColorValue != color) {
        borderColorValue = color;
        setStyleSheet(QString(menuStyle).replace("rgba(71,158,245,1)",borderColorValue.name()));
        repaint();
    }
}

void TMenu::setCornerStyle(CornerStyle style)
{
    if (m_cornerStyle != style) {
        m_cornerStyle = style;
        repaint();
    }
}



void TMenu::paintEvent(QPaintEvent *event) {

    QMenu::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath path;
    QRect r = rect();
    int rSize = m_bRadius;

    switch (m_cornerStyle) {
    case CornerStyle::TopOnly:
        path.moveTo(r.bottomLeft());
        path.lineTo(r.topLeft() + QPoint(0, rSize));
        path.arcTo(QRect(r.topLeft(), QSize(2 * rSize, 2 * rSize)), 180, -90);
        path.lineTo(r.topRight() - QPoint(rSize, 0));
        path.arcTo(QRect(r.topRight() - QPoint(2 * rSize, 0), QSize(2 * rSize, 2 * rSize)), 90, -90);
        path.lineTo(r.bottomRight());
        path.lineTo(r.bottomLeft());
        break;

    case CornerStyle::BottomOnly:
        path.moveTo(r.topLeft());
        path.lineTo(r.bottomLeft() - QPoint(0, rSize));
        path.arcTo(QRect(r.bottomLeft() - QPoint(0, 2 * rSize), QSize(2 * rSize, 2 * rSize)), 180, 90);
        path.lineTo(r.bottomRight() - QPoint(rSize, 0));
        path.arcTo(QRect(r.bottomRight() - QPoint(2 * rSize, 2 * rSize), QSize(2 * rSize, 2 * rSize)), 270, 90);
        path.lineTo(r.topRight());
        path.lineTo(r.topLeft());
        break;

    case CornerStyle::LeftOnly:
        path.moveTo(r.topRight());
        path.lineTo(r.topLeft() + QPoint(rSize, 0));
        path.arcTo(QRect(r.topLeft(), QSize(2 * rSize, 2 * rSize)), 90, 90);
        path.lineTo(r.bottomLeft() + QPoint(0, -rSize));
        path.arcTo(QRect(r.bottomLeft() - QPoint(0, 2 * rSize), QSize(2 * rSize, 2 * rSize)), 180, 90);
        path.lineTo(r.bottomRight());
        path.lineTo(r.topRight());
        break;

    case CornerStyle::RightOnly:
        path.moveTo(r.topLeft());
        path.lineTo(r.topRight() - QPoint(rSize, 0));
        path.arcTo(QRect(r.topRight() - QPoint(2 * rSize, 0), QSize(2 * rSize, 2 * rSize)), 90, -90);
        path.lineTo(r.bottomRight() - QPoint(0, rSize));
        path.arcTo(QRect(r.bottomRight() - QPoint(2 * rSize, 2 * rSize), QSize(2 * rSize, 2 * rSize)), 0, -90);
        path.lineTo(r.bottomLeft());
        path.lineTo(r.topLeft());
        break;

    case CornerStyle::BottomLeft:
        // Rounded corner on the bottom-left only
        path.moveTo(r.topRight());
        path.lineTo(r.topLeft());
        path.lineTo(r.bottomLeft() - QPoint(0, rSize));
        path.arcTo(QRect(r.bottomLeft() - QPoint(0, 2 * rSize), QSize(2 * rSize, 2 * rSize)), 180, 90);
        path.lineTo(r.bottomRight());
        path.lineTo(r.topRight());
        break;

    case CornerStyle::BottomRight:
        // Rounded corner on the bottom-right only
        path.moveTo(r.topLeft());
        path.lineTo(r.topRight());
        path.lineTo(r.bottomRight() - QPoint(0, rSize));
        path.arcTo(QRect(path.currentPosition().toPoint() - QPoint(2 * rSize, rSize), QSize(2 * rSize, 2 * rSize)), 0, -90);
        path.lineTo(r.bottomLeft());
        path.lineTo(r.topLeft());
        break;

    case CornerStyle::None:
        path.addRect(r);
        break;

    case CornerStyle::Default:
    default:
        path.addRoundedRect(r, rSize, rSize);
        break;
    }

    painter.setClipPath(path);

    // Draw border if enabled
    if (enableBorder && borderSize > 0) {
        QPen pen(borderColorValue, borderSize);
        painter.setPen(pen);
        pen.setJoinStyle(Qt::MiterJoin);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }
}

TMenu* TMenu::createMenu(const QString &title, const QIcon &icon)
{
    TMenu* menu = new TMenu(icon,title, this);

    // Copy all relevant properties to the new menu
    menu->setStyleSheet(menuStyle);
    menu->setBorderRadius(m_bRadius);
    menu->setBorder(enableBorder);
    menu->setBorderSize(borderSize);
    menu->setBorderColor(borderColorValue);
    menu->setCornerStyle(m_cornerStyle);
    return menu;
}

TMenu* TMenu::addMenu(const QString &title)
{
    TMenu* menu = createMenu(title,QIcon());
    QMenu::addMenu(menu);
    return menu;
}

TMenu* TMenu::addMenu(const QIcon &icon, const QString &title)
{
    TMenu* menu = createMenu(title, icon);
    menu->setIcon(icon);
    QMenu::addMenu(menu);
    return menu;
}
TMenu* TMenu::addMenu(TMenu* menu)
{
    menu->setBorderRadius(m_bRadius);
    menu->setBorder(enableBorder);
    menu->setBorderSize(borderSize);
    menu->setBorderColor(borderColorValue);
    menu->setCornerStyle(m_cornerStyle);
    QMenu::addMenu(menu);
    return menu;
}
