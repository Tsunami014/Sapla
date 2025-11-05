#pragma once
#include "link.hpp"
#include "../scenes/baseScn.hpp"
#include <QLibrary>

const extern QString suffix;
QString getGamesPath();

class GamePlugin {
public:
    GamePlugin(QString name, Registry& reg, QLibrary* lib);
    ~GamePlugin();
    QString name;
    bool run();
    bool permanent;
private:
    Registry reg;
    QLibrary* lib;
};

extern std::vector<GamePlugin*> games;
extern std::vector<std::pair<QString, QString>> failedGames;
void loadGames();

