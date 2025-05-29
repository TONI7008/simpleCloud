#include "tclipboard.h"
#include "tcloudelt.h"

TClipBoard* TClipBoard::clipBoard=nullptr;

TClipBoard *TClipBoard::instance()
{
    return clipBoard;
}

void TClipBoard::Init()
{
    if(!clipBoard){
        clipBoard=new TClipBoard;
    }
    return;
}

void TClipBoard::cleanUp()
{
    if(clipBoard){
        delete clipBoard;
        clipBoard=nullptr;
    }
}


void TClipBoard::append(eltCore item) {

    m_list.append(item);
}

void TClipBoard::clear() {

    m_list.clear();
}

TClipBoard::TClipBoard(QObject *parent)
    : QObject{parent},m_type(Copy),m_isUsed(false)
{
}


TClipBoard::transactionType TClipBoard::type() const
{
    return m_type;
}

void TClipBoard::setType(transactionType newType)
{
    m_type = newType;

}

