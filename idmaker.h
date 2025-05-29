#ifndef IDMAKER_H
#define IDMAKER_H

#include <QByteArray>
#include <QRandomGenerator>

class IdMaker {
public:
    // Constructeur par défaut (pas nécessaire pour une méthode statique)
    IdMaker() = default;

    // Méthode statique pour générer un QByteArray unique
    static QByteArray makeId(int size=16) {
        QByteArray byteArray;
        byteArray.resize(size); // Redimensionner le QByteArray à la taille souhaitée

        // Remplir le QByteArray avec des valeurs aléatoires
        for (int i = 0; i < size; ++i) {
            byteArray[i] = static_cast<char>(QRandomGenerator::global()->bounded(256)); // Valeurs entre 0 et 255
        }

        return byteArray;
    }

private:
         // Pas besoin de m_size pour une méthode statique
};

#endif // IDMAKER_H
