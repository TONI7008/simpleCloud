#include "home.h"
#include "tsecuritymanager.h"

#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TSecurityManager sMan;
    sMan.loadCertificate(":/ressources/certificates/client/client.key",
                          ":/ressources/certificates/client/client.crt",
                          ":/ressources/certificates/client/ca.crt");


    int fontId = QFontDatabase::addApplicationFont(":/fonts/NotoColorEmoji.ttf");
    QStringList families = QFontDatabase::applicationFontFamilies(fontId);

    if (!families.isEmpty()) {
        //QFontDatabase::addApplicationEmojiFontFamily(families.at(0));
        qDebug() << "Done adding fonts";
    }

    Home w(&sMan);
    w.show();
    return a.exec();
}
