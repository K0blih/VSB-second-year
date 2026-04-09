#ifndef CARDRECORD_H
#define CARDRECORD_H

#include <QString>

struct CardRecord
{
    QString name;
    QString category;
    QString rarity;
    int attack = 0;
    int defense = 0;
    QString description;
    QString imagePath;
};

#endif // CARDRECORD_H
