#include "tcloudelt.h"

TCloudElt::TCloudElt(QWidget *parent)
    : TFrame{parent},m_selected(false),m_view(View::Grid),m_multiSelection(false)
{
    m_name="d";
    m_icon=QIcon();
    m_size=0;
    m_type=None;

    //setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);
}

QString TCloudElt::name() const{
    return m_name;
}

qint64 TCloudElt::size() const
{
    return m_size;
}

TFileInfo TCloudElt::info() const
{
    return m_info;
}

TCloudElt::Type TCloudElt::type() const
{
    return m_type;
}

void TCloudElt::setSelected(bool s)
{
    m_selected=s;
    emit selectedChanged(m_selected);
}

bool TCloudElt::isSelected() const
{
    return m_selected;
}

void TCloudElt::setView(View v)
{
    m_view = v;
    emit viewChanged(m_view);
}

QWidget *TCloudElt::originParent()
{
    return nullptr;
}

TMenu *TCloudElt::fMenu() const
{
    return m_fMenu;
}

void TCloudElt::setFMenu(TMenu *newFMenu)
{
    m_fMenu = newFMenu;
}

bool TCloudElt::multiSelection() const
{
    return m_multiSelection;
}

void TCloudElt::setMultiSelection(bool newMultiSelection)
{
    m_multiSelection = newMultiSelection;
}

eltCore TCloudElt::core()
{
    return eltCore(m_name,m_name,m_info);
}

void TCloudElt::copy()
{
    return;
}

void TCloudElt::cut()
{
    return;
}


