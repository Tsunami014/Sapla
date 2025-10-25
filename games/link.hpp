#pragma once
#include "../main.hpp"
#include "../menu.hpp"

struct Link {
    MainGame* MG;
    const QString** help;
    _MenuesTyp* Menues;
    QGraphicsScene* MScene;
};
