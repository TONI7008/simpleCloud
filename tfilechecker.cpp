#include "tfilechecker.h"
#include "tfolder.h"

TFileChecker* TFileChecker::m_checker=nullptr;

TFileChecker::TFileChecker(QObject *parent) : QObject(parent)
{

}

bool TFileChecker::isOk(QString filename, TFolder *folder)
{
    for(auto t_Elt : std::as_const(folder->m_WfileList)){
        if(t_Elt->name()==filename){
            return false;
        }
    }
    return true;
}

void TFileChecker::cleanUp()
{
    if(m_checker) delete m_checker;
}

void TFileChecker::Init()
{
    if(!m_checker){
        m_checker=new TFileChecker;
    }

    return;
}

TFileChecker *TFileChecker::instance()
{
    return m_checker;
}

